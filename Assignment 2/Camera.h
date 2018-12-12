#pragma once
#include <d3d11.h>
// defines allows more compatible code, precede all xnamath includes with these defines
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>
#include <math.h>

class Camera
{
private:
	
	XMVECTOR m_forward;
	XMVECTOR m_camera_rotation;
	XMVECTOR m_position;
	XMVECTOR m_lookat;
	XMVECTOR m_up;
	XMVECTOR m_direction;

public:

	Camera();
	Camera(float startX, float startY, float startZ, float startRot);
	~Camera();

	void Rotate(float horizontal, float vertical);
	void Forward(float distance);
	void Strafe(float distance);
	void Up(float distance);
	XMMATRIX GetViewMatrix();

	float GetX();
	float GetY();
	float GetZ();



};

