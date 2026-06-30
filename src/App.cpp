#include "App.h"

#include <iostream>
#include <chrono>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"


App::App(int windowWidth, int windowHeight, const char* windowTitle)
{
	//std::cout << " App constructor" << std::endl;
	//Mandatorily intialization of glfw and glew

	GlfwInit(windowWidth, windowHeight, windowTitle);
	GladInit(windowWidth, windowHeight);
	ImGuiInit();

	_scene.Init();

	std::cout<<"Initialization of Gui\n";
	m_GUI.Init(windowWidth,windowHeight);

	containerApplication.SetMargin(MARGIN_TOP,0.0);
	containerApplication.SetMargin(MARGIN_RIGHT,0.0);
	containerApplication.SetMargin(MARGIN_LEFT,0.0);
	containerApplication.SetMargin(MARGIN_BOTTOM,0.0);
    
	containerLeft.SetMargin(MARGIN_TOP,50.0f,ValueType::FIXED);
	containerLeft.SetMargin(MARGIN_BOTTOM,0.0f,ValueType::FIXED);
	containerLeft.SetMargin(MARGIN_LEFT,0.0f,ValueType::FIXED);
	containerLeft.SetWidth(250.0f,ValueType::FIXED);
	containerLeft.SetBGColor(0.2,0.2,0.2);

	containerApplication.AddControl(&containerLeft);

	containerRight.SetMargin(MARGIN_TOP,50.0f,ValueType::FIXED);
	containerRight.SetMargin(MARGIN_BOTTOM,0.0f,ValueType::FIXED);
	containerRight.SetMargin(MARGIN_RIGHT,0.0f,ValueType::FIXED);
	containerRight.SetWidth(500.0f,ValueType::FIXED);
	containerRight.SetBGColor(0.2,0.2,0.2);
	
	containerMeshData.SetMargin(MARGIN_TOP,10.0f);
	containerMeshData.SetMargin(MARGIN_LEFT,10.0f);
	containerMeshData.SetMargin(MARGIN_RIGHT,10.0f);
	containerMeshData.SetMargin(MARGIN_BOTTOM,10.0f);
	containerMeshData.SetBGColor(0.2,0.2,0.2);

	labelVertexCount.SetMargin(MARGIN_TOP,0.0);
	labelVertexCount.SetMargin(MARGIN_LEFT,0.0);
	labelVertexCount.SetHeight(30.0f);
	labelVertexCount.SetWidth(300.0f);
	labelVertexCount.SetText("Vertex count: ");
	labelVertexCount.SetTextColor(1,1,1);

	containerMeshData.AddControl(&labelVertexCount);

	labelTriangleCount.SetMargin(MARGIN_TOP,60.0f);
	labelTriangleCount.SetMargin(MARGIN_LEFT,0.0f);
	labelTriangleCount.SetHeight(30.0f);
	labelTriangleCount.SetWidth(300.0f);
	labelTriangleCount.SetText("Triangle count: ");
	labelTriangleCount.SetTextColor(1,1,1);

	containerMeshData.AddControl(&labelTriangleCount);

	labelBVHNodeCount.SetMargin(MARGIN_TOP,120.0f);
	labelBVHNodeCount.SetMargin(MARGIN_LEFT,0.0f);
	labelBVHNodeCount.SetHeight(30.0f);
	labelBVHNodeCount.SetWidth(300.0f);
	labelBVHNodeCount.SetText("BVH node count: ");
	labelBVHNodeCount.SetTextColor(1,1,1);

	containerMeshData.AddControl(&labelBVHNodeCount);

	
	labelBVHDepth.SetMargin(MARGIN_TOP,180.0f);
	labelBVHDepth.SetMargin(MARGIN_LEFT,0.0f);
	labelBVHDepth.SetHeight(30.0f);
	labelBVHDepth.SetWidth(300.0f);
	labelBVHDepth.SetText("BVH depth: ");
	labelBVHDepth.SetTextColor(1,1,1);

	containerMeshData.AddControl(&labelBVHDepth);
	
	containerRight.AddControl(&containerMeshData);

	containerTextureData.SetMargin(MARGIN_TOP,10.0);
	containerTextureData.SetMargin(MARGIN_LEFT,10.0);
	containerTextureData.SetMargin(MARGIN_RIGHT,10.0);
	containerTextureData.SetMargin(MARGIN_TOP,10.0);
	containerTextureData.SetBGColor(0.2,0.2,0.2);

	labelHeight.SetMargin(MARGIN_TOP,0.0f);
	labelHeight.SetMargin(MARGIN_LEFT,0.0f);
	labelHeight.SetHeight(30.0f);
	labelHeight.SetWidth(200.0f);
	labelHeight.SetText("Height: ");
	labelHeight.SetTextColor(1,1,1);
	
	containerTextureData.AddControl(&labelHeight);

	labelWidth.SetMargin(MARGIN_TOP,60.0f);
	labelWidth.SetMargin(MARGIN_LEFT,0.0f);
	labelWidth.SetHeight(30.0f);
	labelWidth.SetWidth(200.0f);
	labelWidth.SetText("Width: ");
	labelWidth.SetTextColor(1,1,1);
	
	containerTextureData.AddControl(&labelWidth);

	containerRight.AddControl(&containerTextureData);

	containerTextureData.SetInactive();



	containerMaterialData.SetMargin(MARGIN_TOP,10.0);
	containerMaterialData.SetMargin(MARGIN_LEFT,10.0);
	containerMaterialData.SetMargin(MARGIN_RIGHT,10.0);
	containerMaterialData.SetMargin(MARGIN_TOP,10.0);
	containerMaterialData.SetBGColor(0.2,0.2,0.2);

	labelTexture.SetMargin(MARGIN_TOP,15.0f);
	labelTexture.SetMargin(MARGIN_LEFT,0.0f);
	labelTexture.SetHeight(30.0f);
	labelTexture.SetWidth(200.0f);
	labelTexture.SetText("Texture: ");
	labelTexture.SetTextColor(1,1,1);

	containerMaterialData.AddControl(&labelTexture);

	dropdownTexture.SetMargin(MARGIN_TOP,0.0f);
	dropdownTexture.SetMargin(MARGIN_LEFT,200.0f);
	dropdownTexture.SetHeight(50.0f);
	dropdownTexture.SetWidth(200.0f);
	dropdownTexture.SetBGColor(0.9,0.9,0.9);
	dropdownTexture.SetHoverColor(1,1,1);
	dropdownTexture.SetClickColor(0,1,0);
	dropdownTexture.SetChosenOptionTextColor(0,0,0);
	dropdownTexture.SetOptionBaseColor(0.8,0.8,0.8);
	dropdownTexture.SetOptionHoveredColor(0.9,0.9,0.9);
	dropdownTexture.SetOptionClickedColor(0,1,0);
	dropdownTexture.SetScrollBarBaseBGColor(0.2,0.2,0.2);
	dropdownTexture.SetScrollBarBaseColor(0.5,0.5,0.5);
	dropdownTexture.SetScrollBarHoveredColor(0.7,0.7,0.7);
	dropdownTexture.SetScrollBarClickedColor(0,1,0);
	dropdownTexture.SetMaxVisibleOptionCount(5);
	dropdownTexture.AddOption("null",0);
	dropdownTexture.SetScrollBarSize(15.0f);

	containerMaterialData.AddControl(&dropdownTexture);

	labelColor.SetMargin(MARGIN_TOP,75.0f);
	labelColor.SetMargin(MARGIN_LEFT,0.0f);
	labelColor.SetHeight(30.0f);
	labelColor.SetWidth(120.0f);
	labelColor.SetText("Color r: ");
	labelColor.SetTextColor(1,1,1);

	containerMaterialData.AddControl(&labelColor);

	inputRed.SetMargin(MARGIN_TOP,60.0f);
	inputRed.SetMargin(MARGIN_LEFT,140.0f);
	inputRed.SetHeight(50.0f);
	inputRed.SetWidth(80.0f);
	inputRed.SetTextColor(0,0,0);
	inputRed.SetEditHeadColor(0,1,0);
	inputRed.SetFloat(0.0f);
	inputRed.SetBGColor(1,1,1);

	containerMaterialData.AddControl(&inputRed);

	labelGreen.SetMargin(MARGIN_TOP,75.0f);
	labelGreen.SetMargin(MARGIN_LEFT,220.0f);
	labelGreen.SetHeight(30.0f);
	labelGreen.SetWidth(30.0f);
	labelGreen.SetText("g: ");
	labelGreen.SetTextColor(1,1,1);

	containerMaterialData.AddControl(&labelGreen);

	inputGreen.SetMargin(MARGIN_TOP,60.0f);
	inputGreen.SetMargin(MARGIN_LEFT,250.0f);
	inputGreen.SetHeight(50.0f);
	inputGreen.SetWidth(80.0f);
	inputGreen.SetTextColor(0,0,0);
	inputGreen.SetEditHeadColor(0,1,0);
	inputGreen.SetFloat(0.0f);
	inputGreen.SetBGColor(1,1,1);

	containerMaterialData.AddControl(&inputGreen);

	labelBlue.SetMargin(MARGIN_TOP,75.0f);
	labelBlue.SetMargin(MARGIN_LEFT,330.0f);
	labelBlue.SetHeight(30.0f);
	labelBlue.SetWidth(30.0f);
	labelBlue.SetText("b: ");
	labelBlue.SetTextColor(1,1,1);

	containerMaterialData.AddControl(&labelBlue);

	inputBlue.SetMargin(MARGIN_TOP,60.0f);
	inputBlue.SetMargin(MARGIN_LEFT,360.0f);
	inputBlue.SetHeight(50.0f);
	inputBlue.SetWidth(80.0f);
	inputBlue.SetTextColor(0,0,0);
	inputBlue.SetEditHeadColor(0,1,0);
	inputBlue.SetFloat(0.0f);
	inputBlue.SetBGColor(1,1,1);

	containerMaterialData.AddControl(&inputBlue);

	labelRoughness.SetMargin(MARGIN_TOP,135.0f);
	labelRoughness.SetMargin(MARGIN_LEFT,0.0f);
	labelRoughness.SetHeight(30.0f);
	labelRoughness.SetWidth(200.0f);
	labelRoughness.SetText("Roughness: ");
	labelRoughness.SetTextColor(1,1,1);

	containerMaterialData.AddControl(&labelRoughness);

	inputRoughness.SetMargin(MARGIN_TOP,120.0f);
	inputRoughness.SetMargin(MARGIN_LEFT,200.0f);
	inputRoughness.SetHeight(50.0f);
	inputRoughness.SetWidth(100.0f);
	inputRoughness.SetTextColor(0,0,0);
	inputRoughness.SetEditHeadColor(0,1,0);
	inputRoughness.SetFloat(0.0f);
	inputRoughness.SetBGColor(1,1,1);

	containerMaterialData.AddControl(&inputRoughness);

	containerRight.AddControl(&containerMaterialData);

	containerMaterialData.SetInactive();

	containerModelData.SetMargin(MARGIN_TOP,10.0);
	containerModelData.SetMargin(MARGIN_LEFT,10.0);
	containerModelData.SetMargin(MARGIN_RIGHT,10.0);
	containerModelData.SetMargin(MARGIN_TOP,10.0);
	containerModelData.SetBGColor(0.2,0.2,0.2);

	labelMesh.SetMargin(MARGIN_TOP,0.0f);
	labelMesh.SetMargin(MARGIN_LEFT,0.0f);
	labelMesh.SetHeight(50.0f);
	labelMesh.SetWidth(200.0f);
	labelMesh.SetText("Mesh: ");
	labelMesh.SetTextColor(1,1,1);

	containerModelData.AddControl(&labelMesh);

	dropdownMesh.SetMargin(MARGIN_TOP,0.0f);
	dropdownMesh.SetMargin(MARGIN_LEFT,200.0f);
	dropdownMesh.SetHeight(50.0f);
	dropdownMesh.SetWidth(200.0f);
	dropdownMesh.SetBGColor(0.9,0.9,0.9);
	dropdownMesh.SetHoverColor(1,1,1);
	dropdownMesh.SetClickColor(0,1,0);
	dropdownMesh.SetChosenOptionTextColor(0,0,0);
	dropdownMesh.SetOptionBaseColor(0.8,0.8,0.8);
	dropdownMesh.SetOptionHoveredColor(0.9,0.9,0.9);
	dropdownMesh.SetOptionClickedColor(0,1,0);
	dropdownMesh.SetScrollBarBaseBGColor(0.2,0.2,0.2);
	dropdownMesh.SetScrollBarBaseColor(0.5,0.5,0.5);
	dropdownMesh.SetScrollBarHoveredColor(0.7,0.7,0.7);
	dropdownMesh.SetScrollBarClickedColor(0,1,0);
	dropdownMesh.SetMaxVisibleOptionCount(5);
	dropdownMesh.AddOption("null",0);
	dropdownMesh.SetScrollBarSize(15.0f);

	containerModelData.AddControl(&dropdownMesh);

	labelMaterial.SetMargin(MARGIN_TOP,60.0f);
	labelMaterial.SetMargin(MARGIN_LEFT,0.0f);
	labelMaterial.SetHeight(50.0f);
	labelMaterial.SetWidth(200.0f);
	labelMaterial.SetText("Material: ");
	labelMaterial.SetTextColor(1,1,1);

	containerModelData.AddControl(&labelMaterial);

	dropdownMaterial.SetMargin(MARGIN_TOP,60.0f);
	dropdownMaterial.SetMargin(MARGIN_LEFT,200.0f);
	dropdownMaterial.SetHeight(50.0f);
	dropdownMaterial.SetWidth(200.0f);
	dropdownMaterial.SetBGColor(0.9,0.9,0.9);
	dropdownMaterial.SetHoverColor(1,1,1);
	dropdownMaterial.SetClickColor(0,1,0);
	dropdownMaterial.SetChosenOptionTextColor(0,0,0);
	dropdownMaterial.SetOptionBaseColor(0.8,0.8,0.8);
	dropdownMaterial.SetOptionHoveredColor(0.9,0.9,0.9);
	dropdownMaterial.SetOptionClickedColor(0,1,0);
	dropdownMaterial.SetScrollBarBaseBGColor(0.2,0.2,0.2);
	dropdownMaterial.SetScrollBarBaseColor(0.5,0.5,0.5);
	dropdownMaterial.SetScrollBarHoveredColor(0.7,0.7,0.7);
	dropdownMaterial.SetScrollBarClickedColor(0,1,0);
	dropdownMaterial.SetMaxVisibleOptionCount(5);
	dropdownMaterial.AddOption("null",0);
	dropdownMaterial.SetScrollBarSize(15.0f);

	containerModelData.AddControl(&dropdownMaterial);

	containerRight.AddControl(&containerModelData);

	containerModelData.SetInactive();


	containerObjectData.SetMargin(MARGIN_TOP,10.0);
	containerObjectData.SetMargin(MARGIN_LEFT,10.0);
	containerObjectData.SetMargin(MARGIN_RIGHT,10.0);
	containerObjectData.SetMargin(MARGIN_TOP,10.0);
	containerObjectData.SetBGColor(0.2,0.2,0.2);

	labelModel.SetMargin(MARGIN_TOP,0.0f);
	labelModel.SetMargin(MARGIN_LEFT,0.0f);
	labelModel.SetHeight(50.0f);
	labelModel.SetWidth(200.0f);
	labelModel.SetText("Model: ");
	labelModel.SetTextColor(1,1,1);

	containerObjectData.AddControl(&labelModel);

	dropdownModel.SetMargin(MARGIN_TOP,0.0f);
	dropdownModel.SetMargin(MARGIN_LEFT,200.0f);
	dropdownModel.SetHeight(50.0f);
	dropdownModel.SetWidth(200.0f);
	dropdownModel.SetBGColor(0.9,0.9,0.9);
	dropdownModel.SetHoverColor(1,1,1);
	dropdownModel.SetClickColor(0,1,0);
	dropdownModel.SetChosenOptionTextColor(0,0,0);
	dropdownModel.SetOptionBaseColor(0.8,0.8,0.8);
	dropdownModel.SetOptionHoveredColor(0.9,0.9,0.9);
	dropdownModel.SetOptionClickedColor(0,1,0);
	dropdownModel.SetScrollBarBaseBGColor(0.2,0.2,0.2);
	dropdownModel.SetScrollBarBaseColor(0.5,0.5,0.5);
	dropdownModel.SetScrollBarHoveredColor(0.7,0.7,0.7);
	dropdownModel.SetScrollBarClickedColor(0,1,0);
	dropdownModel.SetMaxVisibleOptionCount(5);
	dropdownModel.AddOption("null",0);
	dropdownModel.SetScrollBarSize(15.0f);

	containerObjectData.AddControl(&dropdownModel);

	containerRight.AddControl(&containerObjectData);

	containerApplication.AddControl(&containerRight);

	

	meshPanel.SetMargin(MARGIN_BOTTOM,0.0f,ValueType::FIXED);
	meshPanel.SetMargin(MARGIN_LEFT,250.0f,ValueType::FIXED);
	meshPanel.SetMargin(MARGIN_RIGHT,500.0f,ValueType::FIXED);
	meshPanel.SetHeight(250.0f,ValueType::FIXED);
	meshPanel.SetBGColor(0.3,0.3,0.3);
	meshPanel.SetChildrenHeight(100.0f);
	meshPanel.SetChildrenWidth(200.0f);
	meshPanel.SetLayoutType(LAYOUT_FLOW);
	meshPanel.SetGapHeight(5.0f);
	meshPanel.SetGapWidth(5.0f);
	meshPanel.SetPadding(PADDING_RIGHT,10.0f);
	meshPanel.SetPadding(PADDING_LEFT,10.0f);
	meshPanel.SetPadding(PADDING_TOP,10.0f);
	meshPanel.SetPadding(PADDING_BOTTOM,10.0f);
	meshPanel.SetScrollBarBaseBGColor(0.2,0.2,0.2);
	meshPanel.SetScrollBarBaseColor(0.4,0.4,0.4);
	meshPanel.SetScrollBarHoveredColor(0.6,0.6,0.6);
	meshPanel.SetScrollBarClickedColor(0,1,0);

	buttonLoadMesh.SetBGColor(0.6,0.6,0.6);
	buttonLoadMesh.SetHoverColor(0.7,0.7,0.7);
	buttonLoadMesh.SetClickColor(1,1,1);
	buttonLoadMesh.SetText("LOAD");
	buttonLoadMesh.SetTextColor(0,0,0);
	buttonLoadMesh.SetCallBackContext(this);
	buttonLoadMesh.SetCallback(LoadMeshButtonCallback);
	
	meshPanel.AddControl(&buttonLoadMesh);

	Button* tempButton = new Button();
	tempButton->SetText("Test Mesh");
	meshPanel.AddControl(tempButton);
	
	
	containerApplication.AddControl(&meshPanel);

	
	texturePanel.SetMargin(MARGIN_BOTTOM,0.0f,ValueType::FIXED);
	texturePanel.SetMargin(MARGIN_LEFT,250.0f,ValueType::FIXED);
	texturePanel.SetMargin(MARGIN_RIGHT,500.0f,ValueType::FIXED);
	texturePanel.SetHeight(250.0f,ValueType::FIXED);
	texturePanel.SetBGColor(0.3,0.3,0.3);
	texturePanel.SetChildrenHeight(100.0f);
	texturePanel.SetChildrenWidth(200.0f);
	texturePanel.SetLayoutType(LAYOUT_FLOW);
	texturePanel.SetGapHeight(5.0f);
	texturePanel.SetGapWidth(5.0f);
	texturePanel.SetPadding(PADDING_RIGHT,10.0f);
	texturePanel.SetPadding(PADDING_LEFT,10.0f);
	texturePanel.SetPadding(PADDING_TOP,10.0f);
	texturePanel.SetPadding(PADDING_BOTTOM,10.0f);
	texturePanel.SetScrollBarBaseBGColor(0.2,0.2,0.2);
	texturePanel.SetScrollBarBaseColor(0.4,0.4,0.4);
	texturePanel.SetScrollBarHoveredColor(0.6,0.6,0.6);
	texturePanel.SetScrollBarClickedColor(0,1,0);
	
	Button* tempTexture = new Button();
	tempTexture->SetText("Test Texture");
	texturePanel.AddControl(tempTexture);
	
	containerApplication.AddControl(&texturePanel);

	texturePanel.SetInactive();


	materialPanel.SetMargin(MARGIN_BOTTOM,0.0f,ValueType::FIXED);
	materialPanel.SetMargin(MARGIN_LEFT,250.0f,ValueType::FIXED);
	materialPanel.SetMargin(MARGIN_RIGHT,500.0f,ValueType::FIXED);
	materialPanel.SetHeight(250.0f,ValueType::FIXED);
	materialPanel.SetBGColor(0.3,0.3,0.3);
	materialPanel.SetChildrenHeight(100.0f);
	materialPanel.SetChildrenWidth(200.0f);
	materialPanel.SetLayoutType(LAYOUT_FLOW);
	materialPanel.SetGapHeight(5.0f);
	materialPanel.SetGapWidth(5.0f);
	materialPanel.SetPadding(PADDING_RIGHT,10.0f);
	materialPanel.SetPadding(PADDING_LEFT,10.0f);
	materialPanel.SetPadding(PADDING_TOP,10.0f);
	materialPanel.SetPadding(PADDING_BOTTOM,10.0f);
	materialPanel.SetScrollBarBaseBGColor(0.2,0.2,0.2);
	materialPanel.SetScrollBarBaseColor(0.4,0.4,0.4);
	materialPanel.SetScrollBarHoveredColor(0.6,0.6,0.6);
	materialPanel.SetScrollBarClickedColor(0,1,0);
	
	Button* tempMaterial = new Button();
	tempMaterial->SetText("Test Material");
	materialPanel.AddControl(tempMaterial);
	
	containerApplication.AddControl(&materialPanel);

	materialPanel.SetInactive();



	modelPanel.SetMargin(MARGIN_BOTTOM,0.0f,ValueType::FIXED);
	modelPanel.SetMargin(MARGIN_LEFT,250.0f,ValueType::FIXED);
	modelPanel.SetMargin(MARGIN_RIGHT,500.0f,ValueType::FIXED);
	modelPanel.SetHeight(250.0f,ValueType::FIXED);
	modelPanel.SetBGColor(0.3,0.3,0.3);
	modelPanel.SetChildrenHeight(100.0f);
	modelPanel.SetChildrenWidth(200.0f);
	modelPanel.SetLayoutType(LAYOUT_FLOW);
	modelPanel.SetGapHeight(5.0f);
	modelPanel.SetGapWidth(5.0f);
	modelPanel.SetPadding(PADDING_RIGHT,10.0f);
	modelPanel.SetPadding(PADDING_LEFT,10.0f);
	modelPanel.SetPadding(PADDING_TOP,10.0f);
	modelPanel.SetPadding(PADDING_BOTTOM,10.0f);
	modelPanel.SetScrollBarBaseBGColor(0.2,0.2,0.2);
	modelPanel.SetScrollBarBaseColor(0.4,0.4,0.4);
	modelPanel.SetScrollBarHoveredColor(0.6,0.6,0.6);
	modelPanel.SetScrollBarClickedColor(0,1,0);
	
	Button* tempModel = new Button();
	tempModel->SetText("Test Model");
	modelPanel.AddControl(tempModel);
	
	containerApplication.AddControl(&modelPanel);

	modelPanel.SetInactive();


	objectPanel.SetMargin(MARGIN_BOTTOM,0.0f,ValueType::FIXED);
	objectPanel.SetMargin(MARGIN_LEFT,250.0f,ValueType::FIXED);
	objectPanel.SetMargin(MARGIN_RIGHT,500.0f,ValueType::FIXED);
	objectPanel.SetHeight(250.0f,ValueType::FIXED);
	objectPanel.SetBGColor(0.3,0.3,0.3);
	objectPanel.SetChildrenHeight(100.0f);
	objectPanel.SetChildrenWidth(200.0f);
	objectPanel.SetLayoutType(LAYOUT_FLOW);
	objectPanel.SetGapHeight(5.0f);
	objectPanel.SetGapWidth(5.0f);
	objectPanel.SetPadding(PADDING_RIGHT,10.0f);
	objectPanel.SetPadding(PADDING_LEFT,10.0f);
	objectPanel.SetPadding(PADDING_TOP,10.0f);
	objectPanel.SetPadding(PADDING_BOTTOM,10.0f);
	objectPanel.SetScrollBarBaseBGColor(0.2,0.2,0.2);
	objectPanel.SetScrollBarBaseColor(0.4,0.4,0.4);
	objectPanel.SetScrollBarHoveredColor(0.6,0.6,0.6);
	objectPanel.SetScrollBarClickedColor(0,1,0);
	
	Button* tempObject = new Button();
	tempObject->SetText("Test object");
	objectPanel.AddControl(tempObject);
	
	containerApplication.AddControl(&objectPanel);

	objectPanel.SetInactive();



	containerCanvas.SetMargin(MARGIN_BOTTOM,310.0f,ValueType::FIXED);
	containerCanvas.SetMargin(MARGIN_LEFT,250.0f,ValueType::FIXED);
	containerCanvas.SetMargin(MARGIN_RIGHT,500.0f,ValueType::FIXED);
	containerCanvas.SetMargin(MARGIN_TOP,50.0f,ValueType::FIXED);
	containerCanvas.SetBGColor(0,0,0);

	sceneCanvas.SetMargin(MARGIN_LEFT,0.0f);
	sceneCanvas.SetMargin(MARGIN_TOP,0.0f);
	sceneCanvas.SetMargin(MARGIN_RIGHT,0.0f);
	sceneCanvas.SetMargin(MARGIN_BOTTOM,0.0f);
	sceneCanvas.SetBGColor(0,0,0);
	sceneCanvas.SetTexture(_scene.GetTexture());
	sceneCanvas.SetCallbackContext(this);
	sceneCanvas.SetResizeCallback(CanvasResizeCallback);
	sceneCanvas.SetRenderCallback(CanvasRenderCallback);
	sceneCanvas.SetUpdateCallback(CanvasUpdateCallback);
	sceneCanvas.SetKeyInputCallback(CanvasKeyCallback);
	sceneCanvas.SetMouseMoveCallback(CanvasMouseMoveCallback);

	containerCanvas.AddControl(&sceneCanvas);

	containerApplication.AddControl(&containerCanvas);


	
	mainButtonsGroup.SetCallback(MainButtonsNeonGUICallback);

	containerMainButtons.SetMargin(MARGIN_BOTTOM,250.0f,ValueType::FIXED);
	containerMainButtons.SetMargin(MARGIN_LEFT,250.0f,ValueType::FIXED);
	containerMainButtons.SetMargin(MARGIN_RIGHT,500.0f,ValueType::FIXED);
	containerMainButtons.SetHeight(60.0f,ValueType::FIXED);
	containerMainButtons.SetBGColor(0,0,0);

	containerApplication.AddControl(&containerMainButtons);

	meshButton.SetMargin(MARGIN_BOTTOM,0.0f);
	meshButton.SetMargin(MARGIN_TOP,0.0f);
	meshButton.SetMargin(MARGIN_LEFT,0.0f,ValueType::RELATIVE);
	meshButton.SetMargin(MARGIN_RIGHT,0.8f,ValueType::RELATIVE);
	meshButton.SetBGColor(0.1,0.1,0.1);
	meshButton.SetHoverColor(0.2,0.2,0.2);
	meshButton.SetClickColor(0.3,0.3,0.3);
	meshButton.SetText("Meshes");
	meshButton.SetTextColor(0,1,0);
	meshButton.SetTextSize(30.0,ValueType::FIXED);
	meshButton.SetCallBackContext(this);
	meshButton.SetIndex(0);

	containerMainButtons.AddControl(&meshButton);
	mainButtonsGroup.AddToGroup(&meshButton);

	textureButton.SetMargin(MARGIN_BOTTOM,0.0f);
	textureButton.SetMargin(MARGIN_TOP,0.0f);
	textureButton.SetMargin(MARGIN_LEFT,0.2f,ValueType::RELATIVE);
	textureButton.SetMargin(MARGIN_RIGHT,0.6f,ValueType::RELATIVE);
	textureButton.SetBGColor(0.1,0.1,0.1);
	textureButton.SetHoverColor(0.2,0.2,0.2);
	textureButton.SetClickColor(0.3,0.3,0.3);
	textureButton.SetText("Textures");
	textureButton.SetTextSize(30.0f);
	textureButton.SetTextColor(0,1,0);
	textureButton.SetCallBackContext(this);
	textureButton.SetIndex(1);

	containerMainButtons.AddControl(&textureButton);
	mainButtonsGroup.AddToGroup(&textureButton);

	
	materialButton.SetMargin(MARGIN_BOTTOM,0.0f);
	materialButton.SetMargin(MARGIN_TOP,0.0f);
	materialButton.SetMargin(MARGIN_LEFT,0.4f,ValueType::RELATIVE);
	materialButton.SetMargin(MARGIN_RIGHT,0.4f,ValueType::RELATIVE);
	materialButton.SetBGColor(0.1,0.1,0.1);
	materialButton.SetHoverColor(0.2,0.2,0.2);
	materialButton.SetClickColor(0.3,0.3,0.3);
	materialButton.SetText("Materials");
	materialButton.SetTextColor(0.0,1,0);
	materialButton.SetTextSize(30.0f);
	materialButton.SetCallBackContext(this);
	materialButton.SetIndex(2);

	containerMainButtons.AddControl(&materialButton);
	mainButtonsGroup.AddToGroup(&materialButton);

	
	modelButton.SetMargin(MARGIN_BOTTOM,0.0f);
	modelButton.SetMargin(MARGIN_TOP,0.0f);
	modelButton.SetMargin(MARGIN_LEFT,0.6f,ValueType::RELATIVE);
	modelButton.SetMargin(MARGIN_RIGHT,0.2f,ValueType::RELATIVE);
	modelButton.SetBGColor(0.1,0.1,0.1);
	modelButton.SetHoverColor(0.2,0.2,0.2);
	modelButton.SetClickColor(0.3,0.3,0.3);
	modelButton.SetText("Models");
	modelButton.SetTextSize(30.0f,ValueType::FIXED);
	modelButton.SetTextColor(0,1,0);
	modelButton.SetCallBackContext(this);
	modelButton.SetIndex(3);

	containerMainButtons.AddControl(&modelButton);
	mainButtonsGroup.AddToGroup(&modelButton);

	
	objectButton.SetMargin(MARGIN_BOTTOM,0.0f);
	objectButton.SetMargin(MARGIN_TOP,0.0f);
	objectButton.SetMargin(MARGIN_LEFT,0.8f,ValueType::RELATIVE);
	objectButton.SetMargin(MARGIN_RIGHT,0.0f,ValueType::RELATIVE);
	objectButton.SetBGColor(0.1,0.1,0.1);
	objectButton.SetHoverColor(0.2,0.2,0.2);
	objectButton.SetClickColor(0.3,0.3,0.3);
	objectButton.SetText("Objects");
	objectButton.SetTextColor(0.0,1,0.0);
	objectButton.SetTextSize(30.0f,ValueType::FIXED);
	objectButton.SetCallBackContext(this);
	objectButton.SetIndex(4);

	containerMainButtons.AddControl(&objectButton);
	mainButtonsGroup.AddToGroup(&objectButton);





	//Top Controls
	containerFileMenu.SetMargin(MARGIN_LEFT,0.0f);
	containerFileMenu.SetMargin(MARGIN_TOP,0.0f);
	containerFileMenu.SetHeight(50.0f);
	containerFileMenu.SetWidth(250.0f);
	containerFileMenu.SetBGColor(0.15,0.15,0.15 );
    
	containerApplication.AddControl(&containerFileMenu);



	containerExecutionStats.SetMargin(MARGIN_LEFT,250.0f);
	containerExecutionStats.SetMargin(MARGIN_RIGHT,500.0f);
	containerExecutionStats.SetMargin(MARGIN_TOP,0.0f);
	containerExecutionStats.SetHeight(50.0f);
	containerExecutionStats.SetBGColor(0.1f,0.1f,0.1f);

	containerApplication.AddControl(&containerExecutionStats);


	containerExtreSettings.SetMargin(MARGIN_RIGHT,0.0f);
	containerExtreSettings.SetMargin(MARGIN_TOP,0.0f);
	containerExtreSettings.SetHeight(50.0f);
	containerExtreSettings.SetWidth(500.0f);
	containerExtreSettings.SetBGColor(0.15,0.15,0.15);
    
	containerApplication.AddControl(&containerExtreSettings);

	m_GUI.AddControl(&containerApplication);



	containerFileSelection.SetMargin(MARGIN_TOP,0.0f);
	containerFileSelection.SetMargin(MARGIN_BOTTOM,0.0f);
	containerFileSelection.SetMargin(MARGIN_LEFT,0.0f);
	containerFileSelection.SetMargin(MARGIN_RIGHT,0.0f);
	containerFileSelection.SetBGColor(0.1,0.1,0.1);

	panelFileSelection.SetMargin(MARGIN_TOP,100.0f);
	panelFileSelection.SetWidth(700.0f);
	panelFileSelection.SetMargin(MARGIN_BOTTOM,300.0f);
	panelFileSelection.SetChildrenWidth(1.0f,ValueType::RELATIVE);
	panelFileSelection.SetChildrenHeight(70.0f);
	panelFileSelection.SetBGColor(0.15,0.15,0.15);
	panelFileSelection.SetLayoutType(LAYOUT_VERTICAL);
	panelFileSelection.SetScrollbarSize(30.0f);
	panelFileSelection.SetScrollBarBaseBGColor(0.2,0.2,0.2);
	panelFileSelection.SetScrollBarBaseColor(0.3,0.3,0.3);
	panelFileSelection.SetScrollBarHoveredColor(0.35,0.35,0.35);
	panelFileSelection.SetScrollBarClickedColor(0.4,0.4,0.4);

	containerFileSelection.AddControl(&panelFileSelection);

	fileSelectionButtonsGroup.SetCallback(FileSelectionMenuItemCallback);

	containerLoadCancelButtons.SetMargin(MARGIN_BOTTOM,125.0f);
	containerLoadCancelButtons.SetHeight(50.0f);
	containerLoadCancelButtons.SetWidth(600.0f);
	containerLoadCancelButtons.SetBGColor(0.1,0.1,0.1);

	containerFileSelection.AddControl(&containerLoadCancelButtons);

	buttonLoadFile.SetMargin(MARGIN_BOTTOM,0.0f);
	buttonLoadFile.SetMargin(MARGIN_TOP,0.0f);
	buttonLoadFile.SetMargin(MARGIN_LEFT,0.0f);
	buttonLoadFile.SetWidth(150.0f);
	buttonLoadFile.SetBGColor(0.2,0.2,0.2);
	buttonLoadFile.SetHoverColor(0.0,0.8,0.0);
	buttonLoadFile.SetClickColor(0.2,0.2,0.22);
	buttonLoadFile.SetText("Load");
	buttonLoadFile.SetTextColor(0,0,0);
	buttonLoadFile.SetCallBackContext(this),
	buttonLoadFile.SetCallback(FileSelectionMenuLoadButtonCallback);

	containerLoadCancelButtons.AddControl(&buttonLoadFile);

	buttonCancel.SetMargin(MARGIN_BOTTOM,0.0f);
	buttonCancel.SetMargin(MARGIN_TOP,0.0f);
	buttonCancel.SetMargin(MARGIN_RIGHT,0.0f);
	buttonCancel.SetWidth(150.0f);
	buttonCancel.SetBGColor(0.2,0.2,0.2);
	buttonCancel.SetHoverColor(0.8,0.0,0.0);
	buttonCancel.SetClickColor(0.2,0.2,0.2);
	buttonCancel.SetText("Cancel");
	buttonCancel.SetTextColor(0,0,0);
	buttonCancel.SetCallBackContext(this),
	buttonCancel.SetCallback(FileSelectionMenuCancelButtonCallback);

	containerLoadCancelButtons.AddControl(&buttonCancel);

	m_GUI.AddControl(&containerFileSelection);

	containerFileSelection.SetInactive();


	//camera.Init(glm::vec3(0, 2, 3), glm::vec3(0,0,0), glm::vec3(0, 1, 0),windowWidth,windowHeight);


	//Get some models going
    /*
	int robotTextureIndex = virtualWorld.LoadTexture("res/textures/Robot_Texture.png");
	int robotMaterialIndex = virtualWorld.AddMaterial(robotTextureIndex, 1.0f, 0.1f, false,1.45f,0.7f);
	int robotMeshIndex = virtualWorld.LoadMesh("res/assets/head.obj", robotMaterialIndex);
	virtualWorld.AddObject(robotMeshIndex, glm::vec3(15,0,0),glm::vec3(0,0,0),glm::vec3(1,1,1));

	int suzanneTextureIndex = virtualWorld.LoadTexture("res/textures/wood.jpg");
	int suzanneMaterialIndex = virtualWorld.AddMaterial(suzanneTextureIndex, 1.0f, 0.1f,false, 1.45f, 0.7f);
	int suzanneMeshIndex = virtualWorld.LoadMesh("res/assets/Suzanne.obj", suzanneMaterialIndex);
	virtualWorld.AddObject(suzanneMeshIndex, glm::vec3(0, 0, 12), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));

	int ballTextureIndex = virtualWorld.LoadTexture("res/textures/MarbleBall.png");
	int ballMaterialIndex = virtualWorld.AddMaterial(ballTextureIndex, 0.0f, 0.1f, true, 1.45f, 0.7f);
	int balltMeshIndex = virtualWorld.LoadMesh("res/assets/MarbleBall.obj", ballMaterialIndex);
	virtualWorld.AddObject(balltMeshIndex, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));

	int houseTextureIndex = virtualWorld.LoadTexture("res/textures/old_house.png");
	int houseMaterialIndex = virtualWorld.AddMaterial(houseTextureIndex, 1.0f, 0.1f, false, 1.45f, 0.7f);
	int houseMeshIndex = virtualWorld.LoadMesh("res/assets/old_house.obj", houseMaterialIndex);
	virtualWorld.AddObject(houseMeshIndex, glm::vec3(-13, 0, 0), glm::vec3(-90.f, 0, 0), glm::vec3(2, 2,2));

	virtualWorld.Init(windowWidth, windowHeight,&camera);
	*/
	
}


