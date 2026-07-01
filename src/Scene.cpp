#include "Scene.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>

#ifdef _WIN32

    #include <windows.h>

#endif


Scene::Scene()
{    
    _camera.Init(glm::vec3(0,0,4.0f),glm::vec3(0,0,0),glm::vec3(0,1,0),_viewportWidth,_viewportHeight);
}



void Scene::InitCL()
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

        clGetPlatformInfo(clPlatformIDs[i],CL_PLATFORM_NAME,sizeof(buffer),buffer,nullptr);

        std::cout<< "Platform name: "<<buffer<<"\n";
    }

    clPlatform = clPlatformIDs[0];

    cl_uint clPlatformDeviceCount;
    clError = clGetDeviceIDs(clPlatform,CL_DEVICE_TYPE_GPU,0,nullptr,&clPlatformDeviceCount);
    CHECK_ERROR(clError);

    std::vector<cl_device_id> clPlatformDeviceIDs(clPlatformDeviceCount);
    clError = clGetDeviceIDs(clPlatform,CL_DEVICE_TYPE_GPU,clPlatformDeviceCount,clPlatformDeviceIDs.data(),nullptr);
    CHECK_ERROR(clError);

    std::cout<<"The devices on platform 0. :\n";
    for(int i=0;i<clPlatformDeviceCount;++i)
    {
        char buffer[256];
        clGetDeviceInfo(clPlatformDeviceIDs[i],CL_DEVICE_NAME,sizeof(buffer),buffer,nullptr);
        std::cout<<"    Name: " << buffer<<"\n";

        unsigned int computeUnitCount = 0;
        clGetDeviceInfo(clPlatformDeviceIDs[i],CL_DEVICE_MAX_COMPUTE_UNITS,sizeof(unsigned int),&computeUnitCount,nullptr);
        std::cout<<"    Compute Units: "<<computeUnitCount <<"\n";

        cl_ulong deviceMemory = 0;
        clGetDeviceInfo(clPlatformDeviceIDs[i],CL_DEVICE_GLOBAL_MEM_SIZE,sizeof(cl_ulong),&deviceMemory,nullptr);
        std::cout<<"    Global Memory: "<<deviceMemory <<"\n\n";
    }

    clDevice = clPlatformDeviceIDs[0];


    cl_context_properties clContextProperties[] =
    {
        CL_GL_CONTEXT_KHR,   (cl_context_properties)wglGetCurrentContext(),
        CL_WGL_HDC_KHR,      (cl_context_properties)wglGetCurrentDC(),
        CL_CONTEXT_PLATFORM, (cl_context_properties)clPlatform,
        0
    };

    clContext = clCreateContext(clContextProperties,1,&clDevice,nullptr,nullptr,&clError);
    CHECK_ERROR(clError);

    clCommandQueue = clCreateCommandQueueWithProperties(clContext,clDevice,nullptr,&clError);
    CHECK_ERROR(clError);

    clPathTracerKernel = InitKernel("assets/kernels/pathtracer_kernel.cl");


    clOpenglInteropTex = clCreateFromGLTexture(clContext,CL_MEM_WRITE_ONLY,GL_TEXTURE_2D,0,_renderTexture.GetId(),&clError);
    CHECK_ERROR(clError);

    clCameraDataBuffer = clCreateBuffer(clContext,CL_MEM_READ_ONLY,sizeof(CameraData),nullptr,&clError);
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

    testMesh.Load("assets/models/Suzanne.obj");
    testShader.Init("assets/shaders/testVertex.vert","assets/shaders/testFragment.frag");

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

    _camera.Resize(newWidth,newHeight);

    _viewportWidth = newWidth;
    _viewportHeight = newHeight;

    clOpenglInteropTex = clCreateFromGLTexture(clContext,CL_MEM_WRITE_ONLY,GL_TEXTURE_2D,0,_renderTexture.GetId(),&clError);
    CHECK_ERROR(clError);
}

void Scene::Render()
{
    if(_isRenderingPathTraced)
    {
        PathTracedRender();
    }
    else
    {
        RasterizeRender();
    }
}

void Scene::RasterizeRender()
{
    glViewport(0,0,_viewportWidth,_viewportHeight);

    _renderFrameBuffer.Bind();

    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    testShader.Bind();

    testShader.SetUniform<glm::mat4>("uViewProjectionTransform",_camera.GetPerspectiveMatrix()*_camera.GetViewMatrix());

    testMesh.Draw();

    testShader.Unbind();

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
    cameraData.zNear = _camera.GetZFar();

    clError = clEnqueueWriteBuffer(clCommandQueue,clCameraDataBuffer,CL_TRUE,0,sizeof(CameraData),&cameraData,0,nullptr,nullptr);
    CHECK_ERROR(clError);

    clError = clSetKernelArg(clPathTracerKernel,0,sizeof(cl_mem),&clOpenglInteropTex);CHECK_ERROR(clError);
    clError = clSetKernelArg(clPathTracerKernel,1,sizeof(int),&_viewportWidth);CHECK_ERROR(clError);
    clError = clSetKernelArg(clPathTracerKernel,2,sizeof(int),&_viewportHeight);CHECK_ERROR(clError);
    clError = clSetKernelArg(clPathTracerKernel,3,sizeof(cl_mem),&clCameraDataBuffer);CHECK_ERROR(clError);

    size_t localSize[2] = { 16, 16 };
    size_t globalSize[2] = {((_viewportWidth + localSize[0] -1) / localSize[0]) * localSize[0],
                            ((_viewportHeight + localSize[1] -1) / localSize[1]) * localSize[1]};
    clError = clEnqueueNDRangeKernel(clCommandQueue,clPathTracerKernel,2,nullptr,globalSize,localSize,0,nullptr,nullptr);
    CHECK_ERROR(clError);

    clFinish(clCommandQueue);

    clError = clEnqueueReleaseGLObjects(clCommandQueue,1,&clOpenglInteropTex, 0, nullptr, nullptr);
    CHECK_ERROR(clError);

    clFinish(clCommandQueue);
}



void Scene::Update(float deltaTime)
{
    _camera.Update(deltaTime);
}

void Scene::MouseMove(float newX, float newY)
{
    _currentMousePos = glm::vec2(newX,newY);

    if(_isFreeCam)
    {
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

            _camera.Rotate(dx,dy);
        }
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

        _camera.TryInfluenceMovement(axis,direction,influence);
    }

    if(key == 70 && action == 1) // If F was pressed
    {
        if(_isFreeCam)
        {
            _isFreeCam = false;
        }
        else
        {
            _isFreeCam = true;
            _isMouseFirstPos = true;
        }
    }
}

bool Scene::TryLoadPathTracedMesh(const std::string &filePathRelative, 
    std::vector<VertexPositionData> &newMeshVertexPositions,
    std::vector<TriangleIndicesData> &newMeshTriangleIndices, 
    std::vector<VertexAttributeData> &newMeshVertexAttributes, 
    MeshInfo *meshInfo)
{
    
}

bool Scene::TryLoadMesh(const std::string &filePathRelative, MeshInfo * meshInfo)
{



    return true;
}

