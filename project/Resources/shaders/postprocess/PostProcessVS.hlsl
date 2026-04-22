struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

// SV_VertexID だけで画面全体を覆う三角形を1枚生成する
VSOutput main(uint vid : SV_VertexID)
{
    float2 uv = float2((vid << 1) & 2, vid & 2);
    VSOutput o;
    o.pos = float4(uv * 2.0f - 1.0f, 0.0f, 1.0f);
    o.uv  = float2(uv.x, 1.0f - uv.y);
    return o;
}