void App::GlfwInit(int windowWidth, int windowHeight, const char *windowTitle)
{
	if (!glfwInit())
	{
		std::cout << "[GLFW Error]: Initialization of GLFW failed!" << std::endl;
		return;
	}
	else
	{
		std::cout << "Successfully initialized GLFW!" << std::endl;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	

	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;
	m_window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, NULL, NULL);
	if (!m_window)
	{
		std::cout << "[GLFW Error]: Window creation failed!" << std::endl;
		glfwTerminate();
		return;
	}
	else
	{
		std::cout << "Window creation succesful!" << std::endl;
	}

	glfwSetWindowSizeLimits(m_window, windowWidth, windowHeight, GLFW_DONT_CARE, GLFW_DONT_CARE);

	glfwMakeContextCurrent(m_window);

	glfwSwapInterval(1);

	//Sets the current App context as the window user pointer
	glfwSetWindowUserPointer(m_window, this);

	glfwSetInputMode(m_window, GLFW_LOCK_KEY_MODS, GLFW_TRUE);

	//glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //Callbacks
	glfwSetWindowSizeCallback(m_window, App::WindowSizeCallback);
	glfwSetCursorPosCallback(m_window, App::CursorPosCallback);
	glfwSetKeyCallback(m_window, App::KeyCallback);
	glfwSetMouseButtonCallback(m_window, App::MouseButtonCallback);
	glfwSetScrollCallback(m_window, App::ScrollCallback);
	/*
	
	glfwSetWindowIconifyCallback(m_window, App::WindowIconifiedCallback);
	*/
}

