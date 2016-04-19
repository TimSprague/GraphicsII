#pragma pack_matrix(row_major)

struct INPUT_VERTEX
{
	float3 coordinate : POSITION;
	float3 uv : UV;
	float3 normal : NORMAL;
};

struct OUTPUT_VERTEX
{
	float4 projectedCoordinate : SV_POSITION;
	float3 uv : UV;
	float3 normal : NORMAL;
	float3 worldPosition : WORLD_POSITION;
};

cbuffer OBJECT : register(b0)
{
	float4x4 worldMatrix;
}
cbuffer SCENE : register(b1)
{
	float4x4 vewMatrix;
	float4x4 projectionMatrix;
}

OUTPUT_VERTEX main( INPUT_VERTEX fromVertexBuffer )
{
	OUTPUT_VERTEX sendToRasterizer = (OUTPUT_VERTEX)0;
	/*sendToRasterizer.projectedCoordinate.w = 1;
	
	sendToRasterizer.projectedCoordinate.xy = fromVertexBuffer.coordinate.xy;*/

	//  temp pos from the passed in vertexbuffer and expanding from xyz to xyzw
	float4 localPos = float4 (fromVertexBuffer.coordinate, 1);

	localPos = mul(localPos, worldMatrix);
	// append to the pixelshader for the world position of the object
	sendToRasterizer.worldPosition = localPos.xyz;

	localPos = mul(localPos, vewMatrix);
	localPos = mul(localPos, projectionMatrix);
	// TODO : PART 4 STEP 4
	/*sendToRasterizer.projectedCoordinate.xy += constantOffset;*/
	
	// TODO : PART 3 STEP 7
	sendToRasterizer.projectedCoordinate = localPos;
	sendToRasterizer.uv = fromVertexBuffer.uv;
	// the w must be a 0 so that it rotates the normals on the local position and not in the world
	sendToRasterizer.normal = mul(float4(fromVertexBuffer.normal, 0), worldMatrix).xyz;
	//sendToRasterizer.normal = fromVertexBuffer.normal;
	// END PART 3

	return sendToRasterizer;
}