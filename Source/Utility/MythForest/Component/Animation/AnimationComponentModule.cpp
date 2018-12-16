#include "AnimationComponentModule.h"
#include "AnimationComponent.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;
using namespace PaintsNow::NsSnowyStream;

AnimationComponentModule::AnimationComponentModule(Engine& engine) : ModuleImpl<AnimationComponent>(engine) {}

TObject<IReflect>& AnimationComponentModule::operator () (IReflect& reflect) {
	ReflectClass(AnimationComponentModule)[Interface(Module)];

	if (reflect.IsReflectMethod()) {
		ReflectMethod(RequestCreate)[ScriptMethod = "Create"];
		ReflectMethod(RequestRebuild)[ScriptMethod = "Rebuild"];
	}

	return *this;
}

void AnimationComponentModule::RequestCreate(IScript::Request& request) {
	METHOD_ENTRY(AnimationComponentModule::RequestCreate);
	CHECK_REFERENCES_NONE();

	TShared<AnimationComponent> animationComponent = TShared<AnimationComponent>::Make();
	request.DoLock();
	request << delegate(animationComponent());
	request.UnLock();
}

void AnimationComponentModule::RequestRebuild(IScript::Request& request, IScript::Delegate<AnimationComponent> animationComponent) {
	METHOD_ENTRY(AnimationComponentModule::RequestRebuild);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(animationComponent);

}