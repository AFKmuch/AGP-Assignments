#pragma once
#include "Component.h"

class Player;

class AI : public Component
{
private:
	GameObject* m_pPlayer;
	float m_movementSpeed = 15;


public:
	AI(GameObject* player);
	~AI();
	void Update();
};

