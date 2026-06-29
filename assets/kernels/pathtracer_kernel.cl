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


__kernel void renderimage(
    write_only image2d_t output,
    int viewPortWidth,
    int viewPortHeight,
    __constant CameraData* cameraData
    )
{
    //The execution unit's coordinates based on N-dimensional Range
    int2 threadCoords = (int2)(get_global_id(0),get_global_id(1));

    //Exit if execution unit is outside of texture space
    if(threadCoords.x >= viewPortWidth || threadCoords.y >= viewPortHeight) return;

    float halfWorldViewPortWidth =  tan(radians(cameraData->fovx) * 0.5f );
    float halfWorldViewPortHeight = halfWorldViewPortWidth / cameraData->aspect;

    float2 pixelMidCoordsNdc;
    pixelMidCoordsNdc.x = (((float)threadCoords.x + 0.5f) / viewPortWidth) * 2.0f - 1.0f;
    pixelMidCoordsNdc.y = (((float)threadCoords.y + 0.5f) / viewPortHeight) * 2.0f - 1.0f;

    float3 dirtyRayDirection = cameraData->forward.xyz - 
        pixelMidCoordsNdc.x * halfWorldViewPortWidth * cameraData->leftward.xyz +
        pixelMidCoordsNdc.y * halfWorldViewPortHeight * cameraData->upward.xyz;

    float3 rayDirection = normalize(dirtyRayDirection);

    write_imagef(output,threadCoords,(float4)(rayDirection,1.0f));
}