Texture2D TexAlbedo : register(t0);
SamplerState BasicSampler : register(s0);

struct VS_OUTPUT
{
    float4 pos: SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

cbuffer CameraCB : register(b0)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
    float4 worldCameraPosition;
};

cbuffer DirectionalLightCB : register(b2)
{
    float3 lightDir;
    float pad0;
    float3 lightColor;
    float pad1;
}

float4 ps_main(VS_OUTPUT input) : SV_TARGET
{
    float4 texColor = TexAlbedo.Sample(BasicSampler, input.texCoord);

    if(texColor.a < 0.8f)
        discard;


    float3 finalColor = 0.f;

    finalColor = texColor.xyz * float3(0.02f, 0.02f, 0.02f);
    finalColor += saturate(dot(lightDir, input.normal) * lightColor * texColor.xyz);

    //return float4(float3(0.7f, 0.2f, 0.4f), 1.f);
    return float4(finalColor, 1.f);
}