void App::GladInit(int windowWidth, int windowHeight)
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout<<"[GLAD ERROR]: Failed to load GLAD libraries!\n";
        return;
    }
    else
    {
        std::cout<<"Initialization of GLAD was succesful!\n";
    }

	//Printing the current OpenGL version on the terminal
	std::cout << "Current OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    glViewport(0, 0, windowWidth, windowHeight);
	glEnable(GL_CULL_FACE);

	//glDisable(GL_CULL_FACE);
	
	//glEnable(GL_DEPTH_TEST);
}

void App::ImGuiInit()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

App::~App()
{
	glFinish();

	glfwMakeContextCurrent(nullptr);
	glfwDestroyWindow(m_window);
	glfwTerminate();

	
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	
}

void App::Run()
{
	while (!glfwWindowShouldClose(m_window))
	{

		Update();
		Render();
	}
}

void App::Update()
{
	float currentFrameTime = glfwGetTime();
	deltaTime = currentFrameTime - lastFrameTime;
	lastFrameTime = currentFrameTime;

	timeSinceStart += deltaTime;

	m_GUI.Update(deltaTime);

	//std::cout<<1.0f/deltaTime<<"\n";

	
	/*
	glm::vec3 direction = glm::vec3(0.0f, 0.0f, 0.0f);
	if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
	{
		direction.z += 1.0f;
	}
	if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
	{
		direction.z -= 1.0f;
	}
	if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
	{
		direction.x += 1.0f;
	}
	if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
	{
		direction.x -= 1.0f;
	}
	if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		direction.y += 1.0f;
	}
	if (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		direction.y -= 1.0f;
	}
	bool isSpeedy = false;

	if (glfwGetKey(m_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		isSpeedy = true;
	}
	

	camera.Move(deltaTime, direction, isSpeedy);

	virtualWorld.Update();
	*/


	//viewTransform = m_camera.GetViewMatrix();

	//int rollSumNumber;

	//Ecs.Update(deltatime, rollSumNumber);

	//rollSumAmountString = IntegerToString(rollSumNumber);

	//std::cout << rollSumAmountString << std::endl;

	/*
	AABB debugBox = layoutPanelBottom.GetChildrenBox();
	std::cout<<debugBox.min.x << " " << debugBox.min.y << '\n';
	std::cout<<debugBox.max.x << " " << debugBox.max.y << '\n';
	*/
	//std::cout<<layoutPanelBottom.GetIsObstructed()<< '\n';

	//Gui.Update(deltatime);
	glfwPollEvents();
}

