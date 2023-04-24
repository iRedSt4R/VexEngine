struct VS_INPUT
{
    float2 position : POSITION;
    float2 texcoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

VS_OUTPUT vs_main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = float4(input.position, 0, 1);
    output.texcoord = input.texcoord;
    return output;
}