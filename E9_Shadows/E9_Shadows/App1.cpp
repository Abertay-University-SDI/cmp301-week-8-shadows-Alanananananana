// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1()
{

}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Create Mesh object and shader object
	
	mesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext(), 200);
	Ghost = new AModel(renderer->getDevice(), "res/ghost.fbx");
	Ghost2 = new AModel(renderer->getDevice(), "res/ghost.fbx");
	Ghost3 = new AModel(renderer->getDevice(), "res/ghost.fbx");
	Ghost4 = new AModel(renderer->getDevice(), "res/ghost.fbx");
	sphere = new AModel(renderer->getDevice(), "res/sphere.obj");
	sphere2 = new AModel(renderer->getDevice(), "res/sphere.obj");
	campfire = new AModel(renderer->getDevice(), "res/models/campFire.obj");

	textureMgr->loadTexture(L"ghost", L"res/Ghost_BaseColor.png");
	textureMgr->loadTexture(L"brick", L"res/grassMap2.png");
	textureMgr->loadTexture(L"moon", L"res/moon.png");
	textureMgr->loadTexture(L"height", L"res/grassHeightMap2.png");
	textureMgr->loadTexture(L"campfire", L"res/campfire.png");
	heightMapSRV = textureMgr->getTexture(L"height");

	//initial shaders
	//textureShader = new TextureShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	shadowShader = new ShadowShader(renderer->getDevice(), hwnd);
	objectShader = new ObjectShader(renderer->getDevice(), hwnd);

	//shadow map variables
	int shadowmapWidth = 8192;
	int shadowmapHeight = 8192;
	int sceneWidth = 200;
	int sceneHeight = 200;

	//shadow maps
	shadowMap = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);
	shadowMap2 = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);
	shadowMap3 = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);
	shadowMap4 = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);
	shadowMap5 = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);

	// Configure lights

	light = new Light();
	//light->setAmbientColour(0.6, 0.2, 0, 1);
	light->setDiffuseColour(0.1f, 0.1f, 0.1f, 1.0f);
	light->setDirection(0.0f, -0.7, 0.7);
	light->setPosition(50, 50, 110);
	light->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1, 100);

	light2 = new Light();
	light2->setAmbientColour(0.6, 0.2, 0, 1);
	light2->setDiffuseColour(0.9, 0.3, 0, 1);
	light2->setPosition(campfirePos.x, campfirePos.y+10, campfirePos.z);
	light2->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1, 100);

	light3 = new Light();
	light3->setDiffuseColour(1, 1, 1, 1);
	light3->setPosition(ghostPos.x, 30, ghostPos.z);
	light3->setRange(30);
	light3->setDirection(0, -1, 0);
	light3->setInnerConeAngle(15);
	light3->setOuterConeAngle(30);
	light3->setAttenuation(0.07, 0.007, 0);
	light3->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1, 100);

	light4 = new Light();
	light4->setDiffuseColour(1, 1, 1, 1);
	light4->setPosition(ghostPos2.x, 30, ghostPos2.z);
	light4->setRange(30);
	light4->setDirection(0, -1, 0);
	light4->setInnerConeAngle(15);
	light4->setOuterConeAngle(30);
	light4->setAttenuation(0.07, 0.007, 0);
	light4->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1, 100);

	light5 = new Light();
	light5->setDiffuseColour(1, 1, 1, 1);
	light5->setPosition(ghostPos3.x, 30, ghostPos3.z);
	light5->setRange(30);
	light5->setDirection(0, -1, 0);
	light5->setInnerConeAngle(15);
	light5->setOuterConeAngle(30);
	light5->setAttenuation(0.07, 0.007, 0);
	light5->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1, 100);

	light6 = new Light();
	light6->setDiffuseColour(1, 1, 1, 1);
	light6->setPosition(ghostPos4.x, 30, ghostPos4.z);
	light6->setRange(30);
	light6->setDirection(0, -1, 0);
	light6->setInnerConeAngle(15);
	light6->setOuterConeAngle(30);
	light6->setAttenuation(0.07, 0.007, 0);
	light6->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1, 100);

}

App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.

}


bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}
	
	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::render()
{

	// Perform depth pass
	
	depthPassLight1();
	depthPassLight2();
	depthPassLight3();
	depthPassLight4();
	depthPassLight5();
	depthPassLight6();

	

	finalPass();
	// Render scene

	

	return true;
}

