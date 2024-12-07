Texture2D shaderTexture : register(t0);
Texture2D shadowMapTexture[5] : register(t1);

SamplerState diffuseSampler : register(s0);
SamplerState shadowSampler : register(s1);

cbuffer LightBufferType : register(b0)
{
    //Feels like there should be a better way of doing this, havent figured out yet
    float4 ambient1;
    float4 diffuse1;
    float3 direction1;
    float padding1;
    
    float4 ambient2;
    float4 diffuse2;
    float3 position2;
    float padding2;
    
    float4 diffuse3;
    float3 position3;
    float range3;
    float3 direction3;
    float innerCone3;
    float outerCone3;
    float3 attenuation3;
    //float padding3;
    
    float4 diffuse4;
    float3 position4;
    float range4;
    float3 direction4;
    float innerCone4;
    float outerCone4;
    float3 attenuation4;
    //float padding4;
    
    float4 diffuse5;
    float3 position5;
    float range5;
    float3 direction5;
    float innerCone5;
    float outerCone5;
    float3 attenuation5;
    //float padding5;
    
    float4 diffuse6;
    float3 position6;
    float range6;
    float3 direction6;
    float innerCone6;
    float outerCone6;
    float3 attenuation6;
    //float padding6;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float4 lightViewPos1 : TEXCOORD1;
    float4 lightViewPos2 : TEXCOORD2;
    float4 lightViewPos3 : TEXCOORD3;
    float4 lightViewPos4 : TEXCOORD4;
    float4 lightViewPos5 : TEXCOORD5;
    float4 lightViewPos6 : TEXCOORD6;
    float3 worldPosition : TEXCOORD7;
    float3 viewVector : TEXCOORD8;
};

//Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 colour = saturate(diffuse * intensity);
    return colour;
}

float4 calculatePointLighting(float3 lightDirection, float3 normal, float4 colour)
{
    float diff = max(dot(normalize(normal), normalize(lightDirection)), 0.0);
    //float4 colour = saturate(diffuse * intensity);
    return colour * diff;
}

float4 calculateSpotlight(float3 lightPosition, float3 lightDirection, float3 worldPosition, float3 normal, float4 diffuse, float spotlightInnerCone, float spotlightOuterCone, float3 attenuation, float range)
{
    float3 lightVector = lightPosition - worldPosition;
    float distance = length(lightVector);
    lightVector = normalize(lightVector);

    // Spotlight cone check
    float spotFactor = saturate(dot(lightVector, -lightDirection)); // -lightDirection points outward
    float spotlightIntensity = smoothstep(spotlightOuterCone, spotlightInnerCone, spotFactor);

    // Attenuation
    float attenuationFactor = 1.0 / (attenuation.x + attenuation.y * distance + attenuation.z * distance * distance);
    attenuationFactor *= saturate(1.0 - (distance / range));

    // Diffuse lighting
    float intensity = saturate(dot(normal, lightVector));
    float4 diffuseLight = diffuse * intensity * spotlightIntensity * attenuationFactor;

    return diffuseLight;
}

float4 calculateSpecular(float3 lightDirection, float3 normal, float3 viewVector, float4 specColour, float specPower){
    float3 halfway = normalize(lightDirection + viewVector);
    float specIntensity = pow(max(dot(normal, halfway), 0.0), specPower);
    
    return saturate(specColour * specIntensity);

}

//Is the geometry in our shadow map
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
    //Sample the shadow map (get depth of geometry)
    float depthValue = sMap.Sample(shadowSampler, uv).r;
	//Calculate the depth from the light.
    float lightDepthValue = lightViewPosition.z / lightViewPosition.w;
    lightDepthValue -= bias;

	//Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
    if (lightDepthValue < depthValue)
    {
        return false;
    }
    return true;
}

float2 getProjectiveCoords(float4 lightViewPosition)
{
    //Calculate the projected texture coordinates.
    float2 projTex = lightViewPosition.xy / lightViewPosition.w;
    projTex *= float2(0.5, -0.5);
    projTex += float2(0.5f, 0.5f);
    return projTex;
}

