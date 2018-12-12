#include "Input.h"



Input::Input(int screenWidth, int screenHeight)
{
	m_screenHeight = screenHeight;
	m_screenWidth = screenWidth;
}


Input::~Input()
{
	if (m_mouse_device)
	{
		m_mouse_device->Unacquire();
		m_mouse_device->Release();
	}
	if (m_keyboard_device)
	{
		m_keyboard_device->Unacquire();
		m_keyboard_device->Release();
	}
	if (m_direct_input) m_direct_input->Release();
}

HRESULT Input::Initialise(HINSTANCE	hInst, HWND hWnd)
{
	HRESULT hr;
	ZeroMemory(m_keyboard_keys_state, sizeof(m_keyboard_keys_state));
	hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_direct_input, NULL);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = m_direct_input->CreateDevice(GUID_SysKeyboard, &m_keyboard_device, NULL);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = m_keyboard_device->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr))
	{
		return hr;
	}


	hr = m_keyboard_device->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = m_keyboard_device->Acquire();
	if (FAILED(hr))
	{
		return hr;
	}

	hr = m_direct_input->CreateDevice(GUID_SysMouse, &m_mouse_device, NULL);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = m_mouse_device->SetDataFormat(&c_dfDIMouse);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = m_mouse_device->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = m_mouse_device->Acquire();
	if (FAILED(hr))
	{
		return hr;
	}

	return S_OK;
}

void Input::ReadInputStates()
{
	HRESULT hr;
	hr = m_keyboard_device->GetDeviceState(sizeof(m_keyboard_keys_state), (LPVOID)&m_keyboard_keys_state);
	if (FAILED(hr))
	{
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			m_keyboard_device->Acquire();
		}
	}

	hr = m_mouse_device->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouse_state);
	if (FAILED(hr))
	{
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			m_mouse_device->Acquire();
		}
	}



	// Update the location of the mouse cursor based on the change of the mouse location during the frame.
	m_mouseX += m_mouse_state.lX;
	m_mouseY += m_mouse_state.lY;

	// Ensure the mouse location doesn't exceed the screen width or height.
	if (m_mouseX < 0) 
	{ 
		m_mouseX = 0; 
	}
	if (m_mouseY < 0) 
	{
		m_mouseY = 0; 
	}

	if (m_mouseX > m_screenWidth) 
	{ 
		m_mouseX = m_screenWidth; 
	}
	if (m_mouseY > m_screenHeight) 
	{ 
		m_mouseY = m_screenHeight; 
	}

}

bool Input::KeyIsPressed(unsigned char DI_keycode)
{
	return m_keyboard_keys_state[DI_keycode] & 0x80;
}

void Input::updateScreenSize(int screenWidth, int screenHeight)
{
	m_screenHeight = screenHeight;
	m_screenWidth = screenWidth;
}

XMFLOAT2 Input::GetMouseDelta()
{
	return XMFLOAT2((m_mouse_state.lX * m_sensitivity), ((-m_mouse_state.lY) * m_sensitivity));
}
