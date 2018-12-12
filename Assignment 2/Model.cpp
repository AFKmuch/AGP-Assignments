#include "Model.h"



Model::Model()
{
}

Model::Model(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	m_pD3DDevice = device;
	m_pImmediateContext = deviceContext;

	m_position = XMVectorSet(0, 0, 0, 0);
	m_rotation = XMVectorSet(0, 0, 0, 0);
	m_scale = XMVectorSet(1, 1, 1, 0);
}


Model::~Model()
{
	if (m_pObject) delete m_pObject;
	if (m_pTexture)	m_pTexture->Release();
	if (m_pSampler) m_pSampler->Release();
	if (m_pConstantBuffer) m_pConstantBuffer->Release();
	if (m_pInputLayout) m_pInputLayout->Release();
	if (m_pVShader) m_pVShader->Release();
	if (m_pPShader) m_pPShader->Release();

}

HRESULT Model::LoadObjModel(char * filename)
{
	HRESULT hr = S_OK;
	m_pObject = new ObjFileModel(filename, m_pD3DDevice, m_pImmediateContext);

	if (m_pObject->filename == "FILE NOT LOADED")
	{
		return S_FALSE;
	}

	// create constant buffer
	D3D11_BUFFER_DESC constant_buffer_desc;
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));

	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	constant_buffer_desc.ByteWidth = 112;
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	hr = m_pD3DDevice->CreateBuffer(&constant_buffer_desc, NULL, &m_pConstantBuffer);
	if (FAILED(hr))//Return an error code if failed
	{
		return hr;
	}

	//Load and compile the pixel and vertex shaders - use vs_5_0 to target DX11 hardware only

	ID3DBlob *VS, *PS, *error;
	hr = D3DX11CompileFromFile("model_shaders.hlsl", 0, 0, "ModelVS", "vs_4_0", 0, 0, 0, &VS, &error, 0);

	if (error != 0)//Check for shader compilation error
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))//Don't fail if error is just a warning
		{
			return hr;
		}
	}

	hr = D3DX11CompileFromFile("model_shaders.hlsl", 0, 0, "ModelPS", "ps_4_0", 0, 0, 0, &PS, &error, 0);

	if (error != 0)//Check for shader compilation error
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))//Don't fail if error is just a warning
		{
			return hr;
		}
	}

	//Create shader objects
	hr = m_pD3DDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &m_pVShader);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = m_pD3DDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &m_pPShader);
	if (FAILED(hr))
	{
		return hr;
	}

	//Set the shader objects as active
	m_pImmediateContext->VSSetShader(m_pVShader, 0, 0);
	m_pImmediateContext->PSSetShader(m_pPShader, 0, 0);

	//Create and set the input layout object
	D3D11_INPUT_ELEMENT_DESC iedesc[] =
	{
		//Be very careful setting the correct dxgi format and D3D version
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = m_pD3DDevice->CreateInputLayout(iedesc, ARRAYSIZE(iedesc), VS->GetBufferPointer(), VS->GetBufferSize(), &m_pInputLayout);
	if (FAILED(hr))
	{
		return hr;
	}

	m_pImmediateContext->IASetInputLayout(m_pInputLayout);
	AddTexture((char*)"assets/texture.bmp");
	CalculateModelCentrePoint();
	CalculateBoundingSphereRadius();
	return S_OK;
}

HRESULT Model::Draw(XMMATRIX * view, XMMATRIX * projection)
{
	XMMATRIX transpose;
	XMMATRIX world;
	world = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	world *= XMMatrixRotationX(XMConvertToRadians(m_rotation.x));
	world *= XMMatrixRotationY(XMConvertToRadians(m_rotation.y));
	world *= XMMatrixRotationZ(XMConvertToRadians(m_rotation.z));
	world *= XMMatrixTranslation(m_position.x, m_position.y, m_position.z);


	m_model_cb_values.WorldViewProjection = world * (*view) * (*projection);
	m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, 0, &m_model_cb_values, 0, 0);
	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pImmediateContext->VSSetShader(m_pVShader, 0, 0);
	m_pImmediateContext->PSSetShader(m_pPShader, 0, 0);
	m_pImmediateContext->IASetInputLayout(m_pInputLayout);
	m_pImmediateContext->PSSetSamplers(0, 1, &m_pSampler);
	m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTexture);

	m_pObject->Draw();
	return S_OK;
}

