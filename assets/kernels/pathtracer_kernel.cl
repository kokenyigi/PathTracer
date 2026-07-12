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
    float phi = GenerateRandomFloat(state) * M_PI_F * 2.0f;

    float r = sqrt(1 - z*z); // This is the perpendicular projection's length from (0,0) -> projected radius
    float x = r * cos(phi);
    float y = r * sin(phi);

    return (float3)(x,y,z);
}




float3 SampleGGXDistribution(float3 geometricNormal, float roughness, float random1, float random2)
{
    float alpha = roughness * roughness;
    float phi = 2 * M_PI_F * random1; // radian between 0 and 2pi

    float tanThetaSquared = alpha * alpha * random2 / (1.0f - random2); // Actual inverse CDF sampling

    float cosTheta = sqrt(1.0f / (tanThetaSquared + 1.0f));
    float sinTheta = sqrt(1.0f - cosTheta * cosTheta);

    float3 newNormalLocal;
    newNormalLocal.x = sinTheta * cos(phi);
    newNormalLocal.y = sinTheta * sin(phi);
    newNormalLocal.z = cosTheta;

    // Now we have to transform this locally-defined normal into world space
    // For this we will need a Orto-normal-system, having Z as geometricnormal
    float3 tangentFirst;
    if(fabs(geometricNormal.z) < 0.9999f) // Here we basically check if the geometric normal isnt paralell with axis z(to do cross)
    {
        tangentFirst = normalize(cross((float3)(0,0,1),geometricNormal));
    }
    else
    {
        // If z didnt work, lets use y axis
        tangentFirst = normalize(cross((float3)(0,1,0),geometricNormal));
    }

    float3 tangentSecond = cross(tangentFirst,geometricNormal); // both are normalized, and orthogonal

    float3 sampledNormal = newNormalLocal.x * tangentFirst + newNormalLocal.y * tangentSecond + newNormalLocal.z * geometricNormal;

    return sampledNormal;
}


