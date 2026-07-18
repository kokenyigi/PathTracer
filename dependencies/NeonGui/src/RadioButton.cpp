#include "RadioButton.h"

RadioButton::RadioButton()
{
    
}

RadioButton::~RadioButton()
{
    
}

void RadioButton::SetToggledOn()
{
    isClicked = true;
    if(_groupContext != nullptr)
    {
        if(_groupContext->_currentToggled != nullptr && _groupContext->_currentToggled != this)
        {
            _groupContext->_currentToggled->SetToggledOff();
        }

        _groupContext->_currentToggled = this;
    }
}

void RadioButton::SetToggledOff()
{
    isClicked = false;
    if(_groupContext != nullptr)
    {
        _groupContext->_currentToggled = nullptr;
    }
}

bool RadioButton::RadioButtonClick(int button, int action)
{
    //ControlClick(button, action);

    if(isHovered && _groupContext != nullptr && _groupContext->_currentToggled != this)
    {
        if(button == 0 && action == 0)
        {
            SetToggledOn();

            _groupContext->_callBack(_callBackContext,_index);

            return true;
        }
    }

    return false;
}

void RadioButtonGroup::SetToggledOff()
{
    if(_currentToggled != nullptr)
    {
        RadioButton* current = (RadioButton*)_currentToggled;
        current->SetToggledOff();
    }
}
