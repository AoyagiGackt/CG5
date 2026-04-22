Texture2D<float4> gTexture : register(t0);
SamplerState      gSampler : register(s0);

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

float4 main(PSInput input) : SV_TARGET
{
    float4 color = gTexture.Sample(gSampler, input.uv);

    // Vignetting
    float2 offset = input.uv - 0.5f;
    float vignette = 1.0f - dot(offset, offset) * 2.5f;
    vignette = saturate(vignette);
    vignette = smoothstep(0.0f, 1.0f, vignette);
    color.rgb *= vignette;

    return color;
}
