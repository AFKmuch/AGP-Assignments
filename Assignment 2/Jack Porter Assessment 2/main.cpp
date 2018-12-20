//The #include order is important
#include <d3d11.h>
#include <dxgi.h>
#include <d3dx11.h>
#include <windows.h>
#include <dxerr.h>
#include <vector>
// defines allows more compatible code, precede all xnamath includes with these defines
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>
#include "Camera.h"
#include "text2D.h"
#include "Input.h"
#include "Physics.h"
#include "Time.h"
#include "SkyBox.h"

//////////////////////////////////////////////////////////////////////////////////////
//	Global Variables
//////////////////////////////////////////////////////////////////////////////////////
HINSTANCE	g_hInst = NULL;
HWND		g_hWnd = NULL;
Time*		Time::s_instance = 0;
D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*           g_pD3DDevice = NULL;
ID3D11DeviceContext*    g_pImmediateContext = NULL;
IDXGISwapChain*         g_pSwapChain = NULL;
ID3D11RenderTargetView* g_pBackBufferRTView = NULL;
float g_clear_colour[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
float screenWidth = 1600;
float screenHeight = 900;
ID3D11Buffer*		g_pVertexBuffer;
ID3D11VertexShader*	g_pVertexShader;
ID3D11PixelShader*	g_pPixelShader;
ID3D11InputLayout*	g_pInputLayout;

struct CONSTANT_BUFFER0
{
	XMMATRIX WorldViewProjection; // 64 bytes! Wow!
	XMVECTOR directional_light_vector; // 16
	XMVECTOR directional_light_colour; // 16
	XMVECTOR ambient_light_colour; // 16
};

ID3D11Buffer* g_pConstantBuffer0;
ID3D11DepthStencilView* g_pZBuffer;

Camera* g_pCamera;

XMVECTOR g_directional_light_shines_from = XMVectorSet(0.5f, 1.0f, -1.0f, 0.0f);
XMVECTOR g_directional_light_colour;
XMVECTOR g_ambient_light_colour;

std::vector<Entity*>* g_pEntityList;
Player* g_pPlayer;
Physics* g_pEntity1;
SkyBox* g_pSkyBox;

Input* g_pInput;

// Rename for each tutorial – This will appear in the title bar of the window
char		g_TutorialName[100] = "CGP600 AE2 Jack Porter\0";


//////////////////////////////////////////////////////////////////////////////////////
//	Forward declarations
//////////////////////////////////////////////////////////////////////////////////////
HRESULT InitialiseWindow(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT InitialiseD3D();
void ShutdownD3D();
void RenderFrame(void);
HRESULT InitialiseGraphics(void);

//////////////////////////////////////////////////////////////////////////////////////
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (FAILED(InitialiseWindow(hInstance, nCmdShow)))
	{
		DXTRACE_MSG("Failed to create Window");
		return 0;
	}

	g_pInput = new Input(screenWidth, screenHeight);
	if (FAILED(g_pInput->Initialise(g_hInst, g_hWnd)))
	{
		DXTRACE_MSG("Failed to create Input");
		return 0;
	}

	if (FAILED(InitialiseD3D()))
	{
		DXTRACE_MSG("Failed to create Device");
		return 0;
	}

	if (FAILED(InitialiseGraphics()))
	{
		DXTRACE_MSG("Failed to initialise graphics");
		return 0;
	}
	// Main message loop
	MSG msg = { 0 };
	Time::Instance()->Reset();
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Time::Instance()->Tick();
			RenderFrame();
		}
	}

	ShutdownD3D();

	return (int)msg.wParam;
}


