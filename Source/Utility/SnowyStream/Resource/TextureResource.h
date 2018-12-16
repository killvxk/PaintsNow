// TextureResource.h
// By PaintDream (paintdream@paintdream.com)
// 2018-3-10
//

#ifndef __TEXTURE_RESOURCE_H__
#define __TEXTURE_RESOURCE_H__

#include "GraphicResourceBase.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		class TextureResource : public GraphicResourceBase {
		public:
			TextureResource(ResourceManager& manager, const ResourceManager::UniqueLocation& uniqueID);
			virtual uint64_t GetMemoryUsage() const;
			virtual void Upload(IRender& render);
			virtual void Download(IRender& render);
			virtual void Attach(IRender& render);
			virtual void Detach(IRender& render);
			virtual bool LoadExternalResource(IStreamBase& streamBase, size_t length);

			virtual TObject<IReflect>& operator () (IReflect& reflect);
			IRender::Texture* GetTexture() const;

		public:
			IRender::Texture* instance;	
			IRender::DATA_TYPE dataType;
			IRender::LAYOUT_TYPE layout;
			uint16_t width, height;
			String textureData;
		};
	}
}

#endif // __TEXTURE_RESOURCE_H__