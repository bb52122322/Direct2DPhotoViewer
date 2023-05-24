#pragma once

#ifndef UNICODE
#define UNICODE
#endif
#include <windows.h>
#include <windowsx.h>
#include <iostream>
#include <numbers>
#pragma comment(lib, "user32.lib")

#include <wrl.h>
using namespace Microsoft::WRL;
#include <dxgi1_3.h>
#include <d3d11_2.h>
#include <d2d1_2.h>
#include <d2d1_2helper.h>
#include <dcomp.h>
#include <tchar.h>
#include <wincodec.h>
#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dcomp")

struct ComException
{
	HRESULT result;
	ComException(HRESULT const value) :
		result(value)
	{}
};

inline void HR(HRESULT const result)
{
	if (S_OK != result)
	{
		throw ComException(result);
	}
}