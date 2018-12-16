// ModelComponent.h
// By PaintDream (paintdream@paintdream.com)
// 2018-1-19
//

#ifndef __MODELCOMPONENT_H__
#define __MODELCOMPONENT_H__

#include "../../Entity.h"
#include "../Renderable/RenderableComponent.h"
#include "../../../SnowyStream/Resource/ModelResource.h"

namespace PaintsNow {
	namespace NsMythForest {
		class ModelComponent : public RenderableComponent {
		public:
			enum {
				MODEL_COMPONENT_OVERRIDE_RENDERDATA = RENDERABLE_COMPONENT_CUSTOM_BEGIN,
				MODEL_COMPONENT_CUSTOM_BEGIN = RENDERABLE_COMPONENT_CUSTOM_BEGIN << 1
			};

			// delayed loader
			ModelComponent(TShared<NsSnowyStream::ModelResource> modelResource);
			virtual void RenderInstance(IRender& render, IRender::Target* renderTarget, Instance& instance) override;

		protected:
			virtual bool UpdateRenderData() override;

		protected:
			TShared<NsSnowyStream::ModelResource> modelResource;
			std::vector<String> renderData;
		};
	}
}


#endif // __MODELCOMPONENT_H__
