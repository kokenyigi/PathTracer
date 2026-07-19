#include "LayoutPanel.h"

LayoutPanel::LayoutPanel()
{
    _scrollBar.SetMargin(MARGIN_TOP,0.0f);
    _scrollBar.SetMargin(MARGIN_BOTTOM,0.0f);
    _scrollBar.SetMargin(MARGIN_RIGHT,0.0f);
    _scrollBar.SetWidth(40.0f);
    _scrollBar.SetSliderHeadSize(70.0f); // NOT GOOD
    _scrollBar.SetSliderDirection(SLIDER_DIRECTION_VERTICAL);
    _scrollBar.SetParent(this);
    _scrollBar.SetSliderValue(0.4f);
    _scrollBar.SetSliderValueChangedCallbackContext(this);
    _scrollBar.SetSliderValueChangedCallback(ScrollbarValueChangedCallback);
}

LayoutPanel::~LayoutPanel()
{

}

void LayoutPanel::SetLayoutType(LayoutType type)
{
    _layoutType = type;

    //Do some scrollbar recalc.
    if(_layoutType == LAYOUT_FLOW || _layoutType == LAYOUT_VERTICAL)
    {
        _scrollBar.SetMargin(MARGIN_LEFT,0.0f,ValueType::AUTO);
        _scrollBar.SetMargin(MARGIN_TOP,0.0f);
        _scrollBar.SetWidth(_scrollBarSizeDefinition.value,_scrollBarSizeDefinition.type);
        _scrollBar.SetSliderDirection(SLIDER_DIRECTION_VERTICAL);
    }
    else
    {
        _scrollBar.SetMargin(MARGIN_TOP,0.0f,ValueType::AUTO);
        _scrollBar.SetMargin(MARGIN_LEFT,0.0f);
        _scrollBar.SetHeight(_scrollBarSizeDefinition.value,_scrollBarSizeDefinition.type);
        _scrollBar.SetSliderDirection(SLIDER_DIRECTION_HORIZONTAL);
    }

    // Call Recalculation 
    FullyRecalculateChildren();
}

void LayoutPanel::SetChildrenWidth(float value, ValueType type)
{
    _childrenWidthDefinition.type = type;
    _childrenWidthDefinition.value = value;

    //...
    FullyRecalculateChildren();
}

void LayoutPanel::SetChildrenHeight(float value, ValueType type)
{
    _childrenHeightDefinition.type = type;
    _childrenHeightDefinition.value = value;

    //...
    FullyRecalculateChildren();
}

void LayoutPanel::SetPadding(PaddingType paddingType, float value, ValueType type)
{
    _paddingDefinitions[paddingType].type = type;
    _paddingDefinitions[paddingType].value = value;

    //...
    FullyRecalculateChildren();
}

void LayoutPanel::SetGapWidth(float value, ValueType type)
{
    _gapWidthDefinition = {value,type};

    //...
    FullyRecalculateChildren();
}

void LayoutPanel::SetGapHeight(float value, ValueType type)
{
    _gapHeightDefinition = {value,type};

    //...
    FullyRecalculateChildren();
}



