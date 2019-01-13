#include "AI.h"
#include "Player.h"
#include "Physics.h"


AI::AI(GameObject* player)
{
	m_pPlayer = player;
}

AI::~AI()
{
}

void AI::Update()
{
	if (m_pParent->GetComponent<Physics>()->GetGrounded())
	{
		m_pParent->LookAt(m_pPlayer->GetPosition().x, m_pParent->GetPosition().y, m_pPlayer->GetPosition().z);
		m_pParent->SetVelocity(m_pParent->GetForward().x * m_movementSpeed, m_pParent->GetVelocity().y, m_pParent->GetForward().z * m_movementSpeed);
	}

}
