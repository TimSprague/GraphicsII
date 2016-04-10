struct INPUT_PIXEL
{
	float4 projectedCoordinate : SV_POSITION;
	float3 uv : UV;
	float3 normal : NORMAL;
};

texture2D baseTexture : register(t0); // first texture being passed

SamplerState filters : register(s0); // filter 0 using CLAMP, filter 1 using WRAP

// struct for spotlight, pos,color,direction,coneratio 
struct SPOT_LIGHT 
{
	float4 positionCoordinate: SV_POSITION;
	float4 color : COLOR;
	float4 direction : SV_DIRECTION;
	float4 coneRatio : RATIO;
};

// struct for directional light pos,color 
struct DIRECTIONAL_LIGHT 
{
	float4 direction :SV_DIRECTION;
	float4 color: COLOR;
};

// struct for point light pos,color,radius 
struct POINT_LIGHT
{
	float4 positionCoordinate : SV_POSITION;
	float4 color: COLOR;
	float4 radius: RADIUS;
};

cbuffer spotLight : register(b0)
{

}
cbuffer	directionalLight : register(b1)
{

}
cbuffer pointLight : register(b2)
{

}

float4 main(INPUT_PIXEL) : SV_TARGET
{
	// directional light formula
	//                              light direction its facing    surface normal
	float4 LIGHTRATIO = CLAMP(DOT(-DIRECTIONAL_LIGHT.direction,INPUT_PIXEL.normal));
	//                                lightcolor           surfacecolor
	float4 RESULT = LIGHTRATIO * DIRECTIONAL_LIGHT.color * SURFACECOLOR;
}