#pragma once
#include <vector>
#include "Model.h"
#include "Time.h"
class Entity
{
protected:
	Model* m_pModel;
	std::vector<Entity*>* m_pEntityList;

	XMVECTOR					m_position;
	XMVECTOR					m_rotation;
	XMVECTOR					m_scale;

public:
	
	Entity();
	Entity(std::vector<Entity*>* entityList);
	~Entity();

	virtual void Update();
	void Draw(XMMATRIX* view, XMMATRIX* projection);
	void UpdateLighting(XMVECTOR directionVector, XMVECTOR directionColor, XMVECTOR ambientColor);
	void LookAt(float posX, float posY, float posZ);
	void MoveForward(float distance);


	XMVECTOR GetPosition();
	XMVECTOR GetForward();
	XMVECTOR GetRight();
	XMVECTOR GetRotation();
	XMVECTOR GetScale();

	void SetPosition(float x, float y, float z);
	void SetAngle(float x, float y, float z);
	void SetScale(float scale);
	void SetUpModel(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* modelFileName, char* textureFileName);

	void ChangePosition(float x, float y, float z);
	void ChangeRotation(float x, float y, float z);
	void ChangeScale(float x, float y, float z);

	Model* GetModel();
};

