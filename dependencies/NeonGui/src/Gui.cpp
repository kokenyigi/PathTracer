#include "Gui.h"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp> // transzformációkhoz

/*
* Functions Related To the Graphical Use Interface class
* This class basically acts as a container for our Controls
* 
*/
GUI::GUI()
{
	//Nothing yet
}

GUI::~GUI()
{
	//Nothing yet
}

void GUI::Init(int windowWidth, int windowHeight)
{
	m_guiContext.windowWidth = windowWidth;
	m_guiContext.windowHeight = windowHeight;

    m_guiContext.renderer.Init();

	Texture alphaAlphabetTexture;
	alphaAlphabetTexture.Init("assets/textures/alphabets/alpha_alphabet.png");
	m_guiContext.alphabetTextures.push_back(alphaAlphabetTexture);
	m_guiContext.currentAlphabetType = ALPHABET_ALPHA;

	
}

void GUI::Update(float deltaTime)
{
	m_guiContext.deltaTime = deltaTime;

	for (int i = 0;i < controls.size();++i)
	{
		controls[i]->Update();
	}
}

void GUI::Render()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	for (int i = 0;i < controls.size();++i)
	{
		controls[i]->Render();
	}

	

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

void GUI::Resize(int newWindowWidth, int newWindowHeight)
{
	m_guiContext.windowWidth = newWindowWidth;
	m_guiContext.windowHeight = newWindowHeight;

	for (int i = 0;i < controls.size();++i)
	{
		controls[i]->Resize(); 
	}
}

void GUI::MouseMove(float newMousePosX, float newMousePosY)
{
	m_guiContext.oldMousePos = m_guiContext.currentMousePos;
	m_guiContext.currentMousePos.x = newMousePosX;
	m_guiContext.currentMousePos.y = newMousePosY;

	if(m_guiContext.focusedControl != nullptr)
	{
		m_guiContext.focusedControl->VirtualMouseMove();
	}

	if(!m_guiContext.isMouseDisabled)
	{
		for(int i=0;i<controls.size();++i)
		{
			controls[i]->MouseMove();
		}
	}
    

	
}

void GUI::MouseClick(int button, int action)
{
	if(m_guiContext.focusedControl != nullptr)
	{
		m_guiContext.focusedControl->VirtualClick(button,action);
	}

	if(!m_guiContext.isMouseDisabled)
	{
		for (int i = 0;i < controls.size();++i)
		{
			controls[i]->Click(button,action);
			//std::cout<<"Calling click " << i<< ".th time\n";
		}
	}
	
}

void GUI::MouseWheel(float amount, int direction)
{
	if(m_guiContext.focusedControl != nullptr)
	{
		
		m_guiContext.focusedControl->VirtualMouseWheel(amount,direction);
	}

	if(!m_guiContext.isMouseDisabled)
	{
		for (int i = 0;i < controls.size();++i)
		{
			controls[i]->MouseWheel(amount,direction);
		}
	}
}

void GUI::KeyInput(int key,int action, int mods)
{
	if(m_guiContext.focusedControl != nullptr)
	{	
		m_guiContext.focusedControl->VirtualKeyInput(key,action,mods);
	}

	for (int i = 0;i < controls.size();++i)
	{
		controls[i]->KeyInput(key,action,mods);
	}
}

void GUI::AddControl(Control* control)
{
	controls.push_back(control);
	control->SetGuiContext(GetContext());
}






/*
* Here, below lies the super class, the Control class functions
* These are the same exact functions inside every SubControl class
*/

Control::Control()
{
	
}

Control::~Control()
{
	//Nothing yet
}


void Control::SetRelativePlacement(const glm::vec2 &minPos, const glm::vec2 &maxPos)
{
    this->_box.min = minPos;
    this->_box.max = maxPos;
}

void Control::SetFocused()
{
	if(guiContext != nullptr)
	{
		isFocused = true;
		if(guiContext->focusedControl != nullptr && guiContext->focusedControl != this)
		{
			guiContext->focusedControl->SetUnfocused();
		}

		guiContext->focusedControl = this;
	}
}

void Control::SetUnfocused()
{
	if(guiContext != nullptr)
	{
		if(isFocused || guiContext->focusedControl == this)
		{
			isFocused = false;
			guiContext->focusedControl = nullptr;
		}
	}
}

bool Control::IsCursorOnControl(const glm::vec2& cursorPos)
{
	if(guiContext == nullptr)
	{
		return false;
	}

	glm::vec2 cursorPosRelative = cursorPos / glm::vec2(guiContext->windowWidth,guiContext->windowHeight);

	return cursorPosRelative.x >= _renderBox.min.x && cursorPosRelative.x <= _renderBox.max.x &&
		   cursorPosRelative.y >= _renderBox.min.y && cursorPosRelative.y <= _renderBox.max.y;
}

