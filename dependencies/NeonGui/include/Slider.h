#pragma once
#ifndef SLIDER_H
#define SLIDER_H

#include "Gui.h"

enum SliderDirection
{
    SLIDER_DIRECTION_VERTICAL,
    SLIDER_DIRECTION_HORIZONTAL
};

class Slider : public Control
{
protected:

    SliderDirection _sliderDirection = SLIDER_DIRECTION_VERTICAL;

    // This value ranges [0,1], interpolational value. 
    // It is determined by the ValueType::RELATIVE position of the sliderhead inside the slider
    float _sliderValue = 0.0f; 

    // This box represents the rectangular area of the moving slider head.
    AABB _sliderHeadBox = {glm::vec2(0,0),glm::vec2(1,1)};

    AABB _sliderHeadRenderBox = {glm::vec2(0,0),glm::vec2(1,1)};

    // All of the basic Control coloring now determines the slider head's color.
    // On the other hand, this color is the background color of the whole slider bg body.
    glm::vec3 _baseSliderBGColor = glm::vec3(0.1,0,0.2);

    // THis definition determines the non-ValueType::FIXED side of the sliderhead.
    ValueDefinition _sliderHeadSizeDefinition = {20.0f,ValueType::FIXED};
    

    // Helper variables
    float sliderHeadSizeRelative = 0.2f;
    float interpolatezeroValue = 0.0f;
    float interpolateOneValue = 0.0f;

    // Callback of the slider which is invoked when the slidervalue is changed eg.: sliding the head.
    void* _callbackContext = nullptr;
    void (*_sliderValueChangedCallback)(void* , float) = nullptr;

public:
    Slider(){};
    ~Slider(){};

    void VirtualResize() override
    {
        SliderResize();
    }

    void VirtualRender() override
    {
        SliderRender();
    }

    void VirtualClick(int button, int action) override
    {
        SliderClick(button, action);
    }

    void VirtualMouseMove() override
    {
        SliderMouseMove();
    }

    void SetSliderDirection(SliderDirection direction);
    void SetSliderValue(float value);
    void SetSliderHeadSize(float value, ValueType type = ValueType::FIXED);
    void SetSliderBaseBGColor(float r, float g, float b);
    void SetSliderBaseColor(float r, float g, float b) {_baseBGColor = glm::vec3(r,g,b);}
    void SetSliderHoveredColor(float r, float g, float b) {_hoverBGColor = glm::vec3(r,g,b);}
    void SetSliderClickedColor(float r, float g, float b) {_clickBGColor = glm::vec3(r,g,b);}
    void SetSliderValueChangedCallbackContext(void * context) {_callbackContext = context;}
    void SetSliderValueChangedCallback(void (*callback)(void* , float)){_sliderValueChangedCallback = callback;}

protected:

    // Again, a big ahh procedure.
    void RecalculateSliderHead();

    void DetermineValueBasedOnMousePos(const glm::vec2& mousePosRelative);

    void SliderResize();
    void SliderRender();
    void SliderUpdate(){}
    void SliderClick(int button, int action);
    void SliderMouseMove();

    

};


#endif
