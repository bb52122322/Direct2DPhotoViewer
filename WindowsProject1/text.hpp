#pragma once

#include "framework.hpp"
//
//class Text {
//public:
//	Text(const std::wstring& const string,
//		const Brush& const brush,
//		const float& const fontSize = 20.0f,
//		const std::wstring& const fontFamilyName = L"",
//		const DWRITE_TEXT_ALIGNMENT& const textAlignment = DWRITE_TEXT_ALIGNMENT_LEADING,
//		const DWRITE_PARAGRAPH_ALIGNMENT& const paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR)
//		:string_(string), brush_(brush), fontSize_(fontSize), fontFamilyName_(fontFamilyName),
//		textAlignment_(textAlignment), paragraphAlignment_(paragraphAlignment),
//		pDWriteFactory_(nullptr)
//	{
//		HRESULT hr = S_OK;
//
//		hr = DWriteCreateFactory(
//			DWRITE_FACTORY_TYPE_SHARED,
//			__uuidof(pDWriteFactory_),
//			reinterpret_cast<IUnknown**>(&pDWriteFactory_)
//		);
//		if (FAILED(hr)) { throw L"DWriteCreateFactory function failed.\n"; }
//
//		hr = pDWriteFactory_->CreateTextFormat(
//			fontFamilyName_.c_str(),
//			nullptr,
//			DWRITE_FONT_WEIGHT_NORMAL,
//			DWRITE_FONT_STYLE_NORMAL,
//			DWRITE_FONT_STRETCH_NORMAL,
//			fontSize_,
//			L"",
//			&pDWriteTextFormat_
//		);
//		if (FAILED(hr)) { throw L"IDWriteFactory::CreateTextFormat method failed.\n"; }
//
//		pDWriteTextFormat_->SetTextAlignment(textAlignment_);
//		pDWriteTextFormat_->SetParagraphAlignment(paragraphAlignment_);
//	}
//
//	void string(const std::wstring& const string) {
//		string_ = string;
//	}
//
//	void fontSize(const float& const fontSize) {
//		fontSize_ = fontSize;
//	}
//
//	void brush(const Brush& const brush) {
//		brush_ = brush;
//	}
//
//	void fontFamilyName(const std::wstring& const fontFamilyName) {
//		fontFamilyName_ = fontFamilyName.c_str();
//	}
//
//	/// https://docs.microsoft.com/en-us/windows/win32/api/dwrite/ne-dwrite-dwrite_text_alignment
//	/// <summary>
//	/// DWRITE_TEXT_ALIGNMENT_LEADING:
//	///		The leading edge of the paragraph text is aligned to the leading edge of the layout box.
//	///	DWRITE_TEXT_ALIGNMENT_TRAILING:
//	///		The trailing edge of the paragraph text is aligned to the trailing edge of the layout box.
//	///	DWRITE_TEXT_ALIGNMENT_CENTER:
//	///		The center of the paragraph text is aligned to the center of the layout box.
//	///	DWRITE_TEXT_ALIGNMENT_JUSTIFIED:
//	///		Align text to the leading side, and also justify text to fill the lines.
//	/// </summary>
//	void textAlignment(const DWRITE_TEXT_ALIGNMENT& const textAlignment) {
//		textAlignment_ = textAlignment;
//		pDWriteTextFormat_->SetTextAlignment(textAlignment_);
//	}
//
//	/// https://docs.microsoft.com/ja-jp/windows/win32/api/dwrite/ne-dwrite-dwrite_paragraph_alignment
//	/// <summary>
//	/// DWRITE_PARAGRAPH_ALIGNMENT_NEAR:
//	///		The top of the text flow is aligned to the top edge of the layout box.
//	///	DWRITE_PARAGRAPH_ALIGNMENT_FAR:
//	///		The bottom of the text flow is aligned to the bottom edge of the layout box.
//	///	DWRITE_PARAGRAPH_ALIGNMENT_CENTER:
//	///		The center of the flow is aligned to the center of the layout box.
//	/// </summary>
//	void paragraphAlignment(const DWRITE_PARAGRAPH_ALIGNMENT& const paragraphAlignment) {
//		paragraphAlignment_ = paragraphAlignment;
//		pDWriteTextFormat_->SetParagraphAlignment(paragraphAlignment_);
//	}
//
//
//	void Draw() const {
//		D2D1_SIZE_F renderm_rtSize = brush_.GetPRenderTarget().GetSize();
//
//		brush_.GetPRenderTarget().SetTransform(D2D1::Matrix3x2F::Identity());
//
//		brush_.GetPRenderTarget().DrawText(
//			string_.c_str(),
//			string_.size(),
//			pDWriteTextFormat_,
//			D2D1::RectF(0, 0, renderm_rtSize.width, renderm_rtSize.height),
//			&brush_.GetBrush()
//		);
//	}
//
//private:
//	std::wstring string_;
//	float fontSize_;
//	Brush brush_;
//	std::wstring fontFamilyName_;
//	DWRITE_TEXT_ALIGNMENT textAlignment_;
//	DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment_;
//
//	CComPtr<IDWriteFactory> pDWriteFactory_;
//	CComPtr<IDWriteTextFormat> pDWriteTextFormat_;
//};
//
