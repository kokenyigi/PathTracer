#pragma once
#ifndef FLOATINPUT_H
#define FLOATINPUT_H

#include "TextInput.h"


class FloatInput : public TextInput
{
protected:
    float _minValue = -FLT_MAX;
    float _maxValue = FLT_MAX;

    void (*_floatCallback)(void*,float) = nullptr;
public:
    FloatInput();

    void SetFloatInterval(float min, float max) {_minValue = min; _maxValue = max;}
    void SetFloat(float value);

    void SetFloatCallback(void(*floatCallback)(void* , float)) {_floatCallback = floatCallback;}

protected:
    bool VirtualTextAnalyzer() override;
    bool VirtualIsCharAllowed(char c) override;
};


#endif

