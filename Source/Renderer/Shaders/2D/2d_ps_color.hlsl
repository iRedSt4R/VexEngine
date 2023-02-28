struct VS_OUTPUT
{
    float3 pos: SV_POSITION;
    float3 color : COLOR;
};

float4 ps_main(VS_OUTPUT input)
{
    return float4(input.color, 1.f);
}