//////////////////////////////////////////////////////////////////////////////////////
// Register class and create window
//////////////////////////////////////////////////////////////////////////////////////
HRESULT InitialiseWindow(HINSTANCE hInstance, int nCmdShow)
{
	// Give your app your own name
	char Name[100] = "Jack Porter AE2\0";

	// Register class
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	//   wcex.hbrBackground = (HBRUSH )( COLOR_WINDOW + 1); // Needed for non-D3D apps
	wcex.lpszClassName = Name;

	if (!RegisterClassEx(&wcex)) return E_FAIL;

	// Create window
	g_hInst = hInstance;
	RECT rc = { 0, 0, screenWidth, screenHeight };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	g_hWnd = CreateWindow(Name, g_TutorialName, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left,
		rc.bottom - rc.top, NULL, NULL, hInstance, NULL);
	if (!g_hWnd)
		return E_FAIL;

	ShowWindow(g_hWnd, nCmdShow);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////////////////
// Called every time the application receives a message
//////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_SIZE:
		if (g_pSwapChain)
		{
			g_pImmediateContext->OMSetRenderTargets(0, 0, 0);

			//Release all outstanding references to the swap chain buffer
			g_pBackBufferRTView->Release();
			g_pZBuffer->Release();

			HRESULT hr;
			// Preserve the existing buffer count and format.
			// Automatically choose the width and height to match the client rect for HWNDs.
			hr = g_pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

			// Get buffer and create new render-target-view
			ID3D11Texture2D* pBuffer;
			hr = g_pSwapChain->GetBuffer(0, _uuidof(ID3D11Texture2D), (void**)&pBuffer);

			hr = g_pD3DDevice->CreateRenderTargetView(pBuffer, NULL, &g_pBackBufferRTView);
			pBuffer->Release();


			// Create Z buffer texture
			D3D11_TEXTURE2D_DESC tex2dDesc;
			ZeroMemory(&tex2dDesc, sizeof(tex2dDesc));

			tex2dDesc.Width = LOWORD(lParam);
			tex2dDesc.Height = HIWORD(lParam);
			tex2dDesc.ArraySize = 1;
			tex2dDesc.MipLevels = 1;
			tex2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			tex2dDesc.SampleDesc.Count = 1;
			tex2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			tex2dDesc.Usage = D3D11_USAGE_DEFAULT;

			ID3D11Texture2D *pZBufferTexture;
			hr = g_pD3DDevice->CreateTexture2D(&tex2dDesc, NULL, &pZBufferTexture);
			if (FAILED(hr)) return hr;

			// create the z buffer
			D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
			ZeroMemory(&dsvDesc, sizeof(dsvDesc));
			dsvDesc.Format = tex2dDesc.Format;
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

			g_pD3DDevice->CreateDepthStencilView(pZBufferTexture, &dsvDesc, &g_pZBuffer);
			pZBufferTexture->Release();


			g_pImmediateContext->OMSetRenderTargets(1, &g_pBackBufferRTView, g_pZBuffer);

			// Set up the viewport.
			D3D11_VIEWPORT vp;
			vp.Width = LOWORD(lParam);
			vp.Height = HIWORD(lParam);
			screenWidth = LOWORD(lParam);
			screenHeight = HIWORD(lParam);
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;
			g_pImmediateContext->RSSetViewports(1, &vp);
			g_pInput->updateScreenSize(LOWORD(lParam), HIWORD(lParam));
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////
// Create D3D device and swap chain
//////////////////////////////////////////////////////////////////////////////////////
HRESULT InitialiseD3D()
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(g_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE, // comment out this line if you need to test D3D 11.0 functionality on hardware that doesn't support it
		D3D_DRIVER_TYPE_WARP, // comment this out also to use reference device
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = _ARRAYSIZE(driverTypes); //Changed to _ARRAYSIZE to stop error appearing on home PC

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = _ARRAYSIZE(featureLevels); //Changed to _ARRAYSIZE to stop error appearing on home PC

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = true;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, g_driverType, NULL,
			createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &g_pSwapChain,
			&g_pD3DDevice, &g_featureLevel, &g_pImmediateContext);
		if (SUCCEEDED(hr))
			break;
	}

	if (FAILED(hr))
		return hr;

	// Get pointer to back buffer texture
	ID3D11Texture2D *pBackBufferTexture;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		(LPVOID*)&pBackBufferTexture);

	if (FAILED(hr)) return hr;

	// Use the back buffer texture pointer to create the render target view
	hr = g_pD3DDevice->CreateRenderTargetView(pBackBufferTexture, NULL, &g_pBackBufferRTView);
	pBackBufferTexture->Release();

	if (FAILED(hr)) return hr;

	// Create Z buffer texture
	D3D11_TEXTURE2D_DESC tex2dDesc;
	ZeroMemory(&tex2dDesc, sizeof(tex2dDesc));

	tex2dDesc.Width = width;
	tex2dDesc.Height = height;
	tex2dDesc.ArraySize = 1;
	tex2dDesc.MipLevels = 1;
	tex2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	tex2dDesc.SampleDesc.Count = sd.SampleDesc.Count;
	tex2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	tex2dDesc.Usage = D3D11_USAGE_DEFAULT;

	ID3D11Texture2D *pZBufferTexture;
	hr = g_pD3DDevice->CreateTexture2D(&tex2dDesc, NULL, &pZBufferTexture);
	if (FAILED(hr)) return hr;

	// create the z buffer
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));
	dsvDesc.Format = tex2dDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	g_pD3DDevice->CreateDepthStencilView(pZBufferTexture, &dsvDesc, &g_pZBuffer);
	pZBufferTexture->Release();

	// Set the render target view
	g_pImmediateContext->OMSetRenderTargets(1, &g_pBackBufferRTView, g_pZBuffer);

	// Set the viewport
	D3D11_VIEWPORT viewport;

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (FLOAT)width;
	viewport.Height = (FLOAT)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	g_pImmediateContext->RSSetViewports(1, &viewport);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////////////////
