#include "ParticleSystem.h"

ParticleSystem::ParticleSystem()
{
}

ParticleSystem::ParticleSystem(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	m_pD3DDevice = device;
	m_pImmediateContext = deviceContext;
	for (int i = 0; i < 5000; i++)
	{
		Particle* newParticle = new Particle;
		newParticle->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		newParticle->gravity = 0;
		newParticle->position = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		newParticle->velocity = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		m_free.push_back(newParticle);
	}
}

ParticleSystem::~ParticleSystem()
{
	if (m_pTexture)	m_pTexture->Release();
	if (m_pSampler) m_pSampler->Release();
	if (m_pConstantBuffer) m_pConstantBuffer->Release();
	if (m_pInputLayout) m_pInputLayout->Release();
	if (m_pVShader) m_pVShader->Release();
	if (m_pPShader) m_pPShader->Release();

}

HRESULT ParticleSystem::CreateParticle()
{
	HRESULT hr = S_OK;
	XMFLOAT3 vertices[6] =//verts for the quad NOTE: could be changed to make different shapes?? Side Note: Or you could use alpha and a square so you're only drawing 2 triangles.
	{
		XMFLOAT3(-1.0f,-1.0f,0.0f),
		XMFLOAT3(1.0f,1.0f,0.0f),
		XMFLOAT3(-1.0f,1.0f,0.0f),
		XMFLOAT3(-1.0f,-1.0f,0.0f),
		XMFLOAT3(1.0f,-1.0f,0.0f),
		XMFLOAT3(1.0f,1.0f,0.0f),
	};

	//create the vert buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;//both cpu and gpu
	bufferDesc.ByteWidth = sizeof(XMFLOAT3) * 6/*VERTCOUNT*/;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//edit on the cpu

	hr = m_pD3DDevice->CreateBuffer(&bufferDesc, NULL, &m_pVertexBuffer);

	if (FAILED(hr)) return hr;

	//copy verts to buffer
	D3D11_MAPPED_SUBRESOURCE ms;

	//mapping = locking the buffer which allows writing
	m_pImmediateContext->Map(m_pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	memcpy(ms.pData, vertices, sizeof(vertices));//copy data
												 //unlock the buffer
	m_pImmediateContext->Unmap(m_pVertexBuffer, NULL);

	/*CalcModelCentrePoint();
	CalcBoundingSphereRadius();*/
	LoadShader();
	return S_OK;
}

HRESULT ParticleSystem::LoadShader()
{
	HRESULT hr = S_OK;

	// create constant buffer
	D3D11_BUFFER_DESC constant_buffer_desc;
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));

	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	constant_buffer_desc.ByteWidth = 80;
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	hr = m_pD3DDevice->CreateBuffer(&constant_buffer_desc, NULL, &m_pConstantBuffer);
	if (FAILED(hr))//Return an error code if failed
	{
		return hr;
	}


	//Load and compile the pixel and vertex shaders - use vs_5_0 to target DX11 hardware only

	ID3DBlob *VS, *PS, *error;
	hr = D3DX11CompileFromFile("particle_shader.hlsl", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, &error, 0);

	if (error != 0)//Check for shader compilation error
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))//Don't fail if error is just a warning
		{
			return hr;
		}
	}

	hr = D3DX11CompileFromFile("particle_shader.hlsl", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, &error, 0);

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
	};

	hr = m_pD3DDevice->CreateInputLayout(iedesc, _ARRAYSIZE(iedesc), VS->GetBufferPointer(), VS->GetBufferSize(), &m_pInputLayout);
	if (FAILED(hr))
	{
		return hr;
	}

	m_pImmediateContext->IASetInputLayout(m_pInputLayout);

	//D3D11_SAMPLER_DESC sampler_desc;
	//ZeroMemory(&sampler_desc, sizeof(sampler_desc));
	//sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	//sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	//sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	//sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	//sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
	//
	//m_pD3DDevice->CreateSamplerState(&sampler_desc, &m_pSampler);
	//
	//D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice, textureFileName, NULL, NULL, &m_pTexture, NULL); // Create texture
	return S_OK;
}

HRESULT ParticleSystem::Draw(XMMATRIX * view, XMMATRIX * projection, XMVECTOR* cameraPosition)
{
	std::list<Particle*>::iterator it;
	if (m_active.size() > 0)
	{
		for (int i = 0; i < m_active.size(); i++)
		{
			it = m_active.begin();
			std::advance(it, i);
			(*it)->currentLife -= Time::Instance()->DeltaTime();
			(*it)->velocity = XMVectorSet((*it)->velocity.x, (*it)->velocity.y + ((*it)->gravity * Time::Instance()->DeltaTime()), (*it)->velocity.z, 0);
			(*it)->position += ((*it)->velocity * Time::Instance()->DeltaTime());
			DrawOne((*it), view, projection, cameraPosition);
			if ((*it)->currentLife <= 0)
			{
				m_free.push_back((*it));
				m_active.remove((*it));
			}
		}
	}

	return S_OK;
}

