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

    void SetText(const std::string& text){_inputName.SetText(text);}
    void SetTexure(Texture* texture){_renderTexture = texture;}
    void SetNameChangedCallback(void(*nameChangedCallback)(void*,int,const std::string&)){_nameChangedCallback = nameChangedCallback;}








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
