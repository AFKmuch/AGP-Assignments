#include "ParticleSystem.h"
#include "ModelManager.h"

ParticleSystem* ParticleSystem::s_instance;

ParticleSystem::ParticleSystem()
{
}

ParticleSystem::~ParticleSystem()
{
	std::list<Particle*>::iterator it;
	if (m_active.size() > 0)
	{
		for (int i = m_active.size() - 1; i >= 0; i--)
		{
			it = m_active.begin();
			std::advance(it, i);
			if ((*it)->texture) (*it)->texture->Release();
			if ((*it)->sampler) (*it)->sampler->Release();
		}
	}

	if (m_active.size() > 0)
	{
		for (int i = m_free.size() - 1; i >= 0; i--)
		{
			it = m_free.begin();
			std::advance(it, i);
			if ((*it)->texture) (*it)->texture->Release();
			if ((*it)->sampler) (*it)->sampler->Release();
		}
	}

	if (m_pAlphaBlendEnabled) m_pAlphaBlendEnabled->Release();
	if (m_pAlphaBlendDisabled) m_pAlphaBlendDisabled->Release();
	if (m_pConstantBuffer) m_pConstantBuffer->Release();
	if (m_pInputLayout) m_pInputLayout->Release();
	if (m_pVShader) m_pVShader->Release();
	if (m_pPShader) m_pPShader->Release();
	if (m_pVertexBuffer) m_pVertexBuffer->Release();
	if (m_pImmediateContext) m_pImmediateContext->Release();
	if (m_pD3DDevice) m_pD3DDevice->Release();
	if (m_pDepthWriteParticle) m_pDepthWriteParticle->Release();
	if (m_pDepthWriteSolid) m_pDepthWriteSolid->Release();
	if (m_pRasterParticle) m_pRasterParticle->Release();
	if (m_pRasterSolid) m_pRasterSolid->Release();
}

void ParticleSystem::SetUpParticleSystem(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	m_pD3DDevice = device;
	m_pImmediateContext = deviceContext;

	for (int i = 0; i < 5000; i++)
	{
		Particle* newParticle = new Particle;
		newParticle->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		newParticle->gravity = 0;
		newParticle->sampler = ModelManager::Instance()->LoadTexture((char*)"assets/Particles/DeafaultParticle.bmp")->Sampler;
		newParticle->texture = ModelManager::Instance()->LoadTexture((char*)"assets/Particles/DeafaultParticle.bmp")->Texture;
		newParticle->position = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		newParticle->velocity = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		m_free.push_back(newParticle);
	}

	D3D11_BLEND_DESC b;
	b.RenderTarget[0].BlendEnable = FALSE;
	b.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	b.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	b.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	b.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	b.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	b.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	b.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	b.IndependentBlendEnable = TRUE;
	b.AlphaToCoverageEnable = TRUE;

	m_pD3DDevice->CreateBlendState(&b, &m_pAlphaBlendDisabled);

	D3D11_BLEND_DESC c;
	c.RenderTarget[0].BlendEnable = TRUE;
	c.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	c.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	c.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	c.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	c.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	c.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	c.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	c.IndependentBlendEnable = FALSE;
	c.AlphaToCoverageEnable = FALSE;

	m_pD3DDevice->CreateBlendState(&c, &m_pAlphaBlendEnabled);

}

HRESULT ParticleSystem::CreateParticle()
{
	HRESULT hr = S_OK;
	XMFLOAT3 vertices[] =//verts for the quad NOTE: could be changed to make different shapes?? Side Note: Or you could use alpha and a square so you're only drawing 2 triangles.
	{
		XMFLOAT3(-1.0f,-1.0f,0.0f),	//XMFLOAT2(0.0f, 1.0f) },
		XMFLOAT3(1.0f, 1.0f,0.0f),	//XMFLOAT2(1.0f, 0.0f) },
		XMFLOAT3(-1.0f, 1.0f,0.0f),	//XMFLOAT2(0.0f, 0.0f) },
		XMFLOAT3(-1.0f,-1.0f,0.0f),	//XMFLOAT2(0.0f, 1.0f) },
		XMFLOAT3(1.0f,-1.0f,0.0f),	//XMFLOAT2(1.0f, 1.0f) },
		XMFLOAT3(1.0f, 1.0f,0.0f),	//XMFLOAT2(1.0f, 0.0f) }
	};

	//create the vert buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;//both cpu and gpu
	bufferDesc.ByteWidth = sizeof(vertices);
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

	m_pImmediateContext->VSSetShader(m_pVShader, 0, 0);
	m_pImmediateContext->PSSetShader(m_pPShader, 0, 0);

	m_pImmediateContext->IASetInputLayout(m_pInputLayout);

	m_pImmediateContext->PSSetSamplers(0, 1, &one->sampler);
	m_pImmediateContext->PSSetShaderResources(0, 1, &one->texture);

	m_pImmediateContext->OMSetBlendState(m_pAlphaBlendEnabled, 0, 0xffffffff);

	//draw the vertex buffer to the back buffer
	m_pImmediateContext->Draw(6, 0);

	m_pImmediateContext->OMSetBlendState(m_pAlphaBlendDisabled, 0, 0xffffffff);
	return S_OK;
}

HRESULT ParticleSystem::AddTexture(char * filename, Particle* particle)
{
	particle->texture = ModelManager::Instance()->LoadTexture(filename)->Texture;
	particle->sampler = ModelManager::Instance()->LoadTexture(filename)->Sampler;

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
				AddTexture((char*)"assets/Particles/flame_01.png", (*m_free.begin()));
				//Set random RGB values
				float r = (std::rand() % 2);
				float g = (std::rand() % 2);
				float b = (std::rand() % 2);
				(*m_free.begin())->color = { r, g, b, 1.0f };

				(*m_free.begin())->scale = 1;

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
				AddTexture((char*)"assets/Particles/flame_01.png", (*m_free.begin()));
				//Set random RGB values
				float r = (std::rand() % 2);
				float g = (std::rand() % 2);
				float b = (std::rand() % 2);
				(*m_free.begin())->color = { r, g, b, 1.0f };

				(*m_free.begin())->scale = 1;

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
				AddTexture((char*)"assets/Particles/Snowflake.png", (*m_free.begin()));
				//Set white RGB values
				(*m_free.begin())->color = { 1.0f, 1.0f, 1.0f, 0.5f };

				(*m_free.begin())->scale = 1;

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

ParticleSystem * ParticleSystem::Instance()
{
	if (!s_instance)
	{
		s_instance = new ParticleSystem();
	}
	return s_instance;
}



