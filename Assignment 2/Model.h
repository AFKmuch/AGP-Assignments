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
	ID3D11Device *				m_pD3DDevice;
	ID3D11DeviceContext*		m_pImmediateContext;

	ObjFileModel*				m_pObject;
	ID3D11VertexShader*			m_pVShader;
	ID3D11PixelShader*			m_pPShader;
	ID3D11InputLayout*			m_pInputLayout;
	ID3D11Buffer*				m_pConstantBuffer;
	ID3D11ShaderResourceView*	m_pTexture;
	ID3D11SamplerState*			m_pSampler;

	MODEL_CONSTANT_BUFFER		m_model_cb_values;

	XMVECTOR					m_position;
	XMVECTOR					m_rotation;
	XMVECTOR					m_scale;
	XMVECTOR					m_bounding_sphere_centre;
	float						m_bounding_sphere_radius;

public:
	Model();
	Model(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~Model();
	HRESULT LoadObjModel(char* filename);
	HRESULT Draw(XMMATRIX* view, XMMATRIX* projection);
	HRESULT AddTexture(char* filename);

	void LookAt(float posX, float posZ, float posY);
	void MoveForward(float distance);

	void CalculateModelCentrePoint();
	void CalculateBoundingSphereRadius();

	bool CheckCollision(Model* model);
	
	float getPosX();
	float getPosY();
	float getPosZ();
	float getAngleX();
	float getAngleY();
	float getAngleZ();
	float getScale();
	XMVECTOR GetBoundingSphereWorldSpacePosition();
	float GetBoundingSphereRadius();

	void setPosX(float x);
	void setPosY(float y);
	void setPosZ(float z);
	void setAngleX(float x);
	void setAngleY(float y);
	void setAngleZ(float z);
	void setScale(float scale);
	void setDirectionalLight(XMVECTOR direction, XMVECTOR color);
	void setAmbientLight(XMVECTOR color);

	void changePosX(float x);
	void changePosY(float y);
	void changePosZ(float z);
	void changeAngleX(float x);
	void changeAngleY(float y);
	void changeAngleZ(float z);
	void changeScale(float scale);

};

