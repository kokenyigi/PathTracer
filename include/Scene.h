#pragma once
#ifndef SCENE_H
#define SCENE_H


#define CL_TARGET_OPENCL_VERSION 300
//Platform dependent includes


#include <CL/cl.h>
#include <CL/cl_gl.h>
//#include <CL/cl_gl_ext.h>

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
    float padding1;
    float padding2;
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

struct RgbData
{
    float r;
    float g;
    float b;
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
    int meshIndex = 0;
    int materialIndex = 0;
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

    glm::quat internalRotation;
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
 * So, since we dont want to calculate the real-world aabb bounds of an object each time we need it, we store this temporary structure
 * in a pre-allocated array for efficiency.
 */
struct ObjectBlasInstance
{
    glm::vec3 objectCentroid; //of object AABB (OBB)
    AABB3 worldBoundsBox; // of object
    int objectIndex; //the objectData
};

struct TlasBvhNode
{
    AABB4 box;
    //glm::vec3 boundingBoxMin; //the min corner of the world AABB the TlasbvhNode is in charge of(the chunk of the world that belongs to it)
    int minChildIndex = -1; //this index if its >= 0 points to an actual child of the bvhnode, in this case, maxchild points to the other
                            // However, if it is <0(negative) -> it means that this node is a leaf and therefore has no actual children.
                            // If this is  aleaf node, then maxChildIndex points to the ObjectBlasInstance the bvhnode encapsulates.
    //glm::vec3 boundingBoxMax; // again, just simply the max corner of the world chunk the bvhnode pastors
    int maxChildIndex = -1; // this index either stores the actual other child index of the node, but if the node is a leaf, this stores
                            // an objectBLAS instance.

    int padding0;
    int padding1;
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
    float absMaxRadius;
    float bvhRootMidYLevel;
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
    int modelIndex = 0;
    Transform transform;
};

struct Ray
{
    float tMin = 0.1f;
    float tMax = 1000.0f;
    glm::vec3 origin;
    glm::vec3 direction;
    glm::vec3 invDirection;
};

enum class GizmoType
{
    GIZMO_TRANSLATION = 0,
    GIZMO_SCALE = 1,
    GIZMO_ROTATION = 2
};

enum class PickResultType
{
    NONE,
    OBJECT,
    GIZMO
};

/**
 * This structure is used when the scene is picked for an object.
 * It can store many things, but for now we put the picked object's index into it, or -1 if the picking failed.
 */
struct PickResult
{
    PickResultType type = PickResultType::NONE;
    int pickedIndex = -1;
};

struct ObjectPickInfo
{
    int pickedObjectIndex = -1;
};

struct GizmoPickInfo
{
    int pickedGizmoIndex = -1;
};


struct GizmoInteractionState
{
    glm::vec3 interactionPlanePoint;
    glm::vec3 interactionPlaneNormal;

    glm::vec3 startInteractionPoint;
    glm::vec3 currentInteractionPoint; // we know if its the first or not, because click = first, move = later

    Transform startObjectWorldTransform;
};

struct GizmoInteractionInfo
{
    bool hasInteractionHappend = false;
    int objectIndexWithGizmo = -1;
};




struct BvhRangeData
{
    int startIndex;
    int nodeCount;
};

struct IndirectCommandData
{
    uint32_t count;
    uint32_t instanceCount;
    uint32_t firstIndex;
    int32_t  baseVertex;
    uint32_t baseInstance;
};

struct BestSplitResult
{
    float valueOnAxis;
    int axis;
};

struct PartitionResult
{
    int partitionIndex;
    AABB3 minSideAABB;
    AABB3 maxSideAABB;
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
    bool _isCtrlPressed = false;
	bool _isMouseFirstPos = true;
    glm::vec2 _previousMousePos = glm::vec2(0,0);
    glm::vec2 _currentMousePos = glm::vec2(0,0);
    int _viewportWidth = 100;
    int _viewportHeight = 100;
    bool _doesCameraHaveTheZoomies = false;
    bool _isRenderingPathTraced = true;


