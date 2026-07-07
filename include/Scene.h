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
    AABB4 box;
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
 * This is the unit value that are stored inside the huge-huge RgbaDataBuffer, in a continuos array.
 * These data intervals are then indexed by the later declared textureData.
 */
struct RgbaData
{
    float r;
    float g;
    float b;
    float a;
};

/**
 * This is the unit data structure which's primary goal is to index the array consisting of RGBA values.
 * startindex: the first rgba value's index that the texture begins with, and then the size  of the texture can be calculated 
 * with width * height
 */
struct TextureData
{
    int startIndex;
    int width;
    int height;
};



/**
 * This structure contains necessary information related to materials of different models.
 * Each of these fields can be edited by the outside, so its important good care is taken.
 * It is also important to watch out for the floating-point intervals these values can be in.
 * And as always, once any fields have been changed, the same changes have to be delegated to the GPU as well.
 */
struct MaterialData
{
    cl_float4 albedoColor = {0,0,0,0}; // 0.0f - 1.0f for all, w comp not used
    cl_float4 emissionColor = {0,0,0,0};;

    float emissionStrength = 0.0f; // 0.0f....
    float metallic = 0.0f; // 0.0f - 1.0f
    float roughness = 0.0f; // 0.0f - 1.0f
    float ior = 1.0f; //1.0f - 2.5f

    float transmission = 0.0f; // 0.0f - 1.0f
    int albedoTextureIndex = -1; // -1,0,1.....
    
    int padding0;
    int padding1;
};

/**
 * This structure stores a model CPU side, and it has a meshIndex, because only through this index can we set a proper mesh
 *  to the model (As a user we cant set the bvhrootNode directly, so we have to use this indirection)
 */
struct ModelDataCpu
{
    int meshIndex = -1;
    int materialIndex = -1;
};
/**
 * This struct is very very similiar to the one above, and means the exact same: a specification of a model in GPU space.
 * However, it is missing the indirection created by meshindex. IMPORTANT mental model: There is a thing
 *  we can learn from Database, from all things: if ever there is a one-one relation (for every item of one relation there is exactly
 *  one relation on the other side, for efficiency's sake we can merge these relations straigth up, and this is exactly what
 *  we will do, because GPU side everything has to be as fast as it just can be.)
 */
struct ModelDataGpu
{
    int bvhRootIndex = -1;
    int materialIndex = -1;
};


/**
 * For every object in the scene, we also have a cpu-side only transform component, which just simply shows 
 *  the raw pos, scale, rot components for display and easy querry. 
 * Once this struct is altered from the outside, or the inside, object data's worldtransform 
 *  and invworldtransform have to be recalculated.
 * Rotation is stored in euler indices
 */
struct Transform
{
    glm::vec3 position = glm::vec3(0);
    glm::vec3 scale = glm::vec3(1);
    glm::vec3 rotation = glm::vec3(0);
};


struct ObjectData
{
    int modelIndex = -1;
    int padding1;
    int padding2;
    int padding3;
    glm::mat4 worldTransform = glm::mat4(1.0f);
    glm::mat4 invWorldTransform = glm::mat4(1.0f);
};


/**
 * This structure is just a way to enable communication of Mesh information to the outside eg.: View (App) layer
 */
struct MeshInfo
{
    int vertexCount = -1;
    int triangleCount = -1;
    int bvhNodeCount = -1;
    int bvhDepth = -1;
    int meshIndex = -1;
};

/**
 * This structure is just used for communication between the scene and the outside.
 */
struct TextureInfo
{
    int width = -1;
    int height = -1;
    int textureIndex = -1;
};

/**
 * This struct is simply used to convey the index value the given loaded material is at inside the scene vector
 * The outside can then later on use this index as a pointer/reference, to alter the material from the outside.
 * I have thought about putting other information here, since a material consist of more info than just a regular index, but
 * that is not right.
 */
struct MaterialInfo
{
    int materialIndex = -1;
};


//Same as before, just for communication
struct ModelInfo
{
    int modelIndex = -1;
};


/**
 * Basically information about the loaded object, it only contains an index for now
 */
struct ObjectInfo
{
    int objectIndex;
};

struct ObjectState
{
    int modelIndex = -1;
    Transform transform;
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

    const int _maximumBottomLevelBvhDepth = 32;
    const int _sahBinSize = 32;
    std::vector<BvhNodeData> _bottomLevelBvhNodeDatas;
    cl_mem _bottomLevelBvhNodeDatasBuffer;

    std::vector<MeshBvhRootNodeIndexData> _meshBvhRootIndexData;
    //no buffer, we only need this cpu side

