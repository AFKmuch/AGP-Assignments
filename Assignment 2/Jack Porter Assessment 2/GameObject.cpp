#include "GameObject.h"
#include "Component.h"
#include "Player.h"
#include "Physics.h"
#include "Model.h"
#include "AI.h"
#include <iostream>


GameObject::GameObject()
{
	m_position = XMVectorSet(0, 0, 0, 0);
	m_rotation = XMVectorSet(0, 0, 0, 0);
	m_scale = XMVectorSet(1, 1, 1, 0);
}

GameObject::GameObject(std::vector<GameObject*>* gameObjectList, std::vector<GameObject*>* environmentList)
{
	m_pGameObjectList = gameObjectList;
	m_pEnvironmentList = environmentList;
	m_scale = XMVectorSet(1, 1, 1, 0);
}


GameObject::~GameObject()
{
	if (m_components.size() > 0)
	{
		for (int i = m_components.size() - 1; i >= 0; i--)
		{
			if (m_components.end()->second)
			{
				if (m_components.end()->second) delete m_components.end()->second;			
			}
		}
	}
	if (m_children.size() > 0)
	{
		for (int i = m_children.size() - 1; i >= 0; i--)
		{
			if (m_children[i]) delete m_children[i];
		}
	}
	if (m_pGameObjectList->size() > 0)
	{
		for (int i = m_pGameObjectList->size() - 1; i >= 0; i--)
		{
			if (m_pGameObjectList->at(i)) delete m_pGameObjectList->at(i);
		}
	}
	if (m_pEnvironmentList->size() > 0)
	{
		for (int i = m_pEnvironmentList->size() - 1; i >= 0; i--)
		{
			if (m_pEnvironmentList->at(i)) delete m_pEnvironmentList->at(i);
		}
	}
	if (m_pGameObjectList) delete m_pGameObjectList;
	if (m_pEnvironmentList) delete m_pEnvironmentList;
}

void GameObject::Update(XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection)
{
	// the local_world matrix will be used to calc the local transformations for this node
	XMMATRIX local_world = XMMatrixIdentity();

	local_world = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	local_world *= XMMatrixRotationX(XMConvertToRadians(m_rotation.x));
	local_world *= XMMatrixRotationY(XMConvertToRadians(m_rotation.y));
	local_world *= XMMatrixRotationZ(XMConvertToRadians(m_rotation.z));
	local_world *= XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

	// the local matrix is multiplied by the passed in world matrix that contains the concatenated
	// transformations of all parent nodes so that this nodes transformations are relative to those
	local_world *= *world;

	if (HasComponent<Player>())
	{
		GetComponent<Player>()->Update();
	}

	if (HasComponent<AI>())
	{
		GetComponent<AI>()->Update();
	}

	if (HasComponent<Physics>())
	{
		GetComponent<Physics>()->Update();
	}
	
	ChangePosition(GetVelocity().x * Time::Instance()->DeltaTime(), GetVelocity().y* Time::Instance()->DeltaTime(), GetVelocity().z* Time::Instance()->DeltaTime()); // update movement
	ChangePosition(m_forces.x* Time::Instance()->DeltaTime(), m_forces.y* Time::Instance()->DeltaTime(), m_forces.z* Time::Instance()->DeltaTime());
	m_forces = XMVectorSet(m_forces.x / 1.005 , m_forces.y /1.005, m_forces.z /1.005 , 1);

	if (HasComponent<Model>())
	{
		GetComponent<Model>()->Update(&local_world, view, projection);
	}

	for (int i = 0; i < m_children.size(); i++)
	{
		m_children[i]->Update(&local_world, view, projection); // updates all children
	}
	
}

void GameObject::LookAt(float posX, float posY, float posZ)
{
	float dx = posX - m_position.x;
	float dy = posY - m_position.y;
	float dz = posZ - m_position.z;

	float xz = sqrt((m_position.x * m_position.x) + (m_position.z * m_position.z));

	float yAngle = (atan2(dx, dz) * (180 / XM_PI));
	float xAngle = (-atan2(dy, xz) * (180 / XM_PI));

	if (xAngle > 89)
	{
		xAngle = 89;
	}
	else if (xAngle < -89)
	{
		xAngle = -89;
	}

	m_rotation = XMVectorSet(xAngle, yAngle, 0, 0);
}

