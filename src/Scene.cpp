#include "Scene.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <chrono>

#include <stb_image.h>

#include "Utils.h"

#ifdef _WIN32

    #include <windows.h>

#endif


Scene::Scene()
{    
    _camera.Init(glm::vec3(0,0,4.0f),glm::vec3(0,0,0),glm::vec3(0,1,0),_viewportWidth,_viewportHeight);

    _objectTransforms.reserve(_maximumObjectCount);
    _objectDatas.reserve(_maximumObjectCount);
}

void Scene::MaximizeView()
{
    if(_isViewEnlarged == true) return; //false state

    _isViewEnlarged = true;

    

    //disable any currently ongoing gizmo interactions and object picking stored result.
    ChooseObject(-1); // we disable gizmo here and any chosen object also
}

void Scene::MinimizeView()
{
    if(_isViewEnlarged == false) return; // wrong state

    _isViewEnlarged = false;
}

void Scene::Reset()
{
    cl_int clError;
    if(_vertexPositionData.size() > 0)
    {
        _vertexPositionData.clear();
        _vertexAttributeData.clear();

        clError = clReleaseMemObject(_vertexPositionDataBuffer);CHECK_ERROR(clError);
        clError = clReleaseMemObject(_vertexAttributeDataBuffer);CHECK_ERROR(clError);

        _vertexAttributeDataBuffer = nullptr;
        _vertexPositionDataBuffer= nullptr;
    }
    if(_triangleIndicesData.size() > 0)
    {
        _triangleIndicesData.clear();

        clError = clReleaseMemObject(_triangleIndicesDataBuffer);CHECK_ERROR(clError);
        _triangleIndicesDataBuffer = nullptr;
    }
    if(_bottomLevelBvhNodeDatas.size()>0)
    {
        _bottomLevelBvhNodeDatas.clear();
        clError = clReleaseMemObject(_bottomLevelBvhNodeDatasBuffer);CHECK_ERROR(clError);
        _bottomLevelBvhNodeDatasBuffer = nullptr;

        _debugBlasBvhBoxes.clear();
        _debugBlasBvhMeshRanges.clear();
        glDeleteBuffers(1,&this->_debugBlasBvhBoxesSsboId);
    }
    if(_meshBvhRootIndexData.size()>0)
    {
        _meshBvhRootIndexData.clear();

        for(int i=0;i<_rasterizedMeshes.size();++i)
        {
            _rasterizedMeshes[i].Delete();
        }
        _rasterizedMeshes.clear();
    }
    if(_textureDatas.size()>0)
    {
        _textureDatas.clear();
        _rgbaDatas.clear();

        clError = clReleaseMemObject(_rgbaDatasBuffer);CHECK_ERROR(clError);
        clError = clReleaseMemObject(_textureDatasBuffer);CHECK_ERROR(clError);
        _rgbaDatasBuffer = nullptr;
        _textureDatasBuffer = nullptr;

        for(int i=0;i<_rasterizedTextures.size();++i)
        {
            _rasterizedTextures[i].Delete();
        }
        _rasterizedTextures.clear();
    }
    if(_materialDatas.size()>0)
    {
        _materialDatas.clear();

        clError = clReleaseMemObject(_materialDataBuffer);CHECK_ERROR(clError);
        _materialDataBuffer = nullptr;
    }
    if(_objectDatas.size()>0)
    {
        _objectDatas.clear();
        _objectTransforms.clear();
        _debugBlasBvhIndirectCommandDatas.clear();
        _temporaryObjectBlasInstances.clear();
        _tlasBvhNodes.clear();
    }
    if(_modelDatas.size()>0)
    {
        _modelDatas.clear();

        clError = clReleaseMemObject(_modelDataBuffer);CHECK_ERROR(clError);
        _modelDataBuffer = nullptr;
    }

    ResetPathTracedFrameIndex();
}

static bool FindCLDeviceAndPlatformForGLContext(cl_platform_id& platform, cl_device_id& device)
{
    //Initialization of OpenCL
    cl_int clError;

    //Querry how many platforms (Runtimes) are currently present on the computer 
    cl_uint clPlatformCount = 0;
    clError = clGetPlatformIDs(0,nullptr,&clPlatformCount);

    //Querry each of these runtime IDs
    std::vector<cl_platform_id> clPlatformIDs(clPlatformCount);
    clError = clGetPlatformIDs(clPlatformIDs.size(), clPlatformIDs.data(), nullptr);
    CHECK_ERROR(clError);

    //Print these platforms on the console
    for(int i=0;i<clPlatformIDs.size();++i)
    {
        char buffer[256];

        cl_context_properties properties[] =
        {
            CL_GL_CONTEXT_KHR,reinterpret_cast<cl_context_properties>(wglGetCurrentContext()),
            CL_WGL_HDC_KHR,reinterpret_cast<cl_context_properties>(wglGetCurrentDC()),
            CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(clPlatformIDs[i]),
            0
        };

        auto clGetGLContextInfoKHRPtr =
            reinterpret_cast<clGetGLContextInfoKHR_fn>(
                clGetExtensionFunctionAddressForPlatform(
                    clPlatformIDs[i],
                    "clGetGLContextInfoKHR"
                )
            );

        if (clGetGLContextInfoKHRPtr == nullptr)
        {
            // Ez a platform nem szolgáltatja a függvényt.
            continue;
        }

        cl_device_id currentDevice = nullptr;
        size_t returnedSize = 0;

        clError = clGetGLContextInfoKHRPtr(properties,CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR,sizeof(currentDevice),&currentDevice,&returnedSize);

        clGetPlatformInfo(clPlatformIDs[i],CL_PLATFORM_NAME,sizeof(buffer),buffer,nullptr);

        std::cout<< "Platform name: "<<buffer<<"\n";

        if (clError == CL_SUCCESS && returnedSize == sizeof(cl_device_id) && currentDevice != nullptr)
        {
            platform = clPlatformIDs[i];
            device = currentDevice;
            return true;
        }
    }

    return false;
}

void Scene::InitCL()
{
    cl_int clError;

    bool didFindDeviceWithOpenglContext = FindCLDeviceAndPlatformForGLContext(this->clPlatform,this->clDevice);

    cl_context_properties clContextProperties[] = 
    {
        CL_GL_CONTEXT_KHR,reinterpret_cast<cl_context_properties>(wglGetCurrentContext()),
        CL_WGL_HDC_KHR,reinterpret_cast<cl_context_properties>(wglGetCurrentDC()),
        CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(this->clPlatform),
        0
    };
    
    clContext = clCreateContext(clContextProperties,1,&this->clDevice,nullptr,nullptr,&clError);
    CHECK_ERROR(clError);

    clCommandQueue = clCreateCommandQueueWithProperties(clContext,clDevice,nullptr,&clError);
    CHECK_ERROR(clError);

    clPathTracerKernel = InitKernel("assets/kernels/pathtracer_kernel.cl");


    clOpenglInteropTex = clCreateFromGLTexture(clContext,CL_MEM_WRITE_ONLY,GL_TEXTURE_2D,0,_renderTexture.GetId(),&clError);
    CHECK_ERROR(clError);

    clHelperBuffer = clCreateBuffer(clContext,CL_MEM_READ_WRITE,sizeof(RgbData)*_viewportWidth*_viewportHeight,nullptr,&clError);
    CHECK_ERROR(clError);

    clCameraDataBuffer = clCreateBuffer(clContext,CL_MEM_READ_ONLY,sizeof(CameraData),nullptr,&clError);
    CHECK_ERROR(clError);

    _objectDataBuffer = clCreateBuffer(clContext,CL_MEM_READ_ONLY,sizeof(ObjectData) * _maximumObjectCount,nullptr,&clError);
    CHECK_ERROR(clError);

    _tlasBvhNodesBuffer = clCreateBuffer(clContext,CL_MEM_READ_ONLY,sizeof(TlasBvhNode)* _maximumObjectCount * 2, nullptr,&clError);
    CHECK_ERROR(clError);

}

cl_kernel Scene::InitKernel(const std::string &filePath)
{
    cl_int error;
    std::string kernelSource = "";

    std::ifstream file(filePath);

    std::string linebuffer;
    while (std::getline(file, linebuffer))
    {
        kernelSource.append(linebuffer + '\n');
    }

    file.close();

    const char* kernelSourceC = kernelSource.c_str();

    cl_program program = clCreateProgramWithSource(clContext, 1,&kernelSourceC, nullptr, &error);
    CHECK_ERROR(error);

    error = clBuildProgram(program, 1, &clDevice, nullptr, nullptr, nullptr);
    if (error != CL_SUCCESS) {
        size_t log_size;
        clGetProgramBuildInfo(program, clDevice, CL_PROGRAM_BUILD_LOG, 0, nullptr, &log_size);
        std::vector<char> log(log_size);
        clGetProgramBuildInfo(program, clDevice, CL_PROGRAM_BUILD_LOG, log_size, log.data(), nullptr);
        std::cerr << "Build log:\n" << log.data() << std::endl;
    }

    cl_kernel kernel = clCreateKernel(program, "renderimage", &error);
    CHECK_ERROR(error);

    return kernel;
}



void Scene::Init()
{
    _renderTexture.Init(_viewportWidth,_viewportHeight);
    _renderBuffer.Init(_viewportWidth,_viewportHeight);

    _renderFrameBuffer.Init();

    _renderFrameBuffer.AttachTexture(_renderTexture);
    _renderFrameBuffer.AttachRenderBuffer(_renderBuffer);


    _gizmoShader.Init("assets/shaders/gizmo_pos_norm_tex.vert","assets/shaders/gizmo_pos_norm_tex.frag");

    _arrowGizmoMesh.Load("assets/models/arrowhead.obj");
    _arrowGizmoModelTransforms[0] = glm::mat4(1.0f);
    _arrowGizmoModelTransforms[1] = glm::rotate(glm::mat4(1.0f),glm::radians(90.0f),glm::vec3(0,0,1)) * 
        glm::rotate(glm::mat4(1.0f),glm::radians(90.0f),glm::vec3(1,0,0));
    _arrowGizmoModelTransforms[2] = glm::rotate(glm::mat4(1.0f),glm::radians(270.0f),glm::vec3(0,1,0)) * 
        glm::rotate(glm::mat4(1.0f),glm::radians(270.0f),glm::vec3(1,0,0));

    _cubeGizmoMesh.Load("assets/models/cubehead.obj");
    _cubeGizmoModelTransforms[0] = glm::mat4(1.0f);
    _cubeGizmoModelTransforms[1] = glm::rotate(glm::mat4(1.0f),glm::radians(90.0f),glm::vec3(0,0,1)) * 
        glm::rotate(glm::mat4(1.0f),glm::radians(90.0f),glm::vec3(1,0,0));
    _cubeGizmoModelTransforms[2] = glm::rotate(glm::mat4(1.0f),glm::radians(270.0f),glm::vec3(0,1,0)) * 
        glm::rotate(glm::mat4(1.0f),glm::radians(270.0f),glm::vec3(1,0,0));

    _ringGizmoMesh.Load("assets/models/ring.obj");
    _ringGizmoModelTransforms[0] = glm::mat4(1.0f);
    _ringGizmoModelTransforms[1] = glm::scale(glm::mat4(1.0f),glm::vec3(1,1,0.997)) *
        glm::rotate(glm::mat4(1.0f),glm::radians(90.0f),glm::vec3(0,0,1));
    _ringGizmoModelTransforms[2] = glm::scale(glm::mat4(1.0f),glm::vec3(1,0.997,0.997)) * 
        glm::rotate(glm::mat4(1.0f),glm::radians(90.0f),glm::vec3(0,1,0));

    std::vector<VertexAttributeData> dummyAttrVec;
    std::vector<VertexPositionData> tempPositionalData;
    std::vector<TriangleIndicesData> tempTriangleIndices;
    std::vector<BvhNodeData> tempBvhNodeData;

    
    TryLoadPathTracedMesh("assets/models/arrowhead.obj",tempPositionalData,tempTriangleIndices,dummyAttrVec,tempBvhNodeData,nullptr);

    _gizmoVertexPositionDatas.insert(_gizmoVertexPositionDatas.end(),tempPositionalData.begin(),tempPositionalData.end());
    _gizmoTriangleVertexIndices.insert(_gizmoTriangleVertexIndices.end(),tempTriangleIndices.begin(),tempTriangleIndices.end());
    _gizmoMeshBvhRoots.push_back(_gizmoBvhNodeDatas.size());
    _gizmoBvhNodeDatas.insert(_gizmoBvhNodeDatas.end(),tempBvhNodeData.begin(),tempBvhNodeData.end());

    
    
    TryLoadPathTracedMesh("assets/models/cubehead.obj",tempPositionalData,tempTriangleIndices,dummyAttrVec,tempBvhNodeData,nullptr);
    for(int i=0;i<tempTriangleIndices.size();++i)
    {
        tempTriangleIndices[i].x += _gizmoVertexPositionDatas.size();
        tempTriangleIndices[i].y += _gizmoVertexPositionDatas.size();
        tempTriangleIndices[i].z += _gizmoVertexPositionDatas.size();
    }
    for(int i=0;i<tempBvhNodeData.size();++i)
    {
        tempBvhNodeData[i].startIndex += _gizmoTriangleVertexIndices.size();
        tempBvhNodeData[i].endIndex += _gizmoTriangleVertexIndices.size();
        if(tempBvhNodeData[i].minChild >= 0)
        {
            tempBvhNodeData[i].minChild += _gizmoBvhNodeDatas.size();
            tempBvhNodeData[i].maxChild += _gizmoBvhNodeDatas.size();
        }
        
    }
    _gizmoVertexPositionDatas.insert(_gizmoVertexPositionDatas.end(),tempPositionalData.begin(),tempPositionalData.end());
    _gizmoTriangleVertexIndices.insert(_gizmoTriangleVertexIndices.end(),tempTriangleIndices.begin(),tempTriangleIndices.end());
    _gizmoMeshBvhRoots.push_back(_gizmoBvhNodeDatas.size());
    _gizmoBvhNodeDatas.insert(_gizmoBvhNodeDatas.end(),tempBvhNodeData.begin(),tempBvhNodeData.end());

    TryLoadPathTracedMesh("assets/models/ring.obj",tempPositionalData,tempTriangleIndices,dummyAttrVec,tempBvhNodeData,nullptr);
    for(int i=0;i<tempTriangleIndices.size();++i)
    {
        tempTriangleIndices[i].x += _gizmoVertexPositionDatas.size();
        tempTriangleIndices[i].y += _gizmoVertexPositionDatas.size();
        tempTriangleIndices[i].z += _gizmoVertexPositionDatas.size();
    }
    for(int i=0;i<tempBvhNodeData.size();++i)
    {
        tempBvhNodeData[i].startIndex += _gizmoTriangleVertexIndices.size();
        tempBvhNodeData[i].endIndex += _gizmoTriangleVertexIndices.size();
        if(tempBvhNodeData[i].minChild >= 0)
        {
            tempBvhNodeData[i].minChild += _gizmoBvhNodeDatas.size();
            tempBvhNodeData[i].maxChild += _gizmoBvhNodeDatas.size();
        }
    }
    _gizmoVertexPositionDatas.insert(_gizmoVertexPositionDatas.end(),tempPositionalData.begin(),tempPositionalData.end());
    _gizmoTriangleVertexIndices.insert(_gizmoTriangleVertexIndices.end(),tempTriangleIndices.begin(),tempTriangleIndices.end());
    _gizmoMeshBvhRoots.push_back(_gizmoBvhNodeDatas.size());
    _gizmoBvhNodeDatas.insert(_gizmoBvhNodeDatas.end(),tempBvhNodeData.begin(),tempBvhNodeData.end());
    

    //Lets setup some variables for the bvh accelerational structure's debug rendering view.
    glGenVertexArrays(1,&this->_debugBvhVaoId);
    glBindVertexArray(this->_debugBvhVaoId);

    glGenBuffers(1,&this->_debugBvhVboId);
    glBindBuffer(GL_ARRAY_BUFFER,this->_debugBvhVboId);
    glm::vec3 cubeVertices[8] =
    {
        {-0.5f, -0.5f, -0.5f},
        { 0.5f, -0.5f, -0.5f},
        { 0.5f,  0.5f, -0.5f},
        {-0.5f,  0.5f, -0.5f},

        {-0.5f, -0.5f,  0.5f},
        { 0.5f, -0.5f,  0.5f},
        { 0.5f,  0.5f,  0.5f},
        {-0.5f,  0.5f,  0.5f}
    };
    glBufferData(GL_ARRAY_BUFFER,sizeof(glm::vec3)*8,cubeVertices,GL_STATIC_DRAW);

    glGenBuffers(1,&this->_debugBvhIboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,this->_debugBvhIboId);
    unsigned int cubeIndices[24] =
    {
        // bottom
        0, 1,
        1, 2,
        2, 3,
        3, 0,

        // top
        4, 5,
        5, 6,
        6, 7,
        7, 4,

        // vertical
        0, 4,
        1, 5,
        2, 6,
        3, 7
    };
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(unsigned int) * 24,cubeIndices,GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(glm::vec3),0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

    //now lets set up the indirect command buffer, and the opengl worldtransforms buffer (one is an indirect buffer, other ssbo)
    glGenBuffers(1,&this->_debugBlasBvhBoxesSsboId);

    glGenBuffers(1,&this->_debugBlasBvhObjectWorldtransformsSsboId);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,this->_debugBlasBvhObjectWorldtransformsSsboId);
    glBufferData(GL_SHADER_STORAGE_BUFFER,sizeof(glm::mat4)*this->_maximumObjectCount,nullptr,GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);

    glGenBuffers(1,&this->_debugBlasBvhIndirectCommandsIndirectBufferObjectId);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER,this->_debugBlasBvhIndirectCommandsIndirectBufferObjectId);
    glBufferData(GL_DRAW_INDIRECT_BUFFER,sizeof(IndirectCommandData)*this->_maximumObjectCount,nullptr,GL_DYNAMIC_DRAW);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER,0);

    this->_debugBlasBvhIndirectCommandDatas.reserve(this->_maximumObjectCount);

    this->_debugBvhShader.Init("assets/shaders/debug_bvh_shader.vert","assets/shaders/debug_bvh_shader.frag");


    // Setting up buffers for tlas nodes, both cpu and gpuside(gpu side reserve happens in initcl)
    this->_temporaryObjectBlasInstances.reserve(this->_maximumObjectCount);
    this->_tlasBvhNodes.reserve(this->_maximumObjectCount * 2);

    //setting up debugTlasBvhRendering here
    this->_debugTlasBvhShader.Init("assets/shaders/debug_tlas_bvh_shader.vert","assets/shaders/debug_tlas_bvh_shader.frag");

    glGenBuffers(1,&this->_debugTlasBvhBoxesSsboId);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,this->_debugTlasBvhBoxesSsboId);
    glBufferData(GL_SHADER_STORAGE_BUFFER,sizeof(TlasBvhNode) * this->_maximumObjectCount*2,nullptr,GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);

    InitCL();
}

