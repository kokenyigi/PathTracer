#pragma once
#ifndef LAYOUTPANEL_H
#define LAYOUTPANEL_H

#include "Gui.h"
#include "ContainerBase.h"
#include "Slider.h"

#include <vector>


enum LayoutType
{
    LAYOUT_VERTICAL = 0,
    LAYOUT_HORIZONTAL = 1,
    LAYOUT_FLOW = 2
};

enum PaddingType
{
    PADDING_TOP = 0,
    PADDING_LEFT = 1,
    PADDING_BOTTOM = 2,
    PADDING_RIGHT = 3
};

/*
    A layoutPanel is a container in which the order of children is deterministic.
*/
class LayoutPanel : public ContainerBase
{
private:
    
    //Inherits children container from parent class

    LayoutType _layoutType = LAYOUT_VERTICAL;
    
    //This boolean value is set to true if no children can be displayed
    bool isLayoutObstructed = false;

    /*
     These variables contain how big the children boxes should be.
     Invariants:
    */
    ValueDefinition _childrenWidthDefinition = {20,ValueType::FIXED};
    ValueDefinition _childrenHeightDefinition = {20,ValueType::FIXED};

    /*
     These definitions are similiar to that of margins, however these act Inside the LayoutPanel,
     serving as in inside margin for the children area.
     Invariants:
    */
    ValueDefinition _paddingDefinitions[4];
    
    //should be obvious
    ValueDefinition _gapWidthDefinition;
    ValueDefinition _gapHeightDefinition;

    //Helper Variables for easier calculations

    //The box of the children may be larger than the entire layoutpanel, every child will be positioned inside this.
    AABB _baseChildrenBox = {glm::vec2(0,0),glm::vec2(1,1)};
    AABB _actualChildrenBox = {glm::vec2(0,0),glm::vec2(1,1)};

    float childrenRelativeWidth = 0.0f;
    float childrenRelativeHeight = 0.0f;
    float gapRelativeWidth = 0.0f;
    float gapRelativeHeight = 0.0f;
    float paddingFrontRelative = 0.0f; //The amount of ValueType::RELATIVE padding top when vertical,flow  / left when horizontal
    float paddingEndRelative = 0.0f;

    float interpolateZeroValue  = 0.0f;
    float interpolateOneValue = 0.0f;
    bool isScrollable = false;
    float scrollValue = 0.0f;


    Slider _scrollBar;

    ValueDefinition _scrollBarSizeDefinition = {30.0f,ValueType::FIXED};


public:
    LayoutPanel();
    ~LayoutPanel();

    //Resize is different from a basic container

    void VirtualResize() override
    {
        LayoutPanelResize();
    }

    void VirtualRender() override
    {
        LayoutPanelRender();
    }

    void VirtualClick(int button, int action) override
    {
        LayoutPanelClick(button,action);
    }

    void VirtualMouseWheel(float amount, int direction) override 
    {
        LayoutPanelMouseWheel(amount,direction);
    }
    void VirtualMouseMove()
    {
        LayoutPanelMouseMove();
    }

    void AddControl(Control* control) override
    {
        LayoutPanelAddControl(control);
    }

    //[TODO] mouseWheel is also different

    //[TODO] Click is also gonna be different

    //[TODO] Mousemove is also gonna be different

    void SetLayoutType(LayoutType type);
    void SetChildrenWidth(float value, ValueType type = ValueType::FIXED);
    void SetChildrenHeight(float value, ValueType type = ValueType::FIXED);
    void SetPadding(PaddingType paddingType, float value, ValueType type = ValueType::FIXED);
    void SetGapWidth(float value, ValueType type = ValueType::FIXED);
    void SetGapHeight(float value, ValueType type = ValueType::FIXED);

    void SetScrollValue(float value);

    void SetScrollbarSize(float value, ValueType = ValueType::FIXED);

    //inline AABB GetChildrenBox() const {return _childrenBox;}
    inline bool GetIsObstructed() const {return isLayoutObstructed;}
    
    void SetScrollBarBaseBGColor(float r, float g, float b) {_scrollBar.SetSliderBaseBGColor(r,g,b);}
    void SetScrollBarBaseColor(float r, float g, float b) {_scrollBar.SetSliderBaseBGColor(r,g,b);}
    void SetScrollBarHoveredColor(float r, float g, float b) {_scrollBar.SetSliderHoveredColor(r,g,b);}
    void SetScrollBarClickedColor(float r, float g, float b) {_scrollBar.SetSliderClickedColor(r,g,b);}
    

private:
    static void ScrollbarValueChangedCallback(void* self, float t)
    {
        LayoutPanel* layoutPanel =  (LayoutPanel*)self;
        layoutPanel->SetScrollValue(t);
    }

protected:

    /*
    This procedure calculates the box, the children will be positioned in.
    It takes into consideration the specified width/height/gap/padding definitions as well as the scrollbar.
    This procedure is separated from the rest for readability and reusability.
    */
    void CalculateChildrenBox();

    /*
    Holy procedure...
    This procedure calculates the ValueType::RELATIVE height/width values the scrolling interpolates between.
    It also checks if scrolling is possible or not, if not, then it sets a boolean value to false.
    */
    void CalculateInterpolatingBorders();

    /*
    This procedure calculates each child's position inside the childrenBox.
    */
    void CalculateChildrenPositions();

    /*
    This procedure simply moves the calculated _childrenBox by the specified ValueType::RELATIVE normalized amount.
    It is mainly used when scrolling.
    */
    void MoveChildrenBox(float amount);
    
    void FullyRecalculateChildren();
    
    
    void TurnOffScrollBar() {_scrollBar.SetInactive();}
    void TurnOnScrollBar() {_scrollBar.SetActive();}

    void SetScrollableOn() {isScrollable = true; TurnOnScrollBar();}
    void SetScrollableOff() {isScrollable = false; TurnOffScrollBar();}

    void SetLayoutObstructedOn() {isLayoutObstructed = true; TurnOffScrollBar();}
    void SetLayoutObstructedOff() {isLayoutObstructed = false; if(isScrollable){ TurnOnScrollBar();}}


    void LayoutPanelRender();
    void LayoutPanelResize();
    void LayoutPanelUpdate() {};
    void LayoutPanelClick(int button, int action);
    void LayoutPanelMouseWheel(float amount, int direction);
    void LayoutPanelMouseMove();

    void LayoutPanelAddControl(Control* control);

    void SubControlSetGuicontext(GuiContext* guiContext) override;    
    
};


#endif

