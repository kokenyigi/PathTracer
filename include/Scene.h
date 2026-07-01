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
struct AABB3
{
	cl_float4 min;
    cl_float4 max;
};

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
 * This structure stores the necessary informations for a BVH node.
 * startindex == first triangle of the given box's triangle interval, endIndex == last
 */
struct BvhNodeData
{
    int startIndex = -1;
    int endIndex = -1;
    int maxChild = -1;
    int minChild = -1;
    AABB3 box;
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
    Texture _renderTexture;

    

    int _viewportWidth = 100;
    int _viewportHeight = 100;

    Camera _camera;
    bool _isFreeCam = false;
	bool _isMouseFirstPos = true;
    glm::vec2 _previousMousePos = glm::vec2(0,0);
    glm::vec2 _currentMousePos = glm::vec2(0,0);

    bool _isRenderingPathTraced = true;

    //variables related to Pathtraced rendering


    //Variables related to Rasterized rendering
    RenderBuffer _renderBuffer;
    FrameBuffer _renderFrameBuffer;

    //Extra variables
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


    bool TryLoadPathTracedMesh(const std::string& filePathRelative,
        std::vector<VertexPositionData>& newMeshVertexPositions,
        std::vector<TriangleIndicesData>& newMeshTriangleIndices,
        std::vector<VertexAttributeData>& newMeshVertexAttributes,
        MeshInfo* meshInfo);
    

};


#endif