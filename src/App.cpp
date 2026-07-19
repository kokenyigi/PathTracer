#include "App.h"

#include <iostream>
#include <chrono>

#include <fstream>
#include <sstream>

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

	buttonNewScene.SetMargin(MARGIN_TOP,10.0f);
	buttonNewScene.SetMargin(MARGIN_LEFT,10.0f);
	buttonNewScene.SetHeight(40.0f);
	buttonNewScene.SetWidth(80.0f);
	buttonNewScene.SetBGColor(0.4,0.4,0.4);
	buttonNewScene.SetHoverColor(0.5,0.5,0.5);
	buttonNewScene.SetClickColor(0.6,0.6,0.6);
	buttonNewScene.SetTextColor(1,1,1);
	buttonNewScene.SetText("NEW");
	buttonNewScene.SetCallBackContext(this);
	buttonNewScene.SetCallback(NewSceneButtonCallback);
	containerLeft.AddControl(&buttonNewScene);


	buttonSaveScene.SetMargin(MARGIN_TOP,60.0f);
	buttonSaveScene.SetMargin(MARGIN_LEFT,10.0f);
	buttonSaveScene.SetHeight(40.0f);
	buttonSaveScene.SetWidth(80.0f);
	buttonSaveScene.SetBGColor(0.4,0.4,0.4);
	buttonSaveScene.SetHoverColor(0.5,0.5,0.5);
	buttonSaveScene.SetClickColor(0.6,0.6,0.6);
	buttonSaveScene.SetTextColor(1,1,1);
	buttonSaveScene.SetText("SAVE");
	buttonSaveScene.SetCallBackContext(this);
	buttonSaveScene.SetCallback(SaveSceneButtonCallback);
	containerLeft.AddControl(&buttonSaveScene);


	buttonLoadScene.SetMargin(MARGIN_TOP,110.0f);
	buttonLoadScene.SetMargin(MARGIN_LEFT,10.0f);
	buttonLoadScene.SetHeight(40.0f);
	buttonLoadScene.SetWidth(80.0f);
	buttonLoadScene.SetBGColor(0.4,0.4,0.4);
	buttonLoadScene.SetHoverColor(0.5,0.5,0.5);
	buttonLoadScene.SetClickColor(0.6,0.6,0.6);
	buttonLoadScene.SetTextColor(1,1,1);
	buttonLoadScene.SetText("LOAD");
	buttonLoadScene.SetCallBackContext(this);
	buttonLoadScene.SetCallback(LoadSceneButtonCallback);
	containerLeft.AddControl(&buttonLoadScene);

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

	labelVertexCountNumber.SetMargin(MARGIN_TOP,0.0);
	labelVertexCountNumber.SetMargin(MARGIN_LEFT,300.0);
	labelVertexCountNumber.SetHeight(30.0f);
	labelVertexCountNumber.SetWidth(200.0f);
	labelVertexCountNumber.SetText("");
	labelVertexCountNumber.SetTextColor(1,1,1);

	containerMeshData.AddControl(&labelVertexCountNumber);

	labelTriangleCount.SetMargin(MARGIN_TOP,60.0f);
	labelTriangleCount.SetMargin(MARGIN_LEFT,0.0f);
	labelTriangleCount.SetHeight(30.0f);
	labelTriangleCount.SetWidth(300.0f);
	labelTriangleCount.SetText("Triangle count: ");
	labelTriangleCount.SetTextColor(1,1,1);

	containerMeshData.AddControl(&labelTriangleCount);

	labelTriangleCountNumber.SetMargin(MARGIN_TOP,60.0);
	labelTriangleCountNumber.SetMargin(MARGIN_LEFT,300.0);
	labelTriangleCountNumber.SetHeight(30.0f);
	labelTriangleCountNumber.SetWidth(200.0f);
	labelTriangleCountNumber.SetText("");
	labelTriangleCountNumber.SetTextColor(1,1,1);

	containerMeshData.AddControl(&labelTriangleCountNumber);

	labelBVHNodeCount.SetMargin(MARGIN_TOP,120.0f);
	labelBVHNodeCount.SetMargin(MARGIN_LEFT,0.0f);
	labelBVHNodeCount.SetHeight(30.0f);
	labelBVHNodeCount.SetWidth(300.0f);
	labelBVHNodeCount.SetText("BVH node count: ");
	labelBVHNodeCount.SetTextColor(1,1,1);

	containerMeshData.AddControl(&labelBVHNodeCount);

	labelBVHNodeCountNumber.SetMargin(MARGIN_TOP,120.0);
	labelBVHNodeCountNumber.SetMargin(MARGIN_LEFT,300.0);
	labelBVHNodeCountNumber.SetHeight(30.0f);
	labelBVHNodeCountNumber.SetWidth(200.0f);
	labelBVHNodeCountNumber.SetText("");
	labelBVHNodeCountNumber.SetTextColor(1,1,1);

	containerMeshData.AddControl(&labelBVHNodeCountNumber);

	
	labelBVHDepth.SetMargin(MARGIN_TOP,180.0f);
	labelBVHDepth.SetMargin(MARGIN_LEFT,0.0f);
	labelBVHDepth.SetHeight(30.0f);
	labelBVHDepth.SetWidth(300.0f);
	labelBVHDepth.SetText("BVH depth: ");
	labelBVHDepth.SetTextColor(1,1,1);

	labelBVHDepthNumber.SetMargin(MARGIN_TOP,180.0);
	labelBVHDepthNumber.SetMargin(MARGIN_LEFT,300.0);
	labelBVHDepthNumber.SetHeight(30.0f);
	labelBVHDepthNumber.SetWidth(200.0f);
	labelBVHDepthNumber.SetText("");
	labelBVHDepthNumber.SetTextColor(1,1,1);

	containerMeshData.AddControl(&labelBVHDepthNumber);

	containerMeshData.AddControl(&labelBVHDepth);
	
	containerRight.AddControl(&containerMeshData);

	containerMeshData.SetInactive();

	containerTextureData.SetMargin(MARGIN_TOP,10.0);
	containerTextureData.SetMargin(MARGIN_LEFT,10.0);
	containerTextureData.SetMargin(MARGIN_RIGHT,10.0);
	containerTextureData.SetMargin(MARGIN_TOP,10.0);
	containerTextureData.SetBGColor(0.2,0.2,0.2);

	labelHeight.SetMargin(MARGIN_TOP,60.0f);
	labelHeight.SetMargin(MARGIN_LEFT,0.0f);
	labelHeight.SetHeight(30.0f);
	labelHeight.SetWidth(200.0f);
	labelHeight.SetText("Height: ");
	labelHeight.SetTextColor(1,1,1);
	
	containerTextureData.AddControl(&labelHeight);

	labelTextureHeightNumber.SetMargin(MARGIN_TOP,60.0f);
	labelTextureHeightNumber.SetMargin(MARGIN_LEFT,200.0f);
	labelTextureHeightNumber.SetHeight(30.0f);
	labelTextureHeightNumber.SetWidth(200.0f);
	labelTextureHeightNumber.SetText("");
	labelTextureHeightNumber.SetTextColor(1,1,1);
	
	containerTextureData.AddControl(&labelTextureHeightNumber);

	labelWidth.SetMargin(MARGIN_TOP,0.0f);
	labelWidth.SetMargin(MARGIN_LEFT,0.0f);
	labelWidth.SetHeight(30.0f);
	labelWidth.SetWidth(200.0f);
	labelWidth.SetText("Width: ");
	labelWidth.SetTextColor(1,1,1);
	
	containerTextureData.AddControl(&labelWidth);

	labelTextureWidthNumber.SetMargin(MARGIN_TOP,0.0f);
	labelTextureWidthNumber.SetMargin(MARGIN_LEFT,200.0f);
	labelTextureWidthNumber.SetHeight(30.0f);
	labelTextureWidthNumber.SetWidth(200.0f);
	labelTextureWidthNumber.SetText("");
	labelTextureWidthNumber.SetTextColor(1,1,1);
	
	containerTextureData.AddControl(&labelTextureWidthNumber);

	containerRight.AddControl(&containerTextureData);

	containerTextureData.SetInactive();


	/**
	 * Down here we define all of the controls that our lovely materialData container has.
	 */
	containerMaterialData.SetMargin(MARGIN_TOP,10.0);
	containerMaterialData.SetMargin(MARGIN_LEFT,10.0);
	containerMaterialData.SetMargin(MARGIN_RIGHT,10.0);
	containerMaterialData.SetMargin(MARGIN_TOP,10.0);
	containerMaterialData.SetBGColor(0.2,0.2,0.2);

	labelTexture.SetMargin(MARGIN_TOP,10.0f);
	labelTexture.SetMargin(MARGIN_LEFT,0.0f);
	labelTexture.SetHeight(30.0f);
	labelTexture.SetWidth(130.0f);
	labelTexture.SetText("Texture: ");
	labelTexture.SetTextColor(1,1,1);
	containerMaterialData.AddControl(&labelTexture);

	dropdownTexture.SetMargin(MARGIN_TOP,5.0f);
	dropdownTexture.SetMargin(MARGIN_LEFT,130.0f);
	dropdownTexture.SetHeight(40.0f);
	dropdownTexture.SetWidth(300.0f);
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
	dropdownTexture.AddOption("none",-1);
	dropdownTexture.SetScrollBarSize(15.0f);
	dropdownTexture.SetCallbackContext(this);
	dropdownTexture.SetCallback(AlbedoTextureChosenCallback);
	containerMaterialData.AddControl(&dropdownTexture);

	labelColor.SetMargin(MARGIN_TOP,60.0f);
	labelColor.SetMargin(MARGIN_LEFT,0.0f);
	labelColor.SetHeight(30.0f);
	labelColor.SetWidth(120.0f);
	labelColor.SetText("Color:");
	labelColor.SetTextColor(1,1,1);
	containerMaterialData.AddControl(&labelColor);


	labelRed.SetMargin(MARGIN_TOP,110.0f);
	labelRed.SetMargin(MARGIN_LEFT,20.0f);
	labelRed.SetHeight(30.0f);
	labelRed.SetWidth(50.0f);
	labelRed.SetTextColor(1,1,1);
	labelRed.SetText("r:");
	containerMaterialData.AddControl(&labelRed);

	inputRed.SetMargin(MARGIN_TOP,105.0f);
	inputRed.SetMargin(MARGIN_LEFT,60.0f);
	inputRed.SetHeight(40.0f);
	inputRed.SetWidth(90.0f);
	inputRed.SetTextColor(0,0,0);
	inputRed.SetEditHeadColor(1,0,0);
	inputRed.SetFloat(0.0f);
	inputRed.SetBGColor(1,1,1);
	inputRed.SetFloatInterval(0.0f,1.0f);
	inputRed.SetCallbackContext(this);
	inputRed.SetFloatCallback(AlbedoRedAlteredCallback);
	containerMaterialData.AddControl(&inputRed);

	sliderRed.SetMargin(MARGIN_TOP,110.0f);
	sliderRed.SetMargin(MARGIN_LEFT,170.0f);
	sliderRed.SetHeight(30.0f);
	sliderRed.SetWidth(200.0f);
	sliderRed.SetSliderHeadSize(30.0f);
	sliderRed.SetSliderBaseBGColor(1,1,1);
	sliderRed.SetSliderBaseColor(0,0,0);
	sliderRed.SetSliderHoveredColor(0.1,0.1,0.1);
	sliderRed.SetSliderClickedColor(1,0,0);
	sliderRed.SetSliderDirection(SLIDER_DIRECTION_HORIZONTAL);
	sliderRed.SetSliderValueChangedCallbackContext(this);
	sliderRed.SetSliderValueChangedCallback(AlbedoRedAlteredCallback);
	containerMaterialData.AddControl(&sliderRed);


	labelGreen.SetMargin(MARGIN_TOP,160.0f);
	labelGreen.SetMargin(MARGIN_LEFT,20.0f);
	labelGreen.SetHeight(30.0f);
	labelGreen.SetWidth(50.0f);
	labelGreen.SetTextColor(1,1,1);
	labelGreen.SetText("g:");
	containerMaterialData.AddControl(&labelGreen);

	inputGreen.SetMargin(MARGIN_TOP,155.0f);
	inputGreen.SetMargin(MARGIN_LEFT,60.0f);
	inputGreen.SetHeight(40.0f);
	inputGreen.SetWidth(90.0f);
	inputGreen.SetTextColor(0,0,0);
	inputGreen.SetEditHeadColor(0,1,0);
	inputGreen.SetFloat(0.0f);
	inputGreen.SetBGColor(1,1,1);
	inputGreen.SetFloatInterval(0.0f,1.0f);
	inputGreen.SetCallbackContext(this);
	inputGreen.SetFloatCallback(AlbedoGreenAlteredCallback);
	containerMaterialData.AddControl(&inputGreen);

	sliderGreen.SetMargin(MARGIN_TOP,160.0f);
	sliderGreen.SetMargin(MARGIN_LEFT,170.0f);
	sliderGreen.SetHeight(30.0f);
	sliderGreen.SetWidth(200.0f);
	sliderGreen.SetSliderHeadSize(30.0f);
	sliderGreen.SetSliderBaseBGColor(1,1,1);
	sliderGreen.SetSliderBaseColor(0,0,0);
	sliderGreen.SetSliderHoveredColor(0.1,0.1,0.1);
	sliderGreen.SetSliderClickedColor(0,1,0);
	sliderGreen.SetSliderDirection(SLIDER_DIRECTION_HORIZONTAL);
	sliderGreen.SetSliderValueChangedCallbackContext(this);
	sliderGreen.SetSliderValueChangedCallback(AlbedoGreenAlteredCallback);
	containerMaterialData.AddControl(&sliderGreen);


	labelBlue.SetMargin(MARGIN_TOP,210.0f);
	labelBlue.SetMargin(MARGIN_LEFT,20.0f);
	labelBlue.SetHeight(30.0f);
	labelBlue.SetWidth(50.0f);
	labelBlue.SetTextColor(1,1,1);
	labelBlue.SetText("b:");
	containerMaterialData.AddControl(&labelBlue);

	inputBlue.SetMargin(MARGIN_TOP,205.0f);
	inputBlue.SetMargin(MARGIN_LEFT,60.0f);
	inputBlue.SetHeight(40.0f);
	inputBlue.SetWidth(90.0f);
	inputBlue.SetTextColor(0,0,0);
	inputBlue.SetEditHeadColor(0,0,1);
	inputBlue.SetFloat(0.0f);
	inputBlue.SetBGColor(1,1,1);
	inputBlue.SetFloatInterval(0.0f,1.0f);
	inputBlue.SetCallbackContext(this);
	inputBlue.SetFloatCallback(AlbedoBlueAlteredCallback);
	containerMaterialData.AddControl(&inputBlue);

	sliderBlue.SetMargin(MARGIN_TOP,210.0f);
	sliderBlue.SetMargin(MARGIN_LEFT,170.0f);
	sliderBlue.SetHeight(30.0f);
	sliderBlue.SetWidth(200.0f);
	sliderBlue.SetSliderHeadSize(30.0f);
	sliderBlue.SetSliderBaseBGColor(1,1,1);
	sliderBlue.SetSliderBaseColor(0,0,0);
	sliderBlue.SetSliderHoveredColor(0.1,0.1,0.1);
	sliderBlue.SetSliderClickedColor(0,0,1);
	sliderBlue.SetSliderDirection(SLIDER_DIRECTION_HORIZONTAL);
	sliderBlue.SetSliderValueChangedCallbackContext(this);
	sliderBlue.SetSliderValueChangedCallback(AlbedoBlueAlteredCallback);
	containerMaterialData.AddControl(&sliderBlue);


	labelMetallic.SetMargin(MARGIN_TOP,260.0f);
	labelMetallic.SetMargin(MARGIN_LEFT,0.0f);
	labelMetallic.SetHeight(30.0f);
	labelMetallic.SetWidth(150.0f);
	labelMetallic.SetTextColor(1,1,1);
	labelMetallic.SetText("Metallic: ");
	containerMaterialData.AddControl(&labelMetallic);

	inputMetallic.SetMargin(MARGIN_TOP,255.0f);
	inputMetallic.SetMargin(MARGIN_LEFT,150.0f);
	inputMetallic.SetHeight(40.0f);
	inputMetallic.SetWidth(90.0f);
	inputMetallic.SetTextColor(0,0,0);
	inputMetallic.SetEditHeadColor(0.5,0.5,0.5);
	inputMetallic.SetFloat(0.0f);
	inputMetallic.SetBGColor(1,1,1);
	inputMetallic.SetFloatInterval(0.0f,1.0f);
	inputMetallic.SetCallbackContext(this);
	inputMetallic.SetFloatCallback(MetallicAlteredCallback);
	containerMaterialData.AddControl(&inputMetallic);

	sliderMetallic.SetMargin(MARGIN_TOP,260.0f);
	sliderMetallic.SetMargin(MARGIN_LEFT,260.0f);
	sliderMetallic.SetHeight(30.0f);
	sliderMetallic.SetWidth(200.0f);
	sliderMetallic.SetSliderHeadSize(30.0f);
	sliderMetallic.SetSliderBaseBGColor(1,1,1);
	sliderMetallic.SetSliderBaseColor(0,0,0);
	sliderMetallic.SetSliderHoveredColor(0.1,0.1,0.1);
	sliderMetallic.SetSliderClickedColor(0.5,0.5,0.5);
	sliderMetallic.SetSliderDirection(SLIDER_DIRECTION_HORIZONTAL);
	sliderMetallic.SetSliderValueChangedCallbackContext(this);
	sliderMetallic.SetSliderValueChangedCallback(MetallicAlteredCallback);
	containerMaterialData.AddControl(&sliderMetallic);


	labelRoughness.SetMargin(MARGIN_TOP,310.0f);
	labelRoughness.SetMargin(MARGIN_LEFT,0.0f);
	labelRoughness.SetHeight(30.0f);
	labelRoughness.SetWidth(150.0f);
	labelRoughness.SetTextColor(1,1,1);
	labelRoughness.SetText("Roughness: ");
	containerMaterialData.AddControl(&labelRoughness);

	inputRoughness.SetMargin(MARGIN_TOP,305.0f);
	inputRoughness.SetMargin(MARGIN_LEFT,160.0f);
	inputRoughness.SetHeight(40.0f);
	inputRoughness.SetWidth(90.0f);
	inputRoughness.SetTextColor(0,0,0);
	inputRoughness.SetEditHeadColor(1,0.5,0.0);
	inputRoughness.SetFloat(0.0f);
	inputRoughness.SetBGColor(1,1,1);
	inputRoughness.SetFloatInterval(0.0f,1.0f);
	inputRoughness.SetCallbackContext(this);
	inputRoughness.SetFloatCallback(RoughnessAlteredCallback);
	containerMaterialData.AddControl(&inputRoughness);

	sliderRoughness.SetMargin(MARGIN_TOP,310.0f);
	sliderRoughness.SetMargin(MARGIN_LEFT,270.0f);
	sliderRoughness.SetHeight(30.0f);
	sliderRoughness.SetWidth(200.0f);
	sliderRoughness.SetSliderHeadSize(30.0f);
	sliderRoughness.SetSliderBaseBGColor(1,1,1);
	sliderRoughness.SetSliderBaseColor(0,0,0);
	sliderRoughness.SetSliderHoveredColor(0.1,0.1,0.1);
	sliderRoughness.SetSliderClickedColor(1,0.5,0.0);
	sliderRoughness.SetSliderDirection(SLIDER_DIRECTION_HORIZONTAL);
	sliderRoughness.SetSliderValueChangedCallbackContext(this);
	sliderRoughness.SetSliderValueChangedCallback(RoughnessAlteredCallback);
	containerMaterialData.AddControl(&sliderRoughness);


	labelTransmission.SetMargin(MARGIN_TOP,360.0f);
	labelTransmission.SetMargin(MARGIN_LEFT,0.0f);
	labelTransmission.SetHeight(30.0f);
	labelTransmission.SetWidth(200.0f);
	labelTransmission.SetTextColor(1,1,1);
	labelTransmission.SetText("Transmission: ");
	containerMaterialData.AddControl(&labelTransmission);

	inputTransmission.SetMargin(MARGIN_TOP,355.0f);
	inputTransmission.SetMargin(MARGIN_LEFT,210.0f);
	inputTransmission.SetHeight(40.0f);
	inputTransmission.SetWidth(90.0f);
	inputTransmission.SetTextColor(0,0,0);
	inputTransmission.SetEditHeadColor(1,0.0,0.6);
	inputTransmission.SetFloat(0.0f);
	inputTransmission.SetBGColor(1,1,1);
	inputTransmission.SetFloatInterval(0.0f,1.0f);
	inputTransmission.SetCallbackContext(this);
	inputTransmission.SetFloatCallback(TransmissionAlteredCallback);
	containerMaterialData.AddControl(&inputTransmission);

	sliderTransmission.SetMargin(MARGIN_TOP,360.0f);
	sliderTransmission.SetMargin(MARGIN_LEFT,320.0f);
	sliderTransmission.SetHeight(30.0f);
	sliderTransmission.SetWidth(160.0f);
	sliderTransmission.SetSliderHeadSize(30.0f);
	sliderTransmission.SetSliderBaseBGColor(1,1,1);
	sliderTransmission.SetSliderBaseColor(0,0,0);
	sliderTransmission.SetSliderHoveredColor(0.1,0.1,0.1);
	sliderTransmission.SetSliderClickedColor(1,0.0,0.6);
	sliderTransmission.SetSliderDirection(SLIDER_DIRECTION_HORIZONTAL);
	sliderTransmission.SetSliderValueChangedCallbackContext(this);
	sliderTransmission.SetSliderValueChangedCallback(TransmissionAlteredCallback);
	containerMaterialData.AddControl(&sliderTransmission);


	labelIoR.SetMargin(MARGIN_TOP,410.0f);
	labelIoR.SetMargin(MARGIN_LEFT,0.0f);
	labelIoR.SetHeight(30.0f);
	labelIoR.SetWidth(60.0f);
	labelIoR.SetTextColor(1,1,1);
	labelIoR.SetText("IoR: ");
	containerMaterialData.AddControl(&labelIoR);

	inputIoR.SetMargin(MARGIN_TOP,405.0f);
	inputIoR.SetMargin(MARGIN_LEFT,80.0f);
	inputIoR.SetHeight(40.0f);
	inputIoR.SetWidth(90.0f);
	inputIoR.SetTextColor(0,0,0);
	inputIoR.SetEditHeadColor(1,0.8,0.0);
	inputIoR.SetFloat(0.0f);
	inputIoR.SetBGColor(1,1,1);
	inputIoR.SetFloatInterval(1.0f,2.5f);
	inputIoR.SetFloat(1.0f);
	inputIoR.SetCallbackContext(this);
	inputIoR.SetFloatCallback(IoRInputCallback);
	containerMaterialData.AddControl(&inputIoR);

	sliderIoR.SetMargin(MARGIN_TOP,410.0f);
	sliderIoR.SetMargin(MARGIN_LEFT,190.0f);
	sliderIoR.SetHeight(30.0f);
	sliderIoR.SetWidth(200.0f);
	sliderIoR.SetSliderHeadSize(30.0f);
	sliderIoR.SetSliderBaseBGColor(1,1,1);
	sliderIoR.SetSliderBaseColor(0,0,0);
	sliderIoR.SetSliderHoveredColor(0.1,0.1,0.1);
	sliderIoR.SetSliderClickedColor(1,0.8,0.0);
	sliderIoR.SetSliderDirection(SLIDER_DIRECTION_HORIZONTAL);
	sliderIoR.SetSliderValueChangedCallbackContext(this);
	sliderIoR.SetSliderValueChangedCallback(IoRSliderCallback);
	containerMaterialData.AddControl(&sliderIoR);


	labelEmissionStrength.SetMargin(MARGIN_TOP,460.0f);
	labelEmissionStrength.SetMargin(MARGIN_LEFT,0.0f);
	labelEmissionStrength.SetHeight(30.0f);
	labelEmissionStrength.SetWidth(190.0f);
	labelEmissionStrength.SetTextColor(1,1,1);
	labelEmissionStrength.SetText("E. Strength: ");
	containerMaterialData.AddControl(&labelEmissionStrength);

	inputEmissionStrength.SetMargin(MARGIN_TOP,455.0f);
	inputEmissionStrength.SetMargin(MARGIN_LEFT,190.0f);
	inputEmissionStrength.SetHeight(40.0f);
	inputEmissionStrength.SetWidth(90.0f);
	inputEmissionStrength.SetTextColor(0,0,0);
	inputEmissionStrength.SetEditHeadColor(0,0.3,0.8);
	inputEmissionStrength.SetFloat(0.0f);
	inputEmissionStrength.SetBGColor(1,1,1);
	inputEmissionStrength.SetFloatInterval(0.0,300.0f);
	inputEmissionStrength.SetCallbackContext(this);
	inputEmissionStrength.SetFloatCallback(EmissionStrengthInputCallback);
	containerMaterialData.AddControl(&inputEmissionStrength);

	sliderEmissionStrength.SetMargin(MARGIN_TOP,460.0f);
	sliderEmissionStrength.SetMargin(MARGIN_LEFT,300.0f);
	sliderEmissionStrength.SetHeight(30.0f);
	sliderEmissionStrength.SetWidth(170.0f);
	sliderEmissionStrength.SetSliderHeadSize(30.0f);
	sliderEmissionStrength.SetSliderBaseBGColor(1,1,1);
	sliderEmissionStrength.SetSliderBaseColor(0,0,0);
	sliderEmissionStrength.SetSliderHoveredColor(0.1,0.1,0.1);
	sliderEmissionStrength.SetSliderClickedColor(0,0.3,0.8);
	sliderEmissionStrength.SetSliderDirection(SLIDER_DIRECTION_HORIZONTAL);
	sliderEmissionStrength.SetSliderValueChangedCallbackContext(this);
	sliderEmissionStrength.SetSliderValueChangedCallback(EmissionStrengthSliderCallback);
	containerMaterialData.AddControl(&sliderEmissionStrength);


	labelEmissionColor.SetMargin(MARGIN_TOP,510.0f);
	labelEmissionColor.SetMargin(MARGIN_LEFT,0.0f);
	labelEmissionColor.SetHeight(30.0f);
	labelEmissionColor.SetWidth(200.0f);
	labelEmissionColor.SetText("E. Color:");
	labelEmissionColor.SetTextColor(1,1,1);
	containerMaterialData.AddControl(&labelEmissionColor);


	labelEmissionRed.SetMargin(MARGIN_TOP,560.0f);
	labelEmissionRed.SetMargin(MARGIN_LEFT,20.0f);
	labelEmissionRed.SetHeight(30.0f);
	labelEmissionRed.SetWidth(50.0f);
	labelEmissionRed.SetTextColor(1,1,1);
	labelEmissionRed.SetText("r:");
	containerMaterialData.AddControl(&labelEmissionRed);

	inputEmissionRed.SetMargin(MARGIN_TOP,555.0f);
	inputEmissionRed.SetMargin(MARGIN_LEFT,60.0f);
	inputEmissionRed.SetHeight(40.0f);
	inputEmissionRed.SetWidth(90.0f);
	inputEmissionRed.SetTextColor(0,0,0);
	inputEmissionRed.SetEditHeadColor(1,0,0);
	inputEmissionRed.SetFloat(0.0f);
	inputEmissionRed.SetBGColor(1,1,1);
	inputEmissionRed.SetFloatInterval(0.0f,1.0f);
	inputEmissionRed.SetCallbackContext(this);
	inputEmissionRed.SetFloatCallback(EmissionRedAlteredCallback);
	containerMaterialData.AddControl(&inputEmissionRed);

	sliderEmissionRed.SetMargin(MARGIN_TOP,560.0f);
	sliderEmissionRed.SetMargin(MARGIN_LEFT,170.0f);
	sliderEmissionRed.SetHeight(30.0f);
	sliderEmissionRed.SetWidth(200.0f);
	sliderEmissionRed.SetSliderHeadSize(30.0f);
	sliderEmissionRed.SetSliderBaseBGColor(1,1,1);
	sliderEmissionRed.SetSliderBaseColor(0,0,0);
	sliderEmissionRed.SetSliderHoveredColor(0.1,0.1,0.1);
	sliderEmissionRed.SetSliderClickedColor(1,0,0);
	sliderEmissionRed.SetSliderDirection(SLIDER_DIRECTION_HORIZONTAL);
	sliderEmissionRed.SetSliderValueChangedCallbackContext(this);
	sliderEmissionRed.SetSliderValueChangedCallback(EmissionRedAlteredCallback);
	containerMaterialData.AddControl(&sliderEmissionRed);


	labelEmissionGreen.SetMargin(MARGIN_TOP,610.0f);
	labelEmissionGreen.SetMargin(MARGIN_LEFT,20.0f);
	labelEmissionGreen.SetHeight(30.0f);
	labelEmissionGreen.SetWidth(50.0f);
	labelEmissionGreen.SetTextColor(1,1,1);
	labelEmissionGreen.SetText("g:");
	containerMaterialData.AddControl(&labelEmissionGreen);

	inputEmissionGreen.SetMargin(MARGIN_TOP,605.0f);
	inputEmissionGreen.SetMargin(MARGIN_LEFT,60.0f);
	inputEmissionGreen.SetHeight(40.0f);
	inputEmissionGreen.SetWidth(90.0f);
	inputEmissionGreen.SetTextColor(0,0,0);
	inputEmissionGreen.SetEditHeadColor(0,1,0);
	inputEmissionGreen.SetFloat(0.0f);
	inputEmissionGreen.SetBGColor(1,1,1);
	inputEmissionGreen.SetFloatInterval(0.0f,1.0f);
	inputEmissionGreen.SetCallbackContext(this);
	inputEmissionGreen.SetFloatCallback(EmissionGreenAlteredCallback);
	containerMaterialData.AddControl(&inputEmissionGreen);

	sliderEmissionGreen.SetMargin(MARGIN_TOP,610.0f);
	sliderEmissionGreen.SetMargin(MARGIN_LEFT,170.0f);
	sliderEmissionGreen.SetHeight(30.0f);
	sliderEmissionGreen.SetWidth(200.0f);
	sliderEmissionGreen.SetSliderHeadSize(30.0f);
	sliderEmissionGreen.SetSliderBaseBGColor(1,1,1);
	sliderEmissionGreen.SetSliderBaseColor(0,0,0);
	sliderEmissionGreen.SetSliderHoveredColor(0.1,0.1,0.1);
	sliderEmissionGreen.SetSliderClickedColor(0,1,0);
	sliderEmissionGreen.SetSliderDirection(SLIDER_DIRECTION_HORIZONTAL);
	sliderEmissionGreen.SetSliderValueChangedCallbackContext(this);
	sliderEmissionGreen.SetSliderValueChangedCallback(EmissionGreenAlteredCallback);
	containerMaterialData.AddControl(&sliderEmissionGreen);


	labelEmissionBlue.SetMargin(MARGIN_TOP,660.0f);
	labelEmissionBlue.SetMargin(MARGIN_LEFT,20.0f);
	labelEmissionBlue.SetHeight(30.0f);
	labelEmissionBlue.SetWidth(50.0f);
	labelEmissionBlue.SetTextColor(1,1,1);
	labelEmissionBlue.SetText("b:");
	containerMaterialData.AddControl(&labelEmissionBlue);

	inputEmissionBlue.SetMargin(MARGIN_TOP,655.0f);
	inputEmissionBlue.SetMargin(MARGIN_LEFT,60.0f);
	inputEmissionBlue.SetHeight(40.0f);
	inputEmissionBlue.SetWidth(90.0f);
	inputEmissionBlue.SetTextColor(0,0,0);
	inputEmissionBlue.SetEditHeadColor(0,0,1);
	inputEmissionBlue.SetFloat(0.0f);
	inputEmissionBlue.SetBGColor(1,1,1);
	inputEmissionBlue.SetFloatInterval(0.0f,1.0f);
	inputEmissionBlue.SetCallbackContext(this);
	inputEmissionBlue.SetFloatCallback(EmissionBlueAlteredCallback);
	containerMaterialData.AddControl(&inputEmissionBlue);

	sliderEmissionBlue.SetMargin(MARGIN_TOP,660.0f);
	sliderEmissionBlue.SetMargin(MARGIN_LEFT,170.0f);
	sliderEmissionBlue.SetHeight(30.0f);
	sliderEmissionBlue.SetWidth(200.0f);
	sliderEmissionBlue.SetSliderHeadSize(30.0f);
	sliderEmissionBlue.SetSliderBaseBGColor(1,1,1);
	sliderEmissionBlue.SetSliderBaseColor(0,0,0);
	sliderEmissionBlue.SetSliderHoveredColor(0.1,0.1,0.1);
	sliderEmissionBlue.SetSliderClickedColor(0,0,1);
	sliderEmissionBlue.SetSliderDirection(SLIDER_DIRECTION_HORIZONTAL);
	sliderEmissionBlue.SetSliderValueChangedCallbackContext(this);
	sliderEmissionBlue.SetSliderValueChangedCallback(EmissionBlueAlteredCallback);
	containerMaterialData.AddControl(&sliderEmissionBlue);
	
	containerRight.AddControl(&containerMaterialData);

	containerMaterialData.SetInactive();

	containerModelData.SetMargin(MARGIN_TOP,10.0);
	containerModelData.SetMargin(MARGIN_LEFT,10.0);
	containerModelData.SetMargin(MARGIN_RIGHT,10.0);
	containerModelData.SetMargin(MARGIN_TOP,10.0);
	containerModelData.SetBGColor(0.2,0.2,0.2);

	labelMesh.SetMargin(MARGIN_TOP,10.0f);
	labelMesh.SetMargin(MARGIN_LEFT,0.0f);
	labelMesh.SetHeight(30.0f);
	labelMesh.SetWidth(100.0f);
	labelMesh.SetText("Mesh: ");
	labelMesh.SetTextColor(1,1,1);

	containerModelData.AddControl(&labelMesh);

	dropdownMesh.SetMargin(MARGIN_TOP,5.0f);
	dropdownMesh.SetMargin(MARGIN_LEFT,100.0f);
	dropdownMesh.SetHeight(40.0f);
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
	//dropdownMesh.AddOption("null",0);
	dropdownMesh.SetScrollBarSize(15.0f);
	dropdownMesh.SetCallbackContext(this);
	dropdownMesh.SetCallback(MeshIndexChosenDropdownCallback);
	dropdownMesh.SetPriority(2);

	containerModelData.AddControl(&dropdownMesh);

	labelMaterial.SetMargin(MARGIN_TOP,60.0f);
	labelMaterial.SetMargin(MARGIN_LEFT,0.0f);
	labelMaterial.SetHeight(30.0f);
	labelMaterial.SetWidth(200.0f);
	labelMaterial.SetText("Material: ");
	labelMaterial.SetTextColor(1,1,1);

	containerModelData.AddControl(&labelMaterial);

	dropdownMaterial.SetMargin(MARGIN_TOP,55.0f);
	dropdownMaterial.SetMargin(MARGIN_LEFT,150.0f);
	dropdownMaterial.SetHeight(40.0f);
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
	//dropdownMaterial.AddOption("null",0);
	dropdownMaterial.SetScrollBarSize(15.0f);
	dropdownMaterial.SetCallbackContext(this);
	dropdownMaterial.SetCallback(MaterialIndexChosenDropdownCallback);

	containerModelData.AddControl(&dropdownMaterial);

	containerRight.AddControl(&containerModelData);

	containerModelData.SetInactive();


	containerObjectData.SetMargin(MARGIN_TOP,10.0);
	containerObjectData.SetMargin(MARGIN_LEFT,10.0);
	containerObjectData.SetMargin(MARGIN_RIGHT,10.0);
	containerObjectData.SetMargin(MARGIN_TOP,10.0);
	containerObjectData.SetBGColor(0.2,0.2,0.2);

	labelModel.SetMargin(MARGIN_TOP,10.0f);
	labelModel.SetMargin(MARGIN_LEFT,0.0f);
	labelModel.SetHeight(30.0f);
	labelModel.SetWidth(170.0f);
	labelModel.SetText("Model: ");
	labelModel.SetTextColor(1,1,1);
	containerObjectData.AddControl(&labelModel);

	dropdownModel.SetMargin(MARGIN_TOP,5.0f);
	dropdownModel.SetMargin(MARGIN_LEFT,180.0f);
	dropdownModel.SetHeight(40.0f);
	dropdownModel.SetWidth(250.0f);
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
	dropdownModel.SetScrollBarSize(15.0f);
	dropdownModel.SetCallbackContext(this);
	dropdownModel.SetCallback(ModelIndexChosenDropdownCallback);
	containerObjectData.AddControl(&dropdownModel);


	labelPosition.SetMargin(MARGIN_TOP,60.0f);
	labelPosition.SetMargin(MARGIN_LEFT,0.0f);
	labelPosition.SetHeight(30.0f);
	labelPosition.SetWidth(240.0f);
	labelPosition.SetText("Position: ");
	labelPosition.SetTextColor(1,1,1);
	containerObjectData.AddControl(&labelPosition);

	labelXPos.SetMargin(MARGIN_TOP,110.0f);
	labelXPos.SetMargin(MARGIN_LEFT,20.0f);
	labelXPos.SetHeight(30.0f);
	labelXPos.SetWidth(50.0f);
	labelXPos.SetText("x:");
	labelXPos.SetTextColor(1,1,1);
	containerObjectData.AddControl(&labelXPos);

	inputXPos.SetMargin(MARGIN_TOP,105.0f);
	inputXPos.SetMargin(MARGIN_LEFT,80.0f);
	inputXPos.SetHeight(40.0f);
	inputXPos.SetWidth(200.0f);
	inputXPos.SetTextColor(0,0,0);
	inputXPos.SetEditHeadColor(1,0,0);
	inputXPos.SetFloat(0.0f);
	inputXPos.SetBGColor(1,1,1);
	inputXPos.SetCallbackContext(this);
	inputXPos.SetFloatCallback(CompTypeAlteredCallback<0,0>);
	containerObjectData.AddControl(&inputXPos);


	labelYPos.SetMargin(MARGIN_TOP,160.0f);
	labelYPos.SetMargin(MARGIN_LEFT,20.0f);
	labelYPos.SetHeight(30.0f);
	labelYPos.SetWidth(50.0f);
	labelYPos.SetText("y:");
	labelYPos.SetTextColor(1,1,1);
	containerObjectData.AddControl(&labelYPos);

	inputYPos.SetMargin(MARGIN_TOP,155.0f);
	inputYPos.SetMargin(MARGIN_LEFT,80.0f);
	inputYPos.SetHeight(40.0f);
	inputYPos.SetWidth(200.0f);
	inputYPos.SetTextColor(0,0,0);
	inputYPos.SetEditHeadColor(0,1,0);
	inputYPos.SetFloat(0.0f);
	inputYPos.SetBGColor(1,1,1);
	inputYPos.SetCallbackContext(this);
	inputYPos.SetFloatCallback(CompTypeAlteredCallback<1,0>);
	containerObjectData.AddControl(&inputYPos);


	labelZPos.SetMargin(MARGIN_TOP,210.0f);
	labelZPos.SetMargin(MARGIN_LEFT,20.0f);
	labelZPos.SetHeight(30.0f);
	labelZPos.SetWidth(50.0f);
	labelZPos.SetText("z:");
	labelZPos.SetTextColor(1,1,1);
	containerObjectData.AddControl(&labelZPos);

	inputZPos.SetMargin(MARGIN_TOP,205.0f);
	inputZPos.SetMargin(MARGIN_LEFT,80.0f);
	inputZPos.SetHeight(40.0f);
	inputZPos.SetWidth(200.0f);
	inputZPos.SetTextColor(0,0,0);
	inputZPos.SetEditHeadColor(0,0,1);
	inputZPos.SetFloat(0.0f);
	inputZPos.SetBGColor(1,1,1);
	inputZPos.SetCallbackContext(this);
	inputZPos.SetFloatCallback(CompTypeAlteredCallback<2,0>);
	containerObjectData.AddControl(&inputZPos);


	labelScale.SetMargin(MARGIN_TOP,260.0f);
	labelScale.SetMargin(MARGIN_LEFT,0.0f);
	labelScale.SetHeight(30.0f);
	labelScale.SetWidth(240.0f);
	labelScale.SetText("Scale: ");
	labelScale.SetTextColor(1,1,1);
	containerObjectData.AddControl(&labelScale);


	labelXScale.SetMargin(MARGIN_TOP,310.0f);
	labelXScale.SetMargin(MARGIN_LEFT,20.0f);
	labelXScale.SetHeight(30.0f);
	labelXScale.SetWidth(50.0f);
	labelXScale.SetText("x:");
	labelXScale.SetTextColor(1,1,1);
	containerObjectData.AddControl(&labelXScale);

	inputXScale.SetMargin(MARGIN_TOP,305.0f);
	inputXScale.SetMargin(MARGIN_LEFT,80.0f);
	inputXScale.SetHeight(40.0f);
	inputXScale.SetWidth(200.0f);
	inputXScale.SetTextColor(0,0,0);
	inputXScale.SetEditHeadColor(1,0,0);
	inputXScale.SetFloat(0.0f);
	inputXScale.SetBGColor(1,1,1);
	inputXScale.SetCallbackContext(this);
	inputXScale.SetFloatInterval(FLT_MIN,FLT_MAX);
	inputXScale.SetFloatCallback(CompTypeAlteredCallback<0,1>);
	containerObjectData.AddControl(&inputXScale);


	labelYScale.SetMargin(MARGIN_TOP,360.0f);
	labelYScale.SetMargin(MARGIN_LEFT,20.0f);
	labelYScale.SetHeight(30.0f);
	labelYScale.SetWidth(50.0f);
	labelYScale.SetText("y:");
	labelYScale.SetTextColor(1,1,1);
	containerObjectData.AddControl(&labelYScale);

	inputYScale.SetMargin(MARGIN_TOP,355.0f);
	inputYScale.SetMargin(MARGIN_LEFT,80.0f);
	inputYScale.SetHeight(40.0f);
	inputYScale.SetWidth(200.0f);
	inputYScale.SetTextColor(0,0,0);
	inputYScale.SetEditHeadColor(0,1,0);
	inputYScale.SetFloat(0.0f);
	inputYScale.SetBGColor(1,1,1);
	inputYScale.SetCallbackContext(this);
	inputYScale.SetFloatInterval(FLT_MIN,FLT_MAX);
	inputYScale.SetFloatCallback(CompTypeAlteredCallback<1,1>);
	containerObjectData.AddControl(&inputYScale);


	labelZScale.SetMargin(MARGIN_TOP,410.0f);
	labelZScale.SetMargin(MARGIN_LEFT,20.0f);
	labelZScale.SetHeight(30.0f);
	labelZScale.SetWidth(50.0f);
	labelZScale.SetText("z:");
	labelZScale.SetTextColor(1,1,1);
	containerObjectData.AddControl(&labelZScale);

	inputZScale.SetMargin(MARGIN_TOP,405.0f);
	inputZScale.SetMargin(MARGIN_LEFT,80.0f);
	inputZScale.SetHeight(40.0f);
	inputZScale.SetWidth(200.0f);
	inputZScale.SetTextColor(0,0,0);
	inputZScale.SetEditHeadColor(0,0,1);
	inputZScale.SetFloat(0.0f);
	inputZScale.SetBGColor(1,1,1);
	inputZScale.SetFloatInterval(FLT_MIN,FLT_MAX);
	inputZScale.SetCallbackContext(this);
	inputZScale.SetFloatCallback(CompTypeAlteredCallback<2,1>);
	containerObjectData.AddControl(&inputZScale);
	

	labelRotation.SetMargin(MARGIN_TOP,460.0f);
	labelRotation.SetMargin(MARGIN_LEFT,0.0f);
	labelRotation.SetHeight(30.0f);
	labelRotation.SetWidth(240.0f);
	labelRotation.SetText("Rotation: ");
	labelRotation.SetTextColor(1,1,1);
	containerObjectData.AddControl(&labelRotation);


	labelXRot.SetMargin(MARGIN_TOP,510.0f);
	labelXRot.SetMargin(MARGIN_LEFT,20.0f);
	labelXRot.SetHeight(30.0f);
	labelXRot.SetWidth(50.0f);
	labelXRot.SetText("x:");
	labelXRot.SetTextColor(1,1,1);
	containerObjectData.AddControl(&labelXRot);

	inputXRot.SetMargin(MARGIN_TOP,505.0f);
	inputXRot.SetMargin(MARGIN_LEFT,80.0f);
	inputXRot.SetHeight(40.0f);
	inputXRot.SetWidth(200.0f);
	inputXRot.SetTextColor(0,0,0);
	inputXRot.SetEditHeadColor(1,0,0);
	inputXRot.SetFloat(0.0f);
	inputXRot.SetBGColor(1,1,1);
	inputXRot.SetFloatInterval(0.0f,360.0f);
	inputXRot.SetCallbackContext(this);
	inputXRot.SetFloatCallback(CompTypeAlteredCallback<0,2>);
	containerObjectData.AddControl(&inputXRot);

	sliderXRot.SetMargin(MARGIN_TOP,510.0f);
	sliderXRot.SetMargin(MARGIN_LEFT,300.0f);
	sliderXRot.SetHeight(30.0f);
	sliderXRot.SetWidth(180.0f);
	sliderXRot.SetSliderHeadSize(30.0f);
	sliderXRot.SetSliderBaseBGColor(1,1,1);
	sliderXRot.SetSliderBaseColor(0,0,0);
	sliderXRot.SetSliderHoveredColor(0.1,0.1,0.1);
	sliderXRot.SetSliderClickedColor(1,0,0);
	sliderXRot.SetSliderDirection(SLIDER_DIRECTION_HORIZONTAL);
	sliderXRot.SetSliderValueChangedCallbackContext(this);
	sliderXRot.SetSliderValueChangedCallback(CompTypeAlteredCallback<0,2,true>);
	containerObjectData.AddControl(&sliderXRot);


	labelYRot.SetMargin(MARGIN_TOP,560.0f);
	labelYRot.SetMargin(MARGIN_LEFT,20.0f);
	labelYRot.SetHeight(30.0f);
	labelYRot.SetWidth(50.0f);
	labelYRot.SetText("y:");
	labelYRot.SetTextColor(1,1,1);
	containerObjectData.AddControl(&labelYRot);

	inputYRot.SetMargin(MARGIN_TOP,555.0f);
	inputYRot.SetMargin(MARGIN_LEFT,80.0f);
	inputYRot.SetHeight(40.0f);
	inputYRot.SetWidth(200.0f);
	inputYRot.SetTextColor(0,0,0);
	inputYRot.SetEditHeadColor(0,1,0);
	inputYRot.SetFloat(0.0f);
	inputYRot.SetBGColor(1,1,1);
	inputYRot.SetFloatInterval(0.0f,360.0f);
	inputYRot.SetCallbackContext(this);
	inputYRot.SetFloatCallback(CompTypeAlteredCallback<1,2>);
	containerObjectData.AddControl(&inputYRot);

	sliderYRot.SetMargin(MARGIN_TOP,560.0f);
	sliderYRot.SetMargin(MARGIN_LEFT,300.0f);
	sliderYRot.SetHeight(30.0f);
	sliderYRot.SetWidth(180.0f);
	sliderYRot.SetSliderHeadSize(30.0f);
	sliderYRot.SetSliderBaseBGColor(1,1,1);
	sliderYRot.SetSliderBaseColor(0,0,0);
	sliderYRot.SetSliderHoveredColor(0.1,0.1,0.1);
	sliderYRot.SetSliderClickedColor(0,1,0);
	sliderYRot.SetSliderDirection(SLIDER_DIRECTION_HORIZONTAL);
	sliderYRot.SetSliderValueChangedCallbackContext(this);
	sliderYRot.SetSliderValueChangedCallback(CompTypeAlteredCallback<1,2,true>);
	containerObjectData.AddControl(&sliderYRot);


	labelZRot.SetMargin(MARGIN_TOP,610.0f);
	labelZRot.SetMargin(MARGIN_LEFT,20.0f);
	labelZRot.SetHeight(30.0f);
	labelZRot.SetWidth(50.0f);
	labelZRot.SetText("z:");
	labelZRot.SetTextColor(1,1,1);
	containerObjectData.AddControl(&labelZRot);

	inputZRot.SetMargin(MARGIN_TOP,605.0f);
	inputZRot.SetMargin(MARGIN_LEFT,80.0f);
	inputZRot.SetHeight(40.0f);
	inputZRot.SetWidth(200.0f);
	inputZRot.SetTextColor(0,0,0);
	inputZRot.SetEditHeadColor(0,0,1);
	inputZRot.SetFloat(0.0f);
	inputZRot.SetBGColor(1,1,1);
	inputZRot.SetFloatInterval(0.0f,360.0f);
	inputZRot.SetCallbackContext(this);
	inputZRot.SetFloatCallback(CompTypeAlteredCallback<2,2>);
	containerObjectData.AddControl(&inputZRot);

	sliderZRot.SetMargin(MARGIN_TOP,610.0f);
	sliderZRot.SetMargin(MARGIN_LEFT,300.0f);
	sliderZRot.SetHeight(30.0f);
	sliderZRot.SetWidth(180.0f);
	sliderZRot.SetSliderHeadSize(30.0f);
	sliderZRot.SetSliderBaseBGColor(1,1,1);
	sliderZRot.SetSliderBaseColor(0,0,0);
	sliderZRot.SetSliderHoveredColor(0.1,0.1,0.1);
	sliderZRot.SetSliderClickedColor(0,0,1);
	sliderZRot.SetSliderDirection(SLIDER_DIRECTION_HORIZONTAL);
	sliderZRot.SetSliderValueChangedCallbackContext(this);
	sliderZRot.SetSliderValueChangedCallback(CompTypeAlteredCallback<2,2,true>);
	containerObjectData.AddControl(&sliderZRot);


	buttonDeleteObject.SetMargin(MARGIN_BOTTOM,20.0f);
	buttonDeleteObject.SetWidth(150.0f);
	buttonDeleteObject.SetHeight(50.0f);
	buttonDeleteObject.SetText("DELETE");
	buttonDeleteObject.SetTextColor(0,0,0);
	buttonDeleteObject.SetBGColor(0.9,0.9,0.9);
	buttonDeleteObject.SetHoverColor(0.9,0,0);
	buttonDeleteObject.SetClickColor(0.9,0,0);
	buttonDeleteObject.SetCallBackContext(this);
	buttonDeleteObject.SetCallback(DeleteObjectButtonCallback);
	containerObjectData.AddControl(&buttonDeleteObject);

	containerRight.AddControl(&containerObjectData);

	containerObjectData.SetInactive();

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
	chosenMeshGroup.SetCallback(ChosenMeshButtonCallback);
	
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
	
	buttonTextureLoad.SetBGColor(0.6,0.6,0.6);
	buttonTextureLoad.SetHoverColor(0.7,0.7,0.7);
	buttonTextureLoad.SetClickColor(1,1,1);
	buttonTextureLoad.SetText("LOAD");
	buttonTextureLoad.SetTextColor(0,0,0);
	buttonTextureLoad.SetCallBackContext(this);
	buttonTextureLoad.SetCallback(LoadTextureButtonCallback);
	
	texturePanel.AddControl(&buttonTextureLoad);
	chosenTextureGroup.SetCallback(ChosenTextureButtonCallback);
	
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
	
	buttonMaterialAdd.SetBGColor(0.6,0.6,0.6);
	buttonMaterialAdd.SetHoverColor(0.7,0.7,0.7);
	buttonMaterialAdd.SetClickColor(1,1,1);
	buttonMaterialAdd.SetText("ADD");
	buttonMaterialAdd.SetTextColor(0,0,0);
	buttonMaterialAdd.SetCallBackContext(this);
	buttonMaterialAdd.SetCallback(AddMaterialButtonCallback);
	materialPanel.AddControl(&buttonMaterialAdd);

	chosenMaterialGroup.SetCallback(ChosenMaterialButtonCallback);
	
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
	
	buttonModelAdd.SetBGColor(0.6,0.6,0.6);
	buttonModelAdd.SetHoverColor(0.7,0.7,0.7);
	buttonModelAdd.SetClickColor(1,1,1);
	buttonModelAdd.SetText("ADD");
	buttonModelAdd.SetTextColor(0,0,0);
	buttonModelAdd.SetCallBackContext(this);
	buttonModelAdd.SetCallback(AddModelButtonCallback);
	modelPanel.AddControl(&buttonModelAdd);

	chosenModelGroup.SetCallback(ChosenModelButtonCallback);

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
	
	buttonAddObject.SetBGColor(0.6,0.6,0.6);
	buttonAddObject.SetHoverColor(0.7,0.7,0.7);
	buttonAddObject.SetClickColor(1,1,1);
	buttonAddObject.SetText("ADD");
	buttonAddObject.SetTextColor(0,0,0);
	buttonAddObject.SetCallBackContext(this);
	buttonAddObject.SetCallback(AddObjectButtonCallback);
	objectPanel.AddControl(&buttonAddObject);

	chosenObjectGroup.SetCallback(ChosenObjectButtonCallback);

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
	sceneCanvas.SetBGColor(1,1,1);
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

	containerLoadCancelButtons.SetMargin(MARGIN_BOTTOM,50.0f);
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

	buttonSaveFile.SetMargin(MARGIN_BOTTOM,0.0f);
	buttonSaveFile.SetMargin(MARGIN_TOP,0.0f);
	buttonSaveFile.SetMargin(MARGIN_LEFT,0.0f);
	buttonSaveFile.SetWidth(150.0f);
	buttonSaveFile.SetBGColor(0.2,0.2,0.2);
	buttonSaveFile.SetHoverColor(0.0,0.8,0.0);
	buttonSaveFile.SetClickColor(0.2,0.2,0.22);
	buttonSaveFile.SetText("SAVE");
	buttonSaveFile.SetTextColor(0,0,0);
	buttonSaveFile.SetCallBackContext(this),
	buttonSaveFile.SetCallback(FileMenuSaveButtonCallback);



	containerLoadCancelButtons.AddControl(&buttonSaveFile);

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

	inputSaveFileName.SetMargin(MARGIN_BOTTOM,150.0f);
	inputSaveFileName.SetHeight(50.0f);
	inputSaveFileName.SetWidth(400.0f);
	inputSaveFileName.SetBGColor(1,1,1);
	inputSaveFileName.SetText("-");
	inputSaveFileName.SetTextColor(0,0,0);
	inputSaveFileName.SetEditHeadColor(0,1,0);
	inputSaveFileName.SetCallbackContext(this);
	inputSaveFileName.SetTextChangedCallback(FileMenuSaveTextInputCallback);

	containerFileSelection.AddControl(&inputSaveFileName);

	m_GUI.AddControl(&containerFileSelection);

	containerFileSelection.SetInactive();


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

	fileSelectionButtonsGroup._currentToggled = nullptr;

	for(int i=0;i<_loadedDirectorySpecificFilenames.size();++i)
	{
		RadioButton* newFilenameButton = new RadioButton();
		newFilenameButton->SetCallBackContext(this);
		newFilenameButton->SetBGColor(0.2,0.2,0.2);
		newFilenameButton->SetHoverColor(0.3,0.3,0.3);
		newFilenameButton->SetClickColor(0.4,0.4,0.4);
		newFilenameButton->SetText(_loadedDirectorySpecificFilenames[i]);
		newFilenameButton->SetTextColor(1,1,1);
		newFilenameButton->SetIndex(i);
		fileSelectionButtonsGroup.AddToGroup(newFilenameButton);

		panelFileSelection.AddControl(newFilenameButton);
	}

	_selectedFileNameIndex = -1;
}

