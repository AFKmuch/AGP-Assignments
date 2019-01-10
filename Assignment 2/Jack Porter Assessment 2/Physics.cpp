#include "Physics.h"
#include "Model.h"
#include "Maths.h"

Physics::Physics()
{
	
}

Physics::Physics(float weight, bool simulated)
{
	m_weight = weight;
	m_simulated = simulated;
}


Physics::~Physics()
{
}

void Physics::Update()
{
	if (CheckCollision())
	{
		m_pParent->SetVelocity(0, 0, 0);
	}
	if (m_pParent->GetPosition().y <= -5 && m_pParent->GetVelocity().y <= 0)
	{
		m_pParent->SetVelocity(m_pParent->GetVelocity().x, 0, m_pParent->GetVelocity().z);
		m_pParent->SetPosition(m_pParent->GetPosition().x, -5, m_pParent->GetPosition().z);
	}
	else
	{
		m_pParent->ChangeVelocity(0, (m_gravity * m_weight) * Time::Instance()->DeltaTime(), 0);
	}
}

float Physics::GetWeight()
{
	return m_weight;
}

bool Physics::GetSimulated()
{
	return m_simulated;
}


void Physics::SetWeight(float weight)
{
	m_weight = weight;
}

void Physics::SetSimulated(bool simulated)
{
	m_simulated = simulated;
}


void Physics::ChangeWeight(float weight)
{
	m_weight += weight;
}


bool Physics::CheckCollision()
{
	GameObject* entity;
	for (int i = 0; i < m_pParent->GetGameObjectList()->size(); i++)
	{
		entity = m_pParent->GetGameObjectList()->at(i);
		if (entity == m_pParent)
		{
			continue;
		}
		if (m_pParent->HasComponent<Model>() && entity->HasComponent<Model>())
		{
			XMVECTOR thisModelPos = m_pParent->GetComponent<Model>()->GetBoundingSphereWorldSpacePosition(m_pParent->GetScale(), (m_pParent->GetPosition() + (m_pParent->GetVelocity() * Time::Instance()->DeltaTime())), m_pParent->GetRotation());
			XMVECTOR thatModelPos = entity->GetComponent<Model>()->GetBoundingSphereWorldSpacePosition(entity->GetScale(),( entity->GetPosition() + (entity->GetVelocity() * Time::Instance()->DeltaTime())), entity->GetRotation());
			XMVECTOR distanceVector = XMVectorSet(thatModelPos.x - thisModelPos.x, thatModelPos.y - thisModelPos.y, thatModelPos.z - thisModelPos.z, thatModelPos.w - thisModelPos.w);
			float distance = (distanceVector.x * distanceVector.x) + (distanceVector.y * distanceVector.y) + (distanceVector.z * distanceVector.z);

			distance = sqrt(distance);
			if (distance > (m_pParent->GetComponent<Model>()->GetBoundingSphereRadius(m_pParent->GetScale()) + entity->GetComponent<Model>()->GetBoundingSphereRadius(entity->GetScale())))
			{
				continue;
			}
			else
			{				
				return true;
			}
		}
	}
	return false;
}

bool Physics::CheckCollisionRay(GameObject* entity)
{
	for (int i = 0; i < entity->GetComponent<Model>()->GetModel()->numverts; i+= 3)
	{
		for (int j = 0; j < m_pParent->GetComponent<Model>()->GetModel()->numverts; j+= 3)
		{
			XMVECTOR eP1 = XMVectorSet(entity->GetComponent<Model>()->GetModel()->vertices[i].Pos.x, entity->GetComponent<Model>()->GetModel()->vertices[i].Pos.y, entity->GetComponent<Model>()->GetModel()->vertices[i].Pos.z, 0);
			XMVECTOR eP2 = XMVectorSet(entity->GetComponent<Model>()->GetModel()->vertices[i+1].Pos.x, entity->GetComponent<Model>()->GetModel()->vertices[i+1].Pos.y, entity->GetComponent<Model>()->GetModel()->vertices[i+1].Pos.z, 0);
			XMVECTOR eP3 = XMVectorSet(entity->GetComponent<Model>()->GetModel()->vertices[i+2].Pos.x, entity->GetComponent<Model>()->GetModel()->vertices[i+2].Pos.y, entity->GetComponent<Model>()->GetModel()->vertices[i+2].Pos.z, 0);
			eP1 += (entity->GetPosition() + (entity->GetVelocity() * Time::Instance()->DeltaTime()));
			eP2 += (entity->GetPosition() + (entity->GetVelocity() * Time::Instance()->DeltaTime()));
			eP3 += (entity->GetPosition() + (entity->GetVelocity() * Time::Instance()->DeltaTime()));

			XMVECTOR tP1 = XMVectorSet(m_pParent->GetComponent<Model>()->GetModel()->vertices[i].Pos.x, m_pParent->GetComponent<Model>()->GetModel()->vertices[i].Pos.y, m_pParent->GetComponent<Model>()->GetModel()->vertices[i].Pos.z, 0);
			XMVECTOR tP2 = XMVectorSet(m_pParent->GetComponent<Model>()->GetModel()->vertices[i + 1].Pos.x, m_pParent->GetComponent<Model>()->GetModel()->vertices[i + 1].Pos.y, m_pParent->GetComponent<Model>()->GetModel()->vertices[i + 1].Pos.z, 0);
			XMVECTOR tP3 = XMVectorSet(m_pParent->GetComponent<Model>()->GetModel()->vertices[i + 2].Pos.x, m_pParent->GetComponent<Model>()->GetModel()->vertices[i + 2].Pos.y, m_pParent->GetComponent<Model>()->GetModel()->vertices[i + 2].Pos.z, 0);
			tP1 += (m_pParent->GetPosition() + (m_pParent->GetVelocity() * Time::Instance()->DeltaTime()));
			tP2 += (m_pParent->GetPosition() + (m_pParent->GetVelocity() * Time::Instance()->DeltaTime()));
			tP3 += (m_pParent->GetPosition() + (m_pParent->GetVelocity() * Time::Instance()->DeltaTime()));

			//float result = Maths::PlaneEquation(Maths::CreatePlane(tP1, tP2, tP3), )
		}
	}
	return false;
}

