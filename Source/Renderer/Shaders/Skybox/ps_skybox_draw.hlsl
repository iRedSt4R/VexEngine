Texture2D cubemapTexture[] : register(t0);
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

float2 DirectionToEquirectangularUV(float3 direction)
{
    float u = atan2(direction.z, direction.x) / (2.0 * 3.14159265359) + 0.5;
    float v = asin(direction.y) / 3.14159265359 + 0.5;
    return float2(u, v);
}

float4 ps_main(VS_OUTPUT input) : SV_TARGET
{
    float2 sampleIV = DirectionToEquirectangularUV(input.texCoord);
    return cubemapTexture[cubemapIndex].Sample(cubemapSampler, sampleIV);
    //return float4(0.f, 0.f, 0.f, 1.f);
}