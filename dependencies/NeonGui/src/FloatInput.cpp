#include "FloatInput.h"

FloatInput::FloatInput()
{
    _textLabel.SetText("0.0");
}

void FloatInput::SetFloat(float value)
{
    if(value >= _minValue && value <= _maxValue)
    {
        std::string newText = std::to_string(value);
        _textLabel.SetText(newText);
    }   
}

bool FloatInput::VirtualTextAnalyzer()
{
    float valueOfText = 0.0f;

    try
    {
        valueOfText = std::stof(_textLabel.GetText());
    }
    catch(...)
    {
        return false;
    }

    if(valueOfText >= _minValue && valueOfText <= _maxValue)
    {
        if(_floatCallback != nullptr && _callbackContext != nullptr)
        {
            _floatCallback(_callbackContext,valueOfText);
        }
        return true;
    }
    else
    {
        return false;
    }
}

bool FloatInput::VirtualIsCharAllowed(char c)
{
    if((c<= '9' && c>= '0') || c == '.' || c == '-')
    {
        return true;
    }
    return false;
}