float4 main(InputType input) : SV_TARGET
{
    //Shadow Map Bias
    float shadowMapBias = 0.005f;
    float4 colour = float4(0.f, 0.f, 0.f, 1.f);
    float4 textureColour = shaderTexture.Sample(diffuseSampler, input.tex);

	//Calculate the projected texture coordinates for light 1
    float2 pTexCoord1 = getProjectiveCoords(input.lightViewPos1);
	
    //Shadow test for light 1

    if (hasDepthData(pTexCoord1) && !isInShadow(shadowMapTexture[0], pTexCoord1, input.lightViewPos1, shadowMapBias))
    {
        colour += calculateLighting(-direction1, input.normal, diffuse1);
    }

    float3 lightVector = normalize(position2 - input.worldPosition);
    colour += calculatePointLighting(lightVector, input.normal, diffuse2);
    
    float2 pTexCoord3 = getProjectiveCoords(input.lightViewPos3);
    if (hasDepthData(pTexCoord3) && !isInShadow(shadowMapTexture[1], pTexCoord3, input.lightViewPos3, shadowMapBias))
    {
        colour += calculateSpotlight(position3, direction3, input.worldPosition, input.normal, diffuse3, innerCone3, outerCone3, attenuation3, range3);
    }
    
    float2 pTexCoord4 = getProjectiveCoords(input.lightViewPos4);
    if (hasDepthData(pTexCoord4) && !isInShadow(shadowMapTexture[1], pTexCoord4, input.lightViewPos4, shadowMapBias))
    {
        colour += calculateSpotlight(position4, direction4, input.worldPosition, input.normal, diffuse4, innerCone4, outerCone4, attenuation4, range4);
    }
    
    float2 pTexCoord5 = getProjectiveCoords(input.lightViewPos5);
    if (hasDepthData(pTexCoord5) && !isInShadow(shadowMapTexture[1], pTexCoord5, input.lightViewPos5, shadowMapBias))
    {
        colour += calculateSpotlight(position5, direction5, input.worldPosition, input.normal, diffuse5, innerCone5, outerCone5, attenuation5, range5);
    }
    
    float2 pTexCoord6 = getProjectiveCoords(input.lightViewPos6);
    if (hasDepthData(pTexCoord6) && !isInShadow(shadowMapTexture[1], pTexCoord6, input.lightViewPos6, shadowMapBias))
    {
        colour += calculateSpotlight(position6, direction6, input.worldPosition, input.normal, diffuse6, innerCone6, outerCone6, attenuation6, range6);
    }
    
    
    //Combine ambient light from both lights
    colour += (saturate(ambient1));

    //Return the final color and texture
    return colour * textureColour;
    //return float4(input.normal, 1);
    

}


//float4 main(InputType input) : SV_TARGET
//{
//    //Shadow Map Bias
//    float shadowMapBias = 0.001f;
//    float4 colour = float4(0.f, 0.f, 0.f, 1.f);
//    float4 textureColour = shaderTexture.Sample(diffuseSampler, input.tex);
//	//Calculate the projected texture coordinates for light 1
//    float2 pTexCoord1 = getProjectiveCoords(input.lightViewPos1);
//    float3 viewVector = normalize(input.viewVector);

//    float3 normal = normalize(input.normal);
    
//    float4 pointLightDiffuse;
//    float3 toPointLight;
//    float4 pointlightSpecular;
	
//    //Shadow test for light 1
//    if (hasDepthData(pTexCoord1) && !isInShadow(shadowMapTexture[0], pTexCoord1, input.lightViewPos1, shadowMapBias))
//    {
//        colour += calculateLighting(-direction1, input.normal, diffuse1);
//    }

//    //Calculate the projected texture coordinates for light 2
//    float2 pTexCoord2 = getProjectiveCoords(input.lightViewPos2);
    
//    //Shadow test for light 2
//    //toPointLight = normalize(position2 - input.worldPosition);
//   //pointLightDiffuse = calculatePointLighting(toPointLight, normal, diffuse2);
//    //pointlightSpecular = calculateSpecular(toPointLight, normal, viewVector, float4(0.0, 0.0, 0.0, 1), specularPower);
//    float3 lightVector = normalize(position2 - input.worldPosition);
//    if (hasDepthData(pTexCoord2) && !isInShadow(shadowMapTexture[1], pTexCoord2, input.lightViewPos2, shadowMapBias))
//    {
//        colour += calculateLighting(lightVector, input.normal, diffuse2);
//    }
    
//    //toPointLight = normalize(position2 - input.worldPosition);
//    //pointLightDiffuse = calculatePointLighting(toPointLight, normal, diffuse2);
//    //pointlightSpecular = calculateSpecular(toPointLight, normal, viewVector, float4(0.1, 0.1, 0.1, 1), specularPower);
//    //if (hasDepthData(pTexCoord2) && !isInShadow(shadowMapTexture[1], pTexCoord2, input.lightViewPos2, shadowMapBias))
//    //{
        
//    //    //colour += calculateLighting(-direction2, input.normal, diffuse2);
//    //    colour += (pointLightDiffuse + diffuse1 + ambient1);
//    //    //float3 lightVector = normalize(position2 - input.lightViewPos2.xyz);
//    //    //colour += calculateLighting(lightVector, input.normal, diffuse2);
//    //}
    
//    //colour = saturate(pointLightDiffuse + diffuse1 + ambient1);
    
//    //Combine ambient light from both lights
//    colour += (saturate(ambient1));

//    //Return the final color and texture
//    return (colour * textureColour);
//    //return float4(input.normal, 1);
//}