void Scene::Resize(int newWidth, int newHeight)
{
    if(newWidth <= 0 || newHeight <= 0) return;

    cl_int clError;

    clError = clReleaseMemObject(clOpenglInteropTex);
    CHECK_ERROR(clError);

    _renderTexture.Resize(newWidth,newHeight);
    _renderBuffer.Resize(newWidth,newHeight);

    _renderFrameBuffer.AttachTexture(_renderTexture);
    _renderFrameBuffer.AttachRenderBuffer(_renderBuffer);

    _camera.Resize(newWidth,newHeight);

    _viewportWidth = newWidth;
    _viewportHeight = newHeight;

    clOpenglInteropTex = clCreateFromGLTexture(clContext,CL_MEM_WRITE_ONLY,GL_TEXTURE_2D,0,_renderTexture.GetId(),&clError);
    CHECK_ERROR(clError);

    clError = clReleaseMemObject(clHelperBuffer); CHECK_ERROR(clError);
    clHelperBuffer = clCreateBuffer(clContext,CL_MEM_READ_WRITE,sizeof(RgbaData)*newWidth*newHeight,nullptr,&clError);
    CHECK_ERROR(clError);
    ResetPathTracedFrameIndex();
}


void Scene::Render()
{

    auto start = std::chrono::steady_clock::now();


    if(_isRenderingPathTraced)
    {
        PathTracedRender();
    }
    else
    {
        RasterizeRender();
    }

    if(_isDebugBlasRenderEnabled)
    {
        RenderDebugBlasBvhBoxes();
    }

    if(_isDebugTlasRenderEnabled)
    {
        RenderDebugTlasBvhBoxes();
    }

    RenderGizmo();

    auto end = std::chrono::steady_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    auto renderTime = diff.count();
    _renderingFps = 1000000.0f / renderTime;

}

void Scene::RasterizeRender()
{
    glViewport(0,0,_viewportWidth,_viewportHeight);

    _renderFrameBuffer.Bind();

    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    

    _renderFrameBuffer.Unbind();
}

void Scene::PathTracedRender()
{
    glFinish();

    cl_int clError;
    clError = clEnqueueAcquireGLObjects(clCommandQueue,1,&clOpenglInteropTex,0, nullptr, nullptr);
    CHECK_ERROR(clError);

    CameraData cameraData;
    cameraData.position = GlmToCl(glm::vec4(_camera.GetPosition(),1));
    cameraData.forward = GlmToCl(glm::vec4(_camera.GetFront(),1));
    cameraData.upward = GlmToCl(glm::vec4(_camera.GetUp(),1));
    cameraData.leftward = GlmToCl(glm::vec4(_camera.GetRight(),1));
    cameraData.fovx = _camera.GetFovx();
    cameraData.aspect = _camera.GetAspect();
    cameraData.zNear = _camera.GetZNear();
    cameraData.zFar = _camera.GetZFar();

    clError = clEnqueueWriteBuffer(clCommandQueue,clCameraDataBuffer,CL_TRUE,0,sizeof(CameraData),&cameraData,0,nullptr,nullptr);
    CHECK_ERROR(clError);

    clError = clSetKernelArg(clPathTracerKernel,0,sizeof(cl_mem),&clOpenglInteropTex);CHECK_ERROR(clError);
    clError = clSetKernelArg(clPathTracerKernel,1,sizeof(cl_mem),&clHelperBuffer);CHECK_ERROR(clError);
    clError = clSetKernelArg(clPathTracerKernel,2,sizeof(int),&_viewportWidth);CHECK_ERROR(clError);
    clError = clSetKernelArg(clPathTracerKernel,3,sizeof(int),&_viewportHeight);CHECK_ERROR(clError);
    clError = clSetKernelArg(clPathTracerKernel,4,sizeof(cl_mem),&clCameraDataBuffer);CHECK_ERROR(clError);
    clError = clSetKernelArg(clPathTracerKernel,5,sizeof(cl_mem),&_vertexPositionDataBuffer);CHECK_ERROR(clError);
    clError = clSetKernelArg(clPathTracerKernel,6,sizeof(cl_mem),&_vertexAttributeDataBuffer);CHECK_ERROR(clError);
    clError = clSetKernelArg(clPathTracerKernel,7,sizeof(cl_mem),&_triangleIndicesDataBuffer);CHECK_ERROR(clError);
    clError = clSetKernelArg(clPathTracerKernel,8,sizeof(cl_mem),&_bottomLevelBvhNodeDatasBuffer);CHECK_ERROR(clError);
    clError = clSetKernelArg(clPathTracerKernel,9,sizeof(cl_mem),&_rgbaDatasBuffer);CHECK_ERROR(clError);
    clError = clSetKernelArg(clPathTracerKernel,10,sizeof(cl_mem),&_textureDatasBuffer);CHECK_ERROR(clError);
    clError = clSetKernelArg(clPathTracerKernel,11,sizeof(cl_mem),&_materialDataBuffer);CHECK_ERROR(clError);
    clError = clSetKernelArg(clPathTracerKernel,12,sizeof(cl_mem),&_modelDataBuffer);CHECK_ERROR(clError);
    clError = clSetKernelArg(clPathTracerKernel,13,sizeof(cl_mem),&_objectDataBuffer);CHECK_ERROR(clError);
    //cl_int objectCount = _objectDatas.size();
    clError = clSetKernelArg(clPathTracerKernel,14,sizeof(cl_mem),&this->_tlasBvhNodesBuffer);CHECK_ERROR(clError);
    cl_int tlasBvhRootIndexIndicator = this->_tlasBvhNodes.size() > 0 ? 0 : -1;
    clError = clSetKernelArg(clPathTracerKernel,15,sizeof(int),&tlasBvhRootIndexIndicator);CHECK_ERROR(clError);
    clError = clSetKernelArg(clPathTracerKernel,16,sizeof(int),&_frameIndex);CHECK_ERROR(clError);

    size_t localSize[2] = { 16, 16 };
    size_t globalSize[2] = {((_viewportWidth + localSize[0] -1) / localSize[0]) * localSize[0],
                            ((_viewportHeight + localSize[1] -1) / localSize[1]) * localSize[1]};
    clError = clEnqueueNDRangeKernel(clCommandQueue,clPathTracerKernel,2,nullptr,globalSize,localSize,0,nullptr,nullptr);
    CHECK_ERROR(clError);

    clFinish(clCommandQueue);

    clError = clEnqueueReleaseGLObjects(clCommandQueue,1,&clOpenglInteropTex, 0, nullptr, nullptr);
    CHECK_ERROR(clError);

    clFinish(clCommandQueue);

    if(_frameIndex < 1000000000)
    {
        ++_frameIndex;
    }
}

void Scene::RenderGizmo()
{
    if(!_isGizmoVisible || _objectWithGizmoIndex < 0 || _objectWithGizmoIndex >= _objectDatas.size()) return;

    glViewport(0,0,_viewportWidth,_viewportHeight);
    glEnable(GL_DEPTH_TEST);

    _gizmoShader.Bind();

    _renderFrameBuffer.Bind();

    glClear( GL_DEPTH_BUFFER_BIT);

    _gizmoShader.SetUniform<glm::mat4>("projectionTransform",_camera.GetPerspectiveMatrix());
	_gizmoShader.SetUniform<glm::mat4>("viewTransform",_camera.GetViewMatrix());

    for(int i=0;i<3;++i)
    {
        glm::vec3 color = glm::vec3(0);
        
        color[i] = 0.9;
        if(i == _currentlyHighlightedGizmoAxis)color[i] = 0;
        _gizmoShader.SetUniform<glm::vec3>("uColor",color);

        int objectIndex = _objectWithGizmoIndex;
        Mesh<VertexP3N3T2>* meshPointer = nullptr;
        glm::mat4 gizmoTypeTransform;
        if(_currentGizmoType == GizmoType::GIZMO_TRANSLATION)
        {
            meshPointer = &_arrowGizmoMesh;
            gizmoTypeTransform = _arrowGizmoModelTransforms[i];
        }
        else if(_currentGizmoType == GizmoType::GIZMO_SCALE)
        {
            meshPointer = &_cubeGizmoMesh;
            gizmoTypeTransform = _cubeGizmoModelTransforms[i];
        }
        else
        {
            meshPointer = &_ringGizmoMesh;
            gizmoTypeTransform = glm::mat4_cast(_objectTransforms[objectIndex].internalRotation) * _ringGizmoModelTransforms[i];
        }

        float objectDistanceFromCamera = glm::length( _camera.GetPosition() -  _objectTransforms[objectIndex].position);

        glm::mat4 finalWorldTransform = glm::translate(glm::mat4(1.0f),_objectTransforms[objectIndex].position) *
            glm::scale(glm::mat4(1.0f),glm::vec3(objectDistanceFromCamera * sizeK)) *
            gizmoTypeTransform;

        _gizmoShader.SetUniform<glm::mat4>("uWorldTransform",finalWorldTransform);

        meshPointer->Draw();
    }
	

    _renderFrameBuffer.Unbind();

    _gizmoShader.Unbind();

    glDisable(GL_DEPTH_TEST);
}

