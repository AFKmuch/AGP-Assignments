cbuffer CB0 : register(b0)
{
	matrix WVPMatrix;
    float4 directional_light_vector;
    float4 directional_light_colour;
    float4 ambient_light_colour;
};

//cbuffer CBufferPos : register(b1)
//{
//    float3 offset;
//    float offsetPacking;
//};
//
//
//cbuffer CBufferScale : register(b2)
//{
//    float3 scale;
//    float scalePacking;
//};

Texture2D texture0;
SamplerState sampler0;

struct VOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR; //Note the spelling of this and all instances below
    float2 texcoord : TEXCOORD;
    float3 normal : NORMAL;
};

VOut VShader(float4 position : POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD, float3 normal : NORMAL)
{
    VOut output;

	//color.r *= red_fraction;
    //position.x *= scale.x;
    //position.y *= scale.y;
	//position.x += offset.x;
	//position.y += offset.y;
	output.position = mul(WVPMatrix, position);

    float diffuse_amount = dot(directional_light_vector, normal);

    diffuse_amount = saturate(diffuse_amount);

    output.color = ambient_light_colour + (directional_light_colour * diffuse_amount);

    output.texcoord = texcoord;

	return output;
}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD) : SV_TARGET
{
    return color * texture0.Sample(sampler0, texcoord);
}
