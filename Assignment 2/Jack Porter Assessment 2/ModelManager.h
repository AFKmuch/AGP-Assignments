#pragma once
#include "objfilemodel.h"
#include <map>

struct Object
{
	ObjFileModel*				Model;
	ID3D11VertexShader*			VertexShader;
	ID3D11PixelShader*			PixelShader;
	ID3D11InputLayout*			InputLayout;
	ID3D11Buffer*				ConstantBuffer;
};

struct TextureStruct
{
	ID3D11ShaderResourceView*	Texture;
	ID3D11SamplerState*			Sampler;
};

class ModelManager
{
private:
	static ModelManager*		s_instance;

	ID3D11Device*				m_pD3DDevice;
	ID3D11DeviceContext*		m_pImmediateContext;

	std::map<char*, Object*> m_Models;
	std::map<char*, TextureStruct*> m_Textures;

public:
	ModelManager();
	~ModelManager();

	void SetUpDevice(ID3D11Device * device, ID3D11DeviceContext * deviceContext);
	Object* LoadModel(char* fileName);
	TextureStruct* LoadTexture(char* fileName);

	Object* CreateModel(char* fileName);
	TextureStruct* CreateTexture(char* fileName);

	static ModelManager* Instance();
};