void Scene::RenderDebugBlasBvhBoxes()
{
    glViewport(0,0,_viewportWidth,_viewportHeight);
    

    if (!_isDebugBlasRenderEnabled) return;

    if (_debugBlasBvhIndirectCommandDatas.size() <= 0) return;

    _renderFrameBuffer.Bind();

    glClear( GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);

    _debugBvhShader.Bind();

    glm::mat4 view = _camera.GetViewMatrix();

    glm::mat4 projection = _camera.GetPerspectiveMatrix();

    glm::mat4 viewProjection = projection * view;

    _debugBvhShader.SetUniform<glm::mat4>("uViewProjection",viewProjection);

    glBindBufferBase( GL_SHADER_STORAGE_BUFFER,0,_debugBlasBvhBoxesSsboId);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,1,_debugBlasBvhObjectWorldtransformsSsboId);

    glBindVertexArray(_debugBvhVaoId);

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER,_debugBlasBvhIndirectCommandsIndirectBufferObjectId);

    glMultiDrawElementsIndirect(GL_LINES, GL_UNSIGNED_INT, nullptr, _debugBlasBvhIndirectCommandDatas.size(), sizeof(IndirectCommandData));

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
    glBindVertexArray(0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,0,0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,1,0);

    //glEnable(GL_DEPTH_TEST);

    _debugBvhShader.Unbind();

    _renderFrameBuffer.Unbind();
}

void Scene::RenderDebugTlasBvhBoxes()
{
    glViewport(0,0,_viewportWidth,_viewportHeight);

    if (!_isDebugTlasRenderEnabled) return;

    if (_tlasBvhNodes.size() <= 0) return;

    _renderFrameBuffer.Bind();

    glClear( GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);

    _debugTlasBvhShader.Bind();

    glm::mat4 view = _camera.GetViewMatrix();

    glm::mat4 projection = _camera.GetPerspectiveMatrix();

    glm::mat4 viewProjection = projection * view;

    _debugTlasBvhShader.SetUniform<glm::mat4>("uViewProjection",viewProjection);

    glBindBufferBase( GL_SHADER_STORAGE_BUFFER,0,_debugTlasBvhBoxesSsboId);
    glBindVertexArray(_debugBvhVaoId);

    glDrawElementsInstanced(GL_LINES,24,GL_UNSIGNED_INT,0,_tlasBvhNodes.size());

    glBindVertexArray(0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,0,0);

    _debugTlasBvhShader.Unbind();

    _renderFrameBuffer.Unbind();
}

void Scene::Update(float deltaTime)
{
    _camera.Update(deltaTime);

    if(_camera.GetIsCameraMoving())
    {
        ResetPathTracedFrameIndex();
    }
}

void Scene::MouseMove(float newX, float newY,GizmoInteractionInfo* gizmoInteractionInfo)
{
    _currentMousePos = glm::vec2(newX,newY);
    

    if(_isFreeCam)
    {
        _currentlyHighlightedGizmoAxis = -1;

        if(_isMouseFirstPos)
        {
            _isMouseFirstPos = false;
            _previousMousePos = glm::vec2(newX,newY);
        }
        else
        {
            float dx = _currentMousePos.x - _previousMousePos.x;
            float dy = _previousMousePos.y - _currentMousePos.y;

            _previousMousePos = glm::vec2(newX,newY);

            if(!_isCurrentGizmoInteractedWith)
            {
                _camera.Rotate(dx,dy);
                ResetPathTracedFrameIndex();
            }
        }
    }
    else
    {
        if(_isCurrentGizmoInteractedWith)
        {
            CalculateGizmoInteraction(newX,newY);
            gizmoInteractionInfo->hasInteractionHappend = true;
            gizmoInteractionInfo->objectIndexWithGizmo = _objectWithGizmoIndex;
        }
        else
        {
            _currentlyHighlightedGizmoAxis = -1;

            PickResult currentGizmoPickResult;
            PickCurrentGizmo(newX,newY,&currentGizmoPickResult);
            if(currentGizmoPickResult.type == PickResultType::GIZMO && currentGizmoPickResult.pickedIndex >= 0)
            {
                _currentlyHighlightedGizmoAxis = currentGizmoPickResult.pickedIndex;
                //std::cout<< "Currently Highlighted axis: " << _currentlyHighlightedGizmoAxis << "\n";
            }
        }
        
    }
}

void Scene::MouseClick(int button, int action, ObjectPickInfo* objectPickInfo)
{
    if(button == 0 && action == 0) // if left click
    {
        int x = _currentMousePos.x;
        int y = _currentMousePos.y;

        if(x >= 0 && x < _viewportWidth && y >= 0 && y < _viewportHeight)
        {
            if(_currentlyHighlightedGizmoAxis >= 0 && _isViewEnlarged == false) // If we are hovering above a gizmo axis
            {
                EnterGizmoInteractionMode();
            }
            else
            {
                if(_isViewEnlarged == false)
                {
                    //std::cout<< "Picking starts at x: " << x << " and y: "<<y<<"\n";
                    PickResult pickResult;
                    PickScene(x,y,&pickResult);
                    if(pickResult.type == PickResultType::OBJECT)
                    {
                        //std::cout<<"Picked object with id: " << pickResult->pickedObjectIndex<<"\n";
                        ChooseObject(pickResult.pickedIndex);

                        objectPickInfo->pickedObjectIndex = pickResult.pickedIndex;
                    }
                    else if(pickResult.type == PickResultType::NONE)
                    {
                        ChooseObject(-1);
                        objectPickInfo->pickedObjectIndex = -1;
                    }
                }
                else
                {
                    ChooseObject(-1);
                    objectPickInfo->pickedObjectIndex = -1;
                }
            }
            
        }
    }
    else if(button == 0 && action == 1) // left release
    {
        LeaveGizmoInteractionMode(); // handles proper state
    }
    else if(button == 1 && action == 0) // right click -> we also leave gizmo state
    {
        LeaveGizmoInteractionMode();
        ChooseObject(-1);
        objectPickInfo->pickedObjectIndex = -1;
    }
}

void Scene::KeyInput(int key, int action, int mods)
{
    //Camera stuff
    if(action <= 1) //press or release
    {
        int axis = 0;
        int direction = 0;
        int influence = action == 0? -1 : 1;
        switch(key)
        {
            case 65: //A
                axis = 0;
                direction = 1;
                break;
            case 68: //D
                axis = 0;
                direction = -1;
                break;
            case 87: //W
                axis = 2;
                direction = 1;
                break;
            case 83: //S
                axis = 2;
                direction = -1;
                break;
            case 32: //SPACE
                axis = 1;
                direction = 1;
                break;
            case 340:
                axis = 1;
                direction = -1;
                break;
        }

        if(!_isCurrentGizmoInteractedWith)
        {
            _camera.TryInfluenceMovement(axis,direction,influence);
        }
    }

    if(key == 70 && action == 1) // If F was pressed
    {
        if(!_isCurrentGizmoInteractedWith)
        {
            if(_isFreeCam)
            {
                _isFreeCam = false;

                CheckForHighlightedAxis();
            }
            else
            {
                _isFreeCam = true;
                _isMouseFirstPos = true;

                _currentlyHighlightedGizmoAxis = -1;
            }
        }
    }

    if(action == 1)
    {
        if(!_isCurrentGizmoInteractedWith)
        {
            if(key == 49) // pressed 1
            {
                _currentGizmoType = GizmoType::GIZMO_TRANSLATION;
            }
            else if(key == 50) // pressed 2
            {
                _currentGizmoType = GizmoType::GIZMO_SCALE;
            }
            else if(key == 51) // pressed 3
            {
                _currentGizmoType = GizmoType::GIZMO_ROTATION;
            }

            CheckForHighlightedAxis();
        }
    }

    if( key == 341) // CTRL
    {
        if(action == 1)// press
        {
            _isCtrlPressed = true;
            
        }
        else if(action == 0) // release
        {
            _isCtrlPressed = false;

            
        }
    }

    if(key == 82) // R
    {
        if(action == 1)//Press
        {
            //toggle camera zoomies
            _doesCameraHaveTheZoomies = !_doesCameraHaveTheZoomies;
            this->_camera.SetIsSpeedy(_doesCameraHaveTheZoomies);
        }
    }
}

static float SahBinSplitCost(const glm::vec3& parentBoxSize,const glm::vec3& box1Size, const glm::vec3& box2Size, 
    int box1TriagCount, int box2TriagCount)
{
	float parentBoxArea = 2.0f * (parentBoxSize.x * parentBoxSize.y + parentBoxSize.x * parentBoxSize.z + parentBoxSize.y * parentBoxSize.z);
	float box1Area = 2.0f* (box1Size.x * box1Size.y + box1Size.x * box1Size.z + box1Size.y * box1Size.z);
	float box2Area = 2.0f* (box2Size.x * box2Size.y + box2Size.x * box2Size.z + box2Size.y * box2Size.z);

	return 1.0f + box1Area/parentBoxArea  * box1TriagCount + box2Area/parentBoxArea * box2TriagCount;
}

static float SahLeafCost(const glm::vec3& boxSize, int triagCount)
{
	return triagCount;
}

static void FeedPosToAABB4(AABB4& box, glm::vec3& pos)
{
	if (box.max.x < pos.x)
	{
		box.max.x = pos.x;
	}
	if (box.max.y < pos.y)
	{
		box.max.y = pos.y;
	}
	if (box.max.z < pos.z)
	{
		box.max.z = pos.z;
	}

	if (box.min.x > pos.x)
	{
		box.min.x = pos.x;
	}
	if (box.min.y > pos.y)
	{
		box.min.y = pos.y;

	}
	if (box.min.z > pos.z)
	{
		box.min.z = pos.z;
	}
}

bool Scene::GetSurfaceAreaHeuristicSplitDatas(const BvhNodeData &bvhNode, 
    const std::vector<glm::vec3> &vertexPositions, 
    const std::vector<glm::ivec3> &triangleVertexIndices, 
    int &splitAxisInd, float &splitValueAlongAxis)
{
    glm::vec3 currentNodeBoxSize = glm::vec3(bvhNode.box.max-bvhNode.box.min);

    //std::cout << "Current box size: x: " << currentNodeBoxSize.x <<" y: " << currentNodeBoxSize.y << " z: " << currentNodeBoxSize.z<<"\n";
    float* currentNodeBoxSizePerAxis = &currentNodeBoxSize.x;
    int currentNodeTriangleCount = bvhNode.endIndex-bvhNode.startIndex;

    // If we were to leave the node as a leaf, this would be the cost of doing that.
    float leafCostOfNode = SahLeafCost(currentNodeBoxSize,currentNodeTriangleCount);

    //Now we prepare for a minimum-search, where we will search on the minimum intersection cost of different bin-based splits.
    //We are going to do this according to the SAH's rules.
    float minCost = leafCostOfNode;
    float bestSplitValueAlongAxis = FLT_MAX;
    int bestAxisForSplitting = -1;

    glm::vec3 currentNodeBoxMin = bvhNode.box.min;
    float * currentNodeBoxMinPerAxis = &currentNodeBoxMin.x;
    for(int axisIndex = 0;axisIndex<3;++axisIndex)
    {
        for(int binIndex = 1;binIndex<_sahBinSize;++binIndex)
        {
            // Here we basically specify the mesh-space value we will try to split the given bvhnode's box at.
            // Essentially this will be the quote-on-quote border of the split
            float currentBinValueAlongAxis = ((float)binIndex / (float)_sahBinSize) * currentNodeBoxSizePerAxis[axisIndex] +
                currentNodeBoxMinPerAxis[axisIndex];

            glm::vec3 absoluteMinimumVector = glm::vec3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
            glm::vec3 absoluteMaximumVector = glm::vec3(FLT_MAX,FLT_MAX,FLT_MAX);

            // Here we create two splitboxes, which we will virtually split our triangles into.
            // First, we initialize the boxes to incorrect values, and as we process the triangles
            //  we feed the positions into the boxes, and grow them this way. 
            //  (They are invalid bc their max is the abs minimal vec, and their min is the abs max vec)
            //  (if any of them remain in this state, aka they dont have ANY triangles inside them by the end we discrad the split)
            AABB4 minSideSplitBox = {glm::vec4(absoluteMaximumVector,0.0f),glm::vec4(absoluteMinimumVector,1.0f)};
            int minSideTriangleCount = 0;
            AABB4 maxSideSplitBox = {glm::vec4(absoluteMaximumVector,0.0f),glm::vec4(absoluteMinimumVector,1.0f)};
            int maxSideTriangleCount = 0;
            

            for(int triangleIndex = bvhNode.startIndex; triangleIndex<bvhNode.endIndex; ++triangleIndex)
            {
                glm::ivec3 currentTriangleIndices = triangleVertexIndices[triangleIndex];

                glm::vec3 triangleVertex1 = vertexPositions[currentTriangleIndices.x];
                glm::vec3 triangleVertex2 = vertexPositions[currentTriangleIndices.y];
                glm::vec3 triangleVertex3 = vertexPositions[currentTriangleIndices.z];

                glm::vec3 triangleMiddle = (triangleVertex1 + triangleVertex2 + triangleVertex3) / 3.0f;
                float* triangleMiddlePerAxis = &triangleMiddle.x;
                float currentComparisonValueAlongAxis = triangleMiddlePerAxis[axisIndex];

                //Here we check if the middle of the triangle is to the min side or to the max side of the binSplit border.
                if(currentComparisonValueAlongAxis <= currentBinValueAlongAxis)
                {   
                    //If it is to the min side, we place the triangle inside the minSideSplitBox, by feeding it into the box
                    ++minSideTriangleCount;
                    FeedPosToAABB4(minSideSplitBox,triangleVertex1);
                    FeedPosToAABB4(minSideSplitBox,triangleVertex2);
                    FeedPosToAABB4(minSideSplitBox,triangleVertex3);
                }
                else
                {
                    // If its middle landed on the max side, we feed it into that one
                    ++maxSideTriangleCount;
                    FeedPosToAABB4(maxSideSplitBox,triangleVertex1);
                    FeedPosToAABB4(maxSideSplitBox,triangleVertex2);
                    FeedPosToAABB4(maxSideSplitBox,triangleVertex3);
                }
            }

            // Very important check, if our current split is so bad that no triangles got on one side, 
            // then we definitely dont want to split this way, since it would be worse then leaving it as a leaf.
            if(minSideTriangleCount <= 0 || maxSideTriangleCount <= 0) continue;

            glm::vec3 minSideSplitBoxSize = glm::vec3(minSideSplitBox.max - minSideSplitBox.min);
            glm::vec3 maxSideSplitBoxSize = glm::vec3(maxSideSplitBox.max - maxSideSplitBox.min);

            //std::cout << "Current box size: x: " << minSideSplitBoxSize.x <<" y: " << minSideSplitBoxSize.y << " z: " << minSideSplitBoxSize.z<<"\n";
            // We querry if our current split's cost is better than any previously found ones, even the leafCost
            float currentSahSplitCost = SahBinSplitCost(currentNodeBoxSize,minSideSplitBoxSize,maxSideSplitBoxSize,
                minSideTriangleCount, maxSideTriangleCount);
            //std::cout<< "CurrentSahSplitCost: " << currentSahSplitCost<<"\n";
            //std::cout<< "minCost: " << minCost<<"\n";
            if(currentSahSplitCost < minCost)
            {
                minCost = currentSahSplitCost;
                bestAxisForSplitting = axisIndex;
                bestSplitValueAlongAxis = currentBinValueAlongAxis;
            }
        }
    }

    if(minCost < leafCostOfNode)
    {
        //If we found a better split than leaving it as a leaf.
        splitAxisInd = bestAxisForSplitting;
        splitValueAlongAxis = bestSplitValueAlongAxis;

        return true;
    }
    else
    {
        // If we havent found a better split, then we must not split the node any further, and leave it as a leaf.
        // In this case we just load random bullshit values into the parameters
        splitAxisInd = -1;
        splitValueAlongAxis = FLT_MAX;
    }

    
    return false;
}


