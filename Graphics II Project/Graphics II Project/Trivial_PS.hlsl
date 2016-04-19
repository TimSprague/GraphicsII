//#include "Header.hlsli"

// from vertex shader to pixel shader, same order as the vertex shader
struct INPUT_PIXEL
{
	float4 projectedCoordinate : SV_POSITION;
	float3 uv : UV;
	float3 normal : NORMAL;
	float3 worldPosition : WORLD_POSITION;

};

texture2D baseTexture : register(t0); // first texture

//texture2D detailTexture : register(t1); // second texture

SamplerState filters : register(s0); // filter 0 using CLAMP, filter 1 using WRAP

// Pixel shader performing multi-texturing with a detail texture on a second UV channel
// A simple optimization would be to pack both UV sets into a single register
float4 main(INPUT_PIXEL input): SV_TARGET
{
	// green red alpha blue = grab
	//float4 baseColor = baseTexture.Sample(filters, input.uv.xy).grab; // get base color
	float4 baseColor = baseTexture.Sample(filters, input.uv.xy); // get base color

	//float4 detailColor = detailTexture.Sample(filters[1], detailUV); // get detail effect
	//float4 finalColor = float4(lerp(baseColor.rgb, detailColor.rgb, detailColor.a), baseColor.a);

	return baseColor; // return a transition based on the detail alpha
}