HRESULT ParticleSystem::DrawOne(Particle * one, XMMATRIX * view, XMMATRIX * projection, XMVECTOR * cameraPosition)
{
	//set vertex buffer
	UINT stride = sizeof(XMFLOAT3);
	UINT offset = 0;
	m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//select which primitive type to use 
	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	float yAngle = atan2((cameraPosition->x - one->position.x), (cameraPosition->z - one->position.z));
	float xAngle = -atan2((cameraPosition->y - one->position.y), sqrt(pow((cameraPosition->x - one->position.x), 2) + pow((cameraPosition->z - one->position.z), 2)));
		
	PARTICLE_CONSTANT_BUFFER cBufferValues;
	XMMATRIX world;
	world = XMMatrixScaling(one->scale, one->scale, one->scale);
	world *= XMMatrixRotationX(xAngle);
	world *= XMMatrixRotationY(yAngle);
	//world *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	world *= XMMatrixTranslation(one->position.x, one->position.y, one->position.z);

	cBufferValues.WorldViewProjection = (world) * (*view) * (*projection);
	cBufferValues.color = one->color;
	m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, 0, &cBufferValues, 0, 0);
	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pImmediateContext->PSSetSamplers(0, 1, &m_pSampler);
	m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTexture);

	m_pImmediateContext->VSSetShader(m_pVShader, 0, 0);
	m_pImmediateContext->PSSetShader(m_pPShader, 0, 0);

	m_pImmediateContext->IASetInputLayout(m_pInputLayout);

	//draw the vertex buffer to the back buffer
	m_pImmediateContext->Draw(6, 0);


	return S_OK;
}

HRESULT ParticleSystem::AddTexture(char * filename)
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

void ParticleSystem::LoadEffect(PARTICLE_EFFECT_TYPE type, XMVECTOR startPos, XMVECTOR forward)
{
	switch (type)
	{
	case STREAM:
		for (int i = 0; i < 3; i++)
		{
			if (m_free.size() > 0)
			{
				//Set random RGB values
				float r = (std::rand() % 2);
				float g = (std::rand() % 2);
				float b = (std::rand() % 2);
				(*m_free.begin())->color = { r, g, b, 1.0f };

				(*m_free.begin())->scale = 0.3f;

				//set life and gravity
				(*m_free.begin())->maxLife = 2.0f;
				(*m_free.begin())->currentLife = (*m_free.begin())->maxLife;
				(*m_free.begin())->gravity = 0.0f;

				//set random offset velocities
				float randomHorizontal = (std::rand() % 10) - 5;
				float randomVertical = (std::rand() % 10) - 5;

				//calculate right vector
				XMVECTOR right = XMVector3Cross(XMVectorSet(0, 1, 0, 0), forward);

				//set base velocity
				(*m_free.begin())->velocity = -forward * 10;

				//offset velocity based on random offsets and right vector
				(*m_free.begin())->velocity = XMVectorSet((*m_free.begin())->velocity.x + (right.x * randomHorizontal), randomVertical, (*m_free.begin())->velocity.z + (right.z * randomHorizontal), 0);

				//set start position
				(*m_free.begin())->position = startPos;

				//add to active list and remove from free list
				m_active.push_back((*m_free.begin()));
				m_free.remove((*m_free.begin()));
			}
		}
		break;
	case FOUNTAIN:
		for (int i = 0; i < 3; i++)
		{
			if (m_free.size() > 0)
			{
				//Set random RGB values
				float r = (std::rand() % 2);
				float g = (std::rand() % 2);
				float b = (std::rand() % 2);
				(*m_free.begin())->color = { r, g, b, 1.0f };

				(*m_free.begin())->scale = 0.3f;

				//set life and gravity
				(*m_free.begin())->maxLife = 4.0f;
				(*m_free.begin())->currentLife = (*m_free.begin())->maxLife;
				(*m_free.begin())->gravity = -9.81f;

				//set random offset velocities
				float randomX = (std::rand() % 5) - 2.5f;
				float randomZ = (std::rand() % 5) - 2.5f;
				float randomY = (std::rand() % 5) + 15;

				//set base velocity
				(*m_free.begin())->velocity = XMVectorSet(randomX, randomY, randomZ, 0);

				//set start position
				(*m_free.begin())->position = startPos;

				//add to active list and remove from free list
				m_active.push_back((*m_free.begin()));
				m_free.remove((*m_free.begin()));
			}
		}
		break;
	case SNOW:
		for (int i = 0; i < 5; i++)
		{
			if (m_free.size() > 0)
			{
				//Set white RGB values
				(*m_free.begin())->color = { 1.0f, 1.0f, 1.0f, 0.5f };

				(*m_free.begin())->scale = 0.15f;

				//set life and gravity
				(*m_free.begin())->maxLife = 10.0f;
				(*m_free.begin())->currentLife = (*m_free.begin())->maxLife;
				(*m_free.begin())->gravity = 0;

				//set random offset velocities
				float randomHorizontal = (std::rand() % 8) - 4;
				float randomVertical = (std::rand() % 3) - 3;

				//calculate right vector
				XMVECTOR right = XMVector3Cross(XMVectorSet(0, 1, 0, 0), forward);

				//set base velocity
				(*m_free.begin())->velocity = forward * 0;

				//offset velocity based on random offsets and right vector
				(*m_free.begin())->velocity = XMVectorSet((*m_free.begin())->velocity.x + (right.x * randomHorizontal), randomVertical, (*m_free.begin())->velocity.z + (right.z * randomHorizontal), 0);

				//set start position
				randomHorizontal = (std::rand() % 100) - 50;
				randomVertical = (std::rand() % 10) - 5;
				float randomForward = (std::rand() % 100) - 50;
				(*m_free.begin())->position = startPos;
				float x = (*m_free.begin())->position.x + (right.x * randomHorizontal) + (forward.x * randomForward);
				float y = (*m_free.begin())->position.y + randomVertical;
				float z = (*m_free.begin())->position.z + (right.z * randomHorizontal) + (forward.z * randomForward);
				(*m_free.begin())->position = XMVectorSet(x,y,z, 0);

				//add to active list and remove from free list
				m_active.push_back((*m_free.begin()));
				m_free.remove((*m_free.begin()));
			}
		}
		break;
	default:
		break;
	}
}



