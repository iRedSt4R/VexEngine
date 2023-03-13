Texture2D TexAlbedo[] : register(t0); // bindless Texture2D access from shader visible heap
SamplerState BasicSampler : register(s0);
SamplerState ShadowSampler : register(s1);

struct VS_OUTPUT
{
    float4 pos: SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD0;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float4 shadowPos : TEXCOORD1;
};

cbuffer CameraCB : register(b0)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
    float4 worldCameraPosition;
};

cbuffer DirectionalLightCB : register(b2)
{
    float4x4 dirLightViewMatrix;
	float4x4 dirLightProjMatrix;
    float3 lightDir;
    uint shadowMapIndexInsideHeap;
    float3 lightColor;
    float pad1;
}

cbuffer MeshDataCB : register(b1)
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
}

float4 ps_main(VS_OUTPUT input) : SV_TARGET
{
    float4 texColor = TexAlbedo[albedoIndexInHeap].Sample(BasicSampler, input.texCoord);

    if(texColor.a < 0.8f)
        discard;

    float3 finalColor = 0.f;

    finalColor = texColor.xyz * float3(0.01f, 0.01f, 0.01f);
    


    // shadow calc
    float2 projectTexCoord;
    projectTexCoord.x =  input.shadowPos.x / input.shadowPos.w / 2.0f + 0.5f;
    projectTexCoord.y = -input.shadowPos.y / input.shadowPos.w / 2.0f + 0.5f;

    float depthValue = TexAlbedo[shadowMapIndexInsideHeap].Sample(ShadowSampler, projectTexCoord).r;
    float lightDepthValue = input.shadowPos.z / input.shadowPos.w;
    lightDepthValue = lightDepthValue - 0.001f;

    if(depthValue < lightDepthValue)
    {
        return float4(finalColor, 1.f);
    }

    finalColor += saturate(dot(lightDir, input.normal) * lightColor * 2.f * texColor.xyz);

    //return float4(float3(0.7f, 0.2f, 0.4f), 1.f);
    return float4(finalColor, 1.f);
}