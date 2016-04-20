#include "Model.h"


Model::Model()
{

}

Model::~Model()
{
}

bool Model::loadOBJ(const char *path, vector<float3> & out_verticies, vector<float3> & out_uvs, vector<float3> & out_normals)
{
	vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	vector<float3> temp_vertices, temp_normals, temp_uvs;
	bool unique;

	FILE *file = fopen(path, "r");
	if (file == nullptr)
	{
		cout << "Impossible to open file !/n";
		return false;
	}

	while (true)
	{
		char lineHeader[128];

		int res = fscanf_s(file, "%s", lineHeader,128);
		if (res == EOF)
		{
			break;
		}

		if (strcmp(lineHeader, "v") == 0)
		{
			float3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			//vertex.x = -vertex.x;
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			float3 uv;
			fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = 1.0f - uv.y;
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			float3 normal;
			fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			//normal.x = -normal.x;
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9)
			{
				cout << "File can't be read by our simple parser: (Try exporting with other options\n";
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
	}
	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{
		unsigned int VertexIndex = vertexIndices[i];
		float3 tempVertex = temp_vertices[VertexIndex - 1];
		out_verticies.push_back(tempVertex);
	}
	for (unsigned int i = 0; i < uvIndices.size(); i++)
	{
		unsigned int uvIndex = uvIndices[i];
		float3 tempUV = temp_uvs[uvIndex - 1];
		out_uvs.push_back(tempUV);
	}
	for (unsigned int i = 0; i < normalIndices.size(); i++)
	{
		unsigned int normalsIndex = normalIndices[i];
		float3 tempNormal = temp_normals[normalsIndex - 1];
		out_normals.push_back(tempNormal);
	}
	vector<float3> tempUnique;
	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{
		unsigned int vertexIndex = vertexIndices[i];
		float3 tempVertex = temp_vertices[vertexIndex - 1];
		unique = true;
		for (unsigned int j = 0; j < uniqueVerts.size(); j++)
		{
			float3 uniqueIndex = tempUnique[j];
			// check for like positions and then
			if (tempVertex.x == uniqueIndex.x && tempVertex.y == uniqueIndex.y && tempVertex.z == uniqueIndex.z)
			{
				// set unique to false
				unique = false;
				// push the current value into the index buffer 
				uniqueIndexBuffer.push_back(j);
				// break to go to the next pos in the list
				break;
			}
		}

		if (unique)
		{
			// add the current value into the index buffer that is pointing to the point inside /the /uniqueVert to find the correct pos
			uniqueIndexBuffer.push_back(uniqueVerts.size());
			tempUnique.push_back(tempVertex);
			// build unique verticies vector
			vertex_Normal tempUniqueVertex;
			tempUniqueVertex.pos = temp_vertices[vertexIndices[i] - 1];
			tempUniqueVertex.uv = temp_uvs[uvIndices[i] - 1];
			tempUniqueVertex.normal = temp_normals[normalIndices[i] - 1];
			uniqueVerts.push_back(tempUniqueVertex);
		}
	}

}