void Scene::SplitBvhNodeRecursive(int bvhNodeIndex, int recursionDepth, const std::vector<glm::vec3> &vertexPositions, 
    std::vector<BvhNodeData> &bvhNodeStrorage, std::vector<glm::ivec3> &triangleVertexIndices, MeshInfo* meshInfo)
{
    //Lets quickly add some data to the meshinfo struct
    //std::cout<<"Recursion depth: "<<recursionDepth<<"\n";
    if(meshInfo != nullptr && meshInfo->bvhDepth < recursionDepth)
    {
        meshInfo->bvhDepth = recursionDepth;
    }

    // The current node data we want to either split or keep as a leaf Node.
    BvhNodeData currentNode = bvhNodeStrorage[bvhNodeIndex];

    int currentNodeTriangleCount = currentNode.endIndex - currentNode.startIndex;
    int minSideTriangleCount = 0;
    int maxSideTriangleCount = 0;

    int bestAxisToSplitAt = -1;
    float bestSplitValueAlongAxis = FLT_MAX;

    bool shouldNodeBeSplit = GetSurfaceAreaHeuristicSplitDatas(currentNode,vertexPositions,triangleVertexIndices,
        bestAxisToSplitAt,bestSplitValueAlongAxis);
    
    //If we dont have to split the current node, then we have no more work to do, therefore we return.
    if(!shouldNodeBeSplit) return;
    
    //We now sort the triangles linearly, based on which side of the splitvalue border their mid falls into.
    //The way we will do this, is with a front and back indices, if we find a wrong one on the 
    int frontIndex = currentNode.startIndex;
    int backIndex = currentNode.endIndex -1;

    while(frontIndex <= backIndex)
    {
        //we always analyze the frontindex triangle.
        glm::ivec3 currentTriangleIndices = triangleVertexIndices[frontIndex];

        glm::vec3 posA = vertexPositions[currentTriangleIndices.x];
        glm::vec3 posB = vertexPositions[currentTriangleIndices.y];
        glm::vec3 posC = vertexPositions[currentTriangleIndices.z];

        glm::vec3 currentTriangleMiddle = (posA + posB + posC) / 3.0f;
        float* currentTriangleMiddlePerAxis = &currentTriangleMiddle.x;
        float compareValueAlongAxis = currentTriangleMiddlePerAxis[bestAxisToSplitAt];

        if(compareValueAlongAxis <= bestSplitValueAlongAxis)
        {
            // Here we have found a triangle on the min side, where it should belong at.
            // Since it is at the right place we just march to the next triangle forward
            ++frontIndex;
            ++minSideTriangleCount;
        }
        else
        {
            //We found a triangle that should belong to the max side.
            //Therefore we slap it to the backindex, at the back of the interval, because it will 100% be good there.
            glm::ivec3 triangleIndicesAtBeckIndex = triangleVertexIndices[backIndex];
            triangleVertexIndices[backIndex] = currentTriangleIndices;
            triangleVertexIndices[frontIndex] = triangleIndicesAtBeckIndex;

            --backIndex;
        }
    }

    maxSideTriangleCount = currentNodeTriangleCount - minSideTriangleCount;

    // One final (kind of unnecessary) check
    if(maxSideTriangleCount > 0 && minSideTriangleCount > 0)
    {
        //Here, we create the two new children recursively
        BvhNodeData minChildBvhNode;
        minChildBvhNode.minChild = -1;
        minChildBvhNode.maxChild = -1;
        minChildBvhNode.startIndex = currentNode.startIndex;
        minChildBvhNode.endIndex = currentNode.startIndex + minSideTriangleCount;
        minChildBvhNode.box = CalculateAABB4BasedOnTriangles(currentNode.startIndex,currentNode.startIndex + minSideTriangleCount,
            vertexPositions,triangleVertexIndices);

        // Let's push the newly created minSideChild onto the storage, and set its index as the original node's minCHild
        int minChildIndex = bvhNodeStrorage.size();
        bvhNodeStrorage.push_back(minChildBvhNode);
        bvhNodeStrorage[bvhNodeIndex].minChild = minChildIndex;

        // We created a new node, lets check if its worth splitting that as well -> recursion
        SplitBvhNodeRecursive(minChildIndex,recursionDepth + 1,vertexPositions,bvhNodeStrorage,triangleVertexIndices,meshInfo);

            
        BvhNodeData maxChildBvhNode;
        maxChildBvhNode.minChild = -1;
        maxChildBvhNode.maxChild = -1;
        maxChildBvhNode.startIndex = currentNode.startIndex + minSideTriangleCount;
        maxChildBvhNode.endIndex = currentNode.endIndex;
        maxChildBvhNode.box = CalculateAABB4BasedOnTriangles(currentNode.startIndex + minSideTriangleCount,currentNode.endIndex,
            vertexPositions,triangleVertexIndices);

        int maxChildIndex = bvhNodeStrorage.size();
        bvhNodeStrorage.push_back(maxChildBvhNode);
        bvhNodeStrorage[bvhNodeIndex].maxChild = maxChildIndex;

        SplitBvhNodeRecursive(maxChildIndex,recursionDepth + 1,vertexPositions,bvhNodeStrorage,triangleVertexIndices,meshInfo);
    }
}


bool Scene::TryLoadPathTracedMesh(const std::string &filePathRelative, 
    std::vector<VertexPositionData> &newMeshVertexPositions,
    std::vector<TriangleIndicesData> &newMeshTriangleIndices, 
    std::vector<VertexAttributeData> &newMeshVertexAttributes,
    std::vector<BvhNodeData>& newMeshBvhNodes,
    MeshInfo *meshInfo)
{
    //Clear all containers sent by caller
    newMeshVertexPositions.clear();
    newMeshTriangleIndices.clear();
    newMeshVertexAttributes.clear();
    newMeshBvhNodes.clear();

    std::vector<glm::vec3> newVertexPositions;
    std::vector<glm::vec3> newVertexNormals;
    std::vector<glm::vec2> newVertexTexCoords;
    std::vector<glm::vec<3,int>> newTriangleVertexIndices;

    bool wasFileLoadingSuccessful = TryLoadObjFile(filePathRelative,newVertexPositions,newVertexNormals,
        newVertexTexCoords,newTriangleVertexIndices);

    if(!wasFileLoadingSuccessful) return false;
    
    //Lets copy the recieved data into our new buffers
    newMeshVertexPositions.resize(newVertexPositions.size());
    newMeshVertexAttributes.resize(newVertexPositions.size());
    for(int i=0;i<newMeshVertexPositions.size();++i)
    {
        newMeshVertexPositions[i].x = newVertexPositions[i].x;
        newMeshVertexPositions[i].y = newVertexPositions[i].y;
        newMeshVertexPositions[i].z = newVertexPositions[i].z;

        newMeshVertexAttributes[i].normal.x = newVertexNormals[i].x;
        newMeshVertexAttributes[i].normal.y = newVertexNormals[i].y;
        newMeshVertexAttributes[i].normal.z = newVertexNormals[i].z;
        newMeshVertexAttributes[i].textureCoords.x = newVertexTexCoords[i].x;
        newMeshVertexAttributes[i].textureCoords.y = newVertexTexCoords[i].y;
    }

    //Before we also finalize our triangleIndices vector, we have to create the bvhNode vector structure,
    // Using the splitNode recursive method, which also rearranges the triangles indices, therefore we have to do this first.

    //Now lets recursively create a BVH structure for our mesh.
    // Now lets prepare our root bvh node, which we will later on split recursively
    //This node contains the full range of the mesh's indices
    int outOfBoundsEndIndex =  newTriangleVertexIndices.size();
    BvhNodeData meshRootNode;
    meshRootNode.startIndex = 0;
    meshRootNode.endIndex = outOfBoundsEndIndex;
    meshRootNode.minChild = -1;
    meshRootNode.maxChild = -1;
    meshRootNode.box = CalculateAABB4BasedOnTriangles(0,outOfBoundsEndIndex,newVertexPositions,newTriangleVertexIndices);

    if(meshInfo != nullptr)
    {
        AABB4 box = meshRootNode.box;
        float lengthMax = glm::length(box.max);
        float lengthMin = glm::length(box.min);
        meshInfo->absMaxRadius = fmax(lengthMax,lengthMin);
        meshInfo->bvhRootMidYLevel = (box.max.y + box.min.y)/2;
    }

    // We push the root to the bvhNodeStorage, then recursively try to split it.
    newMeshBvhNodes.push_back(meshRootNode);if(meshInfo != nullptr) meshInfo->bvhDepth = 0;
    SplitBvhNodeRecursive(0,0,newVertexPositions,newMeshBvhNodes,newTriangleVertexIndices,meshInfo);

    //Now that our triangle indices are finalized, we can copy them into the proper arrays.
    newMeshTriangleIndices.resize(newTriangleVertexIndices.size());
    for(int i=0;i<newMeshTriangleIndices.size();++i)
    {
        newMeshTriangleIndices[i].x = newTriangleVertexIndices[i].x;
        newMeshTriangleIndices[i].y = newTriangleVertexIndices[i].y;
        newMeshTriangleIndices[i].z = newTriangleVertexIndices[i].z;
    }

    //lets add some good ol' data to the meshinfo struct
    if(meshInfo != nullptr)
    {
        meshInfo->bvhNodeCount = newMeshBvhNodes.size();
        meshInfo->triangleCount = newMeshTriangleIndices.size();
        meshInfo->vertexCount = newMeshVertexPositions.size();
    }

    return true;
}

//Quick Helper function with appending to GPU-side Buffers
static void AppendToClBuffer(cl_context context, cl_command_queue commandQueue,cl_mem* buffer,
    size_t sizeOfOneItem, int alreadyExistingCount, int appendAmount, void* data)
{
    
    cl_int clError;
    int newFullAmount = alreadyExistingCount + appendAmount;
    //std::cout<< "New buffer size: "<<newFullAmount<<"\n";
    cl_mem newBuffer = clCreateBuffer(context,CL_MEM_READ_ONLY,sizeOfOneItem*newFullAmount,
        nullptr,&clError);
    CHECK_ERROR(clError);
    
    

    if(alreadyExistingCount > 0)
    {
        //Here, we had originally data inside the GPU-SIde buffer, so for efficiency, we perform a GPU-side copying of data.
        clError = clEnqueueCopyBuffer(commandQueue,*buffer,newBuffer,0,0,
            sizeOfOneItem*alreadyExistingCount,0,nullptr,nullptr);
        CHECK_ERROR(clError);
    }
    
    //And now, that the new buffers are ready to be filled up with our data, lets upload that.
    clError = clEnqueueWriteBuffer(commandQueue,newBuffer,CL_TRUE,
        sizeOfOneItem*alreadyExistingCount,sizeOfOneItem*appendAmount,data,0,nullptr,nullptr);
    CHECK_ERROR(clError);

    // And lets delete the old buffers
    if(alreadyExistingCount > 0)
    {
        clError = clReleaseMemObject(*buffer);
        CHECK_ERROR(clError);
    }
    

    *buffer = newBuffer;
}


