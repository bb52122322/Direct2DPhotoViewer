#include "Direct2DWindow.hpp"
#include <format>

Direct2DWindow::Direct2DWindow(HINSTANCE module) :
	module(module),
	img_(L"./PNG_transparency_demonstration_1.png") {
	Direct2DWindow::InitInstance();
	Direct2DWindow::setupDirect2D();

	//RECT rect = {};
	//GetClientRect(window, &rect);
	//const float width = rect.right - rect.left;
	//const float height = rect.bottom - rect.top;

	//imagepos_ = { width / 2,height / 2 };
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
	if (GetForegroundWindow() == window) {
		RECT rect;
		if (GetAsyncKeyState(VK_ESCAPE) & 0x1) {
			PostQuitMessage(0);

		}
		// NO CONTROL KEY PRESS
		if (GetKeyState(VK_CONTROL) >= 0) {
			if (GetKeyState('W') < 0) {
				imagepos_.y -= 20;
			}
			if (GetKeyState('A') < 0) {
				imagepos_.x -= 20;
			}
			if (GetKeyState('S') < 0) {
				imagepos_.y += 20;
			}
			if (GetKeyState('D') < 0) {
				imagepos_.x += 20;
			}
			if (GetKeyState('Q') < 0) {
				scale_ = scale_ * 0.98 < 0.05 ? 0.05 : scale_ * 0.98;
				// 中央に寄せる
				if (scale_ <= 0.051) {
					imagepos_.x *= 0.9;
					imagepos_.y *= 0.9;
				}
			}
			if (GetKeyState('E') < 0) {
				scale_ = scale_ * 1.02 > 10 ? 10 : scale_ * 1.02;
				//imagepos_.x *= 1.02;
				//imagepos_.y *= 1.02;
			}
		}
		// Crtl + XXX
		else {
			if (GetKeyState('W') < 0) {
				GetWindowRect(window, &rect);
				const int aftertop = rect.top - 20 < 0 ? 0 : rect.top - 20;
				const int afterleft = rect.left;
				SetWindowPos(window, nullptr, afterleft, aftertop, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			}
			if (GetKeyState('A') < 0) {
				GetWindowRect(window, &rect);
				const int aftertop = rect.top;
				const int afterleft = rect.left - 20 < 0 ? 0 : rect.left - 20;
				SetWindowPos(window, nullptr, afterleft, aftertop, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			}
			if (GetKeyState('S') < 0) {
				GetWindowRect(window, &rect);
				const int height = rect.bottom - rect.top;
				const int aftertop = rect.bottom >= GetSystemMetrics(SM_CYSCREEN) ? GetSystemMetrics(SM_CYSCREEN) - height: rect.top+20;
				const int afterleft = rect.left;
				SetWindowPos(window, nullptr, afterleft, aftertop, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			}
			if (GetKeyState('D') < 0) {
				GetWindowRect(window, &rect);
				const int width = rect.right - rect.left;
				const int aftertop = rect.top;
				const int afterleft = rect.right >= GetSystemMetrics(SM_CXSCREEN) ? GetSystemMetrics(SM_CXSCREEN) - width: rect.left+20;
				SetWindowPos(window, nullptr, afterleft, aftertop, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			}

			if (GetKeyState('Q') < 0) {
				rot_ -= 3;
				const auto x = imagepos_.x * std::cos(-3 * std::numbers::pi / 180)
					- imagepos_.y * std::sin(-3 * std::numbers::pi / 180);
				const auto y = imagepos_.x * std::sin(-3 * std::numbers::pi / 180)
					+ imagepos_.y * std::cos(-3 * std::numbers::pi / 180);
				imagepos_.x = x;
				imagepos_.y = y;
			}
			if (GetKeyState('E') < 0) {
				rot_ += 3;
				const auto x = imagepos_.x * std::cos(3 * std::numbers::pi / 180)
					- imagepos_.y * std::sin(3 * std::numbers::pi / 180);
				const auto y = imagepos_.x * std::sin(3 * std::numbers::pi / 180)
					+ imagepos_.y * std::cos(3 * std::numbers::pi / 180);
				imagepos_.x = x;
				imagepos_.y = y;
			}

		}
		if (GetKeyState(VK_UP) < 0) {
			GetWindowRect(window, &rect);
			RECT afterRect = {
				rect.left - 10,
				rect.top - 10,
				rect.right + 10,
				rect.bottom + 10
			};
			const int width = afterRect.right - afterRect.left;
			const int height = afterRect.bottom - afterRect.top;

			afterRect = {
				width >= GetSystemMetrics(SM_CXSCREEN) ? rect.left : afterRect.left,
				height >= GetSystemMetrics(SM_CYSCREEN) ? rect.top : afterRect.top,
				width >= GetSystemMetrics(SM_CXSCREEN) ? rect.right : afterRect.right,
				height >= GetSystemMetrics(SM_CYSCREEN) ? rect.bottom : afterRect.bottom
			};

			const int diffwidth = (afterRect.right - afterRect.left) - (rect.right - rect.left);
			const int diffheight = (afterRect.bottom - afterRect.top) - (rect.bottom - rect.top);



			SetWindowPos(window, nullptr,
				afterRect.left,afterRect.top, afterRect.right - afterRect.left, afterRect.bottom - afterRect.top,SWP_NOZORDER);
		}
		if (GetKeyState(VK_DOWN) < 0) {
			GetWindowRect(window, &rect);
			RECT afterRect = {
				rect.left + 10,
				rect.top + 10,
				rect.right - 10,
				rect.bottom - 10
			};
			const int width = afterRect.right - afterRect.left;
			const int height = afterRect.bottom - afterRect.top;

			afterRect = {
				width <= 100 ? rect.left : afterRect.left,
				height <= 100 ? rect.top : afterRect.top,
				width <= 100 ? rect.right : afterRect.right,
				height <= 100 ? rect.bottom : afterRect.bottom
			};

			SetWindowPos(window, nullptr,
				afterRect.left, afterRect.top, afterRect.right - afterRect.left, afterRect.bottom - afterRect.top, SWP_NOZORDER);
		}
		if (GetKeyState(VK_LEFT) < 0) {
			alpha_ = alpha_ - 0.01 < 0.2 ? 0.2 : alpha_ - 0.01;
		}
		if (GetKeyState(VK_RIGHT) < 0) {
			alpha_ = alpha_ + 0.01 > 1.00 ? 1 : alpha_ + 0.01;
		}
		if (GetAsyncKeyState('B') & 0x1) {
			bgcolor_++;
			if (bgcolor_ > 5) bgcolor_ = 0;
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
		if (GetAsyncKeyState('R') & 0x1) {

			imagepos_ = { 0,0 };
			scale_ = 1;
			rot_ = 0;
			alpha_ = 1;

		}
	}
}

void Direct2DWindow::OnMouse(const LPARAM lParam, const int wheel) {

	if (GetForegroundWindow() == window) {
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		const float prevdist = std::pow(dc->GetSize().width / 2 - presscursor.x, 2)
			+ std::pow(dc->GetSize().height / 2 - presscursor.y, 2);

		const float nowdist = std::pow(dc->GetSize().width / 2 - xPos, 2)
			+ std::pow(dc->GetSize().height / 2 - yPos, 2);

		if (GetKeyState(VK_LBUTTON) < 0) {
			// Release the mouse capture and start moving the window
			ReleaseCapture();
			SendMessage(window, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		}
		if (GetKeyState(VK_RBUTTON) < 0) {
			scale_ = scale_ * (nowdist - prevdist) > 10 ? 10 :
				scale_ * (nowdist - prevdist) < 0.05 ? 0.05 : scale_ * (nowdist - prevdist);
		}
		if (GetKeyState(VK_MBUTTON) < 0) {
			imagepos_ = { 0,0 };
			scale_ = 1;
			rot_ = 0;
			alpha_ = 1;
		}
		else {
			presscursor = { -1,-1 };
		}
	}
}

void Direct2DWindow::OnRender() {
	dc->BeginDraw();
	dc->Clear();
	ComPtr<ID2D1SolidColorBrush> brush;
	HR(dc->CreateSolidColorBrush(D2D1::ColorF(std::floor(bgcolor_ / 3), std::floor(bgcolor_ / 3), std::floor(bgcolor_ / 3), 0.5f * (bgcolor_ % 3)), brush.GetAddressOf()));
	dc->SetTransform(D2D1::Matrix3x2F::Identity());
	dc->FillRectangle({ 0,0,dc->GetSize().width,dc->GetSize().height }, brush.Get());
	img_.Show(dc.Get(), imagepos_, scale_, rot_, alpha_, hiquarity_);
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
	case WM_SIZE:
	{
		UINT width = LOWORD(lparam);
		UINT height = HIWORD(lparam);
		OnResize(width, height);
	}
	break;
	case WM_MOUSEHWHEEL:
	{
		scale_ += GET_WHEEL_DELTA_WPARAM(wparam);
	}
	break;
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

LRESULT CALLBACK Direct2DWindow::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
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

	window = CreateWindowEx(WS_EX_NOREDIRECTIONBITMAP,
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


//void Direct2DWindow::OnRenderImage(const wchar_t* filename) {
//	// Create WIC factory
//	ComPtr<IWICImagingFactory> wicFactory;
//	HR(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory)));
//
//	// Create decoder
//	ComPtr<IWICBitmapDecoder> decoder;
//	HR(wicFactory->CreateDecoderFromFilename(filename, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder));
//
//	// Get the first frame
//	ComPtr<IWICBitmapFrameDecode> frame;
//	HR(decoder->GetFrame(0, &frame));
//
//	// Create format converter
//	ComPtr<IWICFormatConverter> converter;
//	HR(wicFactory->CreateFormatConverter(&converter));
//
//	// Initialize the format converter
//	HR(converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom));
//
//	// Create a Direct2D bitmap from the WIC bitmap.
//	ComPtr<ID2D1Bitmap1> bitmap;
//	HR(dc->CreateBitmapFromWicBitmap(converter.Get(), nullptr, &bitmap));
//
//	// Draw the bitmap
//	dc->DrawBitmap(bitmap.Get(),D2D1_RECT_F(imagepos.x, imagepos.y, imagepos.x+100, imagepos.y+100));
//}