float CalculateSmithMaskingShadowingG1(float nDotX,float alpha) // Where alpha is roughness ^ 2
{
    float alphaSquared = alpha * alpha;

    /*
    float denomA = nDotV * sqrt(alphaSquared + (1-alphaSquared)*nDotL*nDotL);
    float denomB = nDotL * sqrt(alphaSquared + (1-alphaSquared)*nDotV*nDotV);

    float denom = denomA + denomB;
    if(denom > 0.0f)
    {
        return 0.5f  / denom;
    }
    return 0.0f;
    */
     return
        (2.0f * nDotX) /
        (nDotX +
         sqrt(alphaSquared +
              (1.0f - alphaSquared) *
              nDotX * nDotX));


    //return 2.0f * nDotL * nDotV / (denomA + denomB);
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
        if(traceResult.t > ray.tMin && traceResult.t < ray.tMax)
        {
            // Setup basic variables
            // Geometric normal is the avarage normal, defined by the mesh.
            float3 geometricNormal = normalize(traceResult.normal);

            // This boolean value is true if and only if the ray is hitting the face from its back side
            bool isBackFace = dot(geometricNormal,ray.direction) > 0;

            // This hitpoint is the one thats calculated by the triangle-intersection algorithm
            // We will have to disposition this a bit along the geometric normal, so that we dont have shadow acne.
            float3 hitPoint = ray.origin + traceResult.t * ray.direction;

            // Easier to dereference variables
            int materialIndex = traceResult.materialIndex;
            float3 albedo = scene->materialData[materialIndex].albedoColor.xyz;
            float3 emission = scene->materialData[materialIndex].emissionStrength*scene->materialData[materialIndex].emissionColor.xyz;
            float ior = scene->materialData[materialIndex].ioR;
            float metallic = scene->materialData[materialIndex].metallic;
            float transmission = scene->materialData[materialIndex].transmission;
            float roughness = scene->materialData[materialIndex].roughness;

            float alpha = roughness * roughness;
            transmission *= (1.0f - metallic);

            retval += throughPut * emission;

            // Calculation of the given microfacet normal
            // We calculate it based on a model of the real world, 
            //  and in this model we "sample" a normal based on the materials roughness.
            // To actually calculate this, we will have to querry the inverse of the CDF of the PDF the model is based upon, which has a
            //  normal distribution in this case.
            float random1 = GenerateRandomFloat(&scene->rngState);
            float random2 = GenerateRandomFloat(&scene->rngState);
            float3 microFacetNormal = SampleGGXDistribution(geometricNormal,roughness,random1, random2);
            
            float activeIoR = ior;
            float3 activeGeometricNormal = geometricNormal;
            float3 activeMicrofacetNormal = microFacetNormal;
            if(isBackFace)
            {
                activeIoR = 1.0f / ior;
                activeMicrofacetNormal = -microFacetNormal;
                activeGeometricNormal = -geometricNormal;
            }

            // Schlick's approximation for the fresnel coefficient
            float3 fresnelDielectricBase = pow((ior - 1.0f) / (ior + 1.0f),2.0f);
            float3 fresnelMetalBase = albedo;
            float3 F0 = mix(fresnelDielectricBase,fresnelMetalBase,metallic);
            float cosTheta = dot(-ray.direction,activeMicrofacetNormal);
            float3 fresnel = F0 + (1.0f - F0) * pow((1.0f - cosTheta),5.0f);
            float fresnelComp = max(fresnel.z,max(fresnel.y,fresnel.x));

            // These probabilities add up to one, so that we can sample one of them by a random [0,1] number later on.
            float specularComp = 0.0f; // Basically the possibility of the ray reflecting specularly.
            float diffuseComp = 0.0f; // same, but diffusely
            float transmissionComp = 0.0f; // same again, but transmission through material
            specularComp += fresnelComp;

            // The energy left inside the light can now go in 2 directions.
            // Firstly, it can refract Into the material, or object, this is decide by how transmissive the material is.
            // Secondly, if the material isnt that transmissive, the rest of the energy goes into reflective energy.
            float remainingAfterFresnel = 1.0f - fresnelComp;
            float reflectiveAmount = remainingAfterFresnel * (1.0f - transmission);
            float transmissiveAmount = remainingAfterFresnel * transmission;
            transmissionComp += transmissiveAmount;

            // If, after both fresnel specular reflectional energy, and transmissive refracting energy
            //   there is still energy inside the light, we have to decide what portion of that energy is going to be diffusive
            //   and what portion will be specular. This is decided by how metallic the material is.
            diffuseComp += reflectiveAmount * (1.0f-metallic);
            specularComp += reflectiveAmount * metallic;
            
            
            
            float3 refractDirection;
            bool canRefract = Refract(&ray.direction , &activeMicrofacetNormal, activeIoR, &refractDirection);
            if(!canRefract)
            {
                specularComp += transmissionComp;
                transmissionComp = 0.0f;
            }
            
            float randomNumber = GenerateRandomFloat(&scene->rngState);
            if(randomNumber <= diffuseComp)
            {
                // Diffuse reflection
                // We use a cosine-weighted hamisphere sampling around the geometric normal.
                ray.direction = normalize(geometricNormal + GenerateRandomVector(&scene->rngState)); 
                throughPut *= albedo; // We only multiply it by the albedo, since the brdf and pdf cancel eachother out.

                ray.origin = hitPoint + activeGeometricNormal* 0.00001f;
            }
            else
            {
                float NdotV = dot(activeGeometricNormal,-ray.direction);
                float NdotM = dot(activeGeometricNormal,activeMicrofacetNormal);
                float VdotM = dot(-ray.direction,activeMicrofacetNormal);

                
                if(NdotM <= 0.0f || NdotV <= 0.0f || VdotM <= 0.0f)
                {
                    throughPut = (float3)(0,0,0);
                    break;
                }
                          

                if(randomNumber <= diffuseComp + specularComp)
                {
                    // Specular Reflection
                    // We sampled earlier a microfacet normal from the GGX sampling method,
                    //   and now we will reflect the ray according to that
                    // Since we used an importance-sampling technique here, GGX, we have to correct for it by
                    //  taking into consideration the used BRDF and PDF.
                    float3 reflectedDirection = normalize(Reflect(&ray.direction, &activeMicrofacetNormal));

                    // Some helper variables for the brdf & pdf
                    float NdotL = dot(activeGeometricNormal,reflectedDirection);

                    
                    if (NdotL <= 0.0f) 
                    {
                        // If we have degenerate microfacet normal reflection, we throw the ray away
                        // This isnt taht good... [TODO] fix
                        throughPut = (float3)(0.0f, 0.0f, 0.0f);
                        break;
                    }
                    

                    float smithGeometricCoefficient = CalculateSmithMaskingShadowingG1(NdotV,alpha) * 
                        CalculateSmithMaskingShadowingG1(NdotL,alpha); 
                    
                    ray.direction = reflectedDirection;
                    throughPut *= fresnel  * smithGeometricCoefficient * (VdotM) / (NdotV * NdotM) ;
                    throughPut /= specularComp;

                    ray.origin = hitPoint + activeGeometricNormal* 0.00001f;
                }
                else
                {
                    // Refraction based on the microfacet normal, and ray direction, and Snell's law.
                    // Previously we already checked for total internal reflection, therefore if we are here
                    //   we are guaranteed that we can refract.
                    float NdotL = fabs(dot(refractDirection,activeGeometricNormal));
                    float G = CalculateSmithMaskingShadowingG1(NdotV,alpha) * CalculateSmithMaskingShadowingG1(NdotL,alpha);

                    ray.direction = normalize(refractDirection);

                    
                    throughPut *= (1.0f - fresnel) * activeIoR * activeIoR * G * (VdotM) / (NdotV * NdotM);
                    throughPut /= transmissionComp;

                    ray.origin = hitPoint + activeGeometricNormal * -0.00001f;

                }
                

            }
        
            
            ray.invDirection = 1.0f / ray.direction;
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

    float unitWorldViewPortWidth = halfWorldViewPortWidth * 2.0f / viewPortWidth;
    float unitWorldViewPortHeight = halfWorldViewPortHeight * 2.0f / viewPortHeight;

    float2 pixelMidCoordsNdc;
    pixelMidCoordsNdc.x = (((float)threadCoords.x + 0.5f) / viewPortWidth) * 2.0f - 1.0f;
    pixelMidCoordsNdc.y = (((float)threadCoords.y + 0.5f) / viewPortHeight) * 2.0f - 1.0f;

    float2 randomJitter = (float2)(GenerateRandomFloat(&scene.rngState),GenerateRandomFloat(&scene.rngState)) -0.5f;
    float3 fullJitter = randomJitter.x *unitWorldViewPortWidth * cameraData->leftward.xyz + 
        randomJitter.y *unitWorldViewPortHeight * cameraData->upward.xyz;

    float3 dirtyRayDirection = cameraData->forward.xyz - 
        pixelMidCoordsNdc.x * halfWorldViewPortWidth * cameraData->leftward.xyz +
        pixelMidCoordsNdc.y * halfWorldViewPortHeight * cameraData->upward.xyz + 
        fullJitter;

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