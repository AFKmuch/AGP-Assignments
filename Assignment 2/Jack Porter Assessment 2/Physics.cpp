#include "Physics.h"



void Physics::LateUpdate()
{
	ChangeVelocity(0, (m_gravity * m_weight) * Time::Instance()->DeltaTime(), 0);
	
	ChangePosition(m_velocity.x* Time::Instance()->DeltaTime(), m_velocity.y* Time::Instance()->DeltaTime(), m_velocity.z* Time::Instance()->DeltaTime());
}

Physics::Physics()
{
}

Physics::Physics(std::vector<Entity*>* entityList, float weight, bool simulated) : Entity(entityList)
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
		SetVelocity(-GetVelocity().x, -GetVelocity().y, -GetVelocity().z);
	}
	if (GetPosition().y < -5)
	{
		SetVelocity(NULL, 10, NULL);
	}
	ChangeRotation(NULL, 0.01f, NULL);

	LateUpdate();
}

float Physics::GetWeight()
{
	return m_weight;
}

bool Physics::GetSimulated()
{
	return m_simulated;
}

XMVECTOR Physics::GetVelocity()
{
	return m_velocity;
}

void Physics::SetWeight(float weight)
{
	m_weight = weight;
}

void Physics::SetSimulated(bool simulated)
{
	m_simulated = simulated;
}

void Physics::SetVelocity(float x, float y, float z)
{
	float newX, newY, newZ;
	if (x == NULL)
	{
		newX = m_velocity.x;
	}
	else
	{
		newX = x;
	}

	if (y == NULL)
	{
		newY = m_velocity.y;
	}
	else
	{
		newY = y;
	}

	if (z == NULL)
	{
		newZ = m_velocity.z;
	}
	else
	{
		newZ = z;
	}

	m_velocity = XMVectorSet(newX, newY, newZ, 0);
}

void Physics::ChangeWeight(float weight)
{
	m_weight += weight;
}

void Physics::ChangeVelocity(float x, float y, float z)
{
	m_velocity = XMVectorSet(m_velocity.x + x, m_velocity.y + y, m_velocity.z + z, 0);
}

bool Physics::CheckCollision()
{
	Entity* entity;
	for (int i = 0; i < m_pEntityList->size(); i++)
	{
		entity = m_pEntityList->at(i);
		if (entity == this)
		{
			return false;
		}
		XMVECTOR thisModelPos = m_pModel->GetBoundingSphereWorldSpacePosition(m_scale, (m_position + m_velocity), m_rotation);
		XMVECTOR thatModelPos = entity->GetModel()->GetBoundingSphereWorldSpacePosition(entity->GetScale(), entity->GetPosition(), entity->GetRotation());
		XMVECTOR distanceVector = XMVectorSet(thatModelPos.x - thisModelPos.x, thatModelPos.y - thisModelPos.y, thatModelPos.z - thisModelPos.z, thatModelPos.w - thisModelPos.w);
		float distance = (distanceVector.x * distanceVector.x) + (distanceVector.y * distanceVector.y) + (distanceVector.z * distanceVector.z);

		distance = sqrt(distance);
		if (distance > (m_pModel->GetBoundingSphereRadius(m_scale) + entity->GetModel()->GetBoundingSphereRadius(entity->GetScale())))
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	return false;
}