void LayoutPanel::CalculateChildrenBox()
{

    if(guiContext == nullptr)
    {
        return;
    }

    SetLayoutObstructedOff();

    glm::vec2 windowRes = glm::vec2(guiContext->windowWidth,guiContext->windowHeight);

    glm::vec2 ownMinFixed = _box.min * windowRes;
    glm::vec2 ownMaxFixed = _box.max * windowRes;
    float ownWidthFixed = ownMaxFixed.x - ownMinFixed.x;
    float ownHeightFixed = ownMaxFixed.y - ownMinFixed.y;
    glm::vec2 ownMin = _box.min;
    glm::vec2 ownMax = _box.max;
    float ownWidth = ownMax.x-ownMin.x;
    float ownHeight = ownMax.y-ownMin.y;

    AABB newChildrenBoxFixed = {glm::vec2(0,0),glm::vec2(0,0)};

    

    if(_layoutType == LAYOUT_VERTICAL)
    {
        /*
        _childrenBox here basically only cares about:
        - width / height
        - gapHeight
        - padding left/right
        */

        float scrollBarWidthFixed = 0.0f;
        if(_scrollBarSizeDefinition.type != ValueType::AUTO && isScrollable)
        {
            scrollBarWidthFixed = CalculateValuebasedOnType(_scrollBarSizeDefinition.type,_scrollBarSizeDefinition.value,ownWidthFixed);
        }


        if(_paddingDefinitions[PADDING_LEFT].type != ValueType::AUTO)
        {
            float calculatedPaddingValue = CalculateValuebasedOnType(_paddingDefinitions[PADDING_LEFT].type,
                _paddingDefinitions[PADDING_LEFT].value,ownWidthFixed);
            newChildrenBoxFixed.min.x = ownMinFixed.x + calculatedPaddingValue;
        }

        if(_paddingDefinitions[PADDING_RIGHT].type != ValueType::AUTO)
        {
            float calculatedPaddingValue = CalculateValuebasedOnType(_paddingDefinitions[PADDING_RIGHT].type,
                _paddingDefinitions[PADDING_RIGHT].value,ownWidthFixed);

            newChildrenBoxFixed.max.x = ownMaxFixed.x - calculatedPaddingValue - scrollBarWidthFixed;
        }

        if(_paddingDefinitions[PADDING_TOP].type != ValueType::AUTO)
        {
            float calculatedPaddingValue = CalculateValuebasedOnType(_paddingDefinitions[PADDING_TOP].type,
                _paddingDefinitions[PADDING_TOP].value,ownHeightFixed);

            newChildrenBoxFixed.min.y = ownMinFixed.y + calculatedPaddingValue;

            paddingFrontRelative = calculatedPaddingValue / windowRes.y;
        }
        else
        {
            newChildrenBoxFixed.min.y = ownMinFixed.y;

            paddingFrontRelative = 0.0f;
        }

        if(_paddingDefinitions[PADDING_BOTTOM].type != ValueType::AUTO)
        {
            float calculatedPaddingValue = CalculateValuebasedOnType(_paddingDefinitions[PADDING_BOTTOM].type,
                _paddingDefinitions[PADDING_BOTTOM].value,ownHeightFixed);

            paddingEndRelative = calculatedPaddingValue / windowRes.y;
        }
        else
        {
            paddingEndRelative = 0.0f;
        }
        
        float calculatedFixedWidthValue = 0.0f;

        if(_childrenWidthDefinition.type != ValueType::AUTO)
        {
            calculatedFixedWidthValue= CalculateValuebasedOnType(_childrenWidthDefinition.type,_childrenWidthDefinition.value,ownWidthFixed);

            childrenRelativeWidth = calculatedFixedWidthValue / windowRes.x;
            /*
            If both padding definitions are ValueType::AUTO or not ValueType::AUTO at the same time, then we allign the children towards the middle.
            This is because with a widthdefinition in place, we can not allow to have padding definitions on both sides.
            */
            if((_paddingDefinitions[PADDING_LEFT].type == ValueType::AUTO && _paddingDefinitions[PADDING_RIGHT].type == ValueType::AUTO) ||
                (_paddingDefinitions[PADDING_LEFT].type != ValueType::AUTO && _paddingDefinitions[PADDING_RIGHT].type != ValueType::AUTO))
            {
                float centerValueFixed = ownMinFixed.x + ownWidthFixed * 0.5f;
                
                newChildrenBoxFixed.min.x = centerValueFixed - calculatedFixedWidthValue * 0.5f;
                newChildrenBoxFixed.max.x = centerValueFixed + calculatedFixedWidthValue * 0.5f;
            }
            else if(_paddingDefinitions[PADDING_LEFT].type != ValueType::AUTO)
            {
                float calculatedPaddingValue = CalculateValuebasedOnType(_paddingDefinitions[PADDING_LEFT].type,
                _paddingDefinitions[PADDING_LEFT].value,ownWidthFixed);
                //newChildrenBoxFixed.min.x = ownMinFixed.x + calculatedPaddingValue;
                //std::cout<<"Enterred here!\n";
                newChildrenBoxFixed.max.x = ownMinFixed.x + calculatedPaddingValue + calculatedFixedWidthValue;

            }
            else if(_paddingDefinitions[PADDING_RIGHT].type != ValueType::AUTO)
            {
                float calculatedPaddingValue = CalculateValuebasedOnType(_paddingDefinitions[PADDING_RIGHT].type,
                _paddingDefinitions[PADDING_RIGHT].value,ownWidthFixed);
                //newChildrenBoxFixed.max.x = ownMaxFixed.x - calculatedPaddingValue;
                newChildrenBoxFixed.min.x = ownMaxFixed.x - calculatedPaddingValue - calculatedFixedWidthValue - scrollBarWidthFixed;
            }
        }
        else
        {
            //If no width is set the childrenbox has been previously calculated by the paddings, however we still have to querry the relativewidth
            calculatedFixedWidthValue= (ownMaxFixed.x - ownMinFixed.x - scrollBarWidthFixed);
            
            childrenRelativeWidth = calculatedFixedWidthValue / windowRes.x;

            newChildrenBoxFixed.min.x = ownMinFixed.x;
            newChildrenBoxFixed.max.x = ownMinFixed.x + calculatedFixedWidthValue;
        }

        /*
        Now we calculate the Y coords of the children box's max.
        For this, we have to calculate some components like: gapHeight, children Height
        */
        float childrenHeightFixed = 0.0f;
        if(_childrenHeightDefinition.type != ValueType::AUTO)
        {
            childrenHeightFixed = CalculateValuebasedOnType(_childrenHeightDefinition.type,_childrenHeightDefinition.value,ownHeightFixed);

            childrenRelativeHeight = childrenHeightFixed / windowRes.y;
        }
        else
        {
            //We set the obstructed flag if height isnt set.
            SetLayoutObstructedOn();
        }

        float childrenHeightGapFixed = 0.0f;
        if(_gapHeightDefinition.type != ValueType::AUTO)
        {
            childrenHeightGapFixed = CalculateValuebasedOnType(_gapHeightDefinition.type,_gapHeightDefinition.value,ownHeightFixed);

            
        }
        gapRelativeHeight = childrenHeightGapFixed / windowRes.y;

        //Now, we have the necessary data to calculate the full height of the childrenbox
        int childrenCount = _children.size();
        float totalHeightFixed = childrenCount * childrenHeightFixed + (childrenCount-1)*childrenHeightGapFixed;

        newChildrenBoxFixed.max.y = newChildrenBoxFixed.min.y + totalHeightFixed;
    }
    else if(_layoutType == LAYOUT_HORIZONTAL)
    {
        /*
        _childrenBox here basically only cares about:
        - width / height
        - gapWidth
        - padding left/right
        */

        
        float scrollBarHeightFixed = 0.0f;
        if(_scrollBarSizeDefinition.type != ValueType::AUTO && isScrollable)
        {
            scrollBarHeightFixed = CalculateValuebasedOnType(_scrollBarSizeDefinition.type,_scrollBarSizeDefinition.value,ownHeightFixed);
        }


        if(_paddingDefinitions[PADDING_TOP].type != ValueType::AUTO)
        {
            float calculatedPaddingValue = CalculateValuebasedOnType(_paddingDefinitions[PADDING_TOP].type,
                _paddingDefinitions[PADDING_TOP].value,ownHeightFixed);
            newChildrenBoxFixed.min.y = ownMinFixed.y + calculatedPaddingValue;
        }

        if(_paddingDefinitions[PADDING_BOTTOM].type != ValueType::AUTO)
        {
            float calculatedPaddingValue = CalculateValuebasedOnType(_paddingDefinitions[PADDING_BOTTOM].type,
                _paddingDefinitions[PADDING_BOTTOM].value,ownHeightFixed);

            newChildrenBoxFixed.max.y = ownMaxFixed.y - calculatedPaddingValue - scrollBarHeightFixed;
        }

        if(_paddingDefinitions[PADDING_LEFT].type != ValueType::AUTO)
        {
            float calculatedPaddingValue = CalculateValuebasedOnType(_paddingDefinitions[PADDING_LEFT].type,
                _paddingDefinitions[PADDING_LEFT].value,ownWidthFixed);

            newChildrenBoxFixed.min.x = ownMinFixed.x + calculatedPaddingValue;

            paddingFrontRelative = calculatedPaddingValue / windowRes.x;
        }
        else
        {
            newChildrenBoxFixed.min.x = ownMinFixed.x;

            paddingFrontRelative = 0.0f;
        }

        if(_paddingDefinitions[PADDING_RIGHT].type != ValueType::AUTO)
        {
            float calculatedPaddingValue = CalculateValuebasedOnType(_paddingDefinitions[PADDING_RIGHT].type,
                _paddingDefinitions[PADDING_RIGHT].value,ownWidthFixed);

            paddingEndRelative = calculatedPaddingValue / windowRes.x;
        }
        else
        {
            paddingEndRelative = 0.0f;
        }
        
        float calculatedFixedHeightValue = 0.0f;

        if(_childrenHeightDefinition.type != ValueType::AUTO)
        {
            calculatedFixedHeightValue= CalculateValuebasedOnType(_childrenHeightDefinition.type,_childrenHeightDefinition.value,ownHeightFixed);

            childrenRelativeHeight = calculatedFixedHeightValue / windowRes.y;

            if((_paddingDefinitions[PADDING_TOP].type == ValueType::AUTO && _paddingDefinitions[PADDING_TOP].type == ValueType::AUTO) ||
                (_paddingDefinitions[PADDING_BOTTOM].type != ValueType::AUTO && _paddingDefinitions[PADDING_BOTTOM].type != ValueType::AUTO))
            {
                float centerValueFixed = ownMinFixed.y + ownHeightFixed * 0.5f;
                
                newChildrenBoxFixed.min.y = centerValueFixed - calculatedFixedHeightValue * 0.5f;
                newChildrenBoxFixed.max.y = centerValueFixed + calculatedFixedHeightValue * 0.5f;
            }
            else if(_paddingDefinitions[PADDING_TOP].type != ValueType::AUTO)
            {
                float calculatedPaddingValue = CalculateValuebasedOnType(_paddingDefinitions[PADDING_TOP].type,
                _paddingDefinitions[PADDING_TOP].value,ownHeightFixed);                
                newChildrenBoxFixed.max.y = ownMinFixed.y + calculatedPaddingValue + calculatedFixedHeightValue;
            }
            else if(_paddingDefinitions[PADDING_BOTTOM].type != ValueType::AUTO)
            {
                float calculatedPaddingValue = CalculateValuebasedOnType(_paddingDefinitions[PADDING_BOTTOM].type,
                _paddingDefinitions[PADDING_BOTTOM].value,ownHeightFixed);
                newChildrenBoxFixed.min.y = ownMaxFixed.y - calculatedPaddingValue - calculatedFixedHeightValue - scrollBarHeightFixed;
            }
        }
        else
        {
            childrenRelativeHeight = (newChildrenBoxFixed.max.y - newChildrenBoxFixed.min.y) / windowRes.y;
        }

        float childrenWidthFixed = 0.0f;
        if(_childrenWidthDefinition.type != ValueType::AUTO)
        {
            childrenWidthFixed = CalculateValuebasedOnType(_childrenWidthDefinition.type,_childrenWidthDefinition.value,ownWidthFixed);

            childrenRelativeWidth = childrenWidthFixed / windowRes.x;
        }
        else
        {
            SetLayoutObstructedOn();
        }

        float childrenWidthGapFixed = 0.0f;
        if(_gapWidthDefinition.type != ValueType::AUTO)
        {
            childrenWidthGapFixed = CalculateValuebasedOnType(_gapWidthDefinition.type,_gapWidthDefinition.value,ownWidthFixed);

            
        }
        gapRelativeWidth = childrenWidthGapFixed / windowRes.x;

        //Now, we have the necessary data to calculate the full height of the childrenbox
        int childrenCount = _children.size();
        float totalWidthFixed = childrenCount * childrenWidthFixed + (childrenCount-1)*childrenWidthGapFixed;

        newChildrenBoxFixed.max.x = newChildrenBoxFixed.min.x + totalWidthFixed;
    }
    else if(_layoutType == LAYOUT_FLOW)
    {

        float scrollBarWidthFixed = 0.0f;
        if(_scrollBarSizeDefinition.type != ValueType::AUTO && isScrollable)
        {
            scrollBarWidthFixed = CalculateValuebasedOnType(_scrollBarSizeDefinition.type,_scrollBarSizeDefinition.value,ownWidthFixed);
        }


        if(_paddingDefinitions[PADDING_LEFT].type != ValueType::AUTO)
        {
            float calculatedPaddingValue = CalculateValuebasedOnType(_paddingDefinitions[PADDING_LEFT].type,
                _paddingDefinitions[PADDING_LEFT].value,ownWidthFixed);
            newChildrenBoxFixed.min.x = ownMinFixed.x + calculatedPaddingValue;
        }
        else
        {
            newChildrenBoxFixed.min.x = ownMinFixed.x;
        }

        if(_paddingDefinitions[PADDING_RIGHT].type != ValueType::AUTO)
        {
            float calculatedPaddingValue = CalculateValuebasedOnType(_paddingDefinitions[PADDING_RIGHT].type,
                _paddingDefinitions[PADDING_RIGHT].value,ownWidthFixed);

            newChildrenBoxFixed.max.x = ownMaxFixed.x - calculatedPaddingValue - scrollBarWidthFixed;
        }
        else
        {
            newChildrenBoxFixed.max.x = ownMaxFixed.x - scrollBarWidthFixed;
        }

        if(_paddingDefinitions[PADDING_TOP].type != ValueType::AUTO)
        {
            float calculatedPaddingValue = CalculateValuebasedOnType(_paddingDefinitions[PADDING_TOP].type,
                _paddingDefinitions[PADDING_TOP].value,ownHeightFixed);

            newChildrenBoxFixed.min.y = ownMinFixed.y + calculatedPaddingValue;

            paddingFrontRelative = calculatedPaddingValue / windowRes.y;
        }
        else
        {
            newChildrenBoxFixed.min.y = ownMinFixed.y;

            paddingFrontRelative = 0.0f;
        }

        if(_paddingDefinitions[PADDING_BOTTOM].type != ValueType::AUTO)
        {
            float calculatedPaddingValue = CalculateValuebasedOnType(_paddingDefinitions[PADDING_BOTTOM].type,
                _paddingDefinitions[PADDING_BOTTOM].value,ownHeightFixed);

            paddingEndRelative = calculatedPaddingValue / windowRes.y;
        }
        else
        {
            paddingEndRelative = 0.0f;
        }
        
        float calculatedFixedWidthValue = 0.0f;
        if(_childrenWidthDefinition.type != ValueType::AUTO)
        {
            calculatedFixedWidthValue= CalculateValuebasedOnType(_childrenWidthDefinition.type,_childrenWidthDefinition.value,ownWidthFixed);

            childrenRelativeWidth = calculatedFixedWidthValue / windowRes.x;
        }
        else
        {
            SetLayoutObstructedOn();
        }

        /*
        Now we calculate the Y coords of the children box's max.
        For this, we have to calculate some components like: gapHeight, children Height
        */
        float childrenHeightFixed = 0.0f;
        if(_childrenHeightDefinition.type != ValueType::AUTO)
        {
            childrenHeightFixed = CalculateValuebasedOnType(_childrenHeightDefinition.type,_childrenHeightDefinition.value,ownHeightFixed);

            childrenRelativeHeight = childrenHeightFixed / windowRes.y;
        }
        else
        {
            //We set the obstructed flag if height isnt set.
            SetLayoutObstructedOn();
        }

        
        float childrenGapWidthFixed = 0.0f;
        if(_gapWidthDefinition.type != ValueType::AUTO)
        {
            childrenGapWidthFixed = CalculateValuebasedOnType(_gapWidthDefinition.type,_gapWidthDefinition.value,ownWidthFixed);
        }
        gapRelativeWidth = childrenGapWidthFixed / windowRes.x;

        float childrenHeightGapFixed = 0.0f;
        if(_gapHeightDefinition.type != ValueType::AUTO)
        {
            childrenHeightGapFixed = CalculateValuebasedOnType(_gapHeightDefinition.type,_gapHeightDefinition.value,ownHeightFixed);
        }
        gapRelativeHeight = childrenHeightGapFixed / windowRes.y;

        // Now, we have the necessary data to calculate the full height of the childrenbox
        // This is in FLOW state, so we have to calculate how many children can fit into one row, and then the rest
        int childrenCount = _children.size();

        float newChildrenBoxWidth = (newChildrenBoxFixed.max.x - newChildrenBoxFixed.min.x);

        if(newChildrenBoxWidth < calculatedFixedWidthValue)
        {
            SetLayoutObstructedOn();
            return;
        }
        
        float floatChildrenPerRow = (childrenGapWidthFixed + newChildrenBoxWidth) / (childrenGapWidthFixed + calculatedFixedWidthValue); 
        int childrenPerRow =  (int)floatChildrenPerRow;
        int rowCount =  ((childrenCount-1) / childrenPerRow) + 1;

        float totalHeightFixed = rowCount *childrenHeightFixed + (rowCount - 1) * childrenHeightGapFixed;

        newChildrenBoxFixed.max.y = newChildrenBoxFixed.min.y + totalHeightFixed;
    }

    if(newChildrenBoxFixed.min.x >= newChildrenBoxFixed.max.x || newChildrenBoxFixed.min.y >= newChildrenBoxFixed.max.y)
    {
        SetLayoutObstructedOn();
    }
    
    // Finally, we normalize the childrenbox.

    _baseChildrenBox.min = newChildrenBoxFixed.min / windowRes;
    _baseChildrenBox.max = newChildrenBoxFixed.max / windowRes;

    _actualChildrenBox.min = _baseChildrenBox.min;
    _actualChildrenBox.max = _baseChildrenBox.max;

    _scrollBar.SetSliderValue(0.0f);

    

    //std::cout<<(_actualChildrenBox.max.x - _actualChildrenBox.min.x ) * windowRes.x << " New CHildContainerWidth\n";

    CalculateInterpolatingBorders();

    // Dont work when the layoutPanel's minmax is set, and it cant resize on its own. IDK why.
    if(!isLayoutObstructed && isScrollable)
    {
        float newSliderHeadSizeRelative = 0.1f;
        if(_layoutType == LAYOUT_FLOW || _layoutType == LAYOUT_VERTICAL)
        {
            newSliderHeadSizeRelative = (_box.max.y - _box.min.y) / (_baseChildrenBox.max.y - _baseChildrenBox.min.y);
        }
        else
        {
            newSliderHeadSizeRelative = (_box.max.x - _box.min.x) / (_baseChildrenBox.max.x - _baseChildrenBox.min.x);
        }
        //std::cout<<newSliderHeadSizeRelative<<"\n";
        _scrollBar.SetSliderHeadSize(newSliderHeadSizeRelative,ValueType::RELATIVE);
    }
    
}

