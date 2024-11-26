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
	
	mesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext(), 100);
	Ghost = new AModel(renderer->getDevice(), "res/ghost.fbx");
	Ghost2 = new AModel(renderer->getDevice(), "res/ghost.fbx");
	Ghost3 = new AModel(renderer->getDevice(), "res/ghost.fbx");
	Ghost4 = new AModel(renderer->getDevice(), "res/ghost.fbx");
	sphere = new AModel(renderer->getDevice(), "res/sphere.obj");
	sphere2 = new AModel(renderer->getDevice(), "res/sphere.obj");

	textureMgr->loadTexture(L"ghost", L"res/Ghost_BaseColor.png");
	textureMgr->loadTexture(L"brick", L"res/grassMap.png");
	textureMgr->loadTexture(L"moon", L"res/moon.png");
	textureMgr->loadTexture(L"height", L"res/grassHeightMap.png");
	heightMapSRV = textureMgr->getTexture(L"height");

	// initial shaders
	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	shadowShader = new ShadowShader(renderer->getDevice(), hwnd);
	objectShader = new ObjectShader(renderer->getDevice(), hwnd);

	// Variables for defining shadow map
	int shadowmapWidth = 8192;
	int shadowmapHeight = 8192;
	int sceneWidth = 100;
	int sceneHeight = 100;

	// This is your shadow map
	shadowMap = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);
	shadowMap2 = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);

	// Configure directional light

	light = new Light();
	//light->setAmbientColour(0.3f, 0.3f, 0.3f, 1.0f);
	//light->setDiffuseColour(0.5f, 0.5f, 1.0f, 1.0f);
	light->setDirection(0.0f, -0.7f, 0.7f);
	light->setPosition(0.f, 20.f, 0.f);
	//light 1 is 30 units away from the teapot in the negative z axis
	light->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1f, 100.f);


	//LIGHT 2 LIGHT 2 LIGHT 2 LIGHT 2 LIGHT 2 LIGHT 2 LIGHT 2 LIGHT 2 LIGHT 2 LIGHT 2 
	light2 = new Light();
	light2->setAmbientColour(0.1f, 0.1f, 0.1f, 1.0f);
	light2->setDiffuseColour(1.0f, 0.5f, 0.5f, 1.0f);
	light2->setDirection(0.0f, -0.7f, -0.7f);
	light2->setPosition(0.f, 20.f, 100.f);
	//light 2 is 30 units away from the teapot in the positive z axis
	light2->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1f, 100.f);
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

	//Ghost 1 
	{
		worldMatrix = renderer->getWorldMatrix();
		transMatrix = XMMatrixTranslation(ghostPos.x, ghostPos.y, ghostPos.z);
		scaleMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
		rotMatrix = XMMatrixRotationX(90);
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);

		Ghost->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
		depthShader->render(renderer->getDeviceContext(), Ghost->getIndexCount());
	}
	//Ghost 2
	{
		worldMatrix = renderer->getWorldMatrix();
		transMatrix = XMMatrixTranslation(ghostPos2.x, ghostPos2.y, ghostPos2.z);
		scaleMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
		rotMatrix = XMMatrixRotationX(90) * XMMatrixRotationY(90);

		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);

		Ghost2->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
		depthShader->render(renderer->getDeviceContext(), Ghost2->getIndexCount());
	}

	//Ghost 3
	{
		worldMatrix = renderer->getWorldMatrix();
		transMatrix = XMMatrixTranslation(ghostPos3.x, ghostPos3.y, ghostPos3.z);
		scaleMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
		rotMatrix = XMMatrixRotationX(90) * XMMatrixRotationY(180);
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);

		Ghost3->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
		depthShader->render(renderer->getDeviceContext(), Ghost3->getIndexCount());
	}

	////Ghost 4
	//{
	//	worldMatrix = renderer->getWorldMatrix();
	//	transMatrix = XMMatrixTranslation(ghostPos4.x, ghostPos4.y, ghostPos4.z);
	//	scaleMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	//	rotMatrix = XMMatrixRotationX(90) * XMMatrixRotationY(270);
	//	worldMatrix = (scaleMatrix * rotMatrix * transMatrix);

	//	Ghost4->sendData(renderer->getDeviceContext());
	//	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	//	depthShader->render(renderer->getDeviceContext(), Ghost4->getIndexCount());
	//}

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

	shadowMap2->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	// Get the view and projection matrices from light 2
	light2->generateViewMatrix();
	XMMATRIX lightViewMatrix2 = light2->getViewMatrix();
	XMMATRIX lightProjectionMatrix2 = light2->getOrthoMatrix();

	worldMatrix = renderer->getWorldMatrix();

	// Render each object for light 2
	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
	mesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix2, lightProjectionMatrix2);
	depthShader->render(renderer->getDeviceContext(), mesh->getIndexCount());


	//Ghost 1
	{
		worldMatrix = renderer->getWorldMatrix();
		transMatrix = XMMatrixTranslation(ghostPos.x, ghostPos.y, ghostPos.z);
		scaleMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);

		Ghost->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix2, lightProjectionMatrix2);
		depthShader->render(renderer->getDeviceContext(), Ghost->getIndexCount());
	}

	//Ghost 2
	{
		worldMatrix = renderer->getWorldMatrix();
		transMatrix = XMMatrixTranslation(ghostPos2.x, ghostPos2.y, ghostPos2.z);
		scaleMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);

		Ghost2->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix2, lightProjectionMatrix2);
		depthShader->render(renderer->getDeviceContext(), Ghost2->getIndexCount());
	}

	//Ghost 3
	{
		worldMatrix = renderer->getWorldMatrix();
		transMatrix = XMMatrixTranslation(ghostPos3.x, ghostPos3.y, ghostPos3.z);
		scaleMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);

		Ghost3->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix2, lightProjectionMatrix2);
		depthShader->render(renderer->getDeviceContext(), Ghost3->getIndexCount());
	}

	////Ghost 4
	//{
	//	worldMatrix = renderer->getWorldMatrix();
	//	transMatrix = XMMatrixTranslation(ghostPos4.x, ghostPos4.y, ghostPos4.z);
	//	scaleMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	//	worldMatrix = (scaleMatrix * rotMatrix * transMatrix);

	//	Ghost4->sendData(renderer->getDeviceContext());
	//	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix2, lightProjectionMatrix2);
	//	depthShader->render(renderer->getDeviceContext(), Ghost4->getIndexCount());
	//}

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

	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}


