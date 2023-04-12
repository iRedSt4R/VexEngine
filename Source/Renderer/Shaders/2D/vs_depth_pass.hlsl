struct VS_INPUT
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
};

cbuffer CameraCB : register(b0)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
};

cbuffer ModelCB : register(b1)
{
    float4x4 meshWorld;
    uint albedoIndexInHeap;
	uint normalIndexInHeap;
	uint roughnessIndexInHeap;
	uint metallicIndexInHeap;
	bool bHaveAlbedoTex;
	bool bHaveNormalTex;
	bool bHaveRoughnessTex;
	bool bHaveMetallicTex;
};


VS_OUTPUT vs_main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.pos = float4(input.pos, 1.f);
    output.pos = mul(output.pos, meshWorld);
    output.pos = mul(output.pos, viewMatrix);
    output.pos = mul(output.pos, projectionMatrix);

    return output;
}