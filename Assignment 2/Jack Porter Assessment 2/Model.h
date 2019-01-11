#pragma once
#include "ModelManager.h"
#include "Component.h"


struct MODEL_CONSTANT_BUFFER
{
	XMMATRIX WorldViewProjection; // 64 Bytes
	XMVECTOR directional_light_vector; // 16
	XMVECTOR directional_light_colour; // 16
	XMVECTOR ambient_light_colour; // 16
}; // 112 bytes

class Model : public Component
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

	MODEL_CONSTANT_BUFFER		m_modelCbValues;

	XMVECTOR					m_boundingSphereCentre;
	float						m_boundingSphereRadius;
	float						m_boundingHeight;
	float						m_boundingWidth;

	XMVECTOR					m_directionalLightVector; 
	XMVECTOR					m_directionalLightColour; 
	XMVECTOR					m_ambientLightColour; 

public:
	Model();
	Model(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMVECTOR directionVector, XMVECTOR directionColor, XMVECTOR ambientColor);
	~Model();
	HRESULT LoadObjModel(char* filename);
	HRESULT LoadShader(char* filename);
	HRESULT Draw(XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection);
	HRESULT AddTexture(char* filename);
	void Update(XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection);
	void CalculateModelCentrePoint();
	void CalculateBoundingSphereRadius();

	XMVECTOR GetBoundingSphereWorldSpacePosition(XMVECTOR scale, XMVECTOR position, XMVECTOR rotation);
	float GetBoundingSphereRadius(XMVECTOR scale);
	float GetCapsuleHeight(XMVECTOR scale);
	float GetCapsuleRadius(XMVECTOR scale);
	XMVECTOR GetModelCentre(XMVECTOR scale);

	void setDirectionalLight(XMVECTOR direction, XMVECTOR color);
	void setAmbientLight(XMVECTOR color);

	void SetUpModel(char * modelFileName, char * textureFileName);

	ObjFileModel* GetModel();
};

