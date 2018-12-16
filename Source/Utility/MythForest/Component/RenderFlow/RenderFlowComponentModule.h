// RenderFlowComponent.h
// By PaintDream (paintdream@paintdream.com)
// 2018-1-19
//

#ifndef __RENDERFLOWCOMPONENTMODULE_H__
#define __RENDERFLOWCOMPONENTMODULE_H__

#include "RenderFlowComponent.h"

namespace PaintsNow {
	namespace NsMythForest {
		class RenderStage;
		class RenderFlowComponent;
		class RenderFlowComponentModule : public ModuleImpl<RenderFlowComponent> {
		public:
			RenderFlowComponentModule(Engine& engine);
			virtual TObject<IReflect>& operator () (IReflect& reflect);
			virtual void RegisterNodeTemplate(const String& key, const TFactoryBase<RenderStage>& t);

			void RequestCreate(IScript::Request& request);
			void RequestCreateRenderStage(IScript::Request& request, IScript::Delegate<RenderFlowComponent> renderFlow, String& name, String& config);
			void RequestEnumerateRenderStagePorts(IScript::Request& request, IScript::Delegate<RenderStage> renderStage);
			void RequestConnectRenderStagePort(IScript::Request& request, IScript::Delegate<RenderStage> from, const String& fromPortName, IScript::Delegate<RenderStage> to, const String& toPortName);
			void RequestDisconnectRenderStagePort(IScript::Request& request, IScript::Delegate<RenderStage> from, const String& fromPortName, IScript::Delegate<RenderStage> to, const String& toPortName);
			void RequestExportRenderStagePort(IScript::Request& request, IScript::Delegate<RenderFlowComponent> renderFlowComponent, String& symbol, IScript::Delegate<RenderStage> stage, String& port);
			void RequestDeleteRenderStage(IScript::Request& request, IScript::Delegate<RenderFlowComponent> renderFlow, IScript::Delegate<RenderStage> renderStage);

		protected:
			std::map<String, TFactoryBase<RenderStage> > stageTemplates;
		};
	}
}


#endif // __RENDERFLOWCOMPONENTMODULE_H__
