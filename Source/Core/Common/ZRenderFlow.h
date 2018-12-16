// ZRenderFlow.h
// By PaintDream (paintdream@paintdream.com)
// 2018-8-12
//

#ifndef __ZRENDERFLOW_H__
#define __ZRENDERFLOW_H__

#include "../Interface/IReflect.h"
#include "../Interface/Interfaces.h"

namespace PaintsNow {
	class IRenderStage {};
	class ZRenderFlow {
	public:
		void InvokeRender(IRender& render); 
		virtual void UpdateCacheStages() = 0;
		typedef std::vector<IRenderStage*> Milestone;

	protected:
		std::vector<Milestone> cachedRenderStages;
	};
}

#endif // __ZRENDERFLOW_H__