#pragma once
#include "framework.hpp"

class Image {
public:
	Image(const std::wstring filename);
	HRESULT Load(const std::wstring filename);
	void Show(ID2D1DeviceContext* const deviceContext,
		const D2D1_POINT_2F pos = { 0,0 },
		const float scale = 1,
		const float rot = 0,
		const float opacity = 1,
		const bool hiquarity = true
	);


private:
	std::wstring filename_;
	ComPtr<IWICImagingFactory> wicFactory_;
	ComPtr<IWICBitmapDecoder> decoder_;
	ComPtr<IWICBitmapFrameDecode> frame_;
	ComPtr<IWICFormatConverter> converter_;
};