void App::SwapToFileSelectionMenu()
{
	containerFileSelection.SetActive();
	containerApplication.SetInactive();

	if(_viewState == AppViewState::VIEWSTATE_PERSISTENCESAVE)
	{
		buttonSaveFile.SetActive();
		inputSaveFileName.SetActive();
		inputSaveFileName.SetText("default");
		buttonLoadFile.SetInactive();
	}
	else
	{
		buttonSaveFile.SetInactive();
		inputSaveFileName.SetInactive();
		buttonLoadFile.SetActive();
	}

	buttonCancel.Click(0,1);
	buttonCancel.MouseMove();
	buttonLoadFile.Click(0,1);
	buttonLoadFile.MouseMove();
	buttonSaveFile.Click(0,1);
	buttonSaveFile.MouseMove();
}

void App::SwapBackToMainMenu()
{
	containerFileSelection.SetInactive();
	containerApplication.SetActive();

	if(_viewState == AppViewState::VIEWSTATE_MESHLOAD)
	{
		buttonLoadMesh.Click(0,1);
		buttonLoadMesh.MouseMove();
	}
	else if(_viewState == AppViewState::VIEWSTATE_TEXTURELOAD)
	{
		buttonTextureLoad.Click(0,1);
		buttonTextureLoad.MouseMove();
	}
	else if(_viewState == AppViewState::VIEWSTATE_PERSISTENCESAVE)
	{
		buttonSaveScene.Click(0,1);
		buttonSaveScene.MouseMove();
	}
	else if(_viewState == AppViewState::VIEWSTATE_PERSISTENCELOAD)
	{
		buttonLoadScene.Click(0,1);
		buttonLoadScene.MouseMove();
	}
	//... TODO
}

