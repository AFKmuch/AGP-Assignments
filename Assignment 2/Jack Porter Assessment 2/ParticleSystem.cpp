#include "ParticleSystem.h"

ParticleSystem::ParticleSystem()
{
}

ParticleSystem::ParticleSystem(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	m_pD3DDevice = device;
	m_pImmediateContext = deviceContext;
	for (int i = 0; i < 100; i++)
	{
		Particle* newParticle = new Particle;
		newParticle->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		newParticle->gravity = 0;
		newParticle->position = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		newParticle->velocity = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		m_free.push_back(newParticle); //continue from here
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
	Particle test;
	test.color = XMFLOAT4(1.0f, 0.0f, 0.3f, 1.0f);
	test.gravity = 1;
	test.position = XMVectorSet(10.0f, 20.0f, 45.0f, 0.0f);
	test.velocity = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	DrawOne(&test, view, projection, cameraPosition);
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

	PARTICLE_CONSTANT_BUFFER cBufferValues;
	XMMATRIX world;
	world = XMMatrixScaling(0.3f, 0.3f, 0.3f);
	//world *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	//world *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
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



