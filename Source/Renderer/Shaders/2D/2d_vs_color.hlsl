struct VS_INPUT
{
    float3 pos : POSITION;
    float3 color : COLOR;
};

struct VS_OUTPUT
{
    float3 pos: SV_POSITION;
    float3 color : COLOR;
};

VS_OUTPUT vs_main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.pos = input.pos;
    output.color = input.color;
    return output;
}