float Control::CalculateValuebasedOnType(ValueType type, float value, float relativeTo)
{
    if(type == ValueType::FIXED)
	{
		return value;
	}
	else if(type == ValueType::RELATIVE)
	{
		return value * relativeTo;
	}

    return 0.0f;
}

float Control::CalculateValuebasedOnType(ValueType type, float value,int axis)
{
	if(guiContext == nullptr) return 0.0f;

	float multiplier = axis == 0 ? guiContext->windowWidth : guiContext->windowHeight;

	if(type == ValueType::FIXED)
	{
		return value;
	}
	else if(type == ValueType::RELATIVE)
	{
		return value * multiplier;
	}

    return 0.0f;
}

void Control::SetParent(Control *parent)
{
	this->parent = parent;

	//TODO: Recalculate minmaxPos
	RecalculatePosition();
}

void Control::SetBGColor(float r, float g, float b)
{
    this->_baseBGColor =glm::vec3(r,g,b);
}

void Control::SetHoverColor(float r, float g, float b)
{
	this->_hoverBGColor = glm::vec3(r,g,b);
}

void Control::SetClickColor(float r, float g, float b)
{
	_clickBGColor = glm::vec3(r,g,b);
}

void Control::SetWidth(float value, ValueType type)
{
	_widthDefinition.value = value;
	_widthDefinition.type = type;

	//If both margins have been previously declared to be something other than ValueType::AUTO
	//then they become invalidated.
	if(_marginDefinitions[MARGIN_LEFT].type != ValueType::AUTO && 
	_marginDefinitions[MARGIN_RIGHT].type != ValueType::AUTO)
	{
		_marginDefinitions[MARGIN_LEFT].value = 0.0f;
		_marginDefinitions[MARGIN_LEFT].type = ValueType::AUTO;

		_marginDefinitions[MARGIN_RIGHT].value = 0.0f;
		_marginDefinitions[MARGIN_RIGHT].type = ValueType::AUTO;
	}

	//TODO: Recalculate minmaxPos
	RecalculatePosition();
}

void Control::SetHeight(float value, ValueType type)
{
	_heightDefinition.value = value;
	_heightDefinition.type = type;

	//If both margins have been previously declared to be something other than ValueType::AUTO
	//then they become invalidated.
	if(_marginDefinitions[MARGIN_TOP].type != ValueType::AUTO && 
	_marginDefinitions[MARGIN_BOTTOM].type != ValueType::AUTO)
	{
		_marginDefinitions[MARGIN_TOP].value = 0.0f;
		_marginDefinitions[MARGIN_TOP].type = ValueType::AUTO;

		_marginDefinitions[MARGIN_BOTTOM].value = 0.0f;
		_marginDefinitions[MARGIN_BOTTOM].type = ValueType::AUTO;
	}

	//TODO: Recalculate minmaxPos
	RecalculatePosition();
}

void Control::SetMargin(MarginType margin, float value, ValueType type)
{
	_marginDefinitions[margin].value = value;
	_marginDefinitions[margin].type = type;

	if(margin == MARGIN_BOTTOM || margin == MARGIN_TOP)
	{
		if(_marginDefinitions[MARGIN_BOTTOM].type != ValueType::AUTO &&
			 _marginDefinitions[MARGIN_TOP].type != ValueType::AUTO && 
			 _heightDefinition.type != ValueType::AUTO)
		{
			//if both margins on this axis have been defined already, we invalidate the height definition.
			_heightDefinition.type = ValueType::AUTO;
			_heightDefinition.value = 0.0f;
		}
	}
	else
	{
		if(_marginDefinitions[MARGIN_LEFT].type != ValueType::AUTO &&
			 _marginDefinitions[MARGIN_RIGHT].type != ValueType::AUTO && 
			 _widthDefinition.type != ValueType::AUTO)
		{
			//same here as above
			_widthDefinition.type = ValueType::AUTO;
			_widthDefinition.value = 0.0f;
		}
	}

	//TODO: Recalculate minmaxPos
	RecalculatePosition();
}