HRESULT Model::AddTexture(char * filename)
{
	D3D11_SAMPLER_DESC sampler_desc;
	ZeroMemory(&sampler_desc, sizeof(sampler_desc));
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

	m_pD3DDevice->CreateSamplerState(&sampler_desc, &m_pSampler);

	D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice, filename, NULL, NULL, &m_pTexture, NULL); // Create texture

	return S_OK;
}

void Model::LookAt(float posX, float posZ, float posY)
{
	float dx = posX - m_position.x;
	float dy = posY - m_position.y;
	float dz = posZ - m_position.z;

	float xz = sqrt((m_position.x * m_position.x) + (m_position.z * m_position.z));

	float yAngle = (atan2(dx, dz) * (180 / XM_PI));
	float xAngle = (-atan2(dy,xz) * (180 / XM_PI));

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

void Model::MoveForward(float distance)
{
	m_position = XMVectorSet(sin(m_rotation.y * (XM_PI / 180)) * distance, -sin(m_rotation.x * (XM_PI / 180.0)) * distance, cos(m_rotation.y * (XM_PI / 180)) * distance, 0);
}

void Model::CalculateModelCentrePoint()
{
	m_bounding_sphere_centre = XMVectorSet(0, 0, 0, 0);
	float maxX = -D3D11_FLOAT32_MAX;;
	float maxY = -D3D11_FLOAT32_MAX;;
	float maxZ = -D3D11_FLOAT32_MAX;;
	float minX = D3D11_FLOAT32_MAX;;
	float minY = D3D11_FLOAT32_MAX;;
	float minZ = D3D11_FLOAT32_MAX;;
 
	for (int i = 0; i < m_pObject->numverts; i++)
	{
		if (m_pObject->vertices[i].Pos.x > maxX)
		{
			maxX = m_pObject->vertices[i].Pos.x;
		}
		else if (m_pObject->vertices[i].Pos.x < minX)
		{
			minX = m_pObject->vertices[i].Pos.x;
		}

		if (m_pObject->vertices[i].Pos.y > maxY)
		{
			maxY = m_pObject->vertices[i].Pos.y;
		}
		else if (m_pObject->vertices[i].Pos.y < minY)
		{
			minY = m_pObject->vertices[i].Pos.y;
		}

		if (m_pObject->vertices[i].Pos.z > maxZ)
		{
			maxZ = m_pObject->vertices[i].Pos.z;
		}
		else if (m_pObject->vertices[i].Pos.z < minZ)
		{
			minZ = m_pObject->vertices[i].Pos.z;
		}
	}
	float distanceX = abs(minX) + abs(maxX);
	float distanceY = abs(minY) + abs(maxY);
	float distanceZ = abs(minZ) + abs(maxZ);
	   
	XMVectorSet(minX + (distanceX / 2), minY + (distanceY / 2), minZ + (distanceZ / 2), 0);
}

void Model::CalculateBoundingSphereRadius()
{
	float maxDistance = 0;
	for (int i = 0; i < m_pObject->numverts; i++)
	{
		float x = m_pObject->vertices[i].Pos.x + m_bounding_sphere_centre.x;
		float y = m_pObject->vertices[i].Pos.y + m_bounding_sphere_centre.y;
		float z = m_pObject->vertices[i].Pos.z + m_bounding_sphere_centre.z;
		float distance = (pow(x, 2) + pow(y, 2) + pow(z, 2));
		if (distance > maxDistance)
		{
			maxDistance = distance;
		}
	}

	m_bounding_sphere_radius = sqrt(maxDistance);
}

bool Model::CheckCollision(Model * model)
{
	if (model == this)
	{
		return false;
	}
	XMVECTOR thisModelPos = GetBoundingSphereWorldSpacePosition();
	XMVECTOR thatModelPos = model->GetBoundingSphereWorldSpacePosition();
	XMVECTOR distanceVector = XMVectorSet(thatModelPos.x - thisModelPos.x, thatModelPos.y - thisModelPos.y, thatModelPos.z - thisModelPos.z, thatModelPos.w - thisModelPos.w);
	float distance = (distanceVector.x * distanceVector.x) + (distanceVector.y * distanceVector.y) + (distanceVector.z * distanceVector.z);

	distance = sqrt(distance);
	if (distance >  (GetBoundingSphereRadius() + model->GetBoundingSphereRadius()))
	{
		return false;
	}
	else
	{
		return true;
	}

	return false;
}

float Model::getPosX()
{
	return m_position.x;
}

float Model::getPosY()
{
	return m_position.y;
}

float Model::getPosZ()
{
	return m_position.z;
}

float Model::getAngleX()
{
	return m_rotation.x;
}

float Model::getAngleY()
{
	return m_rotation.y;
}

float Model::getAngleZ()
{
	return m_rotation.z;
}

float Model::getScale()
{
	return 1;
}

XMVECTOR Model::GetBoundingSphereWorldSpacePosition()
{
	XMMATRIX world;
	world =  XMMatrixScaling(1, 1, 1);
	world *= XMMatrixRotationX(XMConvertToRadians(m_rotation.x));
	world *= XMMatrixRotationY(XMConvertToRadians(m_rotation.y));
	world *= XMMatrixRotationZ(XMConvertToRadians(m_rotation.z));
	world *= XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

	XMVECTOR offset;
	offset = XMVector3Transform(m_bounding_sphere_centre, world);

	return offset;
}

float Model::GetBoundingSphereRadius()
{
	return m_bounding_sphere_radius * 1;
}

void Model::setPosX(float x)
{
	m_position = XMVectorSet(x, m_position.y, m_position.z, 0);
}

void Model::setPosY(float y)
{
	m_position = XMVectorSet(m_position.x, y, m_position.z, 0);
}

void Model::setPosZ(float z)
{
	m_position = XMVectorSet(m_position.x, m_position.y, z, 0);
}

void Model::setAngleX(float x)
{
	m_rotation = XMVectorSet(x, m_rotation.y, m_rotation.z, 0);
}

void Model::setAngleY(float y)
{
	m_rotation = XMVectorSet(m_rotation.x, y, m_rotation.z, 0);
}

void Model::setAngleZ(float z)
{
	m_rotation = XMVectorSet(m_rotation.x, m_rotation.y, z, 0);
}

void Model::setScale(float scale)
{
	m_scale = XMVectorSet(scale, scale, scale, 0);
}

void Model::setDirectionalLight(XMVECTOR direction, XMVECTOR color)
{
	m_model_cb_values.directional_light_vector = direction;
	m_model_cb_values.directional_light_colour = color;
}

void Model::setAmbientLight(XMVECTOR color)
{
	m_model_cb_values.ambient_light_colour = color;
}

void Model::changePosX(float x)
{
	m_position = XMVectorSet(m_position.x + x, m_position.y, m_position.z, 0);
}

void Model::changePosY(float y)
{
	m_position = XMVectorSet(m_position.x, m_position.y + y, m_position.z, 0);
}

void Model::changePosZ(float z)
{
	m_position = XMVectorSet(m_position.x, m_position.y, m_position.z + z, 0);
}

void Model::changeAngleX(float x)
{
	m_rotation = XMVectorSet(m_rotation.x + x, m_rotation.y, m_rotation.z, 0);
}

void Model::changeAngleY(float y)
{
	m_rotation = XMVectorSet(m_rotation.x, m_rotation.y + y, m_rotation.z, 0);
}

void Model::changeAngleZ(float z)
{
	m_rotation = XMVectorSet(m_rotation.x, m_rotation.y, m_rotation.z + z, 0);
}

void Model::changeScale(float scale)
{
	m_scale = XMVectorSet(m_scale.x + scale, m_scale.y + scale, m_scale.z + scale, 0);
}