void App::Render()
{
	glClearColor(0.0,0.0f,0.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//std::cout<<"Rendering\n";
	//virtualWorld.Render(timeSinceStart);

	//glDisable(GL_DEPTH_TEST);
	//m_renderer2d.RenderRectangle({0.5,0.5,0.4,0.4},glm::vec3(0.0f,0.0f,1.0f));
	//glEnable(GL_DEPTH_TEST);
	//opengl boiler plate


	//toggling the main base shader


	//Setting up universal shader unifroms

	//std::cout << isWindowMinimized << std::endl;
	/*
	if (!isWindowMinimized)
	{
		projectionTransform = m_camera.GetPerspectiveMatrix((float)m_windowWidth / (float)m_windowHeight, 0.1f, 1000.0f);


		Ecs.Render(viewTransform, projectionTransform, m_camera.GetPosition(), (float)m_windowWidth, (float)m_windowHeight);

		if (showGui)
		{
			Gui.Render();
		}



		//final backbuffer swap
		
	}
	*/
	//ValueType::AUTO start = std::chrono::steady_clock::now();

	
	m_GUI.Render();

	/*
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();


	ImGui::Begin("Interface");

	ImGui::Text("Hello DearImGui!");

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	*/

	glfwSwapBuffers(m_window);

	//ValueType::AUTO end = std::chrono::steady_clock::now();
	//ValueType::AUTO diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	//std::cout<<"App:" << diff.count() << std::endl;


	
}

void App::RepopulateFileSelectionPanel()
{
	std::vector<Control*>& children = panelFileSelection.GetChildren();
	for(int i=0;i<children.size();++i)
	{
		delete children[i];
	}

	children.clear();

	for(int i=0;i<_loadedDirectorySpecificFilenames.size();++i)
	{
		RadioButton* newFilenameButton = new RadioButton();
		newFilenameButton->SetCallBackContext(this);
		newFilenameButton->SetBGColor(0.2,0.2,0.2);
		newFilenameButton->SetHoverColor(0.3,0.3,0.3);
		newFilenameButton->SetClickColor(0.4,0.4,0.4);
		newFilenameButton->SetText(_loadedDirectorySpecificFilenames[i]);
		newFilenameButton->SetTextColor(1,1,1);
		fileSelectionButtonsGroup.AddToGroup(newFilenameButton);

		panelFileSelection.AddControl(newFilenameButton);
	}
}

void App::WindowSizeCallback(GLFWwindow* window, int width, int height)
{
	App* app = (App*)glfwGetWindowUserPointer(window);
	app->m_windowWidth = width;
	app->m_windowHeight = height;

	if (width != 0 && height != 0)
	{
		glViewport(0, 0, width, height);
		app->m_GUI.Resize(width,height);

		app->Render();
		//app->virtualWorld.Resize(width, height);
		//app->camera.Resize(width, height);
	}
}

void App::CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
	App* app = (App*)glfwGetWindowUserPointer(window);

	app->m_GUI.MouseMove(xpos,ypos);

	/*
	if (app->is_free_cam)
	{
		if (app->is_mouse_first_pos)
		{
			app->lastMouseX = xpos;
			app->lastMouseY = ypos;
			app->is_mouse_first_pos = false;
			//std::cout << "First mouse pos: (x: " << xpos << " y: " << ypos << " )\n";
		}
		else
		{
			float dx = xpos - app->lastMouseX;
			float dy = app->lastMouseY - ypos;

			//std::cout << "Delta mouse pos: (x: " << dx << " y: " << dy << " )\n";

			app->lastMouseX = xpos;
			app->lastMouseY = ypos;

			//app->camera.Rotate(dx, dy);

		}
	}
	
	*/
	


}

