// ModelResource.h
// By PaintDream (paintdream@paintdream.com)
// 2018-3-10
//

#ifndef __MODEL_RESOURCE_H__
#define __MODEL_RESOURCE_H__

#include "GraphicResourceBase.h"
#include "../../../Core/Interface/IAsset.h"
#include "MaterialResource.h"
#include "TextureResource.h"
#include "MeshResource.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		// While ModelResource is not a storagable resource
		class ModelResource : public GraphicResourceBase {
		public:
			ModelResource(ResourceManager& manager, const ResourceManager::UniqueLocation& uniqueID);

			virtual TObject<IReflect>& operator () (IReflect& reflect);
			virtual void Attach(IRender& device);
			virtual void Detach(IRender& device);

			virtual void Upload(IRender& render);

			TShared<MeshResource> meshResource;
			TShared<MaterialResource> materialResource;

			std::vector<TShared<TextureResource> > textureResources;
			std::vector<String> renderData;
		};
	}
}

#endif // __MODEL_RESOURCE_H__