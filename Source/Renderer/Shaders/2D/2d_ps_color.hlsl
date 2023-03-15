TextureCube  TexAlbedo[] : register(t0); // bindless Texture2D access from shader visible heap
SamplerState BasicSampler : register(s0);
SamplerState ShadowSampler : register(s1);

#define M_PI 3.14159265359f
#define PI 3.14159265359f


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
    float4x4 invViewProjMatrix;
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
	uint bHaveAlbedoTex;
	uint bHaveNormalTex;
	uint bHaveRoughnessTex;
	uint bHaveMetallicTex;
}

float FresnelSchlick(float f0, float fd90, float view)
{
	return f0 + (fd90 - f0) * pow(max(1.0f - view, 0.1f), 5.0f);
}

float chiGGX(float v)
{
	return v > 0 ? 1 : 0;
}

float GGX_Distribution(float3 n, float3 h, float alpha)
{
	float NoH = dot(n, h);
	float alpha2 = alpha * alpha;
	float NoH2 = NoH * NoH;
	float den = NoH2 * alpha2 + (1 - NoH2);
	return (chiGGX(NoH) * alpha2) / (PI * den * den);
}
 
float Disney(float3 N, float3 V, float3 L, float roughness)
{
	float3 halfVector = normalize(L + V);

	float NdotL = saturate(dot(N, L));
	float LdotH = saturate(dot(L, halfVector));
	float NdotV = saturate(dot(N, V));

	float energyBias = lerp(0.0f, 0.5f, roughness);
	float energyFactor = lerp(1.0f, 1.0f / 1.51f, roughness);
	float fd90 = energyBias + 2.0f * (LdotH * LdotH) * roughness;
	float f0 = 1.0f;

	float lightScatter = FresnelSchlick(f0, fd90, NdotL).r;
	float viewScatter = FresnelSchlick(f0, fd90, NdotV).r;

	return lightScatter * viewScatter * energyFactor;
}

float3 GGX(float3 N, float3 V, float3 L, float roughness, float3 specular)
{
	float3 h = normalize(L + V);
	float NdotH = saturate(dot(N, h));

	float rough2 = max(roughness * roughness, 2.0e-3f); // capped so spec highlights don't disappear
	float rough4 = rough2 * rough2;

	float d = (NdotH * rough4 - NdotH) * NdotH + 1.0f;
	float D = rough4 / (3.1415926535897932384626433832795f * (d * d)); //PI TO BE ADDED!!!!

	// Fresnel
	float3 reflectivity = specular;
	float fresnel = 1;
	float NdotL = saturate(dot(N, L));
	float LdotH = saturate(dot(L, h));
	float NdotV = saturate(dot(N, V));
	float3 F = reflectivity + (fresnel - fresnel * reflectivity) * exp2((-5.55473f * LdotH - 6.98316f) * LdotH);

	// geometric / visibility
	float k = rough2 * 0.5f;
	float G_SmithL = NdotL * (1.0f - k) + k;
	float G_SmithV = NdotV * (1.0f - k) + k;
	float G = 0.25f / (G_SmithL * G_SmithV);

	return G * D * F;
}


float3 BRDF(float3 L, float3 V, float3 N, float3 cAlbedo, float pMetallic, float pRoughness)
{
	float3 base_color = cAlbedo;
	float metallic = pMetallic;
	float roughness = pRoughness;

	float3  H = normalize(L + V);
	float dot_n_l = dot(N, L);
	float dot_l_h = dot(L, H);
	float dot_n_h = dot(N, H);
	float dot_n_v = dot(N, V);

	float alpha = roughness * roughness;

	float3 diffuse_color = base_color * (1 - metallic);
	float3 diffuse_brdf = diffuse_color;
	diffuse_brdf *= saturate(dot_n_l * Disney(N, V, L, alpha));

	//Reflections
	float3 reflectionVector = normalize(reflect(-V, N));
	float smoothness = 1 - roughness;
	float mipLevel = (1.0f - smoothness * smoothness);
	float4 cs = float4(0.f, 0.0f, 0.0f, 1.f);
	cs = cs * dot_n_l;

	diffuse_brdf = saturate(lerp(diffuse_brdf, cs.rgb, metallic));
	diffuse_brdf = base_color.rgb * diffuse_brdf;


	
	float specPow = pow((1 - roughness), 4);
	float3 specular_brdf = GGX(N, V, L, roughness, float3(specPow, specPow, specPow));

	float3 final_brdf = (diffuse_brdf + specular_brdf);
	return final_brdf;
	}

float4 ps_main(VS_OUTPUT input) : SV_TARGET
{
    float3 finalColor = 0.f;
    float4 texColor = TexAlbedo[albedoIndexInHeap].Sample(BasicSampler, input.texCoord);
    //float3 baseColor = texColor.xyz * float3(0.03f, 0.03f, 0.03f);

    if(texColor.a < 0.8f)
        discard;

    //finalColor = texColor.xyz * float3(0.03f, 0.03f, 0.03f);
    
    // calculate new normal from normal map if exists
    if(0)
    {
        float4 normalMap = TexAlbedo[normalIndexInHeap].Sample( BasicSampler, input.texCoord );
        //normalMap = pow(normalMap, 1/2.2f);
        normalMap = (2.0f*normalMap) - 1.0f;

        //Make sure tangent is completely orthogonal to normal
        input.tangent = normalize(input.tangent - dot(input.tangent, input.normal)*input.normal);

        //Create the biTangent
        float3 biTangent = cross(input.normal, input.tangent);

        //Create the "Texture Space"
        float3x3 texSpace = float3x3(input.tangent, biTangent, input.normal);

        //Convert normal from normal map to texture space and store in input.normal
        input.normal = normalize(mul(normalMap, texSpace));
    }

    // shadow calc
    float2 projectTexCoord;
    projectTexCoord.x =  input.shadowPos.x / input.shadowPos.w / 2.0f + 0.5f;
    projectTexCoord.y = -input.shadowPos.y / input.shadowPos.w / 2.0f + 0.5f;

    float depthValue = TexAlbedo[shadowMapIndexInsideHeap].Sample(ShadowSampler, projectTexCoord).r;
    float lightDepthValue = input.shadowPos.z / input.shadowPos.w;
    lightDepthValue = lightDepthValue - 0.001f;

    //if(depthValue < lightDepthValue)
    //{
        //return float4(finalColor, 1.f);
    //}

    // BRDF ---------------------------------------------------------------------------------
    float roughness = TexAlbedo[roughnessIndexInHeap].Sample( BasicSampler, input.texCoord ).g;
    roughness = pow(roughness, 1/2.8f);
    float metalness = TexAlbedo[roughnessIndexInHeap].Sample( BasicSampler, input.texCoord ).r;
    metalness = pow(metalness, 1/2.8f);
    float3 L = lightDir;
    float3 V = normalize(worldCameraPosition - input.pos.rgb);
    float3 N = normalize(input.normal);
    finalColor = BRDF(L, V, N, texColor.rgb, metalness, roughness);
    // BRDF ---------------------------------------------------------------------------------

    //return float4(TexAlbedo[normalIndexInHeap].Sample( BasicSampler, input.texCoord ).rgb, 1.f);
    return float4(finalColor, 1.f);
}