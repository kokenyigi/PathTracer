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
    int width,
    int height,
    __constant CameraData* cameraData;
    )
{
    
}