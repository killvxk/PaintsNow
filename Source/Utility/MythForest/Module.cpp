#include "Module.h"
#include "Engine.h"
#include "../../Core/Interface/Interfaces.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;

Module::Module(Engine& e) : IScript::Library(e.interfaces.thread), engine(e) {}

void Module::TickFrame() {}

TObject<IReflect>& Module::operator () (IReflect& reflect) {
	ReflectClass(Module)[Interface(IScript::Library)];

	if (reflect.IsReflectProperty()) {
	}

	return *this;
}