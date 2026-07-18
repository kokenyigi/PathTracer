#include "Dropdown.h"

#include <iostream>

Dropdown::Dropdown()
{
    _chosenOptionLabel.SetHeight(0.5f,ValueType::RELATIVE);
    _chosenOptionLabel.SetMargin(MARGIN_LEFT,5.0f,ValueType::FIXED);
    //_chosenOptionLabel.SetMargin(MARGIN_RIGHT,5.0f);
    _chosenOptionLabel.SetTextAllignment(TEXT_ALLIGNMENT_LEFT);
    _chosenOptionLabel.SetParent(this);
	_chosenOptionLabel.SetText("none");

	_optionsPanel.SetScrollbarSize(20.0f);
    _optionsPanel.SetMargin(MARGIN_BOTTOM,0.0f,ValueType::AUTO);
	_optionsPanel.SetMargin(MARGIN_LEFT,0.0f,ValueType::AUTO);
	_optionsPanel.SetMargin(MARGIN_RIGHT,0.0f,ValueType::AUTO);
    _optionsPanel.SetMargin(MARGIN_TOP,0.0f,ValueType::AUTO);
	_optionsPanel.SetHeight(0.0f,ValueType::AUTO);
    _optionsPanel.SetWidth(0.0f,ValueType::AUTO);
	_optionsPanel.SetBGColor(0,0,0.0);
	_optionsPanel.SetChildrenHeight(0.2f,ValueType::RELATIVE);//no good
	_optionsPanel.SetChildrenWidth(0.0f,ValueType::AUTO);
	_optionsPanel.SetLayoutType(LAYOUT_VERTICAL);
	_optionsPanel.SetGapHeight(0.0f,ValueType::AUTO);
	_optionsPanel.SetGapWidth(0.0f,ValueType::AUTO);
	_optionsPanel.SetPadding(PADDING_RIGHT,0.0f,ValueType::AUTO);
	_optionsPanel.SetPadding(PADDING_LEFT,0.0f,ValueType::AUTO);
	_optionsPanel.SetPadding(PADDING_TOP,0.0f,ValueType::AUTO);
	_optionsPanel.SetPadding(PADDING_BOTTOM,0.0f,ValueType::AUTO);
	_optionsPanel.SetParent(this);
	

    _optionsPanel.SetInactive();

	_optionsGroup.SetCallback(OptionButtonCallback);

	SetPriority(1); // Dropdowns have a priority of one
}

void Dropdown::SetChosenOption(int index)
{
	if(index >= 0 && index < _currentOptionCount)
	{
		RadioButton* indexedOption = (RadioButton*)_optionsPanel.GetChildren()[index];
		indexedOption->SetToggledOn();
		_chosenOptionLabel.SetText(indexedOption->GetText());
	}
}

void Dropdown::ClearOptions()
{
	_optionsGroup.SetToggledOff();
	for(int i = 0; i<_optionsPanel.GetChildren().size() ;++i)
	{
		delete _optionsPanel.GetChildren()[i];
	}
	_optionsPanel.GetChildren().clear();

	_currentOptionCount = 0;
	_chosenOptionLabel.SetText("-");
}

void Dropdown::AddOption(const std::string &optionName, int optionIndex)
{
	RadioButton* optionRadioButton = new RadioButton();
	optionRadioButton->SetText(optionName);
	optionRadioButton->SetIndex(optionIndex);
	optionRadioButton->SetCallBackContext(this);
	optionRadioButton->SetTextAllignment(TEXT_ALLIGNMENT_LEFT);
	optionRadioButton->SetTextColor(_optionTextColor.x,_optionTextColor.y,_optionTextColor.z);
	optionRadioButton->SetBGColor(_optionBaseColor.x,_optionBaseColor.y,_optionBaseColor.z);
	optionRadioButton->SetHoverColor(_optionHoverColor.x,_optionHoverColor.y,_optionHoverColor.z);
	optionRadioButton->SetClickColor(_optionClickColor.x,_optionClickColor.y,_optionClickColor.z);
	

	_optionsGroup.AddToGroup(optionRadioButton);
	_optionsPanel.AddControl(optionRadioButton);

	
	

	_currentOptionCount++;

	if(_currentOptionCount > _maxVisibleOptionCount)
	{
		_optionsPanel.SetChildrenHeight(1.0f / (float)_maxVisibleOptionCount,ValueType::RELATIVE);
	}
	else
	{
		_optionsPanel.SetChildrenHeight((1.0f / (float)_currentOptionCount),ValueType::RELATIVE);
	}

	Resize();
}

void Dropdown::OptionButtonCallback(void *context, int index)
{
	Dropdown* dropdown = (Dropdown*)context;

	if(dropdown->_callbackContext != nullptr && dropdown->_callback != nullptr)
	{
		dropdown->_callback(dropdown->_callbackContext,index);

		
	}

	RadioButton* currentlyPressedRadioButton = dropdown->_optionsGroup._currentToggled;

	dropdown->_chosenOptionLabel.SetText(currentlyPressedRadioButton->GetText());

	dropdown->_isOpen = false;
	dropdown->_optionsPanel.SetInactive();
}

