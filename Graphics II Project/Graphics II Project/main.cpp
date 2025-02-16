// D3D LAB 1a "Line Land".
// Author: L.Norri CD DRX, FullSail University

// Introduction:
// Welcome to the first lab of the Direct3D Graphics Programming class.
// This is the ONLY guided lab in this course! 
// Future labs will demand the habits & foundation you develop right now!  
// It is CRITICAL that you follow each and every step. ESPECIALLY THE READING!!!

// TO BEGIN: Open the word document that acompanies this lab and start from the top.

//************************************************************
//************ INCLUDES & DEFINES ****************************
//************************************************************

#include <d3d11.h>
//#include "Loader.h"
// including the direct3D library file
#pragma comment (lib,"d3d11.lib")
#include "Trivial_VS.csh"
#include "Trivial_PS.csh"
#include "Star_VS.csh"
#include "Star_PS.csh"
#include "Cube.h"
#include "faceDiff.h"
#include "Model.h"
#include "Lights.csh"
#include "SkyBoxPixelShader_PS.csh"
#include "SkyBoxVertexShader_VS.csh"
#include "InstanceShader.csh"




#define BACKBUFFER_HEIGHT	768
#define BACKBUFFER_WIDTH	1024
#define AspectRatio ((float)BACKBUFFER_WIDTH/BACKBUFFER_HEIGHT)



//name of the matrix, row 3, 128bit array of floats finding float32, at 0
// example.r[3].m128_f32[0];

//************************************************************
//************ SIMPLE WINDOWS APP CLASS **********************
//************************************************************


class DEMO_APP
{	
	HINSTANCE						application;
	WNDPROC							appWndProc;
	HWND							window;
	// TODO: PART 1 STEP 2
	
	ID3D11Device *device = {};                        // pointer to the direct3d device interfave
	ID3D11DeviceContext *context = {};                // pointer to our direct3d device context
	ID3D11RenderTargetView *rtv = {};                 // pointer to the direct3d renderTargetView
	IDXGISwapChain *swapchain = {};                   // pointer to the swap chain interface
	D3D11_VIEWPORT viewport, viewport2;
	ID3D11InputLayout *input = {};                    // pointer to the direct3d input
	ID3D11InputLayout *starInput = {};                // pointer to the direct3d input for the star
	ID3D11InputLayout *skyBoxInput = {};              // pointer to the input for the skybox
	POINT currPos;

	// TODO: PART 2 STEP 2
	
	ID3D11Buffer *vertexStarBuffer, *vertexMiniGunBuffer, *vertexGroundBuffer, *vertexDeadPoolBuffer, *vertexSkyBoxBuffer, *vertexDragonBuffer, *vertexDragonKnightBuffer;// pointer to hold the information in Vram
	unsigned int numVerts = 12;                      // number of verts in the circle
	ID3D11Buffer *indexBuffer;                       // pointer to hold the points in order to draw
	ID3D11Buffer *starIndexBuffer;                   // pointer to hold the points in order of the star
	ID3D11Buffer *MinigunIndexBuffer;                // pointer to hold the points in order of the gun
	ID3D11Buffer *groundIndexBuffer;                 // pointer to hold the points in order of the ground
	ID3D11Buffer *deadPoolIndexBuffer, *skyBoxIndexBuffer, *dragonIndexBuffer, *dragonKnightIndexBuffer;
	ID3D11Texture2D *depthStencil = NULL;            // pointer to the "depth buffer"
	ID3D11DepthStencilView *depthStencilView;        // the depth stencil

	// TODO: PART 2 STEP 4
	
	ID3D11VertexShader *vertexShader;                // pointer to the vertexShader
	ID3D11PixelShader *pixelShader;                  // pointer to the pixelShader
	ID3D11VertexShader *StarVertexShader;
	ID3D11PixelShader *StarPIxelShader;
	ID3D11PixelShader *LightShader;
	ID3D11PixelShader *skyBoxPixelShader;
	ID3D11VertexShader *skyBoxVertexShader;

	// BEGIN PART 3
	// TODO: PART 3 STEP 1

	ID3D11Buffer *ConstObjectBuffer;                         // pointer to the constant buffer holding the world
	ID3D11Buffer *ConstSceneBuffer;                          // pointer to the scene holding view and projection
	ID3D11Buffer *ConstantPointLightBuffer;
	ID3D11Buffer *ConstantDirectionalLightBuffer;
	ID3D11Buffer *ConstantSpotLightBuffer;

	// for instances
	XMMATRIX instanes[3];
	ID3D11Buffer *InstanceConstanceBuffer;
	ID3D11VertexShader *InstanceVertexShader;

	//ID3D11Buffer *ConstModelBuffer;
	
	XTime timer;
	// TODO: PART 3 STEP 2b
	

	OBJECT_TO_VRAM star, miniGun, ground, Deadpool, skyBox, dragon, dragonKnight;
	SCENE_TO_VRAM camera, camera2;
	POINT_LIGHT PointLight;
	SPOT_LIGHT SpotLight;
	DIRECTIONAL_LIGHT DirectionalLight;

	Model *miniGunModel, *groundModel,*deadpoolModel, *dragonModel, *dragonKnightModel;


	// TODO: PART 3 STEP 4a
	ID3D11Texture2D *texturesArray;     // pointer to the array that holds the number of miplevels
	ID3D11ShaderResourceView *SRV;      // for old header cube
	ID3D11ShaderResourceView *GunSRV;   // shader for the gun texture
	ID3D11ShaderResourceView *GroundSRV;
	ID3D11ShaderResourceView *deadPoolSRV, *skyboxSRV, *dragonSRV, *dragonKnightSRV;
	ID3D11SamplerState *sampleState;     // pointer for the sample

	ID3D11RasterizerState *rasterizer1, *rasterizer2;

	

	RGBA backcolor;
	XMMATRIX VS_ViewMatrix = XMMatrixIdentity();

	float Yscale = (1 / tanf((.5f*FOV)*(PI/180.0f))), zFar = FarPlane, zNear = NearPlane;
	float Xscale = (Yscale*AspectRatio);

	void ThreadedLoading(Model* model, const char *path);
	// no clue
	DEMO_APP() {};


public:
	
	static DEMO_APP *singleton();

	void DEMO_APP_Creation(HINSTANCE hinst, WNDPROC proc);
	bool Run();
	bool ShutDown();
	SCENE_TO_VRAM Movement(SCENE_TO_VRAM&);
	void Resize();
};

DEMO_APP *DEMO_APP::singleton()
{
	// create an instance to use later
	static DEMO_APP viewScreenInstance;
	// return the instance to get information from the project
	return &viewScreenInstance;
}

//************************************************************
//************ CREATION OF OBJECTS & RESOURCES ***************
//************************************************************

void DEMO_APP::DEMO_APP_Creation(HINSTANCE hinst, WNDPROC proc)
{

#pragma region model loading

	miniGunModel = new Model();
	deadpoolModel = new Model();
	groundModel = new Model();
	dragonModel = new Model();
	dragonKnightModel = new Model();
	//14967             14967             14967
	//thread thread1(miniGunModel.loadOBJ, this, "../Graphics II Project/sbv9148irj-Deadpool/Deadpool/DP GUN 1.obj", miniGunModel.pos, miniGunModel.uv, miniGunModel.normal);
	//miniGunModel.loadOBJ("../Graphics II Project/sbv9148irj-Deadpool/Deadpool/crate.obj", miniGunModel.pos, miniGunModel.uv, miniGunModel.normal);
	//thread thread1(&DEMO_APP::ThreadedLoading, this, miniGunModel, "../Graphics II Project/sbv9148irj-Deadpool/Deadpool/DP GUN 1.obj");
	thread thread2(&DEMO_APP::ThreadedLoading, this, deadpoolModel, "../Graphics II Project/sbv9148irj-Deadpool/Deadpool/deadpool sword 1.obj");
	thread thread3(&DEMO_APP::ThreadedLoading, this, groundModel, "../Graphics II Project/sbv9148irj-Deadpool/Deadpool/Ground.obj");
	thread thread4(&DEMO_APP::ThreadedLoading, this, dragonModel, "../Graphics II Project/Mobile - Summoners War - DragonA/dragon_hr-light.obj");
	thread thread5(&DEMO_APP::ThreadedLoading, this, dragonKnightModel, "../Graphics II Project/Mobile - Summoners War - Dragon KnightA/dragonknight_hr-dark.obj");

#pragma endregion

	// ****************** BEGIN WARNING ***********************// 
	// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY! 
	application = hinst;
	appWndProc = proc;

	WNDCLASSEX  wndClass;
	ZeroMemory(&wndClass, sizeof(wndClass));
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.lpfnWndProc = appWndProc;
	wndClass.lpszClassName = L"DirectXApplication";
	wndClass.hInstance = application;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME);
	//wndClass.hIcon			= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_FSICON));
	RegisterClassEx(&wndClass);

	RECT window_size = { 0, 0, BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT };
	AdjustWindowRect(&window_size, WS_OVERLAPPEDWINDOW, false);

	window = CreateWindow(L"DirectXApplication", L"Lab 1a Line Land", WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX),
		CW_USEDEFAULT, CW_USEDEFAULT, window_size.right - window_size.left, window_size.bottom - window_size.top,
		NULL, NULL, application, this);

	ShowWindow(window, SW_SHOW);
	//********************* END WARNING ************************//

	// TODO: PART 1 STEP 3a

	DXGI_SWAP_CHAIN_DESC chain;
	//memset(&chain, 0, 8);
	ZeroMemory(&chain, sizeof(DXGI_SWAP_CHAIN_DESC));
	chain.BufferCount = 1;
	chain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	chain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	chain.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	chain.SampleDesc.Count = 4;
	chain.OutputWindow = window;
	chain.Windowed = true;

	// TODO: PART 1 STEP 3b

	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, NULL, 0, D3D11_SDK_VERSION, &chain, &swapchain, &device, NULL, &context);

	// TODO: PART 1 STEP 4

	// create the imagebackbuffer that will store the image to be printed
	ID3D11Texture2D * imageBackBuffer;


	// get the back bufer on the swap chain and create the texture object stored in the pBackBuffer
	// (layer of back buffer (first in the chain), uniqueid for the com object, void* points to the location of the image object)
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&imageBackBuffer);

	// using teh imagebuffers address to create the render target
	// (pointer to the image/object, struct that describes the render target (backbuffer doesn't need this so null works), address of the backbuffer to be used)
	device->CreateRenderTargetView(imageBackBuffer, NULL, &rtv);
	// must release the image buffer to release threads used by the COM object
	imageBackBuffer->Release();


	// zero out the struct for viewport
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.Width = BACKBUFFER_WIDTH * 0.5f;
	viewport.Height = BACKBUFFER_HEIGHT;
	viewport.MaxDepth = 1;
	viewport.MinDepth = 0;


	context->RSSetViewports(1, &viewport);

	ZeroMemory(&viewport2, sizeof(D3D11_VIEWPORT));

	viewport2.Width = BACKBUFFER_WIDTH *0.5f;
	viewport2.Height = BACKBUFFER_HEIGHT;
	viewport2.MaxDepth = 1;
	viewport2.MinDepth = 0;
	viewport2.TopLeftX = BACKBUFFER_WIDTH *0.5f;
	viewport2.TopLeftY = 0;

	context->RSSetViewports(1, &viewport2);

	// TODO: PART 4 STEP 1
