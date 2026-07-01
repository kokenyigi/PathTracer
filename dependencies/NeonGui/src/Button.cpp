#include "Button.h"

Button::Button()
{
    
}

Button::~Button()
{

}

void Button::SetCallback(void (*callBack)(void *))
{
    _callBack = callBack;
}

void Button::ButtonClick(int button, int action)
{
    ControlClick(button, action);

    if(isHovered && _callBack != nullptr)
    {
        
        if(button == 0 && action == 0)
        {
            //std::cout<<"Button click called\n";
            _callBack(_callBackContext);
            
        }
    }
}
