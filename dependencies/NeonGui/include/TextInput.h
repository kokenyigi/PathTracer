#pragma once
#ifndef TEXTINPUT_H
#define TEXTINPUT_H

#include "Gui.h"
#include "Label.h"

#include <string>

/**
 * The Textinput Control, its text can be edited when focused.
 */
class TextInput : public Control
{
protected:

    /**
     * Has a Label for code reuse and easier rendering visualization.
     */
    Label _textLabel;

    // This string is the one the label is set to when the edited string is not accepted by the textinput's criteria.
    std::string _toRollbackText = "null";

    void* _callbackContext = nullptr;
    void (*_callback)(void* , const std::string&) = nullptr;


    
    
    /** This variable controls where the flickering line-head is located inside the text
     *  insertion happens before the head, and its the same with deletion (backspace) 
     *  For convinience sake, the edithead is the "|" character, in the abc of the text rendering.
     */
    int _editHeadIndex = 0;
    AABB _editHeadBox;
    glm::vec3 _editHeadColor = glm::vec3(0,1,0);



public:
    TextInput();
    ~TextInput(){}

    void VirtualResize() override {TextInputResize();}
    void VirtualRender() override {TextInputRender();}
    void VirtualClick(int button, int action) override {TextInputClick(button,action);}
    void VirtualKeyInput(int key, int action, int mods) override {TextInputKeyInput(key,action,mods);}


    void SetEditHeadColor(float r, float g, float b) {_editHeadColor = glm::vec3(r,g,b);}
    void SetTextColor(float r, float g, float b) {_textLabel.SetTextColor(r,g,b);};
    void SetTextSize(float value, ValueType type = FIXED) {_textLabel.SetHeight(value,type);};

    void SetText(const std::string& text) {_textLabel.SetText(text);}

    void SetCallbackContext(void* callbackContext) {_callbackContext = callbackContext;}
    void SetTextChangedCallback(void(*callback)(void*,const std::string&)) {_callback = callback;}

protected:

    void TextInputResize();
    void TextInputRender();
    void TextInputUpdate(){}
    void TextInputMouseMove(){}
    void TextInputClick(int button, int action);
    void TextInputKeyInput(int key,int action, int mods);
    void TextInputMouseWheel(float amount, int direction){}

    void RecalculateEditHead(); //this proc is the one fully recalculating the edithead's pos, renderpos, and data
    void CalculateLabelProperties(); //This proc changes attributes of the textlabel, such that the edithead is renderred *pretty*
    void SetEditHeadAt(int newIndex); //This proc freely sets the edithead to a reeasonable index

    //Self-explanatory procedures
    void TryInsertCharacterAtHead(char c);
    void TryDeleteCharacterAtHead();
    void TryMoveEditHeadLeft();
    void TryMoveEditHeadRight();

    void TryFinalizeText();
    /**
     * This functions is run each time the TextInput's text changes fully(as in loses focus)
     * It returns whether or not the new string is accepted.
     */
    virtual bool VirtualTextAnalyzer(){return true;}
    virtual bool VirtualIsCharAllowed(char c){return true;}


    void SubControlSetGuicontext(GuiContext* guiContext) override { _textLabel.SetGuiContext(guiContext); }
};


#endif