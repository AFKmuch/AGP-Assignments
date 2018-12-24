#pragma once
#include <d3d11.h>
// defines allows more compatible code, precede all xnamath includes with these defines
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>
#include <math.h>
#include "Player.h"

class Camera
{
private:
	Player* m_pPlayer;
	XMVECTOR m_forward;
	XMVECTOR m_cameraRotation;
	XMVECTOR m_targetRotation;
	XMVECTOR m_position;
	XMVECTOR m_lookat;
	XMVECTOR m_up;
	XMVECTOR m_direction;
	float m_followDistance;
	float m_followHeight;
	float m_followRotation;
	bool  m_followCamera;
	float m_lerpMultiplier = 2;
public:

	Camera();
	Camera(float startX, float startY, float startZ, float startRot);
	~Camera();

	void Rotate(float horizontal, float vertical);
	void Forward(float distance);
	void Strafe(float distance);
	void Up(float distance);
	XMMATRIX GetViewMatrix();
	XMVECTOR GetPosition();
	XMVECTOR GetForward();
	XMVECTOR GetRight();
	XMVECTOR GetRotation();

	void SetUpPlayerFollow(Player* playerPtr, float followDistance, float followHeight, float followRotation);
	void Update();
};