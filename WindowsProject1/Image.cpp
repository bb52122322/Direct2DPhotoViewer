#include "Image.hpp"

Image::Image(const std::wstring filename) {
	// Create WIC factory
	HR(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory_)));
	if (filename != L"") { Load(filename); }

}

HRESULT Image::Load(const std::wstring filename) {
	HRESULT hr = S_OK;// Create decoder
	hr = wicFactory_->CreateDecoderFromFilename(filename.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder_);
	if (FAILED(hr)) return S_FALSE;
	// Get the first frame
	HR(decoder_->GetFrame(0, &frame_));

	// Create format converter
	HR(wicFactory_->CreateFormatConverter(&converter_));

	// Initialize the format converter
	HR(converter_->Initialize(frame_.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom));

	return S_OK;
}


void Image::Show(ID2D1DeviceContext* const deviceContext,
	const D2D1_POINT_2F pos,
	const float scale,
	const float rot,
	const float opacity,
	const bool hiquarity) {

	if (decoder_ == nullptr) return;
	
	// Create a Direct2D bitmap from the WIC bitmap.
	ComPtr<ID2D1Bitmap1> bitmap;
	HRESULT hr = deviceContext->CreateBitmapFromWicBitmap(converter_.Get(), nullptr, &bitmap);
	float centerX = bitmap->GetSize().width / 2.0f;
	float centerY = bitmap->GetSize().height / 2.0f;

	float coordX = deviceContext->GetSize().width / 2.0f;
	float coordY = deviceContext->GetSize().height / 2.0f;


	// 回転の中心を原点に移動する行列を作成します。
	D2D1::Matrix3x2F translationMat = D2D1::Matrix3x2F::Translation(pos.x-centerX+coordX, pos.y - centerY+coordY);
	// 指定した角度で回転する行列を作成します。
	D2D1::Matrix3x2F rotationMat = D2D1::Matrix3x2F::Rotation(rot, { centerX,centerY });
	D2D1::Matrix3x2F scaleMat = D2D1::Matrix3x2F::Scale(scale,scale, {centerX,centerY});

	// 上記のすべての行列を結合して、新しい変換行列を作成します。
	D2D1::Matrix3x2F transform = scaleMat*rotationMat*translationMat;

	// 新しい変換行列を設定します。
	deviceContext->SetTransform(transform);

	// ここでbitmapを描画します
	const auto drawmode = (hiquarity == true) ? D2D1_BITMAP_INTERPOLATION_MODE_LINEAR: D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR;
	deviceContext->DrawBitmap(bitmap.Get(), 0, opacity, drawmode);


}