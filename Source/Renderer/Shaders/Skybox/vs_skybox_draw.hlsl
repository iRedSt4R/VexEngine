struct VS_INPUT
{
    float3 pos : POSITION;
};

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

VS_OUTPUT vs_main(VS_INPUT input)
{
    VS_OUTPUT output;

    output.pos = mul(float4(input.pos, 1.0f), worldViewProjection).xyww;
    output.texCoord = input.pos;

    return output;
}