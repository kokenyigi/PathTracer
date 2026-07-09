uint CalculateInitialRngState(int x, int y, int frameIndex)
{
    return (uint)((uint)(x) * (uint)(1973) + (uint)(y) * (uint)(9277) + (uint)(frameIndex) * (uint)(26699)) | (uint)(1);
}

uint WangHashAndAlterSeed(uint* seed)
{
    *seed = (uint)(*seed ^ (uint)(61)) ^ (uint)(*seed >> (uint)(16));
    *seed *= (uint)(9);
    *seed = *seed ^ (*seed >> 4);
    *seed *= (uint)(0x27d4eb2d);
    *seed = *seed ^ (*seed >> 15);
    return *seed;
}

// Generates a random float uniformly in the 0.0f - 1.0f range
// And also alters state!
float GenerateRandomFloat(uint* state)
{
    return (float)(WangHashAndAlterSeed(state)) / 4294967296.0f;
}

// Generates a vector of unit length, uniformly distributed along a sphere's surface.
float3 GenerateRandomVector(uint* state)
{
    float z = GenerateRandomFloat(state) * 2.0f - 1.0f; // The so called height of the vector, basically its z component
    float phi = GenerateRandomFloat(state) * M_PI_2_F;

    float r = sqrt(1 - z*z); // This is the perpendicular projection's length from (0,0) -> projected radius
    float x = r * cos(phi);
    float y = r * sin(phi);

    return (float3)(x,y,z);
}




float3 Reflect(float3* v, float3* n)
{
	return (*v - 2 * *n * dot(*v, *n));
}

bool Refract(float3* v, float3* n, float eta,float3* outVec)
{
	float cosAlpha = dot(-*v, *n);
	float underSqr = 1.0f - (1 - cosAlpha * cosAlpha) / (eta * eta);

	if (underSqr < 0.0f)
	{
		return false; // NUmerical error -> Total internal reflection happend
	}

	*outVec =  ( *v/eta + *n * (cosAlpha/eta - sqrt(underSqr)) );

	return true;
}

typedef struct
{
    float4 col[4];

} Matrix4x4;

float3 Matrix4x4MultVec4(__global const Matrix4x4* matrix, float4 vector)
{
	//The matrix is represented in coloumn wise order
	float4 retval = 
        vector.x * matrix->col[0] + 
        vector.y * matrix->col[1] +
        vector.z * matrix->col[2] +
        vector.w * matrix->col[3];

	return retval.xyz;
}

float3 Matrix4x4TransposeMultVec4(__global const Matrix4x4* matrix, float4 vector)
{
	float4 retval;
	retval.x = dot(matrix->col[0], vector);
	retval.y = dot(matrix->col[1], vector);
	retval.z = dot(matrix->col[2], vector);
    retval.w = dot(matrix->col[3], vector);
	
	return retval.xyz;
}


typedef struct
{
    float4 position;
    float4 forward;
    float4 upward;
    float4 leftward;
    float fovx;
    float aspect;
    float zNear;
    float zFar;

} CameraData;

typedef struct
{
    float4 min;
    float4 max;

} AABB4;


typedef float4 VertexPositionData;

typedef struct
{
    float4 normal;
    float2 texCoords;

} VertexAttributeData;

typedef int4 TriangleIndicesData;

typedef struct
{
    int startIndex;
    int endIndex;
    int minChild;
    int maxChild;
    AABB4 box;

} BvhNodeData;

typedef float4 RgbaData;

typedef struct 
{
    float r;
    float g;
    float b;

} RgbData;

typedef struct 
{
    int startIndex;
    int width;
    int height;

} TextureData;

typedef struct
{
    float4 albedoColor;
    float4 emissionColor;

    float emissionStrength;
    float metallic;
    float roughness;
    float ioR;

    float transmission;
    int albedoTextureIndex;
    int padding1;
    int padding2;

} MaterialData;

typedef struct
{
    int meshBvhRootIndex;
    int materialIndex;

} ModelData;

typedef struct 
{
    int modelIndex;
    int padding1;
    int padding2;
    int padding3;
    Matrix4x4 worldTransform;
    Matrix4x4 invWorldTransform;

} ObjectData;






typedef struct
{
    float3 origin;
    float tMin;
    float3 direction;
    float tMax;
    float3 invDirection;

} Ray;

typedef struct
{
    float3 barycentricWeights;
    float t;

} TriangleIntersectResult;

typedef struct
{
    float t;

} BoxIntersectResult;