void App1::depthPassLight1()
{
	shadowMap->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());
	// Get the view and projection matrices from light 1
	
	light->generateViewMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX lightViewMatrix = light->getViewMatrix();
	XMMATRIX lightProjectionMatrix = light->getOrthoMatrix();
	XMMATRIX scaleMatrix;
	XMMATRIX rotMatrix;
	XMMATRIX transMatrix;
	// Render each object for light 1
	
	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -0.f);
	mesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), mesh->getIndexCount());


	worldMatrix = renderer->getWorldMatrix();
	transMatrix = XMMatrixTranslation(campfirePos.x, campfirePos.y, campfirePos.z);
	scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	rotMatrix = XMMatrixRotationX(90);
	worldMatrix = (scaleMatrix * rotMatrix * transMatrix);

	campfire->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), campfire->getIndexCount());


	//Ghost 1
	{
		worldMatrix = renderer->getWorldMatrix();
		transMatrix = XMMatrixTranslation(ghostPos.x, ghostPos.y, ghostPos.z);
		scaleMatrix = XMMatrixScaling(ghostScale.x, ghostScale.y, ghostScale.z);
		rotMatrix = XMMatrixRotationX(angleX) * XMMatrixRotationY(-angleY + XMConvertToRadians(90));
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);
		Ghost->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
		depthShader->render(renderer->getDeviceContext(), Ghost->getIndexCount());
	}

	//Ghost 2
	{
		worldMatrix = renderer->getWorldMatrix();
		transMatrix = XMMatrixTranslation(ghostPos2.x, ghostPos2.y, ghostPos2.z);
		scaleMatrix = XMMatrixScaling(ghostScale.x, ghostScale.y, ghostScale.z);
		XMMATRIX rotMatrix = XMMatrixRotationX(angleX) * XMMatrixRotationY(-angleY);
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);
		Ghost2->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
		depthShader->render(renderer->getDeviceContext(), Ghost2->getIndexCount());
	}

	//Ghost 3
	{
		worldMatrix = renderer->getWorldMatrix();
		transMatrix = XMMatrixTranslation(ghostPos3.x, ghostPos3.y, ghostPos3.z);
		scaleMatrix = XMMatrixScaling(ghostScale.x, ghostScale.y, ghostScale.z);
		XMMATRIX rotMatrix = XMMatrixRotationX(angleX) * XMMatrixRotationY(-angleY - XMConvertToRadians(90));
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);
		Ghost3->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
		depthShader->render(renderer->getDeviceContext(), Ghost3->getIndexCount());
	}

	//Ghost 4
	{
		worldMatrix = renderer->getWorldMatrix();
		transMatrix = XMMatrixTranslation(ghostPos4.x, ghostPos4.y, ghostPos4.z);
		scaleMatrix = XMMatrixScaling(ghostScale.x, ghostScale.y, ghostScale.z);
		XMMATRIX rotMatrix = XMMatrixRotationX(angleX) * XMMatrixRotationY(-angleY - XMConvertToRadians(180));
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);
		Ghost4->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
		depthShader->render(renderer->getDeviceContext(), Ghost4->getIndexCount());
	}

	{//// Render spheres for light 1
	//worldMatrix = renderer->getWorldMatrix();
	//worldMatrix = XMMatrixTranslation(ballPos.x, ballPos.y, ballPos.z);
	//worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	//sphere->sendData(renderer->getDeviceContext());
	//depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	//depthShader->render(renderer->getDeviceContext(), sphere->getIndexCount());

	//worldMatrix = renderer->getWorldMatrix();
	//worldMatrix = XMMatrixTranslation(ballPos2.x, ballPos2.y, ballPos2.z);
	//worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	//sphere2->sendData(renderer->getDeviceContext());
	//depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	//depthShader->render(renderer->getDeviceContext(), sphere2->getIndexCount());
	}
	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}