void LayoutPanel::CalculateInterpolatingBorders()
{
    SetScrollableOn();

    
    if(_layoutType == LAYOUT_VERTICAL || _layoutType == LAYOUT_FLOW)
    {
        float childrenBoxHeightRelative = _baseChildrenBox.max.y - _baseChildrenBox.min.y;
        float ownHeightRelative = _box.max.y - _box.min.y;
        float ownHeightMinusPadding = ownHeightRelative - paddingFrontRelative - paddingEndRelative;
        if(ownHeightMinusPadding >= childrenBoxHeightRelative ) 
        {
            SetScrollableOff();
            return;
        }

        float childrenBoxScrollableHeightRelative = childrenBoxHeightRelative - ownHeightMinusPadding;

        interpolateZeroValue = _baseChildrenBox.min.y;
        interpolateOneValue = _baseChildrenBox.min.y - childrenBoxScrollableHeightRelative;
    }
    else if(_layoutType == LAYOUT_HORIZONTAL)
    {                
        // [TODO]
        float childrenBoxWidthRelative = _baseChildrenBox.max.x - _baseChildrenBox.min.x;
        float ownWidthRelative = _box.max.x - _box.min.x;
        float ownWidthMinusPadding = ownWidthRelative - paddingFrontRelative - paddingEndRelative;
        if(ownWidthMinusPadding >= childrenBoxWidthRelative ) 
        {
            SetScrollableOff();
            return;
        }

        float childrenBoxScrollableWidthRelative = childrenBoxWidthRelative - ownWidthMinusPadding;

        interpolateZeroValue = _baseChildrenBox.min.x;
        interpolateOneValue = _baseChildrenBox.min.x - childrenBoxScrollableWidthRelative;
    }
}

