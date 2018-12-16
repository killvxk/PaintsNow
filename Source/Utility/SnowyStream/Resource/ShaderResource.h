// ShaderResource.h
// By PaintDream (paintdream@paintdream.com)
// 2018-3-10
//

#ifndef __SHADER_RESOURCE_H__
#define __SHADER_RESOURCE_H__

#include "GraphicResourceBase.h"
#include "../../../Core/Interface/IAsset.h"
#include "../../../Core/Common/ZPipeline.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		class ShaderResource : public GraphicResourceBase {
		public:
			ShaderResource(ResourceManager& manager, const ResourceManager::UniqueLocation& uniqueID);
			static const String& GetShaderPathPrefix();

			ShaderResource* CreateShadowInstance();
			virtual void GetStaticSwitches(std::map<String, SWITCH_TYPE>& switches);
			virtual void UpdateSwitches(const std::map<String, SWITCH_TYPE>& switches);
			virtual uint64_t GetMemoryUsage() const;
			virtual const String& GetShaderName() const;
			virtual bool operator << (IStreamBase& stream);
			virtual bool operator >> (IStreamBase& stream);
			virtual void Upload(IRender& render);
			virtual void Download(IRender& render);
			virtual void Attach(IRender& render);
			virtual void Detach(IRender& render);
			virtual ZPipeline& GetPipeline();
			virtual Tiny* Duplicate() const;
		};

		template <class T>
		class ShaderResourceImpl : public ShaderResource {
		public:
			ShaderResourceImpl(ResourceManager& manager, const ResourceManager::UniqueLocation& uniqueID) : ShaderResource(manager, uniqueID) {}
			
			virtual ZPipeline& GetPipeline() {
				return pipeline;
			}

			virtual Tiny* Duplicate() const {
				return new ShaderResourceImpl(resourceManager, uniqueLocation);
			}

			virtual void Detach(IRender& render) override {
				pipeline.Cleanup(render);
				ShaderResource::Detach(render);
			}

		protected:
			T pipeline;
		};
	}
}

#endif // __SHADER_RESOURCE_H__