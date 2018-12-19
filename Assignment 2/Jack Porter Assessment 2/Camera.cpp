#include "Camera.h"



Camera::Camera()
{
}

Camera::Camera(float startX, float startY, float startZ, float startRot)
{
	m_position = XMVectorSet(startX, startY, startZ, 0);
	m_camera_rotation.y = startRot;
	m_camera_rotation.x = 0;

	m_forward.x = sin(m_camera_rotation.y * (XM_PI / 180));
	m_forward.z = cos(m_camera_rotation.y * (XM_PI / 180));
	m_followCamera = false;
}


Camera::~Camera()
{
}

void Camera::Rotate(float horizontal, float vertical)
{
	m_camera_rotation.y += horizontal;
	m_camera_rotation.x += vertical;
	if (m_camera_rotation.x >= 89)
	{
		m_camera_rotation.x = 89;
	}
	if (m_camera_rotation.x <= -89)
	{
		m_camera_rotation.x = -89;
	}
	m_forward.x = sin(m_camera_rotation.y * (XM_PI / 180));
	m_forward.z = cos(m_camera_rotation.y * (XM_PI / 180));
	m_forward.y = sin(m_camera_rotation.x * (XM_PI / 180));
}

void Camera::Forward(float distance)
{
	m_position.x += distance * m_forward.x;
	m_position.y += distance * m_forward.y;
	m_position.z += distance * m_forward.z;
}

void Camera::Strafe(float distance)
{
	m_lookat = XMVectorSet(m_position.x + m_forward.x, m_position.y + m_forward.y, m_position.z + m_forward.z, 0);
	m_up = XMVectorSet(0, 1, 0, 0);
	m_direction = XMVector3Cross(m_up, m_lookat - m_position);

	m_position.x += m_direction.x * distance;
	m_position.z += m_direction.z * distance;

}

void Camera::Up(float distance)
{
	m_position.y += distance;
}

XMMATRIX Camera::GetViewMatrix()
{
	//m_position = XMVectorSet(m_x, m_y, m_z, 0);
	m_lookat = XMVectorSet(m_position.x + m_forward.x, m_position.y + m_forward.y, m_position.z + m_forward.z, 0);
	m_up = XMVectorSet(0, 1, 0, 0);

	return XMMatrixLookAtLH(m_position, m_lookat, m_up);
}

XMVECTOR Camera::GetPosition()
{
	return m_position;
}

void Camera::SetUpPlayerFollow(Player * playerPtr, float followDistance, float followHeight, float followRotation)
{
	m_pPlayer = playerPtr;
	m_followDistance = followDistance;
	m_followHeight = followHeight;
	m_followRotation = followRotation;
	m_followCamera = true;
}

void Camera::Update()
{
	if (m_followCamera)
	{
		XMVECTOR targetPosition = XMVectorSet(m_pPlayer->GetPosition().x - (sin(m_camera_rotation.y * (XM_PI / 180)) * m_followDistance), m_pPlayer->GetPosition().y + m_followHeight, m_pPlayer->GetPosition().z - (cos(m_camera_rotation.y * (XM_PI / 180)) * m_followDistance), 0);
		XMVECTOR targetRotation = XMVectorSet(m_followRotation, m_pPlayer->GetRotation().y, 0, 0);
		m_position = XMVectorLerp(m_position, targetPosition, Time::Instance()->DeltaTime() * m_lerpMultiplier);
		m_camera_rotation = XMVectorLerp(m_camera_rotation, targetRotation, Time::Instance()->DeltaTime() * m_lerpMultiplier);
		m_camera_rotation = XMVectorSet(m_followRotation, m_pPlayer->GetRotation().y, 0, 0);
		m_forward.x = sin(m_camera_rotation.y * (XM_PI / 180));
		m_forward.z = cos(m_camera_rotation.y * (XM_PI / 180));
		m_forward.y = sin(m_camera_rotation.x * (XM_PI / 180));

	}


}