// Clean up D3D objects
//////////////////////////////////////////////////////////////////////////////////////
void ShutdownD3D()
{
	if (g_pCamera) delete g_pCamera;
	if (g_pConstantBuffer0) g_pConstantBuffer0->Release();
	if (g_pVertexBuffer) g_pVertexBuffer->Release();
	if (g_pInputLayout) g_pInputLayout->Release();
	if (g_pVertexShader) g_pVertexShader->Release();
	if (g_pPixelShader) g_pPixelShader->Release();
	if (g_pBackBufferRTView) g_pBackBufferRTView->Release();
	if (g_pSwapChain) g_pSwapChain->Release();
	if (g_pImmediateContext) g_pImmediateContext->Release();
	if (g_pD3DDevice) g_pD3DDevice->Release();
}

/////////////////////////////////////////////////////////////////////////////////////////////
//Init graphics - Tutorial 03
/////////////////////////////////////////////////////////////////////////////////////////////
HRESULT InitialiseGraphics()
{
	g_pEntityList = new std::vector<Entity*>();
	g_pPlayer = new Player(g_pEntityList, 1, true, g_pInput);
	g_pEntityList->push_back(g_pPlayer);
	g_pPlayer->SetUpModel(g_pD3DDevice, g_pImmediateContext, (char*)"assets/Player.obj", (char*)"assets/texture.bmp"); //PlayerTexture.bmp is cursed
	g_pPlayer->SetPosition(0, 15, 50);
	g_pPlayer->SetVelocity(0, 1, 0);
	g_pEntity1 = new Physics(g_pEntityList, 1, true);
	g_pEntityList->push_back(g_pEntity1);
	g_pEntity1->SetUpModel(g_pD3DDevice, g_pImmediateContext, (char*)"assets/sphere.obj", (char*)"assets/texture.bmp");
	g_pEntity1->SetPosition(0, 0, 50);
	
	HRESULT hr = S_OK;

	// create constant buffer
	D3D11_BUFFER_DESC constant_buffer_desc;
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));

	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	constant_buffer_desc.ByteWidth = 112;
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	hr = g_pD3DDevice->CreateBuffer(&constant_buffer_desc, NULL, &g_pConstantBuffer0);
	if (FAILED(hr))//Return an error code if failed
	{
		return hr;
	}

	g_pCamera = new Camera(0.0f, 0.0f, -0.5f, 0.0f);
	g_pCamera->SetUpPlayerFollow(g_pPlayer, 50, 75, -45);
	g_pSkyBox = new SkyBox(g_pD3DDevice, g_pImmediateContext, g_pCamera);
	g_pSkyBox->SetUp((char*)"assets/skybox02.dds");
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////////////////////


// Render frame
void RenderFrame(void)
{
	g_pInput->ReadInputStates();
	if (g_pInput->KeyIsPressed(DIK_ESCAPE))
	{
		DestroyWindow(g_hWnd);
	}

	// Clear the back buffer - choose a colour you like
	g_pImmediateContext->ClearRenderTargetView(g_pBackBufferRTView, g_clear_colour);

	g_pImmediateContext->ClearDepthStencilView(g_pZBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);



	g_directional_light_colour = XMVectorSet(0.9f, 0.9f, 0.9f, 0.0f); // 
	g_ambient_light_colour = XMVectorSet(0.05f, 0.05f, 0.25f, 1.0f); // use a small value for ambient

																	 //select which primitive type to use 
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	XMMATRIX projection, view;

	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(70.0), (screenWidth / screenHeight) , 1.0, 1000.0);
	
	g_pCamera->Update();

	view = g_pCamera->GetViewMatrix();
	g_pSkyBox->Draw(&view, &projection);

	for (int i = 0; i < g_pEntityList->size(); i++)
	{
		g_pEntityList->at(i)->Update();
		g_pEntityList->at(i)->UpdateLighting(g_directional_light_shines_from, g_directional_light_colour, g_ambient_light_colour);
		g_pEntityList->at(i)->Draw(&view, &projection);
	}


	// Display what has just been rendered
	g_pSwapChain->Present(0, 0);
}
