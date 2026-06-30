#include "ContainerBase.h"


ContainerBase::ContainerBase()
{

}

ContainerBase::~ContainerBase()
{
}

void ContainerBase::ContainerBaseUpdate()
{
    for(int i=0; i < _children.size() ; ++i)
    {
        _children[i]->Update();
    }
}

void ContainerBase::ContainerBaseRender()
{
    
	
    Rectangle ndcControlRenderRect = {_renderBox.min.x,_renderBox.min.y,
									  _renderBox.max.x - _renderBox.min.x,
									  _renderBox.max.y-_renderBox.min.y};

	guiContext->renderer.RenderRectangle(ndcControlRenderRect,_baseBGColor);


    for(int i=0; i < _children.size() ; ++i)
    {
        _children[i]->Render();
    }
}

void ContainerBase::ContainerBaseResize()
{
    ControlResize();

    CalculateRenderBox();

    for(int i=0; i < _children.size() ; ++i)
    {
        _children[i]->Resize();
    }
}

void ContainerBase::ContainerBaseMouseMove()
{
    if( IsCursorOnControl(guiContext->currentMousePos) || IsCursorOnControl(guiContext->oldMousePos))
    {
        isHovered = true;
        
    }
    else
    {
        isHovered = false;
    }

    for(int i=0;i<_children.size();++i)
    {
        _children[i]->MouseMove();
    }
}

void ContainerBase::ContainerBaseClick(int button, int action)
{
    bool isCurrentMousePosOnContainer = IsCursorOnControl(guiContext->currentMousePos);

    if(isCurrentMousePosOnContainer)
    {
        for(int i = 0; i<_children.size();++i)
        {
            _children[i]->Click(button,action);
        }
    }
}

void ContainerBase::ContainerBaseMouseWheel(float amount, int direction)
{
    if(isHovered)
    {
        for(int i = 0; i<_children.size();++i)
        {
            _children[i]->MouseWheel(amount,direction);
        }
    }
}

void ContainerBase::ContainerBaseAddControl(Control *control)
{
    _children.push_back(control);
    control->SetParent(this);
    if(this->guiContext != nullptr)
    {
        control->SetGuiContext(this->guiContext); 
    }
    
}


