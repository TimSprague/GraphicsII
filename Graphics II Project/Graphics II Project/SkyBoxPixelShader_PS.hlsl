struct INPUT_PIXEL
{
	float4 projectedCoord : SV_POSITION;
	float3 uv : UV;
};

TextureCube baseTexture : register(t0); // texture for the cube

SamplerState filters : register(s0);

float4 main(INPUT_PIXEL input) : SV_TARGET
{
	float4 baseColor = baseTexture.Sample(filters, input.uv);

	return baseColor;
}