#pragma region triangleStuff

	SIMPLE_VERTEX triangleCombo[12];
	int startDegree = 90;
	int increment = 36;
	for (int i = 1; i < 11; i++)
	{
		if ((i % 2) == 0)
		{
			triangleCombo[i] = { cosf(XMConvertToRadians(startDegree))*0.3f, sinf(XMConvertToRadians(startDegree))*0.3f, 0.0f, 1, 0, 0, 1 };
		}
		else
		{
			triangleCombo[i] = { cosf(XMConvertToRadians(startDegree)), sinf(XMConvertToRadians(startDegree)), 0.0f, 0, 1, 0, 1 };
		}
		startDegree -= increment;
	}
	triangleCombo[0] = { 0.0f,0.0f,-0.3f, 0, 0, 1, 1 };
	triangleCombo[11] = { 0.0f,0.0f,0.3f, 0, 0, 1, 1 };

	UINT indexOrder[60];
	int curr = 1;
	// loop and wind to the right, giving the front faces
	for (int i = 0; i < 30; i += 3)
	{
		indexOrder[i] = 0;
		indexOrder[i + 1] = curr;
		indexOrder[i + 2] = ((curr + 1)>10) ? 1 : curr + 1;
		curr++;
	}
	curr = 1;
	// loop and wind to the left, giving the back faces
	for (int i = 30; i < 60; i += 3)
	{
		indexOrder[i] = 11;
		indexOrder[i + 2] = curr;
		indexOrder[i + 1] = ((curr + 1)>10) ? 1 : curr + 1;
		curr++;
	}

#pragma endregion

#pragma region skybox

	COMPLEX_VERTEX skyBoxCombo[8] =
	{
		// back left top
		{ float3(-1,1,1)},

		// back right top
		{ float3(1,1,1)},

		// back right bottom
		{ float3(1,-1,1)},

		// front right bottom
		{ float3(1,-1,-1)},

		// front left bottom
		{ float3(-1,-1,-1)},

		// front left top
		{ float3(-1,1,-1)},

		// front right top
		{ float3(1,1,-1)},

		// back left bottom
		{ float3(-1,-1,1)},

	};

	UINT SkyBoxindexOrder[36] =
//     top face     right face    front face    left face     back face    bootom face
	{0,1,6, 0,6,5, 1,2,3, 1,3,6, 5,6,3, 5,3,4, 0,5,4, 0,4,7, 1,0,7, 1,7,2, 4,3,2, 4,2,7};
	
#pragma endregion
	
	//thread1.join();
	thread2.join();
	thread3.join();
	thread4.join();
	thread5.join();

#pragma region VertexBuffers

	D3D11_BUFFER_DESC triangleBufferDesc;
	ZeroMemory(&triangleBufferDesc, sizeof(D3D11_BUFFER_DESC));
	triangleBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	triangleBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	triangleBufferDesc.CPUAccessFlags = NULL;
	// total size of the buffer
	triangleBufferDesc.ByteWidth = sizeof(SIMPLE_VERTEX) * 12;
	triangleBufferDesc.MiscFlags = NULL;
	triangleBufferDesc.StructureByteStride = sizeof(SIMPLE_VERTEX);

	// gun buffer
	D3D11_BUFFER_DESC gunBufferDesc;
	ZeroMemory(&gunBufferDesc, sizeof(D3D11_BUFFER_DESC));
	gunBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	gunBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	gunBufferDesc.CPUAccessFlags = NULL;
	// total size of the buffer
	gunBufferDesc.ByteWidth = sizeof(Model::vertex_Normal) * (UINT)miniGunModel->uniqueVerts.size();
	gunBufferDesc.MiscFlags = NULL;
	//gunBufferDesc.StructureByteStride = sizeof(Model);

	D3D11_BUFFER_DESC groundBufferDesc;
	ZeroMemory(&groundBufferDesc, sizeof(D3D11_BUFFER_DESC));
	groundBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	groundBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	groundBufferDesc.CPUAccessFlags = NULL;
	// total size of the buffer
	groundBufferDesc.ByteWidth = sizeof(Model::vertex_Normal) * (UINT)groundModel->uniqueVerts.size();
	groundBufferDesc.MiscFlags = NULL;
	//groundBufferDesc.StructureByteStride = sizeof(OBJ_VERT);

	D3D11_BUFFER_DESC deadPoolBufferDesc;
	ZeroMemory(&deadPoolBufferDesc, sizeof(D3D11_BUFFER_DESC));
	deadPoolBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	deadPoolBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	deadPoolBufferDesc.CPUAccessFlags = NULL;
	// total size of the buffer
	deadPoolBufferDesc.ByteWidth = sizeof(Model::vertex_Normal) * (UINT)deadpoolModel->uniqueVerts.size();
	deadPoolBufferDesc.MiscFlags = NULL;

	D3D11_BUFFER_DESC skyboxBufferDesc;
	ZeroMemory(&skyboxBufferDesc, sizeof(D3D11_BUFFER_DESC));
	skyboxBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	skyboxBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	skyboxBufferDesc.CPUAccessFlags = NULL;
	skyboxBufferDesc.ByteWidth = sizeof(COMPLEX_VERTEX) * 8;
	skyboxBufferDesc.MiscFlags = NULL;

	D3D11_BUFFER_DESC dragonBufferDesc;
	ZeroMemory(&dragonBufferDesc, sizeof(D3D11_BUFFER_DESC));
	dragonBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	dragonBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	dragonBufferDesc.CPUAccessFlags = NULL;
	dragonBufferDesc.ByteWidth = sizeof(Model::vertex_Normal) * (UINT)dragonModel->uniqueVerts.size();
	dragonBufferDesc.MiscFlags = NULL;

	D3D11_BUFFER_DESC dragonKnightBufferDesc;
	ZeroMemory(&dragonKnightBufferDesc, sizeof(D3D11_BUFFER_DESC));
	dragonKnightBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	dragonKnightBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	dragonKnightBufferDesc.CPUAccessFlags = NULL;
	dragonKnightBufferDesc.ByteWidth = sizeof(Model::vertex_Normal) * (UINT)dragonKnightModel->uniqueVerts.size();
	dragonKnightBufferDesc.MiscFlags = NULL;

#pragma endregion

