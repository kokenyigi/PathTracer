#include "IntInput.h"

IntInput::IntInput()
{
    _textLabel.SetText("0.0");
}

void IntInput::SetInt(int value)
{
    if(value >= _minValue && value <= _maxValue)
    {
        std::string newText = std::to_string(value);
        _textLabel.SetText(newText);
    }   
}

bool IntInput::VirtualTextAnalyzer()
{
    int valueOfText = 0;

    try
    {
        valueOfText = std::stoi(_textLabel.GetText());
    }
    catch(...)
    {
        return false;
    }

    if(valueOfText >= _minValue && valueOfText <= _maxValue)
    {
        if(_intCallback != nullptr && _callbackContext != nullptr)
        {
            _intCallback(_callbackContext,valueOfText);
        }
        return true;
    }
    else
    {
        return false;
    }
}

bool IntInput::VirtualIsCharAllowed(char c)
{
    if((c<= '9' && c>= '0') || c == '-')
    {
        return true;
    }
    return false;
}