void LayoutPanel::CalculateChildrenPositions()
{
    if(guiContext == nullptr || isLayoutObstructed) return;
    /*
    Invariant: 
        we assume that the _childrenBox is fully calculated, and ready to be used.
        we assume that the four needed childrenDatas: ValueType::RELATIVE width/height/gapWidth/gapheight have been calculated before
    */

    //This is only used in Flow state
    float childrenBoxWidth = _actualChildrenBox.max.x - _actualChildrenBox.min.x;
    float childrenPerRowFloat = (gapRelativeWidth + childrenBoxWidth) / (gapRelativeWidth + childrenRelativeWidth); 
    int childrenPerRow = (int)childrenPerRowFloat;

    

    glm::vec2 currentChildMinPointer = _actualChildrenBox.min;

    
    
    for(int i=0;i<_children.size();++i)
    {
        glm::vec2 currentChildMaxPointer = currentChildMinPointer + glm::vec2(childrenRelativeWidth,childrenRelativeHeight);
        _children[i]->SetRelativePlacement(currentChildMinPointer,currentChildMaxPointer);

        //Since we have changed the data of this child, we have to call a resize event on it.
        

        switch(_layoutType)
        {
            case LAYOUT_VERTICAL:
                currentChildMinPointer += glm::vec2(0.0f,gapRelativeHeight + childrenRelativeHeight);
                break;
            case LAYOUT_HORIZONTAL:
                currentChildMinPointer += glm::vec2(gapRelativeWidth + childrenRelativeWidth,0.0f);
                break;
            case LAYOUT_FLOW:
                //line break
                if(i%childrenPerRow == (childrenPerRow -1))
                {
                    int rowCount = (i+1) / childrenPerRow;
                    float rowDepth  =rowCount * childrenRelativeHeight + (rowCount)*gapRelativeHeight;
                    currentChildMinPointer = _actualChildrenBox.min + glm::vec2(0.0f,rowDepth);

                }
                else //normal behaviour
                {
                    currentChildMinPointer += glm::vec2(gapRelativeWidth + childrenRelativeWidth,0.0f);
                }
                break;
        }

        _children[i]->Resize();
    }
}

