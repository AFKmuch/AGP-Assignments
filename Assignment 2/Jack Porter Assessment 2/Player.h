#pragma once
#include "Physics.h"
#include "Input.h"

class Camera;

class Player : public Physics
{

private:
	Input * m_pInput;
	Camera* m_pCamera;

public:
	Player();
	Player(std::vector<Entity*>* entityList, float weight, bool simulated, Input* input, Camera* camera);
	~Player();
	void Update();
};