void App::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	App* app = (App*)glfwGetWindowUserPointer(window);

	/*
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		if (app->is_free_cam == false)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			app->is_free_cam = true;
			app->is_mouse_first_pos = true;

			double x, y;
			glfwGetCursorPos(window, &x, &y);

			app->lastMouseX = x;
			app->lastMouseY = y;

			
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			app->is_free_cam = false;
		}


	}
		*/

		/*
	if (key == GLFW_KEY_T && action == GLFW_PRESS)
	{
		//app->virtualWorld.SwitchRenderMode();


	}

	if (key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		//app->virtualWorld.debugDepth += 1;
	}

	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		//app->virtualWorld.debugDepth -= 1;
	}

	if(action != GLFW_RELEASE)
	{
		//std::cout<<"Key is being pressed: " << key << "\n";
	
		
	}*/

	app->m_GUI.KeyInput(key,action,mods);

	
	
}

void App::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	App* app = (App*)glfwGetWindowUserPointer(window);

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		//app->Ecs.AddEntity(ICOSAHEDRON);

		//app->Ecs.Update(0.05f);
		double x; //= //(int)(app->last_mouse_x + app->m_windowWidth);
		double y;// = (int)(app->last_mouse_y + app->m_windowHeight);
		glfwGetCursorPos(window, &x, &y);

		std::cout<<"Left mouse button clicked\n";
		app->m_GUI.MouseClick(0,0);
		
		//std::cout << "Picking at: (x :"<< x<< " y: " << y<< ")\n";
		//app->virtualWorld.Pick((int)x, (int)y);
	}
	else if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		std::cout<<"Left mouse button released!\n";
		app->m_GUI.MouseClick(0,1);
		
	}
}

