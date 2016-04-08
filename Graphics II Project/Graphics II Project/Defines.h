#pragma once
#include <iostream>
#include <Windows.h>
#include <ctime>
#include <cmath>
#include "XTime.h"
#include <DirectXMath.h>
#include <atlbase.h>
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

unsigned int rasterArray[NUMPIXELS];
unsigned int currColor;
float rotation = 0;
int input;
unsigned int red = 0xffff0000;
unsigned int green = 0xff00ff00;
unsigned int blue = 0xff0000ff;
unsigned int yellow = 0xffffff00;
unsigned int magenta = 0xffff00ff;
unsigned int pink = 0xffff69b4;
float Yscale = (1 / tanf((.5f*FOV)*(PI/180.0f))), zFar = FarPlane, zNear = NearPlane;
float Xscale = (Yscale*AspectRatio);
float currentPos;




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

//struct XMMATRIX
//{
//	float _e11, _e12, _e13, _e14;
//	float _e21, _e22, _e23, _e24 ;
//	float _e31, _e32, _e33, _e34 ;
//	float _e41, _e42, _e43, _e44 ;
//};

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
	SCREEN_XY() {}
	SCREEN_XY(Vertex a) { x = a.xyzw[0]; y = a.xyzw[1]; }
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


float zBuffer[NUMPIXELS];
RGBA backcolor;
XMMATRIX VS_WorldMatrix{
	1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	0,0,5,1
};
XMMATRIX VS_ViewMatrix{
	1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	0,0,0,1
};
XMMATRIX VS_ProjectionMatrix{
	Xscale,0,0,0,
	0,Yscale,0,0,
	0,0,zFar / (zFar - zNear),1,
	0,0,-(zFar*zNear) / (zFar - zNear),0
};


const Vertex unEditableOriginalLine[] =
{
	{ -0.5,0,0.5,1 },{ -0.4,0,0.5,1 },{ -0.3,0,0.5,1 },{ -0.2,0,0.5,1 },{ -0.1,0,0.5,1 },{ 0,0,0.5,1 },{ 0.1,0,0.5,1 },{ 0.2,0,0.5,1 },{ 0.3,0,0.5,1 },{ 0.4,0,0.5,1 },{ 0.5,0,0.5,1 },
	{ -0.5,0,0.4,1 },{ -0.4,0,0.4,1 },{ -0.3,0,0.4,1 },{ -0.2,0,0.4,1 },{ -0.1,0,0.4,1 },{ 0,0,0.4,1 },{ 0.1,0,0.4,1 },{ 0.2,0,0.4,1 },{ 0.3,0,0.4,1 },{ 0.4,0,0.4,1 },{ 0.5,0,0.4,1 },
	{ -0.5,0,0.3,1 },{ -0.4,0,0.3,1 },{ -0.3,0,0.3,1 },{ -0.2,0,0.3,1 },{ -0.1,0,0.3,1 },{ 0,0,0.3,1 },{ 0.1,0,0.3,1 },{ 0.2,0,0.3,1 },{ 0.3,0,0.3,1 },{ 0.4,0,0.3,1 },{ 0.5,0,0.3,1 },
	{ -0.5,0,0.2,1 },{ -0.4,0,0.2,1 },{ -0.3,0,0.2,1 },{ -0.2,0,0.2,1 },{ -0.1,0,0.2,1 },{ 0,0,0.2,1 },{ 0.1,0,0.2,1 },{ 0.2,0,0.2,1 },{ 0.3,0,0.2,1 },{ 0.4,0,0.2,1 },{ 0.5,0,0.2,1 },
	{ -0.5,0,0.1,1 },{ -0.4,0,0.1,1 },{ -0.3,0,0.1,1 },{ -0.2,0,0.1,1 },{ -0.1,0,0.1,1 },{ 0,0,0.1,1 },{ 0.1,0,0.1,1 },{ 0.2,0,0.1,1 },{ 0.3,0,0.1,1 },{ 0.4,0,0.1,1 },{ 0.5,0,0.1,1 },
	{ -0.5,0,0.0,1 },{ -0.4,0.0,0,1 },{ -0.3,0,0.0,1 },{ -0.2,0,0.0,1 },{ -0.1,0,0.0,1 },{ 0,0,0.0,1 },{ 0.1,0,0.0,1 },{ 0.2,0,0.0,1 },{ 0.3,0,0.0,1 },{ 0.4,0,0.0,1 },{ 0.5,0,0.0,1 },
	{ -0.5,0,-0.1,1 },{ -0.4,0,-0.1,1 },{ -0.3,0,-0.1,1 },{ -0.2,0,-0.1,1 },{ -0.1,0,-0.1,1 },{ 0,0,-0.1,1 },{ 0.1,0,-0.1,1 },{ 0.2,0,-0.1,1 },{ 0.3,0,-0.1,1 },{ 0.4,0,-0.1,1 },{ 0.5,0,-0.1,1 },
	{ -0.5,0,-0.2,1 },{ -0.4,0,-0.2,1 },{ -0.3,0,-0.2,1 },{ -0.2,0,-0.2,1 },{ -0.1,0,-0.2,1 },{ 0,0,-0.2,1 },{ 0.1,0,-0.2,1 },{ 0.2,0,-0.2,1 },{ 0.3,0,-0.2,1 },{ 0.4,0,-0.2,1 },{ 0.5,0,-0.2,1 },
	{ -0.5,0,-0.3,1 },{ -0.4,0,-0.3,1 },{ -0.3,0,-0.3,1 },{ -0.2,0,-0.3,1 },{ -0.1,0,-0.3,1 },{ 0,0,-0.3,1 },{ 0.1,0,-0.3,1 },{ 0.2,0,-0.3,1 },{ 0.3,0,-0.3,1 },{ 0.4,0,-0.3,1 },{ 0.5,0,-0.3,1 },
	{ -0.5,0,-0.4,1 },{ -0.4,0,-0.4,1 },{ -0.3,0,-0.4,1 },{ -0.2,0,-0.4,1 },{ -0.1,0,-0.4,1 },{ 0,0,-0.4,1 },{ 0.1,0,-0.4,1 },{ 0.2,0,-0.4,1 },{ 0.3,0,-0.4,1 },{ 0.4,0,-0.4,1 },{ 0.5,0,-0.4,1 },
	{ -0.5,0,-0.5,1 },{ -0.4,0,-0.5,1 },{ -0.3,0,-0.5,1 },{ -0.2,0,-0.5,1 },{ -0.1,0,-0.5,1 },{ 0,0,-0.5,1 },{ 0.1,0,-0.5,1 },{ 0.2,0,-0.5,1 },{ 0.3,0,-0.5,1 },{ 0.4,0,-0.5,1 },{ 0.5,0,-0.5,1 }

};


