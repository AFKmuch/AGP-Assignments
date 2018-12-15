cbuffer CB0
{
    matrix WVPMatrix; //64 bytes
	float4 directional_light_vector; //16 bytes
	float4 directional_light_colour; //16 bytes
	float4 ambient_light_colour; //16 bytes
}; // 112 bytes

Texture2D texture0;
SamplerState sampler0;

struct VOut
{
    float4 position : SV_POSITION;
    float4 color    : COLOR;
    float2 texcoord : TEXCOORD;
    float3 normal : NORMAL;
};

VOut ModelVS(float4 position : POSITION, float2 texcoord : TEXCOORD, float3 normal : NORMAL)
{
    VOut output;

    float4 default_Color    = { 1.0f, 1.0f, 1.0f, 1.0f };
	float diffuse_amount = dot(directional_light_vector, normal);
	diffuse_amount = saturate(diffuse_amount);

    output.position         = mul(WVPMatrix, position);
    output.texcoord         = texcoord;
	output.color = ambient_light_colour + (directional_light_colour * diffuse_amount);

    return output;
}

float4 ModelPS(float4 position : SV_POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD) : SV_Target
{
    return color * texture0.Sample(sampler0, texcoord);
}