void Dropdown::SetOptionTextColor(float r, float g, float b)
{
	_optionTextColor = glm::vec3(r,g,b);

	std::vector<Control*>& options = _optionsPanel.GetChildren();
	for(int i=0;i<options.size();++i)
	{
		RadioButton* radio = (RadioButton*)options[i];
		radio->SetTextColor(r,g,b);
	}
}

void Dropdown::SetOptionBaseColor(float r, float g, float b)
{
	_optionBaseColor = glm::vec3(r,g,b);

	std::vector<Control*>& options = _optionsPanel.GetChildren();
	for(int i=0;i<options.size();++i)
	{
		RadioButton* radio = (RadioButton*)options[i];
		radio->SetBGColor(r,g,b);
	}
}

void Dropdown::SetOptionHoveredColor(float r, float g, float b)
{
	_optionHoverColor = glm::vec3(r,g,b);

	std::vector<Control*>& options = _optionsPanel.GetChildren();
	for(int i=0;i<options.size();++i)
	{
		RadioButton* radio = (RadioButton*)options[i];
		radio->SetHoverColor(r,g,b);
	}
}

void Dropdown::SetOptionClickedColor(float r, float g, float b)
{
	_optionClickColor = glm::vec3(r,g,b);

	std::vector<Control*>& options = _optionsPanel.GetChildren();
	for(int i=0;i<options.size();++i)
	{
		RadioButton* radio = (RadioButton*)options[i];
		radio->SetClickColor(r,g,b);
	}
}

void Dropdown::DropdownResize()
{
	//std::cout<<"Begin resize\n";
    ControlResize();
    _chosenOptionLabel.Resize();

	CalculateRenderBox();

	_ownBox = _box;
    _ownRenderBox = _renderBox;

	
    

    

    float ownBoxHeightRelative = _box.max.y - _box.min.y;
    _box.max.y = _box.min.y + ((_maxVisibleOptionCount < _currentOptionCount? _maxVisibleOptionCount : _currentOptionCount) + 1) *
	 ownBoxHeightRelative;

    CalculateRenderBox();

	if(_currentOptionCount > 0)
	{
		_optionsPanel.SetRelativePlacement(_ownBox.min + glm::vec2(0,ownBoxHeightRelative),_box.max);
		
		//_optionsPanel.SetMargin(MARGIN_TOP,1.0f / (1.0f + _maxVisibleOptionCount),RELATIVE);

    	_optionsPanel.Resize();
	}

    //std::cout<<"Resize Completed\n";
}

void Dropdown::DropdownRender()
{
	//std::cout<<"Begin render\n";
    Rectangle ndcControlRenderRect = {_ownRenderBox.min.x,_ownRenderBox.min.y,
									  _ownRenderBox.max.x - _ownRenderBox.min.x,
									  _ownRenderBox.max.y-_ownRenderBox.min.y};
	if(isClicked && !_isOpen)
	{
		_renderColor = _clickBGColor;
	}
	else if(isHovered && !_isOpen)
	{
		_renderColor = _hoverBGColor;
	}
	else
	{
		_renderColor = _baseBGColor;
	}

	


	guiContext->renderer.RenderRectangle(ndcControlRenderRect,_renderColor);

	_chosenOptionLabel.Render();
//std::cout<<"End render\n";
	_optionsPanel.Render();
	
}

bool Dropdown::DropdownMouseMove()
{

	bool isCurrentMouseOnOwnRenderBox = false;

	if(guiContext == nullptr)
	{
		isCurrentMouseOnOwnRenderBox = false;
	}

	glm::vec2 cursorPosRelative = guiContext->currentMousePos / glm::vec2(guiContext->windowWidth,guiContext->windowHeight);

	if(cursorPosRelative.x >= _ownRenderBox.min.x && cursorPosRelative.x <= _ownRenderBox.max.x &&
		   cursorPosRelative.y >= _ownRenderBox.min.y && cursorPosRelative.y <= _ownRenderBox.max.y)
	{
		isCurrentMouseOnOwnRenderBox = true;
	}

	if(isCurrentMouseOnOwnRenderBox)
	{
		isHovered = true;
	}
	else
	{
		isHovered = false;
	}

	bool isMouseOnOptionsPanel = _optionsPanel.MouseMove();

	return (isMouseOnOptionsPanel || isCurrentMouseOnOwnRenderBox);
}

void Dropdown::DropdownMouseWheel(float amount, int direction)
{
	_optionsPanel.MouseWheel(amount,direction);
}

void Dropdown::SubControlSetGuicontext(GuiContext *guiContext)
{
	_optionsPanel.SetGuiContext(guiContext);
	_chosenOptionLabel.SetGuiContext(guiContext);

	Resize();
	//std::cout<<"GuiContext set for dropdown\n";
}

bool Dropdown::DropdownMouseClick(int button, int action)
{
	if(button == 0)
	{
		if(action == 0)
		{
			if(isHovered)
			{
				isClicked = true;
				if(!_isOpen && _currentOptionCount > 0)
				{
					_optionsPanel.SetActive();
					_isOpen = true;
				}
				else
				{
					_optionsPanel.SetInactive();
					_isOpen = false;
				}
			}
			else
			{
				isClicked = false;
			}
		}
		else
		{
			isClicked = false;
		}
	}

	bool isOptionsPanelClicked = _optionsPanel.Click(button,action);

	return (isOptionsPanelClicked || isClicked);
}


