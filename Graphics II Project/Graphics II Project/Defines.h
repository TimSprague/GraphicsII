#pragma once
#include <iostream>
#include <Windows.h>
#include <ctime>
#include <cmath>
#include "XTime.h"
#include <DirectXMath.h>
#include <atlbase.h>
#include <algorithm>
#include <vector>
#include "SharedDefines.h"
using namespace DirectX;
using namespace std;


#define RASTER_HEIGHT 500
#define RASTER_WIDTH 500
#define RASTER_WIDTH_HALF (RASTER_WIDTH/2)
#define RASTER_HEIGHT_HALF (RASTER_HEIGHT/2)
#define NUMPIXELS ((RASTER_WIDTH)*(RASTER_HEIGHT))
#define PI 3.14159f
#define FOV 65
#define AspectRatio ((float)RASTER_WIDTH/RASTER_HEIGHT)
#define NearPlane 0.1f
#define FarPlane 100.0f

// point x,y
struct Points
{
	float x;
	float y;
};

// vector x,y,z
struct Vector
{
	float x;
	float y;
	float z;
	float w;
};

// array 0=x,1=y,2=z,3=w
struct Vertex
{
	float xyzw[4];
	float color;
	float u, v;
};

struct SCREEN_XY
{
	unsigned int x;
	unsigned int y;
};
// a pixel
struct  A_PIXEL
{
	float color;
};

struct RGBA
{
	float a = 1.0f;
	float r = 0.0f;
	float g = 0.2f;
	float b = 0.4f;
	
};

struct SIMPLE_VERTEX
{
	float posXYZ[3];
	float rgba[4];
};

struct SEND_TO_RAM
{
	float constantColor[4];
	float constantOffset[2];
	float padding[2];
};

struct OBJECT_TO_VRAM
{
	XMMATRIX worldMatrix;
};

struct SCENE_TO_VRAM
{
	XMMATRIX viewMatrix, projectionMatrix;
};