void LayoutPanel::MoveChildrenBox(float amount)
{
    //std::cout<<isScrollable<<"\n";
    if(guiContext == nullptr || !isScrollable || isLayoutObstructed) return;

    float newT = 0.0f;
    if(_layoutType == LAYOUT_VERTICAL || _layoutType == LAYOUT_FLOW)
    {
        

        float onePixelInRelative = 1.0f / guiContext->windowHeight;
        float placeToMoveMinRelative = _actualChildrenBox.min.y + amount * onePixelInRelative;

        newT =( interpolateZeroValue - placeToMoveMinRelative) / (interpolateZeroValue-interpolateOneValue);

        

        
    }
    else if(_layoutType == LAYOUT_HORIZONTAL)
    {
        float onePixelInRelative = 1.0f / guiContext->windowWidth;
        float placeToMoveMinRelative = _actualChildrenBox.min.x + amount * onePixelInRelative;

        newT =( interpolateZeroValue - placeToMoveMinRelative) / (interpolateZeroValue-interpolateOneValue);
    }

    //std::cout<<newT<<"\n";
    SetScrollValue(newT);
    
}

void LayoutPanel::SetScrollValue(float value)
{
    if(isLayoutObstructed) return;

    if(!isScrollable)
    {
       scrollValue = 0.0f;
       return; 
    }

    if(value < 0.0f)
    {
        value = 0.0f;
    }
    else if(value > 1.0f)
    {
        value = 1.0f;
    }


    _scrollBar.SetSliderValue(value);

    

    scrollValue = value;
    
    if(_layoutType == LAYOUT_VERTICAL || _layoutType == LAYOUT_FLOW)
    {
        float newMinValue = (1-value)*interpolateZeroValue + value *interpolateOneValue;
        _actualChildrenBox.min.y = newMinValue;

        _actualChildrenBox.max.y = newMinValue + (_baseChildrenBox.max.y - _baseChildrenBox.min.y);
    }
    else if(_layoutType == LAYOUT_HORIZONTAL) 
    {
        float newMinValue = (1-value)*interpolateZeroValue + value *interpolateOneValue;
        _actualChildrenBox.min.x = newMinValue;

        _actualChildrenBox.max.x = newMinValue + (_baseChildrenBox.max.x - _baseChildrenBox.min.x);
    }

    
    CalculateChildrenPositions();
}

