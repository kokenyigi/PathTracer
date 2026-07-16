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


    for(int i=_children.size()-1; i>=0 ; --i)
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

    int controlPriority = control->GetPriority();

    int newControlIndex = _children.size()-1;
    int backwardsFirst = _children.size() - 2;
    while(backwardsFirst >= 0 && _children[backwardsFirst]->GetPriority() < controlPriority)
    {
        int priorityOfFirst = _children[backwardsFirst]->GetPriority();
        if(backwardsFirst >= 1)
        {
            int backwardSecond = backwardsFirst - 1;
            int priorityOfSecond = _children[backwardSecond]->GetPriority();
            
            if(priorityOfFirst < priorityOfSecond)
            {
                // we are at a breakpoint in priority, we must swap our new control into the first slot
                Control* tmp = _children[backwardsFirst];
                _children[backwardsFirst] = _children[newControlIndex];
                _children[newControlIndex] = tmp;
                newControlIndex = backwardsFirst;
            }
        }
        else // backwardsfirst == 0
        {
            Control* tmp = _children[backwardsFirst];
            _children[backwardsFirst] = _children[newControlIndex];
            _children[newControlIndex] = tmp;
            newControlIndex = backwardsFirst;
        }
        
        --backwardsFirst;
    }
}


