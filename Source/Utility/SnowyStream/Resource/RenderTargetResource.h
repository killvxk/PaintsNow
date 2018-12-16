// RenderTargetResource.h
// By PaintDream (paintdream@paintdream.com)
// 2018-3-10
//

#ifndef __RENDERTARGET_RESOURCE_H__
#define __RENDERTARGET_RESOURCE_H__

#include "GraphicResourceBase.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		class RenderTargetResource : public GraphicResourceBase {
		public:
			RenderTargetResource(ResourceManager& manager, const ResourceManager::UniqueLocation& uniqueID);
			virtual uint64_t GetMemoryUsage() const;
			virtual void Upload(IRender& render);
			virtual void Download(IRender& render);
			virtual void Attach(IRender& render);
			virtual void Detach(IRender& render);

			virtual TObject<IReflect>& operator () (IReflect& reflect);
			IRender::Target* GetRenderTarget() const;
			void Resize(uint32_t width, uint32_t height, std::vector<IRender::DATA_TYPE>& desc);

		protected:
			IRender::Target* renderTarget;
			uint32_t width, height;
			std::vector<IRender::DATA_TYPE> dataFormat;
		};
	}
}

#endif // __RENDERTARGET_RESOURCE_H__