bool App::TryLoadMesh(const std::string& meshFileName)
{
	std::string meshFilePathRelative = "assets/models/" + meshFileName;

	MeshInfo sceneLoadedMeshInfo;
	bool wasMeshLoadingSuccessful = _scene.TryLoadMesh(meshFilePathRelative,&sceneLoadedMeshInfo);
	if(wasMeshLoadingSuccessful)
	{
		//We add some dynamically allocated controls to handle meshes, like an extra option to the mesh drowpdwon
		// And we also add another radiobutton

		//First, we save the loaded mesh's info into a vector, but we might wanna do this differently later
		storedMeshInfos.push_back(sceneLoadedMeshInfo);

		int meshIndex = sceneLoadedMeshInfo.meshIndex;
		RadioButton* newChosenMeshButton = new RadioButton();
		newChosenMeshButton->SetText(std::to_string(meshIndex));
		newChosenMeshButton->SetCallBackContext(this);
		newChosenMeshButton->SetIndex(meshIndex);

		chosenMeshGroup.AddToGroup(newChosenMeshButton);
		meshPanel.AddControl(newChosenMeshButton);
		dropdownMesh.AddOption(std::to_string(meshIndex),meshIndex);

		_meshRelativeFilePaths.push_back(meshFileName);
	}

	return wasMeshLoadingSuccessful;
}