void App1::finalPass()
{

	ID3D11ShaderResourceView* shadowMaps[2] = { shadowMap->getDepthMapSRV(), shadowMap2->getDepthMapSRV() };
	renderer->getDeviceContext()->PSSetShaderResources(1, 2, shadowMaps);

	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);
	camera->update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	// Render floor
	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -0.f);
	mesh->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
		textureMgr->getTexture(L"brick"), shadowMap->getDepthMapSRV(),
		shadowMap2->getDepthMapSRV(), heightMapSRV,
		light, light2);
	shadowShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	//Ghost 1
	{
		worldMatrix = renderer->getWorldMatrix();
		XMMATRIX transMatrix = XMMatrixTranslation(ghostPos.x, ghostPos.y, ghostPos.z);
		XMMATRIX scaleMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
		float angle = XMConvertToRadians(90);
		XMMATRIX rotMatrix = XMMatrixRotationX(angle);
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);

		Ghost->sendData(renderer->getDeviceContext());
		objectShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
			textureMgr->getTexture(L"ghost"), shadowMap->getDepthMapSRV(),
			shadowMap2->getDepthMapSRV(), 
			light, light2);
		objectShader->render(renderer->getDeviceContext(), Ghost->getIndexCount());
	}

	//Ghost 2
	{
		worldMatrix = renderer->getWorldMatrix();
		XMMATRIX transMatrix = XMMatrixTranslation(ghostPos2.x, ghostPos2.y, ghostPos2.z);
		XMMATRIX scaleMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
		float angle = XMConvertToRadians(90);
		XMMATRIX rotMatrix = XMMatrixRotationX(angle) * XMMatrixRotationY(angle);
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);

		Ghost2->sendData(renderer->getDeviceContext());
		objectShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
			textureMgr->getTexture(L"ghost"), shadowMap->getDepthMapSRV(),
			shadowMap2->getDepthMapSRV(), 
			light, light2);
		objectShader->render(renderer->getDeviceContext(), Ghost2->getIndexCount());
	}

	//Ghost 3
	{
		worldMatrix = renderer->getWorldMatrix();
		XMMATRIX transMatrix = XMMatrixTranslation(ghostPos3.x, ghostPos3.y, ghostPos3.z);
		XMMATRIX scaleMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
		float angleX = XMConvertToRadians(90);
		float angleY = XMConvertToRadians(180);
		XMMATRIX rotMatrix = XMMatrixRotationX(angleX) * XMMatrixRotationY(angleY);
		worldMatrix = (scaleMatrix * rotMatrix * transMatrix);

		Ghost3->sendData(renderer->getDeviceContext());
		objectShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
			textureMgr->getTexture(L"ghost"), shadowMap->getDepthMapSRV(),
			shadowMap2->getDepthMapSRV(), 
			light, light2);
		objectShader->render(renderer->getDeviceContext(), Ghost3->getIndexCount());
	}

	////Ghost 4
	//{
	//	worldMatrix = renderer->getWorldMatrix();
	//	XMMATRIX transMatrix = XMMatrixTranslation(ghostPos4.x, ghostPos4.y, ghostPos4.z);
	//	XMMATRIX scaleMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	//	float angleX = XMConvertToRadians(90);
	//	float angleY = XMConvertToRadians(270);
	//	XMMATRIX rotMatrix = XMMatrixRotationX(angleX) * XMMatrixRotationY(angleY);
	//	worldMatrix = (scaleMatrix * rotMatrix * transMatrix);

	//	Ghost4->sendData(renderer->getDeviceContext());
	//	objectShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
	//		textureMgr->getTexture(L"ghost"), shadowMap->getDepthMapSRV(),
	//		shadowMap2->getDepthMapSRV(), 
	//		light, light2);
	//	objectShader->render(renderer->getDeviceContext(), Ghost4->getIndexCount());
	//}

	// Render spheres using light 1 and light 2
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(ballPos.x, ballPos.y, ballPos.z);
	//worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	sphere->sendData(renderer->getDeviceContext());
	objectShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
		textureMgr->getTexture(L"moon"), shadowMap->getDepthMapSRV(),
		shadowMap2->getDepthMapSRV(), 
		light, light2);
	objectShader->render(renderer->getDeviceContext(), sphere->getIndexCount());


	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(ballPos2.x, ballPos2.y, ballPos2.z);
	//worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	sphere2->sendData(renderer->getDeviceContext());
	objectShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
		textureMgr->getTexture(L"moon"), shadowMap->getDepthMapSRV(),
		shadowMap2->getDepthMapSRV(), 
		light, light2);
	objectShader->render(renderer->getDeviceContext(), sphere2->getIndexCount());

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

	XMFLOAT3 lightPosition = light->getPosition();
	XMFLOAT3 lightDirection = light->getDirection();

	ImGui::SliderFloat("light position X", &lightPosition.x, -50.0f, 50.0f);
	ImGui::SliderFloat("light position Y", &lightPosition.y, -50.0f, 50.0f);
	ImGui::SliderFloat("light position Z", &lightPosition.z, -50.0f, 50.0f);
	ImGui::SliderFloat("Light direction X", &lightDirection.x, -1, 1);
	ImGui::SliderFloat("Light direction Y", &lightDirection.y, -1, 1);
	ImGui::SliderFloat("Light direction Z", &lightDirection.z, -1, 1);

	light->setPosition(lightPosition.x, lightPosition.y, lightPosition.z);
	light->setDirection(lightDirection.x, lightDirection.y, lightDirection.z);

	ballPos = lightPosition;


	XMFLOAT3 light2Position = light2->getPosition();
	XMFLOAT3 light2Direction = light2->getDirection();

	ImGui::SliderFloat("light2 position X", &light2Position.x, -50.0f, 50.0f);
	ImGui::SliderFloat("light2 position Y", &light2Position.y, -50.0f, 50.0f);
	ImGui::SliderFloat("light2 position Z", &light2Position.z, -50.0f, 50.0f);
	ImGui::SliderFloat("Light2 direction X", &light2Direction.x, -1, 1);
	ImGui::SliderFloat("Light2 direction Y", &light2Direction.y, -1, 1);
	ImGui::SliderFloat("Light2 direction Z", &light2Direction.z, -1, 1);

	light2->setPosition(light2Position.x, light2Position.y, light2Position.z);
	light2->setDirection(light2Direction.x, light2Direction.y, light2Direction.z);

	ballPos2 = light2->getPosition();

	XMFLOAT3 ghostPosition = ghostPos;

	ImGui::SliderFloat("ghost position X", &ghostPosition.x, -50.0f, 50.0f);
	ImGui::SliderFloat("ghost position Y", &ghostPosition.y, -50.0f, 50.0f);
	ImGui::SliderFloat("ghost position Z", &ghostPosition.z, -50.0f, 50.0f);

	ghostPos = ghostPosition;
	
	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

