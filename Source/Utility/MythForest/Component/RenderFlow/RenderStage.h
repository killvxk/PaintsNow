// RenderStage.h
// PaintDream (paintdream@paintdream.com)
// 2018-9-11
//

#ifndef __RENDERSTAGE_H__
#define __RENDERSTAGE_H__

#include "../../../../Core/Common/ZRenderFlow.h"
#include "../../../../Core/Object/Graph.h"
#include "RenderPort.h"

namespace PaintsNow {
	namespace NsMythForest {
		class RenderFlowComponent;
		class RenderStage : public GraphNode<SharedTiny, RenderPort>, public IRenderStage {
		public:
			 RenderStage();
			 bool ValidateTargets(ZPipeline& pipeline);

		protected:
			RenderFlowComponent* renderFlowComponent;
		};
	}
}

#endif // __RENDERSTAGE_H__