// ZFontFreetype.h
// By PaintDream (paintdream@paintdream.com)
// 2014-12-16
//

#ifndef __ZFONTFREETYPE_H__
#define __ZFONTFREETYPE_H__

#include "../../../Core/Interface/IFontBase.h"
#include <iconv.h>
#include <ft2build.h>

#ifdef __linux__
#include <ftglyph.h>
#else
#include <freetype/ftglyph.h>
#endif 

namespace PaintsNow {
	class ZFontFreetype final : public IFontBase {
	public:
		virtual Font* Load(IStreamBase& stream, size_t length);
		virtual void Close(Font* font);
		virtual CHARINFO RenderTexture(Font* font, String& data, FONTCHAR character, size_t bitmapSiz, float hinting) const;
	};
}


#endif // __ZFONTFREETYPE_H__