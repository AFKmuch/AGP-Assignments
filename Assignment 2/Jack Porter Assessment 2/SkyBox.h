#pragma once
#include "Camera.h"

struct SKYBOX_CONSTANT_BUFFER
{
	XMMATRIX WorldViewProjection; // 64 Bytes
}; // 64 bytes

struct POS_TEX_NORM_VERTEX//This will be added to and renamed in future tutorials
{
	XMFLOAT3	pos;
	XMFLOAT3	Texture;
	XMFLOAT3	Normal;
};

class SkyBox
{
private:
	ID3D11RasterizerState *		m_pRasterSolid = 0;
	ID3D11RasterizerState *		m_pRasterSkyBox = 0;
	ID3D11DepthStencilState*	m_pDepthWriteSolid = 0;
	ID3D11DepthStencilState*	m_pDepthWriteSkyBox = 0;
	D3D11_RASTERIZER_DESC		m_rasterizerDesc;

	ID3D11Device*				m_pD3DDevice;
	ID3D11DeviceContext*		m_pImmediateContext;

	ID3D11Buffer*				m_pVertexBuffer;
	ID3D11VertexShader*			m_pVShader;
	ID3D11PixelShader*			m_pPShader;
	ID3D11InputLayout*			m_pInputLayout;
	ID3D11Buffer*				m_pConstantBuffer;
	ID3D11ShaderResourceView*	m_pTexture;
	ID3D11SamplerState*			m_pSampler;

	SKYBOX_CONSTANT_BUFFER		m_SkyBoxBufferValues;

	Camera *					m_pCamera;

	XMVECTOR					m_position;


