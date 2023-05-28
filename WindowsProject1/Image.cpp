#include "Image.hpp"

Image::Image(ID2D1DeviceContext* const deviceContext, const std::wstring filename) {
	// Create WIC factory
	auto hr = S_OK;
	hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory_));
	if (FAILED(hr))  MessageBox(nullptr, L"CocreateInstance fialed.", L"ERROR!", MB_OK);
	if (deviceContext != nullptr && filename != L"") { Load(deviceContext, filename); }
}

// To Load Image
HRESULT Image::Load(ID2D1DeviceContext* const deviceContext, const std::wstring filepath) {
	HRESULT hr = S_OK;// Create decoder

	std::wstring filepath_temp = filepath;
	filepath_temp.erase(std::remove_if(filepath_temp.begin(), filepath_temp.end(),
		[](wchar_t c) { return c == L'\"'; }), filepath_temp.end());
	filepath_ = filepath_temp;

	hr = wicFactory_->CreateDecoderFromFilename(filepath_.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder_);
	if (FAILED(hr)) return S_FALSE;
	
	hr = decoder_->GetFrame(0, &frame_);
	if (FAILED(hr)) throw MessageBox(nullptr, L"GetFrame fialed.", L"ERROR!", MB_OK);
	wicFactory_->CreateFormatConverter(&converter_);
	if (FAILED(hr)) throw MessageBox(nullptr, L"CreateFormatConverter fialed.", L"ERROR!", MB_OK);
	converter_->Initialize(frame_.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom);
	if (FAILED(hr)) throw MessageBox(nullptr, L"Initialize fialed.", L"ERROR!", MB_OK);

	deviceContext_ = deviceContext;
	deviceContext_->CreateBitmapFromWicBitmap(converter_.Get(), nullptr, &bitmap_);
	if (FAILED(hr)) throw MessageBox(nullptr, L"CreateBitmapFromWicBitmap fialed.", L"ERROR!", MB_OK);
	
	return S_OK;
}

void Image::Show(const D2D1_POINT_2F pos,
	const D2D1_SIZE_F scale,
	const float rot,
	const float opacity,
	const bool hiquarity) {

	if (decoder_ == nullptr) return;
	
	float centerX = bitmap_->GetSize().width / 2.0f;
	float centerY = bitmap_->GetSize().height / 2.0f;

	float coordX = deviceContext_->GetSize().width / 2.0f;
	float coordY = deviceContext_->GetSize().height / 2.0f;

	// set transform
	D2D1::Matrix3x2F translationMat = D2D1::Matrix3x2F::Translation(pos.x-centerX+coordX, pos.y - centerY+coordY);
	D2D1::Matrix3x2F rotationMat = D2D1::Matrix3x2F::Rotation(rot, { centerX,centerY });
	D2D1::Matrix3x2F scaleMat = D2D1::Matrix3x2F::Scale(scale.width,scale.height, {centerX,centerY});
	D2D1::Matrix3x2F transform = scaleMat*rotationMat*translationMat;

	deviceContext_->SetTransform(transform);

	const auto drawmode = (hiquarity == true) ? D2D1_BITMAP_INTERPOLATION_MODE_LINEAR: D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR;
	deviceContext_->DrawBitmap(bitmap_.Get(), 0, opacity, drawmode);
}

D2D1_SIZE_F Image::GetSize() const {
	if (bitmap_ != nullptr) {
		return bitmap_->GetSize();
	}
	else {
		return {-1, -1};
	}
}