// CameraComponent.h
// By PaintDream (paintdream@paintdream.com)
// 2018-1-19
//

#ifndef __CAMERACOMPONENTMODULE_H__
#define __CAMERACOMPONENTMODULE_H__

#include "CameraComponent.h"

namespace PaintsNow {
	namespace NsMythForest {
		class RenderFlowComponent;
		class EventListenerComponentModule;
		class CameraComponentModule : public ModuleImpl<CameraComponent> {
		public:
			CameraComponentModule(Engine& engine);
			virtual TObject<IReflect>& operator () (IReflect& reflect);

		public:
			// Interfaces
			void RequestCreate(IScript::Request& request, IScript::Delegate<RenderFlowComponent> renderFlowComponent);
			void RequestSetSymbol(IScript::Request& request, IScript::Delegate<CameraComponent> cameraComponent, String& inputSymbol, String& lightSymbol, String& outputSymbol);
			void RequestBindRootEntity(IScript::Request& request, IScript::Delegate<CameraComponent> camera, IScript::Delegate<Entity> entity);
			void RequestComputeCastRayFromPoint(IScript::Request& request, IScript::Delegate<CameraComponent> camera, ZFloat2& screenPosition);
			void RequestSetPerspective(IScript::Request& request, IScript::Delegate<CameraComponent> camera, float d, float n, float f, float r);
			void RequestGetPerspective(IScript::Request& request, IScript::Delegate<CameraComponent> camera);
			void RequestSetVisibleDistance(IScript::Request& request, IScript::Delegate<CameraComponent> camera, float distance);
			void RequestGetVisibleDistance(IScript::Request& request, IScript::Delegate<CameraComponent> camera);
		};
	}
}


#endif // __CAMERACOMPONENTMODULE_H__