TriangleIntersectResult IntersectTriangle(const Ray* ray, float3 p0, float3 p1, float3 p2)
{
	TriangleIntersectResult retval;
	retval.t = ray->tMax + 0.000001f;

	float3 v01 = p1 - p0;
	float3 v02 = p2 - p0;

	/* Möller - Trombore Triangle-intersection algorithm
	Here we want to solve for the following:

	ray->origin + t*ray->direction = u*v01 + v* v02 + *p0

	O = ray->origin
	D = ray->direction

	This way our linear system of equations becomes:

	O - *p0 = -t*D + u*v01 + v*v02

	We get rid of the minus sign in front of t by swappping coloums in the LER later on.
	Furthermore, we use the triple scalar product efficiently to find our solutions for u,v,t

	Eventually we get:
				    u
	[v01,D,v02] * [ t ] = [O - *p0]
				    v

	We want to solve the system above, as efficiently as possible
	*/

	//Firstly we calculate our main denominator determinant
	float3 dirXv02 = cross(ray->direction, v02);
	float baseDeterminant = dot(v01, dirXv02);

	//Early exit if this determinant is pretty small -> means v01, v02, D are linearly codependent
	//This mainly happens when D is paralell to the triangle's plane
	if (fabs(baseDeterminant) < 1e-8f) return retval;
	float invBaseDeterminant = 1.0f / baseDeterminant;

	float3 b = ray->origin - p0;
	float u = dot(b, dirXv02) * invBaseDeterminant;
	if (u < 0.0f || u>1.0f) return retval;

	//This is important, we have this exact order here because we got rid of the minus sign in front of t
	float3  bXv01= cross(b, v01);
	float v = dot(ray->direction,bXv01) * invBaseDeterminant;
	if (v < 0.0f || u + v > 1.0f) return retval;

	float t = dot(v02,bXv01)*invBaseDeterminant;

	//if (t < ray->tMin || t > ray->tMax) return retval;

	retval.t = t;	
	retval.barycentricWeights.x = 1 - u - v;
	retval.barycentricWeights.y = u;
	retval.barycentricWeights.z = v;

	return retval;
}

BoxIntersectResult IntersectAABB4(const Ray* ray,__global const AABB4* box)
{
    BoxIntersectResult retval;
    retval.t = FLT_MAX;

    float tFar = FLT_MAX;
    float tNear = -FLT_MAX;

    for(int axis = 0; axis < 3; ++axis)
    {
        float tLowOnAxis;
        float tHighOnAxis;
        tLowOnAxis = (box->min[axis] - ray->origin[axis])*ray->invDirection[axis];
        tHighOnAxis = (box->max[axis] - ray->origin[axis])*ray->invDirection[axis];

        float tNearOnAxis = min(tLowOnAxis,tHighOnAxis);
        float tFarOnAxis = max(tLowOnAxis,tHighOnAxis);

        tNear = max(tNear,tNearOnAxis);
        tFar = min(tFar,tFarOnAxis);

        if(tNear > tFar) // nohit early exit
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
        retval.t = max(tNear,0.0f);
        return retval;
    }
}

typedef struct
{
    uint rngState;
    __global const VertexPositionData* vertexPositionData;
    __global const VertexAttributeData* vertexAttributeData;
    __global const TriangleIndicesData* triangleIndicesData;
    __global const BvhNodeData* bottomLevelBvhNodeData;
    __global const RgbaData* rgbaData;
    __global const TextureData* textureData;
    __global const MaterialData* materialData;
    __global const ModelData* modelData;
    __global const ObjectData* objectData;
    int objectCount;

} Scene;

typedef struct
{
    float3 normal;
    float t;
    float2 texCoords;
    int materialIndex;

} TraceResult;

#define MAX_BOTTOM_LEVEL_BVH_DEPTH 32

