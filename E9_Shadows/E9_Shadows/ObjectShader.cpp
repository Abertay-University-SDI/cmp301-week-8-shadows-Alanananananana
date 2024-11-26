#include "ObjectShader.h"


ObjectShader::ObjectShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"object_vs.cso", L"object_ps.cso");
}


ObjectShader::~ObjectShader()
{
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}
	if (layout)
	{
		layout->Release();
		layout = 0;
	}
	if (lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}


void ObjectShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC heightBufferDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

	// Sampler for shadow map sampling.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	renderer->CreateSamplerState(&samplerDesc, &sampleStateShadow);

	// Setup light buffer
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);


	////D3D11_BUFFER_DESC heightBufferDesc;
	//heightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	//heightBufferDesc.ByteWidth = sizeof(HeightBufferType);
	//heightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//heightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//heightBufferDesc.MiscFlags = 0;
	//heightBufferDesc.StructureByteStride = 0;
	//renderer->CreateBuffer(&heightBufferDesc, NULL, &heightBuffer);
}


void ObjectShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix,
	const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix,
	ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMap1,
	ID3D11ShaderResourceView* depthMap2,
	Light* light1, Light* light2)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Transpose the matrices for the shader
	XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tview = XMMatrixTranspose(viewMatrix);
	XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);
	XMMATRIX tLightView1 = XMMatrixTranspose(light1->getViewMatrix());
	XMMATRIX tLightProj1 = XMMatrixTranspose(light1->getOrthoMatrix());
	XMMATRIX tLightView2 = XMMatrixTranspose(light2->getViewMatrix());
	XMMATRIX tLightProj2 = XMMatrixTranspose(light2->getOrthoMatrix());

	// Map the matrix buffer
	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	dataPtr->lightView = tLightView1;
	dataPtr->lightProjection = tLightProj1;
	dataPtr->lightView2 = tLightView2;
	dataPtr->lightProjection2 = tLightProj2;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	// Map the light buffer
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	LightBufferType* lightPtr = (LightBufferType*)mappedResource.pData;
	lightPtr->ambient1 = light1->getAmbientColour();
	lightPtr->diffuse1 = light1->getDiffuseColour();
	lightPtr->direction1 = light1->getDirection();
	lightPtr->ambient2 = light2->getAmbientColour();
	lightPtr->diffuse2 = light2->getDiffuseColour();
	lightPtr->direction2 = light2->getDirection();
	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);

	// Bind resources
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->VSSetShaderResources(2, 1, &heightMap);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->VSSetSamplers(2, 1, &sampleStateHeight);

	// Bind shadow maps
	ID3D11ShaderResourceView* shadowMaps[2] = { depthMap1, depthMap2 };
	deviceContext->PSSetShaderResources(1, 2, shadowMaps);
	deviceContext->PSSetSamplers(1, 1, &sampleStateShadow);
}