void App1::depthPassLight2() {

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX lightViewMatrix2 = light2->getViewMatrix();
	XMMATRIX lightProjectionMatrix2 = light2->getOrthoMatrix();
	XMMATRIX scaleMatrix;
	XMMATRIX rotMatrix;
	XMMATRIX transMatrix;



	//// Render each object for light 2
	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
	mesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix2, lightProjectionMatrix2);
	depthShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	//worldMatrix = renderer->getWorldMatrix();
	//transMatrix = XMMatrixTranslation(campfirePos.x, campfirePos.y, campfirePos.z);
	////scaleMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	//rotMatrix = XMMatrixRotationX(90);
	//worldMatrix = (rotMatrix * transMatrix);

	//campfire->sendData(renderer->getDeviceContext());
	//depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix2, lightProjectionMatrix2);
	//depthShader->render(renderer->getDeviceContext(), campfire->getIndexCount());


	//Ghost 1
	{
		worldMatrix = renderer->getWorldMatrix();
		transMatrix = XMMatrixTranslation(ghostPos.x, ghostPos.y, ghostPos.z);
		scaleMatrix = XMMatrixScaling(ghostScale.x, ghostScale.y, ghostScale.z);
		rotMatrix = XMMatrixRotationX(90);
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);
		Ghost->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix2, lightProjectionMatrix2);
		depthShader->render(renderer->getDeviceContext(), Ghost->getIndexCount());
	}

	//Ghost 2
	{
		worldMatrix = renderer->getWorldMatrix();
		transMatrix = XMMatrixTranslation(ghostPos2.x, ghostPos2.y, ghostPos2.z);
		scaleMatrix = XMMatrixScaling(ghostScale.x, ghostScale.y, ghostScale.z);
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);
		rotMatrix = XMMatrixRotationX(90) * XMMatrixRotationY(90);
		Ghost2->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix2, lightProjectionMatrix2);
		depthShader->render(renderer->getDeviceContext(), Ghost2->getIndexCount());
	}

	//Ghost 3
	{
		worldMatrix = renderer->getWorldMatrix();
		transMatrix = XMMatrixTranslation(ghostPos3.x, ghostPos3.y, ghostPos3.z);
		scaleMatrix = XMMatrixScaling(ghostScale.x, ghostScale.y, ghostScale.z);
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);
		rotMatrix = XMMatrixRotationX(90) * XMMatrixRotationY(180);
		Ghost3->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix2, lightProjectionMatrix2);
		depthShader->render(renderer->getDeviceContext(), Ghost3->getIndexCount());
	}

	//Ghost 4
	{
		worldMatrix = renderer->getWorldMatrix();
		transMatrix = XMMatrixTranslation(ghostPos4.x, ghostPos4.y, ghostPos4.z);
		scaleMatrix = XMMatrixScaling(ghostScale.x, ghostScale.y, ghostScale.z);
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);
		rotMatrix = XMMatrixRotationX(90) * XMMatrixRotationY(270);
		Ghost4->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix2, lightProjectionMatrix2);
		depthShader->render(renderer->getDeviceContext(), Ghost4->getIndexCount());
	}

	{//// Render spheres for light 2
	//worldMatrix = renderer->getWorldMatrix();
	//worldMatrix = XMMatrixTranslation(ballPos.x, ballPos.y, ballPos.z);
	//worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	//sphere->sendData(renderer->getDeviceContext());
	//depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix2, lightProjectionMatrix2);
	//depthShader->render(renderer->getDeviceContext(), sphere->getIndexCount());

	//worldMatrix = renderer->getWorldMatrix();
	//worldMatrix = XMMatrixTranslation(ballPos2.x, ballPos2.y, ballPos2.z);
	//worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	//sphere2->sendData(renderer->getDeviceContext());
	//depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix2, lightProjectionMatrix2);
	//depthShader->render(renderer->getDeviceContext(), sphere2->getIndexCount());
	}
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();

}
void App1::depthPassLight3() {
	shadowMap2->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	light3->generateViewMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX lightViewMatrix3 = light3->getViewMatrix();
	XMMATRIX lightProjectionMatrix3 = light3->getOrthoMatrix();
	XMMATRIX scaleMatrix;
	XMMATRIX rotMatrix;
	XMMATRIX transMatrix;



	//// Render each object for light 2
	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
	mesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix3, lightProjectionMatrix3);
	depthShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	transMatrix = XMMatrixTranslation(campfirePos.x, campfirePos.y, campfirePos.z);
	//scaleMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	rotMatrix = XMMatrixRotationX(90);
	worldMatrix = (rotMatrix * transMatrix);

	campfire->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix3, lightProjectionMatrix3);
	depthShader->render(renderer->getDeviceContext(), campfire->getIndexCount());


	//Ghost 1
	{
		worldMatrix = renderer->getWorldMatrix();
		transMatrix = XMMatrixTranslation(ghostPos.x, ghostPos.y, ghostPos.z);
		scaleMatrix = XMMatrixScaling(ghostScale.x, ghostScale.y, ghostScale.z);
		rotMatrix = XMMatrixRotationX(angleX) * XMMatrixRotationY(-angleY + XMConvertToRadians(90));
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);
		Ghost->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix3, lightProjectionMatrix3);
		depthShader->render(renderer->getDeviceContext(), Ghost->getIndexCount());
	}

	//Ghost 2
	{
		worldMatrix = renderer->getWorldMatrix();
		transMatrix = XMMatrixTranslation(ghostPos2.x, ghostPos2.y, ghostPos2.z);
		scaleMatrix = XMMatrixScaling(ghostScale.x, ghostScale.y, ghostScale.z);
		XMMATRIX rotMatrix = XMMatrixRotationX(angleX) * XMMatrixRotationY(-angleY);
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);
		Ghost2->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix3, lightProjectionMatrix3);
		depthShader->render(renderer->getDeviceContext(), Ghost2->getIndexCount());
	}

	//Ghost 3
	{
		worldMatrix = renderer->getWorldMatrix();
		transMatrix = XMMatrixTranslation(ghostPos3.x, ghostPos3.y, ghostPos3.z);
		scaleMatrix = XMMatrixScaling(ghostScale.x, ghostScale.y, ghostScale.z);
		
		XMMATRIX rotMatrix = XMMatrixRotationX(angleX) * XMMatrixRotationY(-angleY - XMConvertToRadians(90));
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);
		Ghost3->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix3, lightProjectionMatrix3);
		depthShader->render(renderer->getDeviceContext(), Ghost3->getIndexCount());
	}

	//Ghost 4
	{
		worldMatrix = renderer->getWorldMatrix();
		transMatrix = XMMatrixTranslation(ghostPos4.x, ghostPos4.y, ghostPos4.z);
		scaleMatrix = XMMatrixScaling(ghostScale.x, ghostScale.y, ghostScale.z);
		
		XMMATRIX rotMatrix = XMMatrixRotationX(angleX) * XMMatrixRotationY(-angleY - XMConvertToRadians(180));
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);
		Ghost4->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix3, lightProjectionMatrix3);
		depthShader->render(renderer->getDeviceContext(), Ghost4->getIndexCount());
	}
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}
void App1::depthPassLight4() {

	shadowMap3->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());
	//light 4
	light4->generateViewMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX lightViewMatrix4 = light4->getViewMatrix();
	XMMATRIX lightProjectionMatrix4 = light4->getOrthoMatrix();
	XMMATRIX scaleMatrix;
	XMMATRIX rotMatrix;
	XMMATRIX transMatrix;



	//// Render each object for light 2
	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
	mesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix4, lightProjectionMatrix4);
	depthShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	transMatrix = XMMatrixTranslation(campfirePos.x, campfirePos.y, campfirePos.z);
	//scaleMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	rotMatrix = XMMatrixRotationX(90);
	worldMatrix = (rotMatrix * transMatrix);

	campfire->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix4, lightProjectionMatrix4);
	depthShader->render(renderer->getDeviceContext(), campfire->getIndexCount());


	//Ghost 1
	{
		worldMatrix = renderer->getWorldMatrix();
		transMatrix = XMMatrixTranslation(ghostPos.x, ghostPos.y, ghostPos.z);
		scaleMatrix = XMMatrixScaling(ghostScale.x, ghostScale.y, ghostScale.z);
		rotMatrix = XMMatrixRotationX(angleX) * XMMatrixRotationY(-angleY + XMConvertToRadians(90));
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);
		Ghost->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix4, lightProjectionMatrix4);
		depthShader->render(renderer->getDeviceContext(), Ghost->getIndexCount());
	}

	//Ghost 2
	{
		worldMatrix = renderer->getWorldMatrix();
		transMatrix = XMMatrixTranslation(ghostPos2.x, ghostPos2.y, ghostPos2.z);
		scaleMatrix = XMMatrixScaling(ghostScale.x, ghostScale.y, ghostScale.z);
		
		XMMATRIX rotMatrix = XMMatrixRotationX(angleX) * XMMatrixRotationY(-angleY);
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);
		Ghost2->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix4, lightProjectionMatrix4);
		depthShader->render(renderer->getDeviceContext(), Ghost2->getIndexCount());
	}

	//Ghost 3
	{
		worldMatrix = renderer->getWorldMatrix();
		transMatrix = XMMatrixTranslation(ghostPos3.x, ghostPos3.y, ghostPos3.z);
		scaleMatrix = XMMatrixScaling(ghostScale.x, ghostScale.y, ghostScale.z);
		
		XMMATRIX rotMatrix = XMMatrixRotationX(angleX) * XMMatrixRotationY(-angleY - XMConvertToRadians(90));
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);
		Ghost3->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix4, lightProjectionMatrix4);
		depthShader->render(renderer->getDeviceContext(), Ghost3->getIndexCount());
	}

	//Ghost 4
	{
		worldMatrix = renderer->getWorldMatrix();
		transMatrix = XMMatrixTranslation(ghostPos4.x, ghostPos4.y, ghostPos4.z);
		scaleMatrix = XMMatrixScaling(ghostScale.x, ghostScale.y, ghostScale.z);
		
		XMMATRIX rotMatrix = XMMatrixRotationX(angleX) * XMMatrixRotationY(-angleY - XMConvertToRadians(180));
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);
		Ghost4->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix4, lightProjectionMatrix4);
		depthShader->render(renderer->getDeviceContext(), Ghost4->getIndexCount());
	}

	{//// Render spheres for light 2
	//worldMatrix = renderer->getWorldMatrix();
	//worldMatrix = XMMatrixTranslation(ballPos.x, ballPos.y, ballPos.z);
	//worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	//sphere->sendData(renderer->getDeviceContext());
	//depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix4, lightProjectionMatrix4);
	//depthShader->render(renderer->getDeviceContext(), sphere->getIndexCount());

	//worldMatrix = renderer->getWorldMatrix();
	//worldMatrix = XMMatrixTranslation(ballPos2.x, ballPos2.y, ballPos2.z);
	//worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	//sphere2->sendData(renderer->getDeviceContext());
	//depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix4, lightProjectionMatrix4);
	//depthShader->render(renderer->getDeviceContext(), sphere2->getIndexCount());
	}
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}
void App1::depthPassLight5() {
	shadowMap4->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());
	//light 5
	light5->generateViewMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX lightViewMatrix5 = light5->getViewMatrix();
	XMMATRIX lightProjectionMatrix5 = light5->getOrthoMatrix();
	XMMATRIX scaleMatrix;
	XMMATRIX rotMatrix;
	XMMATRIX transMatrix;



	//// Render each object for light 2
	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
	mesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix5, lightProjectionMatrix5);
	depthShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	transMatrix = XMMatrixTranslation(campfirePos.x, campfirePos.y, campfirePos.z);
	//scaleMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	rotMatrix = XMMatrixRotationX(90);
	worldMatrix = (rotMatrix * transMatrix);

	campfire->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix5, lightProjectionMatrix5);
	depthShader->render(renderer->getDeviceContext(), campfire->getIndexCount());


	//Ghost 1
	{
		worldMatrix = renderer->getWorldMatrix();
		transMatrix = XMMatrixTranslation(ghostPos.x, ghostPos.y, ghostPos.z);
		scaleMatrix = XMMatrixScaling(ghostScale.x, ghostScale.y, ghostScale.z);
		rotMatrix = XMMatrixRotationX(angleX) * XMMatrixRotationY(-angleY + XMConvertToRadians(90));
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);
		Ghost->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix5, lightProjectionMatrix5);
		depthShader->render(renderer->getDeviceContext(), Ghost->getIndexCount());
	}

	//Ghost 2
	{
		worldMatrix = renderer->getWorldMatrix();
		transMatrix = XMMatrixTranslation(ghostPos2.x, ghostPos2.y, ghostPos2.z);
		scaleMatrix = XMMatrixScaling(ghostScale.x, ghostScale.y, ghostScale.z);

		XMMATRIX rotMatrix = XMMatrixRotationX(angleX) * XMMatrixRotationY(-angleY + XMConvertToRadians(180));
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);
		Ghost2->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix5, lightProjectionMatrix5);
		depthShader->render(renderer->getDeviceContext(), Ghost2->getIndexCount());
	}

	//Ghost 3
	{
		worldMatrix = renderer->getWorldMatrix();
		transMatrix = XMMatrixTranslation(ghostPos3.x, ghostPos3.y, ghostPos3.z);
		scaleMatrix = XMMatrixScaling(ghostScale.x, ghostScale.y, ghostScale.z);

		XMMATRIX rotMatrix = XMMatrixRotationX(angleX) * XMMatrixRotationY(-angleY - XMConvertToRadians(90));
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);
		Ghost3->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix5, lightProjectionMatrix5);
		depthShader->render(renderer->getDeviceContext(), Ghost3->getIndexCount());
	}

	//Ghost 4
	{
		worldMatrix = renderer->getWorldMatrix();
		transMatrix = XMMatrixTranslation(ghostPos4.x, ghostPos4.y, ghostPos4.z);
		scaleMatrix = XMMatrixScaling(ghostScale.x, ghostScale.y, ghostScale.z);

		XMMATRIX rotMatrix = XMMatrixRotationX(angleX) * XMMatrixRotationY(-angleY - XMConvertToRadians(180));
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);
		Ghost4->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix5, lightProjectionMatrix5);
		depthShader->render(renderer->getDeviceContext(), Ghost4->getIndexCount());
	}

	{//// Render spheres for light 2
	//worldMatrix = renderer->getWorldMatrix();
	//worldMatrix = XMMatrixTranslation(ballPos.x, ballPos.y, ballPos.z);
	//worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	//sphere->sendData(renderer->getDeviceContext());
	//depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix5, lightProjectionMatrix5);
	//depthShader->render(renderer->getDeviceContext(), sphere->getIndexCount());

	//worldMatrix = renderer->getWorldMatrix();
	//worldMatrix = XMMatrixTranslation(ballPos2.x, ballPos2.y, ballPos2.z);
	//worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	//sphere2->sendData(renderer->getDeviceContext());
	//depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix5, lightProjectionMatrix5);
	//depthShader->render(renderer->getDeviceContext(), sphere2->getIndexCount());
	}

	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}
