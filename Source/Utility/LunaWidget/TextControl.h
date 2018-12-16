// TextControl.h
// By PaintDream (paintdream@paintdream.com)
// 2018-7-27
//

#ifndef __TEXTCONTROL_H__
#define __TEXTCONTROL_H__

#include "../../Core/Object/Tiny.h"
#include "../SnowyStream/Resource/FontResource.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		class WidgetPipeline;
	}
	namespace NsLunaWidget {
		class TextControl : public SharedTiny {
		public:
			TextControl();
			virtual ~TextControl();

			virtual void FireRender(IRender& render, NsSnowyStream::WidgetPipeline& pipeline, const ZInt2Pair& rect, const ZInt2& totalSize, const ZInt2& padding);

			struct Descriptor {
				Descriptor(int h, int fs);
				int totalWidth;
				int yCoord;
				int firstOffset;

				struct Char {
					Char(int c = 0, int off = 0);
					int xCoord;
					int offset;
				};

				std::vector<Char> allOffsets;
			};

			TShared<NsSnowyStream::FontResource> mainFont;
			int Locate(ZInt2& rowCol, const ZInt2& pt, bool isPtRowCol) const;
			void SetText(const String& text);
			void Scroll(const ZInt2& pt);

		protected:
			int GetLineCount() const;
			void SetPasswordChar(int ch);
			virtual void AppendText(const String& text);
			void SetSize(const ZInt2& size);
			bool IsEmpty() const;
			const ZInt2& GetSize() const;
			const ZInt2& GetFullSize() const;
			void SetPadding(const ZInt2& padding);
			ZInt2 SelectText(const ZInt2Pair& offsetRect) const;
			void TextChange();
			void RenderCharacter(IRender& render, NsSnowyStream::WidgetPipeline& pipeline, const ZInt2Pair& range, const ZInt2Pair& rect, const ZInt2Pair& info, const ZFloat4& color, const ZInt2& totalSize, uint32_t fontSize) const;
			virtual ZInt2 PerformRender(IRender& render, const ZInt2Pair& range, std::vector<Descriptor>& widthInfo, const ZInt2& totalSize, const ZInt2& padding, NsSnowyStream::WidgetPipeline* pipeline) const;
			ZInt2 Fix(int offset) const;

			class TagParser;
			TagParser* parser;
			std::vector<Descriptor> widthInfo;

		public:
			String text;
			ZInt2 size;
			ZInt2 scroll;
			ZInt2 fullSize;
			size_t textLength;
			int passwordChar;
			int cursorChar;
			int cursorPos;
			uint32_t fontSize;
			ZFloat4 cursorColor;
			ZInt2 selectRange;
			ZFloat4 selectColor;
			bool selectRevColor;
			bool cursorRevColor;
		};
	}
}


#endif // __TEXTCONTROL_H__