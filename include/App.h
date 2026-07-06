#pragma once

#ifndef APP_H
#define APP_H

//#include "GL/glew.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Gui.h"
#include "Button.h"
#include "Container.h"
#include "Label.h"
#include "LayoutPanel.h"
#include "RadioButton.h"
#include "Dropdown.h"
#include "TextInput.h"
#include "FloatInput.h"
#include "Canvas.h"

#include "Scene.h"

enum class AppViewState
{
	VIEWSTATE_MAINMENU,
	VIEWSTATE_MESHLOAD,
	VIEWSTATE_TEXTURELOAD,
	VIEWSTATE_PERSISTENCELOAD,
	VIEWSTATE_PERSISTENCESAVE
};

class App
{
private:
	//Window data
	GLFWwindow* m_window = nullptr;
	int m_windowHeight = 0;
	int m_windowWidth = 0;
	bool isWindowMinimized = false;
    float lastMouseX = 0.0f;
	float lastMouseY = 0.0f;
	bool isMouseDisabled = false;
    

	//Virtual World Data
	float deltaTime = 0.0f;
	float lastFrameTime = 0.0f;
	float timeSinceStart = 0.0f;
	
	Scene _scene;

	AppViewState _viewState = AppViewState::VIEWSTATE_MAINMENU;
	

	//GUI Data
	GUI m_GUI;

	Container containerApplication;

	Container containerRight;

	Container containerMeshData;
	Label labelVertexCount; Label labelVertexCountNumber;
	Label labelTriangleCount; Label labelTriangleCountNumber;
	Label labelBVHNodeCount; Label labelBVHNodeCountNumber;
	Label labelBVHDepth; Label labelBVHDepthNumber;

	Container containerTextureData;
	Label labelWidth; Label labelTextureWidthNumber;
	Label labelHeight; Label labelTextureHeightNumber;

	Container containerMaterialData;
	Label labelTexture;Dropdown dropdownTexture;
	Label labelColor;
	Label labelRed; FloatInput inputRed; Slider sliderRed;
	Label labelGreen;FloatInput inputGreen; Slider sliderGreen;
	Label labelBlue; FloatInput inputBlue; Slider sliderBlue;
	Label labelRoughness; FloatInput inputRoughness; Slider sliderRoughness;
	Label labelTransmission; FloatInput inputTransmission; Slider sliderTransmission;
	Label labelIoR;FloatInput inputIoR;Slider sliderIoR;
	Label labelMetallic; FloatInput inputMetallic;Slider sliderMetallic;
	Label labelEmissionStrength;FloatInput inputEmissionStrength;Slider sliderEmissionStrength;
	Label labelEmissionColor;
	Label labelEmissionRed;FloatInput inputEmissionRed;Slider sliderEmissionRed;
	Label labelEmissionGreen;FloatInput inputEmissionGreen;Slider sliderEmissionGreen;
	Label labelEmissionBlue;FloatInput inputEmissionBlue;Slider sliderEmissionBlue;

	Container containerModelData;
	Label labelMesh;Dropdown dropdownMesh;
	Label labelMaterial;Dropdown dropdownMaterial;

	Container containerObjectData;
	Label labelModel; Dropdown dropdownModel;


	Container containerLeft;


	

	
	Container containerCanvas;
	Canvas sceneCanvas;Texture testTexture;

	
	Container containerMainButtons;

	RadioButtonGroup mainButtonsGroup;
	RadioButton meshButton;
	RadioButton textureButton;
	RadioButton materialButton;
	RadioButton modelButton;
	RadioButton objectButton;

	std::vector<MeshInfo> storedMeshInfos;
	LayoutPanel meshPanel;Button buttonLoadMesh;RadioButtonGroup chosenMeshGroup;
	
	std::vector<TextureInfo> storedTextureInfos;
	LayoutPanel texturePanel;Button buttonTextureLoad; RadioButtonGroup chosenTextureGroup;

	std::vector<MaterialInfo> storedMaterialInfos;
	int chosenMaterialIndex = -1;
	LayoutPanel materialPanel;Button buttonMaterialAdd; RadioButtonGroup chosenMaterialGroup;
	
