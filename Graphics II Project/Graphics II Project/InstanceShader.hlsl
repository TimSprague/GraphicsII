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

cbuffer INSTANCES : register(b2)
{
	float4x4 arrays[3];
}

OUTPUT_VERTEX main(INPUT_VERTEX fromVertexBuffer, uint id : SV_InstanceID)
{
	OUTPUT_VERTEX sendToRasterizer = (OUTPUT_VERTEX)0;
	float4 localPos = float4 (fromVertexBuffer.coordinate, 1);

	// uses the arrays world matrix's stored
	localPos = mul(localPos, arrays[id]);
	
	// append to the pixelshader for the world position of the object
	sendToRasterizer.worldPosition = localPos.xyz;

	localPos = mul(localPos, vewMatrix);
	localPos = mul(localPos, projectionMatrix);
	
	sendToRasterizer.projectedCoordinate = localPos;
	sendToRasterizer.uv = fromVertexBuffer.uv;

	// the w must be a 0 so that it rotates the normals on the local position and not in the world
	sendToRasterizer.normal = mul(float4(fromVertexBuffer.normal, 0), arrays[id]).xyz;
	//sendToRasterizer.normal = fromVertexBuffer.normal;
	// END PART 3

	return sendToRasterizer;
}