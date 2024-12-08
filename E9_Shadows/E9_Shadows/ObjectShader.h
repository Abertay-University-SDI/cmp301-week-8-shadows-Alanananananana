
#include "DXF.h"

using namespace std;
using namespace DirectX;


class ObjectShader : public BaseShader
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
		XMMATRIX lightView3;
		XMMATRIX lightProjection3;
		XMMATRIX lightView4;
		XMMATRIX lightProjection4;
		XMMATRIX lightView5;
		XMMATRIX lightProjection5;
		XMMATRIX lightView6;
		XMMATRIX lightProjection6;
	};

	struct LightBufferType
	{
		XMFLOAT4 ambient1;
		XMFLOAT4 diffuse1;
		XMFLOAT3 direction1;
		float padding1;

		XMFLOAT4 ambient2;
		XMFLOAT4 diffuse2;
		XMFLOAT3 position2;
		float padding2;

		XMFLOAT4 diffuse3;
		XMFLOAT3 position3;
		float range3;
		XMFLOAT3 direction3;
		float innerCone3;
		float outerCone3;
		XMFLOAT3 attenuation3;
		//float padding3;

		XMFLOAT4 diffuse4;
		XMFLOAT3 position4;
		float range4;
		XMFLOAT3 direction4;
		float innerCone4;
		float outerCone4;
		XMFLOAT3 attenuation4;
		//float padding4;

		XMFLOAT4 diffuse5;
		XMFLOAT3 position5;
		float range5;
		XMFLOAT3 direction5;
		float innerCone5;
		float outerCone5;
		XMFLOAT3 attenuation5;
		//float padding5;

		XMFLOAT4 diffuse6;
		XMFLOAT3 position6;
		float range6;
		XMFLOAT3 direction6;
		float innerCone6;
		float outerCone6;
		XMFLOAT3 attenuation6;
		//float padding6;
	};

	struct HeightBufferType {
		float heightValues[256][256]; // Example size, based on the height map resolution
	};

	struct CameraBufferType
	{
		XMFLOAT3 cameraPosition;
		float padding;
	};



public:

	ObjectShader(ID3D11Device* device, HWND hwnd);
	~ObjectShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix,
		const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix,
		ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMap1,
		ID3D11ShaderResourceView* depthMap2,
		ID3D11ShaderResourceView* depthMap3,
		ID3D11ShaderResourceView* depthMap4,
		ID3D11ShaderResourceView* depthMap5,
		Light* light1, Light* light2, Light* light3, Light* light4, Light* light5, Light* light6, Camera* camera);

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix,
		const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix,
		ID3D11ShaderResourceView* texture,
		Light* light, Camera* camera);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);



private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;
	ID3D11SamplerState* sampleStateHeight; // Sampler for the height map
	ID3D11Buffer* lightBuffer;
	ID3D11ShaderResourceView* heightMapSRV;
	ID3D11Buffer* cameraBuffer;

	// Add height map
	ID3D11ShaderResourceView* heightMap;

};

//#endif