void LayoutPanel::SetScrollbarSize(float value, ValueType type)
{
    _scrollBarSizeDefinition = {value, type};
    if(_layoutType == LAYOUT_VERTICAL || _layoutType == LAYOUT_FLOW)
    {
        _scrollBar.SetWidth(value,type);
    }
    else
    {
        _scrollBar.SetHeight(value,type);
    }

    _scrollBar.Resize();

    FullyRecalculateChildren();
}

void LayoutPanel::FullyRecalculateChildren()
{
    CalculateChildrenBox();

    

    CalculateChildrenPositions();
}

void LayoutPanel::LayoutPanelRender()
{
    CalculateRenderBox();
	
    Rectangle ndcControlRenderRect = {_renderBox.min.x,_renderBox.min.y,
									  _renderBox.max.x - _renderBox.min.x,
									  _renderBox.max.y-_renderBox.min.y};

	guiContext->renderer.RenderRectangle(ndcControlRenderRect,_baseBGColor);

    if(!isLayoutObstructed)
    {
        for(int i=0; i < _children.size() ; ++i)
        {
            _children[i]->Render();
        }
    }
    
    _scrollBar.Render();
}

void LayoutPanel::LayoutPanelResize()
{
    ControlResize();
    _scrollBar.Resize();

    CalculateRenderBox();

    FullyRecalculateChildren();
}

