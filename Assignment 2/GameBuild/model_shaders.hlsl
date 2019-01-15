cbuffer CB0
{
    matrix WVPMatrix; //64 bytes
	float4 directional_light_vector; //16 bytes
	float4 directional_light_colour; //16 bytes
	float4 ambient_light_colour; //16 bytes
    float4 camera_position; //16
}; // 128 bytes

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
	float diffuse_amount    = dot(directional_light_vector, normal);
	diffuse_amount          = saturate(diffuse_amount);

    output.position         = mul(WVPMatrix, position);
    output.texcoord         = texcoord;
	output.color            = ambient_light_colour + (directional_light_colour * diffuse_amount);
    output.normal = normalize(mul(WVPMatrix, normal));

    return output;
}

float4 ModelPS(float4 position : SV_POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD, float3 normal : NORMAL) : SV_Target
{
    float4 finalColour = color;
    float4 default_Color = { 1.0f, 1.0f, 1.0f, 1.0f };
    float4 ToEye = camera_position - position;
    ToEye = normalize(ToEye);
    float3 HalfWay = normalize(directional_light_vector - ToEye);
    float NDotH = saturate(dot(HalfWay, normal));
    finalColour += default_Color * pow(NDotH, 2) * 2;
    
    return finalColour * texture0.Sample(sampler0, texcoord);
}