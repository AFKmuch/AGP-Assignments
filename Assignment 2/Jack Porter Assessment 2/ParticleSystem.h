#pragma once
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
// defines allows more compatible code, precede all xnamath includes with these defines
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>
#include <math.h>
#include <random>
#include <list>
#include "Time.h"

struct PARTICLE_CONSTANT_BUFFER
{
	XMMATRIX WorldViewProjection; // 64 Bytes
	XMFLOAT4 color;
}; // 80 bytes

static enum PARTICLE_EFFECT_TYPE
{
	STREAM,
	FOUNTAIN,
	SNOW
};

struct Particle
{
	float						gravity;
	XMVECTOR					position;
	XMVECTOR					velocity;
	XMFLOAT4					color;
	ID3D11ShaderResourceView*	texture;
	ID3D11SamplerState*			sampler;
	float						scale;
	float						maxLife;
	float						currentLife;
};

struct POS_TEX_VERT
{
	XMFLOAT3 Position;
	XMFLOAT2 TexCoord;
};

class ParticleSystem
{

private:
	ID3D11RasterizerState *		m_pRasterSolid = 0;
	ID3D11RasterizerState *		m_pRasterParticle = 0;
	ID3D11DepthStencilState*	m_pDepthWriteSolid = 0;
	ID3D11DepthStencilState*	m_pDepthWriteParticle = 0;
	D3D11_RASTERIZER_DESC		m_rasterizerDesc;

	ID3D11Device*				m_pD3DDevice;
	ID3D11DeviceContext*		m_pImmediateContext;

	ID3D11Buffer*				m_pVertexBuffer;
	ID3D11VertexShader*			m_pVShader;
	ID3D11PixelShader*			m_pPShader;
	ID3D11InputLayout*			m_pInputLayout;
	ID3D11Buffer*				m_pConstantBuffer;
	ID3D11BlendState*			m_pAlphaBlendEnabled;
	ID3D11BlendState*			m_pAlphaBlendDisabled;

	PARTICLE_CONSTANT_BUFFER		m_particle_cb_values;

	std::list<Particle*> m_free;
	std::list<Particle*> m_active;

	static ParticleSystem*		s_instance;
public:
	ParticleSystem();
	~ParticleSystem();

	void SetUpParticleSystem(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	HRESULT CreateParticle();
	HRESULT LoadShader();
	HRESULT Draw(XMMATRIX* view, XMMATRIX* projection, XMVECTOR* cameraPosition);
	HRESULT DrawOne(Particle* one, XMMATRIX* view, XMMATRIX* projection, XMVECTOR* cameraPosition);
	HRESULT AddTexture(char * filename, Particle* particle);
	void LoadEffect(PARTICLE_EFFECT_TYPE type, XMVECTOR startPos, XMVECTOR forward);
	
	static ParticleSystem* Instance();
};

