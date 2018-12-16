// ZImageFreeImage.h
// By PaintDream (paintdream@paintdream.com)
// 2014-12-14
//

#ifndef __ZIMAGEFREEIMAGE_H__
#define __ZIMAGEFREEIMAGE_H__

#include "../../../Core/Interface/IStreamBase.h"
#include "../../../Core/Interface/Interfaces.h"
#include "../../../Core/Common/ZType.h"
typedef struct FIBITMAP* PFIBITMAP;

namespace PaintsNow {
	class ZImageFreeImage final : public IImage {
	public:
		virtual Image* Create(size_t width, size_t height, IRender::LAYOUT_TYPE layout, IRender::DATA_TYPE dataType) const override;
		virtual IRender::LAYOUT_TYPE GetLayoutType(Image* image) const override;
		virtual IRender::DATA_TYPE GetDataType(Image* image) const override;
		virtual size_t GetBPP(Image* image) const override;
		virtual size_t GetWidth(Image* image) const override;
		virtual size_t GetHeight(Image* image) const override;
		virtual void* GetBuffer(Image* image) const override;
		virtual bool Load(Image* image, IStreamBase& streamBase, size_t length) const override;
		virtual bool Save(Image* image, IStreamBase& streamBase, const String& type) const override;
		virtual void Delete(Image* image) const override;
	};
}

#endif // __ZIMAGEFREEIMAGE_H__