#pragma once
#ifndef INTINPUT_H
#define INTINPUT_H

#include "TextInput.h"


class IntInput : public TextInput
{
protected:
    int _minValue = -INT_MAX;
    int _maxValue = INT_MAX;

    void (*_intCallback)(void*,int) = nullptr;
public:
    IntInput();

    void SetIntInterval(int min, int max) {_minValue = min; _maxValue = max;}
    void SetInt(int value);

    void SetIntCallback(void (*intCallback)(void*,int)) {_intCallback = intCallback;}

protected:
    bool VirtualTextAnalyzer() override;
    bool VirtualIsCharAllowed(char c) override;
};


#endif