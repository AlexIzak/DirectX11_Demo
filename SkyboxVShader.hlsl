struct VIn
{
    float3 position : POSITION;
    float4 colour : COLOR;
    float2 uv : TEXCOORD;
    float3 norm : NORMAL;
};

struct VOut
{
    float4 position : SV_POSITION;
    float4 colour : COLOR;
    float3 uvw : TEXCOORD;
};

cbuffer CBuffer0
{
    matrix WVP; // 64 bytes
};

VOut main(VIn input)
{
    VOut output;
    
    //Position
    output.position = mul(WVP, float4(input.position, 1));
    //Texture coords
    output.uvw = input.position.xyz;
    
    output.colour = input.colour;
    
	return output;
}