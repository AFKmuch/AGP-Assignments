#include "Model.h"



Model::Model()
{
}

Model::Model(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	m_pD3DDevice = device;
	m_pImmediateContext = deviceContext;

	m_x = 0;
	m_y = 0;
	m_z = 0;
	m_xAngle = 0;
	m_yAngle = 0;
	m_zAngle = 0;
	m_scale = 1;
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
	world = XMMatrixScaling(m_scale, m_scale, m_scale);
	world *= XMMatrixRotationX(XMConvertToRadians(m_xAngle));
	world *= XMMatrixRotationY(XMConvertToRadians(m_yAngle));
	world *= XMMatrixRotationZ(XMConvertToRadians(m_zAngle));
	world *= XMMatrixTranslation(m_x, m_y, m_z);


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
	float dx = posX - m_x;
	float dz = posZ - m_z;
	float dy = posY - m_y;
	float xz = sqrt((m_x * m_x) + (m_z * m_z));
	m_yAngle = (atan2(dx, dz) * (180 / XM_PI));
	m_xAngle = (-atan2(dy,xz) * (180 / XM_PI));
	if (m_xAngle > 89)
	{
		m_xAngle = 89;
	}
	else if (m_xAngle < -89)
	{
		m_xAngle = -89;
	}
}

void Model::MoveForward(float distance)
{
	m_x += sin(m_yAngle * (XM_PI / 180)) * distance;
	m_y += -sin(m_xAngle * (XM_PI / 180.0)) * distance;
	m_z += cos(m_yAngle * (XM_PI / 180)) * distance;
}

void Model::CalculateModelCentrePoint()
{
	m_bounding_sphere_centre_x = 0;
	m_bounding_sphere_centre_y = 0;
	m_bounding_sphere_centre_z = 0;
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
	   
	m_bounding_sphere_centre_x = minX + (distanceX / 2);
	m_bounding_sphere_centre_y = minY + (distanceY / 2);
	m_bounding_sphere_centre_z = minZ + (distanceZ / 2);
}

void Model::CalculateBoundingSphereRadius()
{
	float maxDistance = 0;
	for (int i = 0; i < m_pObject->numverts; i++)
	{
		float x = m_pObject->vertices[i].Pos.x + m_bounding_sphere_centre_x;
		float y = m_pObject->vertices[i].Pos.y + m_bounding_sphere_centre_y;
		float z = m_pObject->vertices[i].Pos.z + m_bounding_sphere_centre_z;
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
	return m_x;
}

float Model::getPosY()
{
	return m_y;
}

float Model::getPosZ()
{
	return m_z;
}

float Model::getAngleX()
{
	return m_xAngle;
}

float Model::getAngleY()
{
	return m_yAngle;
}

float Model::getAngleZ()
{
	return m_zAngle;
}

float Model::getScale()
{
	return m_scale;
}

XMVECTOR Model::GetBoundingSphereWorldSpacePosition()
{
	XMMATRIX world;
	world =  XMMatrixScaling(m_scale, m_scale, m_scale);
	world *= XMMatrixRotationX(XMConvertToRadians(m_xAngle));
	world *= XMMatrixRotationY(XMConvertToRadians(m_yAngle));
	world *= XMMatrixRotationZ(XMConvertToRadians(m_zAngle));
	world *= XMMatrixTranslation(m_x, m_y, m_z);

	XMVECTOR offset;
	offset = XMVectorSet(m_bounding_sphere_centre_x, m_bounding_sphere_centre_y, m_bounding_sphere_centre_z, 0.0);
	offset = XMVector3Transform(offset, world);

	return offset;
}

float Model::GetBoundingSphereRadius()
{
	return m_bounding_sphere_radius * m_scale;
}

void Model::setPosX(float x)
{
	m_x = x;
}

void Model::setPosY(float y)
{
	m_y = y;
}

void Model::setPosZ(float z)
{
	m_z = z;
}

void Model::setAngleX(float x)
{
	m_xAngle = x;
}

void Model::setAngleY(float y)
{
	m_yAngle = y;
}

void Model::setAngleZ(float z)
{
	m_zAngle = z;
}

void Model::setScale(float scale)
{
	m_scale = scale;
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
	m_x += x;
}

void Model::changePosY(float y)
{
	m_y += y;
}

void Model::changePosZ(float z)
{
	m_z += z;
}

void Model::changeAngleX(float x)
{
	m_xAngle += x;
}

void Model::changeAngleY(float y)
{
	m_yAngle += y;
}

void Model::changeAngleZ(float z)
{
	m_zAngle += z;
}

void Model::changeScale(float scale)
{
	m_scale += scale;
}



