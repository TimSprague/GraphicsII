#include "Model.h"


Model::Model()
{

}

Model::Model(float3 _pos, float3 _uv, float3 _normal, float3 _color)
{
	/*pos = _pos;
	uv = _uv;
	normal = _normal;
	color = _color;*/
}


Model::~Model()
{
}

bool Model::loadOBJ(const char *path, vector<float3> & out_verticies, vector<float3> & out_uvs, vector<float3> & out_normals)
{
	vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	vector<float3> temp_vertices, temp_normals, temp_uvs;

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
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			float3 uv;
			fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			float3 normal;
			fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf_s(file, "%d/%d%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9)
			{
				cout << "File can't be read by our simple parser: (Try exporting with other options\n";
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
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
	}

	//ofstream bout;
	//bout.open(path, ios_base::binary | ios_base::app);
	//if (bout.is_open())
	//{
	//	bout.write((char*)&out_verticies, sizeof(Vector));
	//	bout.close;
	//}

}
