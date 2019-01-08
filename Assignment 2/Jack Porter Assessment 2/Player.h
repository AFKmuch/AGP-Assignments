#pragma once
#include "Component.h"

class Camera;
class Input;

class Player : 
	public Component
{

private:
	Input * m_pInput;
	Camera* m_pCamera;
	float m_playerMovementSpeed = 50;
	float m_cameraRotationSensitivity = 50;
	float m_RotationSpeed = 10;

public:
	Player(Input* input, Camera* camera);
	~Player();
	void Update();
};

