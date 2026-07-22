#pragma once
#ifndef IMAGELABELBUTTON_H
#define IMAGELABELBUTTON_H

#include "Gui.h"
#include "TextInput.h"
#include "RadioButton.h"
#include "Button.h"

class ImageLabelButton : public RadioButton
{
protected:

    Button _buttonImage;

    TextInput _inputName;

    void (*_nameChangedCallback)(void*,int,const std::string&) = nullptr;

public:
    ImageLabelButton();
    ~ImageLabelButton(){}

    void SetInputText(const std::string& text){_inputName.SetText(text);}
    std::string GetInputText() {return _inputName.GetText();} 
    void SetButtonTexure(Texture* texture){_buttonImage.SetTexture(texture);}
    void SetNameChangedCallback(void(*nameChangedCallback)(void*,int,const std::string&)){_nameChangedCallback = nameChangedCallback;}

    void VirtualResize() override {ILBResize();}
    void VirtualRender() override {ILBRender();}
    bool VirtualMouseMove() override {return ILBMouseMove();}
    bool VirtualClick(int button, int action) override {return ILBMouseClick(button,action);}




    void SubControlSetGuicontext(GuiContext* guicontext) override {_buttonImage.SetGuiContext(guicontext);_inputName.SetGuiContext(guiContext);}

protected:
    static void InjectiveTextChangedCallback(void* context, const std::string& newText);

    void ILBResize();
    void ILBRender();
    //void ILBUpdate(){}
    bool ILBMouseMove();
    bool ILBMouseClick(int button, int action);
    // rest.. (they aint used)
};

#endif
