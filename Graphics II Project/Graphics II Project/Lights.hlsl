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
float4 magnitude(float4 pos1)
{
	return sqrt(pos1.x*pos1.x + pos1.y*pos1.y + pos1.z*pos1.z + pos1.w *pos1.w);
}

float4 main(INPUT_PIXEL input) : SV_TARGET
{
	// get the color from the texture  AKA SURFACE COLOR
	float4 baseColor = baseTexture.Sample(filters, input.uv.xy);
	float4 ambient = float4(baseColor.xyzw * 0.75f);

	// directional light formula
	//                                     light direction its facing    surface normal
	float DIRECTIONALLIGHTRATIO = clamp(dot(-directionalLightDirection, input.normal),0,1);
	//                                light color           surface color
	float4 DIRECTIONALRESULT = DIRECTIONALLIGHTRATIO * directionalLightColor * baseColor;
	////                                       light position         surface position
	//float4 POINTLIGHTDIRECTION = NORMALIZE(pointLightPosition - input.projectedCoordinate);
	////                                  direction light shines   surface normal
	//float4 POINTLIGHTRATIO = CLMAP(DOT(POINTLIGHTDIRECTION, input.normal));
	////                           light ratio      light color     surface color
	//float4 POINTLIGHTRESULT = (POINTLIGHTRATIO * pointLightColor * baseColor);
	////                                      light position        surface position
	float3 SPOTLIGHTDIRECTION = normalize(spotLightPosition.xyz - input.worldPosition.xyz);
	//                                          light direction        cone direction
	float SPOTLIGHTSURFACERATIO = clamp(dot(-SPOTLIGHTDIRECTION.xyz, spotLightConeDirection.xyz), 0, 1);
	//                               surface ratio           cone ratio
	float SPOTLIGHTSPOTFACTOR = (SPOTLIGHTSURFACERATIO > spotLightConeRatio.x) ? 1 : 0;
	//                                  light direction   surface normal
	float SPOTLIGHTRATIO = clamp(dot(SPOTLIGHTDIRECTION, input.normal), 0, 1);
	//                        spot light factor     light ratio     light color  surface color
	float4 SPOTLIGHTRESULT = SPOTLIGHTSPOTFACTOR * SPOTLIGHTRATIO * spotLightColor *baseColor;
	//                                            
	//float POINTLIGHTATTENUATION = 1.0 - clamp(magnitude(pointLightPosition - input.projectedCoordinate) / 0.5f, 0, 1);
	////
	//float SPOTLIGHTATTENUATION1 = 1.0 - clamp(magnitude(spotLightPosition - input.projectedCoordinate) / 0.5f, 0, 1);
	////
	//float SPOTLIGHTATTENUATION2 = 1.0 - clamp((0.001f - SPOTLIGHTSURFACERATIO) / (0.001f - 0.999), 0, 1);
	////
	//float TOTALATTENUATION = SPOTLIGHTATTENUATION1 * SPOTLIGHTATTENUATION2;

	return saturate(float4(SPOTLIGHTRESULT.xyz/* + ambient.xyz*/, baseColor.a));//saturate(DIRECTIONALRESULT + SPOTLIGHTRESULT + baseColor);

}

