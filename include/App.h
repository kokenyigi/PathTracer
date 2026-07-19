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

#include "ImageLabelButton.h"

enum class AppViewState
{
	VIEWSTATE_MAINMENU,
	VIEWSTATE_MESHLOAD,
	VIEWSTATE_TEXTURELOAD,
	VIEWSTATE_PERSISTENCESAVE,
	VIEWSTATE_PERSISTENCELOAD
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
	Label labelPosition;
	Label labelXPos;FloatInput inputXPos;
	Label labelYPos;FloatInput inputYPos;
	Label labelZPos;FloatInput inputZPos;
	Label labelScale;
	Label labelXScale;FloatInput inputXScale;
	Label labelYScale;FloatInput inputYScale;
	Label labelZScale;FloatInput inputZScale;
	Label labelRotation;
	Label labelXRot;FloatInput inputXRot;Slider sliderXRot;
	Label labelYRot;FloatInput inputYRot;Slider sliderYRot;
	Label labelZRot;FloatInput inputZRot;Slider sliderZRot;
	Button buttonDeleteObject;


	Container containerLeft;
	Button buttonNewScene;
	Button buttonSaveScene;
	Button buttonLoadScene;
	

	
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

	std::vector<ObjectInfo> storedObjectInfos;
	int chosenObjectIndex = -1;
	LayoutPanel objectPanel; Button buttonAddObject; RadioButtonGroup chosenObjectGroup;


	Container containerFileMenu;
	Container containerExecutionStats;
	Container containerExtreSettings;
	
	

	//File selection related variables	
	Container containerFileSelection;
	LayoutPanel panelFileSelection;
	RadioButtonGroup fileSelectionButtonsGroup;
	TextInput inputSaveFileName;	
	Container containerLoadCancelButtons;
	Button buttonSaveFile;Button buttonLoadFile; Button buttonCancel;

	
	std::vector<std::string> _loadedDirectorySpecificFilenames;
	int _selectedFileNameIndex = -1;
	std::string _chosenEditedFileName = "default.scn";

	//Persistence
	std::vector<std::string> _meshRelativeFilePaths;
	std::vector<std::string> _textureRelativeFilePaths;

public:
	App(int windowWidth = 1000, int windowHeight = 800, const char* windowTitle = "_debugTitle");
	~App();
	void Run();

	static void WindowSizeCallback(GLFWwindow* window, int width, int height);
	static void WindowMaximizationCallback(GLFWwindow* window, int maximized);
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
	static void ChosenMeshNameChangedCallback(void * context, int meshIndex, const std::string& newText);

	static void LoadTextureButtonCallback(void* context);
	static void ChosenTextureButtonCallback(void * context, int textureIndex);
	static void ChosenTextureNameChangedCallback(void* context, int textureIndex, const std::string& newName);

	static void AddMaterialButtonCallback(void * context);
	static void ChosenMaterialButtonCallback(void * context, int materialIndex);
	static void ChosenMaterialNameChangedCallback(void* context, int materialIndex, const std::string& newName);
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
	static void ChosenModelNameChangedCallback(void* context , int modelIndex, const std::string& newName);
	static void MeshIndexChosenDropdownCallback(void* context, int chosenOptionIndex);
	static void MaterialIndexChosenDropdownCallback(void* context, int chosenOptionIndex);

	static void AddObjectButtonCallback(void * context);
	static void ChosenObjectButtonCallback(void * context, int objectIndex);
	//static void ChosenModelNameChangedCallback(void* context , int modelIndex, const std::string& newName);
	static void ModelIndexChosenDropdownCallback(void* context, int chosenOptionIndex);
	template<int axis,int compType,bool isSlider = false> // axis: 0-x, 1-y, 2-z, compType: 0-Pos, 1-Scl, 2-Rot,isSlider self explenatory
	static void CompTypeAlteredCallback(void* context , float newValueOnAxis);
	static void DeleteObjectButtonCallback(void* context);


	static void NewSceneButtonCallback(void* context);
	static void SaveSceneButtonCallback(void* context);
	static void LoadSceneButtonCallback(void* context);

	static void FileSelectionMenuItemCallback(void * context, int index);
	static void FileSelectionMenuLoadButtonCallback(void* context);
	static void FileSelectionMenuCancelButtonCallback(void* context);
	static void FileMenuSaveTextInputCallback(void* context, const std::string& newString);
	static void FileMenuSaveButtonCallback(void* context);


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

	bool TryLoadMesh(const std::string& meshFileName);
	bool TryLoadTexture(const std::string& textureFileName);
	void AddMaterial(const MaterialData& newMaterialData);
	bool TryAddModel(const ModelDataCpu& newModelData);
	bool TryAddObject(const ObjectState& newObjectState);

	void SaveScene(const std::string& sceneSavingFileNameRelative);
	bool TryLoadScene(const std::string& sceneFilePathRelative);
	

	/**
	 * This procedure fully reset's the app's state, that means, it will both Scene-side and App-side delete the created structures 
	 *  like, meshes, textures, materials, models, objects and all buttons associated with them.
	 * It acts as a full cleanup of the state, and will be mainly used when we want to load a new app state,( or scene) in.
	 */
	void Reset();
	
};


template <int axis, int compType, bool isSlider>
inline void App::CompTypeAlteredCallback(void *context, float newValueOnAxis)
{
	App* app = (App*)context;
	if(app->chosenObjectIndex != -1)
	{
		int objectIndex = app->chosenObjectIndex;
		ObjectState oldObjectState;
		app->_scene.GetObjectState(objectIndex,&oldObjectState);
		if(compType == 0)
		{
			float* positionArray = &oldObjectState.transform.position.x;
			positionArray[axis] = newValueOnAxis;
		}
		else if(compType == 1)
		{
			float* scaleArray = &oldObjectState.transform.scale.x;
			scaleArray[axis] = newValueOnAxis;
		}
		else
		{
			float* rotationArray = &oldObjectState.transform.rotation.x;
			if(isSlider)
			{
				rotationArray[axis] = newValueOnAxis*360.0f; // since value is normalized

				if(axis==0){app->inputXRot.SetFloat(newValueOnAxis*360.0f);}
				else if(axis==1){app->inputYRot.SetFloat(newValueOnAxis*360.0f);}
				else{app->inputZRot.SetFloat(newValueOnAxis*360.0f);}
			}
			else
			{
				rotationArray[axis] = newValueOnAxis; // is not normalized

				if(axis==0){app->sliderXRot.SetSliderValue(newValueOnAxis/360.0f);}
				else if(axis==1){app->sliderYRot.SetSliderValue(newValueOnAxis/360.0f);}
				else{app->sliderZRot.SetSliderValue(newValueOnAxis/360.0f);}
			}
		}

		app->_scene.TryAlterObject(objectIndex,oldObjectState);
	}
}

#endif