TraceResult IntersectObject(const Ray* ray, int objectIndex,const Scene* scene)
{
    TraceResult retval;
    retval.t = FLT_MAX;

    int3 closestTriangleIndices;
    float3 closestBarycentricWeights;

    Ray transformedRay;
    transformedRay.tMin = ray->tMin;
    transformedRay.tMax = ray->tMax;
    transformedRay.origin = Matrix4x4MultVec4(&scene->objectData[objectIndex].invWorldTransform,(float4)(ray->origin,1.0f));
    transformedRay.direction = Matrix4x4MultVec4(&scene->objectData[objectIndex].invWorldTransform,(float4)(ray->direction,0.0f));
    transformedRay.invDirection = 1.0f / transformedRay.direction;

    int modelIndex = scene->objectData[objectIndex].modelIndex;
    int bvhRootIndex = scene->modelData[modelIndex].meshBvhRootIndex;

    int stack[MAX_BOTTOM_LEVEL_BVH_DEPTH];
    stack[0] = bvhRootIndex;
    int stackCount = 1;

    while(stackCount > 0)
    {
        int currentBvhNodeIndex = stack[stackCount-1];
        --stackCount;
        __global const BvhNodeData* node = &scene->bottomLevelBvhNodeData[currentBvhNodeIndex];

        BoxIntersectResult result = IntersectAABB4(&transformedRay,&node->box);
        if(result.t >= 0 && result.t < transformedRay.tMax)
        {
            // We have intersected the currently analyzed Node
            // If its a leaf node, we have to test it's triangles
            if(node->minChild == -1 && node->maxChild == -1)
            {
                for (int triangle = node->startIndex;triangle < node->endIndex;++triangle)
                {
                    int3 triangleIndices = scene->triangleIndicesData[triangle].xyz;

                    TriangleIntersectResult triangleResult = IntersectTriangle(&transformedRay,
                        scene->vertexPositionData[triangleIndices.x].xyz,
                        scene->vertexPositionData[triangleIndices.y].xyz,
                        scene->vertexPositionData[triangleIndices.z].xyz);

                    if (triangleResult.t > transformedRay.tMin && triangleResult.t < transformedRay.tMax && triangleResult.t < retval.t)
                    {
                        // We found an intersection better than any before

                        retval.t = triangleResult.t;
                        transformedRay.tMax = triangleResult.t;

                        closestTriangleIndices = triangleIndices;
                        closestBarycentricWeights = triangleResult.barycentricWeights;
                    }
                }               
            }
            else
            {
                // We have found an inner node, which, according to our bvh construction, has exactly two children
                float distanceToMinNode = IntersectAABB4(&transformedRay,&scene->bottomLevelBvhNodeData[node->minChild].box).t;
                float distanceToMaxNode = IntersectAABB4(&transformedRay,&scene->bottomLevelBvhNodeData[node->maxChild].box).t;

                if(distanceToMinNode < transformedRay.tMax && distanceToMaxNode < transformedRay.tMax)
                {
                    if(distanceToMinNode < distanceToMaxNode)
                    {
                        stack[stackCount] = node->minChild;
                        stack[stackCount + 1] = node->maxChild;
                    }
                    else
                    {
                        stack[stackCount] = node->maxChild;
                        stack[stackCount + 1] = node->minChild;
                    }
                    stackCount += 2;
                }
                else if(distanceToMinNode < transformedRay.tMax)
                {
                    stack[stackCount] = node->minChild;
                    stackCount += 1;
                }
                else if(distanceToMaxNode < transformedRay.tMax)
                {
                    stack[stackCount] = node->maxChild;
                    stackCount += 1;
                }
            }
        }
    }

    // If the ray intersected a triangle
    if(retval.t < FLT_MAX)
    {
        // We have the information of the closest triangle stored inside variables
        float3 normal = 
            closestBarycentricWeights.x * scene->vertexAttributeData[closestTriangleIndices.x].normal.xyz +
            closestBarycentricWeights.y * scene->vertexAttributeData[closestTriangleIndices.y].normal.xyz +
            closestBarycentricWeights.z * scene->vertexAttributeData[closestTriangleIndices.z].normal.xyz;

        float2 textureCoords = 
            closestBarycentricWeights.x * scene->vertexAttributeData[closestTriangleIndices.x].texCoords.xy +
            closestBarycentricWeights.y * scene->vertexAttributeData[closestTriangleIndices.y].texCoords.xy +
            closestBarycentricWeights.z * scene->vertexAttributeData[closestTriangleIndices.z].texCoords.xy;

        float3 worldNormal = Matrix4x4TransposeMultVec4(&scene->objectData[objectIndex].invWorldTransform, (float4)(normal, 0.0f));

        retval.normal = worldNormal;
        retval.texCoords = textureCoords;
        retval.materialIndex = scene->modelData[modelIndex].materialIndex;     
    }

    return retval;
}

TraceResult TraceRay(const Ray* ray, const Scene* scene)
{
    TraceResult retval;
    retval.t = FLT_MAX;

    for(int objectIndex = 0; objectIndex < scene->objectCount; ++objectIndex)
    {
        TraceResult currentResult = IntersectObject(ray,objectIndex,scene);
        if(currentResult.t > ray->tMin && currentResult.t < ray->tMax && currentResult.t < retval.t)
        {
            retval = currentResult;
        }
    }

    return retval;
}

#define MAX_BOUNCE_COUNT 5

float3 CalculateRayColor(const Ray* primaryRay, const Scene* scene)
{
    float3 retval = (float3)(0,0,0);
    float3 throughPut = (float3)(1.0f,1.0f,1.0f);

    Ray ray = *primaryRay;
    for(int bounceIndex = 0; bounceIndex < MAX_BOUNCE_COUNT; ++ bounceIndex)
    {
        TraceResult traceResult = TraceRay(&ray,scene);
        if(traceResult.t > primaryRay->tMin && traceResult.t < primaryRay->tMax)
        {
            traceResult.normal = normalize(traceResult.normal);

            int materialIndex = traceResult.materialIndex;
            float3 albedo = scene->materialData[materialIndex].albedoColor.xyz;
            float3 emission = scene->materialData[materialIndex].emissionStrength*scene->materialData[materialIndex].emissionColor.xyz;
            
            ray.origin = ray.origin + traceResult.t * ray.direction + traceResult.normal * 0.00001f;
            ray.direction = normalize(traceResult.normal + GenerateRandomVector(&scene->rngState));
            ray.invDirection = 1.0f / ray.direction;

            retval += throughPut * emission;
            throughPut *= albedo;
        }
        else
        {
            break;
        }
    }

    return retval;
}


