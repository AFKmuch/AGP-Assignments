#include "AI.h"
#include "Player.h"



AI::AI(GameObject* player)
{
	m_pPlayer = player;
}

AI::~AI()
{
}

void AI::Update()
{
	m_pParent->LookAt(m_pPlayer->GetPosition().x, m_pParent->GetPosition().y, m_pPlayer->GetPosition().z);
	m_pParent->SetVelocity(m_pParent->GetForward().x * m_movementSpeed, 0, m_pParent->GetForward().z * m_movementSpeed);
}
