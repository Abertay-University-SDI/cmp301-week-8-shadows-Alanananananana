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
	XMFLOAT3 ghostPos = { 0, 7, 80 };
	XMFLOAT3 ghostPos2 = { 30, 7, 55 };
	XMFLOAT3 ghostPos3 = { 0, 7, 25 };
	XMFLOAT3 ghostPos4 = { -30, 7, 55 };
	XMFLOAT3 campfirePos = { 0, 7, 55 };
	XMFLOAT3 orbitCentre = { 0.0f, 7, 55.0f };
	XMMATRIX banjoMatrix;
	XMMATRIX tempMatrix;
	ShadowMap* shadowMap;
	ShadowMap* shadowMap2;
protected:
	bool render();
	void depthPassLight1();
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
	ID3D11ShaderResourceView* heightMapSRV;

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
	float bobSpeed = -0.05;
	float bobHeight = 2;

	float angleX;
	float angleY = 0.1;
};

#endif