__kernel void renderimage(
    write_only image2d_t output,
    __global RgbData* helperBuffer,
    const int viewPortWidth,
    const int viewPortHeight,
    __global const CameraData* cameraData,
    __global const VertexPositionData* vertexPositionData,
    __global const VertexAttributeData* vertexAttributeData,
    __global const TriangleIndicesData* triangleIndicesData,
    __global const BvhNodeData* bottomLevelBvhNodeData,
    __global const RgbaData* rgbaData,
    __global const TextureData* textureData,
    __global const MaterialData* materialData,
    __global const ModelData* modelData,
    __global const ObjectData* objectData,
    const int objectCount,
    const int frameIndex
    )
{
    //The execution unit's coordinates based on N-dimensional Range
    int2 threadCoords = (int2)(get_global_id(0),get_global_id(1));

    //Exit if execution unit is outside of texture space
    if(threadCoords.x >= viewPortWidth || threadCoords.y >= viewPortHeight) return;

    Scene scene;
    scene.vertexPositionData = vertexPositionData;
    scene.vertexAttributeData = vertexAttributeData;
    scene.triangleIndicesData = triangleIndicesData;
    scene.bottomLevelBvhNodeData = bottomLevelBvhNodeData;
    scene.rgbaData = rgbaData;
    scene.textureData = textureData;
    scene.materialData = materialData;
    scene.modelData = modelData;
    scene.objectData = objectData;
    scene.objectCount = objectCount;
    scene.rngState = CalculateInitialRngState(threadCoords.x,threadCoords.y,frameIndex);

    float halfWorldViewPortWidth =  tan(radians(cameraData->fovx) * 0.5f );
    float halfWorldViewPortHeight = halfWorldViewPortWidth / cameraData->aspect;

    float2 pixelMidCoordsNdc;
    pixelMidCoordsNdc.x = (((float)threadCoords.x + 0.5f) / viewPortWidth) * 2.0f - 1.0f;
    pixelMidCoordsNdc.y = (((float)threadCoords.y + 0.5f) / viewPortHeight) * 2.0f - 1.0f;

    float3 dirtyRayDirection = cameraData->forward.xyz - 
        pixelMidCoordsNdc.x * halfWorldViewPortWidth * cameraData->leftward.xyz +
        pixelMidCoordsNdc.y * halfWorldViewPortHeight * cameraData->upward.xyz;

    float3 rayDirection = normalize(dirtyRayDirection);

    Ray primaryRay;
    primaryRay.origin = cameraData->position.xyz;
    primaryRay.direction = rayDirection;
    primaryRay.tMin = cameraData->zNear;
    primaryRay.tMax = cameraData->zFar;
    primaryRay.invDirection = 1.0f / rayDirection;

    float3 calculatedColor = CalculateRayColor(&primaryRay,&scene);
    
    if(frameIndex <= 1)
    {
        helperBuffer[threadCoords.x + threadCoords.y * viewPortWidth].r = calculatedColor.x;
        helperBuffer[threadCoords.x + threadCoords.y * viewPortWidth].g = calculatedColor.y;
        helperBuffer[threadCoords.x + threadCoords.y * viewPortWidth].b = calculatedColor.z;
        write_imagef(output,threadCoords,(float4)(calculatedColor,1.0f));
    }
    else
    {
        float3 previousSumColor = (float3)(0,0,0);
        previousSumColor.x = helperBuffer[threadCoords.x + threadCoords.y * viewPortWidth].r;
        previousSumColor.y = helperBuffer[threadCoords.x + threadCoords.y * viewPortWidth].g;
        previousSumColor.z = helperBuffer[threadCoords.x + threadCoords.y * viewPortWidth].b;

        helperBuffer[threadCoords.x + threadCoords.y * viewPortWidth].r += calculatedColor.x;
        helperBuffer[threadCoords.x + threadCoords.y * viewPortWidth].g += calculatedColor.y;
        helperBuffer[threadCoords.x + threadCoords.y * viewPortWidth].b += calculatedColor.z;

        float3 normalizedColor = (previousSumColor + calculatedColor) / frameIndex;
        write_imagef(output,threadCoords,(float4)(normalizedColor,1.0f));
    }

    
}