void App1::depthPassLight6() {
	shadowMap5->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	//light 6
	light6->generateViewMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX lightViewMatrix6 = light6->getViewMatrix();
	XMMATRIX lightProjectionMatrix6 = light6->getOrthoMatrix();
	XMMATRIX scaleMatrix;
	XMMATRIX rotMatrix;
	XMMATRIX transMatrix;



	//// Render each object for light 2
	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
	mesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix6, lightProjectionMatrix6);
	depthShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	transMatrix = XMMatrixTranslation(campfirePos.x, campfirePos.y, campfirePos.z);
	//scaleMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	rotMatrix = XMMatrixRotationX(90);
	worldMatrix = (rotMatrix * transMatrix);

	campfire->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix6, lightProjectionMatrix6);
	depthShader->render(renderer->getDeviceContext(), campfire->getIndexCount());


	//Ghost 1
	{
		worldMatrix = renderer->getWorldMatrix();
		transMatrix = XMMatrixTranslation(ghostPos.x, ghostPos.y, ghostPos.z);
		scaleMatrix = XMMatrixScaling(ghostScale.x, ghostScale.y, ghostScale.z);
		rotMatrix = XMMatrixRotationX(angleX) * XMMatrixRotationY(-angleY + XMConvertToRadians(90));
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);
		Ghost->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix6, lightProjectionMatrix6);
		depthShader->render(renderer->getDeviceContext(), Ghost->getIndexCount());
	}

	//Ghost 2
	{
		worldMatrix = renderer->getWorldMatrix();
		transMatrix = XMMatrixTranslation(ghostPos2.x, ghostPos2.y, ghostPos2.z);
		scaleMatrix = XMMatrixScaling(ghostScale.x, ghostScale.y, ghostScale.z);

		XMMATRIX rotMatrix = XMMatrixRotationX(angleX) * XMMatrixRotationY(-angleY + XMConvertToRadians(180));
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);
		Ghost2->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix6, lightProjectionMatrix6);
		depthShader->render(renderer->getDeviceContext(), Ghost2->getIndexCount());
	}

	//Ghost 3
	{
		worldMatrix = renderer->getWorldMatrix();
		transMatrix = XMMatrixTranslation(ghostPos3.x, ghostPos3.y, ghostPos3.z);
		scaleMatrix = XMMatrixScaling(ghostScale.x, ghostScale.y, ghostScale.z);

		XMMATRIX rotMatrix = XMMatrixRotationX(angleX) * XMMatrixRotationY(-angleY - XMConvertToRadians(90));
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);
		Ghost3->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix6, lightProjectionMatrix6);
		depthShader->render(renderer->getDeviceContext(), Ghost3->getIndexCount());
	}

	//Ghost 4
	{
		worldMatrix = renderer->getWorldMatrix();
		transMatrix = XMMatrixTranslation(ghostPos4.x, ghostPos4.y, ghostPos4.z);
		scaleMatrix = XMMatrixScaling(ghostScale.x, ghostScale.y, ghostScale.z);

		XMMATRIX rotMatrix = XMMatrixRotationX(angleX) * XMMatrixRotationY(-angleY - XMConvertToRadians(180));
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);
		Ghost4->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix6, lightProjectionMatrix6);
		depthShader->render(renderer->getDeviceContext(), Ghost4->getIndexCount());
	}

	{//// Render spheres for light 2
	//worldMatrix = renderer->getWorldMatrix();
	//worldMatrix = XMMatrixTranslation(ballPos.x, ballPos.y, ballPos.z);
	//worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	//sphere->sendData(renderer->getDeviceContext());
	//depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix6, lightProjectionMatrix6);
	//depthShader->render(renderer->getDeviceContext(), sphere->getIndexCount());

	//worldMatrix = renderer->getWorldMatrix();
	//worldMatrix = XMMatrixTranslation(ballPos2.x, ballPos2.y, ballPos2.z);
	//worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	//sphere2->sendData(renderer->getDeviceContext());
	//depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix6, lightProjectionMatrix6);
	//depthShader->render(renderer->getDeviceContext(), sphere2->getIndexCount());
	}

	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}
