#include "SkyBox.h"



SkyBox::SkyBox(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Camera* cameraPtr)
{
	m_pD3DDevice = device;
	m_pImmediateContext = deviceContext;
	m_position = XMVectorSet(0, 0, 0, 0);
	m_pCamera = cameraPtr;
}


SkyBox::~SkyBox()
{
	if (m_pCamera) delete m_pCamera;
	if (m_pSampler) m_pSampler->Release();
	if (m_pTexture) m_pTexture->Release();
	if (m_pConstantBuffer) m_pConstantBuffer->Release();
	if (m_pInputLayout) m_pInputLayout->Release();
	if (m_pVShader) m_pVShader->Release();
	if (m_pPShader) m_pPShader->Release();
	if (m_pVertexBuffer) m_pVertexBuffer->Release();
	if (m_pImmediateContext) m_pImmediateContext->Release();
	if (m_pD3DDevice) m_pD3DDevice->Release();
	if (m_pDepthWriteSkyBox) m_pDepthWriteSkyBox->Release();
	if (m_pDepthWriteSolid) m_pDepthWriteSolid->Release();
	if (m_pRasterSkyBox) m_pRasterSkyBox->Release();
	if (m_pRasterSolid) m_pRasterSolid->Release();
}

HRESULT SkyBox::SetUp(char* textureFileName)
{
	HRESULT hr = S_OK;

	//Set up and create vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;										//Allows use by CPU and GPU
	bufferDesc.ByteWidth = sizeof(vertices);							//Set the total size of the buffer (in this case, 3 vertices)
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;							//Set the type of buffer to vertex buffer
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;							//Allow access by the CPU
	hr = m_pD3DDevice->CreateBuffer(&bufferDesc, NULL, &m_pVertexBuffer);		//Create the buffer

	if (FAILED(hr))//Return an error code if failed
	{
		return hr;
	}

	// create constant buffer
	D3D11_BUFFER_DESC constant_buffer_desc;
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));

	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	constant_buffer_desc.ByteWidth = 64;
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	hr = m_pD3DDevice->CreateBuffer(&constant_buffer_desc, NULL, &m_pConstantBuffer);
	if (FAILED(hr))//Return an error code if failed
	{
		return hr;
	}

	//Copy the vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;

	//Lock the buffer to allow writing
	m_pImmediateContext->Map(m_pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);

	//Copy the data
	memcpy(ms.pData, vertices, sizeof(vertices));

	//Unlock the buffer
	m_pImmediateContext->Unmap(m_pVertexBuffer, NULL);


	//Load and compile the pixel and vertex shaders - use vs_5_0 to target DX11 hardware only

	ID3DBlob *VS, *PS, *error;
	hr = D3DX11CompileFromFile("sky_shader.hlsl", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, &error, 0);

	if (error != 0)//Check for shader compilation error
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))//Don't fail if error is just a warning
		{
			return hr;
		}
	}

	hr = D3DX11CompileFromFile("sky_shader.hlsl", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, &error, 0);

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
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = m_pD3DDevice->CreateInputLayout(iedesc, _ARRAYSIZE(iedesc), VS->GetBufferPointer(), VS->GetBufferSize(), &m_pInputLayout);
	if (FAILED(hr))
	{
		return hr;
	}

	m_pImmediateContext->IASetInputLayout(m_pInputLayout);

	D3D11_SAMPLER_DESC sampler_desc;
	ZeroMemory(&sampler_desc, sizeof(sampler_desc));
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

	m_pD3DDevice->CreateSamplerState(&sampler_desc, &m_pSampler);

	D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice, textureFileName, NULL, NULL, &m_pTexture, NULL); // Create texture
	return S_OK;
}

void SkyBox::Draw(XMMATRIX * view, XMMATRIX * projection)
{
	SetPosition(m_pCamera->GetPosition());

	HRESULT hr = S_OK;
	ZeroMemory(&m_rasterizerDesc, sizeof(m_rasterizerDesc));

	D3D11_DEPTH_STENCIL_DESC DSDecsc;
	ZeroMemory(&DSDecsc, sizeof(DSDecsc));

	//set skybox culling and depth
	DSDecsc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	hr = m_pD3DDevice->CreateDepthStencilState(&DSDecsc, &m_pDepthWriteSkyBox);

	m_rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	m_rasterizerDesc.CullMode = D3D11_CULL_FRONT;
	hr = m_pD3DDevice->CreateRasterizerState(&m_rasterizerDesc, &m_pRasterSkyBox);
	m_pImmediateContext->RSSetState(m_pRasterSkyBox);
	m_pImmediateContext->OMSetDepthStencilState(m_pDepthWriteSkyBox, 0);

	//set vertex buffer
	UINT stride = sizeof(POS_TEX_NORM_VERTEX);
	UINT offset = 0;
	m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//select which primitive type to use 
	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	SKYBOX_CONSTANT_BUFFER cBufferValues;
	XMMATRIX world;
	world = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

	cBufferValues.WorldViewProjection = (world) * (*view) * (*projection);
	m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, 0, &cBufferValues, 0, 0);
	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pImmediateContext->PSSetSamplers(0, 1, &m_pSampler);
	m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTexture);

	m_pImmediateContext->VSSetShader(m_pVShader, 0, 0);
	m_pImmediateContext->PSSetShader(m_pPShader, 0, 0);

	m_pImmediateContext->IASetInputLayout(m_pInputLayout);

	//draw the vertex buffer to the back buffer
	m_pImmediateContext->Draw(36, 0);

	//Reset culling and depth
	m_rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	m_rasterizerDesc.CullMode = D3D11_CULL_BACK;
	hr = m_pD3DDevice->CreateRasterizerState(&m_rasterizerDesc, &m_pRasterSolid);
	m_pImmediateContext->RSSetState(m_pRasterSolid);

	DSDecsc.DepthEnable = true;
	DSDecsc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DSDecsc.DepthFunc = D3D11_COMPARISON_LESS;
	hr = m_pD3DDevice->CreateDepthStencilState(&DSDecsc, &m_pDepthWriteSolid);
	m_pImmediateContext->OMSetDepthStencilState(m_pDepthWriteSolid, 0);
}

void SkyBox::SetPosition(XMVECTOR position)
{
	m_position = position;
}
