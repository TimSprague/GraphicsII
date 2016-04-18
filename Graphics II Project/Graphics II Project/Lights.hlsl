struct INPUT_PIXEL
{
	float4 projectedCoordinate : SV_POSITION;
	float3 uv : UV;
	float3 normal : NORMAL;
	float3 worldPosition : WORLD_POSITION;
};

texture2D baseTexture : register(t0); // first texture being passed

SamplerState filters : register(s0); // filter 0 using CLAMP, filter 1 using WRAP

									 // struct for spotlight, pos,color,direction,coneratio 
cbuffer spotLight : register(b0)
{
	float4 spotLightPosition;
	float4 spotLightColor;
	float4 spotLightConeDirection;
	float4 spotLightConeRatio;
	matrix spotWorldMatrix;
}
// struct for directional light pos,color 
cbuffer	directionalLight : register(b1)
{
	float4 directionalLightDirection;
	float4 directionalLightColor;
	matrix directionalWorldMatrix;
}
// struct for point light pos,color,radius 
cbuffer pointLight : register(b2)
{
	float4 pointLightPosition;
	float4 pointLightColor;
	float4 pointLightRadius;
	matrix pointWorldMatrix;
}

float4 main(INPUT_PIXEL input) : SV_TARGET
{
	// get the color from the texture  AKA SURFACE COLOR
	float4 baseColor = baseTexture.Sample(filters, input.uv.xy);
	float4 ambient = float4(baseColor.xyzw * 0.25f);

	// directional light formula
	//                                     light direction its facing    surface normal
	float DIRECTIONALLIGHTRATIO = clamp(dot(-directionalLightDirection.xyz, normalize(input.normal)),0,1);
	//                                light color           surface color
	float4 DIRECTIONALRESULT = DIRECTIONALLIGHTRATIO * directionalLightColor * baseColor;
	//                                       light position         surface position
	float4 POINTLIGHTDIRECTION = float4(normalize(pointLightPosition.xyz - input.worldPosition.xyz),1);
	//                                  direction light shines   surface normal
	float POINTLIGHTRATIO = clamp(dot(POINTLIGHTDIRECTION.xyz, normalize(input.normal)),0,1);
	//                           light ratio      light color     surface color
	float4 POINTLIGHTRESULT = (POINTLIGHTRATIO * pointLightColor * baseColor);
	//                                                      light position           surface position          light radius
	float POINTLIGHTATTENUATION = 1.0 - saturate(length(pointLightPosition.xyz - input.worldPosition.xyz) / pointLightRadius.x);
	//
	POINTLIGHTRESULT = POINTLIGHTRESULT * POINTLIGHTATTENUATION;
	//                                      light position        surface position
	float3 SPOTLIGHTDIRECTION = normalize(spotLightPosition.xyz - input.worldPosition.xyz);
	//                                          light direction        cone direction
	float SPOTLIGHTSURFACERATIO = clamp(dot(-SPOTLIGHTDIRECTION.xyz, spotLightConeDirection.xyz), 0, 1);
	//                               surface ratio           cone ratio
	float SPOTLIGHTSPOTFACTOR = (SPOTLIGHTSURFACERATIO > spotLightConeRatio.x) ? 1 : 0;
	//                                  light direction   surface normal
	float SPOTLIGHTRATIO = clamp(dot(SPOTLIGHTDIRECTION, normalize(input.normal)), 0, 1);
	//                        spot light factor     light ratio     light color  surface color
	float4 SPOTLIGHTRESULT = SPOTLIGHTSPOTFACTOR * SPOTLIGHTRATIO * spotLightColor *baseColor;
	//                                                   light position           surface position         light radius
	float SPOTLIGHTATTENUATION1 = 1.0 - saturate(length(spotLightPosition.xyz - input.worldPosition.xyz) / 10);
	//                                               innerconeration           surfaceratio             innerconeration         outerconeratio
	float SPOTLIGHTATTENUATION2 = 1.0 - saturate( (spotLightConeRatio.x - SPOTLIGHTSURFACERATIO) / (spotLightConeRatio.x - spotLightConeRatio.y) );
	//
	//float TOTALATTENUATION = SPOTLIGHTATTENUATION1 * SPOTLIGHTATTENUATION2;   // gives a return token error
	//
	SPOTLIGHTRESULT = /*TOTALATTENUATION*/SPOTLIGHTRESULT * (SPOTLIGHTATTENUATION1 * SPOTLIGHTATTENUATION2);

	return saturate(float4(POINTLIGHTRESULT.xyz + DIRECTIONALRESULT.xyz + SPOTLIGHTRESULT.xyz + baseColor.rgb, baseColor.a));

}

