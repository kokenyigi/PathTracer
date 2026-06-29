#pragma once
#ifndef SCENE_H
#define SCENE_H


#define CL_TARGET_OPENCL_VERSION 300
//Platform dependent includes


#include <CL/cl.h>
#include <CL/cl_gl.h>

#include "MyOpenGL.h"
#include "Camera.h"

/**
 * This class is basically a renderer, it handles IO, and renders the given scene into a texture.
 * In our application it is used to render the scene inside a canvas control.
 */
class Scene
{
private:
    Texture _renderTexture;

    RenderBuffer _renderBuffer;
    FrameBuffer _renderFrameBuffer;

    int _viewportWidth = 100;
    int _viewportHeight = 100;

    Camera _camera;
    bool _isFreeCam = false;
	bool _isMouseFirstPos = true;
    glm::vec2 _previousMousePos = glm::vec2(0,0);
    glm::vec2 _currentMousePos = glm::vec2(0,0);

    //Extra variables
    Mesh<VertexP3N3T2> testMesh;
    Shader testShader;

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

    void RasterizeRender();

    void PathTracedRender();

};


#endif