void App1::finalPass()
{

	ID3D11ShaderResourceView* shadowMaps[5] = { shadowMap->getDepthMapSRV(), shadowMap2->getDepthMapSRV(), shadowMap3->getDepthMapSRV(), shadowMap4->getDepthMapSRV(), shadowMap5->getDepthMapSRV() };
	renderer->getDeviceContext()->PSSetShaderResources(1, 5, shadowMaps);

	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);
	camera->update();

	angleX = XMConvertToRadians(90);
	angleY += speed;
	bobAngle += bobSpeed;


	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	// Render floor
	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -0.f);
	mesh->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
		textureMgr->getTexture(L"brick"), 
		shadowMap->getDepthMapSRV(),
		shadowMap2->getDepthMapSRV(),
		shadowMap3->getDepthMapSRV(), 
		shadowMap4->getDepthMapSRV(), 
		shadowMap5->getDepthMapSRV(), heightMapSRV,
		light, light2, light3, light4, light5, light6, camera);
	shadowShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	//CAMPFIRE

	worldMatrix = renderer->getWorldMatrix();
	XMMATRIX transMatrix = XMMatrixTranslation(campfirePos.x, campfirePos.y, campfirePos.z);
	XMMATRIX scaleMatrix = XMMatrixScaling(5, 5, 5);

	//XMMATRIX rotMatrix = XMMatrixRotationX(angleX) * XMMatrixRotationY(angleX);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	//worldMatrix = XMMatrixMultiply(worldMatrix, rotMatrix);
	worldMatrix = XMMatrixMultiply(worldMatrix, transMatrix);

	campfire->sendData(renderer->getDeviceContext());
	objectShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
		textureMgr->getTexture(L"campfire"), 
		shadowMap->getDepthMapSRV(),
		shadowMap2->getDepthMapSRV(),
		shadowMap3->getDepthMapSRV(),
		shadowMap4->getDepthMapSRV(),
		shadowMap5->getDepthMapSRV(),
		light, light2, light3, light4, light5, light6, camera);
	objectShader->render(renderer->getDeviceContext(), campfire->getIndexCount());


	//Ghost 1
	{
		angleY += speed; //Increment angle for circular motion
		offsetX = radius * cos(angleY); //offsets the position of the ghost by an amount dependant on the angle Y (Which is being constantly changed by "speed". By taking the cosine of this, we can always get a value between -1 and 1. After multiplying THAT by the radius, we get a circle :D
		offsetY = bobHeight * cos(-bobAngle + XMConvertToRadians(90)); //Creates a bobbing effect for the ghosts to move up and down slightly when moving. Added the 90 radians to offset the bobbing from the other ghosts so that theyre not perfectly in sync
		offsetZ = radius * sin(angleY); //Offsets the position of the Z so that we move in a circle. We use sine to compliment the cosine of the X axis, otherwise the ghost just moves in a diagonal line rather than a circle
		ghostPos.x = orbitCentre.x + offsetX; //Sets the new positions of the ghost to the ghostPos vector
		ghostPos.y = orbitCentre.y + offsetY;
		ghostPos.z = orbitCentre.z + offsetZ;

		// Transform the ghost
		worldMatrix = renderer->getWorldMatrix();
		light3->setPosition(ghostPos.x, 30, ghostPos.z);
		XMMATRIX transMatrix = XMMatrixTranslation(ghostPos.x, ghostPos.y, ghostPos.z);
		XMMATRIX scaleMatrix = XMMatrixScaling(ghostScale.x, ghostScale.y, ghostScale.z);
		XMMATRIX rotMatrix = XMMatrixRotationX(angleX) * XMMatrixRotationY(-angleY + XMConvertToRadians(90));
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);

		Ghost->sendData(renderer->getDeviceContext());
		objectShader->setShaderParameters(renderer->getDeviceContext(), 
			worldMatrix, viewMatrix, projectionMatrix,
			textureMgr->getTexture(L"ghost"), 
			shadowMap->getDepthMapSRV(),
			shadowMap2->getDepthMapSRV(),
			shadowMap3->getDepthMapSRV(),
			shadowMap4->getDepthMapSRV(),
			shadowMap5->getDepthMapSRV(),
			light, light2, light3, light4, light5, light6, camera);
		objectShader->render(renderer->getDeviceContext(), Ghost->getIndexCount());
	}

	//Ghost 2
	{		
		offsetX = radius * cos(angleY + XMConvertToRadians(90));
		offsetY = bobHeight * cos(bobAngle);
		offsetZ = radius * sin(angleY + XMConvertToRadians(90));
		ghostPos2.x = orbitCentre.x + offsetX;
		ghostPos2.y = orbitCentre.y + offsetY;
		ghostPos2.z = orbitCentre.z + offsetZ;
		
		// Transform the ghost
		worldMatrix = renderer->getWorldMatrix();
		light4->setPosition(ghostPos2.x, 30, ghostPos2.z);
		XMMATRIX transMatrix = XMMatrixTranslation(ghostPos2.x, ghostPos2.y, ghostPos2.z);
		XMMATRIX scaleMatrix = XMMatrixScaling(ghostScale.x, ghostScale.y, ghostScale.z);
		XMMATRIX rotMatrix = XMMatrixRotationX(angleX) * XMMatrixRotationY(-angleY);
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);

		Ghost2->sendData(renderer->getDeviceContext());
		objectShader->setShaderParameters(renderer->getDeviceContext(), 
			worldMatrix, viewMatrix, projectionMatrix, 
			textureMgr->getTexture(L"ghost"), 
			shadowMap->getDepthMapSRV(), 
			shadowMap2->getDepthMapSRV(),
			shadowMap3->getDepthMapSRV(),
			shadowMap4->getDepthMapSRV(),
			shadowMap5->getDepthMapSRV(),
			light, light2, light3, light4, light5, light6, camera);
		objectShader->render(renderer->getDeviceContext(), Ghost2->getIndexCount());
	}

	//Ghost 3
	{
		angleY += speed;
		offsetX = radius * cos(angleY + XMConvertToRadians(180));
		offsetY = bobHeight * cos(bobAngle + XMConvertToRadians(180));
		offsetZ = radius * sin(angleY + XMConvertToRadians(180));
		ghostPos3.x = orbitCentre.x + offsetX;
		ghostPos3.y = orbitCentre.y + offsetY;
		ghostPos3.z = orbitCentre.z + offsetZ;

		// Transform the ghost
		worldMatrix = renderer->getWorldMatrix();
		light5->setPosition(ghostPos3.x, 30, ghostPos3.z);
		XMMATRIX transMatrix = XMMatrixTranslation(ghostPos3.x, ghostPos3.y, ghostPos3.z);
		XMMATRIX scaleMatrix = XMMatrixScaling(ghostScale.x, ghostScale.y, ghostScale.z);
		XMMATRIX rotMatrix = XMMatrixRotationX(angleX) * XMMatrixRotationY(-angleY - XMConvertToRadians(90));
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);

		Ghost3->sendData(renderer->getDeviceContext());
		objectShader->setShaderParameters(renderer->getDeviceContext(), 
			worldMatrix, viewMatrix, projectionMatrix,
			textureMgr->getTexture(L"ghost"), 
			shadowMap->getDepthMapSRV(),
			shadowMap2->getDepthMapSRV(),
			shadowMap3->getDepthMapSRV(),
			shadowMap4->getDepthMapSRV(),
			shadowMap5->getDepthMapSRV(),
			light, light2, light3, light4, light5, light6, camera);
		objectShader->render(renderer->getDeviceContext(), Ghost3->getIndexCount());
	}

	//Ghost 4
	{
		angleY += speed; //Increment angle for circular motion
		offsetX = radius * cos(angleY - XMConvertToRadians(90));
		offsetY = bobHeight * cos(-bobAngle - XMConvertToRadians(90));
		offsetZ = radius * sin(angleY - XMConvertToRadians(90));
		ghostPos4.x = orbitCentre.x + offsetX;
		ghostPos4.y = orbitCentre.y + offsetY;
		ghostPos4.z = orbitCentre.z + offsetZ;

		worldMatrix = renderer->getWorldMatrix();
		light6->setPosition(ghostPos4.x, 30, ghostPos4.z);
		XMMATRIX transMatrix = XMMatrixTranslation(ghostPos4.x, ghostPos4.y, ghostPos4.z);
		XMMATRIX scaleMatrix = XMMatrixScaling(ghostScale.x, ghostScale.y, ghostScale.z);
		XMMATRIX rotMatrix = XMMatrixRotationX(angleX) * XMMatrixRotationY(-angleY - XMConvertToRadians(180));
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);

		Ghost4->sendData(renderer->getDeviceContext());
		objectShader->setShaderParameters(renderer->getDeviceContext(), 
			worldMatrix, viewMatrix, projectionMatrix,
			textureMgr->getTexture(L"ghost"), 
			shadowMap->getDepthMapSRV(),
			shadowMap2->getDepthMapSRV(),
			shadowMap3->getDepthMapSRV(),
			shadowMap4->getDepthMapSRV(),
			shadowMap5->getDepthMapSRV(),
			light, light2, light3, light4, light5, light6, camera);
		objectShader->render(renderer->getDeviceContext(), Ghost4->getIndexCount());
	}

	//// Render spheres using light 1 and light 2
	//worldMatrix = renderer->getWorldMatrix();
	//worldMatrix = XMMatrixTranslation(ballPos.x, ballPos.y, ballPos.z);
	////worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	//sphere->sendData(renderer->getDeviceContext());
	//objectShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
	//	textureMgr->getTexture(L"moon"), shadowMap->getDepthMapSRV(),
	//	shadowMap2->getDepthMapSRV(), 
	//	light, light2, camera);
	//objectShader->render(renderer->getDeviceContext(), sphere->getIndexCount());


	//worldMatrix = renderer->getWorldMatrix();
	//worldMatrix = XMMatrixTranslation(ballPos2.x, ballPos2.y, ballPos2.z);
	////worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	//sphere2->sendData(renderer->getDeviceContext());
	//objectShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
	//	textureMgr->getTexture(L"moon"), shadowMap->getDepthMapSRV(),
	//	shadowMap2->getDepthMapSRV(), 
	//	light, light2, camera);
	//objectShader->render(renderer->getDeviceContext(), sphere2->getIndexCount());

	// Render GUI
	gui();
	renderer->endScene();
}




