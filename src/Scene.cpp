#include "Scene.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "Utils.h"

#ifdef _WIN32

    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
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

static float SahBinSplitCost(const glm::vec3 parentBoxSize,const glm::vec3& box1Size, const glm::vec3& box2Size, 
    int box1TriagCount, int box2TriagCount)
{
	float parentBoxArea = 2 * (parentBoxSize.x * parentBoxSize.y + parentBoxSize.x * parentBoxSize.z + parentBoxSize.y * parentBoxSize.z);
	float box1Area = 2 * (box1Size.x * box1Size.y + box1Size.x * box1Size.z + box1Size.y * box1Size.z);
	float box2Area = 2 * (box2Size.x * box2Size.y + box2Size.x * box2Size.z + box2Size.y * box2Size.z);

	return 1.0f + box1Area/parentBoxArea  * box1TriagCount + box2Area/parentBoxArea * box2TriagCount;
}

static float SahLeafCost(const glm::vec3& boxSize, int triagCount)
{
	return triagCount;
}

void FeedPosToAABB4(AABB4& box, glm::vec3& pos)
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

    newMeshTriangleIndices.resize(newTriangleVertexIndices.size());
    for(int i=0;i<newMeshTriangleIndices.size();++i)
    {
        newMeshTriangleIndices[i].x = newTriangleVertexIndices[i].x;
        newMeshTriangleIndices[i].y = newTriangleVertexIndices[i].y;
        newMeshTriangleIndices[i].z = newTriangleVertexIndices[i].z;
    }

    //Now lets recursively create a BVH structure for our mesh.
    int outOfBoundsEndIndex =  newTriangleVertexIndices.size();
    // Now lets prepare our root bvh node, which we will later on split recursively
    //This node contains the full range of the mesh's indices
    BvhNodeData meshRootNode;
    meshRootNode.startIndex = 0;
    meshRootNode.endIndex = outOfBoundsEndIndex;
    meshRootNode.minChild = -1;
    meshRootNode.maxChild = -1;
    meshRootNode.box = CalculateAABB4BasedOnTriangles(0,outOfBoundsEndIndex,newVertexPositions,newTriangleVertexIndices);


}


bool Scene::GetSurfaceAreaHeuristicSplitDatas(const BvhNodeData &bvhNode, 
    const std::vector<glm::vec3> &vertexPositions, 
    const std::vector<glm::ivec3> &triangleVertexIndices, 
    int &splitAxisInd, float &splitValueAlongAxis)
{
    glm::vec3 currentNodeBoxSize = glm::vec3(bvhNode.box.max-bvhNode.box.min);
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
            glm::vec3 absoluteMaximumVector = glm::vec3(-FLT_MAX,-FLT_MAX,-FLT_MAX);

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

            // We querry if our current split's cost is better than any previously found ones, even the leafCost
            float currentSahSplitCost = SahBinSplitCost(currentNodeBoxSize,minSideSplitBoxSize,maxSideSplitBoxSize,
                minSideTriangleCount, maxSideTriangleCount);
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

bool Scene::TryLoadMesh(const std::string &filePathRelative, MeshInfo * meshInfo)
{



    return true;
}

