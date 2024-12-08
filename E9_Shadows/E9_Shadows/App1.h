// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	// include dxframework
#include "TextureShader.h"
#include "ShadowShader.h"
#include "ObjectShader.h"
#include "DepthShader.h"


class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();
	XMFLOAT3 ballPos = { 50, 7, -10 };
	XMFLOAT3 ballPos2 = { 50, 7, -40 };
	XMFLOAT3 ghostPos = { 55, 7, 135 };
	XMFLOAT3 ghostPos2 = { 85, 7, 110 };
	XMFLOAT3 ghostPos3 = { 55, 7, 85 };
	XMFLOAT3 ghostPos4 = { 25, 7, 110 };
	XMFLOAT3 campfirePos = { 55, 7, 110 };
	XMFLOAT3 orbitCentre = { 55, 7, 110 };
	XMFLOAT3 ghostScale = { 0.075, 0.075, 0.075 };
	XMMATRIX banjoMatrix;
	XMMATRIX tempMatrix;
	ShadowMap* shadowMap;
	ShadowMap* shadowMap2;
	ShadowMap* shadowMap3;
	ShadowMap* shadowMap4;
	ShadowMap* shadowMap5;
protected:
	bool render();
	
	void depthPassLight1();
	void depthPassLight2();
	void depthPassLight3();
	void depthPassLight4();
	void depthPassLight5();
	void depthPassLight6();
	void finalPass();
	void gui();

private:
	TextureShader* textureShader;
	ShadowShader* shadowShader;
	DepthShader* depthShader;
	ObjectShader* objectShader;
	PlaneMesh* mesh;
	Light* light;
	Light* light2;
	Light* light3;
	Light* light4;
	Light* light5;
	Light* light6;
	Light* light7;

	ID3D11ShaderResourceView* heightMapSRV;
	ID3D11RenderTargetView* renderTargetView;
	ID3D11ShaderResourceView* renderTargetSRV; //To access the rendered scene in the shaders
	ID3D11Texture2D* renderTargetTexture;

	//AModel* model;
	AModel* Ghost;
	AModel* Ghost2;
	AModel* Ghost3;
	AModel* Ghost4;
	AModel* cube;
	AModel* sphere;
	AModel* sphere2;
	AModel* campfire;

	//float rotationSpeed = 0.001;
	//float moveSpeed = 0.1;
	float radius = 25.0f; //Radius from the centre of the scene
	float speed = -0.002f;  //Adjust speed of rotation
	float offsetX;
	float offsetZ;
	float offsetY;

	float bobAngle = 0.1;
	float bobSpeed = -0.02;
	float bobHeight = 2;

	float angleX;
	float angleY = 0.1;
};

#endif