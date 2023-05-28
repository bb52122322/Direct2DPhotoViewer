#pragma once
#include "framework.hpp"
#include "Image.hpp"

namespace {
	constexpr float SCALE_SPEED_KEY = 128.0 / 125;
	constexpr float SCALE_SPEED_WHEEL = SCALE_SPEED_KEY / 25;
	constexpr float SCALE_MAX = 10;
	constexpr float SCALE_MIN = 0.05;
	constexpr float ROTATE_UNIT = 3.0;
	constexpr float WINDOW_MOVE_SPEED = 20;
	constexpr float WINDOW_SIZE_SPEED = 20;
	const D2D1_SIZE_F WINDOW_SIZE_MAX = { GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN) };
	constexpr D2D1_SIZE_F WINDOW_SIZE_MIN = { 100,100 };
}

class Direct2DWindow {
public:
	Direct2DWindow(HINSTANCE module);

	void run();

private:
	HINSTANCE module;
	HWND window;
	ComPtr<ID3D11Device> direct3dDevice;
	ComPtr<IDXGIDevice> dxgiDevice;
	ComPtr<IDXGIFactory2> dxFactory;
	ComPtr<IDXGISwapChain1> swapChain;
	ComPtr<ID2D1Factory2> d2Factory;
	ComPtr<ID2D1Device1> d2Device;
	ComPtr<ID2D1DeviceContext> dc;
	ComPtr<IDCompositionDevice> dcompDevice;
	ComPtr<IDCompositionTarget> target;
	ComPtr<IDCompositionVisual> visual;
	ComPtr<ID2D1Bitmap1> bitmap_;

	
	Image img_;
	std::wstring filename_;
	D2D1_POINT_2F imagepos_ = { 0,0 };
	D2D1_SIZE_F scale_ = { 1,1 };
	float rot_ = 0;
	float alpha_ = 1;
	int bgcolor_ = 2;
	bool hiquarity_ = true;
	bool topmost_ = false;
	bool enableEvent = true;


	// mouse events
	D2D1_POINT_2L prevcursorpos_;


	void OnRender();
	void OnKeyboard();
	void OnMouse(const LPARAM lParam, int wheel = 0);
	void OnResize(UINT width, UINT height);

	void ImageResetEvent();
	void ImageScaleEvent(const float scale, D2D1_POINT_2F center = {0,0});
	void ImageRotateEvent(const float rot);
	void WindowMoveEvent(const D2D1_POINT_2F moveVec);
	void WindowSizeEvent(const D2D1_SIZE_F sizeDelta);

	void OpenFileEvent(const bool new_window = true);
	void DroppedFileEvent(const std::wstring filepath);

	LRESULT CALLBACK WindowProcInstance(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
	inline static LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	ATOM RegisterWindowClass();
	void InitInstance();
	void setupDirect2D();

};


