#include "ModelManager.h"

ModelManager* ModelManager::s_instance;

ModelManager::ModelManager()
{

}


ModelManager::~ModelManager()
{
}

void ModelManager::SetUpDevice(ID3D11Device * device, ID3D11DeviceContext * deviceContext)
{
	m_pD3DDevice = device;
	m_pImmediateContext = deviceContext;
}

Object * ModelManager::LoadModel(char * fileName)
{
	if (m_Models.find(fileName) != m_Models.end())
	{
		return m_Models[fileName];
	}
	else
	{
		m_Models[fileName] = CreateModel(fileName);
		return m_Models[fileName];
	}
}

Object * ModelManager::CreateModel(char * fileName)
{
	HRESULT hr = S_OK;
	Object* newObject = new Object();
	newObject->Model = new ObjFileModel(fileName, m_pD3DDevice, m_pImmediateContext);

	if (newObject->Model->filename == "FILE NOT LOADED")
	{
		return nullptr;
	}

	// create constant buffer
	D3D11_BUFFER_DESC constant_buffer_desc;
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));

	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	constant_buffer_desc.ByteWidth = 112;
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	hr = m_pD3DDevice->CreateBuffer(&constant_buffer_desc, NULL, &newObject->ConstantBuffer);
	if (FAILED(hr))//Return an error code if failed
	{
		return nullptr;
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
			return nullptr;
		}
	}

	hr = D3DX11CompileFromFile("model_shaders.hlsl", 0, 0, "ModelPS", "ps_4_0", 0, 0, 0, &PS, &error, 0);

	if (error != 0)//Check for shader compilation error
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))//Don't fail if error is just a warning
		{
			return nullptr;
		}
	}

	//Create shader objects
	hr = m_pD3DDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &newObject->VertexShader);
	if (FAILED(hr))
	{
		return nullptr;
	}

	hr = m_pD3DDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &newObject->PixelShader);
	if (FAILED(hr))
	{
		return nullptr;
	}

	//Set the shader objects as active
	m_pImmediateContext->VSSetShader(newObject->VertexShader, 0, 0);
	m_pImmediateContext->PSSetShader(newObject->PixelShader, 0, 0);

	//Create and set the input layout object
	D3D11_INPUT_ELEMENT_DESC iedesc[] =
	{
		//Be very careful setting the correct dxgi format and D3D version
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = m_pD3DDevice->CreateInputLayout(iedesc, _ARRAYSIZE(iedesc), VS->GetBufferPointer(), VS->GetBufferSize(), &newObject->InputLayout);
	if (FAILED(hr))
	{
		return nullptr;
	}

	m_pImmediateContext->IASetInputLayout(newObject->InputLayout);
	return newObject;
}

ModelManager * ModelManager::Instance()
{
	if (!s_instance)
	{
		s_instance = new ModelManager();
	}
	return s_instance;
}