	const POS_TEX_NORM_VERTEX vertices[36] =
	{
		// back face 
	{ XMFLOAT3(-3.0f,  3.0f, 3.0f),		XMFLOAT3(-1.0f,  1.0f,  1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
	{ XMFLOAT3(-3.0f, -3.0f, 3.0f),		XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
	{ XMFLOAT3(3.0f,  3.0f, 3.0f),		XMFLOAT3( 1.0f,  1.0f,  1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
	{ XMFLOAT3(3.0f,  3.0f, 3.0f),		XMFLOAT3( 1.0f,  1.0f,  1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
	{ XMFLOAT3(-3.0f, -3.0f, 3.0f),		XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
	{ XMFLOAT3(3.0f, -3.0f, 3.0f),		XMFLOAT3( 1.0f, -1.0f,  1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
								
	// front face				
	{ XMFLOAT3(-3.0f, -3.0f, -3.0f),	XMFLOAT3(-1.0f, -1.0f, -1.0), XMFLOAT3(0.0f, 0.0f, -1.0f) },
	{ XMFLOAT3(-3.0f,  3.0f, -3.0f),	XMFLOAT3(-1.0f,  1.0f, -1.0), XMFLOAT3(0.0f, 0.0f, -1.0f) },
	{ XMFLOAT3(3.0f,  3.0f, -3.0f),		XMFLOAT3( 1.0f,  1.0f, -1.0), XMFLOAT3(0.0f, 0.0f, -1.0f) },
	{ XMFLOAT3(-3.0f, -3.0f, -3.0f),	XMFLOAT3(-1.0f, -1.0f, -1.0), XMFLOAT3(0.0f, 0.0f, -1.0f) },
	{ XMFLOAT3(3.0f,  3.0f, -3.0f),		XMFLOAT3( 1.0f,  1.0f, -1.0), XMFLOAT3(0.0f, 0.0f, -1.0f) },
	{ XMFLOAT3(3.0f, -3.0f, -3.0f),		XMFLOAT3( 1.0f, -1.0f, -1.0), XMFLOAT3(0.0f, 0.0f, -1.0f) },
								
	// left face				
	{ XMFLOAT3(-3.0f, -3.0f, -3.0f),	XMFLOAT3(-1.0f, -1.0f, -1.0), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
	{ XMFLOAT3(-3.0f, -3.0f,  3.0f),	XMFLOAT3(-1.0f, -1.0f,  1.0), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
	{ XMFLOAT3(-3.0f,  3.0f, -3.0f),	XMFLOAT3(-1.0f,  1.0f, -1.0), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
	{ XMFLOAT3(-3.0f, -3.0f,  3.0f),	XMFLOAT3(-1.0f, -1.0f,  1.0), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
	{ XMFLOAT3(-3.0f,  3.0f,  3.0f),	XMFLOAT3(-1.0f,  1.0f,  1.0), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
	{ XMFLOAT3(-3.0f,  3.0f, -3.0f),	XMFLOAT3(-1.0f,  1.0f, -1.0), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
										
	// right face						
	{ XMFLOAT3(3.0f, -3.0f,  3.0f),		XMFLOAT3( 1.0f, -1.0f,  1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
	{ XMFLOAT3(3.0f, -3.0f, -3.0f),		XMFLOAT3( 1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
	{ XMFLOAT3(3.0f,  3.0f, -3.0f),		XMFLOAT3( 1.0f,  1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
	{ XMFLOAT3(3.0f,  3.0f,  3.0f),		XMFLOAT3( 1.0f,  1.0f,  1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
	{ XMFLOAT3(3.0f, -3.0f,  3.0f),		XMFLOAT3( 1.0f, -1.0f,  1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
	{ XMFLOAT3(3.0f,  3.0f, -3.0f),		XMFLOAT3( 1.0f,  1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
												
	// bottom face							   	
	{ XMFLOAT3(3.0f, -3.0f, -3.0f),		XMFLOAT3( 1.0f, -1.0f, -1.0), XMFLOAT3(0.0f, -1.0f, 0.0f) },
	{ XMFLOAT3(3.0f, -3.0f,  3.0f),		XMFLOAT3( 1.0f, -1.0f,  1.0), XMFLOAT3(0.0f, -1.0f, 0.0f) },
	{ XMFLOAT3(-3.0f, -3.0f, -3.0f),	XMFLOAT3(-1.0f, -1.0f, -1.0), XMFLOAT3(0.0f, -1.0f, 0.0f) },
	{ XMFLOAT3(3.0f, -3.0f,  3.0f),		XMFLOAT3( 1.0f, -1.0f,  1.0), XMFLOAT3(0.0f, -1.0f, 0.0f) },
	{ XMFLOAT3(-3.0f, -3.0f,  3.0f),	XMFLOAT3(-1.0f, -1.0f,  1.0), XMFLOAT3(0.0f, -1.0f, 0.0f) },
	{ XMFLOAT3(-3.0f, -3.0f, -3.0f),	XMFLOAT3(-1.0f, -1.0f, -1.0), XMFLOAT3(0.0f, -1.0f, 0.0f) },
											
	// top face								
	{ XMFLOAT3(3.0f, 3.0f,  3.0f),		XMFLOAT3( 1.0f,  1.0f,  1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
	{ XMFLOAT3(3.0f, 3.0f, -3.0f),		XMFLOAT3( 1.0f,  1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
	{ XMFLOAT3(-3.0f, 3.0f, -3.0f),		XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
	{ XMFLOAT3(-3.0f, 3.0f,  3.0f),		XMFLOAT3(-1.0f,  1.0f,  1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
	{ XMFLOAT3(3.0f, 3.0f,  3.0f),		XMFLOAT3( 1.0f,  1.0f,  1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
	{ XMFLOAT3(-3.0f, 3.0f, -3.0f),		XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) }
	};

public:
	SkyBox(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Camera* cameraPtr);
	~SkyBox();
	HRESULT SetUp(char* textureFileName);
	void Draw(XMMATRIX* view, XMMATRIX* projection);
	void SetPosition(XMVECTOR position);
};