bool Scene::TryLoadMesh(const std::string &filePathRelative, MeshInfo * meshInfo)
{
    std::vector<VertexPositionData> newMeshVertexPositions;
    std::vector<VertexAttributeData> newMeshVertexAttributes;
    std::vector<TriangleIndicesData> newMeshTriangleIndices;
    std::vector<BvhNodeData> newMeshBvhNodes;

    bool wasLoadingSuccesful = TryLoadPathTracedMesh(filePathRelative,newMeshVertexPositions,newMeshTriangleIndices,
        newMeshVertexAttributes,newMeshBvhNodes,meshInfo);

    if(!wasLoadingSuccesful) return false;

    AddNewBvhNodeBoxesToDebugSsbo(newMeshBvhNodes);

    // Lets merge this new mesh data with the already existing ones.
    // But out data is now in its own, normalized indexing system, and we have to translate it to the global one.
    int alreadyExistingVertexCount = _vertexPositionData.size(); //Is equal to attribute size
    int alreadyExistingTriangleIndicesCount = _triangleIndicesData.size();
    int alreadyExistingBottomLevelBvhNodeCount = _bottomLevelBvhNodeDatas.size();

    int newMeshRootBvhNodeIndex = alreadyExistingBottomLevelBvhNodeCount;

    // when we merge the positions and attributes(vertices into the global array, their index shifts)
    // It gets shifted by the amount of already existing vertex count.
    for(int i=0;i<newMeshTriangleIndices.size();++i)
    {
        newMeshTriangleIndices[i].x += alreadyExistingVertexCount;
        newMeshTriangleIndices[i].y += alreadyExistingVertexCount;
        newMeshTriangleIndices[i].z += alreadyExistingVertexCount;
    }

    // Similiar thing happens with the bvh leaf node's triangle index interval pointers.
    // But these get shifted by the amount of triangle indices already present
    for(int i = 0;i<newMeshBvhNodes.size();++i)
    {
        newMeshBvhNodes[i].startIndex += alreadyExistingTriangleIndicesCount;
        newMeshBvhNodes[i].endIndex += alreadyExistingTriangleIndicesCount;
    }

    // And we also have to shift away the bvhNodes children indices, by the amount of already existing bvhNodeCOunt
    for(int i=0;i<newMeshBvhNodes.size();++i)
    {
        if(newMeshBvhNodes[i].minChild >= 0 && newMeshBvhNodes[i].maxChild >= 0)
        {
            //Otherwise it is a leaf and we dont want to alter its -1 , -1 (minchild, maxchild) special state
            newMeshBvhNodes[i].minChild += alreadyExistingBottomLevelBvhNodeCount;
            newMeshBvhNodes[i].maxChild += alreadyExistingBottomLevelBvhNodeCount;
        }
    }

    //Our data is now well-prepared to be merged with CPU-side  buffers
    _vertexPositionData.insert(_vertexPositionData.end(),newMeshVertexPositions.begin(),newMeshVertexPositions.end());
    _vertexAttributeData.insert(_vertexAttributeData.end(),newMeshVertexAttributes.begin(),newMeshVertexAttributes.end());
    _triangleIndicesData.insert(_triangleIndicesData.end(),newMeshTriangleIndices.begin(),newMeshTriangleIndices.end());
    _bottomLevelBvhNodeDatas.insert(_bottomLevelBvhNodeDatas.end(),newMeshBvhNodes.begin(),newMeshBvhNodes.end());

    // We will need to be able to querry the bvhRootIndex of a mesh, based on its index.
    // Therefore we have a vector that does exactly this, but it is only needed CPU-side, and not GPU side, since it would be an 
    // INDIRECTION.
    int newMeshIndex = _meshBvhRootIndexData.size();
    if(meshInfo != nullptr)
    {
        meshInfo->meshIndex = newMeshIndex;
    }
    _meshBvhRootIndexData.push_back(newMeshRootBvhNodeIndex);

    // Let's upload our data to the GPU!
    AppendToClBuffer(clContext,clCommandQueue,&_vertexPositionDataBuffer,sizeof(VertexPositionData),alreadyExistingVertexCount,
        newMeshVertexPositions.size(),newMeshVertexPositions.data());

    AppendToClBuffer(clContext,clCommandQueue,&_vertexAttributeDataBuffer,sizeof(VertexAttributeData),alreadyExistingVertexCount,
        newMeshVertexAttributes.size(),newMeshVertexAttributes.data());

    AppendToClBuffer(clContext,clCommandQueue,&_triangleIndicesDataBuffer,sizeof(TriangleIndicesData),alreadyExistingTriangleIndicesCount,
        newMeshTriangleIndices.size(),newMeshTriangleIndices.data());

    AppendToClBuffer(clContext,clCommandQueue,&_bottomLevelBvhNodeDatasBuffer,sizeof(BvhNodeData),alreadyExistingBottomLevelBvhNodeCount,
        newMeshBvhNodes.size(),newMeshBvhNodes.data());

    // We also load a rasterized verrsion of the mesh..i know kinda memory bad, but itizwhaditis
    Mesh<VertexP3N3T2> newRasterMesh;
    newRasterMesh.Load(filePathRelative);
    _rasterizedMeshes.push_back(newRasterMesh);

    

    return true;
}


bool Scene::TryLoadPathTracedTexture(const std::string &filePathRelative, std::vector<RgbaData> &newRgbaDatas, 
    int &width, int &height, TextureInfo *textureInfo)
{
    newRgbaDatas.clear();

    //Lets use stb_image header/library to load the .png, .jpg, .bmp picture into a floatstream, having RGBA units
    int loadedTextureWidth = -1;
    int loadedTextureHeight = -1;
    int channelsInPicture = -1;
    float * pngByteStream = stbi_loadf(filePathRelative.c_str(),&loadedTextureWidth,&loadedTextureHeight,&channelsInPicture,4);

    if(pngByteStream == nullptr) return false;

    width = loadedTextureWidth;
    height = loadedTextureHeight;

    // Here we do a pre-allocation for faster loading times, 
    // and move all the data from the continuos array into the structured array
    newRgbaDatas.resize(loadedTextureWidth * loadedTextureHeight);
    for(int i=0; i<loadedTextureWidth * loadedTextureHeight; ++i)
    {
        newRgbaDatas[i].r = pngByteStream[4*i];
        newRgbaDatas[i].g = pngByteStream[4*i + 1];
        newRgbaDatas[i].b = pngByteStream[4*i + 2];
        newRgbaDatas[i].a = pngByteStream[4*i + 3];
    }

    if(textureInfo != nullptr)
    {
        textureInfo->width = width;
        textureInfo->height = height;
    }

    stbi_image_free(pngByteStream);

    /**
     * HOLY GUACKAMOLE PLEASE WATCH OUT HERE
     * Since we store POINTERS to these textures inside the App(View) layer, we have to be REALLLLLYY careful,
     *  because those stored pointers will be invalidated once we push into the vector, therefore we must reset all of those pointers.
     */
    Texture newRasterTexture;
    newRasterTexture.Init(filePathRelative);
    _rasterizedTextures.push_back(newRasterTexture);

    return true;
}



bool Scene::TryLoadTexture(const std::string &filePathRelative, TextureInfo *textureInfo)
{
    std::vector<RgbaData> newRgbaDatas;
    int newTextureWidth = -1;
    int newTextureHeight = -1;

    bool wasTextureLoadingSuccesful = TryLoadPathTracedTexture(filePathRelative,newRgbaDatas,
        newTextureWidth,newTextureHeight,textureInfo);
    
    if(!wasTextureLoadingSuccesful) return false;

    //Now, that our data is loaded, we have to merge it with out already established data buffers
    // First, we need these two integer datas, for insertion into the gpu buffers.
    int alreadyExistingRgbaValueCount = _rgbaDatas.size();

    //We can safely just append the RGBA values 
    _rgbaDatas.insert(_rgbaDatas.end(),newRgbaDatas.begin(),newRgbaDatas.end());

    //Creation of the texture
    TextureData newTextureData;
    newTextureData.width = newTextureWidth;
    newTextureData.height = newTextureHeight;
    //Because this is the index where the loaded texture's first rgba(top left) corner value starts
    newTextureData.startIndex = alreadyExistingRgbaValueCount; 

    int alreadyExistingTextureDataCount = _textureDatas.size();
    if(textureInfo != nullptr)
    {
        textureInfo->textureIndex = alreadyExistingTextureDataCount;
    }
    _textureDatas.push_back(newTextureData);

    //Now that we merged our data CPU-side, we have to do the same GPU side, for this we use our convinience function
    AppendToClBuffer(clContext,clCommandQueue,&_rgbaDatasBuffer,sizeof(RgbaData),alreadyExistingRgbaValueCount,
        newTextureWidth*newTextureHeight,newRgbaDatas.data());
    AppendToClBuffer(clContext,clCommandQueue,&_textureDatasBuffer,sizeof(TextureData),alreadyExistingTextureDataCount,1,
        &newTextureData);

    return true;
}

bool Scene::TryAddMaterial(MaterialInfo * materialInfo)
{
    MaterialData newMaterialData;
    
    int alreadyExistingMaterialDataCount = _materialDatas.size();
    if(materialInfo != nullptr)
    {
        materialInfo->materialIndex = alreadyExistingMaterialDataCount;
    }

    _materialDatas.push_back(newMaterialData);

    AppendToClBuffer(clContext,clCommandQueue,&_materialDataBuffer,sizeof(MaterialData),alreadyExistingMaterialDataCount,
        1,&newMaterialData);

    return true;
}

bool Scene::GetMaterialData(int materialIndex, MaterialData *materialData)
{
    if(materialIndex >= 0 && materialIndex < _materialDatas.size())
    {
        *materialData = _materialDatas[materialIndex];
        return true;
    }
    return false;
}

bool Scene::TryAlterMaterial(int materialIndex, const MaterialData &alterredMaterialData)
{
    if(materialIndex>=0 && materialIndex < _materialDatas.size())
    {
        _materialDatas[materialIndex] = alterredMaterialData;

        cl_int clError;
        clError = clEnqueueWriteBuffer(clCommandQueue,_materialDataBuffer,CL_TRUE,sizeof(MaterialData)*materialIndex,
            sizeof(MaterialData),&alterredMaterialData,0,nullptr,nullptr);
        CHECK_ERROR(clError);

        ResetPathTracedFrameIndex();
        return true;
    }
    return false;
}

bool Scene::TryAddModel(ModelInfo *modelInfo)
{
    // If there are no meshes or no materials already created fail.
    if(_meshBvhRootIndexData.size() <= 0 || _materialDatas.size() <= 0) return false;

    ModelDataCpu newModelDataCpu;
    newModelDataCpu.meshIndex = 0;
    newModelDataCpu.materialIndex = 0;

    ModelDataGpu newModelDataGpu;
    newModelDataGpu.materialIndex = 0;
    newModelDataGpu.bvhRootIndex = _meshBvhRootIndexData[0];

    int alreadyExistingModelDatas = _modelDatas.size();
    if(modelInfo != nullptr)
    {
        modelInfo->modelIndex = alreadyExistingModelDatas;
    }

    _modelDatas.push_back(newModelDataCpu);

    AppendToClBuffer(clContext,clCommandQueue,&_modelDataBuffer,sizeof(ModelDataGpu),alreadyExistingModelDatas,
        1,&newModelDataGpu);

    return true;
}

bool Scene::GetModelData(int modelIndex, ModelDataCpu *modelData)
{
    if(modelIndex >= 0 && modelIndex < _modelDatas.size())
    {
        *modelData = _modelDatas[modelIndex];
        return true;
    }
    return false;
}

bool Scene::TryAlterModel(int modelIndex, const ModelDataCpu &alteredModelData)
{
    if(modelIndex>=0 && modelIndex < _modelDatas.size())
    {
        ModelDataGpu alteredModelDataGpu;
        alteredModelDataGpu.materialIndex = alteredModelData.materialIndex;
        alteredModelDataGpu.bvhRootIndex = _meshBvhRootIndexData[alteredModelData.meshIndex];

        _modelDatas[modelIndex] = alteredModelData;

        cl_int clError;
        clError = clEnqueueWriteBuffer(clCommandQueue,_modelDataBuffer,CL_TRUE,sizeof(ModelDataGpu)*modelIndex,
            sizeof(ModelDataGpu),&alteredModelDataGpu,0,nullptr,nullptr);
        CHECK_ERROR(clError);
        ResetPathTracedFrameIndex();

        return true;
    }
    return false;
}


void Scene::RecalculateWorldTransformOfObject(int objectIndex)
{
    glm::vec3 position = _objectTransforms[objectIndex].position;
    glm::vec3 scale = _objectTransforms[objectIndex].scale;
    glm::vec3 rotation = _objectTransforms[objectIndex].rotation;

    _objectTransforms[objectIndex].internalRotation = glm::quat(glm::radians(rotation));

    glm::mat4 rotationTransform = glm::mat4_cast( _objectTransforms[objectIndex].internalRotation);

    glm::mat4 worldTransform = glm::translate(glm::mat4(1.0f),position) * rotationTransform * glm::scale(glm::mat4(1.0f),scale);

    _objectDatas[objectIndex].worldTransform = worldTransform;
    _objectDatas[objectIndex].invWorldTransform = glm::inverse(worldTransform);
}



bool Scene::TryAddObject(ObjectInfo *objectInfo)
{
    if(_modelDatas.size() <= 0 || _objectDatas.size() >= _maximumObjectCount) return false;

    Transform newObjectTransform;
    ObjectData newObjectData;
    newObjectData.modelIndex = 0;
    
    int alreadyExistingObjectDataCount = _objectDatas.size();
    _objectTransforms.push_back(newObjectTransform);
    _objectDatas.push_back(newObjectData);

    if(objectInfo != nullptr)
    {
        objectInfo->objectIndex = alreadyExistingObjectDataCount;
    }

    //We calculate the world and invWorld transforms based on transform of objects
    RecalculateWorldTransformOfObject(alreadyExistingObjectDataCount);

    cl_int clError;
    clError = clEnqueueWriteBuffer(clCommandQueue,_objectDataBuffer,CL_TRUE,sizeof(ObjectData)*alreadyExistingObjectDataCount,
        sizeof(ObjectData),&newObjectData,0,nullptr,nullptr);
    CHECK_ERROR(clError);

    //now lets append a worldtransform to the debugobjectworldtrasnform ssbo.
    // and also apppend a a new indirectcommandData to our indirect buffer.
    
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,this->_debugBlasBvhObjectWorldtransformsSsboId);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER,sizeof(glm::mat4)*alreadyExistingObjectDataCount,sizeof(glm::mat4),&newObjectData.worldTransform);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER,this->_debugBlasBvhIndirectCommandsIndirectBufferObjectId);
    int meshIndexOfObject = this->_modelDatas[newObjectData.modelIndex].meshIndex;
    IndirectCommandData newIndirectObjectDrawCommandData = 
    { //Fill this bad boi up
        24, // The number of indices/elements we wanna draw per instance.
        (this->_debugBlasBvhMeshRanges[meshIndexOfObject].nodeCount),
        0,
        0,
        (this->_debugBlasBvhMeshRanges[meshIndexOfObject].startIndex)
    };

    _debugBlasBvhIndirectCommandDatas.push_back(newIndirectObjectDrawCommandData);

    glBufferSubData(GL_DRAW_INDIRECT_BUFFER,sizeof(IndirectCommandData) * alreadyExistingObjectDataCount,sizeof(IndirectCommandData),
        &newIndirectObjectDrawCommandData);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER,0);

    ResetPathTracedFrameIndex();

    this->ReconstructTlasBvh();

    return true;
}

bool Scene::GetObjectState(int objectIndex, ObjectState *objectState)
{
    if(objectIndex >= 0 && objectIndex < _objectTransforms.size())
    {
        objectState->modelIndex = _objectDatas[objectIndex].modelIndex;
        objectState->transform = _objectTransforms[objectIndex];
        return true;
    }
    return false;
}

