#include "Slider.h"
#include <iostream>

void Slider::SetSliderDirection(SliderDirection direction)
{
    _sliderDirection = direction;

    RecalculateSliderHead();
}

void Slider::SetSliderValue(float value)
{
    _sliderValue = value;

    RecalculateSliderHead();
}

void Slider::SetSliderHeadSize(float value, ValueType type)
{
    _sliderHeadSizeDefinition = {value,type};

    RecalculateSliderHead();
}

void Slider::SetSliderBaseBGColor(float r, float g, float b)
{
    _baseSliderBGColor = glm::vec3(r,g,b);
}

void Slider::RecalculateSliderHead()
{

    if(guiContext == nullptr)
    {
        return;
    }

    glm::vec2 windowRes = glm::vec2(guiContext->windowWidth,guiContext->windowHeight);

    glm::vec2 ownMinFixed = _box.min * windowRes;
    glm::vec2 ownMaxFixed = _box.max * windowRes;
    float ownWidthFixed = ownMaxFixed.x - ownMinFixed.x;
    float ownHeightFixed = ownMaxFixed.y - ownMinFixed.y;
    
    glm::vec2 sliderHeadMinFixed = glm::vec2(0,0);
    glm::vec2 sliderHeadMaxFixed = glm::vec2(0,0);
    if(_sliderDirection == SLIDER_DIRECTION_VERTICAL)
    {
        float calculatedHeightValueFixed = CalculateValuebasedOnType(_sliderHeadSizeDefinition.type,_sliderHeadSizeDefinition.value,ownHeightFixed);

        if(calculatedHeightValueFixed > ownHeightFixed) return;

        sliderHeadMinFixed = glm::vec2(ownMinFixed.x, ownMinFixed.y + 
            _sliderValue * (ownHeightFixed-calculatedHeightValueFixed));


        sliderHeadMaxFixed = glm::vec2(ownMaxFixed.x, ownMinFixed.y + 
            _sliderValue * (ownHeightFixed-calculatedHeightValueFixed) + calculatedHeightValueFixed);

        sliderHeadSizeRelative = calculatedHeightValueFixed / windowRes.y;

        interpolatezeroValue = _box.min.y + sliderHeadSizeRelative * 0.5f;
        interpolateOneValue = _box.max.y - sliderHeadSizeRelative * 0.5f;
    }
    else if(_sliderDirection == SLIDER_DIRECTION_HORIZONTAL)
    {
        float calculatedWidthValueFixed = CalculateValuebasedOnType(_sliderHeadSizeDefinition.type,_sliderHeadSizeDefinition.value,ownWidthFixed);

        if(calculatedWidthValueFixed > ownWidthFixed) return;

        sliderHeadMinFixed = glm::vec2((1.0f - _sliderValue) * ownMinFixed.x + _sliderValue * (ownMaxFixed.x- calculatedWidthValueFixed),
            ownMinFixed.y);

        sliderHeadMaxFixed = glm::vec2(
            (1.0f - _sliderValue)* ownMinFixed.x + _sliderValue * (ownMaxFixed.x- calculatedWidthValueFixed) + calculatedWidthValueFixed,
            ownMaxFixed.y);

        sliderHeadSizeRelative = calculatedWidthValueFixed / windowRes.x;

        interpolatezeroValue = _box.min.x + sliderHeadSizeRelative * 0.5f;
        interpolateOneValue = _box.max.x - sliderHeadSizeRelative * 0.5f;
    }

    _sliderHeadBox.min = sliderHeadMinFixed / windowRes;
    _sliderHeadBox.max = sliderHeadMaxFixed / windowRes;
}

void Slider::DetermineValueBasedOnMousePos(const glm::vec2 &mousePosRelative)
{
    float newSliderValue = 0.0f;
    float interpolatedValue = 0.0f;
    if(_sliderDirection == SLIDER_DIRECTION_VERTICAL)
    {
        interpolatedValue = mousePosRelative.y;
    }
    else if(_sliderDirection == SLIDER_DIRECTION_HORIZONTAL)
    {
        interpolatedValue = mousePosRelative.x;
    }

    newSliderValue = (interpolatedValue - interpolatezeroValue) / (interpolateOneValue - interpolatezeroValue);

    if(newSliderValue < 0.0f)
    {
        newSliderValue = 0.0f;
    }
    else if(newSliderValue > 1.0f)
    {
        newSliderValue = 1.0f;
    }

    SetSliderValue(newSliderValue);

    if(_callbackContext != nullptr && _sliderValueChangedCallback != nullptr)
    {
        _sliderValueChangedCallback(_callbackContext,newSliderValue);
    }
}

void Slider::SliderResize()
{
    ControlResize();

    RecalculateSliderHead();
}

void Slider::SliderRender()
{
    CalculateRenderBox();
	
    Rectangle ndcControlRenderRect = {_renderBox.min.x,_renderBox.min.y,
									  _renderBox.max.x - _renderBox.min.x,
									  _renderBox.max.y-_renderBox.min.y};

	guiContext->renderer.RenderRectangle(ndcControlRenderRect,_baseSliderBGColor);

    IntersectAABB(_sliderHeadBox,_renderBox,_sliderHeadRenderBox);

    Rectangle ndcSliderHeadRect = {_sliderHeadRenderBox.min.x,_sliderHeadRenderBox.min.y,
									  _sliderHeadRenderBox.max.x - _sliderHeadRenderBox.min.x,
									  _sliderHeadRenderBox.max.y-_sliderHeadRenderBox.min.y};

    glm::vec3 finalRenderColor = _baseBGColor;
    if(isFocused)
    {
        finalRenderColor = _clickBGColor;
    }
    else if(isHovered)
    {
        finalRenderColor = _hoverBGColor;
    }

    guiContext->renderer.RenderRectangle(ndcSliderHeadRect,finalRenderColor);
}

void Slider::SliderClick(int button, int action)
{
    if(guiContext ==nullptr || button != 0) return;

    glm::vec2 mousePosRelative = guiContext->currentMousePos / glm::vec2(guiContext->windowWidth,guiContext->windowHeight);

    bool isMousePosOnSlider = IsPointInsideAABB(_renderBox,mousePosRelative);
    if(isMousePosOnSlider && action == 0)
    {
        SetFocused();
        DetermineValueBasedOnMousePos(mousePosRelative);
        //std::cout<<"Got into press\n";
    }
    else
    {
        //std::cout<<"Got into else\n";
        if(action == 1)
        {
            SetUnfocused();
        }
    }
}

void Slider::SliderMouseMove()
{
    glm::vec2 windowRes = glm::vec2(guiContext->windowWidth,guiContext->windowHeight);
    glm::vec2 mousePosRelative = guiContext->currentMousePos / windowRes;

    if(isFocused)
    {
        DetermineValueBasedOnMousePos(mousePosRelative);
    }
    else
    {
        
        isHovered = IsPointInsideAABB(_sliderHeadRenderBox,mousePosRelative);        
    }
}
