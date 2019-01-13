#pragma once
#include "Component.h"

class Player;

class AI : public Component
{
private:
	GameObject* m_pPlayer;
	float m_movementSpeed = 50;


public:
	AI(GameObject* player);
	~AI();
	void Update();
};

