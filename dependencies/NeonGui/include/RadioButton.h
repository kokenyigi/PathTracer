#pragma once
#ifndef RADIOBUTTON_H
#define RADIOBUTTON_H

#include "ButtonBase.h"

//forward declaration
class RadioButtonGroup;

class RadioButton : public ButtonBase
{
protected:

    // Radiobuttons dont come alone, and this context is whats keeping them together
    RadioButtonGroup* _groupContext = nullptr;

    //The index of a RadioButton makes the callback behave potentially differently when called.
    int _index = 0;
    
    

public:
    RadioButton();
    ~RadioButton();

    void SetIndex(int index) {_index = index;}
    void SetGroupContext(RadioButtonGroup* context) {_groupContext = context;}

    inline int GetIndex(){return _index;}

    void SetToggledOn();
    void SetToggledOff();

    void VirtualClick(int button, int action) override
    {
        RadioButtonClick(button,action);
    }

protected:
        
    void RadioButtonClick(int button, int action);
    //rest is the same as a buttonbase & control
};


/**
 * A class that holds the radiobuttons shared context
 */
class RadioButtonGroup
{
private:
    
public:

    // The currently toggled radiobutton of the group
    RadioButton* _currentToggled = nullptr;

    //The Main callback is called with the specified index when the radiobutton is clicked.
    void (*_callBack)(void* , int) = nullptr;

    void AddToGroup(RadioButton* radioButton)
    {
        radioButton->SetGroupContext(this);
    }

    void RemoveFromGroup(RadioButton* radioButton)
    {
        radioButton->SetGroupContext(nullptr);
    }

    void SetToggledOff();

    void SetCallback(void(*callBack)(void*,int)) {_callBack = callBack;} 
private:
};


#endif
