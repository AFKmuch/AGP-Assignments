#pragma once
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>
#include <vector>
#include <map>
#include <type_traits>
#include <memory>
#include "text2D.h"

class Component;

class GameObject
{
private:



	std::map<const std::type_info*, Component*> m_components;

	std::vector<GameObject*> m_children;

	std::vector<GameObject*>* m_pGameObjectList;
	std::vector<GameObject*>* m_pEnvironmentList;

	XMVECTOR					m_position;
	XMVECTOR					m_rotation;
	XMVECTOR					m_scale;
	XMVECTOR					m_velocity;
	XMVECTOR					m_forces;

public:

	GameObject();
	GameObject(std::vector<GameObject*>* gameObjectList, std::vector<GameObject*>* environmentList);
	~GameObject();

	void Update(XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection);
	void LookAt(float posX, float posY, float posZ);
	void MoveForward(float distance);


	XMVECTOR GetPosition();
	XMVECTOR GetForward();
	XMVECTOR GetRight();
	XMVECTOR GetRotation();
	XMVECTOR GetScale();
	XMVECTOR GetVelocity();
	XMVECTOR GetForces();
	std::vector<GameObject*>* GetGameObjectList();
	std::vector<GameObject*>* GetEnvironmentList();

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetScale(float scale);
	void SetVelocity(float x, float y, float z);
	void SetUpModel(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* modelFileName, char* textureFileName);

	void ChangePosition(float x, float y, float z);
	void ChangeRotation(float x, float y, float z);
	void ChangeScale(float x, float y, float z);
	void ChangeVelocity(float x, float y, float z);

	void AddForce(float x, float y, float z, bool Impulse);
	

	void AddChild(GameObject* child);
	bool DetatchChild(GameObject* child);

	void AddComponent(Component* component);

	template <typename t>
	t* GetComponent()
	{
		if (m_components.count(&typeid(t)) != 0)
		{
			return static_cast<t*>(m_components[&typeid(t)]);
		}
		else
		{
			return nullptr;
		}
	}

	template <typename t>
	bool HasComponent()
	{
		if (m_components.count(&typeid(t)) != 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};

