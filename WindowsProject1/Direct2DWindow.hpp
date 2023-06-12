#pragma once
#include "framework.hpp"
#include "Image.hpp"

namespace {
	constexpr auto SCALE_SPEED_KEY = 128 / 125.0;
	constexpr auto SCALE_SPEED_WHEEL = SCALE_SPEED_KEY / 25;
	constexpr auto SCALE_MAX = 10;
	constexpr auto SCALE_MIN = 0.05;
	constexpr auto ROTATE_UNIT = 3;
	constexpr auto WINDOW_MOVE_SPEED = 20;
	constexpr auto WINDOW_SIZE_SPEED = 10;
	const D2D1_SIZE_F WINDOW_SIZE_MAX = { 
		static_cast<float>(GetSystemMetrics(SM_CXSCREEN)),
		static_cast<float>(GetSystemMetrics(SM_CYSCREEN)) };
	constexpr D2D1_SIZE_F WINDOW_SIZE_MIN = { 100,100 };
}

class Direct2DWindow {
public:
	Direct2DWindow(const HINSTANCE hInstance);

	void run();

private:
	HINSTANCE hInstance_;
	HWND hWnd_;
	ComPtr<ID3D11Device> d3dDevice_;
	ComPtr<IDXGIDevice> dxgiDevice_;
	ComPtr<IDXGIFactory2> dxgiFactory_;
	ComPtr<IDXGISwapChain1> dxgiSwapChain_;
	ComPtr<ID2D1Factory2> d2Factory_;
	ComPtr<ID2D1Device1> d2dDevice_;
	ComPtr<ID2D1DeviceContext> d2dDeviceContext_;
	ComPtr<IDCompositionDevice> dCompositionDevice_;
	ComPtr<IDCompositionTarget> dCompositionTarget_;
	ComPtr<IDCompositionVisual> dCompositionVisual_;
	ComPtr<ID2D1Bitmap1> bitmap_;

	
	Image image_;
	std::wstring filepath_;
	D2D1_POINT_2F imagepos_ = { 0,0 };
	D2D1_SIZE_F scale_ = { 1,1 };
	float rot_ = 0.0f;
	float alpha_ = 1.0f;
	int bgcolor_ = 2;
	bool hiquarity_ = true;
	bool topmost_ = false;
	bool enableEvent_ = true;


	// mouse events
	D2D1_POINT_2L prevcursorpos_;


	void OnRender();
	void OnKeyboard();
	void OnMouse(const LPARAM lParam, const int wheel = 0);
	void OnResize(const UINT width, const UINT height);

	void ImageResetEvent();
	void ImageScaleEvent(const float scale, D2D1_POINT_2F center = {0,0});
	void ImageRotateEvent(const float rot);
	void WindowMoveEvent(const D2D1_POINT_2F moveVec);
	void WindowSizeEvent(const D2D1_SIZE_F sizeDelta);

	void OpenFileEvent(const bool new_window = true);
	void DroppedFileEvent(const std::wstring filepath, const bool newWindow=true);

	LRESULT CALLBACK WindowProcInstance(const HWND hWnd, const UINT msg, const WPARAM wparam, const LPARAM lparam);
	inline static LRESULT CALLBACK WindowProc(const HWND hWnd, const UINT msg, const WPARAM wParam, const LPARAM lParam);

	ATOM RegisterWindowClass();
	void InitInstance();
	void setupDirect2D();

};


