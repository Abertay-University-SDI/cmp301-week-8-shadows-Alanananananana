cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewMatrix;
    matrix lightProjectionMatrix;
    matrix lightViewMatrix2;
    matrix lightProjectionMatrix2;
    matrix lightViewMatrix3;
    matrix lightProjectionMatrix3;
    matrix lightViewMatrix4;
    matrix lightProjectionMatrix4;
    matrix lightViewMatrix5;
    matrix lightProjectionMatrix5;
    matrix lightViewMatrix6;
    matrix lightProjectionMatrix6;
};

Texture2D heightMap : register(t2); // Height map texture
SamplerState heightSampler : register(s2);

cbuffer CameraBuffer : register(b1)
{
    float3 cameraPosition;
    float padding;
}

cbuffer HeightMapBuffer : register(b2)
{
    float mapHeight;
    float3 padding2;
}

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
    float4 lightViewPos3 : TEXCOORD3;
    float4 lightViewPos4 : TEXCOORD4;
    float4 lightViewPos5 : TEXCOORD5;
    float4 lightViewPos6 : TEXCOORD6;
    float3 worldPosition : TEXCOORD7;
    float3 viewVector : TEXCOORD8;
};

float GetHeight(float2 uv)
{
    float height = heightMap.SampleLevel(heightSampler, uv, 0).r;
    return height * mapHeight; //Scale the height
}

float3 CalculateNormals(float2 uv)
{
    float offset = 0.01;
    
    //finding offset point from first point along x-axis
    float2 xOffsetPoint = float2(uv.x + offset, uv.y);
    
    //finding the difference between the 2 points' height to calc tangent only along x axis -- knows how much to slope
    float3 tangent = float3(1, GetHeight(xOffsetPoint) - GetHeight(uv), 0);

    //finding offset point from first point along z-axis
    float2 zOffsetPoint = float2(uv.x, uv.y + offset);
    
    //finding the difference between the 2 points' height to calc bitangent 
    float3 bitangent = float3(0, GetHeight(zOffsetPoint) - GetHeight(uv), 1);

    //cross product of tangent and bitangent = the normal
    return normalize(cross(bitangent, tangent));
}

OutputType main(InputType input)
{
    OutputType output;

    // Calculate vertex displacement
    float height = GetHeight(input.tex);
    input.position.y += height;

    // Calculate world position
    float4 worldPosition = mul(input.position, worldMatrix);
    output.worldPosition = worldPosition.xyz;

    // Transform vertex position to view and projection spaces
    output.position = mul(worldPosition, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Calculate shadow positions for light 1 and light 2
    output.lightViewPos1 = mul(worldPosition, lightViewMatrix);
    output.lightViewPos1 = mul(output.lightViewPos1, lightProjectionMatrix);

    output.lightViewPos2 = mul(worldPosition, lightViewMatrix2);
    output.lightViewPos2 = mul(output.lightViewPos2, lightProjectionMatrix2);
    
    output.lightViewPos3 = mul(worldPosition, lightViewMatrix3);
    output.lightViewPos3 = mul(output.lightViewPos3, lightProjectionMatrix3);

    output.lightViewPos4 = mul(worldPosition, lightViewMatrix4);
    output.lightViewPos4 = mul(output.lightViewPos4, lightProjectionMatrix4);

    output.lightViewPos5 = mul(worldPosition, lightViewMatrix5);
    output.lightViewPos5 = mul(output.lightViewPos5, lightProjectionMatrix5);

    output.lightViewPos6 = mul(worldPosition, lightViewMatrix6);
    output.lightViewPos6 = mul(output.lightViewPos6, lightProjectionMatrix6);

    // Pass texture coordinates and transformed normals
    output.tex = input.tex;
    output.normal = mul(input.normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);
    
    output.normal = CalculateNormals(input.tex);
    
    output.viewVector = cameraPosition.xyz - worldPosition.xyz;
    output.viewVector = normalize(output.viewVector);

    return output;
}
