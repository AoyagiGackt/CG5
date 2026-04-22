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
    float gray = dot(color.rgb, float3(0.2125f, 0.7154f, 0.0721f));
    return float4(gray, gray, gray, color.a);
}
