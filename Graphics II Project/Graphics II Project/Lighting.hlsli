struct INPUT_PIXEL
{
	float4 projectedCoordinate : SV_POSITION;
	float3 uv : UV;
	float3 normal : NORMAL;
};

texture2D baseTexture : register(t0); // first texture being passed

SamplerState filters : register(s0); // filter 0 using CLAMP, filter 1 using WRAP

// struct for spotlight, pos,color,direction,coneratio 
cbuffer spotLight : register(b0)
{
	float4 spotLightPosition;
	float4 spotLightColor;
	float4 spotLightDirection;
	float4 spotLightConeRatio;
}
// struct for directional light pos,color 
cbuffer	directionalLight : register(b1)
{
	float4 directionalLightDirection;
	float4 directionalLightColr;
}
// struct for point light pos,color,radius 
cbuffer pointLight : register(b2)
{
	float4 pointLightPosition;
	float4 pointLightColor;
	float4 pointLightRadius;
}

float4 main(INPUT_PIXEL input) : SV_TARGET
{
	// get the color from the texture  AKA SURFACE COLOR
	float4 baseColor = baseTexture.Sample(filters, input.uv.xy);


	// directional light formula
	//                                     light direction its facing    surface normal
	float4 DIRECTIONALLIGHTRATIO = CLAMP(DOT(-directionalLightDirection, input.normal));
	//                                light color           surface color
	float4 DIRECTIONALRESULT = DIRECTIONALLIGHTRATIO * directionalLightColor * basecolor;
	//                                       light position         surface position
	float4 POINTLIGHTDIRECTION = NORMALIZE(pointLightPosition - input.projectedCoordinate);
	//                                  direction light shines   surface normal
	float4 POINTLIGHTRATIO = CLMAP(DOT(POINTLIGHTDIRECTION, input.normal));
	//                           light ratio      light color     surface color
	float4 POINTLIGHTRESULT = (POINTLIGHTRATIO * pointLightColor * baseColor);

}