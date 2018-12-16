// IImage.h
// By PaintDream (paintdream@paintdream.com)
// 2014-12-14
//

#ifndef __IIMAGE_H__
#define __IIMAGE_H__


#include "../PaintsNow.h"
#include "../Common/ZType.h"
#include "IRender.h"
#include "IDevice.h"
#include <string>

namespace PaintsNow {
	class IArchive;
	class IImage : public IDevice {
	public:
		virtual ~IImage();
		class Image {};
		static int GetPixelSize(VRender::DATA_TYPE dataType, VRender::LAYOUT_TYPE layout);
		virtual Image* Create(size_t width, size_t height, VRender::LAYOUT_TYPE layout, VRender::DATA_TYPE dataType) const = 0;
		virtual VRender::LAYOUT_TYPE GetLayoutType(Image* image) const = 0;
		virtual VRender::DATA_TYPE GetDataType(Image* image) const = 0;
		virtual size_t GetBPP(Image* image) const = 0;
		virtual size_t GetWidth(Image* image) const = 0;
		virtual size_t GetHeight(Image* image) const = 0;
		virtual void* GetBuffer(Image* image) const = 0;
		virtual bool Load(Image* image, IStreamBase& streamBase, size_t length) const = 0;
		virtual bool Save(Image* image, IStreamBase& streamBase, const String& type) const = 0;
		virtual void Delete(Image* image) const = 0;
	};
}


#endif // __IIMAGE_H__