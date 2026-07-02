#pragma once
#ifndef SCENE_H
#define SCENE_H


#define CL_TARGET_OPENCL_VERSION 300
//Platform dependent includes


#include <CL/cl.h>
#include <CL/cl_gl.h>

#include "MyOpenGL.h"
#include "Camera.h"

static cl_float4 GlmToCl(const glm::vec4& glm)
{
    cl_float4 retval;
    retval.x = glm.x;
    retval.y = glm.y;
    retval.z = glm.z;
    retval.w = glm.w;

    return retval;
}

static glm::vec4 ClToGlm(const cl_float4& cl)
{
    glm::vec4 retval;
    retval.x = cl.x;
    retval.y = cl.y;
    retval.z = cl.z;
    retval.w = cl.w;

    return retval;
}

 
#include "Utils.h"

/**
 * This structure is basically written into GPU memory each frame,
 * Containes necessary information related to the user's camera.
 */
struct CameraData
{
    cl_float4 position;
    cl_float4 forward;
    cl_float4 upward;
    cl_float4 leftward;
    float fovx;
    float aspect;
    float zNear;
    float zFar;
};

/**
 * Three - dimensional Axis Alligned Bounding Box
 *  THe OpenCL way -> cl_float3 = cl_float4 allignment wise.
 */


/**
 * Just a simple way to unify the namespace namings.
 */
typedef cl_float4 VertexPositionData;

/**
 * x,y and z store the 3 indices of the triangle.
 */
typedef cl_int4 TriangleIndicesData;

/**
 * In the Pathtraced sense its worth it to seperate vertex positions from other attributes.
 * The reason is that the positions are accessed frequently, to check for arithmetic triangle-ray intersections
 * However, the other attributes arent needed for this, therefore we get a bit of cache-optimization.
 */
struct VertexAttributeData
{
    cl_float4 normal;
    cl_float2 textureCoords;
};

/**
 * The reason we have a seperate field for these, is because each mesh has a bvhRootNode, inside the bvhnodes buffer
 * and we want to keep track which of these nodes is the actual start of the mesh, this is mainly only needed CPU-side
 * because, on CPU side is where the logic happens, aka, if we want to change the mesh of a model, then we have to specify
 * which mesh we want to change it to, and what better way is there rather than using these veriables down below, that
 * point to the rootindex of its bvhnode structure?
 */
typedef int MeshBvhRootNodeIndexData;

/**
 * This structure stores the necessary informations for a BVH node.
 * startindex == first triangle of the given box's triangle interval, endIndex == last
 */
struct BvhNodeData
{
    int startIndex = -1;
    int endIndex = -1;
    int maxChild = -1;
    int minChild = -1;
    AABB4 box;
};

/**
 * This structure is just a way to enable communication of Mesh information to the outside eg.: View (App) layer
 */
struct MeshInfo
{
    int vertexCount = 0;
    int triangleCount = 0;
    int bvhNodeCount = 0;
    int bvhDepth = 0;
};


/**
 * This class is basically a renderer, it handles IO, and renders the given scene into a texture.
 * In our application it is used to render the scene inside a canvas control.
 */
class Scene
{
private:
    /**
     * The main texture we are rendering into int all rendering modes(Rasterized / Pathtraced)
     * as stated above, this class's only job is to properly create this texture based on its internal context.
     */
    Texture _renderTexture;

    Camera _camera;
    bool _isFreeCam = false;
	bool _isMouseFirstPos = true;
    glm::vec2 _previousMousePos = glm::vec2(0,0);
    glm::vec2 _currentMousePos = glm::vec2(0,0);
    int _viewportWidth = 100;
    int _viewportHeight = 100;

    bool _isRenderingPathTraced = true;

