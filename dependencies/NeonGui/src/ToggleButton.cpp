#include "ToggleButton.h"

void ToggleButton::ImitateTurnOff()
{
    if(this->_isToggled == false) return;

    this->_isToggled = false;
    if(this->_callBack != nullptr && this->_callBackContext != nullptr)
    {
        this->_callBack(this->_callBackContext,this->_isToggled);
    }
}

bool ToggleButton::ToggleButtonClick(int button, int action)
{
    bool isControlClicked = ControlClick(button, action);

    if(isHovered && _callBack != nullptr && _callBackContext != nullptr)
    {
        
        if(button == 0 && action == 0)
        {
            _isToggled = !_isToggled;
            _callBack(_callBackContext,_isToggled);
        }
    }

    return isControlClicked;
}

void ToggleButton::ToggleButtonRender()
{
    CalculateRenderBox();
	
    Rectangle ndcControlRenderRect = {_renderBox.min.x,_renderBox.min.y,
									  _renderBox.max.x - _renderBox.min.x,
									  _renderBox.max.y-_renderBox.min.y};

	if(_renderTexture == nullptr)
	{
		if(_isToggled)
        {
            _renderColor = _clickBGColor;
        }
        else
        {
            _renderColor = _baseBGColor;
        }

		guiContext->renderer.RenderRectangle(ndcControlRenderRect,_renderColor);
	}
	else
	{
		//Textured rendering
		AABB srcBox = {glm::vec2(0,0),glm::vec2(1,1)};
		TransformBasedOnTwoRelativeAABB(_box,_renderBox,srcBox);

		Rectangle srcRect = {srcBox.min.x,srcBox.min.y,srcBox.max.x - srcBox.min.x, srcBox.max.y-srcBox.min.y};

		//guiContext->renderer.RenderTexturedRectangle(ndcControlRenderRect,srcRect,*_renderTexture);

        if(_isToggled)
        {
            _renderColor = _clickBGColor;
        }
        else
        {
            _renderColor = _baseBGColor;
        }

		guiContext->renderer.RenderColoredTexturedRectangle(ndcControlRenderRect,srcRect,_renderColor,*_renderTexture);

		//std::cout<< "texture: " << _renderTexture->GetWidth() <<" " << _renderTexture->GetHeight() << "\n";
	}
	
}
