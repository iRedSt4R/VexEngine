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
    float4 pos: SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD0;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float4 shadowPos : TEXCOORD1;
    float3 viewDirection : TEXCOORD2;
};

cbuffer CameraCB : register(b0)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
    float4x4 invViewProjMatrix;
    float4 worldCameraPosition;
    uint bHaveDiffuseIBL;
	uint diffuseIBLIndex;
	uint bHaveSpecularIBL;
	uint specularIBLIndex;
	uint bHaveBRDFIBL;
	uint BRDFIBLIndex;
};

// Constant Buffers
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

cbuffer DirectionalLightCB : register(b2)
{
    float4x4 dirLightViewMatrix;
	float4x4 dirLightProjMatrix;
    float3 lightDir;
    uint shadowMapIndexInsideHeap;
    float3 lightColor;
    float pad1;
}

VS_OUTPUT vs_main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.pos = float4(input.pos, 1.f);
    output.pos = mul(output.pos, meshWorld);

    output.texCoord = input.texCoord;
    output.normal = (mul(input.normal, (float3x3)meshWorld));
    output.tangent = normalize(mul(input.tangent, (float3x3)meshWorld));
    output.bitangent = mul(input.bitangent, (float3x3)meshWorld);
    //output.tangent = input.tangent;
    //output.bitangent = input.bitangent;
    //output.normal = normalize(output.normal);
    //output.tangent = normalize(output.tangent - dot(output.tangent, normalize(output.normal)) * normalize(output.normal));
    //output.bitangent = cross(output.normal, output.tangent);

	output.viewDirection = worldCameraPosition.xyz - output.pos.xyz;
	output.viewDirection = normalize(output.viewDirection);

    output.pos = mul(output.pos, viewMatrix);
    output.pos = mul(output.pos, projectionMatrix);

    // calculate pixel position in shadow map for comparison
    output.shadowPos = float4(input.pos, 1.f);
    output.shadowPos = mul(output.shadowPos, dirLightViewMatrix);
    output.shadowPos = mul(output.shadowPos, dirLightProjMatrix);

    return output;
}