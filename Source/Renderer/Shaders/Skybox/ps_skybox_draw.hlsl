TextureCube cubemapTexture[] : register(t0);
SamplerState cubemapSampler : register(s0);

struct VS_OUTPUT
{
    float4 pos: SV_POSITION;
    float3 texCoord : TEXCOORD0;
};

cbuffer cubemapCB : register(b0)
{
    float4x4 worldViewProjection;
    uint cubemapIndex;
};

float4 ps_main(VS_OUTPUT input) : SV_TARGET
{
    return cubemapTexture[cubemapIndex].Sample(cubemapSampler, input.texCoord);
    //return float4(0.f, 0.f, 0.f, 1.f);
}