void Control::RecalculatePosition()
{
	//If the guiContext hasn't been set yet, we can't process any data.
	if(guiContext == nullptr)
	{
		return;
	}

	int windowWidth = guiContext->windowWidth;
	int windowHeight = guiContext->windowHeight;

	glm::vec2 windowResolution = glm::vec2(windowWidth,windowHeight);

	//If the Control has no parent, it still calculates with parentMin and parentMax,
	//but now these values span the whole window.
	glm::vec2 parentMinRelative = glm::vec2(0,0);
	glm::vec2 parentMaxRelative = glm::vec2(1,1);

	if(parent != nullptr)
	{
		//If the Control does actually have a parent we aquire the necessary informations.
		AABB parentBox = parent->GetBox();

		parentMinRelative = parentBox.min;
		parentMaxRelative = parentBox.max;
	}

	glm::vec2 parentMinFixed = parentMinRelative * windowResolution;
	glm::vec2 parentMaxFixed = parentMaxRelative * windowResolution;

	glm::vec2 parentFixedResolution = glm::vec2(parentMaxFixed.x - parentMinFixed.x, parentMaxFixed.y - parentMinFixed.y);

	glm::vec2 ownMinFixed = _box.min * windowResolution;
	glm::vec2 ownMaxFixed = _box.max * windowResolution;

	//If there is a marginDefinition for Left margin, we determine its impact on the control.
	if(_marginDefinitions[MARGIN_LEFT].type == ValueType::FIXED)
	{
		ownMinFixed.x = parentMinFixed.x + _marginDefinitions[MARGIN_LEFT].value;
	}
	else if(_marginDefinitions[MARGIN_LEFT].type == ValueType::RELATIVE)
	{
		ownMinFixed.x = parentMinFixed.x + _marginDefinitions[MARGIN_LEFT].value * parentFixedResolution.x;
	}

	//Same procedure for the top margin.
	if(_marginDefinitions[MARGIN_TOP].type == ValueType::FIXED)
	{
		ownMinFixed.y = parentMinFixed.y + _marginDefinitions[MARGIN_TOP].value;
	}
	else if(_marginDefinitions[MARGIN_TOP].type == ValueType::RELATIVE)
	{
		ownMinFixed.y = parentMinFixed.y + _marginDefinitions[MARGIN_TOP].value * parentFixedResolution.y;
	}


	//Now it calculates the other two margins
	if(_marginDefinitions[MARGIN_RIGHT].type == ValueType::FIXED)
	{
		ownMaxFixed.x = parentMaxFixed.x - _marginDefinitions[MARGIN_RIGHT].value;
	}
	else if(_marginDefinitions[MARGIN_RIGHT].type == ValueType::RELATIVE)
	{
		ownMaxFixed.x = parentMaxFixed.x - _marginDefinitions[MARGIN_RIGHT].value * parentFixedResolution.x;
	}

	if(_marginDefinitions[MARGIN_BOTTOM].type == ValueType::FIXED)
	{
		ownMaxFixed.y = parentMaxFixed.y - _marginDefinitions[MARGIN_BOTTOM].value;
	}
	else if(_marginDefinitions[MARGIN_BOTTOM].type == ValueType::RELATIVE)
	{
		ownMaxFixed.y = parentMaxFixed.y - _marginDefinitions[MARGIN_BOTTOM].value * parentFixedResolution.y;
	}

	//Properly adjusting to the width
	if(_widthDefinition.type != ValueType::AUTO)
	{
		if(_widthDefinition.type == ValueType::FIXED)
		{
			if(_marginDefinitions[MARGIN_LEFT].type != ValueType::AUTO)
			{
				ownMaxFixed.x = ownMinFixed.x + _widthDefinition.value;
			}
			else if(_marginDefinitions[MARGIN_RIGHT].type != ValueType::AUTO)
			{
				ownMinFixed.x = ownMaxFixed.x - _widthDefinition.value;
			}
			else if(_marginDefinitions[MARGIN_RIGHT].type == ValueType::AUTO && _marginDefinitions[MARGIN_LEFT].type == ValueType::AUTO)
			{
				//we center the control based on its width
				float centerValueX = parentMinFixed.x + parentFixedResolution.x * 0.5f;
				ownMinFixed.x = centerValueX - _widthDefinition.value / 2.0f;
				ownMaxFixed.x = centerValueX + _widthDefinition.value / 2.0f;
			}
		}
		else if(_widthDefinition.type == ValueType::RELATIVE)
		{
			if(_marginDefinitions[MARGIN_LEFT].type != ValueType::AUTO)
			{
				ownMaxFixed.x = ownMinFixed.x + _widthDefinition.value * parentFixedResolution.x;
			}
			else if(_marginDefinitions[MARGIN_RIGHT].type != ValueType::AUTO)
			{
				ownMinFixed.x = ownMaxFixed.x - _widthDefinition.value * parentFixedResolution.x;
			}
			else if(_marginDefinitions[MARGIN_RIGHT].type == ValueType::AUTO && _marginDefinitions[MARGIN_LEFT].type == ValueType::AUTO)
			{
				float centerValueX = parentMinFixed.x + parentFixedResolution.x * 0.5f;
				ownMinFixed.x = centerValueX - _widthDefinition.value * parentFixedResolution.x / 2.0f;
				ownMaxFixed.x = centerValueX + _widthDefinition.value * parentFixedResolution.x / 2.0f;
			}
		}
	}

	//Properly Adjusting height
	if(_heightDefinition.type != ValueType::AUTO)
	{
		if(_heightDefinition.type == ValueType::FIXED)
		{
			if(_marginDefinitions[MARGIN_TOP].type != ValueType::AUTO)
			{
				ownMaxFixed.y = ownMinFixed.y + _heightDefinition.value;
			}
			else if(_marginDefinitions[MARGIN_BOTTOM].type != ValueType::AUTO)
			{
				ownMinFixed.y = ownMaxFixed.y - _heightDefinition.value;
			}
			else if(_marginDefinitions[MARGIN_TOP].type == ValueType::AUTO && _marginDefinitions[MARGIN_BOTTOM].type == ValueType::AUTO)
			{
				float centerValueY = parentMinFixed.y + parentFixedResolution.y * 0.5f;
				ownMinFixed.y = centerValueY - _heightDefinition.value / 2.0f;
				ownMaxFixed.y = centerValueY + _heightDefinition.value / 2.0f;
			}
		}
		else if(_heightDefinition.type == ValueType::RELATIVE)
		{
			if(_marginDefinitions[MARGIN_TOP].type != ValueType::AUTO)
			{
				ownMaxFixed.y = ownMinFixed.y + _heightDefinition.value * parentFixedResolution.y;
			}
			else if(_marginDefinitions[MARGIN_BOTTOM].type != ValueType::AUTO)
			{
				ownMinFixed.y = ownMaxFixed.y - _heightDefinition.value * parentFixedResolution.y;
			}
			else if(_marginDefinitions[MARGIN_TOP].type == ValueType::AUTO && _marginDefinitions[MARGIN_BOTTOM].type == ValueType::AUTO)
			{
				float centerValueY = parentMinFixed.y + parentFixedResolution.y * 0.5f;
				ownMinFixed.y = centerValueY - _heightDefinition.value * parentFixedResolution.y / 2.0f;
				ownMaxFixed.y = centerValueY + _heightDefinition.value * parentFixedResolution.y / 2.0f;
			}
		}
	}
	
	//Finally, we normalize the newly calculated positions.
	_box.min = ownMinFixed / windowResolution;
	_box.max = ownMaxFixed / windowResolution;

}

