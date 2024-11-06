Texture2D shaderTexture : register(t0);
Texture2D shadowMapTexture[2] : register(t1);

SamplerState diffuseSampler : register(s0);
SamplerState shadowSampler : register(s1);

cbuffer LightBufferType : register(b0)
{
    float4 ambient1;
    float4 diffuse1;
    float3 direction1;
    float padding1;
    float4 ambient2;
    float4 diffuse2;
    float3 direction2;
    float padding2;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float4 lightViewPos1 : TEXCOORD1;
    float4 lightViewPos2 : TEXCOORD2;
};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 colour = saturate(diffuse * intensity);
    return colour;
}

// Is the geometry in our shadow map
bool hasDepthData(float2 uv)
{
    if (uv.x < 0.f || uv.x > 1.f || uv.y < 0.f || uv.y > 1.f)
    {
        return false;
    }
    return true;
}

bool isInShadow(Texture2D sMap, float2 uv, float4 lightViewPosition, float bias)
{
    // Sample the shadow map (get depth of geometry)
    float depthValue = sMap.Sample(shadowSampler, uv).r;
	// Calculate the depth from the light.
    float lightDepthValue = lightViewPosition.z / lightViewPosition.w;
    lightDepthValue -= bias;

	// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
    if (lightDepthValue < depthValue)
    {
        return false;
    }
    return true;
}

float2 getProjectiveCoords(float4 lightViewPosition)
{
    // Calculate the projected texture coordinates.
    float2 projTex = lightViewPosition.xy / lightViewPosition.w;
    projTex *= float2(0.5, -0.5);
    projTex += float2(0.5f, 0.5f);
    return projTex;
}

float4 main(InputType input) : SV_TARGET
{
    // Shadow Map Bias
    float shadowMapBias = 0.001f; // Adjusted bias for improved shadow accuracy
    float4 colour = float4(0.f, 0.f, 0.f, 1.f);
    float4 textureColour = shaderTexture.Sample(diffuseSampler, input.tex);

	// Calculate the projected texture coordinates for light 1
    float2 pTexCoord1 = getProjectiveCoords(input.lightViewPos1);
	
    // Shadow test for light 1
    if (hasDepthData(pTexCoord1) && !isInShadow(shadowMapTexture[0], pTexCoord1, input.lightViewPos1, shadowMapBias))
    {
        colour += calculateLighting(-direction1, input.normal, diffuse1);
    }

    // Calculate the projected texture coordinates for light 2
    float2 pTexCoord2 = getProjectiveCoords(input.lightViewPos2);
    
    // Shadow test for light 2
    if (hasDepthData(pTexCoord2) && !isInShadow(shadowMapTexture[1], pTexCoord2, input.lightViewPos2, shadowMapBias))
    {
        colour += calculateLighting(-direction2, input.normal, diffuse2);
    }
    
    // Combine ambient light from both lights
    colour += (saturate(ambient1 + ambient2));

    // Return the final color, modulated by the texture color
    return colour * textureColour;

}