const Vertex BoxLines[] =
{  //          0                      1                    2                     3                    4                      5                        6                    7
	{ -0.25,0.25,-0.25,1 },{ -0.25,0.25,0.25,1 },{ 0.25,0.25,0.25,1 },{ 0.25,-0.25,0.25,1 },{ 0.25,-0.25,-0.25,1 },{ -0.25,-0.25,-0.25,1 },{ -0.25,-0.25,0.25,1 },{ 0.25,0.25,-0.25,1 }
};

const Vertex Triangle[] =
{
	{ -0.25,+0.25,.25,1,red,0,1 },{ +0.25,-0.25,.25,1,blue,0,1 },{ +0.25,+0.25,0.25,1,green,0,1 }
};

const Vertex TriangleCube[] =
{
	// left face bottom triangle 0,1,2
	{-0.25,0.25,0.25,1,red,0,0}, {-0.25,-0.25,-0.25,1,red,1,1}, {-0.25,-0.25,0.25,1,red,1,0},
	//         0                              1                             2
	// left face top triangle 0,1,2
	{-0.25,0.25,0.25,1,red,0,0}, {-0.25,0.25,-0.25,1,red,0,1},{-0.25,-0.25,-0.25,1,red,1,1},
	//      3                                  4                            5

	// Top face bottom triangle 0,1,2
	{-0.25,0.25,0.25,1,green,0,0}, {0.25,0.25,-0.25,1,green,1,1}, {-0.25,0.25,-0.25,1,green,0,1},
	//            6                            7                                8
	// Top face top triangle 0,1,2
	{-0.25,0.25,0.25,1,green,0,0}, {0.25,0.25,0.25,1,green,1,0}, {0.25,0.25,-0.25,1,green,1,1},
	//             9                           10                               11  
	
	// Right face bottom triangle 0,1,2
	{ 0.25,0.25,-0.25,1,pink,0,0}, { 0.25,-0.25,0.25,1,pink,1,1}, { 0.25,-0.25,-0.25,1,pink,1,0 },
	//            12                          13                           14
	// Right face top triangle 0,1,2      this thing is the messing up the triangle selection
	{ 0.25,0.25,-0.25,1,pink,0,0},{ 0.25,0.25,0.25,1,pink,0,1 } ,{ 0.25,-0.25,0.25,1,pink,1,1},
	//            15                              16                        17
	
	// Bottom face bottom triangle 0,1,2
	{-0.25,-0.25,0.25,1,yellow,0,0}, {0.25,-0.25,-0.25,1,yellow,1,1},{-0.25,-0.25,-0.25,1,yellow,0,1},
	//            18                                       19                                   20
	// Bottom face top triangle 0,1,2
	{-0.25,-0.25,0.25,1,yellow,0,0}, {0.25,-0.25,0.25,1,yellow,1,0},{0.25,-0.25,-0.25,1,yellow,1,1},
	//            21                                      22                                23
	
	// Front face bottom triangle 0,1,2
	{-0.25,0.25,-0.25,1,magenta,0,0}, {0.25,-0.25,-0.25,1,magenta,1,1}, {-0.25,-0.25,-0.25,1,magenta,1,0},
	//            24                                     25                                26
	// Front face top triangle 0,1,2
	{ -0.25,0.25,-0.25,1,magenta,0,0 }, {0.25,0.25,-0.25,1,magenta,0,1},{ 0.25,-0.25,-0.25,1,magenta,1,1 },
	//            27                                    28                                 29

	// Back face bottom triangle 0,1,2
	{0.25,0.25,0.25,1,blue,0,0}, {-0.25,-0.25,0.25,1,blue,1,1}, {0.25,-0.25,0.25,1,blue,1,0},
	//           30                                   31                                  32
	// Back face top triangle 0,1,2
	{0.25,0.25,0.25,1,blue,0,0 }, {-0.25,0.25,0.25,1,blue,0,1},{ -0.25,-0.25,0.25,1,blue,1,1 }
	//           33                                   34                                  35
};

