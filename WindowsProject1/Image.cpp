#include "Image.hpp"

Image::Image(ID2D1DeviceContext* const deviceContext, const std::wstring filename) {
	// Create WIC factory
	HR(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory_)));
	if (deviceContext != nullptr && filename != L"") { Load(deviceContext, filename); }
}

HRESULT Image::Load(ID2D1DeviceContext* const deviceContext, const std::wstring filename) {
	HRESULT hr = S_OK;// Create decoder
	hr = wicFactory_->CreateDecoderFromFilename(filename.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder_);
	if (FAILED(hr)) return S_FALSE;
	// Get the first frame
	HR(decoder_->GetFrame(0, &frame_));

	// Create format converter
	HR(wicFactory_->CreateFormatConverter(&converter_));

	// Initialize the format converter
	HR(converter_->Initialize(frame_.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom));

	deviceContext_ = deviceContext;
	HR(deviceContext_->CreateBitmapFromWicBitmap(converter_.Get(), nullptr, &bitmap_));
	return S_OK;
}


void Image::Show(const D2D1_POINT_2F pos,
	const D2D1_SIZE_F scale,
	const float rot,
	const float opacity,
	const bool hiquarity) {

	if (decoder_ == nullptr) return;
	
	// Create a Direct2D bitmap from the WIC bitmap.

	float centerX = bitmap_->GetSize().width / 2.0f;
	float centerY = bitmap_->GetSize().height / 2.0f;

	float coordX = deviceContext_->GetSize().width / 2.0f;
	float coordY = deviceContext_->GetSize().height / 2.0f;


	// 回転の中心を原点に移動する行列を作成します。
	D2D1::Matrix3x2F translationMat = D2D1::Matrix3x2F::Translation(pos.x-centerX+coordX, pos.y - centerY+coordY);
	// 指定した角度で回転する行列を作成します。
	D2D1::Matrix3x2F rotationMat = D2D1::Matrix3x2F::Rotation(rot, { centerX,centerY });
	D2D1::Matrix3x2F scaleMat = D2D1::Matrix3x2F::Scale(scale.width,scale.height, {centerX,centerY});

	// 上記のすべての行列を結合して、新しい変換行列を作成します。
	D2D1::Matrix3x2F transform = scaleMat*rotationMat*translationMat;

	// 新しい変換行列を設定します。
	deviceContext_->SetTransform(transform);

	// ここでbitmapを描画します
	const auto drawmode = (hiquarity == true) ? D2D1_BITMAP_INTERPOLATION_MODE_LINEAR: D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR;
	deviceContext_->DrawBitmap(bitmap_.Get(), 0, opacity, drawmode);


}