Texture2D TexAlbedo[] : register(t0, space0); // bindless Texture2D access from shader visible heap
TextureCube cubemapTexture[] : register(t1, space1); // same srv heap as above, but bind as textureCube (direct srvHeap indexing available in shader model 6.6 is not yet widely supported)
SamplerState BasicSampler : register(s0);
SamplerState ShadowSampler : register(s1);
SamplerState cubemapSampler : register(s2);

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
	float3 viewDirection : TEXCOORD2;
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
    int cubemapIndex;
}

cbuffer MeshDataCB : register(b1)
{
    float4x4 meshWorld;
	float3 materialColor;
	float materialRoughness;
	float materialMetalness;
    uint albedoIndexInHeap;
	uint normalIndexInHeap;
	uint roughnessIndexInHeap;
	uint metallicIndexInHeap;
	uint bHaveAlbedoTex;
	uint bHaveNormalTex;
	uint bHaveRoughnessTex;
	uint bHaveMetallicTex;
}

float3 F_Schlick (in float3 f0 , in float f90 , in float u )
{
	return f0 + ( f90 - f0 ) * pow (1.f - u , 5.f);
}

float V_SmithGGXCorrelated ( float NdotL , float NdotV , float alphaG )
{
	float alphaG2 = alphaG * alphaG ;
	// Caution : the " NdotL *" and " NdotV *" are explicitely inversed , this is not a mistake .
	float Lambda_GGXV = NdotL * sqrt (( - NdotV * alphaG2 + NdotV ) * NdotV + alphaG2 );
	float Lambda_GGXL = NdotV * sqrt (( - NdotL * alphaG2 + NdotL ) * NdotL + alphaG2 );
	return 0.5f / ( Lambda_GGXV + Lambda_GGXL );
}

float D_GGX ( float NdotH , float m )
{
	// Divide by PI is apply later
	float m2 = m * m ;
	m2 = max(m2, 0.0001);
	float f = ( NdotH * m2 - NdotH ) * NdotH + 1;
	return m2 / (f * f) ;
}

float Fr_DisneyDiffuse ( float NdotV , float NdotL , float LdotH , float linearRoughness )
{
	float energyBias = lerp (0 , 0.5 , linearRoughness );
	float energyFactor = lerp (1.0 , 1.0 / 1.51 , linearRoughness );
	float fd90 = energyBias + 2.0f * LdotH * LdotH * linearRoughness;
	float3 f0 = float3 (1.0f , 1.0f , 1.0f);
	float lightScatter = F_Schlick ( f0 , fd90 , NdotL ).r;
	float viewScatter = F_Schlick (f0 , fd90 , NdotV ).r;

	return energyFactor * lightScatter * viewScatter;
 }