void Control::ControlUpdate()
{
	//Not yet implemented

	
}

void Control::CalculateRenderBox()
{
	if(parent != nullptr)
	{
		bool doesIntersectWithParent = IntersectAABB(_box,parent->GetRenderBox(),_renderBox);
		
	}
	else
	{
		_renderBox = _box;
	}
}

void Control::ControlRender()
{
	
	CalculateRenderBox();
	
    Rectangle ndcControlRenderRect = {_renderBox.min.x,_renderBox.min.y,
									  _renderBox.max.x - _renderBox.min.x,
									  _renderBox.max.y-_renderBox.min.y};

	if(_renderTexture == nullptr)
	{
		if(isClicked)
		{
			_renderColor = _clickBGColor;
		}
		else if(isHovered)
		{
			_renderColor = _hoverBGColor;
		}
		else
		{
			_renderColor = _baseBGColor;
		}

		guiContext->renderer.RenderRectangle(ndcControlRenderRect,_renderColor);
	}
	else
	{
		//Textured rendering
		AABB srcBox = {glm::vec2(0,0),glm::vec2(1,1)};
		TransformBasedOnTwoRelativeAABB(_box,_renderBox,srcBox);

		Rectangle srcRect = {srcBox.min.x,srcBox.min.y,srcBox.max.x - srcBox.min.x, srcBox.max.y-srcBox.min.y};

		//guiContext->renderer.RenderTexturedRectangle(ndcControlRenderRect,srcRect,*_renderTexture);

		guiContext->renderer.RenderColoredTexturedRectangle(ndcControlRenderRect,srcRect,_baseBGColor,*_renderTexture);

		//std::cout<< "texture: " << _renderTexture->GetWidth() <<" " << _renderTexture->GetHeight() << "\n";
	}
	

	//TODO: Add texture rendering in an efficient manner
}

void Control::ControlResize()
{
	RecalculatePosition();
}

bool Control::ControlClick(int button, int action)
{
	if(button == 0)
	{
		if(action == 0)
		{
			if(isHovered)
			{
				isClicked = true;

				return true;
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

	return false;
}

bool Control::ControlMouseMove()
{
	bool isCurrentCursorOnControl = IsCursorOnControl(guiContext->currentMousePos);

	if(isCurrentCursorOnControl)
	{
		isHovered = true;
	}
	else
	{
		isHovered = false;
	}

	return isCurrentCursorOnControl;
}
