#pragma once
#include "Physics.h"
#include "Input.h"

class Player : public Physics
{
private:
	Input * m_pInput;
public:
	Player();
	Player(std::vector<Entity*>* entityList, float weight, bool simulated, Input* input);
	~Player();
	void Update();
};

