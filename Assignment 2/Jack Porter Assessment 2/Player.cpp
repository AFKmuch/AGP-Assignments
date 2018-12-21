#include "Player.h"



Player::Player()
{
}

Player::Player(std::vector<Entity*>* entityList, float weight, bool simulated, Input* input) : Physics(entityList, weight, simulated)
{
	m_pInput = input;
}


Player::~Player()
{
}

void Player::Update()
{
	XMVECTOR gravity = XMVectorSet(0, GetVelocity().y, 0, 0);
	m_velocity = gravity + (GetForward() * (m_pInput->GetMovementAxis().y * 50)) + (GetRight() * (m_pInput->GetMovementAxis().x * 50));
	ChangeRotation(0, (m_pInput->GetRotationAxis().x * 50)* Time::Instance()->DeltaTime(), 0);
	if (m_pInput->KeyIsPressed(DIK_SPACE) && GetVelocity().y == 0)
	{
		SetVelocity(GetVelocity().x, 100, GetVelocity().z);
	}
	LateUpdate();
}
