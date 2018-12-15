#pragma once
#include "Entity.h"

class Physics : public Entity
{
private:
	const float m_gravity = -0.00981;
protected:
	float m_weight;
	bool m_simulated;
	XMVECTOR m_velocity;

	void LateUpdate();
public:
	Physics();
	Physics(std::vector<Entity*>* entityList, float weight, bool simulated);
	~Physics();

	void Update();

	float GetWeight();
	bool GetSimulated();
	XMVECTOR GetVelocity();

	void SetWeight(float weight);
	void SetSimulated(bool simulated);
	void SetVelocity(float x, float y, float z);

	void ChangeWeight(float weight);
	void ChangeVelocity(float x, float y, float z);
};

