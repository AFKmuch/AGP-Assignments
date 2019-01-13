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
	float m_playerMovementSpeed = 100;
	float m_playerJumpHeight = 50;
	float m_cameraRotationSensitivity = 50;
	float m_RotationSpeed = 10;
	int	m_frameNumber = 0;
	int	m_maxFrameNumber;
	float m_frameUpdateCount = 0.25;
	float m_currentFrameCount = 0;
	float m_maxHealth = 100;
	float m_health;
	std::vector<char*> m_idleAnim;

public:
	Player(Input* input, Camera* camera);
	~Player();
	void Update();
	void ChangeHealth(float change);
	float GetHealth();
	void LoadAnims();
};


