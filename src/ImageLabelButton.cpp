#include "ImageLabelButton.h"

ImageLabelButton::ImageLabelButton()
{
    _buttonImage.SetHeight(90);
    _buttonImage.SetMargin(MARGIN_TOP,10.0f);
    _buttonImage.SetWidth(200.0f);
    _buttonImage.SetParent(this);
    _buttonImage.SetBGColor(1,1,1);

    //_inputName.SetMargin(MARGIN_LEFT,10.0f);
    _inputName.SetMargin(MARGIN_BOTTOM,10.0f);
    _inputName.SetHeight(30.0f);
    _inputName.SetWidth(230.0f);
    
    _inputName.SetCallbackContext(this);
    _inputName.SetTextChangedCallback(InjectiveTextChangedCallback);
    _inputName.SetText("_");
    _inputName.SetBGColor(1,1,1);
    _inputName.SetTextColor(0,0,0);
    _inputName.SetEditHeadColor(0,1,0);
    _inputName.SetParent(this);

    this->SetBGColor(0.4,0.4,0.4);
    this->SetHoverColor(0.5,0.5,0.5);
    this->SetClickColor(0.6,0.6,0.6);
    this->_label.SetText("");

}

void ImageLabelButton::InjectiveTextChangedCallback(void *context, const std::string &newText)
{
    ImageLabelButton* self = (ImageLabelButton*)context;

    if(self->_callBackContext != nullptr && self->_nameChangedCallback != nullptr)
    {
        self->_nameChangedCallback(self->_callBackContext,self->_index,newText);
    }
}

void ImageLabelButton::ILBResize()
{
    ButtonBaseResize();
    CalculateRenderBox();

    

    _buttonImage.Resize();
    _inputName.Resize();

    

    //std::cout<<"Resize Didnt crash\n";
}

void ImageLabelButton::ILBRender()
{
    ControlRender();
    
    _buttonImage.Render();
    _inputName.Render();

    //std::cout<<"Resize Didnt crash\n";
}

bool ImageLabelButton::ILBMouseMove()
{
    bool isCursorOnBase = IsCursorOnControl(guiContext->currentMousePos);
    bool doesInputNameHandleMouseMove = _inputName.MouseMove();
    if(isCursorOnBase && !doesInputNameHandleMouseMove)
    {
        isHovered = true;
    }
    else
    {
        isHovered = false;
    }
    //std::cout<<"MouseMove Didnt crash\n";
    return (isCursorOnBase);


}

bool ImageLabelButton::ILBMouseClick(int button, int action)
{
    bool doesInputNameHandleMouseClick = _inputName.Click(button,action);
    bool doesBaseHandleMouseClick = false;
    if(!doesInputNameHandleMouseClick)
    {
        doesBaseHandleMouseClick = RadioButtonClick(button,action);
    }
    //std::cout<<"Click Didnt crash\n";
    return (doesBaseHandleMouseClick || doesInputNameHandleMouseClick);
}
