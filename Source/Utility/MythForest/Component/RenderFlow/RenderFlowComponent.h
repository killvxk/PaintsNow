// StandardRenderFlow.h
// By PaintDream (paintdream@paintdream.com)
// 2018-8-12
//

#ifndef __STANDARDRENDERFLOW_H__
#define __STANDARDRENDERFLOW_H__

#include "../../Entity.h"
#include "../../Component.h"
#include "../../../../Core/Object/Graph.h"
#include "../../../../Core/Common/ZRenderFlow.h"
#include "RenderStage.h"

namespace PaintsNow {
	namespace NsMythForest {
		class RenderFlowComponent : public Component, public ZRenderFlow, protected Graph<RenderStage> {
		public:
			RenderFlowComponent();
			enum RENDERFLOWFLAG {
				RENDERFLOWCOMPONENT_OUT_COLOR = COMPONENT_CUSTOM_BEGIN,
				RENDERFLOWCOMPONENT_OUT_DEPTH = COMPONENT_CUSTOM_BEGIN << 1,
				RENDERFLOWCOMPONENT_CUSTOM_BEGIN = COMPONENT_CUSTOM_BEGIN << 2
			};

			void AddNode(RenderStage* renderStage);
			void RemoveNode(RenderStage* renderStage);
			bool ExportSymbol(const String& symbol, RenderStage* renderStage, const String& port);
			RenderStage::Port* ImportSymbol(const String& symbol);
			virtual void UpdateCacheStages();

		protected:
			virtual TObject<IReflect>& operator () (IReflect& reflect);
			std::map<String, std::pair<RenderStage*, String> > symbolMap;
		};
	}
}


#endif // __STANDARDRENDERFLOW_H__