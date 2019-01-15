cbuffer CB0
{
	matrix WVPMatrix; //64 bytes
	float4 color; //16 bytes
}; // 80 bytes

Texture2D texture0;
SamplerState sampler0;

struct VOut
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
};

VOut VShader(float4 position : POSITION)
{
	VOut output;
	
	output.position = mul(WVPMatrix, position);
	output.color = color;
    output.texcoord = (position.xy / 2)+0.5f;   // this is dirty but adding texcoords to the shader messed up 
                                                // the mesh for the particle its self and would only draw one
                                                // half for literally no reason, as I copied the mesh from a
                                                // working project.
	return output;
}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD) : SV_Target
{
	//float distsq = texcoord.x * texcoord.x + texcoord.y * texcoord.y;
	//clip(1.0f - distsq);
	return color * texture0.Sample(sampler0, texcoord);
}