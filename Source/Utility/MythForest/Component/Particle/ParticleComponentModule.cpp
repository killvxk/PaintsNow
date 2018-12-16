#include "ParticleComponentModule.h"
#include "../../Engine.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;
using namespace PaintsNow::NsSnowyStream;

ParticleComponentModule::ParticleComponentModule(Engine& engine) : ModuleImpl<ParticleComponent>(engine) {}
ParticleComponentModule::~ParticleComponentModule() {}

TObject<IReflect>& ParticleComponentModule::operator () (IReflect& reflect) {
	ReflectClass(ParticleComponentModule)[Interface(Module)];
	if (reflect.IsReflectMethod()) {
		ReflectMethod(RequestCreate)[ScriptMethod = "Create"];
		ReflectMethod(RequestRebuild)[ScriptMethod = "Rebuild"];
	}

	return *this;
}

void ParticleComponentModule::RequestCreate(IScript::Request& request) {
	METHOD_ENTRY(ParticleComponentModule::RequestCreate);
	CHECK_REFERENCES_NONE();

	TShared<ParticleComponent> particleComponent = TShared<ParticleComponent>::Make();
	request.DoLock();
	request << delegate(particleComponent());
	request.UnLock();
}

void ParticleComponentModule::RequestRebuild(IScript::Request& request, IScript::Delegate<ParticleComponent> particleComponent) {
	METHOD_ENTRY(ParticleComponentModule::RequestRebuild);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(particleComponent);

}

