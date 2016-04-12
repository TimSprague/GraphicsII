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
#include "DDSTextureLoader.h"


#define BACKBUFFER_WIDTH	500
#define BACKBUFFER_HEIGHT	500


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
	D3D11_VIEWPORT viewport;
	ID3D11InputLayout *input = {};                    // pointer to the direct3d input
	ID3D11InputLayout *starInput = {};                // pointer to the direct3d input for the star

	// TODO: PART 2 STEP 2
	
	ID3D11Buffer *vertexStarBuffer, *vertexMiniGunBuffer, *vertexGroundBuffer;// pointer to hold the information in Vram
	unsigned int numVerts = 12;                      // number of verts in the circle
	ID3D11Buffer *indexBuffer;                       // pointer to hold the points in order to draw
	ID3D11Buffer *starIndexBuffer;                   // pointer to hold the points in order of the star
	ID3D11Buffer *MinigunIndexBuffer;                // pointer to hold the points in order of the gun
	ID3D11Buffer *groundIndexBuffer;                 // pointer to hold the points in order of the ground
	ID3D11Texture2D *depthStencil = NULL;            // pointer to the "depth buffer"
	ID3D11DepthStencilView *depthStencilView;        // the depth stencil

	// TODO: PART 2 STEP 4
	
	ID3D11VertexShader *vertexShader;                // pointer to the vertexShader
	ID3D11PixelShader *pixelShader;                  // pointer to the pixelShader
	ID3D11VertexShader *StarVertexShader;
	ID3D11PixelShader *StarPIxelShader;

	// BEGIN PART 3
	// TODO: PART 3 STEP 1

	ID3D11Buffer *ConstObjectBuffer;                         // pointer to the constant buffer holding the world
	ID3D11Buffer *ConstSceneBuffer;                          // pointer to the scene holding view and projection
	//ID3D11Buffer *ConstModelBuffer;
	
	XTime timer;
	// TODO: PART 3 STEP 2b
	
	OBJECT_TO_VRAM star;
	OBJECT_TO_VRAM miniGun;
	OBJECT_TO_VRAM ground;
	SCENE_TO_VRAM camera;

	Model miniGunModel;
	Model groundModel;

	// TODO: PART 3 STEP 4a
	ID3D11Texture2D *texturesArray;     // pointer to the array that holds the number of miplevels
	ID3D11ShaderResourceView *SRV;      // for old header cube
	ID3D11ShaderResourceView *GunSRV;   // shader for the gun texture
	ID3D11ShaderResourceView *GroundSRV;
	ID3D11SamplerState *sampleState;     // pointer for the sample

	RGBA backcolor;
	XMMATRIX VS_ViewMatrix = XMMatrixIdentity();

	float Yscale = (1 / tanf((.5f*FOV)*(PI/180.0f))), zFar = FarPlane, zNear = NearPlane;
	float Xscale = (Yscale*AspectRatio);
public:
	// BEGIN PART 2
	// TODO: PART 2 STEP 1
	
	DEMO_APP(HINSTANCE hinst, WNDPROC proc);
	bool Run();
	bool ShutDown();
};

//************************************************************
//************ CREATION OF OBJECTS & RESOURCES ***************
//************************************************************

DEMO_APP::DEMO_APP(HINSTANCE hinst, WNDPROC proc)
{
	// ****************** BEGIN WARNING ***********************// 
	// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY! 
	application = hinst; 
	appWndProc = proc; 

	WNDCLASSEX  wndClass;
    ZeroMemory( &wndClass, sizeof( wndClass ) );
    wndClass.cbSize         = sizeof( WNDCLASSEX );             
    wndClass.lpfnWndProc    = appWndProc;						
    wndClass.lpszClassName  = L"DirectXApplication";            
	wndClass.hInstance      = application;		               
    wndClass.hCursor        = LoadCursor( NULL, IDC_ARROW );    
    wndClass.hbrBackground  = ( HBRUSH )( COLOR_WINDOWFRAME ); 
	//wndClass.hIcon			= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_FSICON));
    RegisterClassEx( &wndClass );

	RECT window_size = { 0, 0, BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT };
	AdjustWindowRect(&window_size, WS_OVERLAPPEDWINDOW, false);

	window = CreateWindow(	L"DirectXApplication", L"Lab 1a Line Land",	WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME|WS_MAXIMIZEBOX), 
							CW_USEDEFAULT, CW_USEDEFAULT, window_size.right-window_size.left, window_size.bottom-window_size.top,					
							NULL, NULL,	application, this );												

    ShowWindow( window, SW_SHOW );
	//********************* END WARNING ************************//

	// TODO: PART 1 STEP 3a

	DXGI_SWAP_CHAIN_DESC chain;
	//memset(&chain, 0, 8);
	ZeroMemory(&chain, sizeof(DXGI_SWAP_CHAIN_DESC));
	chain.BufferCount = 1;
	chain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	chain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	chain.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	chain.SampleDesc.Count = 1;
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

	viewport.Width =  BACKBUFFER_WIDTH;
	viewport.Height = BACKBUFFER_HEIGHT;
	viewport.MaxDepth = 1;
	viewport.MinDepth = 0;
	context->RSSetViewports(1, &viewport);

	// TODO: PART 4 STEP 1
