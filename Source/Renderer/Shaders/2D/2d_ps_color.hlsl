Texture2D TexAlbedo[] : register(t0); // bindless Texture2D access from shader visible heap
SamplerState BasicSampler : register(s0);
SamplerState ShadowSampler : register(s1);

#define M_PI 3.14159265359f
#define PI 3.14159265359f

float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

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

float4 ps_main(VS_OUTPUT input) : SV_TARGET
{
    float3 finalColor = 0.f;
    float4 texColor = TexAlbedo[albedoIndexInHeap].Sample(BasicSampler, input.texCoord);
   

    if(texColor.a < 0.8f)
        discard;

    //finalColor = texColor.xyz * float3(0.03f, 0.03f, 0.03f);
    
    // calculate new normal from normal map if exists
    if(bHaveNormalTex == 1)
    {
        float3 Normal = normalize(input.normal);
        float3 Tangent = normalize(input.tangent);
        Tangent = normalize(Tangent - dot(Tangent, input.texCoord) * Normal);
        float3 Binormal = cross(Tangent, Normal);
        float3 BumpMapNormal = TexAlbedo[normalIndexInHeap].Sample(BasicSampler, input.texCoord);
        BumpMapNormal = pow(BumpMapNormal, 1/2.2f);
        BumpMapNormal = (2.0f * BumpMapNormal) - 1.0f;
        float3 NewNormal;
        float3x3 TBN = float3x3(Tangent, Binormal, Normal);
        NewNormal = mul(BumpMapNormal, TBN);
        NewNormal = normalize(NewNormal);
        input.normal = NewNormal;
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
   // }
    //finalColor += saturate(dot(lightDir, input.normal) * lightColor * texColor.xyz);
    //float roughness = TexAlbedo[roughnessIndexInHeap].Sample(BasicSampler, input.texCoord).g;
    //roughness = pow(roughness, 1/2.2f);
    //float metallic = TexAlbedo[roughnessIndexInHeap].Sample(BasicSampler, input.texCoord).r;
    //roughness = pow(roughness, 1/2.2f);
    float metallic = 0.0f;
    float roughness = 0.2f;
    // BRDF ---------------------------------------------------------------------------------
    float3 N = normalize(input.normal);
    float3 V = normalize(worldCameraPosition - input.pos);

    float3 F0 = float3(0.04f, 0.04f, 0.04f); 
    F0 = lerp(F0, texColor.xyz, metallic);
	           
    // reflectance equation
    float3 Lo = float3(0.0f, 0.0f, 0.0f);
    // calculate per-light radiance
    float3 L = normalize(-lightDir);
    float3 H = normalize(V + L);
    float3 radiance  = lightColor;        
    
    // cook-torrance brdf
    float NDF = DistributionGGX(N, H, roughness);        
    float G   = GeometrySmith(N, V, L, roughness);      
    float3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       
    
    float3 kS = F;
    float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
    kD *= 1.0 - metallic;	  
    
    float3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    float3 specular     = numerator / denominator;  
        
    // add to outgoing radiance Lo
    float NdotL = max(dot(N, L), 0.0);                
    Lo += (kD * texColor.xyz / PI + specular) * radiance * NdotL;  
  
    float3 ambient = float3(0.03f, 0.03f, 0.03f) * texColor.xyz; //  * ao (from ao map if exists)
    float3 color = ambient + Lo;
	
    color = color / (color + float3(1.0f, 1.0f, 1.0f));
    //color = pow(color, 1.0/2.2);  
   
    // BRDF ---------------------------------------------------------------------------------

    return float4(color, 1.f);
}