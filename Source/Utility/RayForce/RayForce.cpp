#include "RayForce.h"
#include "Tunnel.h"
#include "LibLoader.h"

using namespace PaintsNow;
using namespace PaintsNow::NsRayForce;


RayForce::RayForce(IThread& thread, NsSnowyStream::SnowyStream& s) : Library(thread), snowyStream(s) {}

TObject<IReflect>& RayForce::operator () (IReflect& reflect) {
	ReflectClass(RayForce)[Interface(IScript::Library)];
	if (reflect.IsReflectMethod()) {
		ReflectMethod(RequestCreateObject)[ScriptMethod = "CreateObject"];
		ReflectMethod(RequestQueryInterface)[ScriptMethod = "QueryInterface"];
	}

	return *this;
}

void RayForce::RequestCreateObject(IScript::Request& request, String& path, String& data) {
	METHOD_ENTRY(RayForce::RequestCreateObject);

	IReflectObject* object = nullptr;
	IArchive& archive = snowyStream.GetInterfaces().archive;
	Bridge* bridge = nullptr;
	for (std::list<Bridge*>::iterator p = bridges.begin(); p != bridges.end(); ++p) {
		if ((object = (*p)->Create(request, archive, path, data))) {
			bridge = *p;
			break;
		}
	}

	if (object != nullptr) {
		assert(bridge != nullptr);
		// will be managed by script.
		TShared<Tunnel> tunnel = TShared<Tunnel>::Make(bridge, object);
		request.DoLock();
		request << delegate(tunnel());
		request.UnLock();
	}
}

void RayForce::RequestQueryInterface(IScript::Request& request, IScript::Delegate<Tunnel> tunnel) {
	METHOD_ENTRY(RayForce::RequestQueryInterface);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(tunnel);

	tunnel->Dump(request);
}

void RayForce::Register(Bridge* bridge) {
	DoLock();
	bridges.push_back(bridge);
	UnLock();
}