#pragma region triangleStuff

	SIMPLE_VERTEX triangleCombo[12];
	int startDegree = 90;
	int increment = 36;
	for(int i = 1; i < 11; i++)
	{
		if ((i%2) == 0)
		{
			triangleCombo[i] = {cosf(XMConvertToRadians(startDegree))*0.3f, sinf(XMConvertToRadians(startDegree))*0.3f, 0.0f, 1, 0, 0, 1};
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
	for (int i = 0; i < 30; i+=3)
	{
		indexOrder[i] = 0;
		indexOrder[i + 1] = curr;
		indexOrder[i + 2] = ((curr + 1)>10) ? 1 : curr + 1;
		curr++;
	}
	curr = 1;
	// loop and wind to the left, giving the back faces
	for (int i = 30; i < 60; i+=3)
	{
		indexOrder[i] = 11;
		indexOrder[i + 2] = curr;
		indexOrder[i + 1] = ((curr + 1)>10) ? 1 : curr + 1;
		curr++;
	}

#pragma endregion



#pragma region model loading

                                                                                                //14967             14967             14967
	miniGunModel.loadOBJ("../Graphics II Project/sbv9148irj-Deadpool/Deadpool/DP GUN 1.obj", miniGunModel.pos, miniGunModel.uv, miniGunModel.normal); 

	groundModel.loadOBJ("../Graphics II Project/sbv9148irj-Deadpool/Deadpool/Ground.obj", groundModel.pos, groundModel.uv, groundModel.normal);
	//miniGunModel.loadOBJ("../Graphics II Project/Dragon/dragon1.obj", miniGunModel.pos, miniGunModel.uv, miniGunModel.normal);

#pragma endregion
	// TODO: PART 2 STEP 3b

	D3D11_BUFFER_DESC triangleBufferDesc;
	ZeroMemory(&triangleBufferDesc, sizeof(D3D11_BUFFER_DESC));
	triangleBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	triangleBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	triangleBufferDesc.CPUAccessFlags = NULL;
	// total size of the buffer
	triangleBufferDesc.ByteWidth = sizeof(SIMPLE_VERTEX)*12;
	triangleBufferDesc.MiscFlags = NULL;
	triangleBufferDesc.StructureByteStride = sizeof(SIMPLE_VERTEX);

    // gun buffer
	D3D11_BUFFER_DESC gunBufferDesc;
	ZeroMemory(&gunBufferDesc, sizeof(D3D11_BUFFER_DESC));
	gunBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	gunBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	gunBufferDesc.CPUAccessFlags = NULL;
	// total size of the buffer
	gunBufferDesc.ByteWidth = sizeof(Model::vertex_Normal) * (UINT)miniGunModel.uniqueVerts.size();
	gunBufferDesc.MiscFlags = NULL;
	//gunBufferDesc.StructureByteStride = sizeof(Model);

	D3D11_BUFFER_DESC groundBufferDesc;
	ZeroMemory(&groundBufferDesc, sizeof(D3D11_BUFFER_DESC));
	groundBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	groundBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	groundBufferDesc.CPUAccessFlags = NULL;
	// total size of the buffer
	groundBufferDesc.ByteWidth = sizeof(Model::vertex_Normal) * (UINT)groundModel.uniqueVerts.size();
	groundBufferDesc.MiscFlags = NULL;
	//groundBufferDesc.StructureByteStride = sizeof(OBJ_VERT);

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
	gunIndexBufferDesc.ByteWidth = sizeof(UINT) *(UINT)miniGunModel.uniqueIndexBuffer.size();
	gunIndexBufferDesc.MiscFlags = NULL;
	gunIndexBufferDesc.StructureByteStride = sizeof(UINT);

	// cube index buffer
	D3D11_BUFFER_DESC groundIndexBufferDesc;
	ZeroMemory(&groundIndexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	groundIndexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	groundIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	groundIndexBufferDesc.CPUAccessFlags = NULL;
	groundIndexBufferDesc.ByteWidth = sizeof(UINT) * (UINT)groundModel.uniqueIndexBuffer.size();
	groundIndexBufferDesc.MiscFlags = NULL;
	groundIndexBufferDesc.StructureByteStride = sizeof(const unsigned int);

	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = BACKBUFFER_WIDTH;
	descDepth.Height = BACKBUFFER_HEIGHT;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
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

	//D3D11_TEXTURE2D_DESC gunTextureDesc;

	
	
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	//ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	D3D11_SAMPLER_DESC sampleDesc = {};
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampleDesc.ComparisonFunc = D3D11_COMPARISON_GREATER;
	sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;

    // TODO: PART 2 STEP 3c
	D3D11_SUBRESOURCE_DATA subTriData = {};
	subTriData.pSysMem = triangleCombo;
	subTriData.SysMemPitch = 0;
	subTriData.SysMemSlicePitch = 0;

	D3D11_SUBRESOURCE_DATA triangleindexData;
	triangleindexData.pSysMem = indexOrder;
	triangleindexData.SysMemPitch = 0;
	triangleindexData.SysMemSlicePitch = 0;

	//float3 tempvert[8]; 2483 dragon      2599 gun
	Model::vertex_Normal tempvert[2599];
	//unsigned int vertsize = miniGunModel.uniqueVerts.size();
//	tempvert = new float3[vertsize];
	for (unsigned int i = 0; i < miniGunModel.uniqueVerts.size(); ++i)
	{
		tempvert[i] = miniGunModel.uniqueVerts[i];
	}

	D3D11_SUBRESOURCE_DATA subMiniGunData = {};
	subMiniGunData.pSysMem = tempvert;
	subMiniGunData.SysMemPitch = 0;
	subMiniGunData.SysMemSlicePitch = 0;

	unsigned int tempIndices[14967]; // 14394 dragon       14967 gun
	//unsigned int size = miniGunModel.uniqueIndexBuffer.size();
	//tempIndices = new UINT[size];
	for (unsigned int i = 0; i < miniGunModel.uniqueIndexBuffer.size(); i++)
	{
		tempIndices[i] = miniGunModel.uniqueIndexBuffer[i];
	}

	D3D11_SUBRESOURCE_DATA minigunIndexData = {};
	minigunIndexData.pSysMem = tempIndices;
	minigunIndexData.SysMemPitch = 0;
	minigunIndexData.SysMemSlicePitch = 0;

	Model::vertex_Normal tempGroundVert[4];
	for (unsigned int i = 0; i < groundModel.uniqueVerts.size(); i++)
	{
		tempGroundVert[i] = groundModel.uniqueVerts[i];
	}

	D3D11_SUBRESOURCE_DATA groundSubData = {};
	groundSubData.pSysMem = tempGroundVert;
	groundSubData.SysMemPitch = 0;
	groundSubData.SysMemSlicePitch = 0;

	unsigned int tempGroundIndices[6]; 									
	for (unsigned int i = 0; i < groundModel.uniqueIndexBuffer.size(); i++)
	{
		tempGroundIndices[i] = groundModel.uniqueIndexBuffer[i];
	}
	D3D11_SUBRESOURCE_DATA groundIndexData;
	groundIndexData.pSysMem = tempGroundIndices;
	groundIndexData.SysMemPitch = 0;
	groundIndexData.SysMemSlicePitch = 0;

	

	D3D11_SUBRESOURCE_DATA subTextureData[faceDiff_numlevels] = {};
	for (int i = 0; i < faceDiff_numlevels; i++)
	{
		subTextureData[i].pSysMem = &faceDiff_pixels[faceDiff_leveloffsets[i]];
		subTextureData[i].SysMemPitch = (faceDiff_width >> i)*sizeof(UINT);
		subTextureData[i].SysMemSlicePitch = 0;
	}

	// TODO: PART 2 STEP 3d
	
	// the device is the d3d11 device created earlier used to create all the buffer
	device->CreateBuffer(&triangleBufferDesc, &subTriData, &vertexStarBuffer);
	device->CreateBuffer(&indexBufferDesc, &triangleindexData, &starIndexBuffer);

	device->CreateBuffer(&groundBufferDesc, &groundSubData, &vertexGroundBuffer);
	device->CreateBuffer(&groundIndexBufferDesc, &groundIndexData, &groundIndexBuffer);

	device->CreateBuffer(&gunBufferDesc, &subMiniGunData, &vertexMiniGunBuffer);
	device->CreateBuffer(&gunIndexBufferDesc, &minigunIndexData, &MinigunIndexBuffer);

	device->CreateTexture2D(&descDepth, NULL, &depthStencil);
	device->CreateDepthStencilView(depthStencil, &descDSV, &depthStencilView);
	device->CreateTexture2D(&textureDesc, subTextureData, &texturesArray);
	//device->CreateBuffer(&textureDesc, subTextureData, &textureBuffer);
	device->CreateShaderResourceView(texturesArray, nullptr, &SRV);

	// loading using the ddstexture
	CreateDDSTextureFromFile(device, L"../Graphics II Project/sbv9148irj-Deadpool/Deadpool/WEP_MP7_TEXTSET_Color_NormX1.dds", NULL, &GunSRV );
	CreateDDSTextureFromFile(device, L"texture3.dds", NULL, &GroundSRV);


	// ADD SHADERS TO PROJECT, SET BUILD OPTIONS & COMPILE
	//delete tempvert;
	//delete tempIndices;
	//

	// TODO: PART 2 STEP 7
	
	device->CreateVertexShader(Trivial_VS, sizeof(Trivial_VS), nullptr, &vertexShader);
	device->CreatePixelShader(Trivial_PS, sizeof(Trivial_PS), nullptr, &pixelShader);
	device->CreateVertexShader(Star_VS, sizeof(Star_VS), nullptr, &StarVertexShader);
	device->CreatePixelShader(Star_PS, sizeof(Star_PS), nullptr, &StarPIxelShader);

	// TODO: PART 2 STEP 8a
	
	D3D11_INPUT_ELEMENT_DESC vLayout[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{ "UV",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	};

	D3D11_INPUT_ELEMENT_DESC starLayout[] = 
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};
	// TODO: PART 2 STEP 8b
	//UINT num_elements = sizeof(vLayout) / sizeof(vLayout[0]);
	device->CreateInputLayout(vLayout, ARRAYSIZE(vLayout), Trivial_VS, sizeof(Trivial_VS), &input);
	device->CreateInputLayout(starLayout, ARRAYSIZE(starLayout), Star_VS, sizeof(Star_VS), &starInput);
	
	// TODO: PART 3 STEP 3

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

	device->CreateBuffer(&constbufferstuff, NULL, &ConstObjectBuffer);
	device->CreateBuffer(&constbufferstuffScene, NULL, &ConstSceneBuffer);
	//device->CreateBuffer(&constbufferstuffModel, NULL, &ConstModelBuffer);
	
	// TODO: PART 3 STEP 4b
	ground.worldMatrix =  XMMatrixIdentity();
	ground.worldMatrix = XMMatrixTranslation(0, -1, 0);
	star.worldMatrix = XMMatrixIdentity();
	star.worldMatrix = XMMatrixTranslation(-4, 3, 8);
	miniGun.worldMatrix = XMMatrixIdentity();
	// scaling from the identity down to .05 to make it smaller
	miniGun.worldMatrix.r[0].m128_f32[0] = .05f;
	miniGun.worldMatrix.r[1].m128_f32[1] = .05f;
	miniGun.worldMatrix.r[2].m128_f32[2] = .05f;
	// set the position of xyz 
	miniGun.worldMatrix.r[3].m128_f32[0] = 0;
	miniGun.worldMatrix.r[3].m128_f32[1] = 0;
	miniGun.worldMatrix.r[3].m128_f32[2] = 8;
	//miniGun.worldMatrix = XMMatrixTranslation(5, 5, 5);
	camera.projectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(65),AspectRatio,zNear,zFar);
	camera.viewMatrix = XMMatrixInverse(NULL, XMMatrixIdentity());
	//miniGun.worldMatrix = XMMatrixMultiply(XMMatrixRotationY(XMConvertToRadians(90)), miniGun.worldMatrix);
	//miniGun.worldMatrix = XMMatrixMultiply(XMMatrixRotationX(XMConvertToRadians(270)), miniGun.worldMatrix);

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

	//ground.worldMatrix = XMMatrixMultiply(XMMatrixRotationY(timer.Delta()), Cube.worldMatrix);
	star.worldMatrix = XMMatrixMultiply(XMMatrixRotationY(timer.Delta()), star.worldMatrix);
	//miniGun.worldMatrix = XMMatrixMultiply(XMMatrixRotationY(90), miniGun.worldMatrix);
		
	// set the render target equal to the backbuffer for use bind one or more render targets atomically
	// (number of render targets to set(usually 1), pointer to list of viewable objects, the depthstencilview(if null it is not bound))
	//context->OMSetRenderTargets(1, &rtv, NULL);

	context->OMSetRenderTargets(1, &rtv, depthStencilView);
	
	// TODO: PART 1 STEP 7b
	
	// set the device to draw at the viewports starting position
	context->RSSetViewports(1, &viewport);

	// TODO: PART 1 STEP 7c
	
	// clear the screen
	context->ClearRenderTargetView(rtv, &(backcolor.a,backcolor.g,backcolor.b,backcolor.r));
	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1, 0);

	 //TODO: PART 5 STEP 4
#pragma region movement

	camera.viewMatrix = VS_ViewMatrix;

	// local =	rot/trans/scale  TIMES   matrix
	// global = MATRIX			 TIMES   rot/trans/scale

	if (GetAsyncKeyState('W')) // move forward
	{
		XMVECTOR temp = { 0, 0, 1*timer.Delta(), camera.viewMatrix.r[3].m128_f32[3] };
		camera.viewMatrix = XMMatrixMultiply(XMMatrixTranslationFromVector(temp), camera.viewMatrix);
	}
	if (GetAsyncKeyState('S')) // move backward
	{
		XMVECTOR temp = { 0, 0,1* -timer.Delta(), camera.viewMatrix.r[3].m128_f32[3] };
		camera.viewMatrix = XMMatrixMultiply(XMMatrixTranslationFromVector(temp), camera.viewMatrix);
	}
	if (GetAsyncKeyState('A')) // look left
	{
		XMVECTOR temp = { camera.viewMatrix.r[3].m128_f32[0], camera.viewMatrix.r[3].m128_f32[1], camera.viewMatrix.r[3].m128_f32[2], camera.viewMatrix.r[3].m128_f32[3] };
		
		camera.viewMatrix.r[3].m128_f32[0] = 0;
		camera.viewMatrix.r[3].m128_f32[1] = 0;
		camera.viewMatrix.r[3].m128_f32[2] = 0;
		camera.viewMatrix.r[3].m128_f32[3] = 1;

		camera.viewMatrix = XMMatrixMultiply(camera.viewMatrix, XMMatrixRotationY(-timer.Delta()));
		camera.viewMatrix.r[3].m128_f32[0] = temp.m128_f32[0];
		camera.viewMatrix.r[3].m128_f32[1] = temp.m128_f32[1];
		camera.viewMatrix.r[3].m128_f32[2] = temp.m128_f32[2];

	}
	if (GetAsyncKeyState('D')) // look right
	{
		XMVECTOR temp = { camera.viewMatrix.r[3].m128_f32[0], camera.viewMatrix.r[3].m128_f32[1], camera.viewMatrix.r[3].m128_f32[2], camera.viewMatrix.r[3].m128_f32[3] };

		camera.viewMatrix.r[3].m128_f32[0] = 0;
		camera.viewMatrix.r[3].m128_f32[1] = 0;
		camera.viewMatrix.r[3].m128_f32[2] = 0;
		camera.viewMatrix.r[3].m128_f32[3] = 1;

		camera.viewMatrix = XMMatrixMultiply(camera.viewMatrix, XMMatrixRotationY(timer.Delta()));
		camera.viewMatrix.r[3].m128_f32[0] = temp.m128_f32[0];
		camera.viewMatrix.r[3].m128_f32[1] = temp.m128_f32[1];
		camera.viewMatrix.r[3].m128_f32[2] = temp.m128_f32[2];
	}
	if (GetAsyncKeyState('Q')) // strafe left
	{
		XMVECTOR temp = { -1 * timer.Delta(), 0, 0, camera.viewMatrix.r[3].m128_f32[3] };
		camera.viewMatrix = XMMatrixMultiply(XMMatrixTranslationFromVector(temp), camera.viewMatrix);
	}
	if (GetAsyncKeyState('E')) // strafe right
	{
		XMVECTOR temp = { 1 * timer.Delta(), 0, 0, camera.viewMatrix.r[3].m128_f32[3] };
		camera.viewMatrix = XMMatrixMultiply(XMMatrixTranslationFromVector(temp),camera.viewMatrix);
	}
	if (GetAsyncKeyState('R'))
	{
		XMVECTOR temp = { camera.viewMatrix.r[3].m128_f32[0], camera.viewMatrix.r[3].m128_f32[1], camera.viewMatrix.r[3].m128_f32[2], camera.viewMatrix.r[3].m128_f32[3] };

		camera.viewMatrix.r[3].m128_f32[0] = 0;
		camera.viewMatrix.r[3].m128_f32[1] = 0;
		camera.viewMatrix.r[3].m128_f32[2] = 0;
		camera.viewMatrix.r[3].m128_f32[3] = 1;

		camera.viewMatrix = XMMatrixMultiply(XMMatrixRotationX(timer.Delta()), camera.viewMatrix);
		camera.viewMatrix.r[3].m128_f32[0] = temp.m128_f32[0];
		camera.viewMatrix.r[3].m128_f32[1] = temp.m128_f32[1];
		camera.viewMatrix.r[3].m128_f32[2] = temp.m128_f32[2];
	}
	if (GetAsyncKeyState('V'))
	{
		XMVECTOR temp = { camera.viewMatrix.r[3].m128_f32[0], camera.viewMatrix.r[3].m128_f32[1], camera.viewMatrix.r[3].m128_f32[2], camera.viewMatrix.r[3].m128_f32[3] };

		camera.viewMatrix.r[3].m128_f32[0] = 0;
		camera.viewMatrix.r[3].m128_f32[1] = 0;
		camera.viewMatrix.r[3].m128_f32[2] = 0;
		camera.viewMatrix.r[3].m128_f32[3] = 1;

		camera.viewMatrix = XMMatrixMultiply(XMMatrixRotationX(-timer.Delta()), camera.viewMatrix);
		camera.viewMatrix.r[3].m128_f32[0] = temp.m128_f32[0];
		camera.viewMatrix.r[3].m128_f32[1] = temp.m128_f32[1];
		camera.viewMatrix.r[3].m128_f32[2] = temp.m128_f32[2];
	}
	if (GetAsyncKeyState('Z')) // strafe left
	{
		XMVECTOR temp = {0,  1 * timer.Delta(), 0, camera.viewMatrix.r[3].m128_f32[3] };
		camera.viewMatrix = XMMatrixMultiply(XMMatrixTranslationFromVector(temp), camera.viewMatrix);
	}
	if (GetAsyncKeyState('X')) // strafe right
	{
		XMVECTOR temp = {0,  -1 * timer.Delta(), 0, camera.viewMatrix.r[3].m128_f32[3] };
		camera.viewMatrix = XMMatrixMultiply(XMMatrixTranslationFromVector(temp), camera.viewMatrix);
	}
	//if (GetAsyncKeyState(MOUSEEVENTF_RIGHTDOWN))
	//{
	//	currentPos = GetCursorPos();
	//}

	VS_ViewMatrix = camera.viewMatrix;

#pragma endregion

	
	// TODO: PART 3 STEP 5
	
	//D3D11_MAPPED_SUBRESOURCE map;
	//context->Map(ConstObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	//memcpy(map.pData, &Cube, sizeof(OBJECT_TO_VRAM));
	//context->Unmap(ConstObjectBuffer, 0);

	camera.viewMatrix = XMMatrixInverse(NULL, VS_ViewMatrix);

	D3D11_MAPPED_SUBRESOURCE map2;
	context->Map(ConstSceneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map2);
	memcpy(map2.pData, &camera, sizeof(SCENE_TO_VRAM));
	context->Unmap(ConstSceneBuffer, 0);

	// TODO: PART 3 STEP 6
	// 0 and 1 corelate to the buffer 0 or 1 in the vertex shader HLSL
	context->VSSetConstantBuffers(0, 1, &ConstObjectBuffer);
	context->VSSetConstantBuffers(1, 1, &ConstSceneBuffer);
	//context->VSSetConstantBuffers(2, 1, &ConstModelBuffer);

	// TODO: PART 2 STEP 9a
	//// cube from header
	//UINT stride = sizeof(OBJ_VERT);
	//// triangle created
	////UINT stride = sizeof(SIMPLE_VERTEX);
	//UINT offset = 0;
	//context->IASetVertexBuffers(0, 1, &vertexCubeBuffer,&stride, &offset);
	//context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// TODO: PART 2 STEP 9b

	context->VSSetShader(vertexShader, NULL, 0);
	context->PSSetShader(pixelShader, NULL, 0);
	//context->PSSetShaderResources(0, 1, &SRV);
	context->PSSetSamplers(0, 1, &sampleState);
	
	// TODO: PART 2 STEP 9c
	
	context->IASetInputLayout(input);

	// TODO: PART 2 STEP 9d
	
	//context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// TODO: PART 2 STEP 10
	
	//context->DrawIndexed(1692, 0, 0);

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
	// for the anti aliseing
	//context->RSSetState(rasterizerState);
	context->DrawIndexed(60, 0, 0);

	D3D11_MAPPED_SUBRESOURCE gunMap;
	context->Map(ConstObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gunMap);
	memcpy(gunMap.pData, &miniGun, sizeof(OBJECT_TO_VRAM));
	context->Unmap(ConstObjectBuffer, 0);
	UINT gunstride = sizeof(Model::vertex_Normal);
	UINT gunOffset = 0;
	context->IASetVertexBuffers(0, 1, &vertexMiniGunBuffer, &gunstride, &gunOffset);
	context->IASetIndexBuffer(MinigunIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetInputLayout(input);
	context->PSSetShaderResources(0, 1, &GunSRV);
	context->VSSetShader(vertexShader, NULL, 0);
	context->PSSetShader(pixelShader, NULL, 0);
	context->DrawIndexed(miniGunModel.uniqueIndexBuffer.size(), 0, 0);


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
	context->PSSetShader(pixelShader, NULL, 0);
	context->DrawIndexed(groundModel.uniqueIndexBuffer.size(), 0, 0);
	// END PART 2

	// TODO: PART 1 STEP 8

	// put the backbuffer to the front buffer
	swapchain->Present(0, 0);


	// END OF PART 1
	return true; 
}

//************************************************************
//************ DESTRUCTION ***********************************
//************************************************************

bool DEMO_APP::ShutDown()
{
	// TODO: PART 1 STEP 6

#define SAFE_RELEASE(P) {if(P){P->Release(); P=nullptr;}}

	SAFE_RELEASE(swapchain);
	SAFE_RELEASE(device);
	SAFE_RELEASE(context);
	SAFE_RELEASE(rtv);
	//SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(vertexShader);
	SAFE_RELEASE(pixelShader);
	SAFE_RELEASE(input);
	SAFE_RELEASE(ConstObjectBuffer);
	SAFE_RELEASE(ConstSceneBuffer);



	UnregisterClass( L"DirectXApplication", application ); 
	return true;
}

//************************************************************
//************ WINDOWS RELATED *******************************
//************************************************************

// ****************** BEGIN WARNING ***********************// 
// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY!
	
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine,	int nCmdShow );						   
LRESULT CALLBACK WndProc(HWND hWnd,	UINT message, WPARAM wparam, LPARAM lparam );		
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE, LPTSTR, int )
{
	srand(unsigned int(time(0)));
	DEMO_APP myApp(hInstance,(WNDPROC)WndProc);	
    MSG msg; ZeroMemory( &msg, sizeof( msg ) );
    while ( msg.message != WM_QUIT && myApp.Run() )
    {	
	    if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        { 
            TranslateMessage( &msg );
            DispatchMessage( &msg ); 
        }
    }
	myApp.ShutDown(); 
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
    }
    return DefWindowProc( hWnd, message, wParam, lParam );
}
//********************* END WARNING ************************//