    std::vector<RgbaData> _rgbaDatas;
    cl_mem _rgbaDatasBuffer;

    std::vector<TextureData> _textureDatas;
    cl_mem _textureDatasBuffer;
    
    //float
    std::vector<MaterialData> _materialDatas;
    cl_mem _materialDataBuffer;

    std::vector<ModelDataCpu> _modelDatas;
    cl_mem _modelDataBuffer;
    
    const int _maximumObjectCount = 200;
    std::vector<Transform> _objectTransforms;
    std::vector<ObjectData> _objectDatas;
    cl_mem _objectDataBuffer;



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

    /**
     * This function can be called by outside, and it tries to load a .png, .jpg picture, into both
     * CPU side and GPU side memory. If the given picture doesnt have alpha channel, then we give it some(preprocessing of texture)
     * Otherwise loads similiarly as a mesh.
     */
    bool TryLoadTexture(const std::string& filePathRelative, TextureInfo* textureInfo = nullptr);


    /**
     * This function just simply append a new materialData struct to the end of the already existing ones, 
     *  both CPU and GPU side. the newly created materialData's index is then written into the parameter MaterialInfo.
     * The return value of the function implies whether or not the addition of the material was successful.
     */
    bool TryAddMaterial(MaterialInfo* materialInfo = nullptr);
    bool GetMaterialData(int materialIndex, MaterialData* materialData); //returns false when index out of bounds
    bool TryAlterMaterial(int materialIndex, const MaterialData& alterredMaterialData); // same

    /**
     * This function tries to add a modeldata instance to the already existing ones both CPU and GPU side, it fails loudly with a false
     *  return value, if there are no meshes or materials already present (Why would u wanna create a model without meshes and materials)
     *  returns true on success, and sets the mesh to the 0.th index and material to the 0.th as well.
     */
    bool TryAddModel(ModelInfo* modelInfo = nullptr);
    bool GetModelData(int modelIndex,ModelDataCpu* modelData); // retval == false if index out of bounds
    bool TryAlterModel(int modelIndex,const ModelDataCpu& alteredModelData); //same

    /**
     * This function tries to add an object to an already allocated object buffer both CPU and GPU side.
     * It can fail: if objectBuffer is full or if no models have been created yet (MAKE A MODEL FIRST)
     * retval = success identifier
     */
    bool TryAddObject(ObjectInfo* objectInfo = nullptr);
    bool GetObjectState(int objectIndex, ObjectState* objectState); // getter for objectState
    bool TryAlterObject(int objectIndex, const ObjectState& alteredObjectState); //also writes data to GPU
    bool TryDeleteObject(int objectIndex); // Back-swaps object, and erases end element

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
        MeshInfo* meshInfo = nullptr);

    /**
     * To the function above here lie all the helper, mini functions, mainly related to splitting the Bvh node into smaller ones.
     * This is done recursingly, by using something called Surface Area Heuristic - SAH for short, which tries its best to choose
     * the best possible axis(x,y,z) and splitValue( 0.0f -------0.6f ---1.0f) Along that exis.
     * VERY IMPORTANT! Returns whether or not it found a split thats better than leaving it as a leaf node.
     */
    bool GetSurfaceAreaHeuristicSplitDatas(const BvhNodeData& node,const std::vector<glm::vec3>& vertexPositions,
        const std::vector<glm::ivec3>& triangleVertexIndices,
        int& splitAxisInd, float& splitValueAlongAxis);
    
    /**
     * This is a recursive procedure which does two main things:
     *  - first it creates new BVH nodes based on whether or not it is worth it to split a currently existing one
     *  - secondly, it rearranges the triangles inside the vector passed as argumat, 
     *    because bvhnodes are structures which act on INTERVALS, and yeah thats it.
     */
    void SplitBvhNodeRecursive(int bvhNodeIndex, int recursionDepth,const std::vector<glm::vec3>& vertexPositions, 
        std::vector<BvhNodeData>& bvhNodeStrorage,std::vector<glm::ivec3>& triangleVertexIndices,MeshInfo* meshInfo = nullptr);



    /**
     * This function loads the necessary float bytestream from the .png, .jpg, .bmp formatted binary file.
     * It returns whether or not the reading was succesful. it also constructs the rgbaDatas vector, and acts as
     * basically a helper function for the main tryloadtexture func.
     */
    bool TryLoadPathTracedTexture(const std::string& filePathRelative,std::vector<RgbaData>& newRgbaDatas,
        int& width, int& height,TextureInfo* textureInfo = nullptr);
    

    //This helper function just simply calculates the world transformation of an object.
    void RecalculateWorldTransformOfObject(int objectIndex);
};


#endif