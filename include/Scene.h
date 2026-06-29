#pragma once
#ifndef SCENE_H
#define SCENE_H


#define CL_TARGET_OPENCL_VERSION 300
//Platform dependent includes


#include <CL/cl.h>
#include <CL/cl_gl.h>

#include "MyOpenGL.h"
#include "Camera.h"

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


    void Delete();

private:
    void InitCL();
    cl_kernel InitKernel(const std::string& filePath);

    void RasterizeRender();

    void PathTracedRender();

};


#endif