cbuffer CB0
{
	matrix WVPMatrix; //64 bytes
};

TextureCube cube0;
SamplerState sampler0;

struct VOut
{
	float4 position : SV_POSITION;
	float4 color    : COLOR;
	float3 texcoord : TEXCOORD;
	float3 normal : NORMAL;
};

VOut VShader(float4 position : POSITION, float3 texcoord : TEXCOORD, float3 normal : NORMAL)
{
	VOut output;

	float4 default_Color = { 1.0f, 1.0f, 1.0f, 1.0f };

	output.texcoord = texcoord;
	output.position = mul(WVPMatrix, position);
	output.color = default_Color;

	return output;
}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR, float3 texcoord : TEXCOORD) : SV_Target
{
	return color * cube0.Sample(sampler0, texcoord);
}