bool Scene::TryAlterObject(int objectIndex, const ObjectState& alteredObjectState)
{
    if(objectIndex >= 0 && objectIndex < _objectTransforms.size())
    {
        _objectTransforms[objectIndex] = alteredObjectState.transform;
        _objectDatas[objectIndex].modelIndex = alteredObjectState.modelIndex;
        RecalculateWorldTransformOfObject(objectIndex);

        //Upload changes to GPU
        cl_int clError;
        clError = clEnqueueWriteBuffer(clCommandQueue,_objectDataBuffer,CL_TRUE,sizeof(ObjectData)*objectIndex,
            sizeof(ObjectData),&_objectDatas[objectIndex],0,nullptr,nullptr);
        CHECK_ERROR(clError);

        // We have to write the changes into both object transforms debug buffer ssbo
        // and we also have to potentially change the indirect draw command of the object.
        int meshIndex = this->_modelDatas[this->_objectDatas[objectIndex].modelIndex].meshIndex;
        IndirectCommandData alteredObjectIndirectDrawCommandData=
        {
            24,
            this->_debugBlasBvhMeshRanges[meshIndex].nodeCount,
            0,
            0,
            this->_debugBlasBvhMeshRanges[meshIndex].startIndex
        };

        glBindBuffer(GL_DRAW_INDIRECT_BUFFER,this->_debugBlasBvhIndirectCommandsIndirectBufferObjectId);
        glBufferSubData(GL_DRAW_INDIRECT_BUFFER,sizeof(IndirectCommandData)*objectIndex,
            sizeof(IndirectCommandData),&alteredObjectIndirectDrawCommandData); 
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER,0);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER,this->_debugBlasBvhObjectWorldtransformsSsboId);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER,sizeof(glm::mat4)*objectIndex,sizeof(glm::mat4),&this->_objectDatas[objectIndex].worldTransform);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);

        this->_debugBlasBvhIndirectCommandDatas[objectIndex] = alteredObjectIndirectDrawCommandData;

        ResetPathTracedFrameIndex();

        this->ReconstructTlasBvh();

        return true;
    }
    return false;
}

bool Scene::TryDeleteObject(int objectIndex)
{
    if(objectIndex >= 0 && _objectDatas.size() > objectIndex)
    {
        int backIndex = _objectDatas.size() - 1;
        if(objectIndex != backIndex)
        {
            //We have to do a copying
            _objectDatas[objectIndex] = _objectDatas[backIndex];
            _objectTransforms[objectIndex] = _objectTransforms[backIndex];

            cl_int clError;
            clError = clEnqueueWriteBuffer(clCommandQueue,_objectDataBuffer,CL_TRUE,sizeof(ObjectData)*objectIndex,
                sizeof(ObjectData),&_objectDatas[backIndex],0,nullptr,nullptr);
            CHECK_ERROR(clError);


            //Opengl buffer state sortout
            glBindBuffer(GL_DRAW_INDIRECT_BUFFER,this->_debugBlasBvhIndirectCommandsIndirectBufferObjectId);
            glBufferSubData(GL_DRAW_INDIRECT_BUFFER,sizeof(IndirectCommandData)*objectIndex,
                sizeof(IndirectCommandData),&this->_debugBlasBvhIndirectCommandDatas[backIndex]); 
            glBindBuffer(GL_DRAW_INDIRECT_BUFFER,0);

            glBindBuffer(GL_SHADER_STORAGE_BUFFER,this->_debugBlasBvhObjectWorldtransformsSsboId);
            glBufferSubData(GL_SHADER_STORAGE_BUFFER,sizeof(glm::mat4)*objectIndex,sizeof(glm::mat4),
                &this->_objectDatas[backIndex].worldTransform);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);

            this->_debugBlasBvhIndirectCommandDatas[objectIndex] = this->_debugBlasBvhIndirectCommandDatas[backIndex];
        }

        //Deletion is simply just forgeting about a given index.
        _objectDatas.pop_back();
        _objectTransforms.pop_back();

        this->_debugBlasBvhIndirectCommandDatas.pop_back();

        ResetPathTracedFrameIndex();
        ChooseObject(-1);

        this->ReconstructTlasBvh();
        
        return true;
    }

    return false;
}

void Scene::ChooseObject(int objectIndex)
{
    if(objectIndex >= 0 && _objectDatas.size() > objectIndex)
    {
        // Set the gizmo and state to a specific object   
        _isGizmoVisible = true; // set the gizmo to be visible
        _objectWithGizmoIndex = objectIndex;
    }
    else
    {
        // Disable the gizmo, since nothing was chosen.
        // But we keep the gizmo type
        _isGizmoVisible = false;
        _objectWithGizmoIndex = -1;
    }
}

float Scene::IntersectPlane(const Ray &ray, const glm::vec3 &planePoint, const glm::vec3 &planeNormal)
{
    float retval = ray.tMax;

    float dirDotNormal=dot(ray.direction, planeNormal);
    if(fabsf(dirDotNormal) < 0.00001f)
    {
        return retval;
    }

    retval = dot(planePoint - ray.origin,planeNormal) / dirDotNormal;
    return retval;
}

float Scene::IntersectTriangle(const Ray &ray, const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2)
{
    float retval;
	retval = ray.tMax + 0.000001f;

	glm::vec3 v01 = p1 - p0;
	glm::vec3 v02 = p2 - p0;

	//Firstly we calculate our main denominator determinant
	glm::vec3 dirXv02 = glm::cross(ray.direction, v02);
	float baseDeterminant = glm::dot(v01, dirXv02);

	//Early exit if this determinant is pretty small -> means v01, v02, D are linearly codependent
	//This mainly happens when D is paralell to the triangle's plane
	if (fabs(baseDeterminant) < 1e-8f) return retval;
	float invBaseDeterminant = 1.0f / baseDeterminant;

	glm::vec3 b = ray.origin - p0;
	float u = glm::dot(b, dirXv02) * invBaseDeterminant;
	if (u < 0.0f || u>1.0f) return retval;

	//This is important, we have this exact order here because we got rid of the minus sign in front of t
	glm::vec3  bXv01= glm::cross(b, v01);
	float v = glm::dot(ray.direction,bXv01) * invBaseDeterminant;
	if (v < 0.0f || u + v > 1.0f) return retval;

	float t = glm::dot(v02,bXv01)*invBaseDeterminant;

	retval = t;	

	return retval;
}

float Scene::IntersectBox(const Ray &ray, const AABB4 &box)
{
    float retval;
    retval = FLT_MAX;

    float tFar = FLT_MAX;
    float tNear = -FLT_MAX;

    for(int axis = 0; axis < 3; ++axis)
    {
        float tLowOnAxis;
        float tHighOnAxis;
        tLowOnAxis = (box.min[axis] - ray.origin[axis])*ray.invDirection[axis];
        tHighOnAxis = (box.max[axis] - ray.origin[axis])*ray.invDirection[axis];

        float tNearOnAxis = fmin(tLowOnAxis,tHighOnAxis);
        float tFarOnAxis = fmax(tLowOnAxis,tHighOnAxis);

        tNear = fmax(tNear,tNearOnAxis);
        tFar = fmin(tFar,tFarOnAxis);

        if(tNear > tFar)
        {
            return retval;
        }
    }

    if(tFar < 0)
    {
        return retval; //nohit
    }
    else
    {
        retval = fmax(tNear,0.0f);
        return retval;
    }
}

float Scene::IntersectBvhNodeRecursive(Ray &ray, 
    const std::vector<VertexPositionData> &vertexPositions, 
    const std::vector<TriangleIndicesData> &triangleVertexIndices, 
    const std::vector<BvhNodeData> &bvhNodeDatas, const int meshBvhRootIndex)
{
    float retval;
    retval = ray.tMax;
    
    BvhNodeData bvhNode = bvhNodeDatas[meshBvhRootIndex];
    float boxResult = IntersectBox(ray, bvhNode.box);

    if (boxResult > ray.tMin && boxResult < ray.tMax)
    {
        //We hit the box
        if (bvhNode.minChild == -1 && bvhNode.maxChild == -1)
        {
            //If the node is a child
            for (int i = bvhNode.startIndex;i < bvhNode.endIndex;++i)
            {
                glm::ivec3 triagIndices = glm::ivec3(triangleVertexIndices[i].x, triangleVertexIndices[i].y, triangleVertexIndices[i].z);

                glm::vec3 triagP0 = glm::vec3(vertexPositions[triagIndices.x].x,
                    vertexPositions[triagIndices.x].y,
                    vertexPositions[triagIndices.x].z);

                glm::vec3 triagP1 = glm::vec3(vertexPositions[triagIndices.y].x,
                    vertexPositions[triagIndices.y].y,
                    vertexPositions[triagIndices.y].z);

                glm::vec3 triagP2 = glm::vec3(vertexPositions[triagIndices.z].x,
                    vertexPositions[triagIndices.z].y,
                    vertexPositions[triagIndices.z].z);

                float triagResult = IntersectTriangle(ray, triagP0, triagP1, triagP2);
                if (triagResult > ray.tMin && triagResult < ray.tMax && retval > triagResult)
                {
                    retval = triagResult;
                    ray.tMax = triagResult;
                }
            }
        }
        else
        {
            //if the node is an inner node
            //we go further down the recursion
            float minChildBoxResult = IntersectBox(ray, bvhNodeDatas[bvhNode.minChild].box);
            float maxChildBoxResult = IntersectBox(ray, bvhNodeDatas[bvhNode.maxChild].box);

            float minResult = ray.tMax;
            float maxResult = ray.tMax;
            if(minChildBoxResult < ray.tMax && maxChildBoxResult < ray.tMax)
            {
                if(minChildBoxResult < maxChildBoxResult)
                {
                    minResult = IntersectBvhNodeRecursive(ray,vertexPositions,triangleVertexIndices,bvhNodeDatas,bvhNode.minChild);
                    maxResult = IntersectBvhNodeRecursive(ray,vertexPositions,triangleVertexIndices,bvhNodeDatas,bvhNode.maxChild);
                }
                else
                {
                    maxResult = IntersectBvhNodeRecursive(ray,vertexPositions,triangleVertexIndices,bvhNodeDatas,bvhNode.maxChild);
                    minResult = IntersectBvhNodeRecursive(ray,vertexPositions,triangleVertexIndices,bvhNodeDatas,bvhNode.minChild);
                }
                
            }
            else if(minChildBoxResult < ray.tMax)
            {
                minResult = IntersectBvhNodeRecursive(ray,vertexPositions,triangleVertexIndices,bvhNodeDatas,bvhNode.minChild);
            }
            else if(maxChildBoxResult < ray.tMax)
            {
                maxResult = IntersectBvhNodeRecursive(ray,vertexPositions,triangleVertexIndices,bvhNodeDatas,bvhNode.maxChild);
            }

            retval = fmin(minResult,maxResult);
        }
    }

    return retval;
}

float Scene::IntersectObject(const Ray &ray, 
    const std::vector<VertexPositionData> &vertexPositions, 
    const std::vector<TriangleIndicesData> &triangleVertexIndices, 
    const std::vector<BvhNodeData> &bvhNodeDatas, const int meshBvhRootIndex, 
    const glm::mat4 &inverseWorldTransform)
{
    Ray transformedRay;
    transformedRay.tMin = ray.tMin;
    transformedRay.tMax = ray.tMax;
    transformedRay.origin = glm::vec3(inverseWorldTransform * glm::vec4(ray.origin, 1.0f));
    transformedRay.direction = glm::vec3(inverseWorldTransform * glm::vec4(ray.direction, 0.0f));
    transformedRay.invDirection = 1.0f / transformedRay.direction;

    float retval = ray.tMax;

    float meshResult = IntersectBvhNodeRecursive(transformedRay, vertexPositions,triangleVertexIndices,bvhNodeDatas,meshBvhRootIndex);
    if (meshResult > ray.tMin && meshResult < ray.tMax)
    {
        retval = meshResult;
    }

    return retval;
}

glm::vec3 Scene::CalculateRayDirection(int x, int y)
{
    //calculating with ndc-like coordiantes is really easy, currently our origo is in the left upper corner, 
    //   so we have to get it to the left bottom corner.
    glm::vec2 pixelCoordInLeftBottomSystem = glm::vec2(x,_viewportHeight - y);

    float halfWorldViewPortWidth =  tan(glm::radians(_camera.GetFovx()) * 0.5f );
    float halfWorldViewPortHeight = halfWorldViewPortWidth / _camera.GetAspect();

    glm::vec2 pixelMidCoordsNdc;
    pixelMidCoordsNdc.x = (((float)pixelCoordInLeftBottomSystem.x + 0.5f) / _viewportWidth) * 2.0f - 1.0f;
    pixelMidCoordsNdc.y = (((float)pixelCoordInLeftBottomSystem.y + 0.5f) / _viewportHeight) * 2.0f - 1.0f;

    glm::vec3 rayDirection = _camera.GetFront() - 
        pixelMidCoordsNdc.x * halfWorldViewPortWidth * _camera.GetRight() +
        pixelMidCoordsNdc.y * halfWorldViewPortHeight * _camera.GetUp();

    return glm::normalize(rayDirection);
}

void Scene::PickScene(int x, int y, PickResult *pickResult)
{
    Ray pickRay;
    pickRay.origin = _camera.GetPosition();
    pickRay.direction = CalculateRayDirection(x,y);
    pickRay.tMin = _camera.GetZNear();
    pickRay.tMax = _camera.GetZFar();
    pickRay.invDirection = 1.0f / pickRay.direction;

    float retval = pickRay.tMax;
    int pickedObjectIndex = -1;
    for(int i=0;i<_objectDatas.size();++i)
    {
        float currentObjectResult = IntersectObject(pickRay,_vertexPositionData,_triangleIndicesData,_bottomLevelBvhNodeDatas,
            _meshBvhRootIndexData[_modelDatas[_objectDatas[i].modelIndex].meshIndex],_objectDatas[i].invWorldTransform);
        if(currentObjectResult > pickRay.tMin && currentObjectResult < pickRay.tMax && currentObjectResult < retval)
        {
            retval = currentObjectResult;
            pickedObjectIndex = i;
        }
    }

    if(pickedObjectIndex >= 0)
    {
        pickResult->type = PickResultType::OBJECT;
        pickResult->pickedIndex = pickedObjectIndex;
    }
}