void App::ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
	App* app = (App*)glfwGetWindowUserPointer(window);

	app->m_GUI.MouseWheel(yoffset,yoffset>0 ? 1 : -1);
}



void App::MainButtonsNeonGUICallback(void *context, int index)
{
	App* app = (App*)context;
	//std::cout<<"A RadioButton has been clicked with id: " << index << '\n';

	app->containerMeshData.SetInactive();
	app->containerTextureData.SetInactive();
	app->containerMaterialData.SetInactive();
	app->containerModelData.SetInactive();
	app->containerObjectData.SetInactive();

	app->meshPanel.SetInactive();
	app->texturePanel.SetInactive();
	app->materialPanel.SetInactive();
	app->modelPanel.SetInactive();
	app->objectPanel.SetInactive();

	if(index == 0)
	{
		app->containerMeshData.SetActive();
		app->meshPanel.SetActive();
	}
	else if(index==1)
	{
		app->containerTextureData.SetActive();
		app->texturePanel.SetActive();
	}
	else if(index==2)
	{
		app->containerMaterialData.SetActive();
		app->materialPanel.SetActive();
	}
	else if(index==3)
	{
		app->containerModelData.SetActive();
		app->modelPanel.SetActive();
	}
	else
	{
		app->containerObjectData.SetActive();
		app->objectPanel.SetActive();
	}
}

