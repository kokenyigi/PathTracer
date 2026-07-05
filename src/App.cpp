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
	containerMaterialData.AddControl(&sliderEmissionBlue);
	
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
	//buttonMaterialAdd.SetCallback();
	materialPanel.AddControl(&buttonMaterialAdd);
	
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

	buttonCancel.Click(0,1);
	buttonCancel.MouseMove();
	buttonLoadFile.Click(0,1);
	buttonLoadFile.MouseMove();
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

	}
	//... TODO
}

void App::TryLoadMeshData()
{
	std::string meshFilePathRelative = "assets/models/" + _loadedDirectorySpecificFilenames[_selectedFileNameIndex];

	//std::cout<<"Began Loading meshData...\n";


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
	}


	//std::cout<<"Ended Loading meshData...\n";
}


void App::ChosenMeshButtonCallback(void *context, int meshIndex)
{
	//Kinda bad practice, will fix later, right now, the readiobutton mesh has the same index as the loaded mesh,
	// But this wont always be the case, since if we delete a mesh, almost always this kind of indexing will fuck up.
	// [TODO] FIXXXXX
	App* app = (App*)context;

	MeshInfo chosenMeshButtonsMeshInfo = app->storedMeshInfos[meshIndex];

	app->labelVertexCountNumber.SetText(std::to_string(chosenMeshButtonsMeshInfo.vertexCount));
	app->labelTriangleCountNumber.SetText(std::to_string(chosenMeshButtonsMeshInfo.triangleCount));
	app->labelBVHNodeCountNumber.SetText(std::to_string(chosenMeshButtonsMeshInfo.bvhNodeCount));
	app->labelBVHDepthNumber.SetText(std::to_string(chosenMeshButtonsMeshInfo.bvhDepth));
}


void App::TryLoadTextureData()
{
	std::string textureFilePathRelative = "assets/textures/" + _loadedDirectorySpecificFilenames[_selectedFileNameIndex];

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
	}
}


void App::ChosenTextureButtonCallback(void *context, int textureIndex)
{
	App* app = (App*)context;

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
	
	app->SwapToFileSelectionMenu();

	app->_viewState = AppViewState::VIEWSTATE_MESHLOAD;

	app->RepopulateFileSelectionPanel();
}

void App::LoadTextureButtonCallback(void *context)
{
	App* app = (App*)context;

	std::string textureFolderRelative = "assets/textures/"; // For now its this
	std::vector<std::string> acceptableExtensions = {".png",".jpg",".bmp"};

	GetFileNamesWithSpecificExtension(textureFolderRelative,acceptableExtensions,app->_loadedDirectorySpecificFilenames);

	if(app->_loadedDirectorySpecificFilenames.size()==0) return;
	
	app->SwapToFileSelectionMenu();

	app->_viewState = AppViewState::VIEWSTATE_TEXTURELOAD;

	app->RepopulateFileSelectionPanel();
}


void App::FileSelectionMenuCancelButtonCallback(void *context)
{
	App* app = (App*)context;
	
	app->SwapBackToMainMenu();

	app->_viewState = AppViewState::VIEWSTATE_MAINMENU;
}

void App::FileSelectionMenuItemCallback(void *context, int index)
{
	App* app = (App*)context;
	app->_selectedFileNameIndex = index;
}

void App::FileSelectionMenuLoadButtonCallback(void *context)
{
	App* app = (App*)context;
	if(app->_selectedFileNameIndex < 0 || app->_selectedFileNameIndex >= app->_loadedDirectorySpecificFilenames.size()) return;

	if(app->_viewState == AppViewState::VIEWSTATE_MESHLOAD)
	{
		app->TryLoadMeshData();

		
	}
	else if(app->_viewState == AppViewState::VIEWSTATE_TEXTURELOAD)
	{
		app->TryLoadTextureData();
	}

	app->SwapBackToMainMenu();
	app->_viewState = AppViewState::VIEWSTATE_MAINMENU;
}


