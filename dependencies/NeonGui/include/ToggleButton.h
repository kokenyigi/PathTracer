#pragma once
#ifndef TOGGLEBUTTON_H
#define TOGGLEBUTTON_H

#include "Gui.h"

#include "ButtonBase.h"

class ToggleButton : public ButtonBase
{
private:
    bool _isToggled;

    //In the case of a simple Button simply a basic handler function is called with the context.
    void(* _callBack)(void*,bool) = nullptr;


public:

    bool VirtualClick(int button, int action) override
    {
        return ToggleButtonClick(button, action);
    }

    void VirtualRender() override
    {
        ToggleButtonRender();
    }

    void SetCallback(void(*callBack)(void*,bool)){this->_callBack = callBack;}
protected:

    bool ToggleButtonClick(int button, int action);
    void ToggleButtonRender();
};





#endif