#include "Physics.h"
#include "Model.h"
#include "Maths.h"
#include "AI.h"
#include "Player.h"

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
	if (CheckCapsuelCollision())
	{
	}
	if (m_pParent->GetPosition().y <= -5 && m_pParent->GetVelocity().y <= 0)
	{
		m_pParent->SetVelocity(m_pParent->GetVelocity().x, 0, m_pParent->GetVelocity().z);
		m_pParent->SetPosition(m_pParent->GetPosition().x, -5, m_pParent->GetPosition().z);
	}
	else
	{
		if (m_simulated)
		{
			m_pParent->ChangeVelocity(-m_pParent->GetVelocity().x / 2, (m_gravity * m_weight) * Time::Instance()->DeltaTime(), -m_pParent->GetVelocity().z / 2);
			
		}
		else
		{
			m_pParent->ChangeVelocity(-m_pParent->GetVelocity().x / 2, -m_pParent->GetVelocity().y / 2, -m_pParent->GetVelocity().z / 2);
		}

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


bool Physics::CheckSphereCollision()
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

bool Physics::CheckCapsuelCollision()
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
			XMVECTOR thisModelPos = m_pParent->GetComponent<Model>()->GetModelCentre(m_pParent->GetScale()) + m_pParent->GetPosition();
			XMVECTOR thatModelPos = entity->GetComponent<Model>()->GetModelCentre(entity->GetScale()) + entity->GetPosition();

			float thisRadius = m_pParent->GetComponent<Model>()->GetCapsuleRadius(m_pParent->GetScale());
			float thatRadius = entity->GetComponent<Model>()->GetCapsuleRadius(entity->GetScale());

			float thisBaseHeight = abs(m_pParent->GetComponent<Model>()->GetCapsuleHeight(m_pParent->GetScale()));
			float thatBaseHeight = abs(entity->GetComponent<Model>()->GetCapsuleHeight(entity->GetScale()));

			float thisTotalHeight = thisBaseHeight + (thisRadius * 2);
			float thatTotalHeight = thatBaseHeight + (thatRadius * 2);

			float thisFinalHeight = 0;
			float thatFinalHeight = 0;
			float difference = 0;

			if (thisModelPos.y + (thisTotalHeight / 2) >= thatModelPos.y - (thatTotalHeight / 2) && thisModelPos.y - (thisTotalHeight / 2) <= thatModelPos.y + (thatTotalHeight / 2))
			{
				difference = (thatModelPos.y + (thatTotalHeight / 2)) - (thisModelPos.y - (thisTotalHeight / 2));
				thisFinalHeight = thisRadius + (difference / 2);
				thatFinalHeight = thatTotalHeight - (difference / 2);
			}
			else if (thisModelPos.y - (thisTotalHeight / 2 ) >= thatModelPos.y + (thatTotalHeight / 2))
			{
				thisFinalHeight = 0;
				thatFinalHeight = thatBaseHeight / 2;
			}
			else if (thisModelPos.y + (thisTotalHeight / 2) <= thatModelPos.y - (thatTotalHeight / 2))
			{
				thisFinalHeight = thisBaseHeight / 2;
				thatFinalHeight = 0;
			}
			XMVECTOR thisModelCheckPos = XMVectorSet(thisModelPos.x, thisModelPos.y + thisFinalHeight, thisModelPos.z, 0);
			XMVECTOR thatModelCheckPos = XMVectorSet(thatModelPos.x, thatModelPos.y + thatFinalHeight, thatModelPos.z, 0);

			XMVECTOR distanceVector = XMVectorSet(thatModelCheckPos.x - thisModelCheckPos.x, thatModelCheckPos.y - thisModelCheckPos.y, thatModelCheckPos.z - thisModelCheckPos.z, 1);
			float distance = (distanceVector.x * distanceVector.x) + (distanceVector.y * distanceVector.y) + (distanceVector.z * distanceVector.z);

			distance = sqrt(distance);
			if (distance > (thisRadius + thatRadius))
			{
				continue;
			}
			else
			{
				float normalized = sqrt(pow(distanceVector.x, 2) + pow(distanceVector.y, 2) + pow(distanceVector.z, 2));
				XMVECTOR norm = distanceVector * normalized;
				norm /= 10;
				if (m_pParent->HasComponent<AI>() && entity->HasComponent<Player>())
				{
					entity->GetComponent<Player>()->ChangeHealth(-10);
					entity->AddForce(m_pParent->GetVelocity().x * 10, 100, m_pParent->GetVelocity().y * 10, true);
					return true;
				}
				m_pParent->SetVelocity(-norm.x * entity->GetComponent<Physics>()->GetWeight(), -norm.y * entity->GetComponent<Physics>()->GetWeight(), -norm.z * entity->GetComponent<Physics>()->GetWeight());
				entity->SetVelocity(-norm.x * m_pParent->GetComponent<Physics>()->GetWeight(), -norm.y * m_pParent->GetComponent<Physics>()->GetWeight(), -norm.z * m_pParent->GetComponent<Physics>()->GetWeight());
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

