struct VIn
{
    float3 position : POSITION;
    float4 colour : COLOR;
    float2 uv : TEXCOORD;
    float3 norm : NORMAL;
};

struct VOut
{
    float4 position : SV_Position;
    float4 colour : COLOR;
    float2 uv : TEXCOORD0;
    float3 reflectionUVW : TEXCOORD1;
};

struct PointLight
{
    float4 position; // 16 bytes
    float4 colour; // 16 bytes

    float strength; // 4 bytes
    bool enabled; // 4 bytes
    float2 padding; // 8 bytes
	// Total = 48 bytes
};

cbuffer CBuffer0
{
#define MAX_POINT_LIGHTS 8
    
    matrix WVP; //64 bytes
    matrix WV; // 64 bytes
    float4 ambientLightCol; // 16 bytes
    float4 directionalLightDir; // 16 bytes
    float4 directionalLightCol; // 16 bytes
    //float4 pointLightPos; // 16 bytes
    //float4 pointLightCol; // 16 bytes
    //float pointLightStrength; // 4 bytes
    PointLight pointLights[MAX_POINT_LIGHTS]; //384 bytes
};

VOut main(VIn input)
{
    VOut output;
    
    //Position
    //output.position = mul(WVP, input.position);
    output.position = mul(WVP, float4(input.position, 1));
    //Texture coords
    output.uv = input.uv;
    
    //Lighting
    float diffuseAmount = dot(directionalLightDir.xyz, input.norm);
    diffuseAmount = saturate(diffuseAmount);
    float3 directionalFinal = directionalLightCol * diffuseAmount;
    
    //Point Lighting
    //float4 pointLightDir = normalize(pointLightPos - float4(input.position, 1));
    //float pointLightDistance = length(pointLightPos - float4(input.position, 1));
    //float pointLightAttenuation = pointLightStrength / (pointLightDistance * pointLightDistance + pointLightStrength); // A = Strength / d^2 + Strength
    //float pointAmount = dot(pointLightDir.xyz, input.norm) * pointLightAttenuation;
    //pointAmount = saturate(pointAmount);
    //float3 pointFinal = pointLightCol * pointAmount;
    
    float3 pointFinal = float3(0, 0, 0);
    for (int i = 0; i < MAX_POINT_LIGHTS; ++i)
    {
        if (!pointLights[i].enabled)
            continue;
        
        //float4 pointLightDir = normalize(pointLights[i].position - input.position);
        //float pointLightDistance = length(pointLights[i].position - input.position);
        float4 pointLightDir = normalize(pointLights[i].position - float4(input.position, 1));
        float pointLightDistance = length(pointLights[i].position - float4(input.position, 1));
        float pointLightAttenuation = pointLights[i].strength / (pointLightDistance * pointLightDistance + pointLights[i].strength);
        float pointAmount = dot(pointLightDir.xyz, input.norm) * pointLightAttenuation;
        pointAmount = saturate(pointAmount);
        pointFinal += pointLights[i].colour * pointAmount;
        //TODO - create a data structure that enumerates types of lights (directional, point, spot) and all the necessary properties of that light.
        //Then we can have different functions within our shader code that will calculate the light colour contribution for each different light according to its type.
    }
    
    //Vertex position relative to camera
    float3 wvpos = mul(WV, float4(input.position, 1));
    //float3 wvpos = mul(WV, input.position);
    //Surface normal relative to camera
    float3 wvnormal = mul(WV, float4(input.norm, 0));
    //Obtain reverse eye vector
    float3 eyer = -normalize(wvpos);
    //Compute reflection vector
    output.reflectionUVW = 2.0 * dot(eyer, wvnormal) * wvnormal - eyer;
    output.reflectionUVW = mul(float4(output.reflectionUVW, 0), WV);
    
    //Final colour
    output.colour = saturate(ambientLightCol + float4(directionalFinal, 1) + float4(pointFinal, 1));
    //output.colour = float4(pointFinal, 1);
    
    
    return output;
}