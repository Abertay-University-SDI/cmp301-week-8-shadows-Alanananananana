// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	// include dxframework
#include "TextureShader.h"
#include "ShadowShader.h"
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
	XMFLOAT3 ghostPos = { 0, 5, 50 };
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
	PlaneMesh* mesh;
	Light* light2;
	Light* light;
	

	//AModel* model;
	AModel* Ghost;
	AModel* cube;
	AModel* sphere;
	AModel* sphere2;
	

	
};

#endif