void App::ChosenMeshButtonCallback(void *context, int meshIndex)
{
	//Kinda bad practice, will fix later, right now, the readiobutton mesh has the same index as the loaded mesh,
	// But this wont always be the case, since if we delete a mesh, almost always this kind of indexing will fuck up.
	// [TODO] FIXXXXX
	App* app = (App*)context;

	app->containerMeshData.SetActive();

	MeshInfo chosenMeshButtonsMeshInfo = app->storedMeshInfos[meshIndex];

	app->labelVertexCountNumber.SetText(std::to_string(chosenMeshButtonsMeshInfo.vertexCount));
	app->labelTriangleCountNumber.SetText(std::to_string(chosenMeshButtonsMeshInfo.triangleCount));
	app->labelBVHNodeCountNumber.SetText(std::to_string(chosenMeshButtonsMeshInfo.bvhNodeCount));
	app->labelBVHDepthNumber.SetText(std::to_string(chosenMeshButtonsMeshInfo.bvhDepth));
}


bool App::TryLoadTexture(const std::string& textureFileName)
{
	std::string textureFilePathRelative = "assets/textures/" + textureFileName;

	TextureInfo sceneLoadedTextureInfo;
	bool wasTextureLoadingSuccessful = _scene.TryLoadTexture(textureFilePathRelative,&sceneLoadedTextureInfo);
	if(wasTextureLoadingSuccessful)
	{
		//We add some dynamically allocated controls to handle meshes, like an extra option to the mesh drowpdwon
		// And we also add another radiobutton
		//First, we save the loaded mesh's info into a vector, but we might wanna do this differently later
		storedTextureInfos.push_back(sceneLoadedTextureInfo);

		int textureIndex = sceneLoadedTextureInfo.textureIndex;
		RadioButton* newChosenTextureButton = new RadioButton();
		newChosenTextureButton->SetText(std::to_string(textureIndex));
		newChosenTextureButton->SetCallBackContext(this);
		newChosenTextureButton->SetIndex(textureIndex);

		chosenTextureGroup.AddToGroup(newChosenTextureButton);
		texturePanel.AddControl(newChosenTextureButton);
		dropdownTexture.AddOption(std::to_string(textureIndex),textureIndex);

		_textureRelativeFilePaths.push_back(textureFileName);
	}

	return wasTextureLoadingSuccessful;
}