void GameObject::MoveForward(float distance)
{
	float posX = sin(m_rotation.y * (XM_PI / 180)) * distance;
	float posY = -sin(m_rotation.x * (XM_PI / 180.0)) * distance;
	float posZ = cos(m_rotation.y * (XM_PI / 180)) * distance;
	m_position = XMVectorSet(m_position.x + posX, m_position.y + posY, m_position.z + posZ, 0);
}



XMVECTOR GameObject::GetPosition()
{
	return m_position;
}

XMVECTOR GameObject::GetForward()
{
	float x = sin(m_rotation.y * (XM_PI / 180));
	float y = -sin(m_rotation.x * (XM_PI / 180.0));
	float z = cos(m_rotation.y * (XM_PI / 180));
	return XMVectorSet(x, y, z, 0);
}

XMVECTOR GameObject::GetRight()
{
	return XMVector3Cross(XMVectorSet(0,1,0,0),GetForward());
}

XMVECTOR GameObject::GetRotation()
{
	return m_rotation;
}



XMVECTOR GameObject::GetScale()
{
	return m_scale;
}

XMVECTOR GameObject::GetVelocity()
{
	return m_velocity;
}

XMVECTOR GameObject::GetForces()
{
	return m_forces;
}

std::vector<GameObject*>* GameObject::GetGameObjectList()
{
	return m_pGameObjectList;
}

std::vector<GameObject*>* GameObject::GetEnvironmentList()
{
	return  m_pEnvironmentList;
}

void GameObject::SetPosition(float x, float y, float z)
{

	m_position = XMVectorSet(x, y, z, 0);
}

void GameObject::SetRotation(float x, float y, float z)
{
	m_rotation = XMVectorSet(x, y, z, 0);
}

void GameObject::SetScale(float scale)
{
	m_scale = XMVectorSet(scale, scale, scale, 0);
}

void GameObject::SetVelocity(float x, float y, float z)
{
	m_velocity = XMVectorSet(x, y, z, 1);
}

void GameObject::SetUpModel(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char * modelFileName, char * textureFileName)
{

}

void GameObject::ChangePosition(float x, float y, float z)
{
	m_position = XMVectorSet(m_position.x + x, m_position.y + y, m_position.z + z, 0);
}

void GameObject::ChangeRotation(float x, float y, float z)
{
	m_rotation = XMVectorSet(m_rotation.x + x, m_rotation.y + y, m_rotation.z + z, 0);
}

void GameObject::ChangeScale(float x, float y, float z)
{
	m_scale = XMVectorSet(m_scale.x + x, m_scale.y + y, m_scale.z + z, 0);
}

void GameObject::ChangeVelocity(float x, float y, float z)
{
	m_velocity = XMVectorSet(m_velocity.x + x, m_velocity.y + y, m_velocity.z + z, 1);
}

void GameObject::AddForce(float x, float y, float z, bool Impulse)
{
	if (Impulse)
	{
		m_forces += XMVectorSet(x, y, z, 1);
	}
	else
	{
		m_forces += XMVectorSet(x * Time::Instance()->DeltaTime(), y * Time::Instance()->DeltaTime(), z * Time::Instance()->DeltaTime(), 1);
	}
}


void GameObject::AddChild(GameObject * child)
{
	m_children.push_back(child);
}

bool GameObject::DetatchChild(GameObject * child)
{
	// traverse tree to find node to detatch
	for (int i = 0; i < m_children.size(); i++)
	{
		if (child == m_children[i])
		{
			m_children.erase(m_children.begin() + i);
			return true;
		}
		if (m_children[i]->DetatchChild(child) == true) return true;
	}
	return false; // node not in this tree
}

void GameObject::AddComponent(Component * component)
{
	m_components[&typeid(*component)] = component;
	component->SetParent(this);
}

