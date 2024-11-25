#ifndef _SHADOWSHADER_H_
#define _SHADOWSHADER_H_

#include "DXF.h"

using namespace std;
using namespace DirectX;


class ShadowShader : public BaseShader
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView;
		XMMATRIX lightProjection;
		XMMATRIX lightView2;
		XMMATRIX lightProjection2;
	};

	struct LightBufferType
	{
		XMFLOAT4 ambient1;
		XMFLOAT4 diffuse1;
		XMFLOAT3 direction1;
		float padding1;

		XMFLOAT4 ambient2;
		XMFLOAT4 diffuse2;
		XMFLOAT3 direction2;
		float padding2;
	};

	struct HeightBufferType {
		float heightValues[256][256]; // Example size, based on the height map resolution
	};



public:

	ShadowShader(ID3D11Device* device, HWND hwnd);
	~ShadowShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix,
		const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix,
		ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMap1,
		ID3D11ShaderResourceView* depthMap2, ID3D11ShaderResourceView* heightMap,
		Light* light1, Light* light2);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

	

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;
	ID3D11SamplerState* sampleStateHeight; // Sampler for the height map
	ID3D11Buffer* lightBuffer;
	ID3D11ShaderResourceView* heightMapSRV;

	// Add height map
	ID3D11ShaderResourceView* heightMap;

};

#endif