void App::AddMaterial(const MaterialData& newMaterialData)
{
	MaterialInfo newMaterialInfo;
	bool wasAddingMaterialSuccessful = _scene.TryAddMaterial(&newMaterialInfo);
	if(wasAddingMaterialSuccessful)
	{
		storedMaterialInfos.push_back(newMaterialInfo);

		_scene.TryAlterMaterial(newMaterialInfo.materialIndex,newMaterialData);
		
		RadioButton* newMaterialButton = new RadioButton();
		newMaterialButton->SetText(std::to_string(newMaterialInfo.materialIndex));
		newMaterialButton->SetCallBackContext(this);
		newMaterialButton->SetIndex(newMaterialInfo.materialIndex);

		chosenMaterialGroup.AddToGroup(newMaterialButton);
		materialPanel.AddControl(newMaterialButton);
		dropdownMaterial.AddOption(std::to_string(newMaterialInfo.materialIndex),newMaterialInfo.materialIndex);
	}
}

bool App::TryAddModel(const ModelDataCpu &newModelData)
{
    ModelInfo newModelInfo;
	bool wasAddingModelSuccessful = _scene.TryAddModel(&newModelInfo);
	if(wasAddingModelSuccessful)
	{
		storedModelInfos.push_back(newModelInfo);

		_scene.TryAlterModel(newModelInfo.modelIndex,newModelData);
		
		RadioButton* newModelButton = new RadioButton();
		newModelButton->SetText(std::to_string(newModelInfo.modelIndex));
		newModelButton->SetCallBackContext(this);
		newModelButton->SetIndex(newModelInfo.modelIndex);

		chosenModelGroup.AddToGroup(newModelButton);
		modelPanel.AddControl(newModelButton);
		dropdownModel.AddOption(std::to_string(newModelInfo.modelIndex),newModelInfo.modelIndex);
	}

	return wasAddingModelSuccessful;
}