    /**
     * Here lie the variables which are necessary for the Pathtraced rendering.
     * For each vector of data(CPU side) -> we have a cl_mem buffer on the device(GPU)
     * We must manage the communication between  these two in an efficient manner because
     * Moving memory is fucking slow.
     */
    std::vector<VertexPositionData> _vertexPositionData;
    cl_mem _vertexPositionDataBuffer;

    std::vector<VertexAttributeData> _vertexAttributeData;
    cl_mem _vertexAttributeDataBuffer;

    std::vector<TriangleIndicesData> _triangleIndicesData;
    cl_mem _triangleIndicesDataBuffer;

    int _maximumBottomLevelBvhDepth = 32;
    int _sahBinSize = 32;
    std::vector<BvhNodeData> _bottomLevelBvhNodeDatas;
    cl_mem _bottomLevelBvhNodeDatasBuffer;

    std::vector<MeshBvhRootNodeIndexData> _meshBvhRootIndexData;
    //no buffer, we only need this cpu side
    
    



    //Variables related to Rasterized rendering
    RenderBuffer _renderBuffer;
    FrameBuffer _renderFrameBuffer;

    Mesh<VertexP3N3T2> testMesh;
    Shader testShader;
    

    //OpenCL related variables
    cl_platform_id clPlatform;
    cl_device_id clDevice;
    cl_context clContext;
    cl_command_queue clCommandQueue;
    cl_kernel clPathTracerKernel;

    cl_mem clOpenglInteropTex;
    cl_mem clCameraDataBuffer;

public:
    Scene();
    ~Scene(){}

    void Init();

    Texture* GetTexture() {return &_renderTexture;}

    void Resize(int newWidth, int newHeight);
    void Render();
    void Update(float deltaTime);
    void MouseMove(float newX, float newY);
    void MouseClick(int button, int action);
    void MouseWheel(float amount, int direction);
    void KeyInput(int key, int action, int mods);

    /**
     * This functions tries its best to load a .obj mesh file into both the pathtraced and raytraced scene.
     * Returns true if successful, false otherwise.
     * With its second arg, one can querry information about the loaded mesh e.g.: bvh node count,
     *  bvh depth, vertex count, triag count
     */
    bool TryLoadMesh(const std::string& filePathRelative, MeshInfo* meshInfo = nullptr);


    void Delete();

private:
    void InitCL();
    cl_kernel InitKernel(const std::string& filePath);

    void RasterizeRender();
    void PathTracedRender();






    /**
     * This helper function basically tries to load, and preprocess the necessary data for a pathtraced mesh
     * These prepared datas are then written into the vectors given as parameters.
     * DISCLAIMER!! these datas cant be merged directly into the main container structures and buffers, since
     * for example the indices are starting from 0, which wouldn't be the case ever, therefore we must "move"
     * a lot of indices befora we can safely MERGE.
     * return true/false whether the .obj file load was successful or not.
     */
    bool TryLoadPathTracedMesh(const std::string& filePathRelative,
        std::vector<VertexPositionData>& newMeshVertexPositions,
        std::vector<TriangleIndicesData>& newMeshTriangleIndices,
        std::vector<VertexAttributeData>& newMeshVertexAttributes,
        std::vector<BvhNodeData>& newMeshBvhNodes,
        MeshInfo* meshInfo);

    /**
     * To the function above here lie all the helper, mini functions, mainly related to splitting the Bvh node into smaller ones.
     * This is done recursingly, by using something called Surface Area Heuristic - SAH for short, which tries its best to choose
     * the best possible axis(x,y,z) and splitValue( 0.0f -------0.6f ---1.0f) Along that exis.
     * VERY IMPORTANT! Returns whether or not it found a split thats better than leaving it as a leaf node.
     */
    bool GetSurfaceAreaHeuristicSplitDatas(const BvhNodeData& node,const std::vector<glm::vec3>& vertexPositions,
        const std::vector<glm::ivec3>& triangleVertexIndices,
        int& splitAxisInd, float& splitValueAlongAxis);

    void SplitBvhNode();// [TODO]
};


#endif