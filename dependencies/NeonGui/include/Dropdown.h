#pragma once
#ifndef DROPDOWN_H
#define DROPDOWN_H

#include "Gui.h"

//#include "ButtonBase.h"

#include "LayoutPanel.h"
#include "RadioButton.h"
#include "Label.h"

/**
 * A Dropdown is basically an interractable ButtonBase, composited with a layoutpanel full of radiobuttons
 * A Dropdown in its most basic state acts similiarly to a button, has a text display, hover, click works fine.
 * However the real magic happens when we click the dropdown, then its layoutpanel gets activated, starts functioning.
 * Once an option is chosen == a radiobutton from the layoutpanel is clicked then the text changes on the base,
 * And a callback is called, with the index of the clicked radiobutton.
 * Functions just like a button till it is clicked, after that the layoutpanel becaomes visible and 
 */
class Dropdown : public Control
{
protected:

    /**  
     * The main Label representing the currently chosen option.
     * This Label is visible when the dropdown is in its default state
     */
    Label _chosenOptionLabel;

    /**
     * This is the main LayoutPanel which stores the Radiobuttons inside it.
     * Its min/max is calculated by the Dropdown, and its size depends mainly also on the maxVisibleOptions variable.
     */
    LayoutPanel _optionsPanel;
    glm::vec3 _optionBaseColor = glm::vec3(0.3,0,0.1);
    glm::vec3 _optionHoverColor = glm::vec3(0.1,0.4,0.1);
    glm::vec3 _optionClickColor = glm::vec3(1,0.4,0.0);
    glm::vec3 _optionTextColor = glm::vec3(0);

    /**
     * This is the group that the inner Radiobuttons are connected by.
     * The necessary callback is also defined here.
     */
    RadioButtonGroup _optionsGroup;

    void* _callbackContext = nullptr;
    //This function is run when an option has been chosen, the dropdown passes the chosen option's index as a parameter
    void (*_callback)(void* , int) = nullptr;


    //Maximum Visible Radiobuttons inside the optionsPanel at a time.
    int _maxVisibleOptionCount = 5;
    int _currentOptionCount = 0;

    AABB _ownBox;
    AABB _ownRenderBox;

    bool _isOpen = false;

public:
    Dropdown();
    ~Dropdown(){}

    void SetCallbackContext(void* context){_callbackContext = context;}
    void SetCallback(void (*callback)(void*,int)){_callback = callback;}
    void SetMaxVisibleOptionCount(int count) {_maxVisibleOptionCount = count; DropdownResize();}
    void SetChosenOption(int index); //TODO
    void SetScrollBarSize(float size, ValueType type = FIXED) {_optionsPanel.SetScrollbarSize(size,type);}

    void AddOption(const std::string& optionName, int optionIndex);

    static void OptionButtonCallback(void* context, int index);



    void VirtualRender() override
    {
        DropdownRender();
    }
    void VirtualResize() override
    {
        DropdownResize();
    }
    void VirtualMouseMove() override
    {
        DropdownMouseMove();
    }
    void VirtualClick(int button, int action) override
    {
        DropdownMouseClick(button,action);
    }
    void VirtualMouseWheel(float amount, int direction) override
    {
        DropdownMouseWheel(amount,direction);
    }

    void SetChosenOptionTextColor(float r, float g, float b) {_chosenOptionLabel.SetTextColor(r,g,b);}
    void SetOptionTextColor(float r, float g, float b);
    void SetOptionBaseColor(float r, float g, float b);
    void SetOptionHoveredColor(float r, float g, float b);
    void SetOptionClickedColor(float r, float g, float b);
    void SetScrollBarBaseBGColor(float r, float g, float b) {_optionsPanel.SetScrollBarBaseBGColor(r,g,b);}
    void SetScrollBarBaseColor(float r, float g, float b) {_optionsPanel.SetScrollBarBaseColor(r,g,b);}
    void SetScrollBarHoveredColor(float r, float g, float b) {_optionsPanel.SetScrollBarHoveredColor(r,g,b);}
    void SetScrollBarClickedColor(float r, float g, float b) {_optionsPanel.SetScrollBarClickedColor(r,g,b);}

protected:

    void DropdownResize();
    void DropdownRender();
    void DropdownUpdate(){}
    void DropdownMouseClick(int button, int action);
    void DropdownMouseMove();
    void DropdownKeyPressed(char key){}
    void DropdownMouseWheel(float amount, int direction);

    void SubControlSetGuicontext(GuiContext* guiContext) override;

};


#endif