void App::CanvasResizeCallback(void *context, int newW, int newH)
{
	App* app = (App*)context;

	app->_scene.Resize(newW,newH);
}

void App::CanvasRenderCallback(void *context)
{
	App* app = (App*)context;

	app->_scene.Render();

	//THis shit is very importantus
	glViewport(0,0,app->m_windowWidth,app->m_windowHeight);
}

void App::CanvasUpdateCallback(void *context, float deltaTime)
{
	App* app = (App*)context;

	app->_scene.Update(deltaTime);
}

void App::CanvasKeyCallback(void *context, int key, int action, int mods)
{
	App* app = (App*)context;

	app->_scene.KeyInput(key,action,mods);

	if(key == 70 && action == 1) // F pressed
	{
		if(!app->isMouseDisabled)
		{
			app->isMouseDisabled = true;
			glfwSetInputMode(app->m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			app->isMouseDisabled = false;
			glfwSetInputMode(app->m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		app->m_GUI.ToggleMouseEnabledDisabled();
	}
}

void App::CanvasMouseMoveCallback(void *context, float newX, float newY)
{
	App* app = (App*)context;

	app->_scene.MouseMove(newX,newY);
}


void App::LoadMeshButtonCallback(void *context)
{
	App* app = (App*)context;

	std::string meshFolderRelativePath = "assets/models/"; // For now its this
	std::vector<std::string> acceptableExtensions = {".obj"};

	GetFileNamesWithSpecificExtension(meshFolderRelativePath,acceptableExtensions,app->_loadedDirectorySpecificFilenames);

	if(app->_loadedDirectorySpecificFilenames.size()==0) return;

	//app->containerFileSelection.MouseMove();
	

	app->containerFileSelection.SetActive();
	app->containerApplication.SetInactive();

	app->buttonCancel.Click(0,1);
	app->buttonCancel.MouseMove();
	app->buttonLoadFile.Click(0,1);
	app->buttonLoadFile.MouseMove();
	

	app->_viewState = AppViewState::VIEWSTATE_MESHLOAD;

	app->RepopulateFileSelectionPanel();
}

void App::FileSelectionMenuCancelButtonCallback(void *context)
{
	App* app = (App*)context;
	
	

	app->containerFileSelection.SetInactive();
	app->containerApplication.SetActive();

	app->buttonLoadMesh.Click(0,1);
	app->buttonLoadMesh.MouseMove();

	app->_viewState = AppViewState::VIEWSTATE_MAINMENU;
}

void App::FileSelectionMenuItemCallback(void *context, int index)
{

}

void App::FileSelectionMenuLoadButtonCallback(void *context)
{
}


