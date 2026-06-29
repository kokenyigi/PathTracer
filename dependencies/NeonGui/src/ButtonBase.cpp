#include "ButtonBase.h"

ButtonBase::ButtonBase()
{
    _label.SetHeight(0.5f,ValueType::RELATIVE);
    _label.SetMargin(MARGIN_LEFT,0.0f);
    _label.SetMargin(MARGIN_RIGHT,0.0f);
    _label.SetTextAllignment(TEXT_ALLIGNMENT_MID);
    _label.SetParent(this);
}

ButtonBase::~ButtonBase()
{

}

void ButtonBase::SetText(const std::string &newText)
{
    _label.SetText(newText);
}

void ButtonBase::SetTextSize(float value, ValueType type)
{
    _label.SetHeight(value,type);
}

void ButtonBase::SetTextColor(float r, float g, float b)
{
    _label.SetTextColor(r,g,b);
}

void ButtonBase::ButtonBaseRender()
{
    ControlRender();

    _label.Render();
}

void ButtonBase::ButtonBaseResize()
{
    ControlResize();

    _label.Resize();
}