    // Gizmo-related informations
    bool _isGizmoVisible = false;
    int _objectWithGizmoIndex = -1;
    GizmoType _currentGizmoType = GizmoType::GIZMO_TRANSLATION;
    bool _isMouseDown = false;

    const float sizeK = 0.08f;
    Shader _gizmoShader;
    Mesh<VertexP3N3T2> _arrowGizmoMesh;
    glm::mat4 _arrowGizmoModelTransforms[3];
    Mesh<VertexP3N3T2> _cubeGizmoMesh;
    glm::mat4 _cubeGizmoModelTransforms[3];
    Mesh<VertexP3N3T2> _ringGizmoMesh;
    glm::mat4 _ringGizmoModelTransforms[3];

    std::vector<VertexPositionData> _gizmoVertexPositionDatas;
    std::vector<TriangleIndicesData> _gizmoTriangleVertexIndices;
    std::vector<BvhNodeData> _gizmoBvhNodeDatas;
    std::vector<int> _gizmoMeshBvhRoots;
    int _currentlyHighlightedGizmoAxis = -1;

    bool _isCurrentGizmoInteractedWith = false;
    GizmoInteractionState _gizmoInteractionState;

    /**
     * Here lie the variables which are necessary for the Pathtraced rendering.
     * For each vector of data(CPU side) -> we have a cl_mem buffer on the device(GPU)
     * We must manage the communication between  these two in an efficient manner because
     * Moving memory is fucking slow.
     */
    std::vector<VertexPositionData> _vertexPositionData;
    cl_mem _vertexPositionDataBuffer = nullptr;

    std::vector<VertexAttributeData> _vertexAttributeData;
    cl_mem _vertexAttributeDataBuffer = nullptr;

    std::vector<TriangleIndicesData> _triangleIndicesData;
    cl_mem _triangleIndicesDataBuffer = nullptr;

    const int _maximumBottomLevelBvhDepth = 32;
    const int _sahBinSize = 32;
    std::vector<BvhNodeData> _bottomLevelBvhNodeDatas;
    cl_mem _bottomLevelBvhNodeDatasBuffer = nullptr;

    std::vector<MeshBvhRootNodeIndexData> _meshBvhRootIndexData;
    //no buffer, we only need this cpu side

    std::vector<RgbaData> _rgbaDatas;
    cl_mem _rgbaDatasBuffer  = nullptr;

    std::vector<TextureData> _textureDatas;
    cl_mem _textureDatasBuffer = nullptr;
    
    //float
    std::vector<MaterialData> _materialDatas;
    cl_mem _materialDataBuffer = nullptr;

    std::vector<ModelDataCpu> _modelDatas;
    cl_mem _modelDataBuffer = nullptr;
    
    const int _maximumObjectCount = 200;
    std::vector<Transform> _objectTransforms;
    std::vector<ObjectData> _objectDatas;
    cl_mem _objectDataBuffer = nullptr;

    const int _tlasSahBinCount = 32;
    std::vector<ObjectBlasInstance> _temporaryObjectBlasInstances;
    std::vector<TlasBvhNode> _tlasBvhNodes;
    cl_mem _tlasBvhNodesBuffer = nullptr;

    unsigned int _frameIndex = 0;

    //Variables related to Rasterized rendering
    RenderBuffer _renderBuffer;
    FrameBuffer _renderFrameBuffer;

    std::vector<Mesh<VertexP3N3T2,Triangles>> _rasterizedMeshes;
    std::vector<Texture> _rasterizedTextures;


    //Debug Bvh Rendering Datas
    bool _isDebugBlasRenderEnabled = false;
    Shader _debugBvhShader;

    unsigned int _debugBvhVaoId = 0;
    unsigned int _debugBvhVboId = 0;
    unsigned int _debugBvhIboId = 0;

    const unsigned int _maximumDebugRenderBvhDepth = 11;

    std::vector<AABB4> _debugBlasBvhBoxes;
    unsigned int _debugBlasBvhBoxesSsboId = 0;
    std::vector<BvhRangeData> _debugBlasBvhMeshRanges; // no need to keep the ranges gpu side, since they will be a part of the indirect cmds