bool App::TryAddObject(const ObjectState &newObjectState)
{
    ObjectInfo newObjectInfo;
	bool wasAddingObjectSuccessful = _scene.TryAddObject(&newObjectInfo);
	if(wasAddingObjectSuccessful)
	{	
		storedObjectInfos.push_back(newObjectInfo);
		_scene.TryAlterObject(newObjectInfo.objectIndex,newObjectState);

		RadioButton* newObjectButton = new RadioButton();
		newObjectButton->SetText(std::to_string(newObjectInfo.objectIndex));
		newObjectButton->SetCallBackContext(this);
		newObjectButton->SetIndex(newObjectInfo.objectIndex);

		chosenObjectGroup.AddToGroup(newObjectButton);
		objectPanel.AddControl(newObjectButton);		
	}

	return wasAddingObjectSuccessful;
}

void App::SaveScene(const std::string &sceneSavingFileNameRelative)
{
	//std::cout<<"Saving Scene into: " << sceneSavingFileNameRelative << "\n";

	int meshCount = _meshRelativeFilePaths.size();
	int textureCount = _textureRelativeFilePaths.size();
	int materialCount = storedMaterialInfos.size();
	int modelCount = storedModelInfos.size();
	int objectCount = storedObjectInfos.size();

	if(meshCount + textureCount + materialCount + modelCount + objectCount <= 0) return; // we have nothing to save

	std::ofstream saveFile(sceneSavingFileNameRelative);

	if(!saveFile) return; // couldn't open savefile

	// Lets start saving otherwise
	for(int i = 0; i<meshCount; ++i)
	{
		saveFile << "mesh " << _meshRelativeFilePaths[i] << "\n";
	}
	for(int i=0;i<textureCount;++i)
	{
		saveFile << "tex " << _textureRelativeFilePaths[i] << "\n";
	}
	for(int i=0;i<materialCount;++i)
	{
		int materialIndex = storedMaterialInfos[i].materialIndex;
		MaterialData material;
		bool doesMaterialExist = _scene.GetMaterialData(materialIndex,&material);
		if(doesMaterialExist)
		{
			saveFile << "mat ";
			saveFile << material.albedoColor.x << " " << material.albedoColor.y << " " << material.albedoColor.z << " ";
			saveFile << material.albedoTextureIndex << " ";
			saveFile << material.emissionStrength << " ";
			saveFile << material.emissionColor.x << " " << material.emissionColor.y << " " << material.emissionColor.z << " ";
			saveFile << material.transmission << " "<<material.metallic << " " << material.roughness << " " << material.ior << "\n";
		}
	}
	for(int i=0;i<modelCount;++i)
	{
		int modelIndex = storedModelInfos[i].modelIndex;
		ModelDataCpu model;
		bool doesModelExist = _scene.GetModelData(modelIndex,&model);
		if(doesModelExist)
		{
			saveFile << "mod ";
			saveFile << model.meshIndex << " ";
			saveFile << model.materialIndex << "\n";
		}
	}
	for(int i=0;i<objectCount;++i)
	{
		int objectIndex = storedObjectInfos[i].objectIndex;
		ObjectState object;
		bool doesObjectExist = _scene.GetObjectState(objectIndex,&object);
		if(doesObjectExist)
		{
			saveFile << "obj ";
			saveFile << object.modelIndex << " ";
			saveFile << object.transform.position.x << " " << object.transform.position.y << " " << object.transform.position.z << " ";
			saveFile << object.transform.scale.x << " " << object.transform.scale.y << " " << object.transform.scale.z << " ";
			saveFile << object.transform.rotation.x << " " << object.transform.rotation.y << " " << object.transform.rotation.z << "\n";
		}
	}

	saveFile.close();

	//std::cout<<"Finished Saving the scene.\n";
}

bool App::TryLoadScene(const std::string &sceneLoadingFileName)
{
	std::string fullToBeLoadedSceneFilePathRelative = "assets/saves/" + sceneLoadingFileName;

	Reset();

	std::ifstream loadFile(fullToBeLoadedSceneFilePathRelative);
	std::string fileLineBuffer;
	while(std::getline(loadFile,fileLineBuffer))
	{
		std::stringstream lineStringStream(fileLineBuffer);
		std::vector<std::string> lineWords;
		std::string wordBuffer;
		while(lineStringStream >> wordBuffer)
		{
			lineWords.push_back(wordBuffer);
		}

		//now lineWords has all the words of the line
		if(lineWords.size() == 0) continue; // empty line for some reason, we definitely skip.

		if(lineWords[0] == "mesh")
		{
			if(lineWords.size() < 2) return false;
			// We have found a mesh entry, lets try and load it.
			bool wasLoadingMeshSuccesful = TryLoadMesh(lineWords[1]);
			if(!wasLoadingMeshSuccesful) return false;
		}
		else if(lineWords[0] == "tex")
		{
			if(lineWords.size() < 2) return false;

			bool wasLoadingTextureSuccessful = TryLoadTexture(lineWords[1]);
			if(!wasLoadingTextureSuccessful) return false;
		}
		else if(lineWords[0] == "mat")
		{
			if(lineWords.size() < 13) return false;

			MaterialData newMaterialData;
			newMaterialData.albedoColor.x = std::stof(lineWords[1]);
			newMaterialData.albedoColor.y = std::stof(lineWords[2]);
			newMaterialData.albedoColor.z = std::stof(lineWords[3]);
			newMaterialData.albedoTextureIndex = std::stod(lineWords[4]);
			newMaterialData.emissionStrength = std::stof(lineWords[5]);
			newMaterialData.emissionColor.x = std::stof(lineWords[6]);
			newMaterialData.emissionColor.y = std::stof(lineWords[7]);
			newMaterialData.emissionColor.z = std::stof(lineWords[8]);
			newMaterialData.transmission = std::stof(lineWords[9]);
			newMaterialData.metallic= std::stof(lineWords[10]);
			newMaterialData.roughness = std::stof(lineWords[11]);
			newMaterialData.ior = std::stof(lineWords[12]);

			AddMaterial(newMaterialData);
		}
		else if(lineWords[0] == "mod")
		{
			if(lineWords.size() < 3) return false;

			ModelDataCpu newModelData;
			newModelData.meshIndex = std::stod(lineWords[1]);
			newModelData.materialIndex = std::stod(lineWords[2]);

			bool wasAddingModelSuccseful = TryAddModel(newModelData);
			if(!wasAddingModelSuccseful) return false;
		}
		else if(lineWords[0] == "obj")
		{
			if(lineWords.size() < 11) return false;

			ObjectState newObjectState;
			newObjectState.modelIndex = std::stod(lineWords[1]);
			newObjectState.transform.position.x = std::stof(lineWords[2]);
			newObjectState.transform.position.y = std::stof(lineWords[3]);
			newObjectState.transform.position.z = std::stof(lineWords[4]);
			newObjectState.transform.scale.x = std::stof(lineWords[5]);
			newObjectState.transform.scale.y = std::stof(lineWords[6]);
			newObjectState.transform.scale.z = std::stof(lineWords[7]);
			newObjectState.transform.rotation.x = std::stof(lineWords[8]);
			newObjectState.transform.rotation.y = std::stof(lineWords[9]);
			newObjectState.transform.rotation.z = std::stof(lineWords[10]);

			bool wasAddingObjectSuccesful = TryAddObject(newObjectState);
			if(!wasAddingObjectSuccesful) return false;
		}
	}

	loadFile.close();

    return true;
}

