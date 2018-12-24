#include "Player.h"
#include "Camera.h"

Player::Player()
{
}

Player::Player(std::vector<Entity*>* entityList, float weight, bool simulated, Input* input, Camera* camera) : Physics(entityList, weight, simulated)
{
	m_pInput = input;
	m_pCamera = camera;
}


Player::~Player()
{
}

void Player::Update()
{
	XMVECTOR gravity = XMVectorSet(0, GetVelocity().y, 0, 0);
	XMVECTOR forward = XMVectorSet(m_pCamera->GetForward().x, 0, m_pCamera->GetForward().z, 0);
	
	//move player in relation to camera
	m_velocity = gravity + (forward * (m_pInput->GetMovementAxis().y * 50)) + (m_pCamera->GetRight() * (m_pInput->GetMovementAxis().x * 50));
	
	//rotate the camera
	m_pCamera->Rotate((m_pInput->GetRotationAxis().x * 50)* Time::Instance()->DeltaTime(), 0);

	// if moving set rotation based on movement
	float velocityMagnitude = (pow(m_velocity.x, 2) + pow(m_velocity.z, 2));
	if (velocityMagnitude > 0)
	{
		float yAngle = atan2(((m_position.x + m_velocity.x) - m_position.x), ((m_position.z + m_velocity.z) -m_position.z )) * (180 / XM_PI); 
		XMVECTOR targetRotation = XMVectorSet(0, yAngle, 0, 0);
		m_rotation = XMVectorLerp(m_rotation, targetRotation, Time::Instance()->DeltaTime() * 4);
	}

	//Jump
	if (m_pInput->KeyIsPressed(DIK_SPACE) && GetVelocity().y == 0)
	{
		SetVelocity(GetVelocity().x, 100, GetVelocity().z);
	}

	LateUpdate();
}
