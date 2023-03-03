Texture2D TexAlbedo : register(t0);
SamplerState BasicSampler : register(s0);

struct VS_OUTPUT
{
    float4 pos: SV_POSITION;
    float2 texCoord : TEXCOORD;
};

cbuffer CameraCB : register(b0)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
    float4 worldCameraPosition;
};

float4 ps_main(VS_OUTPUT input) : SV_TARGET
{
    float4 texColor = TexAlbedo.Sample(BasicSampler, input.texCoord);

    if(texColor.a < 0.8f)
        discard;
    //return float4(float3(0.7f, 0.2f, 0.4f), 1.f);
    return float4(texColor.xyz, 1.f);
}