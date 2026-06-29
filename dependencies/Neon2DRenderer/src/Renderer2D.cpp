#include "Renderer2D.h"

#include <vector>

Renderer2D::Renderer2D()
{
}

Renderer2D::~Renderer2D()
{
}

void Renderer2D::Init()
{
    axisAllignedQuadColorShader.Init(
        "assets/shaders/AABC_vertex_shader.vert",
        "assets/shaders/AABC_fragment_shader.frag");

    axisAllignedQuadTextureShader.Init("assets/shaders/AABT_vertex_shader.vert","assets/shaders/AABT_fragment_shader.frag");
    axisAllignedQuadColorTexturedShader.Init("assets/shaders/AABCT_vertex_shader.vert","assets/shaders/AABCT_fragment_shader.frag");

    std::vector<VertexP2> simpleQuadVertices = 
    {
        {glm::vec2(-1,-1)},
        {glm::vec2(1,-1)},
        {glm::vec2(1,1)},
        {glm::vec2(-1,1)},
    };

    std::vector<unsigned int> quadIndices =
    {
        0,1,2,
        2,3,0
    };

    simpleQuadMesh.Init(simpleQuadVertices,quadIndices);

    std::vector<VertexP2T2> texturedQuadVertices = 
    {
        {glm::vec2(-1,-1),glm::vec2(0,0)},
        {glm::vec2(1,-1),glm::vec2(1,0)},
        {glm::vec2(1,1),glm::vec2(1,1)},
        {glm::vec2(-1,1),glm::vec2(0,1)},
    };

    texturedQuadMesh.Init(texturedQuadVertices,quadIndices);


}

void Renderer2D::RenderRectangle(const Rectangle &dstRect, const glm::vec3& rgb)
{
    axisAllignedQuadColorShader.Bind();
    axisAllignedQuadColorShader.SetUniform<glm::vec3>("uColor",rgb);

    glm::vec2 posScale = glm::vec2(dstRect.w, dstRect.h);
    glm::vec2 posOffset = glm::vec2(dstRect.w-1.0f + dstRect.x*2,-dstRect.h+1.0f-dstRect.y * 2);

    axisAllignedQuadColorShader.SetUniform<glm::vec2>("uPosScale",posScale);
    axisAllignedQuadColorShader.SetUniform<glm::vec2>("uPosOffset",posOffset);

    simpleQuadMesh.Draw();

    axisAllignedQuadColorShader.Unbind();
}

void Renderer2D::RenderTexturedRectangle(const Rectangle &dstRect, const Rectangle &srcRect, const Texture &texture)
{
    axisAllignedQuadTextureShader.Bind();

    texture.Bind(0);

    axisAllignedQuadTextureShader.SetUniform<int>("uTexture",0);
    
    glm::vec2 posScale = glm::vec2(dstRect.w, dstRect.h);
    glm::vec2 posOffset = glm::vec2(dstRect.w-1.0f + dstRect.x*2,-dstRect.h+1.0f-dstRect.y * 2);

    axisAllignedQuadTextureShader.SetUniform<glm::vec2>("uPosScale",posScale);
    axisAllignedQuadTextureShader.SetUniform<glm::vec2>("uPosOffset",posOffset);

    glm::vec2 textureScale = glm::vec2(srcRect.w,srcRect.h);
    glm::vec2 textureOffset = glm::vec2(srcRect.x,1.0f-srcRect.y-srcRect.h);

    axisAllignedQuadTextureShader.SetUniform<glm::vec2>("uTextureOffset",textureOffset);
    axisAllignedQuadTextureShader.SetUniform<glm::vec2>("uTextureScale",textureScale);

    texturedQuadMesh.Draw();

    texture.Unbind();

    axisAllignedQuadTextureShader.Unbind();
}

void Renderer2D::RenderColoredTexturedRectangle(const Rectangle &dstRect, const Rectangle &srcRect, const glm::vec3 &rgb, const Texture &texture)
{
    axisAllignedQuadColorTexturedShader.Bind();

    texture.Bind(0);

    axisAllignedQuadColorTexturedShader.SetUniform<int>("uTexture",0);
    axisAllignedQuadColorTexturedShader.SetUniform<glm::vec3>("uColor",rgb);
    
    glm::vec2 posScale = glm::vec2(dstRect.w, dstRect.h);
    glm::vec2 posOffset = glm::vec2(dstRect.w-1.0f + dstRect.x*2,-dstRect.h+1.0f-dstRect.y * 2);

    axisAllignedQuadColorTexturedShader.SetUniform<glm::vec2>("uPosScale",posScale);
    axisAllignedQuadColorTexturedShader.SetUniform<glm::vec2>("uPosOffset",posOffset);

    glm::vec2 textureScale = glm::vec2(srcRect.w,srcRect.h);
    glm::vec2 textureOffset = glm::vec2(srcRect.x,1.0f-srcRect.y-srcRect.h);

    axisAllignedQuadColorTexturedShader.SetUniform<glm::vec2>("uTextureOffset",textureOffset);
    axisAllignedQuadColorTexturedShader.SetUniform<glm::vec2>("uTextureScale",textureScale);

    texturedQuadMesh.Draw();

    texture.Unbind();

    axisAllignedQuadColorTexturedShader.Unbind();
}
