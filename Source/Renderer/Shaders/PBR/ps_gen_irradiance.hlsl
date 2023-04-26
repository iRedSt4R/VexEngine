Texture2D inputTexture[] : register(t0);
SamplerState inputSampler : register(s0);

#define PI 3.14159265359f

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

static const float3 DefNormals[6] = {
    float3(1, 0, 0), float3(-1, 0, 0),
    float3(0, 1, 0), float3(0, -1, 0),
    float3(0, 0, 1), float3(0, 0, -1)
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


float2 directionToEquirectangular(float3 direction)
{
    float u = atan2(direction.z, direction.x) / (2.0 * 3.14159265359) + 0.5;
    float v = asin(direction.y) / 3.14159265359 + 0.5;
    return float2(u, v);
}

// float3 color = inputTexture[cubemapIndex].Sample(inputSampler, uv).rgb;
float3 convoluteIrradiance(float3 normal, uint seed)
{
    float3 irradiance = float3(0, 0, 0);
    const uint sampleCount = 16;
    float weightSum = 0.0;

    for (uint i = 0; i < sampleCount; ++i)
    {
        // Generate random numbers
        float2 rand = float2(float(i % 7 + 1) / 7.0, float(i / 7 + 1) / 7.0) * float(seed % 7 + 1) / 7.0;
        float phi = rand.x * 2.0 * 3.14159265359;
        float cosTheta = 1.0 - rand.y;
        float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

        // Calculate direction vector
        float3 direction;
        direction.x = sinTheta * cos(phi);
        direction.y = cosTheta;
        direction.z = sinTheta * sin(phi);

        // Transform direction to tangent space
        direction = normalize(direction.x * cross(normal, float3(0, 1, 0)) + direction.y * normal + direction.z * cross(normal, float3(0, 1, 0)));

        float2 uv = directionToEquirectangular(direction);
        float3 color = inputTexture[cubemapIndex].Sample(inputSampler, uv).rgb;

        // Accumulate irradiance
        irradiance += color * cosTheta;
        weightSum += cosTheta;
    }

    irradiance /= weightSum;
    return irradiance;
}

//COLOR[n]
struct PS_OUT
{
    float4 outColor : SV_Target;
    //uint outSlice : SV_RenderTargetArrayIndex;
};


PS_OUT ps_main(VS_OUTPUT input)
{
    PS_OUT outData;
    float3 direction = computeDirection(input.texcoord, rootSliceIndex);
    float3 normal = normalize(direction);

    uint seed = rootSliceIndex * 7 + 1;
    float3 irradiance = convoluteIrradiance(normal, seed);
    
    outData.outColor = float4(irradiance, 1.0);
    return outData;
}