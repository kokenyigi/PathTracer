#include "Scene.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>

#include <stb_image.h>

#include "Utils.h"

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
        maxChildBvhNode.startIndex = currentNode.startIndex + minSideTriangleCount + 1;
        maxChildBvhNode.endIndex = currentNode.endIndex;
        maxChildBvhNode.box = CalculateAABB4BasedOnTriangles(currentNode.startIndex + minSideTriangleCount + 1,currentNode.endIndex,
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

    // We push the root to the bvhNodeStorage, then recursively try to split it.
    newMeshBvhNodes.push_back(meshRootNode); meshInfo->bvhDepth = 0;
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

        return true;
    }
    return false;
}
