// TerrainResource.h
// By PaintDream (paintdream@paintdream.com)
// 2018-3-10
//

#ifndef __TERRAINRESOURCE_H__
#define __TERRAINRESOURCE_H__

#include "GraphicResourceBase.h"
#include "../../../Core/Interface/IAsset.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		class TerrainResource : public GraphicResourceBase {
		public:
			TerrainResource(ResourceManager& manager, const ResourceManager::UniqueLocation& uniqueID);
			virtual TObject<IReflect>& operator () (IReflect& reflect);
			virtual uint64_t GetMemoryUsage() const;
			virtual bool operator << (IStreamBase& stream);
			virtual bool operator >> (IStreamBase& stream);
			virtual void Upload(IRender& render);
			virtual void Download(IRender& render);
			virtual void Attach(IRender& render);
			virtual void Detach(IRender& render);

		protected:
			std::vector<float> terrainData;
			uint32_t width, height;
			float deltaHeight;
		};
	}
}

#endif // __TERRAINRESOURCE_H__