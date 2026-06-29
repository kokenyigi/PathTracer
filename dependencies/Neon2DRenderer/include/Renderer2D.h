#pragma once
#ifndef RENDERER2D_H
#define RENDERER2D_H

#include "MyOpenGL.h"
#include "Utils.h"

class Renderer2D
{
private:
    Mesh<VertexP2> simpleQuadMesh;
    Mesh<VertexP2T2> texturedQuadMesh;

    Shader axisAllignedQuadColorTexturedShader;
    Shader axisAllignedQuadTextureShader;
    Shader axisAllignedQuadColorShader;
public:
    Renderer2D();
    ~Renderer2D();

    void Init();

    void RenderRectangle(const Rectangle& dstRect,const glm::vec3& rgb);
    void RenderTexturedRectangle(const Rectangle& dstRect, const Rectangle& srcRect,const Texture& texture);
    void RenderColoredTexturedRectangle(const Rectangle& dstRect, const Rectangle& srcRect,const glm::vec3& rgb,const Texture& texture);
};

#endif