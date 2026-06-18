#pragma once

static char const* g_defaultShaderSource = R"(
cbuffer CameraConstants : register(b2)
{
    float4x4 WorldToCameraTransform;
    float4x4 CameraToRenderTransform;
    float4x4 RenderToClipTransform;
};

cbuffer ModelConstants : register(b3)
{
    float4x4 ModelToWorldTransform;
    float4   ModelColor;
};

Texture2D diffuseTexture : register(t0);
SamplerState diffuseSampler : register(s0);

struct vs_input_t
{
    float3 modelSpacePosition : POSITION;
    float4 color              : COLOR;
    float2 uv                 : TEXCOORD;
};

struct v2p_t
{
    float4 clipSpacePosition  : SV_Position;
    float4 color              : COLOR;
    float2 uv                 : TEXCOORD;
};

v2p_t VertexMain(vs_input_t input)
{
    v2p_t v2p;

    float4 modelPos = float4(input.modelSpacePosition, 1.0f);
    float4 worldPos  = mul(ModelToWorldTransform, modelPos);
    float4 cameraPos = mul(WorldToCameraTransform, worldPos);
    float4 renderPos = mul(CameraToRenderTransform, cameraPos);
    v2p.clipSpacePosition = mul(RenderToClipTransform, renderPos);

    v2p.color = input.color * ModelColor;
    v2p.uv = input.uv;
    return v2p;
}

float4 PixelMain(v2p_t input) : SV_Target0
{
    float4 textureColor = diffuseTexture.Sample(diffuseSampler, input.uv);
	float4 vertexColor = input.color;
	float4 color = textureColor * vertexColor;
	clip(color.a - 0.01f);
	return float4(color);
}
)";