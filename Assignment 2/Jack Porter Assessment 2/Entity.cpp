#include "Entity.h"



Entity::Entity()
{
	m_position = XMVectorSet(0, 0, 0, 0);
	m_rotation = XMVectorSet(0, 0, 0, 0);
	m_scale = XMVectorSet(1, 1, 1, 0);
}

Entity::Entity(std::vector<Entity*>* entityList)
{
	m_pEntityList = entityList;
	m_scale = XMVectorSet(1, 1, 1,0);
}


Entity::~Entity()
{
}




void Entity::Update()
{
}

void Entity::Draw(XMMATRIX* view, XMMATRIX* projection)
{
	XMMATRIX world;
	world = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	world *= XMMatrixRotationX(XMConvertToRadians(m_rotation.x));
	world *= XMMatrixRotationY(XMConvertToRadians(m_rotation.y));
	world *= XMMatrixRotationZ(XMConvertToRadians(m_rotation.z));
	world *= XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

	m_pModel->Draw(&world, view, projection);
}

void Entity::UpdateLighting(XMVECTOR directionVector, XMVECTOR directionColor, XMVECTOR ambientColor)
{
	m_pModel->setDirectionalLight(directionVector, directionColor);
	m_pModel->setAmbientLight(ambientColor);
}

void Entity::LookAt(float posX, float posY, float posZ)
{
	float dx = posX - m_position.x;
	float dy = posY - m_position.y;
	float dz = posZ - m_position.z;

	float xz = sqrt((m_position.x * m_position.x) + (m_position.z * m_position.z));

	float yAngle = (atan2(dx, dz) * (180 / XM_PI));
	float xAngle = (-atan2(dy, xz) * (180 / XM_PI));

	if (xAngle > 89)
	{
		xAngle = 89;
	}
	else if (xAngle < -89)
	{
		xAngle = -89;
	}

	m_rotation = XMVectorSet(xAngle, yAngle, 0, 0);
}

void Entity::MoveForward(float distance)
{
	float posX = sin(m_rotation.y * (XM_PI / 180)) * distance;
	float posY = -sin(m_rotation.x * (XM_PI / 180.0)) * distance;
	float posZ = cos(m_rotation.y * (XM_PI / 180)) * distance;
	m_position = XMVectorSet(m_position.x + posX, m_position.y + posY, m_position.z + posZ, 0);
}



XMVECTOR Entity::GetPosition()
{
	return m_position;
}

XMVECTOR Entity::GetForward()
{
	float x = sin(m_rotation.y * (XM_PI / 180));
	float y = -sin(m_rotation.x * (XM_PI / 180.0));
	float z = cos(m_rotation.y * (XM_PI / 180));
	return XMVectorSet(x, y, z, 0);
}

XMVECTOR Entity::GetRight()
{
	return XMVector3Cross(XMVectorSet(0,1,0,0),GetForward());
}

XMVECTOR Entity::GetRotation()
{
	return m_rotation;
}



XMVECTOR Entity::GetScale()
{
	return m_scale;
}

void Entity::SetPosition(float x, float y, float z)
{

	m_position = XMVectorSet(x, y, z, 0);
}

void Entity::SetAngle(float x, float y, float z)
{
	m_rotation = XMVectorSet(x, y, z, 0);
}

void Entity::SetScale(float scale)
{
	m_scale = XMVectorSet(scale, scale, scale, 0);
}

void Entity::SetUpModel(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char * modelFileName, char * textureFileName)
{
	m_pModel = new Model(device, deviceContext);
	//Add Model
	m_pModel->LoadObjModel(modelFileName);
	m_pModel->AddTexture(textureFileName);

	m_pModel->CalculateModelCentrePoint();
	m_pModel->CalculateBoundingSphereRadius();
}

void Entity::ChangePosition(float x, float y, float z)
{
	m_position = XMVectorSet(m_position.x + x, m_position.y + y, m_position.z + z, 0);
}

void Entity::ChangeRotation(float x, float y, float z)
{
	m_rotation = XMVectorSet(m_rotation.x + x, m_rotation.y + y, m_rotation.z + z, 0);
}

void Entity::ChangeScale(float x, float y, float z)
{
	m_scale = XMVectorSet(m_scale.x + x, m_scale.y + y, m_scale.z + z, 0);
}

Model * Entity::GetModel()
{
	return m_pModel;
}

