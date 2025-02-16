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
#include "DDSTextureLoader.h"
#include <thread>
using namespace DirectX;
using namespace std;


#define RASTER_HEIGHT 768
#define RASTER_WIDTH 1024
#define PI 3.14159f
#define FOV 65
#define NearPlane 0.1f
#define FarPlane 100.0f

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

struct COMPLEX_VERTEX
{
	float3 pos;
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

struct POINT_LIGHT
{
	float4 pointLightPosition;
	float4 pointLightColor;
	float4 pointLightRadius;
	XMMATRIX worldMatrix;

};

struct DIRECTIONAL_LIGHT
{
	float4 directionalLightDirection;
	float4 directionalLightColor;
	XMMATRIX worldMatrix;

};

struct SPOT_LIGHT
{
	float4 spotLightPosition;
	float4 spotLightColor;
	float4 spotLightConeDirection;
	float4 spotLightConeRatio;
	XMMATRIX worldMatrix;

};



