#pragma once
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>
#include "Time.h"
#include "GameObject.h"

class GameObject;

class Component
{
protected:
	GameObject * m_pParent;

public:
	Component();
	~Component();
	void virtual Update(XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection);
	void SetParent(GameObject* parent);
	GameObject* GetParent();
};

