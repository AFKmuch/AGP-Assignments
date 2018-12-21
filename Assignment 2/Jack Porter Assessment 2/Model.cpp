#include "Model.h"



Model::Model()
{
}

Model::Model(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	m_pD3DDevice = device;
	m_pImmediateContext = deviceContext;


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

	hr = m_pD3DDevice->CreateInputLayout(iedesc, _ARRAYSIZE(iedesc), VS->GetBufferPointer(), VS->GetBufferSize(), &m_pInputLayout);
	if (FAILED(hr))
	{
		return hr;
	}

	m_pImmediateContext->IASetInputLayout(m_pInputLayout);
	return S_OK;
}

HRESULT Model::LoadShader(char * filename)
{
	HRESULT hr = S_OK;
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
	hr = D3DX11CompileFromFile(filename, 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, &error, 0);

	if (error != 0)//Check for shader compilation error
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))//Don't fail if error is just a warning
		{
			return hr;
		}
	}

	hr = D3DX11CompileFromFile(filename, 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, &error, 0);

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

	hr = m_pD3DDevice->CreateInputLayout(iedesc, _ARRAYSIZE(iedesc), VS->GetBufferPointer(), VS->GetBufferSize(), &m_pInputLayout);
	if (FAILED(hr))
	{
		return hr;
	}

	m_pImmediateContext->IASetInputLayout(m_pInputLayout);
	return S_OK;
}

HRESULT Model::Draw(XMMATRIX* world, XMMATRIX * view, XMMATRIX * projection)
{
	XMMATRIX transpose;
	transpose = XMMatrixTranspose((*world));
	m_model_cb_values.WorldViewProjection = (*world) * (*view) * (*projection);
	m_model_cb_values.directional_light_vector = XMVector3Transform(m_model_cb_values.directional_light_vector, transpose);
	m_model_cb_values.directional_light_vector = XMVector3Normalize(m_model_cb_values.directional_light_vector);
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

XMVECTOR Model::GetBoundingSphereWorldSpacePosition(XMVECTOR scale, XMVECTOR position, XMVECTOR rotation)
{
	XMMATRIX world;
	world = XMMatrixScaling(scale.x, scale.y, scale.z);
	world *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	world *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	world *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	world *= XMMatrixTranslation(position.x, position.y, position.z);

	XMVECTOR offset;
	offset = XMVector3Transform(m_bounding_sphere_centre, world);

	return offset;
}

float Model::GetBoundingSphereRadius(XMVECTOR scale)
{
	float maxScale = 0;
	if (scale.x > maxScale)
	{
		maxScale = scale.x;
	}
	if (scale.y > maxScale)
	{
		maxScale = scale.y;
	}
	if (scale.z > maxScale)
	{
		maxScale = scale.z;
	}
	return m_bounding_sphere_radius * maxScale;
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



