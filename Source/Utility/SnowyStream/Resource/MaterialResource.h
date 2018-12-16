// MaterialResource.h
// By PaintDream (paintdream@paintdream.com)
// 2018-3-10
//

#ifndef __MATERIAL_RESOURCE_H__
#define __MATERIAL_RESOURCE_H__

#include "GraphicResourceBase.h"
#include "ShaderResource.h"
#include "../../../Core/Interface/IAsset.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		class MaterialResource : public GraphicResourceBase {
		public:
			MaterialResource(ResourceManager& manager, const ResourceManager::UniqueLocation& uniqueID);
			virtual uint64_t GetMemoryUsage() const;
			virtual void Upload(IRender& render);
			virtual void Download(IRender& render);
			virtual void Attach(IRender& render);
			virtual void Detach(IRender& render);
			virtual TObject<IReflect>& operator () (IReflect& reflect);

			size_t GetMaterialCount() const;
			TShared<ShaderResource> GetMaterialShader(size_t i) const;
			const IAsset::Material& GetMaterialParam(size_t i) const;
			void FlushMaterials(IRender& render);

		protected:
			TShared<ShaderResource> MakeShaderResource(IRender& render, const IAsset::Material& material);
			String MatchStaticSwitches(std::map<String, SWITCH_TYPE>& switches, const IAsset::Material& material);

		protected:
			IAsset::MaterialCollection collection;
			std::vector<TShared<ShaderResource> > shaderResources;
		};
	}
}

#endif // __MATERIAL_RESOURCE_H__