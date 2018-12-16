// MythForest.h
// By PaintDream (paintdream@paintdream.com)
// 2015-5-1
//

#ifndef __MYTHFOREST_H__
#define __MYTHFOREST_H__

#include "../SnowyStream/SnowyStream.h"
#include "../../Core/Interface/IScript.h"
#include "Entity.h"
#include "Engine.h"
#include "Component/Animation/AnimationComponentModule.h"
#include "Component/Batch/BatchComponentModule.h"
#include "Component/Camera/CameraComponentModule.h"
#include "Component/Compute/ComputeComponentModule.h"
#include "Component/Event/EventListenerComponentModule.h"
#include "Component/Field/FieldComponentModule.h"
#include "Component/Form/FormComponentModule.h"
#include "Component/Light/LightComponentModule.h"
#include "Component/Model/ModelComponentModule.h"
#include "Component/Particle/ParticleComponentModule.h"
#include "Component/RenderFlow/RenderFlowComponentModule.h"
#include "Component/Shape/ShapeComponentModule.h"
#include "Component/Sound/SoundComponentModule.h"
#include "Component/Space/SpaceComponentModule.h"
#include "Component/Surface/SurfaceComponentModule.h"
#include "Component/Terrain/TerrainComponentModule.h"
#include "Component/Transform/TransformComponentModule.h"

namespace PaintsNow {
	namespace NsMythForest {
		class MythForest : public IScript::Library {
		public:
			MythForest(Interfaces& interfaces, NsSnowyStream::SnowyStream& snowyStream, NsBridgeSunset::BridgeSunset& bridgeSunset, NsFluteLeaves::FluteLeaves& fluteLeaves, size_t warpCount);
			virtual ~MythForest();
			virtual TObject<IReflect>& operator () (IReflect& reflect);

			// static int main(int argc, char* argv[]);
			virtual void OnRender();

		public:
			void RequestEnumerateComponentModules(IScript::Request& request);

			// Entity-Component System APIs.
			void RequestCreateEntity(IScript::Request& request);
			void RequestQueueRoutine(IScript::Request& request, IScript::Delegate<Unit> entity, IScript::Request::Ref callback);
			void RequestAddEntityComponent(IScript::Request& request, IScript::Delegate<Entity> entity, IScript::Delegate<Component> component);
			void RequestRemoveEntityComponent(IScript::Request& request, IScript::Delegate<Entity> entity, IScript::Delegate<Component> component);
			void RequestUpdateEntity(IScript::Request& request, IScript::Delegate<Entity> entity);
			void RequestGetEntityComponents(IScript::Request& request, IScript::Delegate<Entity> entity, String& componentName);
			void RequestGetUniqueEntityComponent(IScript::Request& request, IScript::Delegate<Entity> entity, String& componentName);
			void RequestClearEntityComponents(IScript::Request& request, IScript::Delegate<Entity> entity);

			// Build-in sub modules
		private:
			Engine engine;
			AnimationComponentModule animationComponentModule;
			BatchComponentModule batchComponentModule;
			CameraComponentModule cameraComponentModule;
			ComputeComponentModule computeComponentModule;
			EventListenerComponentModule eventListenerComponentModule;
			FieldComponentModule fieldComponentModule;
			FormComponentModule formComponentModule;
			LightComponentModule lightComponentModule;
			ModelComponentModule modelComponentModule;
			ParticleComponentModule particleComponentModule;
			RenderFlowComponentModule renderFlowComponentModule;
			ShapeComponentModule shapeComponentModule;
			SoundComponentModule soundComponentModule;
			SpaceComponentModule spaceComponentModule;
			SurfaceComponentModule surfaceComponentModule;
			TerrainComponentModule terrainComponentModule;
			TransformComponentModule transformComponentModule;

			TShared<Entity> globalEntity;
		};
	}
}


#endif // __MYTHFOREST_H__
