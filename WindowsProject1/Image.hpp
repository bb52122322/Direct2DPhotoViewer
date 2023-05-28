#pragma once
#include "framework.hpp"

class Image {
public:
	Image(ID2D1DeviceContext* const deviceContext = nullptr, 
		const std::wstring filepath = L"");
	HRESULT Load(ID2D1DeviceContext* const deviceContext, 
		const std::wstring filename);
	void Show(const D2D1_POINT_2F pos = { 0,0 },
		const D2D1_SIZE_F scale = {1,1},
		const float rot = 0,
		const float opacity = 1,
		const bool hiquarity = true
	);
	D2D1_SIZE_F GetSize() const;

private:
	std::wstring filepath_;
	ComPtr<IWICImagingFactory> wicFactory_;
	ComPtr<IWICBitmapDecoder> decoder_;
	ComPtr<IWICBitmapFrameDecode> frame_;
	ComPtr<IWICFormatConverter> converter_;
	ComPtr<ID2D1Bitmap1> bitmap_;
	ComPtr<ID2D1DeviceContext> deviceContext_;
};