void App::Reset()
{
	_scene.Reset();

	storedMeshInfos.clear();
	storedTextureInfos.clear();
	storedMaterialInfos.clear();
	storedModelInfos.clear();
	storedObjectInfos.clear();

	chosenMeshGroup.SetToggledOff();
	chosenTextureGroup.SetToggledOff();
	chosenMaterialGroup.SetToggledOff();
	chosenModelGroup.SetToggledOff();
	chosenObjectGroup.SetToggledOff();

	dropdownMesh.ClearOptions();
	dropdownTexture.ClearOptions();
	dropdownMaterial.ClearOptions();
	dropdownModel.ClearOptions();

	for(int i=1;i<meshPanel.GetChildren().size();++i)
	{
		RadioButton* givenButton = (RadioButton*)meshPanel.GetChildren()[i];
		delete givenButton;
	}
	meshPanel.GetChildren().clear();
	meshPanel.AddControl(&buttonLoadMesh);

	for(int i=1;i<texturePanel.GetChildren().size();++i)
	{
		RadioButton* givenButton = (RadioButton*)texturePanel.GetChildren()[i];
		delete givenButton;
	}
	texturePanel.GetChildren().clear();
	texturePanel.AddControl(&buttonTextureLoad);

	for(int i=1;i<materialPanel.GetChildren().size();++i)
	{
		RadioButton* givenButton = (RadioButton*)materialPanel.GetChildren()[i];
		delete givenButton;
	}
	materialPanel.GetChildren().clear();
	materialPanel.AddControl(&buttonMaterialAdd);

	for(int i=1;i<modelPanel.GetChildren().size();++i)
	{
		RadioButton* givenButton = (RadioButton*)modelPanel.GetChildren()[i];
		delete givenButton;
	}
	modelPanel.GetChildren().clear();
	modelPanel.AddControl(&buttonModelAdd);

	for(int i=1;i<objectPanel.GetChildren().size();++i)
	{
		RadioButton* givenButton = (RadioButton*)objectPanel.GetChildren()[i];
		delete givenButton;
	}
	objectPanel.GetChildren().clear();
	objectPanel.AddControl(&buttonAddObject);

	containerMeshData.SetInactive();
	containerTextureData.SetInactive();
	containerMaterialData.SetInactive();
	containerModelData.SetInactive();
	containerObjectData.SetInactive();

	mainButtonsGroup.SetToggledOff();

	//meshPanel.SetInactive();
	meshPanel.SetActive();
	texturePanel.SetInactive();
	materialPanel.SetInactive();
	modelPanel.SetInactive();
	objectPanel.SetInactive();

	_meshRelativeFilePaths.clear();
	_textureRelativeFilePaths.clear();
}

void App::ChosenTextureButtonCallback(void *context, int textureIndex)
{
	App* app = (App*)context;

	app->containerTextureData.SetActive();

	TextureInfo chosenTextureButtonTextureInfo = app->storedTextureInfos[textureIndex];

	app->labelTextureWidthNumber.SetText(std::to_string(chosenTextureButtonTextureInfo.width));
	app->labelTextureHeightNumber.SetText(std::to_string(chosenTextureButtonTextureInfo.height));
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

		//std::cout<<"Left mouse button clicked\n";
		app->m_GUI.MouseClick(0,0);
		
		//std::cout << "Picking at: (x :"<< x<< " y: " << y<< ")\n";
		//app->virtualWorld.Pick((int)x, (int)y);
	}
	else if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		//std::cout<<"Left mouse button released!\n";
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

	app->chosenMeshGroup.SetToggledOff();
	app->chosenTextureGroup.SetToggledOff();
	app->chosenMaterialGroup.SetToggledOff();
	app->chosenModelGroup.SetToggledOff();
	app->chosenObjectGroup.SetToggledOff();

	if(index == 0)
	{
		//app->containerMeshData.SetActive();
		app->meshPanel.SetActive();
	}
	else if(index==1)
	{
		//app->containerTextureData.SetActive();
		app->texturePanel.SetActive();
	}
	else if(index==2)
	{
		//app->containerMaterialData.SetActive();
		app->materialPanel.SetActive();
	}
	else if(index==3)
	{
		//app->containerModelData.SetActive();
		app->modelPanel.SetActive();
	}
	else
	{
		//app->containerObjectData.SetActive();
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
	
	app->_viewState = AppViewState::VIEWSTATE_MESHLOAD;

	app->SwapToFileSelectionMenu();

	

	app->RepopulateFileSelectionPanel();
}

void App::LoadTextureButtonCallback(void *context)
{
	App* app = (App*)context;

	std::string textureFolderRelative = "assets/textures/"; // For now its this
	std::vector<std::string> acceptableExtensions = {".png",".jpg",".bmp"};

	GetFileNamesWithSpecificExtension(textureFolderRelative,acceptableExtensions,app->_loadedDirectorySpecificFilenames);

	if(app->_loadedDirectorySpecificFilenames.size()==0) return;
	
	app->_viewState = AppViewState::VIEWSTATE_TEXTURELOAD;

	app->SwapToFileSelectionMenu();

	

	app->RepopulateFileSelectionPanel();
}



void App::FileSelectionMenuCancelButtonCallback(void *context)
{
	App* app = (App*)context;
	
	app->SwapBackToMainMenu();

	app->_viewState = AppViewState::VIEWSTATE_MAINMENU;
}

void App::FileMenuSaveTextInputCallback(void *context, const std::string &newString)
{
	App* app = (App*)context;
	app->_chosenEditedFileName = newString;
	app->fileSelectionButtonsGroup.SetToggledOff();
}

void App::FileMenuSaveButtonCallback(void *context)
{
	App* app = (App*)context;
	if(app->_chosenEditedFileName.length() == 0) return;

	if(app->_chosenEditedFileName.length() >= 4)
	{
		//Lets check if the end of the filename end with .scn if not, we add it to it
		const std::string& s = app->_chosenEditedFileName;
		if(s[s.length()-1] != 'n' || s[s.length()-2] != 'c' || s[s.length()-3] != 's' || s[s.length()-4] != '.')
		{
			app->_chosenEditedFileName += ".scn";
		}
	}

	std::string fullSaveRelativeFilePath = "assets/saves/" + app->_chosenEditedFileName;
	app->SaveScene(fullSaveRelativeFilePath);

	app->SwapBackToMainMenu();

	app->_viewState = AppViewState::VIEWSTATE_MAINMENU;	
}

void App::FileSelectionMenuItemCallback(void *context, int index)
{
	App* app = (App*)context;
	app->_selectedFileNameIndex = index;
	if(app->_viewState == AppViewState::VIEWSTATE_PERSISTENCESAVE)
	{
		app->_chosenEditedFileName = app->_loadedDirectorySpecificFilenames[index];
		app->inputSaveFileName.SetText(app->_loadedDirectorySpecificFilenames[index]);
	}
}

void App::FileSelectionMenuLoadButtonCallback(void *context)
{
	App* app = (App*)context;
	if(app->_selectedFileNameIndex < 0 || app->_selectedFileNameIndex >= app->_loadedDirectorySpecificFilenames.size()) return;

	std::string chosenFileName = app->_loadedDirectorySpecificFilenames[app->_selectedFileNameIndex];

	if(app->_viewState == AppViewState::VIEWSTATE_MESHLOAD)
	{
		app->TryLoadMesh( chosenFileName);
	}
	else if(app->_viewState == AppViewState::VIEWSTATE_TEXTURELOAD)
	{
		app->TryLoadTexture( chosenFileName);
	}
	else if(app->_viewState == AppViewState::VIEWSTATE_PERSISTENCELOAD)
	{
		bool wasSceneloadingSuccesful = app->TryLoadScene(chosenFileName);
		if(!wasSceneloadingSuccesful)
		{
			app->Reset();
		}
	}

	app->SwapBackToMainMenu();
	app->_viewState = AppViewState::VIEWSTATE_MAINMENU;
}

void App::AddMaterialButtonCallback(void *context)
{
	App* app = (App*)context;

	MaterialData newBasicMaterialData;
	app->AddMaterial(newBasicMaterialData);
}

void App::ChosenMaterialButtonCallback(void *context, int materialIndex)
{
	App* app = (App*)context;

	app->containerMaterialData.SetActive();

	app->chosenMaterialIndex = materialIndex;

	MaterialInfo materialInfo = app->storedMaterialInfos[materialIndex];
	MaterialData storedMaterialData;
	app->_scene.GetMaterialData(materialInfo.materialIndex,&storedMaterialData);

	//std::cout<<"Sotred Albedo texture Index: " << storedMaterialData.albedoTextureIndex <<"\n";
	app->dropdownTexture.SetChosenOption(storedMaterialData.albedoTextureIndex == -1 ? 0 : storedMaterialData.albedoTextureIndex + 1);

	app->inputRed.SetFloat(storedMaterialData.albedoColor.x);
	app->sliderRed.SetSliderValue(storedMaterialData.albedoColor.x);
	app->inputGreen.SetFloat(storedMaterialData.albedoColor.y);
	app->sliderGreen.SetSliderValue(storedMaterialData.albedoColor.y);
	app->inputBlue.SetFloat(storedMaterialData.albedoColor.z);
	app->sliderBlue.SetSliderValue(storedMaterialData.albedoColor.z);

	app->inputRoughness.SetFloat(storedMaterialData.roughness);
	app->sliderRoughness.SetSliderValue(storedMaterialData.roughness);
	app->inputTransmission.SetFloat(storedMaterialData.transmission);
	app->sliderTransmission.SetSliderValue(storedMaterialData.transmission);
	app->inputMetallic.SetFloat(storedMaterialData.metallic);
	app->sliderMetallic.SetSliderValue(storedMaterialData.metallic);


	app->inputEmissionRed.SetFloat(storedMaterialData.emissionColor.x);
	app->sliderEmissionRed.SetSliderValue(storedMaterialData.emissionColor.x);
	app->inputEmissionGreen.SetFloat(storedMaterialData.emissionColor.y);
	app->sliderEmissionGreen.SetSliderValue(storedMaterialData.emissionColor.y);
	app->inputEmissionBlue.SetFloat(storedMaterialData.emissionColor.z);
	app->sliderEmissionBlue.SetSliderValue(storedMaterialData.emissionColor.z);

	app->inputIoR.SetFloat(storedMaterialData.ior);
	app->sliderIoR.SetSliderValue((storedMaterialData.ior-1.0f) / 1.5f);

	app->inputEmissionStrength.SetFloat(storedMaterialData.emissionStrength);
	app->sliderEmissionStrength.SetSliderValue(storedMaterialData.emissionStrength / 300.0f);

}

void App::AlbedoTextureChosenCallback(void *context, int optionTextureIndex)
{
	App* app = (App*)context;
	if(app->chosenMaterialIndex != -1)
	{
		int materialIndex = app->chosenMaterialIndex;
		MaterialData oldData;
		app->_scene.GetMaterialData(materialIndex,&oldData);
		oldData.albedoTextureIndex = optionTextureIndex;
		app->_scene.TryAlterMaterial(materialIndex,oldData);
	}
}

void App::AlbedoRedAlteredCallback(void *context, float redValue)
{
	App* app = (App*)context;
	if(app->chosenMaterialIndex != -1)
	{
		int materialIndex = app->chosenMaterialIndex;
		MaterialData oldData;
		app->_scene.GetMaterialData(materialIndex,&oldData);
		oldData.albedoColor.x = redValue;
		app->_scene.TryAlterMaterial(materialIndex,oldData);

		app->inputRed.SetFloat(redValue);
		app->sliderRed.SetSliderValue(redValue);
	}
}

void App::AlbedoGreenAlteredCallback(void *context, float greenVlaue)
{
	App* app = (App*)context;
	if(app->chosenMaterialIndex != -1)
	{
		int materialIndex = app->chosenMaterialIndex;
		MaterialData oldData;
		app->_scene.GetMaterialData(materialIndex,&oldData);
		oldData.albedoColor.y = greenVlaue;
		app->_scene.TryAlterMaterial(materialIndex,oldData);

		app->inputGreen.SetFloat(greenVlaue);
		app->sliderGreen.SetSliderValue(greenVlaue);
	}
}

void App::AlbedoBlueAlteredCallback(void *context, float blueValue)
{
	App* app = (App*)context;
	if(app->chosenMaterialIndex != -1)
	{
		int materialIndex = app->chosenMaterialIndex;
		MaterialData oldData;
		app->_scene.GetMaterialData(materialIndex,&oldData);
		oldData.albedoColor.z = blueValue;
		app->_scene.TryAlterMaterial(materialIndex,oldData);

		app->inputBlue.SetFloat(blueValue);
		app->sliderBlue.SetSliderValue(blueValue);
	}
}

