#include "Component.h"


Component::Component()
{

}

Component::~Component()
{
}

void Component::Update(XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection)
{
}

void Component::SetParent(GameObject * parent)
{
	m_pParent = parent;
}

GameObject * Component::GetParent()
{
	return m_pParent;
}


