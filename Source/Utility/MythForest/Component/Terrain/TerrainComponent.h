// TerrainComponent.h
// By PaintDream (paintdream@paintdream.com)
// 2018-1-19
//

#ifndef __TERRAINCOMPONENT_H__
#define __TERRAINCOMPONENT_H__

#include "../../Entity.h"
#include "../Renderable/RenderableComponent.h"
#include "../../../SnowyStream/Resource/TerrainResource.h"

namespace PaintsNow {
	namespace NsMythForest {
		class TerrainComponent : public RenderableComponent {
		public:
			TerrainComponent();
			virtual bool UpdateRenderData() override;
			virtual void RenderInstance(IRender& render, IRender::Target* renderTarget, Instance& instance) override;

			TShared<NsSnowyStream::TerrainResource> terrainResource;
		};
	}
}


#endif // __TERRAINCOMPONENT_H__