bool LayoutPanel::LayoutPanelClick(int button, int action)
{

    bool isContainerBaseClicked = false;
    if(!isLayoutObstructed)
    {
        isContainerBaseClicked = ContainerBaseClick(button,action);
    }
    
    bool isScrollbarClicked = _scrollBar.Click(button,action);

    return (isScrollbarClicked || isContainerBaseClicked);
}

void LayoutPanel::LayoutPanelMouseWheel(float amount, int direction)
{
    if(isHovered)
    {
        MoveChildrenBox(2.0f * direction);
        CalculateChildrenPositions();
    }
}

bool LayoutPanel::LayoutPanelMouseMove()
{
    bool didContainerBaseHandleMouseMove = ContainerBaseMouseMove();

    bool didScrollbarHandleMouseMove = false;
    if(!isLayoutObstructed && isScrollable)
    {
        didScrollbarHandleMouseMove = _scrollBar.MouseMove();
    }

    return (didScrollbarHandleMouseMove || didContainerBaseHandleMouseMove);
}

void LayoutPanel::LayoutPanelAddControl(Control *control)
{
    ContainerBaseAddControl(control);

    FullyRecalculateChildren();
}

void LayoutPanel::SubControlSetGuicontext(GuiContext *guiContext)
{
    _scrollBar.SetGuiContext(guiContext);
    _scrollBar.Resize();

    

    for(int i=0;i<_children.size();++i)
    {
        _children[i]->SetGuiContext(guiContext);
    }

    FullyRecalculateChildren();
}
