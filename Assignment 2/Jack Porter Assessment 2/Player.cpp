#include "Player.h"
#include "Camera.h"
#include "Input.h"

Player::Player(Input* input, Camera* camera)
{
	m_pInput = input;
	m_pCamera = camera;
}


Player::~Player()
{
}

void Player::Update()
{
	XMVECTOR gravity = XMVectorSet(0, m_pParent->GetVelocity().y, 0, 0);
	XMVECTOR forward = XMVectorSet(m_pCamera->GetForward().x, 0, m_pCamera->GetForward().z, 0);
	
	//move player in relation to camera
	XMVECTOR velocity = gravity + (forward * (m_pInput->GetMovementAxis().y * m_playerMovementSpeed)) + (m_pCamera->GetRight() * (m_pInput->GetMovementAxis().x * m_playerMovementSpeed));
	m_pParent->SetVelocity(velocity.x, velocity.y, velocity.z);
	//rotate the camera
	m_pCamera->Rotate((m_pInput->GetRotationAxis().x * m_cameraRotationSensitivity)* Time::Instance()->DeltaTime(), 0);

	// if moving set rotation based on movement
	float velocityMagnitude = (pow(m_pParent->GetVelocity().x, 2) + pow(m_pParent->GetVelocity().z, 2));
	if (velocityMagnitude > 0)
	{
		float yAngle = atan2(((m_pParent->GetPosition().x + m_pParent->GetVelocity().x) - m_pParent->GetPosition().x), ((m_pParent->GetPosition().z + m_pParent->GetVelocity().z) - m_pParent->GetPosition().z )) * (180 / XM_PI);
		if (yAngle < m_pParent->GetRotation().y - 180)
		{
			yAngle += 360; //stops player rotating 270 degrees instead of 90
		}
		else if (yAngle > m_pParent->GetRotation().y + 180)
		{
			yAngle -= 360; //stops player rotating 270 degrees instead of 90
		}
		XMVECTOR targetRotation = XMVectorSet(0, yAngle, 0, 0);
		XMVECTOR rotation = XMVectorLerp(m_pParent->GetRotation(), targetRotation, Time::Instance()->DeltaTime() * m_RotationSpeed);
		if (rotation.y > 360)
		{
			rotation.y -= 360; // if rotation goes above 360, reduce it to stop overflow
		}
		else if (rotation.y < -360)
		{
			rotation.y += 360; // if rotation goes below -360, increase it to stop overflow
		}
		m_pParent->SetRotation(rotation.x, rotation.y, rotation.z);
	}
	//Jump
	if (m_pInput->KeyIsPressed(DIK_SPACE) && m_pParent->GetVelocity().y == 0)
	{
		m_pParent->SetVelocity(m_pParent->GetVelocity().x, 100, m_pParent->GetVelocity().z);
	}
}
