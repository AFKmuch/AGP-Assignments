#pragma once
#include "objfilemodel.h"

struct MODEL_CONSTANT_BUFFER
{
	XMMATRIX WorldViewProjection; // 64 Bytes
	XMVECTOR directional_light_vector; // 16
	XMVECTOR directional_light_colour; // 16
	XMVECTOR ambient_light_colour; // 16
}; // 112 bytes

class Model
{
private:
	ID3D11Device * m_pD3DDevice;
	ID3D11DeviceContext*		m_pImmediateContext;

	ObjFileModel*				m_pObject;
	ID3D11VertexShader*			m_pVShader;
	ID3D11PixelShader*			m_pPShader;
	ID3D11InputLayout*			m_pInputLayout;
	ID3D11Buffer*				m_pConstantBuffer;
	ID3D11ShaderResourceView*	m_pTexture;
	ID3D11SamplerState*			m_pSampler;

	MODEL_CONSTANT_BUFFER		m_model_cb_values;

	XMVECTOR					m_bounding_sphere_centre;
	float						m_bounding_sphere_radius;

public:
	Model();
	Model(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~Model();
	HRESULT LoadObjModel(char* filename);
	HRESULT LoadShader(char* filename);
	HRESULT Draw(XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection);
	HRESULT AddTexture(char* filename);

	void CalculateModelCentrePoint();
	void CalculateBoundingSphereRadius();

	XMVECTOR GetBoundingSphereWorldSpacePosition(XMVECTOR scale, XMVECTOR position, XMVECTOR rotation);
	float GetBoundingSphereRadius(XMVECTOR scale);

	void setDirectionalLight(XMVECTOR direction, XMVECTOR color);
	void setAmbientLight(XMVECTOR color);

};