void App::MetallicAlteredCallback(void *context, float metallicValue)
{
	App* app = (App*)context;
	if(app->chosenMaterialIndex != -1)
	{
		int materialIndex = app->chosenMaterialIndex;
		MaterialData oldData;
		app->_scene.GetMaterialData(materialIndex,&oldData);
		oldData.metallic = metallicValue;
		app->_scene.TryAlterMaterial(materialIndex,oldData);

		app->inputMetallic.SetFloat(metallicValue);
		app->sliderMetallic.SetSliderValue(metallicValue);
	}
}

void App::RoughnessAlteredCallback(void *context, float roughnessValue)
{
	App* app = (App*)context;
	if(app->chosenMaterialIndex != -1)
	{
		int materialIndex = app->chosenMaterialIndex;
		MaterialData oldData;
		app->_scene.GetMaterialData(materialIndex,&oldData);
		oldData.roughness = roughnessValue;
		app->_scene.TryAlterMaterial(materialIndex,oldData);

		app->inputRoughness.SetFloat(roughnessValue);
		app->sliderRoughness.SetSliderValue(roughnessValue);
	}
}

void App::TransmissionAlteredCallback(void *context, float transmissionValue)
{
	App* app = (App*)context;
	if(app->chosenMaterialIndex != -1)
	{
		int materialIndex = app->chosenMaterialIndex;
		MaterialData oldData;
		app->_scene.GetMaterialData(materialIndex,&oldData);
		oldData.transmission = transmissionValue;
		app->_scene.TryAlterMaterial(materialIndex,oldData);

		app->inputTransmission.SetFloat(transmissionValue);
		app->sliderTransmission.SetSliderValue(transmissionValue);
	}
}

void App::IoRInputCallback(void *context, float IoRValue)
{
	App* app = (App*)context;
	if(app->chosenMaterialIndex != -1)
	{
		int materialIndex = app->chosenMaterialIndex;
		MaterialData oldData;
		app->_scene.GetMaterialData(materialIndex,&oldData);
		oldData.ior = IoRValue;
		app->_scene.TryAlterMaterial(materialIndex,oldData);

		app->sliderIoR.SetSliderValue((IoRValue - 1.0f) / 1.5f);
	}
}

void App::IoRSliderCallback(void *context, float IoRValueNormalized)
{
	App* app = (App*)context;
	if(app->chosenMaterialIndex != -1)
	{
		int materialIndex = app->chosenMaterialIndex;
		MaterialData oldData;
		app->_scene.GetMaterialData(materialIndex,&oldData);
		float newIoRValue = IoRValueNormalized * 1.5f + 1.0f;
		oldData.ior = newIoRValue;
		app->_scene.TryAlterMaterial(materialIndex,oldData);

		app->inputIoR.SetFloat(newIoRValue);
	}
}

void App::EmissionStrengthInputCallback(void *context, float emissionStrengthValue)
{
	App* app = (App*)context;
	if(app->chosenMaterialIndex != -1)
	{
		int materialIndex = app->chosenMaterialIndex;
		MaterialData oldData;
		app->_scene.GetMaterialData(materialIndex,&oldData);
		oldData.emissionStrength = emissionStrengthValue;
		app->_scene.TryAlterMaterial(materialIndex,oldData);

		app->sliderEmissionStrength.SetSliderValue(emissionStrengthValue / 300.0f);
	}
}

void App::EmissionStrengthSliderCallback(void *context, float emissionStrengthValueNormalized)
{
	App* app = (App*)context;
	if(app->chosenMaterialIndex != -1)
	{
		int materialIndex = app->chosenMaterialIndex;
		MaterialData oldData;
		app->_scene.GetMaterialData(materialIndex,&oldData);
		float newEmissionStrengthValue = emissionStrengthValueNormalized * 300.0f;
		oldData.emissionStrength = newEmissionStrengthValue;
		app->_scene.TryAlterMaterial(materialIndex,oldData);

		app->inputEmissionStrength.SetFloat(newEmissionStrengthValue);
	}
}

void App::EmissionRedAlteredCallback(void *context, float redEmissionValue)
{
	App* app = (App*)context;
	if(app->chosenMaterialIndex != -1)
	{
		int materialIndex = app->chosenMaterialIndex;
		MaterialData oldData;
		app->_scene.GetMaterialData(materialIndex,&oldData);
		oldData.emissionColor.x = redEmissionValue;
		app->_scene.TryAlterMaterial(materialIndex,oldData);

		app->inputEmissionRed.SetFloat(redEmissionValue);
		app->sliderEmissionRed.SetSliderValue(redEmissionValue);
	}
}

void App::EmissionGreenAlteredCallback(void *context, float greenEmissionValue)
{
	App* app = (App*)context;
	if(app->chosenMaterialIndex != -1)
	{
		int materialIndex = app->chosenMaterialIndex;
		MaterialData oldData;
		app->_scene.GetMaterialData(materialIndex,&oldData);
		oldData.emissionColor.y = greenEmissionValue;
		app->_scene.TryAlterMaterial(materialIndex,oldData);

		app->inputEmissionGreen.SetFloat(greenEmissionValue);
		app->sliderEmissionGreen.SetSliderValue(greenEmissionValue);
	}
}

void App::EmissionBlueAlteredCallback(void *context, float blueEmissionValue)
{
	App* app = (App*)context;
	if(app->chosenMaterialIndex != -1)
	{
		int materialIndex = app->chosenMaterialIndex;
		MaterialData oldData;
		app->_scene.GetMaterialData(materialIndex,&oldData);
		oldData.emissionColor.z = blueEmissionValue;
		app->_scene.TryAlterMaterial(materialIndex,oldData);

		app->inputEmissionBlue.SetFloat(blueEmissionValue);
		app->sliderEmissionBlue.SetSliderValue(blueEmissionValue);
	}
}

void App::AddModelButtonCallback(void *context)
{
	App* app = (App*)context;

	ModelDataCpu newModelData;
	app->TryAddModel(newModelData);
}

void App::ChosenModelButtonCallback(void *context, int modelIndex)
{
	App* app = (App*)context;

	app->containerModelData.SetActive();

	app->chosenModelIndex = modelIndex;

	ModelInfo modelInfo = app->storedModelInfos[modelIndex];
	ModelDataCpu storedModelDataCpu;
	app->_scene.GetModelData(modelInfo.modelIndex,&storedModelDataCpu);

	app->dropdownMesh.SetChosenOption(storedModelDataCpu.meshIndex);
	app->dropdownMaterial.SetChosenOption(storedModelDataCpu.materialIndex);
}

void App::MeshIndexChosenDropdownCallback(void *context, int chosenOptionIndex)
{
	App* app = (App*)context;
	if(app->chosenModelIndex != -1)
	{
		int modelIndex = app->chosenModelIndex;
		ModelDataCpu oldData;
		app->_scene.GetModelData(modelIndex,&oldData);
		oldData.meshIndex = chosenOptionIndex;
		app->_scene.TryAlterModel(modelIndex,oldData);
	}
}

void App::MaterialIndexChosenDropdownCallback(void *context, int chosenOptionIndex)
{
	App* app = (App*)context;
	if(app->chosenModelIndex != -1)
	{
		int modelIndex = app->chosenModelIndex;
		ModelDataCpu oldData;
		app->_scene.GetModelData(modelIndex,&oldData);
		oldData.materialIndex = chosenOptionIndex;
		app->_scene.TryAlterModel(modelIndex,oldData);
	}
}


void App::AddObjectButtonCallback(void *context)
{
	App* app = (App*)context;

	ObjectState newObjectState;
	app->TryAddObject(newObjectState);
}

void App::ChosenObjectButtonCallback(void *context, int objectIndex)
{
	App* app = (App*)context;

	

	app->containerObjectData.SetActive();

	app->buttonDeleteObject.MouseMove();

	app->chosenObjectIndex = objectIndex;

	ObjectState storedObjectState;
	app->_scene.GetObjectState(objectIndex,&storedObjectState);

	app->dropdownModel.SetChosenOption(storedObjectState.modelIndex);

	app->inputXPos.SetFloat(storedObjectState.transform.position.x);
	app->inputYPos.SetFloat(storedObjectState.transform.position.y);
	app->inputZPos.SetFloat(storedObjectState.transform.position.z);

	app->inputXScale.SetFloat(storedObjectState.transform.scale.x);
	app->inputYScale.SetFloat(storedObjectState.transform.scale.y);
	app->inputZScale.SetFloat(storedObjectState.transform.scale.z);


	app->inputXRot.SetFloat(storedObjectState.transform.rotation.x);
	app->sliderXRot.SetSliderValue(storedObjectState.transform.rotation.x / 360.0f);
	app->inputYRot.SetFloat(storedObjectState.transform.rotation.y);
	app->sliderYRot.SetSliderValue(storedObjectState.transform.rotation.y / 360.0f);
	app->inputZRot.SetFloat(storedObjectState.transform.rotation.z);
	app->sliderZRot.SetSliderValue(storedObjectState.transform.rotation.z / 360.0f);
}

void App::ModelIndexChosenDropdownCallback(void *context, int chosenOptionIndex)
{
	App* app = (App*)context;
	if(app->chosenObjectIndex != -1)
	{
		int objectIndex = app->chosenObjectIndex;

		ObjectState oldObjectState;
		app->_scene.GetObjectState(objectIndex,&oldObjectState);
		oldObjectState.modelIndex = chosenOptionIndex;
		app->_scene.TryAlterObject(objectIndex,oldObjectState);
	}
}

void App::DeleteObjectButtonCallback(void *context)
{
	App* app = (App*)context;
	if(app->chosenObjectIndex != -1)
	{
		int objectIndex = app->chosenObjectIndex;
		bool wasDeletionSuccesful = app->_scene.TryDeleteObject(objectIndex);
		if(wasDeletionSuccesful)
		{
			//We untoggle the currently toggled object button, and disable the objectData container
			// we only do this to keep a clean state
			RadioButton* currentlyToggled = app->chosenObjectGroup._currentToggled;
			if(currentlyToggled != nullptr)
			{
				currentlyToggled->SetToggledOff();
			}
			app->containerObjectData.SetInactive();
			app->buttonDeleteObject.Click(0,1);

			std::vector<Control *>& children = app->objectPanel.GetChildren();
			int objectButtonDeletionIndex = objectIndex + 1;
			int objectButtonsEndIndex = children.size() -1;
			RadioButton* deletionButton = (RadioButton*)children[objectButtonDeletionIndex];
			RadioButton* endButton = (RadioButton*)children[objectButtonsEndIndex];
			if(objectButtonDeletionIndex != objectButtonsEndIndex)
			{
				//we do a copying
				deletionButton->SetText(endButton->GetText());
			}

			delete endButton;
			children.pop_back();

			app->storedObjectInfos.pop_back();
		}
	}
}

void App::NewSceneButtonCallback(void *context)
{
	App* app = (App*)context;
	app->Reset();
}

void App::SaveSceneButtonCallback(void *context)
{
	App* app = (App*)context;

	std::string savesFolderRelative = "assets/saves/"; // For now its this
	std::vector<std::string> acceptableExtensions = {".scn"};

	GetFileNamesWithSpecificExtension(savesFolderRelative,acceptableExtensions,app->_loadedDirectorySpecificFilenames);

	if(app->_loadedDirectorySpecificFilenames.size() <= 0) return;
	
	app->_viewState = AppViewState::VIEWSTATE_PERSISTENCESAVE;

	app->SwapToFileSelectionMenu();

	

	

	app->RepopulateFileSelectionPanel();
}

void App::LoadSceneButtonCallback(void *context)
{
	App* app = (App*)context;

	std::string savesFolderRelative = "assets/saves/"; // For now its this
	std::vector<std::string> acceptableExtensions = {".scn"};

	GetFileNamesWithSpecificExtension(savesFolderRelative,acceptableExtensions,app->_loadedDirectorySpecificFilenames);
	
	app->_viewState = AppViewState::VIEWSTATE_PERSISTENCELOAD;

	app->SwapToFileSelectionMenu();

	app->RepopulateFileSelectionPanel();
}