float3 BRDF(float3 L, float3 V, float3 N, float3 cAlbedo, float pMetallic, float pRoughness)
{
	float3 base_color = cAlbedo;
	float metallic = pMetallic;
	float roughness = pRoughness;

	float3 H = normalize (V + L);
	float dot_n_l = dot(N, L);
	float dot_l_h = dot(L, H);
	float dot_n_h = dot(N, H);
	float dot_n_v = dot(N, V);

	float alpha = roughness * roughness;

	float NdotV = abs( dot (N , V )); // avoid artifact
	float LdotH = saturate ( dot (L , H ));
	float NdotH = saturate ( dot (N , H ));
	float NdotL = saturate ( dot (N , L ));

	float energyBias = lerp(0.0f, 0.5f, roughness);
	float energyFactor = lerp(1.0f, 1.0f / 1.51f, roughness);
	float fd90 = energyBias + 2.0f * (LdotH * LdotH) * roughness;
	//float fd90 = 0.f;
	//float f0 = 1.f;
	float3 f0 = float3(0.04f, 0.04f, 0.04f);
	f0 = lerp(f0, cAlbedo.rgb, metallic);
	
	// Specular BRDF
	float3 F = F_Schlick (f0 , fd90 , LdotH );
	float Vis = V_SmithGGXCorrelated ( NdotV , NdotL , roughness );
	float D = D_GGX ( NdotH , roughness );
	float Fr = D * F * Vis / PI ;

	float3 specColor = float3(1.f, 1.f, 1.f);
	specColor = lerp(specColor, cAlbedo.rgb, metallic);

	// Diffuse BRDF
	float Fd = Fr_DisneyDiffuse ( NdotV , NdotL , LdotH , roughness * roughness  ) / PI;

	// reflection
	float3 reflectionVector = normalize(reflect(-V, N));
	float smoothness = 1 - roughness;
	float mipLevel = (1.0f - smoothness * smoothness) * 10.0f;
	float4 cs = cubemapTexture[cubemapIndex].SampleLevel(cubemapSampler, reflectionVector, mipLevel);

 	float3 kS = F;
    float3 kD = float3(1.0, 1.0, 1.0) - kS;
    kD *= 1.0 - metallic;	  

	//abledo = saturate(lerp(abledo, cs.rgb, metallic));
	//abledo = saturate(abledo.rgb + cs.rgb);
	float3 albedoColor = cAlbedo * kD;
	//float3 diffuseWithRef = ((Fd * cAlbedo * 1.2f *  NdotL) + (Fr * float3(1.f, 1.f, 1.f) * NdotL));
	//return saturate((Fd * cAlbedo * NdotL) + (Fr * float3(1.f, 1.f, 1.f))) * NdotL;
	//* float3(0.9f, 0.75f, 0.4f)
	return ((albedoColor + Fr * specColor * 1.f) * Fd * 18.f * float3(0.9f, 0.75f, 0.4f) * NdotL);
	//return cs.rgb * NdotL;
	//return cs.rgb;
}

float4 ps_main(VS_OUTPUT input) : SV_TARGET
{
	float dX = 1.f/1920.f;
	float dY = 1.f/1080.f;
    float3 finalColor = 0.f;
	
    float4 texColor;
	if(bHaveAlbedoTex)
		texColor = TexAlbedo[albedoIndexInHeap].Sample(BasicSampler, input.texCoord);
	else
		texColor = float4(materialColor, 1.f);

    finalColor = texColor.xyz * float3(0.05f, 0.05f, 0.05f);

    if(texColor.a < 0.8f)
        discard;

	float3 newNormal = input.normal;
    // calculate new normal from normal map if exists
    if(bHaveNormalTex)
    {
        float3 normalMap = (TexAlbedo[normalIndexInHeap].Sample( BasicSampler, input.texCoord ).xyz * 2.0f) - 1.0f;
    	float3x3 TBN = float3x3(input.tangent, input.bitangent, input.normal);
		input.normal = normalize(mul(normalMap, TBN));
		//input.normal = 	normalWorldSpace;	
		newNormal = input.normal;																																																																						
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
    //    return float4(finalColor, 1.f);
    //}

	float finalRoughness = materialRoughness;
	if(bHaveRoughnessTex)
		finalRoughness = TexAlbedo[roughnessIndexInHeap].Sample(BasicSampler, input.texCoord).g;

	float finalMetallics = materialMetalness;
	if(bHaveMetallicTex)
		finalMetallics = TexAlbedo[roughnessIndexInHeap].Sample(BasicSampler, input.texCoord).r;
	

    // BRDF ---------------------------------------------------------------------------------
    float3 L = lightDir;
    float3 V = input.viewDirection;
    float3 N = normalize(input.normal);
    finalColor = (BRDF(L, V, N, texColor.rgb, 1.f, min(finalRoughness+0.001f, 1.f)));
	//finalColor = finalColor + BRDF(L, V, N, texColor.rgb, 0.f, 0.1f);
    // BRDF ---------------------------------------------------------------------------------

    //return float4(TexAlbedo[normalIndexInHeap].Sample( BasicSampler, input.texCoord ).rgb, 1.f);
    return float4(finalColor, 1.f);
}