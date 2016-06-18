#include "Renderer.h"
#include "../BackEnd/RenderBackEnd.h"
#include <algorithm>
#include "../Engine/Engine.h"
namespace tzw {

Renderer * Renderer::m_instance = nullptr;
Renderer::Renderer()
{
    m_enable3DRender = true;
    m_enableGUIRender = true;
}

Renderer *Renderer::shared()
{
    if(!Renderer::m_instance)
    {
        Renderer::m_instance = new Renderer();
    }
    return Renderer::m_instance;
}

void Renderer::addRenderCommand(RenderCommand command)
{
    Engine::shared()->increaseDrawCallCount();
    switch(command.type())
    {
    case RenderCommand::RenderType::GUI:
        m_GUICommandList.push_back(command);
        break;
    case RenderCommand::RenderType::Common:
        m_CommonCommand.push_back(command);
        break;
    default:
        break;
    }
}

bool GUICommandSort(const RenderCommand &a,const RenderCommand &b)
{
    return a.Zorder() < b.Zorder();
}

void Renderer::renderAll()
{
    if(m_enable3DRender)
    {
        renderAllCommon();
    }
    if(m_enableGUIRender)
    {
        renderAllGUI();
    }
    clear();
}

void Renderer::renderAllCommon()
{
    for(auto i = m_CommonCommand.begin();i!=m_CommonCommand.end();i++)
    {
        RenderCommand &command = (*i);
        renderCommon(command);
    }
}

void Renderer::renderAllGUI()
{
    std::stable_sort(m_GUICommandList.begin(),m_GUICommandList.end(),GUICommandSort);
    RenderBackEnd::shared()->disableFunction(RenderFlag::RenderFunction::DepthTest);
    for(auto i =m_GUICommandList.begin();i!=m_GUICommandList.end();i++)
    {
        RenderCommand &command = (*i);
        renderGUI(command);
    }
    RenderBackEnd::shared()->enableFunction(RenderFlag::RenderFunction::DepthTest);
}

void Renderer::renderGUI(RenderCommand &command)
{
    render(command);
}

void Renderer::renderCommon(RenderCommand &command)
{
    RenderBackEnd::shared()->setDepthTestMethod(command.depthTestPolicy().depthTest());
    render(command);
}

void Renderer::clear()
{
    m_CommonCommand.clear();
    m_GUICommandList.clear();
}

void Renderer::render(RenderCommand &command)
{
    command.m_technique->use();
    command.m_mesh->getArrayBuf()->use();
    command.m_mesh->getIndexBuf()->use();
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //bind gl program
    auto program = command.m_technique->program();
    // Offset for position
    quintptr offset = 0;
    Engine::shared()->increaseVerticesIndicesCount(command.m_mesh->getVerticesSize(),command.m_mesh->getIndicesSize());
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));
    // Offset for texture coordinate
    offset += sizeof(vec3);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));
    offset += sizeof(vec2);

    int normalLocation = program->attributeLocation("a_normal");
    program->enableAttributeArray(normalLocation);
    program->setAttributeBuffer(normalLocation,GL_FLOAT,offset,3,sizeof(VertexData));
    switch(command.m_primitiveType)
    {
    case RenderCommand::PrimitiveType::TRIANGLES:
        RenderBackEnd::shared()->drawElement(RenderFlag::IndicesType::Triangles,command.m_mesh->getIndicesSize(),0);
        break;
    case RenderCommand::PrimitiveType::TRIANGLE_STRIP:
        RenderBackEnd::shared()->drawElement(RenderFlag::IndicesType::TriangleStrip,command.m_mesh->getIndicesSize(),0);
       break;
    }
}
///
/// \brief 获取是否禁用3D渲染
/// \return
///
bool Renderer::enable3DRender() const
{
    return m_enable3DRender;
}
///
/// \brief 决定是否禁用3D渲染
/// \param enable3DRender
///
void Renderer::setEnable3DRender(bool enable3DRender)
{
    m_enable3DRender = enable3DRender;
}

///
/// \brief 获取是否渲染GUI
/// \return
///
bool Renderer::enableGUIRender() const
{
    return m_enableGUIRender;
}

///
/// \brief 决定是否禁用GUI渲染
/// \param enableGUIRender
///
void Renderer::setEnableGUIRender(bool enableGUIRender)
{
    m_enableGUIRender = enableGUIRender;
}



} // namespace tzw

