// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/

#include "Loader.h"



bool loadOBJ(const char *path, vector<Vector> & out_verticies, vector<Vector> & out_uvs, vector<Vector> & out_normals)
{
	vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	vector<Vector> temp_vertices, temp_normals, temp_uvs;

	FILE *file = fopen(path, "r");
	if (file == nullptr)
	{
		cout << "Impossible to open file !/n";
		return false;
	}

	while(true)
	{
		char lineHeader[128];
		
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
		{
			break;
		}

		if (strcmp(lineHeader, "v") == 0)
		{
			Vector vertex;
			fscanf(file, "%f %f %f\n", &vertex.pos.x, &vertex.pos.y, &vertex.pos.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			Vector uv;
			fscanf(file, "%f %f\n", &uv.pos.x, &uv.pos.y);
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			Vector normal;
			fscanf(file, "%f %f %f\n", &normal.pos.x, &normal.pos.y, &normal.pos.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
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
			Vector tempVertex = temp_vertices[VertexIndex - 1];
			out_verticies.push_back(tempVertex);
		}
	}

	ofstream bout;
	bout.open(path, ios_base::binary | ios_base::app);
	if (bout.is_open())
	{
		bout.write((char*)&out_verticies, sizeof(Vector));
		bout.close;
	}

}

void main()
{
	
}