void Scene::PickCurrentGizmo(int x, int y, PickResult *pickResult)
{
    if(!_isGizmoVisible || _objectWithGizmoIndex < 0 || _objectWithGizmoIndex >= _objectDatas.size()) return;

    if(!(x >= 0 && x < _viewportWidth && y >= 0 && y < _viewportHeight)) return;

    Ray pickRay;
    pickRay.origin = _camera.GetPosition();
    pickRay.direction = CalculateRayDirection(x,y);
    pickRay.tMin = 0;
    pickRay.tMax = _camera.GetZFar();
    pickRay.invDirection = 1.0f / pickRay.direction;

    float bestResult = pickRay.tMax;
    int pickedAxisIndex = -1;

    int objectIndex = _objectWithGizmoIndex;
    int gizmoMeshBvhRootIndex = _gizmoMeshBvhRoots[(int)_currentGizmoType];
    
    glm::mat4 gizmoTypeTransform(1.0f);
    for(int axis = 0;axis<3;++axis)
    {
        if(_currentGizmoType == GizmoType::GIZMO_TRANSLATION)
        {
            gizmoTypeTransform = _arrowGizmoModelTransforms[axis];
        }
        else if(_currentGizmoType == GizmoType::GIZMO_SCALE)
        {
            gizmoTypeTransform = _cubeGizmoModelTransforms[axis];
        }
        else
        {
            gizmoTypeTransform =  glm::mat4_cast(_objectTransforms[objectIndex].internalRotation) * _ringGizmoModelTransforms[axis];
            //std::cout<<"Picking ring\n";
        }

        
        float objectDistanceFromCamera = glm::length( _camera.GetPosition() -  _objectTransforms[objectIndex].position);

        glm::mat4 finalWorldTransform = glm::translate(glm::mat4(1.0f),_objectTransforms[objectIndex].position) *
            glm::scale(glm::mat4(1.0f),glm::vec3(objectDistanceFromCamera * sizeK)) *
            gizmoTypeTransform;

        glm::mat4 inverseFinalWorldTransform = glm::inverse(finalWorldTransform);

        float rayIntersectAxisResult = IntersectObject(pickRay,_gizmoVertexPositionDatas,_gizmoTriangleVertexIndices,_gizmoBvhNodeDatas,
            gizmoMeshBvhRootIndex,inverseFinalWorldTransform);
        
        if(rayIntersectAxisResult > pickRay.tMin && rayIntersectAxisResult < pickRay.tMax && rayIntersectAxisResult < bestResult)
        {
            bestResult = rayIntersectAxisResult;
            pickedAxisIndex = axis;
        }
    }
    

    if(pickedAxisIndex >= 0)
    {
        pickResult->type = PickResultType::GIZMO;
        pickResult->pickedIndex = pickedAxisIndex;
        //std::cout<<"Chosen GizmoAxis: " << pickedAxisIndex << "\n";
    }
}

void Scene::EnterGizmoInteractionMode()
{
    // Only if we arent already interacting with a gizmo(technically impossible)
    if(_isCurrentGizmoInteractedWith) return;

    if(this->_currentMousePos.x < 0 || this->_currentMousePos.y < 0 || 
        this->_currentMousePos.x > this->_viewportWidth || this->_currentMousePos.y > this->_viewportHeight)
    {
        // if, when we want to enter interaction mode the cursor is somewhere in Narnia
        return;
    }

    // can only enter gizmo mode if the current gizmo is highlighted and shit
    // if the gizmo isnt visible why bother turning this on?
    if(!this->_isGizmoVisible || this->_currentlyHighlightedGizmoAxis < 0 || this->_objectWithGizmoIndex < 0) return;

    // Now we can be sure that we are doing the right thing, so lets calculate the state the interaction process will work with.
    this->_gizmoInteractionState.startObjectWorldTransform = this->_objectTransforms[this->_objectWithGizmoIndex];
    this->_gizmoInteractionState.interactionPlanePoint = this->_objectTransforms[this->_objectWithGizmoIndex].position;
    
    // For helpful thinking lets declare the current gizmo interaction axis we are working on.
    //std::cout<<"Axis: " << _currentlyHighlightedGizmoAxis << "\n";
    glm::vec3 interactionAxisLocal = glm::vec3(0);
    interactionAxisLocal[this->_currentlyHighlightedGizmoAxis] = 1;

    Ray startInteractionRay;
    startInteractionRay.tMin = 0.0f;
    startInteractionRay.tMax = _camera.GetZFar();
    startInteractionRay.origin = _camera.GetPosition();
    startInteractionRay.direction = CalculateRayDirection(this->_currentMousePos.x,this->_currentMousePos.y);

    // we calculate the normal of the interaction plane differently
    if(_currentGizmoType == GizmoType::GIZMO_ROTATION)
    {
        this->_gizmoInteractionState.interactionPlaneNormal = glm::vec3(
            glm::mat4_cast(this->_objectTransforms[this->_objectWithGizmoIndex].internalRotation) * 
            glm::vec4(interactionAxisLocal,0)
        );

        
    }
    else // 
    {
        glm::vec3 translateScalePlaneNormal = glm::normalize(
            glm::cross(interactionAxisLocal,glm::cross(startInteractionRay.direction,interactionAxisLocal))
        );

        this->_gizmoInteractionState.interactionPlaneNormal = translateScalePlaneNormal;
    }

    if(fabsf(glm::dot(startInteractionRay.direction,_gizmoInteractionState.interactionPlaneNormal))< 0.00001f) return;


    // now, we toggle the interaction for real, these previous check were fundamental, trust me.
    _isCurrentGizmoInteractedWith = true;

    // Here we just calculate basic ray-plane intersection point.
    float traceResult = IntersectPlane(startInteractionRay,
        this->_gizmoInteractionState.interactionPlanePoint,this->_gizmoInteractionState.interactionPlaneNormal);
    glm::vec3 dirtyStartIntersectionPoint = startInteractionRay.origin + traceResult * startInteractionRay.direction;

    // Lets calculate the REAL gizmo start interaction position
    if(_currentGizmoType == GizmoType::GIZMO_ROTATION)
    {
        this->_gizmoInteractionState.startInteractionPoint = 
            glm::normalize(dirtyStartIntersectionPoint - _gizmoInteractionState.interactionPlanePoint);
    }
    else
    {
        //Project plane intersection point onto axis;[TODO] - dont wanna now :P
        glm::vec3 objectMid = this->_objectTransforms[this->_objectWithGizmoIndex].position;
        glm::vec3 objMidToDirtyPoint = dirtyStartIntersectionPoint - objectMid;
        float projectionAmount  = glm::dot(objMidToDirtyPoint,interactionAxisLocal);
        glm::vec3 projectedPointOnAxis = objectMid + projectionAmount * interactionAxisLocal;
        this->_gizmoInteractionState.startInteractionPoint = projectedPointOnAxis;
    }
}

void Scene::CalculateGizmoInteraction(int newX, int newY)
{

    
    if(!_isCurrentGizmoInteractedWith) return;

    if(newX < 0 || newX > _viewportWidth || newY < 0 || newY > _viewportHeight) return;

    Ray currentInteractionRay;
    currentInteractionRay.tMin = 0.0f;
    currentInteractionRay.tMax = _camera.GetZFar();
    currentInteractionRay.origin = _camera.GetPosition();
    currentInteractionRay.direction = CalculateRayDirection(newX,newY);

    float interactionPointDistance = IntersectPlane(currentInteractionRay,
        _gizmoInteractionState.interactionPlanePoint,_gizmoInteractionState.interactionPlaneNormal);
    
    if(interactionPointDistance >= currentInteractionRay.tMax || interactionPointDistance <= currentInteractionRay.tMin) 
    {
        return; // Invalid intersection.
    }

    

    
    glm::vec3 dirtyInteractionPoint = currentInteractionRay.origin + interactionPointDistance * currentInteractionRay.direction;

    ObjectState newobjectState;
    this->GetObjectState(_objectWithGizmoIndex,&newobjectState);

    if(_currentGizmoType == GizmoType::GIZMO_ROTATION)
    {

        


        this->_gizmoInteractionState.currentInteractionPoint =
            glm::normalize(dirtyInteractionPoint - _gizmoInteractionState.interactionPlanePoint);

        float angle = atan2(glm::dot(_gizmoInteractionState.interactionPlaneNormal,
                                glm::cross(_gizmoInteractionState.startInteractionPoint, _gizmoInteractionState.currentInteractionPoint)), 
                            glm::dot(_gizmoInteractionState.startInteractionPoint, _gizmoInteractionState.currentInteractionPoint));

        glm::quat deltaQuaternion = glm::angleAxis(angle,_gizmoInteractionState.interactionPlaneNormal);

        newobjectState.transform.internalRotation = deltaQuaternion * _gizmoInteractionState.startObjectWorldTransform.internalRotation;

        glm::vec3 eulerDegrees = glm::degrees(glm::eulerAngles(newobjectState.transform.internalRotation));

        newobjectState.transform.rotation = eulerDegrees;

        

    }
    else
    {
        //std::cout<<"We run\n";
        glm::vec3 interactionAxisLocal = glm::vec3(0);
        //std::cout<<"Axis: " << _currentlyHighlightedGizmoAxis <<"\n";
        interactionAxisLocal[this->_currentlyHighlightedGizmoAxis] = 1;

        glm::vec3 objMidToDirtyPoint = dirtyInteractionPoint - _gizmoInteractionState.startInteractionPoint;
        float projectionAmount  = glm::dot(objMidToDirtyPoint,interactionAxisLocal);
        glm::vec3 projectedPointOnAxis = _gizmoInteractionState.startInteractionPoint + projectionAmount * interactionAxisLocal;
        this->_gizmoInteractionState.currentInteractionPoint = projectedPointOnAxis;

        
        glm::vec3 deltaPositionVector = _gizmoInteractionState.currentInteractionPoint - _gizmoInteractionState.startInteractionPoint;

        if(_currentGizmoType == GizmoType::GIZMO_TRANSLATION)
        {
            

            newobjectState.transform.position = _gizmoInteractionState.startObjectWorldTransform.position + deltaPositionVector;

            
        }
        else // Scaling
        {
            // Perhaps we should put this is a system variable
            const float scalingSensitivity = 1.0f;

            newobjectState.transform.scale = _gizmoInteractionState.startObjectWorldTransform.scale + deltaPositionVector*scalingSensitivity;
            
        }

        
    }

    this->TryAlterObject(_objectWithGizmoIndex,newobjectState);

}

void Scene::LeaveGizmoInteractionMode()
{
    if(!_isCurrentGizmoInteractedWith) return; // called in invalid state

    //We should also calculate whether or not the mouse cursor is still on top of the gizmo.
    CheckForHighlightedAxis();

    _isCurrentGizmoInteractedWith = false;
}

void Scene::CheckForHighlightedAxis()
{
    _currentlyHighlightedGizmoAxis = -1;

    PickResult currentGizmoPickResult;
    PickCurrentGizmo(_currentMousePos.x,_currentMousePos.y,&currentGizmoPickResult);
    if(currentGizmoPickResult.type == PickResultType::GIZMO && currentGizmoPickResult.pickedIndex >= 0)
    {
        _currentlyHighlightedGizmoAxis = currentGizmoPickResult.pickedIndex;
    }
}

void Scene::HelperBvhNodeBoxExtractorRecursive(std::vector<BvhNodeData> &bvhNodeDatas,int nodeIndex, int depth, 
    std::vector<AABB4> &extractVector)
{
    if(depth > this->_maximumDebugRenderBvhDepth) return;

    if(nodeIndex< 0 ) return;

    BvhNodeData* nodePtr = &bvhNodeDatas[nodeIndex];

    extractVector.push_back(nodePtr->box);

    HelperBvhNodeBoxExtractorRecursive(bvhNodeDatas,nodePtr->minChild,depth + 1,extractVector);
    HelperBvhNodeBoxExtractorRecursive(bvhNodeDatas,nodePtr->maxChild,depth + 1,extractVector);
}

void Scene::AddNewBvhNodeBoxesToDebugSsbo(std::vector<BvhNodeData> &bvhNodeDatas)
{
    if(bvhNodeDatas.size() <= 0) return;

    std::vector<AABB4> tempProcessVector;
    tempProcessVector.reserve(bvhNodeDatas.size());

    HelperBvhNodeBoxExtractorRecursive(bvhNodeDatas,0,0,tempProcessVector);

    //now, temp vec has all the needed ddebug aabb4 informations, push it into our main debug vector, and also copy it up to the gpu.

    //lets make a new buffer on gpu, and copy our existing buffer into it + our new buffer
    unsigned int tempSsboBufferId = 0;
    glGenBuffers(1,&tempSsboBufferId);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,tempSsboBufferId);
    glBufferData(GL_SHADER_STORAGE_BUFFER,sizeof(AABB4)*(_debugBlasBvhBoxes.size() + tempProcessVector.size()),nullptr,GL_STATIC_DRAW);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER,sizeof(AABB4)*this->_debugBlasBvhBoxes.size(),sizeof(AABB4)*tempProcessVector.size(),
        tempProcessVector.data());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);

    if(_debugBlasBvhBoxes.size() > 0)
    {
        glBindBuffer(GL_COPY_WRITE_BUFFER,tempSsboBufferId);
        glBindBuffer(GL_COPY_READ_BUFFER,this->_debugBlasBvhBoxesSsboId);
        glCopyBufferSubData(GL_COPY_READ_BUFFER,GL_COPY_WRITE_BUFFER,0,0,sizeof(AABB4)*this->_debugBlasBvhBoxes.size());
        glBindBuffer(GL_COPY_WRITE_BUFFER,0);
        glBindBuffer(GL_COPY_READ_BUFFER,0);    
    }
    
    glDeleteBuffers(1,&this->_debugBlasBvhBoxesSsboId);
    this->_debugBlasBvhBoxesSsboId = tempSsboBufferId;

    BvhRangeData newBvhBoxRangeData = 
    {
        this->_debugBlasBvhBoxes.size(),
        tempProcessVector.size()
    };
    this->_debugBlasBvhMeshRanges.push_back(newBvhBoxRangeData);

    this->_debugBlasBvhBoxes.insert(this->_debugBlasBvhBoxes.end(),tempProcessVector.begin(),tempProcessVector.end());
}

