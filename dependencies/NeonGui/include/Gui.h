#pragma once
#ifndef NGUI_H
#define NGUI_H

#include <vector>

#include "MyOpenGL.h"

#include "Renderer2D.h"

#include "Utils.h"

enum AlphabetType
{
	ALPHABET_ALPHA = 0,
};

class Control;

struct GuiContext
{
	int windowHeight;
	int windowWidth;

	glm::vec2 oldMousePos;
	glm::vec2 currentMousePos;

	Renderer2D renderer;

	float deltaTime = 1.0f;

	AlphabetType currentAlphabetType;
	std::vector<Texture> alphabetTextures;

	Control* focusedControl = nullptr;

	bool isMouseDisabled = false;
	/*
	Texture guiTexture;
	Texture alphaBetTexture;
	*/
};



enum MarginType
{
	MARGIN_TOP = 0,
	MARGIN_BOTTOM = 1,
	MARGIN_LEFT = 2,
	MARGIN_RIGHT = 3
};




class Control
{
protected:
	//We have to have a gui context from which we derive information 
	GuiContext* guiContext = nullptr;

	//Every Control can have a parent, this value being null means no parent
	Control* parent = nullptr;

	//These variables are updated when certain I/O events happen
	bool isActive = true;
	bool isFocused = false;

	bool isHovered = false;
	bool isClicked = false;

	// This value can be only modified when the control is added to a container, for safety reasons
	// Controls with higher priority will be drawn over the ones with lower priority
	// Controls with higher priority will get events first, instead of the ones with lower.
	int _priority = 0;
	
	Texture* _renderTexture = nullptr;

	//This variable stores the base background color of the control
	glm::vec3 _baseBGColor = glm::vec3(0.8,0.0,0.3);
	//This color is used when the mouse hovers above the control
	glm::vec3 _hoverBGColor = glm::vec3(0.9,0.0,0.4);
	//This value is shown when we click the control, but have not released the mouse yet.
	glm::vec3 _clickBGColor = glm::vec3(0.5,0.0,0.0);

	//The final color of the base control's rectangle when rendering.
	glm::vec3 _renderColor = glm::vec3(0.6,0,0.2);

	//These variables store the control's basic box, where it is rendered and where it can be interracted with
	AABB _box = {glm::vec2(0,0),glm::vec2(1,1)};

	AABB _renderBox = {glm::vec2(0,0),glm::vec2(1,1)};

	ValueDefinition _marginDefinitions[4];
	ValueDefinition _widthDefinition;
	ValueDefinition _heightDefinition;
	
public:
	Control();
	virtual ~Control();

	void Update()
	{
		VirtualUpdate();
	}

	void Render()
	{
		if(isActive)
		{
			VirtualRender();
		}
	}

	void Resize()
	{
		VirtualResize();
	}

	void MouseMove()
	{
		if(!isFocused && isActive)
		{
			VirtualMouseMove();
		}
	}

	/**
	 * button == 0 -> LEFT click
	 * button == 1 -> RIGHT click
	 * 
	 * action == 0 -> PRESS
	 * action == 1 -> RELEASE
	 */
	void Click(int button, int action)
	{
		if(!isFocused && isActive)
		{
			VirtualClick(button,action);
		}
	}

	void MouseWheel(float amount, int direction)
	{
		if(!isFocused && isActive)
		{
			VirtualMouseWheel(amount,direction);
		}
	}

	void KeyInput(int key,int action, int mods)
	{
		if(!isFocused && isActive)
		{
			VirtualKeyInput(key,action,mods);
		}
	}






	virtual void VirtualUpdate()
	{
		ControlUpdate();
	}
	
	virtual void VirtualRender()
	{
		ControlRender();
	}

	virtual void VirtualResize()
	{
		ControlResize();
	}

	virtual void VirtualMouseMove()
	{
		ControlMouseMove();
	}

	virtual void VirtualClick(int button, int action)
	{
		ControlClick(button,action);
	}

	virtual void VirtualMouseWheel(float amount, int direction)
	{
		//Control doesn't do shit on mousewheel.
	}

	virtual void VirtualKeyInput(int key, int action, int mods)
	{
		// A Control doesn't do anything when keypress happens.
	}
	

	
	//Kinda weird thing, brokes some SOLID stuff, whateva
	//This function calls a virtual subfunction,
	//which propagates the change in guicontext ptr to potential children of the Control.
	void SetGuiContext(GuiContext* guiContext)
	{
		this->guiContext = guiContext;
		

		SubControlSetGuicontext(guiContext);

		Resize();
	}

