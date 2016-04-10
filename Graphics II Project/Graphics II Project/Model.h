#pragma once
#include "Defines.h"

class Model
{
	

public:
	vector<float3> pos;
	vector<float3> uv;
	vector<float3> normal;
	vector<float3> color;

	Model();
	Model(float3 _pos, float3 _uv, float3 _normal, float3 _color);
	~Model();


	bool loadOBJ(const char *path, vector<float3> & out_verticies, vector<float3> & out_uvs, vector<float3> & out_normals);
};