static float GetTlasSahSplitCost(const AABB3& minBox, int minCount, const AABB3& maxBox, int maxCount)
{
    glm::vec3 minBoxSize = minBox.max - minBox.min;
    glm::vec3 maxBoxSize = maxBox.max - maxBox.min;

    float minBoxArea = 2.0f* (minBoxSize.x * minBoxSize.y + minBoxSize.x * minBoxSize.z + minBoxSize.y * minBoxSize.z);
	float maxBoxArea = 2.0f* (maxBoxSize.x * maxBoxSize.y + maxBoxSize.x * maxBoxSize.z + maxBoxSize.y * maxBoxSize.z);

	return minBoxArea  * minCount + maxBoxArea * maxCount;
}

BestSplitResult Scene::FindBestSahSplitOfInterval(const TlasBvhNode& processedTlasNode, int intervalStart, int intervalEnd)
{
    //pre assumptions: atleast 2 tempoarary nodes exist on interval, 
    // we use SAH for best splitting search, firstly we go through all axes, determining the minside extreme centroid, 
    // and the maxside extreme centroid, after that we use bins to between these two centroid, first bin is on start centroid, 
    // last is on the maxside end extreme centroid.

    AABB3 tlasNodeAABB = {
        glm::vec3(processedTlasNode.box.min),
        glm::vec3(processedTlasNode.box.max)
    };

    float bestSahCost = FLT_MAX;
    BestSplitResult retvalResult;
    retvalResult.axis = -1;
    for(int axis = 0; axis < 3 ;++axis)
    {
        //max + min search for extreme centroids on the interval
        float minCentroidValueOnAxis = FLT_MAX;
        float maxCentroidValueOnAxis = -FLT_MAX;
        for(int i = intervalStart ; i<intervalEnd;++i)
        {
            if(_temporaryObjectBlasInstances[i].objectCentroid[axis] < minCentroidValueOnAxis)
            {
                minCentroidValueOnAxis = _temporaryObjectBlasInstances[i].objectCentroid[axis];
            }

            if(_temporaryObjectBlasInstances[i].objectCentroid[axis] > maxCentroidValueOnAxis)
            {
                maxCentroidValueOnAxis = _temporaryObjectBlasInstances[i].objectCentroid[axis];
            }
        }

        //okay, now the two values calculated above hold our end of the binning interval, lets start binning!
        float lengthOfTheBinningInterval = (maxCentroidValueOnAxis - minCentroidValueOnAxis);
        float deltaPerBinOnAxis = lengthOfTheBinningInterval / (_tlasSahBinCount -1); // -1, because we want the 
                                                                                      //last bin to be the last centroid of the 
                                                                                      //interval on axis
        for(int bin=0;bin<_tlasSahBinCount;++bin)
        {
            float currentBinValueOnAxis = minCentroidValueOnAxis + bin*deltaPerBinOnAxis;
            AABB3 minSideBox = {glm::vec3(FLT_MAX,FLT_MAX,FLT_MAX),glm::vec3(-FLT_MAX,-FLT_MAX,-FLT_MAX)};
            int minSideCount = 0;
            AABB3 maxSideBox = {glm::vec3(FLT_MAX,FLT_MAX,FLT_MAX),glm::vec3(-FLT_MAX,-FLT_MAX,-FLT_MAX)};
            int maxSideCount = 0;
            for(int i=intervalStart; i<intervalEnd;++i)
            {
                ObjectBlasInstance* tempInstancePtr = &_temporaryObjectBlasInstances[i];
                if(tempInstancePtr->objectCentroid[axis] < currentBinValueOnAxis)
                {
                    ++minSideCount;
                    FeedAABB3ToAABB3(minSideBox,tempInstancePtr->worldBoundsBox);
                }
                else
                {
                    ++maxSideCount;
                    FeedAABB3ToAABB3(maxSideBox,tempInstancePtr->worldBoundsBox);
                }
            }

            // Just to filter our degenerate cases.
            if (minSideCount == 0 || maxSideCount == 0) continue;

            // there should be atleast one blas instance on either side.
            float determinedSahCostOfSplit = GetTlasSahSplitCost(minSideBox,minSideCount,maxSideBox,maxSideCount);

            if(determinedSahCostOfSplit < bestSahCost)
            {
                bestSahCost = determinedSahCostOfSplit;
                retvalResult.axis = axis;
                retvalResult.valueOnAxis = currentBinValueOnAxis;
            }
        } 
    }

    return retvalResult;
}

PartitionResult Scene::PartitionTemporaryInstanceInterval(int partitionAxis, float partitionValueOnAxis, int intervalStart, int intervalEnd)
{
    PartitionResult retval;
    retval.partitionIndex = -1;

    if(intervalEnd - intervalStart < 2) return retval;

    AABB3 minSideBox = {glm::vec3(FLT_MAX,FLT_MAX,FLT_MAX),glm::vec3(-FLT_MAX,-FLT_MAX,-FLT_MAX)};
    AABB3 maxSideBox = {glm::vec3(FLT_MAX,FLT_MAX,FLT_MAX),glm::vec3(-FLT_MAX,-FLT_MAX,-FLT_MAX)};

    int frontIndex = intervalStart;
    int endIndex = intervalEnd -1;
    while(frontIndex <= endIndex)
    {
        ObjectBlasInstance* tempInstancePtr = &this->_temporaryObjectBlasInstances[frontIndex];
        if(tempInstancePtr->objectCentroid[partitionAxis] >= partitionValueOnAxis)
        {
            // you are at the wrong side(should be at the max side, but instead is at the min side)
            // lets put this temp instance to the back of the interval, and lets put the value existing there to the front, and decrease
            // backindex, because the value we put there is valid.
            ObjectBlasInstance endTempInstanceTempCopy = this->_temporaryObjectBlasInstances[endIndex];
            this->_temporaryObjectBlasInstances[endIndex] = this->_temporaryObjectBlasInstances[frontIndex];
            this->_temporaryObjectBlasInstances[frontIndex] = endTempInstanceTempCopy;
            --endIndex;
        }
        else
        {
            //this means the front is at the right place, we just have to increment frontIndex
            ++frontIndex;
        }
    }

    retval.partitionIndex = frontIndex;

    for(int i=intervalStart;i<intervalEnd;++i)
    {
        if(i < frontIndex)
        {
            //minside
            FeedAABB3ToAABB3(minSideBox,_temporaryObjectBlasInstances[i].worldBoundsBox);
        }
        else
        {
            //maxside;
            FeedAABB3ToAABB3(maxSideBox,_temporaryObjectBlasInstances[i].worldBoundsBox);
        }
    }

    retval.minSideAABB = minSideBox;
    retval.maxSideAABB = maxSideBox;

    return retval; //i hope this is good(seems good) //frontindex should always point to the last last min side object instance + 1
}

PartitionResult Scene::FallbackPartitionInterval(int intervalStart, int intervalEnd)
{
    PartitionResult retvalResult;
    int partitionIndex = intervalStart + (intervalEnd - intervalStart) / 2;
    retvalResult.partitionIndex = partitionIndex;

    AABB3 minSideBox = {glm::vec3(FLT_MAX,FLT_MAX,FLT_MAX),glm::vec3(-FLT_MAX,-FLT_MAX,-FLT_MAX)};
    AABB3 maxSideBox = {glm::vec3(FLT_MAX,FLT_MAX,FLT_MAX),glm::vec3(-FLT_MAX,-FLT_MAX,-FLT_MAX)};

    for(int i=intervalStart;i<intervalEnd;++i)
    {
        if(i < partitionIndex)
        {
            //minside
            FeedAABB3ToAABB3(minSideBox,_temporaryObjectBlasInstances[i].worldBoundsBox);
        }
        else
        {
            //maxside;
            FeedAABB3ToAABB3(maxSideBox,_temporaryObjectBlasInstances[i].worldBoundsBox);
        }
    }

    retvalResult.minSideAABB = minSideBox;
    retvalResult.maxSideAABB = maxSideBox;

    return retvalResult;
}

void Scene::TrySplitTlasNodeRecursive(int tlasBvhNodeIndex, int intervalStart, int intervalEnd)
{
    TlasBvhNode currentNode = this->_tlasBvhNodes[tlasBvhNodeIndex];
    if((intervalEnd - intervalStart) > 1)
    {
        //internal Node
        // we know the interval, 
        BestSplitResult bestWayToSplitCurrentNode = FindBestSahSplitOfInterval(currentNode,intervalStart,intervalEnd);

        int splittingAxis = bestWayToSplitCurrentNode.axis;
        float splittingValueOnAxis = bestWayToSplitCurrentNode.valueOnAxis;

        PartitionResult partitionOfTlasNodeResult;
        //It can happen, that sometimes sah cant determine an optimal split, in the case of a lot of objects are in the same place, and the binning
        // interval is sooo tiny tiny, that producing a good result is not possible, in this degenerate case we will just simply 
        // split the interval in half, cuz we HAVE to make some kind of split.
        // the degenerate case mostly happens because of 0 length of axis (all axis) -> the centroid points overlap
        if(bestWayToSplitCurrentNode.axis >= 0) // good case
        {
            partitionOfTlasNodeResult = PartitionTemporaryInstanceInterval(bestWayToSplitCurrentNode.axis,
                bestWayToSplitCurrentNode.valueOnAxis,intervalStart,intervalEnd);
        }
        else // bad case
        {
            partitionOfTlasNodeResult = FallbackPartitionInterval(intervalStart,intervalEnd);
        }

        

        //now we partitioned the given interval, lets create two children nodes for the two new intervals based on partition, and split those
        // recursively aswell.
        TlasBvhNode minSideChild;
        minSideChild.box = {glm::vec4(partitionOfTlasNodeResult.minSideAABB.min,0),glm::vec4(partitionOfTlasNodeResult.minSideAABB.max,0)};

        TlasBvhNode maxSideChild;
        maxSideChild.box = {glm::vec4(partitionOfTlasNodeResult.maxSideAABB.min,0),glm::vec4(partitionOfTlasNodeResult.maxSideAABB.max,0)};

        int minSideChildIndexInTlasNodes = this->_tlasBvhNodes.size();
        this->_tlasBvhNodes.push_back(minSideChild);

        int maxSideChildIndexInTlasNodes = this->_tlasBvhNodes.size();
        this->_tlasBvhNodes.push_back(maxSideChild);

        // we also have to set where the children of the currently split(parent) node are located at(its box is already calculated.)
        this->_tlasBvhNodes[tlasBvhNodeIndex].minChildIndex = minSideChildIndexInTlasNodes;
        this->_tlasBvhNodes[tlasBvhNodeIndex].maxChildIndex = maxSideChildIndexInTlasNodes;


        //std::cout<<"Interval Start: " << intervalStart << " partition: " << partitionOfTlasNodeResult.partitionIndex << " end: "<< intervalEnd << "\n";
        // Now, everything has been set for the parent node, and the box of the children has been also set(through the partition func)
        // here, we still have to recursively split down these newly created nodes aswell.
        TrySplitTlasNodeRecursive(minSideChildIndexInTlasNodes,intervalStart,partitionOfTlasNodeResult.partitionIndex);
        TrySplitTlasNodeRecursive(maxSideChildIndexInTlasNodes,partitionOfTlasNodeResult.partitionIndex,intervalEnd);
        // and we are finished.
    }
    else
    {
        //Leaf node
        int objectIndex = _temporaryObjectBlasInstances[intervalStart].objectIndex;
        _tlasBvhNodes[tlasBvhNodeIndex].maxChildIndex = objectIndex;
        _tlasBvhNodes[tlasBvhNodeIndex].minChildIndex = -1; // the leaf "signal"
    }
}

void Scene::ReconstructTlasBvh()
{

    this->_tlasBvhNodes.clear();

    this->_temporaryObjectBlasInstances.clear();

    if(this->_objectDatas.size() <= 0) return; // no object, no need for any of this shish

    
    TlasBvhNode rootTlasNode;

    AABB3 aabbOfTlasBvhNodeRoot;
    aabbOfTlasBvhNodeRoot.min = glm::vec3(FLT_MAX,FLT_MAX,FLT_MAX);
    aabbOfTlasBvhNodeRoot.max = glm::vec3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
    //First, we have to calculate and load our temporary objectblasinstances.
    for(int i=0;i<_objectDatas.size();++i)
    {
        ObjectBlasInstance currentTemporaryInstance;
        currentTemporaryInstance.objectIndex = i;

        int blasBvhRootIndexOfCurrentObject = _meshBvhRootIndexData[_modelDatas[_objectDatas[i].modelIndex].meshIndex];
        AABB3 localBlasRootBox;
        localBlasRootBox.min = glm::vec3(_bottomLevelBvhNodeDatas[blasBvhRootIndexOfCurrentObject].box.min);
        localBlasRootBox.max = glm::vec3(_bottomLevelBvhNodeDatas[blasBvhRootIndexOfCurrentObject].box.max);
        currentTemporaryInstance.worldBoundsBox = GetWorldBoundsOfTransformedAABB(_objectDatas[i].worldTransform,localBlasRootBox);

        currentTemporaryInstance.objectCentroid = 
            (currentTemporaryInstance.worldBoundsBox.max + currentTemporaryInstance.worldBoundsBox.min) * 0.5f;

        FeedAABB3ToAABB3(aabbOfTlasBvhNodeRoot,currentTemporaryInstance.worldBoundsBox);

        _temporaryObjectBlasInstances.push_back(currentTemporaryInstance);
    }

    rootTlasNode.box = {glm::vec4(aabbOfTlasBvhNodeRoot.min,0),glm::vec4(aabbOfTlasBvhNodeRoot.max,0)};

    this->_tlasBvhNodes.push_back(rootTlasNode);
    TrySplitTlasNodeRecursive(0,0,this->_temporaryObjectBlasInstances.size());

    //the function called above has calculated all bvh nodes, we now have to upload this data both to opencl and opengl buffers for rendering
    // lets upload data to GPU 
    cl_int clError;
    clError = clEnqueueWriteBuffer(clCommandQueue,this->_tlasBvhNodesBuffer,CL_TRUE,0,sizeof(TlasBvhNode) * this->_tlasBvhNodes.size(),
        this->_tlasBvhNodes.data(),0,nullptr,nullptr);
    CHECK_ERROR(clError);

    //now to opengl SSBO
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,this->_debugTlasBvhBoxesSsboId);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER,0,sizeof(TlasBvhNode) * this->_tlasBvhNodes.size(),_tlasBvhNodes.data());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);

    //Finish, i think?
    
}