	std::vector<ModelInfo> storedModelInfos;
	int chosenModelIndex = -1;
	LayoutPanel modelPanel; Button buttonModelAdd; RadioButtonGroup chosenModelGroup;

	int chosenObjectIndex = -1;
	LayoutPanel objectPanel; Button buttonAddObject; RadioButtonGroup chosenObjectGroup;


	Container containerFileMenu;
	Container containerExecutionStats;
	Container containerExtreSettings;
	
	

	//File selection related variables	
	Container containerFileSelection;
	LayoutPanel panelFileSelection;
	RadioButtonGroup fileSelectionButtonsGroup;

	Container containerLoadCancelButtons;
	Button buttonLoadFile; Button buttonCancel;

	std::vector<std::string> _loadedDirectorySpecificFilenames;
	int _selectedFileNameIndex = -1;

public:
	App(int windowWidth = 1000, int windowHeight = 800, const char* windowTitle = "_debugTitle");
	~App();
	void Run();

	static void WindowSizeCallback(GLFWwindow* window, int width, int height);
	static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	
	static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	/*
	static void WindowIconifiedCallback(GLFWwindow* window, int isIconified);
	*/

	

	static void MainButtonsNeonGUICallback(void * context, int index);

	static void LoadMeshButtonCallback(void* context);
	static void ChosenMeshButtonCallback(void* context, int meshIndex);

	static void LoadTextureButtonCallback(void* context);
	static void ChosenTextureButtonCallback(void * context, int textureIndex);

	static void AddMaterialButtonCallback(void * context);
	static void ChosenMaterialButtonCallback(void * context, int materialIndex);
	static void AlbedoTextureChosenCallback(void*context, int optionTextureIndex);
	static void AlbedoRedAlteredCallback(void*context,float redValue);
	static void AlbedoGreenAlteredCallback(void*context,float greenValue);
	static void AlbedoBlueAlteredCallback(void*context,float blueValue);
	static void MetallicAlteredCallback(void*context,float metallicValue);
	static void RoughnessAlteredCallback(void*context,float roughnessValue);
	static void TransmissionAlteredCallback(void*context,float transmissionValue);
	static void IoRInputCallback(void*context,float IoRValue);
	static void IoRSliderCallback(void*context,float IoRValueNormalized);
	static void EmissionStrengthInputCallback(void*context,float emissionStrengthValue);
	static void EmissionStrengthSliderCallback(void*context,float emissionStrengthNormalizedValue);
	static void EmissionRedAlteredCallback(void* context,float redValue);
	static void EmissionGreenAlteredCallback(void* context,float greenValue);
	static void EmissionBlueAlteredCallback(void* context,float blueValue);

	static void AddModelButtonCallback(void * context);
	static void ChosenModelButtonCallback(void * context, int modelIndex);
	static void MeshIndexChosenDropdownCallback(void* context, int chosenOptionIndex);
	static void MaterialIndexChosenDropdownCallback(void* context, int chosenOptionIndex);

	static void FileSelectionMenuItemCallback(void * context, int index);
	static void FileSelectionMenuLoadButtonCallback(void* context);
	static void FileSelectionMenuCancelButtonCallback(void* context);


	static void CanvasResizeCallback(void * context, int newW, int newH);
	static void CanvasRenderCallback(void* context);
	static void CanvasUpdateCallback(void* context, float deltaTime);
	static void CanvasKeyCallback(void* context, int key, int action, int mods);
	static void CanvasMouseMoveCallback(void * context , float newX, float newY);
	

private:
	void GlfwInit(int windowWidth, int windowHeight, const char* windowTitle);
	void GladInit(int windowWidth, int windowHeight);
	void ImGuiInit();
	void Update();
	void Render();

	void RepopulateFileSelectionPanel();

	void SwapToFileSelectionMenu();
	void SwapBackToMainMenu();

	void TryLoadMeshData();
	void TryLoadTextureData();
	//void TryLoadGamePersistenceData--orsmth
};


#endif