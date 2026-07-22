#include "../Common/Pixel/PixelShader2DHeader.hlsli"

// 定数バッファ：スロット4番目(b4と書く)
cbuffer cbParam : register(b4)
{
    float4 g_color;
}

float4 main(PS_INPUT PSInput) : SV_TARGET
{

	// 元画像
    float4 color = tex.Sample(texSampler, PSInput.uv);

    // RGBを反転
    color.rgb = 1.0f - color.rgb;

    return color;

}