void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	
	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	//XMFLOAT3 lightPosition = light->getPosition();
	//XMFLOAT3 lightDirection = light->getDirection();

	//ImGui::SliderFloat("light position X", &lightPosition.x, -50.0f, 50.0f);
	//ImGui::SliderFloat("light position Y", &lightPosition.y, -50.0f, 50.0f);
	//ImGui::SliderFloat("light position Z", &lightPosition.z, -50.0f, 50.0f);
	//ImGui::SliderFloat("Light direction X", &lightDirection.x, -1, 1);
	//ImGui::SliderFloat("Light direction Y", &lightDirection.y, -1, 1);
	//ImGui::SliderFloat("Light direction Z", &lightDirection.z, -1, 1);

	//light->setPosition(lightPosition.x, lightPosition.y, lightPosition.z);
	//light->setDirection(lightDirection.x, lightDirection.y, lightDirection.z);

	//ballPos = lightPosition;


	//XMFLOAT3 light2Position = light2->getPosition();
	////XMFLOAT3 light2Direction = light2->getDirection();

	//ImGui::SliderFloat("light2 position X", &light2Position.x, -100, 100);
	//ImGui::SliderFloat("light2 position Y", &light2Position.y, -100, 100);
	//ImGui::SliderFloat("light2 position Z", &light2Position.z, -100, 100);

	////ImGui::SliderFloat("Light2 direction X", &light2Direction.x, -1, 1);
	////ImGui::SliderFloat("Light2 direction Y", &light2Direction.y, -1, 1);
	////ImGui::SliderFloat("Light2 direction Z", &light2Direction.z, -1, 1);

	//light2->setPosition(light2Position.x, light2Position.y, light2Position.z);
	////light2->setDirection(light2Direction.x, light2Direction.y, light2Direction.z);
	//ballPos2 = light2->getPosition();


	/*ImGui::SliderFloat("ghost speed", &speed, -0.1, 0.1f);
	ImGui::SliderFloat("bob speed", &bobSpeed, -0.1, 0.1f);
	ImGui::SliderFloat("bob height", &bobHeight, 0, 10);*/


	//XMFLOAT3 lightPosition = light3->getPosition();
	XMFLOAT3 lightAttenuation = light3->getAttenuation();
	float lightRange = light3->getRange();
	float lightInnerAngle = light3->getInnerConeAngle();
	float lightOuterAngle = light3->getOuterConeAngle();

	ImGui::SliderFloat("Light attenuation Constant", &lightAttenuation.x, -10, 10);
	ImGui::SliderFloat("Light direction Lnear", &lightAttenuation.y, -1, 1);
	ImGui::SliderFloat("Light direction Quadratic", &lightAttenuation.z, -0.1, 0.1);

	ImGui::SliderFloat("Light range", &lightRange, 0, 500);
	ImGui::SliderFloat("Light inner angle", &lightInnerAngle, 0, 180);
	ImGui::SliderFloat("Light outer angle", &lightOuterAngle, 0, 180);

	light3->setAttenuation(lightAttenuation.x, lightAttenuation.y, lightAttenuation.z);
	light3->setRange(lightRange);
	light3->setInnerConeAngle(lightInnerAngle);
	light3->setOuterConeAngle(lightOuterAngle);
	XMFLOAT3 light2Direction = light3->getDirection();
	ImGui::SliderFloat("Light3 direction X", &light2Direction.x, -1, 1);
	ImGui::SliderFloat("Light3 direction Y", &light2Direction.y, -1, 1);
	ImGui::SliderFloat("Light3 direction Z", &light2Direction.z, -1, 1);

	light3->setDirection(light2Direction.x, light2Direction.y, light2Direction.z);


	

	//XMFLOAT3 ghostPosition = ghostPos;

	//ImGui::SliderFloat("ghost position X", &ghostPosition.x, -50.0f, 50.0f);
	//ImGui::SliderFloat("ghost position Y", &ghostPosition.y, -50.0f, 50.0f);
	//ImGui::SliderFloat("ghost position Z", &ghostPosition.z, -50.0f, 50.0f);

	//ghostPos = ghostPosition;
	
	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}