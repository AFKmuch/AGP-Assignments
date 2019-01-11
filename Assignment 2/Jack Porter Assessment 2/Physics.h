#pragma once
#include "Component.h"

class Physics : public Component
{
private:

	const float m_gravity = -9.81;
	float m_weight;
	bool m_simulated;

public:
	Physics();
	Physics(float weight, bool simulated);
	~Physics();

	void Update();
	bool CheckSphereCollision();
	bool CheckCapsuelCollision();
	bool CheckCollisionRay(GameObject* entity);

	float GetWeight();
	bool GetSimulated();

	void SetWeight(float weight);
	void SetSimulated(bool simulated);

	void ChangeWeight(float weight);
};

