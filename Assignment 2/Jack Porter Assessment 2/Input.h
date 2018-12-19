#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <d3dx11.h>
#include <windows.h>
#include <dxerr.h>
// defines allows more compatible code, precede all xnamath includes with these defines
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>
#include <dinput.h>

class Input
{
private:
	IDirectInput8* m_direct_input;
	IDirectInputDevice8 * m_keyboard_device;
	IDirectInputDevice8 * m_mouse_device;
	unsigned char m_keyboard_keys_state[256];
	DIMOUSESTATE m_mouse_state;
	int m_screenWidth, m_screenHeight;
	int m_mouseX, m_mouseY;
	int m_mouseXDelta, m_mouseYDelta;
	float m_sensitivity = 10;
public:
	Input(int screenWidth, int screenHeight);
	~Input();
	HRESULT Initialise(HINSTANCE hInst, HWND hWnd);
	void ReadInputStates();
	bool KeyIsPressed(unsigned char DI_keycode);
	void updateScreenSize(int screenWidth, int screenHeight);
	XMFLOAT2 GetMouseDelta();
	XMFLOAT2 GetMovementAxis();
	XMFLOAT2 GetRotationAxis();
};