    //std::vector<glm::mat4> _debugObjectBvhInstanceWorldTransforms; // are already stored
    unsigned int _debugBlasBvhObjectWorldtransformsSsboId = 0;
    std::vector<IndirectCommandData> _debugBlasBvhIndirectCommandDatas;
    unsigned int _debugBlasBvhIndirectCommandsIndirectBufferObjectId = 0;

    // laughably, for the ssbo we only need because we just simply do regular old instancing using an ssbo where data of tlasnodes is stored. 
    bool _isDebugTlasRenderEnabled = false;
    Shader _debugTlasBvhShader;
    unsigned int _debugTlasBvhBoxesSsboId = 0;

    
    float _renderingFps;

    bool _isViewEnlarged = false;
    
    //OpenCL related variables
    cl_platform_id clPlatform;
    cl_device_id clDevice;
    cl_context clContext;
    cl_command_queue clCommandQueue;
    cl_kernel clPathTracerKernel;

    cl_mem clOpenglInteropTex; cl_mem clHelperBuffer = nullptr; // <-- stores the summed rgb values, which we will later divide
    cl_mem clCameraDataBuffer = nullptr;

public:
    Scene();
    ~Scene(){}

    void Init();

    Texture* GetTexture() {return &_renderTexture;}

    void Resize(int newWidth, int newHeight);
    void Render();
    void Update(float deltaTime);
    void MouseMove(float newX, float newY,GizmoInteractionInfo* gizmoInteractionInfo);
    void MouseClick(int button, int action, ObjectPickInfo* pickInfo);
    void MouseWheel(float amount, int direction);
    void KeyInput(int key, int action, int mods);

    /**
     * This functions tries its best to load a .obj mesh file into both the pathtraced and raytraced scene.
     * Returns true if successful, false otherwise.
     * With its second arg, one can querry information about the loaded mesh e.g.: bvh node count,
     *  bvh depth, vertex count, triag count
     */
    bool TryLoadMesh(const std::string& filePathRelative, MeshInfo* meshInfo = nullptr);
    inline Mesh<VertexP3N3T2>* GetRasterMeshPointer(int meshIndex){return &_rasterizedMeshes[meshIndex];}


    /**
     * This function can be called by outside, and it tries to load a .png, .jpg picture, into both
     * CPU side and GPU side memory. If the given picture doesnt have alpha channel, then we give it some(preprocessing of texture)
     * Otherwise loads similiarly as a mesh.
     */
    bool TryLoadTexture(const std::string& filePathRelative, TextureInfo* textureInfo = nullptr);
    inline Texture* GetRasterTexturePointer(int textureIndex){return &_rasterizedTextures[textureIndex];}

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
    void ChooseObject(int objectIndex);

    void SetBlasDebugView(bool isEnabled){this->_isDebugBlasRenderEnabled = isEnabled;}
    void SetTlasDebugView(bool isEnabled){this->_isDebugTlasRenderEnabled = isEnabled;}

    float GetRenderingFps(){return this->_renderingFps;}

    /**
     * This function is called whenever the scene canvas in the APP layer becomes full screen, if this is the case, we want to disable
     * all gizmo interaction, rendering, all that stuff, debug tlas and blas views will be disabled in the app layer, so no need to do 
     * anything with that
     */
    void MaximizeView();
    void MinimizeView();

    /**
     * This functions resets all buffers and vectors, and every single memory 
     *  that has been previously allocated both CPU and GPU side.
     * NOTE: this invalidates any indices, or references from the outside to any container from the inside of the scene.
     * Use With Caution!
     */
    void Reset();

    void Delete();

private:
    void InitCL();
    cl_kernel InitKernel(const std::string& filePath);

    void RasterizeRender();
    void PathTracedRender();

    void RenderGizmo();

    void RenderDebugBlasBvhBoxes();
    void RenderDebugTlasBvhBoxes();

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



    void ResetPathTracedFrameIndex() {_frameIndex = 1;}