#pragma region IndexBuffers
	// triangle indexbuffer
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = NULL;
	indexBufferDesc.ByteWidth = sizeof(UINT) * 60;
	indexBufferDesc.MiscFlags = NULL;
	indexBufferDesc.StructureByteStride = sizeof(UINT);

	// gun indexbuffer
	D3D11_BUFFER_DESC gunIndexBufferDesc;
	ZeroMemory(&gunIndexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	gunIndexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	gunIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	gunIndexBufferDesc.CPUAccessFlags = NULL;
	gunIndexBufferDesc.ByteWidth = sizeof(UINT) *(UINT)miniGunModel->uniqueIndexBuffer.size();
	gunIndexBufferDesc.MiscFlags = NULL;
	gunIndexBufferDesc.StructureByteStride = sizeof(UINT);

	// cube index buffer
	D3D11_BUFFER_DESC groundIndexBufferDesc;
	ZeroMemory(&groundIndexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	groundIndexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	groundIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	groundIndexBufferDesc.CPUAccessFlags = NULL;
	groundIndexBufferDesc.ByteWidth = sizeof(UINT) * (UINT)groundModel->uniqueIndexBuffer.size();
	groundIndexBufferDesc.MiscFlags = NULL;
	groundIndexBufferDesc.StructureByteStride = sizeof(const unsigned int);

	// deadpool index buffer
	D3D11_BUFFER_DESC deadPoolIndexBufferDesc;
	ZeroMemory(&deadPoolIndexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	deadPoolIndexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	deadPoolIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	deadPoolIndexBufferDesc.CPUAccessFlags = NULL;
	deadPoolIndexBufferDesc.ByteWidth = sizeof(UINT) * (UINT)deadpoolModel->uniqueIndexBuffer.size();
	deadPoolIndexBufferDesc.MiscFlags = NULL;
	deadPoolIndexBufferDesc.StructureByteStride = sizeof(const unsigned int);

	D3D11_BUFFER_DESC skyboxIndexBufferDesc;
	ZeroMemory(&skyboxIndexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	skyboxIndexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	skyboxIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	skyboxIndexBufferDesc.CPUAccessFlags = NULL;
	skyboxIndexBufferDesc.ByteWidth = sizeof(UINT) * 36;
	skyboxIndexBufferDesc.MiscFlags = NULL;
	skyboxIndexBufferDesc.StructureByteStride = sizeof(UINT);

	D3D11_BUFFER_DESC dragonIndexBufferDesc;
	ZeroMemory(&dragonIndexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	dragonIndexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	dragonIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	dragonIndexBufferDesc.CPUAccessFlags = NULL;
	dragonIndexBufferDesc.ByteWidth = sizeof(UINT) * (UINT)dragonModel->uniqueIndexBuffer.size();
	dragonIndexBufferDesc.MiscFlags = NULL;
	dragonIndexBufferDesc.StructureByteStride = sizeof(UINT);

	D3D11_BUFFER_DESC dragonKnightIndexBufferDesc;
	ZeroMemory(&dragonKnightIndexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	dragonKnightIndexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	dragonKnightIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	dragonKnightIndexBufferDesc.CPUAccessFlags = NULL;
	dragonKnightIndexBufferDesc.ByteWidth = sizeof(UINT) * (UINT)dragonKnightModel->uniqueIndexBuffer.size();
	dragonKnightIndexBufferDesc.MiscFlags = NULL;
	dragonKnightIndexBufferDesc.StructureByteStride = sizeof(UINT);


#pragma endregion

#pragma region Depth/DSV SampleDesc
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = BACKBUFFER_WIDTH;
	descDepth.Height = BACKBUFFER_HEIGHT;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 4;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Width = faceDiff_width;
	textureDesc.Height = faceDiff_height;
	textureDesc.MipLevels = faceDiff_numlevels;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	//ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	descDSV.Texture2D.MipSlice = 0;

	D3D11_SAMPLER_DESC sampleDesc = {};
	ZeroMemory(&sampleDesc, sizeof(D3D11_SAMPLER_DESC));
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampleDesc.ComparisonFunc = D3D11_COMPARISON_GREATER;
	sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;

#pragma endregion

#pragma region SubResourceData
	// TODO: PART 2 STEP 3c
	D3D11_SUBRESOURCE_DATA subTriData = {};
	subTriData.pSysMem = triangleCombo;
	subTriData.SysMemPitch = 0;
	subTriData.SysMemSlicePitch = 0;

	D3D11_SUBRESOURCE_DATA triangleindexData;
	triangleindexData.pSysMem = indexOrder;
	triangleindexData.SysMemPitch = 0;
	triangleindexData.SysMemSlicePitch = 0;

	D3D11_SUBRESOURCE_DATA subMiniGunData = {};
	subMiniGunData.pSysMem = miniGunModel->uniqueVerts.data();
	subMiniGunData.SysMemPitch = 0;
	subMiniGunData.SysMemSlicePitch = 0;

	D3D11_SUBRESOURCE_DATA minigunIndexData = {};
	minigunIndexData.pSysMem = miniGunModel->uniqueIndexBuffer.data();
	minigunIndexData.SysMemPitch = 0;
	minigunIndexData.SysMemSlicePitch = 0;

	D3D11_SUBRESOURCE_DATA groundSubData = {};
	groundSubData.pSysMem = groundModel->uniqueVerts.data();
	groundSubData.SysMemPitch = 0;
	groundSubData.SysMemSlicePitch = 0;

	D3D11_SUBRESOURCE_DATA groundIndexData;
	groundIndexData.pSysMem = groundModel->uniqueIndexBuffer.data();
	groundIndexData.SysMemPitch = 0;
	groundIndexData.SysMemSlicePitch = 0;

	D3D11_SUBRESOURCE_DATA deadPoolData = {};
	deadPoolData.pSysMem = deadpoolModel->uniqueVerts.data();
	deadPoolData.SysMemPitch = 0;
	deadPoolData.SysMemSlicePitch = 0;

	D3D11_SUBRESOURCE_DATA deadPoolIndexData = {};
	deadPoolIndexData.pSysMem = deadpoolModel->uniqueIndexBuffer.data();
	deadPoolIndexData.SysMemPitch = 0;
	deadPoolIndexData.SysMemSlicePitch = 0;

	D3D11_SUBRESOURCE_DATA subSkyboxData = {};
	subSkyboxData.pSysMem = skyBoxCombo;
	subSkyboxData.SysMemPitch = 0;
	subSkyboxData.SysMemSlicePitch = 0;

	D3D11_SUBRESOURCE_DATA skyboxIndexData = {};
	skyboxIndexData.pSysMem = SkyBoxindexOrder;
	skyboxIndexData.SysMemPitch = 0;
	skyboxIndexData.SysMemSlicePitch = 0;

	D3D11_SUBRESOURCE_DATA subDragonData = {};
	subDragonData.pSysMem = dragonModel->uniqueVerts.data();
	subDragonData.SysMemPitch = 0;
	subDragonData.SysMemSlicePitch = 0;

	D3D11_SUBRESOURCE_DATA subDragonIndexData = {};
	subDragonIndexData.pSysMem = dragonModel->uniqueIndexBuffer.data();
	subDragonIndexData.SysMemPitch = 0;
	subDragonIndexData.SysMemSlicePitch = 0;

	D3D11_SUBRESOURCE_DATA subDragonKnightData = {};
	subDragonKnightData.pSysMem = dragonKnightModel->uniqueVerts.data();
	subDragonKnightData.SysMemPitch = 0;
	subDragonKnightData.SysMemSlicePitch = 0;

	D3D11_SUBRESOURCE_DATA subDragonKnightIndexData = {};
	subDragonKnightIndexData.pSysMem = dragonKnightModel->uniqueIndexBuffer.data();
	subDragonKnightIndexData.SysMemPitch = 0;
	subDragonKnightIndexData.SysMemSlicePitch = 0;

	D3D11_SUBRESOURCE_DATA subInstanceData = {};
	subInstanceData.pSysMem = &instanes;
	subInstanceData.SysMemPitch = 0;
	subInstanceData.SysMemSlicePitch = 0;

	D3D11_SUBRESOURCE_DATA subTextureData[faceDiff_numlevels] = {};
	for (int i = 0; i < faceDiff_numlevels; i++)
	{
		subTextureData[i].pSysMem = &faceDiff_pixels[faceDiff_leveloffsets[i]];
		subTextureData[i].SysMemPitch = (faceDiff_width >> i)*sizeof(UINT);
		subTextureData[i].SysMemSlicePitch = 0;
	}

#pragma endregion

#pragma region CreateBuffers/Textures/Shaders

	// the device is the d3d11 device created earlier used to create all the buffer
	device->CreateBuffer(&triangleBufferDesc, &subTriData, &vertexStarBuffer);
	device->CreateBuffer(&indexBufferDesc, &triangleindexData, &starIndexBuffer);

	device->CreateBuffer(&groundBufferDesc, &groundSubData, &vertexGroundBuffer);
	device->CreateBuffer(&groundIndexBufferDesc, &groundIndexData, &groundIndexBuffer);

	//device->CreateBuffer(&gunBufferDesc, &subMiniGunData, &vertexMiniGunBuffer);
	//device->CreateBuffer(&gunIndexBufferDesc, &minigunIndexData, &MinigunIndexBuffer);

	device->CreateBuffer(&deadPoolBufferDesc, &deadPoolData, &vertexDeadPoolBuffer);
	device->CreateBuffer(&deadPoolIndexBufferDesc, &deadPoolIndexData, &deadPoolIndexBuffer);

	device->CreateBuffer(&skyboxBufferDesc, &subSkyboxData, &vertexSkyBoxBuffer);
	device->CreateBuffer(&skyboxIndexBufferDesc, &skyboxIndexData, &skyBoxIndexBuffer);

	device->CreateBuffer(&dragonBufferDesc, &subDragonData, &vertexDragonBuffer);
	device->CreateBuffer(&dragonIndexBufferDesc, &subDragonIndexData, &dragonIndexBuffer);

	device->CreateBuffer(&dragonKnightBufferDesc, &subDragonKnightData, &vertexDragonKnightBuffer);
	device->CreateBuffer(&dragonKnightIndexBufferDesc, &subDragonKnightIndexData, &dragonKnightIndexBuffer);


	device->CreateTexture2D(&descDepth, NULL, &depthStencil);
	device->CreateDepthStencilView(depthStencil, &descDSV, &depthStencilView);
	device->CreateTexture2D(&textureDesc, subTextureData, &texturesArray);
	device->CreateShaderResourceView(texturesArray, nullptr, &SRV);

	// loading using the ddstexture
	//CreateDDSTextureFromFile(device, L"../Graphics II Project/sbv9148irj-Deadpool/Deadpool/WEP_MP7_TEXTSET_Color_NormX.dds", NULL, &GunSRV);
	CreateDDSTextureFromFile(device, L"Seamless tileable ice snow cracks ground texture.dds", NULL, &GroundSRV);
	CreateDDSTextureFromFile(device, L"../Graphics II Project/sbv9148irj-Deadpool/Deadpool/DPROP_DeadpoolSword_TEXSET_Color_NormX.dds", NULL, &deadPoolSRV);
	CreateDDSTextureFromFile(device, L"SunsetSkybox.dds", NULL, &skyboxSRV);
	CreateDDSTextureFromFile(device, L"../Graphics II Project/Mobile - Summoners War - DragonA/dragon_hr-light.dds", NULL, &dragonSRV);
	CreateDDSTextureFromFile(device, L"../Graphics II Project/Mobile - Summoners War - Dragon KnightA/dragonknight_hr-dark.dds", NULL, &dragonKnightSRV);

	device->CreateVertexShader(Trivial_VS, sizeof(Trivial_VS), nullptr, &vertexShader);
	device->CreatePixelShader(Trivial_PS, sizeof(Trivial_PS), nullptr, &pixelShader);
	device->CreateVertexShader(Star_VS, sizeof(Star_VS), nullptr, &StarVertexShader);
	device->CreatePixelShader(Star_PS, sizeof(Star_PS), nullptr, &StarPIxelShader);
	device->CreatePixelShader(Lights, sizeof(Lights), nullptr, &LightShader);
	device->CreatePixelShader(SkyBoxPixelShader_PS, sizeof(SkyBoxPixelShader_PS), nullptr, &skyBoxPixelShader);
	device->CreateVertexShader(SkyBoxVertexShader_VS, sizeof(SkyBoxVertexShader_VS),nullptr, &skyBoxVertexShader);
	device->CreateVertexShader(InstanceShader, sizeof(InstanceShader), nullptr, &InstanceVertexShader);

#pragma endregion
	// TODO: PART 2 STEP 8a

#pragma region INPUTS
	D3D11_INPUT_ELEMENT_DESC vLayout[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{ "UV",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0}
	};

	D3D11_INPUT_ELEMENT_DESC starLayout[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	D3D11_INPUT_ELEMENT_DESC skyboxLayout[] = 
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};
	// TODO: PART 2 STEP 8b
	//UINT num_elements = sizeof(vLayout) / sizeof(vLayout[0]);
	device->CreateInputLayout(vLayout, ARRAYSIZE(vLayout), Trivial_VS, sizeof(Trivial_VS), &input);
	device->CreateInputLayout(starLayout, ARRAYSIZE(starLayout), Star_VS, sizeof(Star_VS), &starInput);
	device->CreateInputLayout(skyboxLayout, ARRAYSIZE(skyboxLayout), SkyBoxVertexShader_VS, sizeof(SkyBoxVertexShader_VS), &skyBoxInput);

#pragma endregion
	// TODO: PART 3 STEP 3
#pragma region ConstBufferDesc
	D3D11_BUFFER_DESC constbufferstuff;
	ZeroMemory(&constbufferstuff, sizeof(D3D11_BUFFER_DESC));
	constbufferstuff.ByteWidth = sizeof(OBJECT_TO_VRAM);
	constbufferstuff.Usage = D3D11_USAGE_DYNAMIC;
	constbufferstuff.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constbufferstuff.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constbufferstuff.StructureByteStride = sizeof(float);

	D3D11_BUFFER_DESC constbufferstuffScene;
	ZeroMemory(&constbufferstuffScene, sizeof(D3D11_BUFFER_DESC));
	constbufferstuffScene.ByteWidth = sizeof(SCENE_TO_VRAM);
	constbufferstuffScene.Usage = D3D11_USAGE_DYNAMIC;
	constbufferstuffScene.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constbufferstuffScene.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constbufferstuffScene.StructureByteStride = sizeof(float);

	D3D11_BUFFER_DESC constbufferstuffModel;
	ZeroMemory(&constbufferstuffModel, sizeof(D3D11_BUFFER_DESC));
	constbufferstuffModel.ByteWidth = sizeof(Model);
	constbufferstuffModel.Usage = D3D11_USAGE_DYNAMIC;
	constbufferstuffModel.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constbufferstuffModel.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constbufferstuffModel.StructureByteStride = sizeof(float);

	D3D11_BUFFER_DESC constbufferDirectionalLight;
	ZeroMemory(&constbufferDirectionalLight, sizeof(D3D11_BUFFER_DESC));
	constbufferDirectionalLight.ByteWidth = sizeof(DIRECTIONAL_LIGHT);
	constbufferDirectionalLight.Usage = D3D11_USAGE_DYNAMIC;
	constbufferDirectionalLight.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constbufferDirectionalLight.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constbufferDirectionalLight.StructureByteStride = sizeof(float);

	D3D11_BUFFER_DESC constBufferSpotLight;
	ZeroMemory(&constBufferSpotLight, sizeof(D3D11_BUFFER_DESC));
	constBufferSpotLight.ByteWidth = sizeof(SPOT_LIGHT);
	constBufferSpotLight.Usage = D3D11_USAGE_DYNAMIC;
	constBufferSpotLight.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufferSpotLight.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constBufferSpotLight.StructureByteStride = sizeof(float);

	D3D11_BUFFER_DESC constBufferPointLight;
	ZeroMemory(&constBufferPointLight, sizeof(D3D11_BUFFER_DESC));
	constBufferPointLight.ByteWidth = sizeof(POINT_LIGHT);
	constBufferPointLight.Usage = D3D11_USAGE_DYNAMIC;
	constBufferPointLight.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufferPointLight.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constBufferPointLight.StructureByteStride = sizeof(float);

	D3D11_BUFFER_DESC constBufferInstance;
	ZeroMemory(&constBufferInstance, sizeof(D3D11_BUFFER_DESC));
	constBufferInstance.ByteWidth = sizeof(instanes);
	constBufferInstance.Usage = D3D11_USAGE_DYNAMIC;
	constBufferInstance.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufferInstance.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constBufferInstance.StructureByteStride = sizeof(XMMATRIX);


	device->CreateBuffer(&constbufferstuff, NULL, &ConstObjectBuffer);
	device->CreateBuffer(&constbufferstuffScene, NULL, &ConstSceneBuffer);
	device->CreateBuffer(&constbufferDirectionalLight, NULL, &ConstantDirectionalLightBuffer);
	device->CreateBuffer(&constBufferSpotLight, NULL, &ConstantSpotLightBuffer);
	device->CreateBuffer(&constBufferPointLight, NULL, &ConstantPointLightBuffer);
	device->CreateBuffer(&constBufferInstance, &subInstanceData, &InstanceConstanceBuffer);

#pragma endregion

#pragma region setWorldMatrix

	// TODO: PART 3 STEP 4b
	ground.worldMatrix = XMMatrixIdentity();
	ground.worldMatrix = XMMatrixTranslation(0, -1, 0);

	star.worldMatrix = XMMatrixIdentity();
	star.worldMatrix = XMMatrixTranslation(-4, 3, 8);

	miniGun.worldMatrix = XMMatrixIdentity();
	// scaling from the identity down to .05 to make it smaller
	miniGun.worldMatrix = XMMatrixScaling(.05f, .05f, .05f);
	// set the position of xyz 
	miniGun.worldMatrix.r[3].m128_f32[0] = 8;
	miniGun.worldMatrix.r[3].m128_f32[1] = 0;
	miniGun.worldMatrix.r[3].m128_f32[2] = -3;

	camera.projectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(65), AspectRatio * 0.5f, zNear, zFar);
	camera.viewMatrix = XMMatrixInverse(NULL, XMMatrixIdentity());
	
	camera2.projectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(65), AspectRatio * 0.5f, zNear, zFar);
	camera2.viewMatrix = XMMatrixInverse(NULL, XMMatrixIdentity());
	
	camera2.viewMatrix = XMMatrixTranslation(35, 15, 0);
	
	XMVECTOR temp = { camera2.viewMatrix.r[3].m128_f32[0], camera2.viewMatrix.r[3].m128_f32[1], camera2.viewMatrix.r[3].m128_f32[2], camera2.viewMatrix.r[3].m128_f32[3] };

	XMVECTOR worldUp = { 0,1,0 };
	camera2.viewMatrix = XMMatrixLookAtLH(temp, { 0,0,0 }, worldUp);

	Deadpool.worldMatrix = XMMatrixIdentity();
	Deadpool.worldMatrix = XMMatrixScaling(0.05f, 0.05f, 0.05f);
	Deadpool.worldMatrix.r[3].m128_f32[0] = 8;
	Deadpool.worldMatrix.r[3].m128_f32[1] = 0;
	Deadpool.worldMatrix.r[3].m128_f32[2] = 0;

	// lighting
	DirectionalLight.worldMatrix = XMMatrixIdentity();
	DirectionalLight.directionalLightColor = { 0.3f,1.0f,0.3f,0.0f };
	DirectionalLight.directionalLightDirection = { 0.0f,0.0f,1.0f,0.0f };
	DirectionalLight.worldMatrix.r[3].m128_f32[0] = DirectionalLight.directionalLightDirection.x;
	DirectionalLight.worldMatrix.r[3].m128_f32[1] = DirectionalLight.directionalLightDirection.y;
	DirectionalLight.worldMatrix.r[3].m128_f32[2] = DirectionalLight.directionalLightDirection.z;
	DirectionalLight.worldMatrix.r[3].m128_f32[3] = DirectionalLight.directionalLightDirection.w;

	SpotLight.worldMatrix = XMMatrixIdentity();
	SpotLight.spotLightColor = { 0.3f,0.5f,1.0f,1.0f };
	SpotLight.spotLightConeDirection = { 0.0f,-1.0f, 0.0f,0.0f };
	SpotLight.spotLightConeRatio = {cosf(XMConvertToRadians(50)), cosf(XMConvertToRadians(100)), 0.0f, 0.0f};
	SpotLight.spotLightPosition = { 0.0f,3.0f,0.0f,0.0f };
	// position
	SpotLight.worldMatrix.r[3].m128_f32[0] = SpotLight.spotLightPosition.x;
	SpotLight.worldMatrix.r[3].m128_f32[1] = SpotLight.spotLightPosition.y;
	SpotLight.worldMatrix.r[3].m128_f32[2] = SpotLight.spotLightPosition.z;
	SpotLight.worldMatrix.r[3].m128_f32[3] = SpotLight.spotLightPosition.w;

	PointLight.pointLightColor = { 1.0f,0.3f,0.3f,1.0f };
	PointLight.pointLightPosition = { -4,1,4,1 };
	PointLight.pointLightRadius = { 10,0.01f,0.01f,0.1f };
	PointLight.worldMatrix = XMMatrixIdentity();
	PointLight.worldMatrix = XMMatrixTranslation(PointLight.pointLightPosition.x, PointLight.pointLightPosition.y, PointLight.pointLightPosition.z);

	skyBox.worldMatrix = XMMatrixIdentity();
	skyBox.worldMatrix = XMMatrixTranslation(0, 0, 0);

	/*dragon.worldMatrix = XMMatrixIdentity();
	dragon.worldMatrix = XMMatrixScaling(0.05f, 0.05f, 0.05f);
	dragon.worldMatrix.r[3].m128_f32[0] = 0;
	dragon.worldMatrix.r[3].m128_f32[1] = 0;
	dragon.worldMatrix.r[3].m128_f32[2] = 10;*/

	dragonKnight.worldMatrix = XMMatrixIdentity();
	dragonKnight.worldMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f);
	dragonKnight.worldMatrix.r[3].m128_f32[0] = 0;
	dragonKnight.worldMatrix.r[3].m128_f32[1] = -0.5f;
	dragonKnight.worldMatrix.r[3].m128_f32[2] = 1;

	instanes[0] = dragon.worldMatrix = XMMatrixIdentity();
	dragon.worldMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f);
	instanes[0] = dragon.worldMatrix;
	instanes[0].r[3].m128_f32[0]= 0;
	instanes[0].r[3].m128_f32[1]= -0.5f;
	instanes[0].r[3].m128_f32[2]= 10;

	instanes[1] = dragon.worldMatrix ;
	instanes[1].r[3].m128_f32[0] = 10;
	instanes[1].r[3].m128_f32[1] = -0.5f;
	instanes[1].r[3].m128_f32[2] = 10;

	instanes[2] = dragon.worldMatrix;
	instanes[2].r[3].m128_f32[0] = -10;
	instanes[2].r[3].m128_f32[1] = -0.5f;
	instanes[2].r[3].m128_f32[2] = 10;




#pragma endregion

	D3D11_RASTERIZER_DESC rasterizer1desc;
	ZeroMemory(&rasterizer1desc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizer1desc.FillMode = D3D11_FILL_SOLID;
	rasterizer1desc.CullMode = D3D11_CULL_BACK;
	rasterizer1desc.FrontCounterClockwise = TRUE;
	rasterizer1desc.DepthClipEnable = TRUE;
	rasterizer1desc.MultisampleEnable = TRUE;

	device->CreateRasterizerState(&rasterizer1desc,&rasterizer1);

	D3D11_RASTERIZER_DESC rasterizer2desc;
	ZeroMemory(&rasterizer2desc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizer2desc.FillMode = D3D11_FILL_SOLID;
	rasterizer2desc.CullMode = D3D11_CULL_NONE;
	rasterizer2desc.FrontCounterClockwise = false;
	rasterizer2desc.DepthClipEnable = TRUE;
	rasterizer2desc.MultisampleEnable = TRUE;


	device->CreateRasterizerState(&rasterizer2desc,&rasterizer2);

	device->CreateSamplerState(&sampleDesc,&sampleState);

}

//************************************************************
//************ EXECUTION *************************************
//************************************************************

bool DEMO_APP::Run()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// this function call will set a breakpoint at the location of a leaked block
	// set the parameter to the identifier for a leaked block
	_CrtSetBreakAlloc(-1L);

	timer.Signal();
	// TODO: PART 4 STEP 2	

	star.worldMatrix = XMMatrixMultiply(XMMatrixRotationY(timer.Delta()), star.worldMatrix);
	miniGun.worldMatrix = XMMatrixMultiply(XMMatrixRotationY(timer.Delta()), miniGun.worldMatrix);



	// make temp matrix then set it to the xyz for the direction and multiply it for rotation and pass values back to direction for spot light

	// set the render target equal to the backbuffer for use bind one or more render targets atomically
	// (number of render targets to set(usually 1), pointer to list of viewable objects, the depthstencilview(if null it is not bound))
	context->OMSetRenderTargets(1, &rtv, depthStencilView);
	
	// TODO: PART 1 STEP 7b
	
	// set the device to draw at the viewports starting position
	//context->RSSetViewports(1, &viewport);
	//context->RSSetViewports(1, &viewport2);

	// TODO: PART 1 STEP 7c
	
	// clear the screen
	context->ClearRenderTargetView(rtv, &(backcolor.a, backcolor.g, backcolor.b, backcolor.r));
	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1, 0);

	 //TODO: PART 5 STEP 4

#pragma region movement

	Movement(camera);
	GetCursorPos(&currPos);
	VS_ViewMatrix = camera.viewMatrix;


#pragma endregion

	
	// TODO: PART 3 STEP 5
	
	camera.viewMatrix = XMMatrixInverse(NULL, VS_ViewMatrix);


	context->VSSetConstantBuffers(0, 1, &ConstObjectBuffer);
	context->VSSetConstantBuffers(1, 1, &ConstSceneBuffer);
	context->PSSetConstantBuffers(1, 1, &ConstantDirectionalLightBuffer);
	context->PSSetConstantBuffers(0, 1, &ConstantSpotLightBuffer);
	context->PSSetConstantBuffers(2, 1, &ConstantPointLightBuffer);
	context->VSSetConstantBuffers(2, 1, &InstanceConstanceBuffer);
	

	// TODO: PART 3 STEP 6
	// 0 and 1 corelate to the buffer 0 or 1 in the vertex shader HLSL


	context->VSSetShader(vertexShader, NULL, 0);
	context->PSSetShader(pixelShader, NULL, 0);
	//context->PSSetShaderResources(0, 1, &SRV);
	context->PSSetSamplers(0, 1, &sampleState);
	
	// TODO: PART 2 STEP 9c
	
	context->IASetInputLayout(input);

#pragma region Draw
	for (int i = 0; i < 2; i++)
	{
		if (i == 0)
		{
			D3D11_MAPPED_SUBRESOURCE map2;
			context->Map(ConstSceneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map2);
			memcpy(map2.pData, &camera, sizeof(SCENE_TO_VRAM));
			context->Unmap(ConstSceneBuffer, 0);

		

			context->RSSetViewports(1, &viewport);

		}
		else
		{
			D3D11_MAPPED_SUBRESOURCE map3;
			context->Map(ConstSceneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map3);
			memcpy(map3.pData, &camera2, sizeof(SCENE_TO_VRAM));
			context->Unmap(ConstSceneBuffer, 0);

			context->RSSetViewports(1, &viewport2);
			skyBox.worldMatrix.r[3] = XMMatrixInverse(NULL, camera2.viewMatrix).r[3];

		}



		// for the anti aliseing
		context->RSSetState(rasterizer1);

		D3D11_MAPPED_SUBRESOURCE skyboxMap;
		context->Map(ConstObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &skyboxMap);
		memcpy(skyboxMap.pData, &skyBox, sizeof(OBJECT_TO_VRAM));
		context->Unmap(ConstObjectBuffer, 0);

		UINT skyboxStride = sizeof(COMPLEX_VERTEX);
		UINT skyboxOffset = 0;
		context->IASetVertexBuffers(0, 1, &vertexSkyBoxBuffer, &skyboxStride, &skyboxOffset);
		context->IASetIndexBuffer(skyBoxIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetInputLayout(skyBoxInput);
		context->PSSetShaderResources(0, 1, &skyboxSRV);
		context->VSSetShader(skyBoxVertexShader, NULL, 0);
		context->PSSetShader(skyBoxPixelShader, NULL, 0);
		context->DrawIndexed(36, 0, 0);

		// sets the skybox out to 1 and everything else below is at its own depth
		context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1, 0);

		context->RSSetState(rasterizer2);

		D3D11_MAPPED_SUBRESOURCE groundMap;
		context->Map(ConstObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &groundMap);
		memcpy(groundMap.pData, &ground, sizeof(OBJECT_TO_VRAM));
		context->Unmap(ConstObjectBuffer, 0);

		UINT groundstride = sizeof(Model::vertex_Normal);
		UINT groundOffset = 0;
		context->IASetVertexBuffers(0, 1, &vertexGroundBuffer, &groundstride, &groundOffset);
		context->IASetIndexBuffer(groundIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		context->IASetInputLayout(input);
		context->PSSetShaderResources(0, 1, &GroundSRV);
		context->VSSetShader(vertexShader, NULL, 0);
		context->PSSetShader(LightShader, NULL, 0);
		context->DrawIndexed(groundModel->uniqueIndexBuffer.size(), 0, 0);


		D3D11_MAPPED_SUBRESOURCE triMap;
		context->Map(ConstObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &triMap);
		memcpy(triMap.pData, &star, sizeof(OBJECT_TO_VRAM));
		context->Unmap(ConstObjectBuffer, 0);
		// triangle created
		UINT starstride = sizeof(SIMPLE_VERTEX);
		UINT staroffset = 0;
		context->IASetVertexBuffers(0, 1, &vertexStarBuffer, &starstride, &staroffset);
		context->IASetIndexBuffer(starIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetInputLayout(starInput);
		// set the shaders for the grid to be drawn
		context->VSSetShader(StarVertexShader, NULL, 0);
		context->PSSetShader(StarPIxelShader, NULL, 0);

		context->DrawIndexed(60, 0, 0);

		//D3D11_MAPPED_SUBRESOURCE gunMap;
		//context->Map(ConstObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gunMap);
		//memcpy(gunMap.pData, &miniGun, sizeof(OBJECT_TO_VRAM));
		//context->Unmap(ConstObjectBuffer, 0);

		//UINT gunstride = sizeof(Model::vertex_Normal);
		//UINT gunOffset = 0;
		//context->IASetVertexBuffers(0, 1, &vertexMiniGunBuffer, &gunstride, &gunOffset);
		//context->IASetIndexBuffer(MinigunIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		//context->IASetInputLayout(input);
		//context->PSSetShaderResources(0, 1, &GunSRV);
		//context->VSSetShader(vertexShader, NULL, 0);
		//context->PSSetShader(LightShader, NULL, 0);
		//context->DrawIndexed(miniGunModel->uniqueIndexBuffer.size(), 0, 0);

		D3D11_MAPPED_SUBRESOURCE DeadPoolMap;
		context->Map(ConstObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &DeadPoolMap);
		memcpy(DeadPoolMap.pData, &Deadpool, sizeof(OBJECT_TO_VRAM));
		context->Unmap(ConstObjectBuffer, 0);

		UINT deadpoolStride = sizeof(Model::vertex_Normal);
		UINT deadPoolOffset = 0;
		context->IASetVertexBuffers(0, 1, &vertexDeadPoolBuffer, &deadpoolStride, &deadPoolOffset);
		context->IASetIndexBuffer(deadPoolIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		context->IASetInputLayout(input);
		context->PSSetShaderResources(0, 1, &deadPoolSRV);
		context->VSSetShader(vertexShader, NULL, 0);
		context->PSSetShader(LightShader, NULL, 0);
		context->DrawIndexed(deadpoolModel->uniqueIndexBuffer.size(), 0, 0);

		//D3D11_MAPPED_SUBRESOURCE DragonMap;
		//context->Map(ConstObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &DragonMap);
		//memcpy(DragonMap.pData, &dragon, sizeof(OBJECT_TO_VRAM));
		//context->Unmap(ConstObjectBuffer, 0);

		D3D11_MAPPED_SUBRESOURCE DragonInstanceMap;
		context->Map(InstanceConstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &DragonInstanceMap);
		memcpy(DragonInstanceMap.pData, instanes, sizeof(instanes));
		context->Unmap(InstanceConstanceBuffer, 0);

		UINT dragonStride = sizeof(Model::vertex_Normal);
		UINT dragonOffset = 0;
		context->IASetVertexBuffers(0, 1, &vertexDragonBuffer, &dragonStride, &dragonOffset);
		context->IASetIndexBuffer(dragonIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		context->IASetInputLayout(input);
		context->PSSetShaderResources(0, 1, &dragonSRV);
		//context->VSSetShader(vertexShader, NULL, 0);
		context->VSSetShader(InstanceVertexShader, NULL, 0);
		context->PSSetShader(LightShader, NULL, 0);
		//context->DrawIndexed(dragonModel->uniqueIndexBuffer.size(), 0, 0);
		context->DrawIndexedInstanced(dragonModel->uniqueIndexBuffer.size(), 3, 0, 0, 0);

		D3D11_MAPPED_SUBRESOURCE DragonKnightMap;
		context->Map(ConstObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &DragonKnightMap);
		memcpy(DragonKnightMap.pData, &dragonKnight, sizeof(OBJECT_TO_VRAM));
		context->Unmap(ConstObjectBuffer, 0);

		UINT dragonKnightStride = sizeof(Model::vertex_Normal);
		UINT dragonKnightOffset = 0;
		context->IASetVertexBuffers(0, 1, &vertexDragonKnightBuffer, &dragonKnightStride, &dragonKnightOffset);
		context->IASetIndexBuffer(dragonKnightIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		context->IASetInputLayout(input);
		context->PSSetShaderResources(0, 1, &dragonKnightSRV);
		context->VSSetShader(vertexShader, NULL, 0);
		context->PSSetShader(LightShader, NULL, 0);
		context->DrawIndexed(dragonKnightModel->uniqueIndexBuffer.size(), 0, 0);
		// END PART 2

		D3D11_MAPPED_SUBRESOURCE DirectionalLightMap;
		context->Map(ConstantDirectionalLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &DirectionalLightMap);
		memcpy(DirectionalLightMap.pData, &DirectionalLight, sizeof(DIRECTIONAL_LIGHT));
		context->Unmap(ConstantDirectionalLightBuffer, 0);
		context->PSSetShader(LightShader, NULL, 0);

		D3D11_MAPPED_SUBRESOURCE SpotLightMap;
		context->Map(ConstantSpotLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &SpotLightMap);
		memcpy(SpotLightMap.pData, &SpotLight, sizeof(SPOT_LIGHT));
		context->Unmap(ConstantSpotLightBuffer, 0);
		context->PSSetShader(LightShader, NULL, 0);

		D3D11_MAPPED_SUBRESOURCE PointLightMap;
		context->Map(ConstantPointLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &PointLightMap);
		memcpy(PointLightMap.pData, &PointLight, sizeof(POINT_LIGHT));
		context->Unmap(ConstantPointLightBuffer, 0);
		context->PSSetShader(LightShader, NULL, 0);

	}
	
#pragma endregion

	// TODO: PART 1 STEP 8

	// put the backbuffer to the front buffer
	swapchain->Present(0, 0);


	// END OF PART 1
	return true; 
}

SCENE_TO_VRAM DEMO_APP::Movement(SCENE_TO_VRAM &_camera)
{
	_camera.viewMatrix = VS_ViewMatrix;

	// local =	rot/trans/scale  TIMES   matrix
	// global = MATRIX			 TIMES   rot/trans/scale

	if (GetAsyncKeyState('W')) // move forward
	{
		XMVECTOR temp = { 0, 0, 10 * timer.Delta(), _camera.viewMatrix.r[3].m128_f32[3] };
		camera.viewMatrix = XMMatrixMultiply(XMMatrixTranslationFromVector(temp), camera.viewMatrix);
	}
	if (GetAsyncKeyState('S')) // move backward
	{
		XMVECTOR temp = { 0, 0,5 * -timer.Delta(), _camera.viewMatrix.r[3].m128_f32[3] };
		_camera.viewMatrix = XMMatrixMultiply(XMMatrixTranslationFromVector(temp), _camera.viewMatrix);

	}
	if (GetAsyncKeyState('A')) // look left
	{
		XMVECTOR temp = { _camera.viewMatrix.r[3].m128_f32[0], _camera.viewMatrix.r[3].m128_f32[1], _camera.viewMatrix.r[3].m128_f32[2], _camera.viewMatrix.r[3].m128_f32[3] };

		_camera.viewMatrix.r[3].m128_f32[0] = 0;
		_camera.viewMatrix.r[3].m128_f32[1] = 0;
		_camera.viewMatrix.r[3].m128_f32[2] = 0;
		_camera.viewMatrix.r[3].m128_f32[3] = 1;

		_camera.viewMatrix = XMMatrixMultiply(_camera.viewMatrix, XMMatrixRotationY(-timer.Delta() * 10));
		_camera.viewMatrix.r[3].m128_f32[0] = temp.m128_f32[0];
		_camera.viewMatrix.r[3].m128_f32[1] = temp.m128_f32[1];
		_camera.viewMatrix.r[3].m128_f32[2] = temp.m128_f32[2];

	}
	if (GetAsyncKeyState('D')) // look right
	{
		XMVECTOR temp = { _camera.viewMatrix.r[3].m128_f32[0], _camera.viewMatrix.r[3].m128_f32[1], _camera.viewMatrix.r[3].m128_f32[2], _camera.viewMatrix.r[3].m128_f32[3] };

		_camera.viewMatrix.r[3].m128_f32[0] = 0;
		_camera.viewMatrix.r[3].m128_f32[1] = 0;
		_camera.viewMatrix.r[3].m128_f32[2] = 0;
		_camera.viewMatrix.r[3].m128_f32[3] = 1;

		_camera.viewMatrix = XMMatrixMultiply(_camera.viewMatrix, XMMatrixRotationY(timer.Delta() * 5));
		_camera.viewMatrix.r[3].m128_f32[0] = temp.m128_f32[0];
		_camera.viewMatrix.r[3].m128_f32[1] = temp.m128_f32[1];
		_camera.viewMatrix.r[3].m128_f32[2] = temp.m128_f32[2];
	}
	if (GetAsyncKeyState('Q')) // strafe left
	{
		XMVECTOR temp = { -10 * timer.Delta(), 0, 0, _camera.viewMatrix.r[3].m128_f32[3] };
		_camera.viewMatrix = XMMatrixMultiply(XMMatrixTranslationFromVector(temp), _camera.viewMatrix);
	}
	if (GetAsyncKeyState('E')) // strafe right
	{
		XMVECTOR temp = { 10 * timer.Delta(), 0, 0, _camera.viewMatrix.r[3].m128_f32[3] };
		_camera.viewMatrix = XMMatrixMultiply(XMMatrixTranslationFromVector(temp), _camera.viewMatrix);
	}
	if (GetAsyncKeyState('Z')) // strafe left
	{
		XMVECTOR temp = { 0,  10 * timer.Delta(), 0, _camera.viewMatrix.r[3].m128_f32[3] };
		_camera.viewMatrix = XMMatrixMultiply(XMMatrixTranslationFromVector(temp), camera.viewMatrix);
	}
	if (GetAsyncKeyState('X')) // strafe right
	{
		XMVECTOR temp = { 0,  -10 * timer.Delta(), 0, _camera.viewMatrix.r[3].m128_f32[3] };
		_camera.viewMatrix = XMMatrixMultiply(XMMatrixTranslationFromVector(temp), _camera.viewMatrix);
	}

	if (GetAsyncKeyState(VK_RBUTTON))
	{
		POINT tempMouse;
		GetCursorPos(&tempMouse);
		float deltaX = tempMouse.x - currPos.x;
		float deltaY = tempMouse.y - currPos.y;

		//_camera.viewMatrix = XMMatrixInverse(0, _camera.viewMatrix);

		XMVECTOR tempPos = { _camera.viewMatrix.r[3].m128_f32[0], _camera.viewMatrix.r[3].m128_f32[1], _camera.viewMatrix.r[3].m128_f32[2], _camera.viewMatrix.r[3].m128_f32[3] };

		_camera.viewMatrix.r[3].m128_f32[0] = 0;
		_camera.viewMatrix.r[3].m128_f32[1] = 0;
		_camera.viewMatrix.r[3].m128_f32[2] = 0;
		//_camera.viewMatrix.r[3].m128_f32[3] = 1;

		_camera.viewMatrix = XMMatrixMultiply(XMMatrixRotationX(deltaY * (10 * timer.Delta())),_camera.viewMatrix);
		_camera.viewMatrix = XMMatrixMultiply( _camera.viewMatrix, XMMatrixRotationY(deltaX*(10 * timer.Delta())));

		_camera.viewMatrix.r[3].m128_f32[0] = tempPos.m128_f32[0];
		_camera.viewMatrix.r[3].m128_f32[1] = tempPos.m128_f32[1];
		_camera.viewMatrix.r[3].m128_f32[2] = tempPos.m128_f32[2];

		//_camera.viewMatrix = XMMatrixInverse(0, _camera.viewMatrix);

		currPos = tempMouse;

	}
	
#pragma region pointlight movement
	// moving the point light
	if (GetAsyncKeyState(VK_LEFT))
	{
		XMVECTOR temp = { -10 * timer.Delta(),0,0,PointLight.worldMatrix.r[3].m128_f32[3] };
		PointLight.worldMatrix = XMMatrixMultiply(XMMatrixTranslationFromVector(temp), PointLight.worldMatrix);
		PointLight.pointLightPosition.x = PointLight.worldMatrix.r[3].m128_f32[0];
		PointLight.pointLightPosition.y = PointLight.worldMatrix.r[3].m128_f32[1];
		PointLight.pointLightPosition.z = PointLight.worldMatrix.r[3].m128_f32[2];
		PointLight.pointLightPosition.w = PointLight.worldMatrix.r[3].m128_f32[3];
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		XMVECTOR temp = { 10 * timer.Delta(),0,0,PointLight.worldMatrix.r[3].m128_f32[3] };
		PointLight.worldMatrix = XMMatrixMultiply(XMMatrixTranslationFromVector(temp), PointLight.worldMatrix);
		PointLight.pointLightPosition.x = PointLight.worldMatrix.r[3].m128_f32[0];
		PointLight.pointLightPosition.y = PointLight.worldMatrix.r[3].m128_f32[1];
		PointLight.pointLightPosition.z = PointLight.worldMatrix.r[3].m128_f32[2];
		PointLight.pointLightPosition.w = PointLight.worldMatrix.r[3].m128_f32[3];
	}
	if (GetAsyncKeyState(VK_RCONTROL))
	{
		XMVECTOR temp = { 0,-10 * timer.Delta(),0,PointLight.worldMatrix.r[3].m128_f32[3] };
		PointLight.worldMatrix = XMMatrixMultiply(XMMatrixTranslationFromVector(temp), PointLight.worldMatrix);
		PointLight.pointLightPosition.x = PointLight.worldMatrix.r[3].m128_f32[0];
		PointLight.pointLightPosition.y = PointLight.worldMatrix.r[3].m128_f32[1];
		PointLight.pointLightPosition.z = PointLight.worldMatrix.r[3].m128_f32[2];
		PointLight.pointLightPosition.w = PointLight.worldMatrix.r[3].m128_f32[3];
	}
	if (GetAsyncKeyState(VK_RSHIFT))
	{
		XMVECTOR temp = { 0,10 * timer.Delta(),0,PointLight.worldMatrix.r[3].m128_f32[3] };
		PointLight.worldMatrix = XMMatrixMultiply(XMMatrixTranslationFromVector(temp), PointLight.worldMatrix);
		PointLight.pointLightPosition.x = PointLight.worldMatrix.r[3].m128_f32[0];
		PointLight.pointLightPosition.y = PointLight.worldMatrix.r[3].m128_f32[1];
		PointLight.pointLightPosition.z = PointLight.worldMatrix.r[3].m128_f32[2];
		PointLight.pointLightPosition.w = PointLight.worldMatrix.r[3].m128_f32[3];
	}
	if (GetAsyncKeyState(VK_UP))
	{
		XMVECTOR temp = { 0,0,10 * timer.Delta(),PointLight.worldMatrix.r[3].m128_f32[3] };
		PointLight.worldMatrix = XMMatrixMultiply(XMMatrixTranslationFromVector(temp), PointLight.worldMatrix);
		PointLight.pointLightPosition.x = PointLight.worldMatrix.r[3].m128_f32[0];
		PointLight.pointLightPosition.y = PointLight.worldMatrix.r[3].m128_f32[1];
		PointLight.pointLightPosition.z = PointLight.worldMatrix.r[3].m128_f32[2];
		PointLight.pointLightPosition.w = PointLight.worldMatrix.r[3].m128_f32[3];
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		XMVECTOR temp = { 0,0,-10 * timer.Delta(),PointLight.worldMatrix.r[3].m128_f32[3] };
		PointLight.worldMatrix = XMMatrixMultiply(XMMatrixTranslationFromVector(temp), PointLight.worldMatrix);
		PointLight.pointLightPosition.x = PointLight.worldMatrix.r[3].m128_f32[0];
		PointLight.pointLightPosition.y = PointLight.worldMatrix.r[3].m128_f32[1];
		PointLight.pointLightPosition.z = PointLight.worldMatrix.r[3].m128_f32[2];
		PointLight.pointLightPosition.w = PointLight.worldMatrix.r[3].m128_f32[3];
	}
#pragma endregion
#pragma region spotlight movement
	if (GetAsyncKeyState('J'))
	{
		XMVECTOR temp = { -10 * timer.Delta(),0,0,SpotLight.worldMatrix.r[3].m128_f32[3] };
		SpotLight.worldMatrix = XMMatrixMultiply(XMMatrixTranslationFromVector(temp), SpotLight.worldMatrix);
		SpotLight.spotLightPosition.x = SpotLight.worldMatrix.r[3].m128_f32[0];
		SpotLight.spotLightPosition.y = SpotLight.worldMatrix.r[3].m128_f32[1];
		SpotLight.spotLightPosition.z = SpotLight.worldMatrix.r[3].m128_f32[2];
		SpotLight.spotLightPosition.w = SpotLight.worldMatrix.r[3].m128_f32[3];
	}
	if (GetAsyncKeyState('L'))
	{
		XMVECTOR temp = { 10 * timer.Delta(),0,0,SpotLight.worldMatrix.r[3].m128_f32[3] };
		SpotLight.worldMatrix = XMMatrixMultiply(XMMatrixTranslationFromVector(temp), SpotLight.worldMatrix);
		SpotLight.spotLightPosition.x = SpotLight.worldMatrix.r[3].m128_f32[0];
		SpotLight.spotLightPosition.y = SpotLight.worldMatrix.r[3].m128_f32[1];
		SpotLight.spotLightPosition.z = SpotLight.worldMatrix.r[3].m128_f32[2];
		SpotLight.spotLightPosition.w = SpotLight.worldMatrix.r[3].m128_f32[3];
	}
	if (GetAsyncKeyState('I'))
	{
		XMVECTOR temp = {0, 0,10 * timer.Delta(),SpotLight.worldMatrix.r[3].m128_f32[3] };
		SpotLight.worldMatrix = XMMatrixMultiply(XMMatrixTranslationFromVector(temp), SpotLight.worldMatrix);
		SpotLight.spotLightPosition.x = SpotLight.worldMatrix.r[3].m128_f32[0];
		SpotLight.spotLightPosition.y = SpotLight.worldMatrix.r[3].m128_f32[1];
		SpotLight.spotLightPosition.z = SpotLight.worldMatrix.r[3].m128_f32[2];
		SpotLight.spotLightPosition.w = SpotLight.worldMatrix.r[3].m128_f32[3];
	}
	if (GetAsyncKeyState('K'))
	{
		XMVECTOR temp = {0,0, -10 * timer.Delta(),SpotLight.worldMatrix.r[3].m128_f32[3] };
		SpotLight.worldMatrix = XMMatrixMultiply(XMMatrixTranslationFromVector(temp), SpotLight.worldMatrix);
		SpotLight.spotLightPosition.x = SpotLight.worldMatrix.r[3].m128_f32[0];
		SpotLight.spotLightPosition.y = SpotLight.worldMatrix.r[3].m128_f32[1];
		SpotLight.spotLightPosition.z = SpotLight.worldMatrix.r[3].m128_f32[2];
		SpotLight.spotLightPosition.w = SpotLight.worldMatrix.r[3].m128_f32[3];
	}
	if (GetAsyncKeyState('U'))
	{
		SpotLight.spotLightConeDirection.x -= timer.Delta();
	}
	if (GetAsyncKeyState('O'))
	{
		SpotLight.spotLightConeDirection.x += timer.Delta();
	}
	if (GetAsyncKeyState('N'))
	{
		SpotLight.spotLightConeDirection.z -= timer.Delta();
	}
	if (GetAsyncKeyState('M'))
	{
		SpotLight.spotLightConeDirection.z += timer.Delta();
	}
#pragma endregion
#pragma region directional movement
	XMMATRIX rotation = XMMatrixIdentity();
	rotation.r[3].m128_f32[0] = DirectionalLight.directionalLightDirection.x;
	rotation.r[3].m128_f32[1] = DirectionalLight.directionalLightDirection.y;
	rotation.r[3].m128_f32[2] = DirectionalLight.directionalLightDirection.z;
	rotation = XMMatrixMultiply(rotation, XMMatrixRotationX(XMConvertToRadians((float)timer.Delta() * 20)));
	DirectionalLight.directionalLightDirection.x = rotation.r[3].m128_f32[0];
	DirectionalLight.directionalLightDirection.y = rotation.r[3].m128_f32[1];
	DirectionalLight.directionalLightDirection.z = rotation.r[3].m128_f32[2];
#pragma endregion

	//// move the skybox to match the cameras postion so you can never get to the edge of the skybox
	skyBox.worldMatrix.r[3].m128_f32[0] = _camera.viewMatrix.r[3].m128_f32[0];
	skyBox.worldMatrix.r[3].m128_f32[1] = _camera.viewMatrix.r[3].m128_f32[1];
	skyBox.worldMatrix.r[3].m128_f32[2] = _camera.viewMatrix.r[3].m128_f32[2];
	skyBox.worldMatrix.r[3].m128_f32[3] = _camera.viewMatrix.r[3].m128_f32[3];

	camera2.viewMatrix = XMMatrixMultiply( XMMatrixRotationY(timer.Delta()*0.05f),camera2.viewMatrix);

	//XMVECTOR temp = { camera2.viewMatrix.r[3].m128_f32[0], camera2.viewMatrix.r[3].m128_f32[1], camera2.viewMatrix.r[3].m128_f32[2], camera2.viewMatrix.r[3].m128_f32[3] };

	//camera2.viewMatrix.r[3].m128_f32[0] = 0;
	//camera2.viewMatrix.r[3].m128_f32[1] = 0;
	//camera2.viewMatrix.r[3].m128_f32[2] = 0;
	//camera2.viewMatrix.r[3].m128_f32[3] = 1;

	////camera2.viewMatrix = 
	//XMVECTOR worldUp = { 0,1,0 };
	//camera2.viewMatrix = XMMatrixLookAtLH(temp, { 0,0,0 }, worldUp);


	return _camera;
}

void DEMO_APP::ThreadedLoading(Model* model, const char *path)
{
	model->loadOBJ(path);
}


//************************************************************
//************ DESTRUCTION ***********************************
//************************************************************

bool DEMO_APP::ShutDown()
{
	// TODO: PART 1 STEP 6
	swapchain->SetFullscreenState(false, nullptr);

#define SAFE_RELEASE(P) {if(P){P->Release(); P=nullptr;}}

	SAFE_RELEASE(swapchain);
	SAFE_RELEASE(device);
	SAFE_RELEASE(context);
	SAFE_RELEASE(rtv);
	SAFE_RELEASE(vertexShader);
	SAFE_RELEASE(pixelShader);
	SAFE_RELEASE(input);
	SAFE_RELEASE(ConstObjectBuffer);
	SAFE_RELEASE(ConstSceneBuffer);
	SAFE_RELEASE(ConstantPointLightBuffer);
	SAFE_RELEASE(ConstantDirectionalLightBuffer);
	SAFE_RELEASE(ConstantSpotLightBuffer);
	SAFE_RELEASE(LightShader);
	SAFE_RELEASE(skyBoxPixelShader);
	SAFE_RELEASE(skyBoxVertexShader);
	SAFE_RELEASE(starInput);
	SAFE_RELEASE(skyBoxInput);
	//SAFE_RELEASE(GunSRV);
	SAFE_RELEASE(SRV);
	SAFE_RELEASE(GroundSRV);
	SAFE_RELEASE(deadPoolSRV);
	SAFE_RELEASE(sampleState);
	SAFE_RELEASE(skyboxSRV);
	SAFE_RELEASE(rasterizer1);
	SAFE_RELEASE(rasterizer2);
	SAFE_RELEASE(vertexStarBuffer);
	//SAFE_RELEASE(vertexMiniGunBuffer);
	SAFE_RELEASE(vertexGroundBuffer);
	SAFE_RELEASE(vertexDeadPoolBuffer);
	SAFE_RELEASE(vertexSkyBoxBuffer);
	SAFE_RELEASE(starIndexBuffer);
	//SAFE_RELEASE(MinigunIndexBuffer);
	SAFE_RELEASE(groundIndexBuffer);
	SAFE_RELEASE(deadPoolIndexBuffer);
	SAFE_RELEASE(skyBoxIndexBuffer);
	SAFE_RELEASE(depthStencil);
	SAFE_RELEASE(depthStencilView);
	SAFE_RELEASE(StarVertexShader);
	SAFE_RELEASE(StarPIxelShader);
	SAFE_RELEASE(texturesArray);
	SAFE_RELEASE(dragonIndexBuffer);
	SAFE_RELEASE(dragonSRV);
	SAFE_RELEASE(vertexDragonBuffer);
	SAFE_RELEASE(vertexDragonKnightBuffer);
	SAFE_RELEASE(dragonKnightIndexBuffer);
	SAFE_RELEASE(dragonKnightSRV);
	SAFE_RELEASE(InstanceConstanceBuffer);
	SAFE_RELEASE(InstanceVertexShader);
	delete miniGunModel;
	delete groundModel;
	delete deadpoolModel;
	delete dragonModel;
	delete dragonKnightModel;

	UnregisterClass( L"DirectXApplication", application ); 
	return true;
}


//************************************************************
//************ WINDOWS RELATED *******************************
//************************************************************


void DEMO_APP::Resize()
{
	// check to make sure somethign is in the swapchain
	if (swapchain == nullptr)
	{
		return;
	}
	else
	{
		context->OMSetRenderTargets(0, 0, 0);

		//rtv->Release();

		HRESULT hr;

		hr = swapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

		ID3D11Texture2D *resizeBuffer;

		hr = swapchain->GetBuffer(0, __uuidof(resizeBuffer), (void**)&resizeBuffer);

		hr = device->CreateRenderTargetView(resizeBuffer, NULL, &rtv);

		resizeBuffer->Release();
		context->OMSetRenderTargets(1, &rtv, NULL);

		RECT windowResize;

		GetWindowRect(window, &windowResize);


		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

		viewport.Width = (windowResize.right-windowResize.left) * 0.5f;
		viewport.Height = (windowResize.bottom - windowResize.top);
		viewport.MaxDepth = 1;
		viewport.MinDepth = 0;


		context->RSSetViewports(1, &viewport);

		ZeroMemory(&viewport2, sizeof(D3D11_VIEWPORT));

		viewport2.Width = (windowResize.right - windowResize.left)*0.5f;
		viewport2.Height = (windowResize.bottom - windowResize.top);
		viewport2.MaxDepth = 1;
		viewport2.MinDepth = 0;
		viewport2.TopLeftX = (windowResize.right - windowResize.left)*0.5f;
		viewport2.TopLeftY = 0;

		context->RSSetViewports(1, &viewport2);
	}
}

// ****************** BEGIN WARNING ***********************// 
// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY!
	
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine,	int nCmdShow );						   
LRESULT CALLBACK WndProc(HWND hWnd,	UINT message, WPARAM wparam, LPARAM lparam );		
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE, LPTSTR, int )
{
	srand(unsigned int(time(0)));
	//DEMO_APP myApp(hInstance,(WNDPROC)WndProc);
	// create the singleton with the information created in the initializeing function
	DEMO_APP::singleton()->DEMO_APP_Creation(hInstance, (WNDPROC)WndProc);
    MSG msg; ZeroMemory( &msg, sizeof( msg ) );
    while ( msg.message != WM_QUIT && DEMO_APP::singleton()->Run() )
    {	
	    if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        { 
            TranslateMessage( &msg );
            DispatchMessage( &msg ); 
        }
    }
	DEMO_APP::singleton()->ShutDown();
	return 0; 
}
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    if(GetAsyncKeyState(VK_ESCAPE))
		message = WM_DESTROY;
    switch ( message )
    {
        case ( WM_DESTROY ): { PostQuitMessage( 0 ); }
        break;
		case(WM_SIZE): {DEMO_APP::singleton()->Resize(); }
		break;
    }

    return DefWindowProc( hWnd, message, wParam, lParam );
}
//********************* END WARNING ************************//
