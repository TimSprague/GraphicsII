#pragma pack_matrix(row_major)
struct INPUT_VERTEX
{
	float3 position: POSITION;
};

struct OUTPUT_VERTEX
{
	float4 projectedCoord : SV_POSITION;
	float3 uv : UV;
};

cbuffer OBJECT : register(b0)
{
	float4x4 worldMatrix;
}

cbuffer SCENE : register(b1)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
}

OUTPUT_VERTEX main( INPUT_VERTEX fromVertexBuffer)
{
	OUTPUT_VERTEX sendToRasterizer = (OUTPUT_VERTEX)0;

	float4 LocalPos = float4(fromVertexBuffer.position, 1);
	LocalPos = mul(LocalPos, worldMatrix);
	LocalPos = mul(LocalPos, viewMatrix);
	LocalPos = mul(LocalPos, projectionMatrix);

	sendToRasterizer.projectedCoord =LocalPos;
	sendToRasterizer.uv = fromVertexBuffer.position.xyz;

	return sendToRasterizer;
}