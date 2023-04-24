Texture2D inputTexture[] : register(t0);
SamplerState inputSampler : register(s0);

static const float3 directions[6] = {
    float3(1, 0, 0), float3(-1, 0, 0),
    float3(0, 1, 0), float3(0, -1, 0),
    float3(0, 0, 1), float3(0, 0, -1)
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

cbuffer cubemapCB : register(b0)
{
    float4x4 worldViewProjection;
    uint cubemapIndex;
    uint sliceIndex;
};

cbuffer rootCB : register(b1)
{
    uint rootSliceIndex;
};

float3 computeDirection(float2 uv, uint faceIndex)
{
    float u = uv.x * 2.0 - 1.0;
    float v = uv.y * 2.0 - 1.0;

    float3 direction;

    if (faceIndex == 0) // Positive X
        direction = float3(1, -v, -u);
    else if (faceIndex == 1) // Negative X
        direction = float3(-1, -v, u);
    else if (faceIndex == 2) // Positive Y
        direction = float3(u, 1, v);
    else if (faceIndex == 3) // Negative Y
        direction = float3(u, -1, -v);
    else if (faceIndex == 4) // Positive Z
        direction = float3(u, -v, 1);
    else // Negative Z
        direction = float3(-u, -v, -1);

    return normalize(direction);
}
//COLOR[n]
struct PS_OUT
{
    float4 outColor : SV_Target;
    //uint outSlice : SV_RenderTargetArrayIndex;
};

float2 DirectionToEquirectangularUV(float3 direction)
{
    float u = atan2(direction.z, direction.x) / (2.0 * 3.14159265359) + 0.5;
    float v = asin(direction.y) / 3.14159265359 + 0.5;
    return float2(u, v);
}

PS_OUT ps_main(VS_OUTPUT input)
{
    PS_OUT outStruct;
    //float2 uv = input.texcoord;
    //float4 color = inputTexture[cubemapIndex].Sample(inputSampler, uv);
    float3 direction = computeDirection(input.texcoord, rootSliceIndex);
    float2 uv = DirectionToEquirectangularUV(direction);
    float4 color = inputTexture[cubemapIndex].Sample(inputSampler, uv);
    

    outStruct.outColor = color;
    //outStruct.outSlice = sliceIndex;
    return outStruct;
}