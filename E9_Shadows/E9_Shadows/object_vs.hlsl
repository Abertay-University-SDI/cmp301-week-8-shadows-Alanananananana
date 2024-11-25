cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewMatrix;
    matrix lightProjectionMatrix;
    matrix lightViewMatrix2;
    matrix lightProjectionMatrix2;
};

Texture2D heightMap : register(t2); // Height map texture
SamplerState heightSampler : register(s2);

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float4 lightViewPos1 : TEXCOORD1;
    float4 lightViewPos2 : TEXCOORD2;
};


float GetHeight(float2 uv)
{
    // Sample the height map texture using SampleLevel to avoid unsupported operations in vertex shaders
    float height = heightMap.SampleLevel(heightSampler, uv, 0).r;
    return height * 10.0f; // Scale the height appropriately
}

OutputType main(InputType input)
{
    OutputType output;

    // Calculate vertex displacement
    float height = GetHeight(input.tex);
    input.position.y += height;

    // Calculate world position
    float4 worldPosition = mul(input.position, worldMatrix);

    // Transform vertex position to view and projection spaces
    output.position = mul(worldPosition, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Calculate shadow positions for light 1 and light 2
    output.lightViewPos1 = mul(worldPosition, lightViewMatrix);
    output.lightViewPos1 = mul(output.lightViewPos1, lightProjectionMatrix);

    output.lightViewPos2 = mul(worldPosition, lightViewMatrix2);
    output.lightViewPos2 = mul(output.lightViewPos2, lightProjectionMatrix2);

    // Pass texture coordinates and transformed normals
    output.tex = input.tex;
    output.normal = mul(input.normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);

    return output;
}