//// Hack Matrix Inverse
//XMMATRIX HackUrnverse(XMMATRIX m)
//{
//	//name of the matrix, row 3, 128bit array of floats finding float32, at 0
//	// example.r[3].m128_f32[0];
//	XMMATRIX tempM;
//	XMVECTOR tempV, newTempVertex;
//	tempM.r[0].m128_f32[0] = m.r[0].m128_f32[0];
//	tempM.r[0].m128_f32[1] = m.r[0].m128_f32[1];
//	tempM.r[0].m128_f32[2] = m.r[0].m128_f32[2];
//	tempM.r[0].m128_f32[3] = 0;
//	tempM.r[1].m128_f32[0] = m.r[0].m128_f32[0];
//	tempM.r[1].m128_f32[1] = m.r[0].m128_f32[1];
//	tempM.r[1].m128_f32[2] = m.r[0].m128_f32[2];
//	tempM.r[1].m128_f32[3] = 0;
//	tempM.r[2].m128_f32[0] = m.r[0].m128_f32[0];
//	tempM.r[2].m128_f32[1] = m.r[0].m128_f32[1];
//	tempM.r[2].m128_f32[2] = m.r[0].m128_f32[2];
//	tempM.r[2].m128_f32[3] = 0;
//	tempM.r[3].m128_f32[0] = 0;
//	tempM.r[3].m128_f32[1] = 0;
//	tempM.r[3].m128_f32[2] = 0;
//	tempM.r[3].m128_f32[3] = 0;
//	tempV.m128_f32[0] = m.r[3].m128_f32[0];
//	tempV.m128_f32[1] = m.r[3].m128_f32[1];
//	tempV.m128_f32[2] = m.r[3].m128_f32[2];
//	tempV.m128_f32[3] = m.r[3].m128_f32[3];
//	tempM = XMMatrixTranspose(m);
//	newTempVertex = MultiplyVertexByMatrix3x3(tempV, tempM); 
//	newTempVertex = XMVectorNegate(newTempVertex);
//	tempM.r[3].m128_f32[0] = newTempVertex.m128_f32[0];
//	tempM.r[3].m128_f32[1] = newTempVertex.m128_f32[1];
//	tempM.r[3].m128_f32[2] = newTempVertex.m128_f32[2];
//	tempM.r[3].m128_f32[3] = m.r[3].m128_f32[3];
//
//	return tempM;
//
//}