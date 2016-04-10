#pragma once
#include <algorithm>
#include <math.h>
#include <vector>
#include <cmath>
#include <iostream>
#include "SharedDefines.h"
#include <DirectXMath.h>
#include <atlbase.h>
#include <Windows.h>
#include <fstream>

using namespace DirectX;
using namespace std;

struct Vector
{
	float3 pos;
};

bool loadOBJ(const char *path, vector<Vector> & out_verticies, vector<Vector> & out_uvs, vector<Vector> & out_normals)