    float IntersectPlane(const Ray& ray, const glm::vec3& planePoint, const glm::vec3& planeNormal);
    float IntersectTriangle(const Ray& ray,const glm::vec3& p0,const glm::vec3& p1,const glm::vec3& p2);
    float IntersectBox(const Ray& ray, const AABB4& box);
    float IntersectBvhNodeRecursive(Ray& ray, 
        const std::vector<VertexPositionData>& vertexPositions,
        const std::vector<TriangleIndicesData>& triangleVertexIndices,
        const std::vector<BvhNodeData>& bvhNodeDatas,
        const int meshBvhRootIndex);
    float IntersectObject(const Ray& ray, 
        const std::vector<VertexPositionData>& vertexPositions,
        const std::vector<TriangleIndicesData>& triangleVertexIndices,
        const std::vector<BvhNodeData>& bvhNodeDatas,
        const int meshBvhRootIndex,
        const glm::mat4& inverseWorldTransform);

    // Based on camera and the properties of the window, it calculates the ray direction
    glm::vec3 CalculateRayDirection(int x, int y);

    void PickScene( int x, int y, PickResult* pickResult);
    void PickCurrentGizmo(int x, int y,PickResult* pickResult);


    /**
     * This function is resposible for the smooth entering into gizmo interaction state.
     * This procedure calculates the necessary variables for later gizmo-interaction (mouse drag) and
     *  stores it inside the class' gizmointeractionstate variable.
     */
    void EnterGizmoInteractionMode();
    
    /**
     * Based on the perviously calculated start interaction state this procedure calculates the state for the current interaction moment.
     * This includes calculating the current interaction point, and trasnforming the object according to that point.
     * It also updates the interaction state.
     */
    void CalculateGizmoInteraction(int newX, int newY);
    void LeaveGizmoInteractionMode();

    /**
     * This procedure is just providing a quick way to check if the current mouse cursor position is above the currently visible gizmo
     * If it is, then it toggles highlighting index for that axis, if not, sets it to -1
     */
    void CheckForHighlightedAxis();


    void HelperBvhNodeBoxExtractorRecursive(std::vector<BvhNodeData>& bvhNodeDatas,int nodeIndex, int depth,
         std::vector<AABB4>& extractVector);
    void AddNewBvhNodeBoxesToDebugSsbo(std::vector<BvhNodeData>& bvhNodeDatas);



    

    /**
     * This function analyzes the given interval of object-level blas instance world bound AABBs, 
     *  and determines the best splitting value(on axis value),
     *  and best axis to split at, using binned(32 usually) Surface Area heuristic.
     */
    BestSplitResult FindBestSahSplitOfInterval(const TlasBvhNode& processedTlasNode, int intervalStart, int intervalEnd);

    /**
     * This functions will be called after the ideal split has been determined by the function above.
     * based on the data returned there, it will move around the temporary blas instances on an interval, according to the split value and axis
     * specified in its paramter list. 
     * Returns the where the middle of the partition is(the first index on the max side)
     */
    PartitionResult PartitionTemporaryInstanceInterval(int partitionAxis,float partitionValueOnAxis,int intervalStart, int intervalEnd);

    /**
     * This functions is called when the interval cant be split by SAH well, therefore we just simply split it by the x axis, and in the
     * Middle of the interval
     */
    PartitionResult FallbackPartitionInterval(int intervalStart, int IntervalEnd);

    /**
     * This procedure after using the functions above, determines the best split with it, re-arranges the temporary object instances,
     * and splits the given temporary blas instance interval in two(tries), it fails to split in the case of only one instance existing
     * inside the interval, int htis case the node we wanted to split becomes a leaf node.
     */
    void TrySplitTlasNodeRecursive(int tlasBvhNodeIndex, int intervalStart, int intervalEnd);
 
    /**
     * This function fully recalculates all temporary objectblasintances, and after that it builds up the tlas bvh tree, by recursively
     *  splitting the root node 
     */
    void ReconstructTlasBvh();
};


#endif