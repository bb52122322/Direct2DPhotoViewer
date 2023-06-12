#include "Direct2DWindow.hpp"

Direct2DWindow::Direct2DWindow(const HINSTANCE hInstance) :
	hInstance_(hInstance),
	image_(Image::Image()) {
	Direct2DWindow::InitInstance();
	Direct2DWindow::setupDirect2D();
	wchar_t buffer[128];
	int argc = 0;
	LPWSTR* argv = ::CommandLineToArgvW(GetCommandLine(), &argc);
	if (argv == nullptr) {
		OutputDebugString(L"FAILED!!!");
	}
	else if (argc > 1) {
		filepath_ = std::format(L"{}", argv[1]).c_str();
		image_.Load(d2dDeviceContext_.Get(), filepath_);
		for (auto i = 2; i < argc; ++i) {
			wchar_t applicationPath[FILENAME_MAX];
			if (GetModuleFileName(nullptr, applicationPath, FILENAME_MAX)) {
				ShellExecute(hWnd_, L"open", applicationPath, argv[i], nullptr, SW_SHOW);
			}
		}
	}

	// Adjast window size.
		const auto scale = std::min(d2dDeviceContext_->GetSize().width * 1.0 / image_.GetSize().width,
			d2dDeviceContext_->GetSize().height * 1.0 / image_.GetSize().height);
		const auto width = std::min(image_.GetSize().width * scale, d2dDeviceContext_->GetSize().width * 1.0);
		const auto height = std::min(image_.GetSize().height * scale, d2dDeviceContext_->GetSize().height * 1.0);
		SetWindowPos(hWnd_, nullptr, 0, 0, width, height, SWP_NOZORDER | SWP_NOMOVE);
		ImageResetEvent();
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

void Direct2DWindow::ImageResetEvent() {
	imagepos_ = { 0,0 };
		const auto scale = std::min(d2dDeviceContext_->GetSize().width / image_.GetSize().width,
			d2dDeviceContext_->GetSize().height / image_.GetSize().height);
		scale_ = { scale,scale };
		rot_ = 0;

}

void Direct2DWindow::ImageScaleEvent(const float scale, D2D1_POINT_2F center) {
	scale_.width = scale * scale_.width;
	scale_.height = scale * scale_.height;

	auto type = scale_.width > .0f ? 1.0f : -1.0f;
	if (std::abs(scale_.width) > ::SCALE_MAX) {
		imagepos_.x = (imagepos_.x - center.x) * (::SCALE_MAX/(scale_.width/scale)) + center.x;
		scale_.width = (scale_.width > .0f ? 1.0f : -1.0f) * ::SCALE_MAX;
	}
	else if (std::abs(scale_.width) < ::SCALE_MIN) {
		scale_.width = (scale_.width > .0f ? 1.0f : -1.0f) * ::SCALE_MIN;
		imagepos_.x = (imagepos_.x - center.x) * ::SCALE_MIN + center.x;
	}
	else {
		imagepos_.x = (imagepos_.x - center.x) * scale + center.x;
	}

	if (std::abs(scale_.height) > ::SCALE_MAX) {
		imagepos_.y = (imagepos_.y - center.y) * (::SCALE_MAX / (scale_.height / scale)) + center.y;
		scale_.height = (scale_.height > .0f ? 1.0f : -1.0f) * ::SCALE_MAX;
	}
	else if (std::abs(scale_.height) < ::SCALE_MIN) {
		scale_.height = (scale_.height > .0f ? 1.0f : -1.0f) * ::SCALE_MIN;
		imagepos_.y = (imagepos_.y - center.y) * SCALE_MIN+ center.y;
	}
	else {
		imagepos_.y = (imagepos_.y - center.y) * scale + center.y;
	}
}

void Direct2DWindow::ImageRotateEvent(const float rot) {
	rot_ += rot;
	const auto x = imagepos_.x* std::cos(rot * std::numbers::pi / 180)
		- imagepos_.y * std::sin(rot * std::numbers::pi / 180);
	const auto y = imagepos_.x * std::sin(rot * std::numbers::pi / 180)
		+ imagepos_.y  * std::cos(rot * std::numbers::pi / 180);
	imagepos_.x = x;
	imagepos_.y = y;
}

void Direct2DWindow::WindowMoveEvent(const D2D1_POINT_2F moveVec){
	RECT rect;
	GetWindowRect(hWnd_, &rect);
	const auto aftertop = rect.top + moveVec.y < 0 ? 0 : 
		(rect.top + moveVec.y > GetSystemMetrics(SM_CYSCREEN) - d2dDeviceContext_->GetSize().height ?
			GetSystemMetrics(SM_CYSCREEN) - d2dDeviceContext_->GetSize().height : rect.top + moveVec.y);
	const auto afterleft = rect.left + moveVec.x < 0 ? 0 :
		(rect.left + moveVec.x > GetSystemMetrics(SM_CXSCREEN) - d2dDeviceContext_->GetSize().width ?
			GetSystemMetrics(SM_CXSCREEN)-d2dDeviceContext_->GetSize().width : rect.left + moveVec.x);
	SetWindowPos(hWnd_, nullptr, afterleft, aftertop, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}

void Direct2DWindow::WindowSizeEvent(const D2D1_SIZE_F sizeDelta) {
	RECT rect;
	GetWindowRect(hWnd_, &rect);
	RECT afterRect = {
		static_cast<long>(rect.left - sizeDelta.width),
		static_cast<long>(rect.top - sizeDelta.height),
		static_cast<long>(rect.right + sizeDelta.width),
		static_cast<long>(rect.bottom + sizeDelta.height)
	};
	const int width = afterRect.right - afterRect.left;
	const int height = afterRect.bottom - afterRect.top;

	afterRect = {
		width <= ::WINDOW_SIZE_MIN.width ? rect.left : afterRect.left,
		height <= ::WINDOW_SIZE_MIN.height ? rect.top : afterRect.top,
		width <= ::WINDOW_SIZE_MIN.width ? rect.right : afterRect.right,
		height <= ::WINDOW_SIZE_MIN.height ? rect.bottom : afterRect.bottom
	};

	afterRect = {
		width > ::WINDOW_SIZE_MAX.width ? rect.left : afterRect.left,
		height > ::WINDOW_SIZE_MAX.height ? rect.top : afterRect.top,
		width > ::WINDOW_SIZE_MAX.width ? rect.right : afterRect.right,
		height > ::WINDOW_SIZE_MAX.height ? rect.bottom : afterRect.bottom
	};

	const int diffwidth = (afterRect.right - afterRect.left) - (rect.right - rect.left);
	const int diffheight = (afterRect.bottom - afterRect.top) - (rect.bottom - rect.top);

	SetWindowPos(hWnd_, nullptr,
		afterRect.left, afterRect.top, afterRect.right - afterRect.left, afterRect.bottom - afterRect.top, SWP_NOZORDER);
}

void Direct2DWindow::OpenFileEvent(const bool new_window) {
	// 一時的に最上面をOFF
	SetWindowPos(hWnd_, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);

	OPENFILENAME ofn;
	wchar_t filepath[FILENAME_MAX] = L"";

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = nullptr;
	ofn.lpstrFilter = L"All Files (*.*)\0*.*\0";
	ofn.lpstrFile = filepath;
	ofn.nMaxFile = FILENAME_MAX;
	ofn.lpstrDefExt = L"";

	if(new_window == true){
		ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT;

	}
	else {
		ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	}
	if (GetOpenFileName(&ofn))
	{
		if (new_window == true) {
			wchar_t applicationPath[256];
			if (GetModuleFileName(nullptr, applicationPath, 256)) {
				wchar_t* p = ofn.lpstrFile;
				while (*p)
				{
					ShellExecute(hWnd_, L"open", applicationPath, std::wstring(L"\"" + std::wstring(p) + L"\"").c_str(), nullptr, SW_SHOW);
					p += lstrlen(p) + 1;
				}
			}
		}
		else {
			image_.Load(d2dDeviceContext_.Get(), std::wstring(L"\"" + std::wstring(filepath) + L"\"").c_str());
			ImageResetEvent();
		}
	}

	if (topmost_ == true) {
		SetWindowPos(hWnd_, HWND_TOPMOST, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
	}
	else {
		SetWindowPos(hWnd_, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
	}
	Sleep(1000);
}

void Direct2DWindow::DroppedFileEvent(const std::wstring filepath, const bool newWindow) {
	if (newWindow == true || GetKeyState(VK_CONTROL) & 0x8000) {
		wchar_t applicationPath[256];
		if (GetModuleFileName(nullptr, applicationPath, 256)) {
			ShellExecute(hWnd_, L"open", applicationPath, filepath.c_str(), nullptr, SW_SHOW);
		}
	}
	else {
		image_.Load(d2dDeviceContext_.Get(), filepath);
		ImageResetEvent();
	}
}

void Direct2DWindow::OnKeyboard() {
	static int timer_flip = 0; // timer for QE (flip)
	static int timer_esc = 0; // timer for ESC (quit)
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000 && GetAsyncKeyState(VK_F12) & 0x1) {
		enableEvent_ = !enableEvent_;
		auto style = GetWindowLongPtr(hWnd_, GWL_EXSTYLE);
		if (enableEvent_ == true) {
			style &= ~WS_EX_TRANSPARENT;
			SetWindowLongPtr(hWnd_, GWL_EXSTYLE, style);
		}
		else {
			style |= WS_EX_TRANSPARENT;
			SetWindowLongPtr(hWnd_, GWL_EXSTYLE, style);
		}
		MessageBeep(MB_ICONINFORMATION);
	}
	if (GetForegroundWindow() == hWnd_) {
		RECT rect;
		if (GetKeyState(VK_ESCAPE) & 0x8000) {
			if (timer_esc > 15) {
				PostQuitMessage(0);
			}
			timer_esc++; 
		}
		else { timer_esc = 0; }
		// NO CONTROL KEY PRESS
		if (!(GetKeyState(VK_CONTROL) & 0x8000)) {
			// rotate
			if (GetKeyState('A') & 0x8000) {
				ImageRotateEvent(-3);
			}
			if (GetKeyState('D') & 0x8000) {
				ImageRotateEvent(3);
			}
			//scale
			if (GetKeyState('S') & 0x8000) {
				ImageScaleEvent(1 / ::SCALE_SPEED_KEY);
			}
			if (GetKeyState('W') & 0x8000) {
				ImageScaleEvent(::SCALE_SPEED_KEY);
			}
			//flip
			if (GetKeyState('Q') & 0x8000) {
				if (timer_flip == 0 || timer_flip > 60) {
					scale_.width *= -1;
				}
				timer_flip++;

			}
			else if (GetKeyState('E') & 0x8000) {
				if (timer_flip == 0 || timer_flip > 60) {
					scale_.height *= -1;
				}
				timer_flip++;
			}
			else {
				timer_flip = 0;
			}
			// Move Window
			if (GetKeyState(VK_UP) & 0x8000) {
				WindowMoveEvent({ 0, -::WINDOW_MOVE_SPEED });
			}
			if (GetKeyState(VK_LEFT) & 0x8000) {
				WindowMoveEvent({ -::WINDOW_MOVE_SPEED, 0 });
			}
			if (GetKeyState(VK_DOWN) & 0x8000) {
				WindowMoveEvent({ 0, ::WINDOW_MOVE_SPEED });
			}
			if (GetKeyState(VK_RIGHT) & 0x8000) {
				WindowMoveEvent({ ::WINDOW_MOVE_SPEED,0 });
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
					SetWindowPos(hWnd_, HWND_TOPMOST, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
					PlaySound(L"C:/Windows/Media/Speech on.wav", nullptr, SND_FILENAME | SND_ASYNC);
				}
				else {
					SetWindowPos(hWnd_, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
					PlaySound(L"C:/Windows/Media/Speech off.wav", nullptr, SND_FILENAME | SND_ASYNC);
				}
			}
			if (GetAsyncKeyState('N') & 0x1) {
				wchar_t applicationPath[256];
				if (GetModuleFileName(nullptr, applicationPath, 256)) {
					ShellExecute(hWnd_, L"open", applicationPath, filepath_.c_str(), nullptr, SW_SHOW);
				}
			}
			if (GetAsyncKeyState('F') & 0x1) {
				OpenFileEvent(false);
			}

			if (GetAsyncKeyState('R') & 0x1) {
				ImageResetEvent();

			}
		}
		// Crtl + XXX
		else {
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
				SetLayeredWindowAttributes(hWnd_, 0, 255 * alpha_, LWA_ALPHA);
			}
			if (GetKeyState('E') & 0x8000) {
				alpha_ = alpha_ + 0.01 > 1.00 ? 1 : alpha_ + 0.01;
				SetLayeredWindowAttributes(hWnd_, 0, 255 * alpha_, LWA_ALPHA);
			}

			if (GetKeyState(VK_UP) & 0x8000) {
				WindowSizeEvent({0,::WINDOW_SIZE_SPEED});
			}
			if (GetKeyState(VK_DOWN) & 0x8000) {
				WindowSizeEvent({ 0,-::WINDOW_SIZE_SPEED });
			}

			if (GetKeyState(VK_RIGHT) & 0x8000) {
				WindowSizeEvent({ ::WINDOW_SIZE_SPEED , 0});
			}
			if (GetKeyState(VK_LEFT) & 0x8000) {
				WindowSizeEvent({ -::WINDOW_SIZE_SPEED , 0 });
			}
			if (GetAsyncKeyState('F') & 0x1) {
				OpenFileEvent(true);
			}
		}
	}
}

void Direct2DWindow::OnMouse(const LPARAM lParam, const int wheel) {

	if (GetForegroundWindow() == hWnd_) {
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		if (GetKeyState(VK_LBUTTON) & 0x8000) {
			if (GetKeyState(VK_CONTROL) & 0x8000) {
				const auto addrot = 180 / std::numbers::pi * (2 * std::numbers::pi + std::arg(-std::complex(xPos - d2dDeviceContext_->GetSize().width / 2.0, yPos - d2dDeviceContext_->GetSize().height / 2.0))
					- std::arg(-std::complex(prevcursorpos_.x - d2dDeviceContext_->GetSize().width / 2.0, prevcursorpos_.y - d2dDeviceContext_->GetSize().height / 2.0)));
				ImageRotateEvent(addrot);

			}
			else {
				// Release the mouse capture and start moving the window
				ReleaseCapture();
				SendMessage(hWnd_, WM_NCLBUTTONDOWN, HTCAPTION, 0);
			}
		}
		if (GetKeyState(VK_RBUTTON) < 0) {
			imagepos_.x += xPos - prevcursorpos_.x;
			imagepos_.y += yPos - prevcursorpos_.y;
		}
		if (GetKeyState(VK_MBUTTON) < 0) {
			imagepos_ = { 0,0 };
			scale_ = { 1,1 };
			rot_ = 0;
			alpha_ = 1;
		}

		prevcursorpos_ = { xPos, yPos };
	}
}

void Direct2DWindow::OnRender() {
	d2dDeviceContext_->BeginDraw();
	d2dDeviceContext_->Clear();
	ComPtr<ID2D1SolidColorBrush> brush;
	HR(d2dDeviceContext_->CreateSolidColorBrush(D2D1::ColorF(std::floor(bgcolor_ / 5.0f), std::floor(bgcolor_ / 5.0f), std::floor(bgcolor_ / 5.0f), .25f * (bgcolor_ % 5)), brush.GetAddressOf()));
	d2dDeviceContext_->SetTransform(D2D1::Matrix3x2F::Identity());
	d2dDeviceContext_->FillRectangle({ 0,0,d2dDeviceContext_->GetSize().width,d2dDeviceContext_->GetSize().height }, brush.Get());
	image_.Show(imagepos_, scale_, rot_, 1, hiquarity_);
	HR(d2dDeviceContext_->EndDraw());
	HR(dxgiSwapChain_->Present(1, 0));
	HR(dCompositionDevice_->Commit());
}

void Direct2DWindow::OnResize(const UINT width, const UINT height)
{
	if (d2dDeviceContext_ && dxgiSwapChain_)
	{
		d2dDeviceContext_->SetTarget(nullptr);  // Clear the existing dCompositionTarget_ in the device context

		// Resize the swap chain
		DXGI_SWAP_CHAIN_DESC1 desc = {};
		dxgiSwapChain_->GetDesc1(&desc);
		HRESULT hr = dxgiSwapChain_->ResizeBuffers(desc.BufferCount, width, height, desc.Format, desc.Flags);
		// Retrieve the back buffer from the swap chain
		ComPtr<IDXGISurface2> surface;
		HR(dxgiSwapChain_->GetBuffer(0, IID_PPV_ARGS(&surface)));
		// Create a bitmap linked to the swap chain surface
		D2D1_BITMAP_PROPERTIES1 properties = {};
		properties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
		properties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
		properties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET |
			D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

		ComPtr<ID2D1Bitmap1> bitmap;
		HR(d2dDeviceContext_->CreateBitmapFromDxgiSurface(surface.Get(), &properties, &bitmap));
		// Set the bitmap as the new dCompositionTarget_ in the device context
		d2dDeviceContext_->SetTarget(bitmap.Get());
	}
}


LRESULT CALLBACK Direct2DWindow::WindowProcInstance(const HWND hWnd,const UINT msg, const WPARAM wParam, const LPARAM lParam) {

	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_MOUSEWHEEL:
	{
		POINT pt;
		GetCursorPos(&pt); // マウスカーソルのスクリーン座標を取得
		ScreenToClient(hWnd, & pt);
		ImageScaleEvent(1 + GET_WHEEL_DELTA_WPARAM(wParam) / 120.0 * ::SCALE_SPEED_WHEEL,
			{pt.x - d2dDeviceContext_->GetSize().width / 2.0f ,pt.y - d2dDeviceContext_->GetSize().height / 2.0f });
	}
	break;
	case WM_SIZE:
	{
		const auto width = LOWORD(lParam);
		const auto height = HIWORD(lParam);
		OnResize(width, height);
	}
	break;
	case WM_DROPFILES:
	{
		const HDROP hdrop = reinterpret_cast<HDROP>(wParam);
		const auto fileCount = DragQueryFile(hdrop, ~0, nullptr, 0); // ドロップされたファイルの数を取得

		for (auto i = 0u; i < fileCount; ++i)
		{
			WCHAR filePath[MAX_PATH];
			DragQueryFile(hdrop, i, filePath, MAX_PATH); // ファイルのパスを取得

			DroppedFileEvent(L"\"" + std::wstring(filePath) + L"\"",
				i == 0 ? false: true
				);
		}

		DragFinish(hdrop); // ドラッグ＆ドロップ操作を終了
	}
	break;
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		OnMouse(lParam);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

inline LRESULT CALLBACK Direct2DWindow::WindowProc(const HWND hWnd, const UINT msg, const WPARAM wParam, const LPARAM lParam) {

	Direct2DWindow* app;
	switch (msg) {
	case WM_CREATE:
		app = reinterpret_cast<Direct2DWindow*>(((LPCREATESTRUCT)lParam)->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(app));
		break;
	default:
		app = reinterpret_cast<Direct2DWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
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
	wcex.hInstance = hInstance_;
	wcex.hIcon = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	wcex.hbrBackground = nullptr;
	wcex.lpszMenuName = L"window";
	wcex.hCursor = LoadCursor(nullptr, IDI_APPLICATION);
	wcex.lpszClassName = L"D2DWindow";

	return RegisterClassExW(&wcex);
}

void Direct2DWindow::InitInstance() {
	RegisterWindowClass();

	hWnd_ = CreateWindowEx(WS_EX_LAYERED | WS_EX_NOREDIRECTIONBITMAP,
		L"D2DWindow", L"Direct2D Photo Viewer",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		nullptr, nullptr, hInstance_, this);

	auto lStyle = GetWindowLong(hWnd_, GWL_STYLE);
	lStyle &= ~(WS_CAPTION | WS_BORDER);
	SetWindowLong(hWnd_, GWL_STYLE, lStyle);
	DragAcceptFiles(hWnd_, TRUE); // accept dropping files

}

void Direct2DWindow::setupDirect2D() {
	HR(D3D11CreateDevice(nullptr,    // Adapter
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,    // Module
		D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		nullptr, 0, // Highest available feature level
		D3D11_SDK_VERSION,
		&d3dDevice_,
		nullptr,    // Actual feature level
		nullptr));  // Device context

	HR(d3dDevice_.As(&dxgiDevice_));

	HR(CreateDXGIFactory2(
		DXGI_CREATE_FACTORY_DEBUG,
		__uuidof(dxgiFactory_),
		reinterpret_cast<void**>(dxgiFactory_.GetAddressOf())));

	DXGI_SWAP_CHAIN_DESC1 description = {};

	description.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	description.BufferCount = 2;
	description.SampleDesc.Count = 1;
	description.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;

	RECT rect = {};
	GetClientRect(hWnd_, &rect);
	description.Width = rect.right - rect.left;
	description.Height = rect.bottom - rect.top;

	HR(dxgiFactory_->CreateSwapChainForComposition(dxgiDevice_.Get(),
		&description,
		nullptr, // Don’t restrict
		dxgiSwapChain_.GetAddressOf()));

	// Create a single-threaded Direct2D factory with debugging information
	D2D1_FACTORY_OPTIONS const options = { D2D1_DEBUG_LEVEL_INFORMATION };
	HR(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
		options,
		d2Factory_.GetAddressOf()));
	// Create the Direct2D device that links back to the Direct3D device
	HR(d2Factory_->CreateDevice(dxgiDevice_.Get(),
		d2dDevice_.GetAddressOf()));
	// Create the Direct2D device context that is the actual render dCompositionTarget_
	// and exposes drawing commands
	HR(d2dDevice_->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
		d2dDeviceContext_.GetAddressOf()));
	// Retrieve the swap chain's back buffer
	ComPtr<IDXGISurface2> surface;
	HR(dxgiSwapChain_->GetBuffer(
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
	HR(d2dDeviceContext_->CreateBitmapFromDxgiSurface(surface.Get(),
		properties,
		bitmap.GetAddressOf()));
	// Point the device context to the bitmap for rendering
	d2dDeviceContext_->SetTarget(bitmap.Get());

	HR(DCompositionCreateDevice(
		dxgiDevice_.Get(),
		__uuidof(dCompositionDevice_),
		reinterpret_cast<void**>(dCompositionDevice_.GetAddressOf())));

	HR(dCompositionDevice_->CreateTargetForHwnd(hWnd_,
		true, // Top most
		dCompositionTarget_.GetAddressOf()));

	HR(dCompositionDevice_->CreateVisual(dCompositionVisual_.GetAddressOf()));
	HR(dCompositionVisual_->SetContent(dxgiSwapChain_.Get()));
	HR(dCompositionTarget_->SetRoot(dCompositionVisual_.Get()));

}