	//Sets the parent of the Control.
	void SetParent(Control* parent);

	//Sets the base background color of the Control.
	void SetBGColor(float r, float g, float b);
	void SetHoverColor(float r, float g, float b);
	void SetClickColor(float r, float g, float b);

	void SetTexture(Texture* texture) {_renderTexture = texture;}

	//Sets the width of the Control.
	//By setting the Height of a Control, both left and right margins will be invalidated.
	void SetWidth(float value, ValueType type = ValueType::FIXED);
	
	//Sets the height of the Control.
	//By setting the Width of a Control, both top and bottom margin will be invalidated.
	void SetHeight(float value, ValueType type = ValueType::FIXED);

	//Sets the margin of the Control in a specific direction.
	//If Both top-bottom or both left-right margins are set, the height or width of the Control will be invalidated.
	void SetMargin(MarginType margin, float value, ValueType type = ValueType::FIXED);

	void SetRelativePlacement(const glm::vec2& minPos,const glm::vec2& maxPos);

	void SetActive() {isActive = true;}
	void SetInactive(){isActive = false;}
	void SetFocused();
	void SetUnfocused();

	void SetPriority(int newPriority){_priority = newPriority;}

	

	inline AABB GetBox() const {return this->_box;}
	inline AABB GetRenderBox() const {return this->_renderBox;}
	inline ValueDefinition GetMarginDefinition(MarginType type) const {return _marginDefinitions[type];}
	inline ValueDefinition GetWidthDefinition() const {return this->_widthDefinition;}
	inline ValueDefinition GetHeightDefinition() const {return this->_heightDefinition;}
	inline bool GetIsActive() const {return isActive;}
	inline bool GetIsFocused() const {return isFocused;}
	inline int GetPriority() const {return _priority;}
private:
	//A control doesn't have any exclusive private methods, cuz why would it have..idk.
protected:

	//This method recalculates the minimum and maximum positions of the Control.
	//Its best practice to call this each time the control is resized, or its variables are tinkerred with.
	void RecalculatePosition();

	
	void CalculateRenderBox();
	
	void ControlUpdate();
	void ControlRender();
	void ControlResize();
	void ControlClick(int button, int action);
	void ControlMouseMove();

	

	virtual void SubControlSetGuicontext(GuiContext* guiContext){}

	

	bool IsCursorOnControl(const glm::vec2& cursorPos);
	float CalculateValuebasedOnType(ValueType type, float value,int axis);
	float CalculateValuebasedOnType(ValueType type, float value,float relativeTo);
};


class GUI
{
private:
	//All previous member variables have been compressed into the context below
	//Every single control there is, has to have a guiContext at the time of creation, because then this given control knows
	//How to properly calculate its internal state using the context, this context pointer also enables it to render,
	//without having to be passed render parameters
	
	GuiContext m_guiContext;






	//The main Control container
	//Note: Later on its better for this to be a universal tree, since
	//Controls can have subcontrols implanted inside them, like a layoutpanel or some shit
	//For now, a linear data structure can be enough since no subcontainer is needed
	std::vector<Control*> controls;

	//The main Quad mesh is a rectangle, which will be used to render ALL Controls
	//In 2D cases no more is required Mesh-sense
	//For resource optimization we will only have one quad like this, and we will pass it to render calls
	//It starts out as a NDC coordinated rectangle, with vertex dimensions: (-1,1) x (-1,1)
	//And texture dimensions: (0,0) x (1,1) <---vertically inverted openGL's texture coordinate system
	

	//This texture so far contains all gui control sprites, and to draw the correct ones,
	//The start quad's textureCoords must be transformd to match the dimensions of the
	

	//This shader is used for gui rendering, its important to enable and later disable glalpha blending when used
	//Now i gotta write another shader uuuarogh..
	//Also very important to disable gldepthtest before rendering gui or else shit happens

	

public:
	GUI();
	~GUI();

	void Init(int windowWidth, int windowHeight);

	GuiContext* GetContext() { return &m_guiContext; }

	void ToggleMouseEnabledDisabled() {m_guiContext.isMouseDisabled =  m_guiContext.isMouseDisabled == false ? true : false;}

	void Update(float deltaTime);
	void Render();
	void Resize(int newWindowWidth, int newWindowHeight);
	void MouseMove(float newMousePosX, float newMousePosY );
	void MouseClick(int button, int action);
	void MouseWheel(float amount,int direction);
	void KeyInput(int key,int action, int mods);

	//Needs still something like:
	//void KeyPressed(char key);

	void AddControl(Control* control);
};


#endif