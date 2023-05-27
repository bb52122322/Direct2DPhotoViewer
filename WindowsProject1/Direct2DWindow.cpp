#include "Direct2DWindow.hpp"
#include <format>

Direct2DWindow::Direct2DWindow(HINSTANCE module) :
	module(module),
	img_(Image::Image()) {
	Direct2DWindow::InitInstance();
	Direct2DWindow::setupDirect2D();
	wchar_t buffer[128];
	int argc = 0;
	LPWSTR* argv = ::CommandLineToArgvW(GetCommandLine(), &argc);
	if (argv == nullptr) {
		OutputDebugString(L"FAILED!!!");
	}
	else if (argc > 1) {
		filename_ = std::format(L"{}", argv[1]).c_str();
		img_.Load(dc.Get(), filename_);

	}
	LocalFree(argv);
}


void Direct2DWindow::run() {
	MSG message;

	while (true) {


		while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT) return;
			DispatchMessage(&message);
		}
		OnKeyboard();
		OnRender();

	}
}

void Direct2DWindow::OnKeyboard() {
	static int timer = 0;
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000 && GetAsyncKeyState(VK_F12) & 0x1) {
		enableEvent = !enableEvent;
		auto style = GetWindowLongPtr(window, GWL_EXSTYLE);
		if (enableEvent == true) {
			style &= ~WS_EX_TRANSPARENT;
			SetWindowLongPtr(window, GWL_EXSTYLE, style);
		}
		else {
			style |= WS_EX_TRANSPARENT;
			SetWindowLongPtr(window, GWL_EXSTYLE, style);
		}
	}
	if (GetForegroundWindow() == window) {
		RECT rect;
		if (GetAsyncKeyState(VK_ESCAPE) & 0x1) {
			PostQuitMessage(0);

		}
		// NO CONTROL KEY PRESS
		if (!(GetKeyState(VK_SHIFT) & 0x8000)) {
			// rotate
			if (GetKeyState('A') & 0x8000) {
				rot_ -= 3;
				const auto x = imagepos_.x * std::cos(-3 * std::numbers::pi / 180)
					- imagepos_.y * std::sin(-3 * std::numbers::pi / 180);
				const auto y = imagepos_.x * std::sin(-3 * std::numbers::pi / 180)
					+ imagepos_.y * std::cos(-3 * std::numbers::pi / 180);
				imagepos_.x = x;
				imagepos_.y = y;

			}
			if (GetKeyState('D') & 0x8000) {
				rot_ += 3;
				const auto x = imagepos_.x * std::cos(3 * std::numbers::pi / 180)
					- imagepos_.y * std::sin(3 * std::numbers::pi / 180);
				const auto y = imagepos_.x * std::sin(3 * std::numbers::pi / 180)
					+ imagepos_.y * std::cos(3 * std::numbers::pi / 180);
				imagepos_.x = x;
				imagepos_.y = y;

			}
			//scale
			if (GetKeyState('S') & 0x8000) {
				if (scale_.width > 0) {
					scale_.width = scale_.width * 0.98 < 0.05 ? 0.05 : scale_.width * 0.98;
				}
				else {
					scale_.width = scale_.width * 0.98 > -0.05 ? -0.05 : scale_.width * 0.98;
				}
				if (scale_.height > 0) {
					scale_.height = scale_.height * 0.98 < 0.05 ? 0.05 : scale_.height * 0.98;
				}
				else {
					scale_.height = scale_.height * 0.98 > -0.05 ? -0.05 : scale_.height * 0.98;
				}
				imagepos_.x *= 0.98;
				imagepos_.y *= 0.98;
			}
			if (GetKeyState('W') & 0x8000) {
				if (scale_.width > 0) {
					scale_.width = scale_.width * 1.02 > 10 ? 10 : scale_.width * 1.02;
				}
				else {
					scale_.width = scale_.width * 1.02 < -10 ? -10 : scale_.width * 1.02;
				}
				if (scale_.height > 0) {
					scale_.height = scale_.height * 1.02 > 10 ? 10 : scale_.height * 1.02;
				}
				else {
					scale_.height = scale_.height * 1.02 < -10 ? -10 : scale_.height * 1.02;
				}

				if (std::abs(scale_.width) < 9.99) {
					imagepos_.x *= 1.02;
					imagepos_.y *= 1.02;
				}

			}
			//flip
			if (GetKeyState('Q') & 0x8000) {
				if (timer == 0 || timer > 60) {
					scale_.width *= -1;
				}
				timer++;

			}
			else if (GetKeyState('E') & 0x8000) {
				if (timer == 0 || timer > 60) {
					scale_.height *= -1;
				}
				timer++;
			}
			else {
				timer = 0;
			}
			// Move Window
			if (GetKeyState(VK_UP) & 0x8000) {
				GetWindowRect(window, &rect);
				const int aftertop = rect.top - 20 < 0 ? 0 : rect.top - 20;
				const int afterleft = rect.left;
				SetWindowPos(window, nullptr, afterleft, aftertop, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			}
			if (GetKeyState(VK_LEFT) & 0x8000) {
				GetWindowRect(window, &rect);
				const int aftertop = rect.top;
				const int afterleft = rect.left - 20 < 0 ? 0 : rect.left - 20;
				SetWindowPos(window, nullptr, afterleft, aftertop, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			}
			if (GetKeyState(VK_DOWN) & 0x8000) {
				GetWindowRect(window, &rect);
				const int height = rect.bottom - rect.top;
				const int aftertop = rect.bottom >= GetSystemMetrics(SM_CYSCREEN) ? GetSystemMetrics(SM_CYSCREEN) - height : rect.top + 20;
				const int afterleft = rect.left;
				SetWindowPos(window, nullptr, afterleft, aftertop, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			}
			if (GetKeyState(VK_RIGHT) & 0x8000) {
				GetWindowRect(window, &rect);
				const int width = rect.right - rect.left;
				const int aftertop = rect.top;
				const int afterleft = rect.right >= GetSystemMetrics(SM_CXSCREEN) ? GetSystemMetrics(SM_CXSCREEN) - width : rect.left + 20;
				SetWindowPos(window, nullptr, afterleft, aftertop, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			}
			if (GetAsyncKeyState('B') & 0x1) {
				bgcolor_++;
				if (bgcolor_ > 9) bgcolor_ = 0;
			}
			if (GetAsyncKeyState('L') & 0x1) {
				hiquarity_ = !hiquarity_;
			}
			if (GetAsyncKeyState('T') & 0x1) {
				topmost_ = !topmost_;
				if (topmost_ == true) {
					SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
				}
				else {
					SetWindowPos(window, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
				}
			}
			if (GetAsyncKeyState('N') & 0x1) {
				wchar_t applicationPath[256];
				if (GetModuleFileName(nullptr, applicationPath, 256)) {
					ShellExecute(window, L"open", applicationPath, filename_.c_str(), nullptr, SW_SHOW);
				}
			}
			if (GetAsyncKeyState('O') & 0x1) {
				// 一時的に最上面をOFF
				SetWindowPos(window, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);

				OPENFILENAME ofn;
				wchar_t filename[256] = L"";

				ZeroMemory(&ofn, sizeof(ofn));

				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = nullptr;
				ofn.lpstrFilter = L"All Files (*.*)\0*.*\0";
				ofn.lpstrFile = filename;
				ofn.nMaxFile = 256;
				ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
				ofn.lpstrDefExt = L"";

				if (GetOpenFileName(&ofn))
				{
					wchar_t applicationPath[256];
					if (GetModuleFileName(nullptr, applicationPath, 256)) {
						ShellExecute(window, L"open", applicationPath, filename, nullptr, SW_SHOW);
					}
				}
				if (topmost_ == true) {
					SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
				}
				else {
					SetWindowPos(window, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
				}
			}
			if (GetAsyncKeyState('F') & 0x1) {
				// 一時的に最上面をOFF
				SetWindowPos(window, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);

				OPENFILENAME ofn;
				wchar_t filename[256] = L"";

				ZeroMemory(&ofn, sizeof(ofn));

				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = nullptr;
				ofn.lpstrFilter = L"All Files (*.*)\0*.*\0";
				ofn.lpstrFile = filename;
				ofn.nMaxFile = 256;
				ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
				ofn.lpstrDefExt = L"";

				if (GetOpenFileName(&ofn))
				{
					img_.Load(dc.Get(), filename);
				}
				if (topmost_ == true) {
					SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
				}
				else {
					SetWindowPos(window, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
				}
			}

			if (GetAsyncKeyState('R') & 0x1) {

				imagepos_ = { 0,0 };
				scale_ = { 1,1 };
				rot_ = 0;
				alpha_ = 1;

			}
		}
		// Crtl + XXX
		else {
			timer = 0;
			if (GetKeyState('W') & 0x8000) {
				imagepos_.y -= 10;
			}
			if (GetKeyState('A') & 0x8000) {
				imagepos_.x -= 10;
			}
			if (GetKeyState('S') & 0x8000) {
				imagepos_.y += 10;
			}
			if (GetKeyState('D') & 0x8000) {
				imagepos_.x += 10;
			}
			if (GetKeyState('Q') & 0x8000) {
				alpha_ = alpha_ - 0.01 < 0.2 ? 0.2 : alpha_ - 0.01;
				SetLayeredWindowAttributes(window, 0, 255 * alpha_, LWA_ALPHA);
			}
			if (GetKeyState('E') & 0x8000) {
				alpha_ = alpha_ + 0.01 > 1.00 ? 1 : alpha_ + 0.01;
				SetLayeredWindowAttributes(window, 0, 255 * alpha_, LWA_ALPHA);
			}

			if (GetKeyState(VK_UP) & 0x8000) {
				GetWindowRect(window, &rect);
				RECT afterRect = {
					rect.left,
					rect.top - 10,
					rect.right,
					rect.bottom + 10
				};
				const int height = afterRect.bottom - afterRect.top;


				afterRect = {
					rect.left,
					height >= GetSystemMetrics(SM_CYSCREEN) ? rect.top : afterRect.top,
					rect.right,
					height >= GetSystemMetrics(SM_CYSCREEN) ? rect.bottom : afterRect.bottom
				};

				const int diffwidth = (afterRect.right - afterRect.left) - (rect.right - rect.left);
				const int diffheight = (afterRect.bottom - afterRect.top) - (rect.bottom - rect.top);

				SetWindowPos(window, nullptr,
					afterRect.left, afterRect.top, afterRect.right - afterRect.left, afterRect.bottom - afterRect.top, SWP_NOZORDER);
			}
			if (GetKeyState(VK_DOWN) & 0x8000) {
				GetWindowRect(window, &rect);
				RECT afterRect = {
					rect.left,
					rect.top + 10,
					rect.right,
					rect.bottom - 10
				};
				const int height = afterRect.bottom - afterRect.top;

				afterRect = {
					rect.left,
					height <= 100 ? rect.top : afterRect.top,
					rect.right,
					height <= 100 ? rect.bottom : afterRect.bottom
				};

				SetWindowPos(window, nullptr,
					afterRect.left, afterRect.top, afterRect.right - afterRect.left, afterRect.bottom - afterRect.top, SWP_NOZORDER);
			}

			if (GetKeyState(VK_RIGHT) & 0x8000) {
				GetWindowRect(window, &rect);
				RECT afterRect = {
					rect.left - 10,
					rect.top,
					rect.right + 10,
					rect.bottom
				};
				const int width = afterRect.right - afterRect.left;

				afterRect = {
					width >= GetSystemMetrics(SM_CXSCREEN) ? rect.left : afterRect.left,
					rect.top,
					width >= GetSystemMetrics(SM_CXSCREEN) ? rect.right : afterRect.right,
					rect.bottom
				};

				const int diffwidth = (afterRect.right - afterRect.left) - (rect.right - rect.left);
				const int diffheight = (afterRect.bottom - afterRect.top) - (rect.bottom - rect.top);

				SetWindowPos(window, nullptr,
					afterRect.left, afterRect.top, afterRect.right - afterRect.left, afterRect.bottom - afterRect.top, SWP_NOZORDER);
			}
			if (GetKeyState(VK_LEFT) & 0x8000) {
				GetWindowRect(window, &rect);
				RECT afterRect = {
					rect.left + 10,
					rect.top,
					rect.right - 10,
					rect.bottom
				};
				const int width = afterRect.right - afterRect.left;

				afterRect = {
					width <= 100 ? rect.left : afterRect.left,
					rect.top,
					width <= 100 ? rect.right : afterRect.right,
					rect.bottom
				};

				SetWindowPos(window, nullptr,
					afterRect.left, afterRect.top, afterRect.right - afterRect.left, afterRect.bottom - afterRect.top, SWP_NOZORDER);
			}
		}
	}
	Sleep(1);
}

void Direct2DWindow::OnMouse(const LPARAM lParam, const int wheel) {

	if (GetForegroundWindow() == window) {
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		if (GetKeyState(VK_LBUTTON) & 0x8000) {
			if (prevbuttondown_ == false) {
				prevpresspos_ = { xPos, yPos };
				prevbuttondown_ == true;
			}
			if (GetKeyState(VK_SHIFT) & 0x8000) {

				imagepos_.x += xPos - prevcursorpos_.x;
				imagepos_.y += yPos - prevcursorpos_.y;
			}
			else {
				// Release the mouse capture and start moving the window
				ReleaseCapture();
				SendMessage(window, WM_NCLBUTTONDOWN, HTCAPTION, 0);
			}
		}
		else if (GetKeyState(VK_RBUTTON) < 0) {
			if (prevbuttondown_ == false) {
				prevpresspos_ = { xPos, yPos };
				prevbuttondown_ == true;
			}
			
			const auto addrot = 180 / std::numbers::pi * (2 * std::numbers::pi + std::arg(-std::complex(xPos - dc->GetSize().width / 2.0, yPos - dc->GetSize().height / 2.0))
				- std::arg(-std::complex(prevcursorpos_.x - dc->GetSize().width / 2.0, prevcursorpos_.y - dc->GetSize().height / 2.0)));
			rot_ += addrot;
			const auto x = imagepos_.x * std::cos(addrot * std::numbers::pi / 180)
				- imagepos_.y * std::sin(addrot * std::numbers::pi / 180);
			const auto y = imagepos_.x * std::sin(addrot * std::numbers::pi / 180)
				+ imagepos_.y * std::cos(addrot * std::numbers::pi / 180);
			imagepos_.x = x;
			imagepos_.y = y;

		}
		else if (GetKeyState(VK_MBUTTON) < 0) {
			imagepos_ = { 0,0 };
			scale_ = { 1,1 };
			rot_ = 0;
			alpha_ = 1;
		}
		else {
			prevbuttondown_ = false;
		}
		prevcursorpos_ = { xPos, yPos };
	}
}

void Direct2DWindow::OnRender() {
	dc->BeginDraw();
	dc->Clear();
	ComPtr<ID2D1SolidColorBrush> brush;
	HR(dc->CreateSolidColorBrush(D2D1::ColorF(std::floor(bgcolor_ / 5), std::floor(bgcolor_ / 5), std::floor(bgcolor_ / 5), 0.25f * (bgcolor_ % 5)), brush.GetAddressOf()));
	dc->SetTransform(D2D1::Matrix3x2F::Identity());
	dc->FillRectangle({ 0,0,dc->GetSize().width,dc->GetSize().height }, brush.Get());
	img_.Show(imagepos_, scale_, rot_, 1, hiquarity_);
	HR(dc->EndDraw());
	HR(swapChain->Present(1, 0));
	HR(dcompDevice->Commit());
}

void Direct2DWindow::OnResize(UINT width, UINT height)
{
	if (dc && swapChain)
	{
		dc->SetTarget(nullptr);  // Clear the existing target in the device context

		// Resize the swap chain
		DXGI_SWAP_CHAIN_DESC1 desc = {};
		swapChain->GetDesc1(&desc);
		HRESULT hr = swapChain->ResizeBuffers(desc.BufferCount, width, height, desc.Format, desc.Flags);
		// Retrieve the back buffer from the swap chain
		ComPtr<IDXGISurface2> surface;
		HR(swapChain->GetBuffer(0, IID_PPV_ARGS(&surface)));
		// Create a bitmap linked to the swap chain surface
		D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));

		ComPtr<ID2D1Bitmap1> bitmap;
		HR(dc->CreateBitmapFromDxgiSurface(surface.Get(), &bitmapProperties, &bitmap));
		// Set the bitmap as the new target in the device context
		dc->SetTarget(bitmap.Get());
	}
}




LRESULT CALLBACK Direct2DWindow::WindowProcInstance(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {

	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_MOUSEWHEEL:
	{
		scale_.width *= 1 + GET_WHEEL_DELTA_WPARAM(wparam) / 1200.0;
		scale_.height *= 1 + GET_WHEEL_DELTA_WPARAM(wparam) / 1200.0;
		if (std::abs(scale_.width) < 0.05) {
			scale_.width = scale_.height = 0.05;
			imagepos_.x *= 0.98;
			imagepos_.y *= 0.98;
		}
		if (std::abs(scale_.width) > 10) {
			scale_.width = scale_.height = 10;
		}
	}
	break;
	case WM_SIZE:
	{
		UINT width = LOWORD(lparam);
		UINT height = HIWORD(lparam);
		OnResize(width, height);
	}
	break;
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		OnMouse(lparam);
		break;
	default:
		return DefWindowProc(window, message, wparam, lparam);
	}
	return 0;
}

inline LRESULT CALLBACK Direct2DWindow::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	Direct2DWindow* app;
	switch (msg) {
	case WM_CREATE:
		app = (Direct2DWindow*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)app);
		break;
	default:
		app = (Direct2DWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	}
	return app->WindowProcInstance(hWnd, msg, wParam, lParam);

	return true;
}

ATOM Direct2DWindow::RegisterWindowClass()
{
	// Register the window class.
	WNDCLASSEXW wcex = { sizeof(WNDCLASSEX) };
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(LONG_PTR);
	wcex.hInstance = module;
	wcex.hbrBackground = nullptr;
	wcex.lpszMenuName = L"window";
	wcex.hCursor = LoadCursor(nullptr, IDI_APPLICATION);
	wcex.lpszClassName = L"D2DWindow";

	return RegisterClassExW(&wcex);
}

void Direct2DWindow::InitInstance() {
	RegisterWindowClass();

	window = CreateWindowEx(WS_EX_LAYERED | WS_EX_NOREDIRECTIONBITMAP,
		L"D2DWindow", L"Sample",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		nullptr, nullptr, module, this);

	LONG lStyle = GetWindowLong(window, GWL_STYLE);
	lStyle &= ~(WS_CAPTION | WS_BORDER);
	SetWindowLong(window, GWL_STYLE, lStyle);
	SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);

}

void Direct2DWindow::setupDirect2D() {
	HR(D3D11CreateDevice(nullptr,    // Adapter
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,    // Module
		D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		nullptr, 0, // Highest available feature level
		D3D11_SDK_VERSION,
		&direct3dDevice,
		nullptr,    // Actual feature level
		nullptr));  // Device context

	HR(direct3dDevice.As(&dxgiDevice));

	HR(CreateDXGIFactory2(
		DXGI_CREATE_FACTORY_DEBUG,
		__uuidof(dxFactory),
		reinterpret_cast<void**>(dxFactory.GetAddressOf())));

	DXGI_SWAP_CHAIN_DESC1 description = {};

	description.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	description.BufferCount = 2;
	description.SampleDesc.Count = 1;
	description.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;

	RECT rect = {};
	GetClientRect(window, &rect);
	description.Width = rect.right - rect.left;
	description.Height = rect.bottom - rect.top;

	HR(dxFactory->CreateSwapChainForComposition(dxgiDevice.Get(),
		&description,
		nullptr, // Don’t restrict
		swapChain.GetAddressOf()));

	// Create a single-threaded Direct2D factory with debugging information
	D2D1_FACTORY_OPTIONS const options = { D2D1_DEBUG_LEVEL_INFORMATION };
	HR(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
		options,
		d2Factory.GetAddressOf()));
	// Create the Direct2D device that links back to the Direct3D device
	HR(d2Factory->CreateDevice(dxgiDevice.Get(),
		d2Device.GetAddressOf()));
	// Create the Direct2D device context that is the actual render target
	// and exposes drawing commands
	HR(d2Device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
		dc.GetAddressOf()));
	// Retrieve the swap chain's back buffer
	ComPtr<IDXGISurface2> surface;
	HR(swapChain->GetBuffer(
		0, // index
		__uuidof(surface),
		reinterpret_cast<void**>(surface.GetAddressOf())));
	// Create a Direct2D bitmap that points to the swap chain surface
	D2D1_BITMAP_PROPERTIES1 properties = {};
	properties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
	properties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
	properties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET |
		D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

	ComPtr<ID2D1Bitmap1> bitmap;
	HR(dc->CreateBitmapFromDxgiSurface(surface.Get(),
		properties,
		bitmap.GetAddressOf()));
	// Point the device context to the bitmap for rendering
	dc->SetTarget(bitmap.Get());

	HR(DCompositionCreateDevice(
		dxgiDevice.Get(),
		__uuidof(dcompDevice),
		reinterpret_cast<void**>(dcompDevice.GetAddressOf())));

	HR(dcompDevice->CreateTargetForHwnd(window,
		true, // Top most
		target.GetAddressOf()));

	HR(dcompDevice->CreateVisual(visual.GetAddressOf()));
	HR(visual->SetContent(swapChain.Get()));
	HR(target->SetRoot(visual.Get()));

}

