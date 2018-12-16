#include "StdAfx.h"

#include "WinScriptHost.h"
#include "ScriptEngine.h"
using namespace PaintsNow;

class Factory : public TFactoryBase<IScript> {
public:
	Factory() : TFactoryBase<IScript>(Wrap(this, &Factory::CreateEx)) {}

	IScript* CreateEx(const String& info = "") const {
		static ZThreadPthread pthread(false);
		return new CScriptEngine(pthread, info.empty() ? "JScript" : info);
	}
} theFactory;


extern "C" bool LeavesMain(const char* key, PaintsNow::IReflectObject* object, IScript::Request* scriptRequest) {
	static ZThreadPthread thread;
	static UniqueAllocator localAllocator(thread);
	SetGlobalUniqueAllocator(&localAllocator);
	String config = key;

	if (config == "Config") {
		PaintsNow::NsLeavesWing::LeavesApi* api = static_cast<PaintsNow::NsLeavesWing::LeavesApi*>(object);
		api->RegisterFactory("IScript", "WinScriptHost", theFactory);
		_LeavesApi = api;
	}

	return true; // load success
}