#include "ProxyStub.h"

using namespace PaintsNow;
using namespace PaintsNow::NsGalaxyWeaver;

ProxyStub::ProxyStub(IThread& thread, ITunnel& tunnel, const String& entry, const TWrapper<void, IScript::Request&, bool, ZRemoteProxy::STATUS, const String&>& statusHandler) : remoteProxy(thread, tunnel, Wrap(this, &ProxyStub::StaticCreate), entry, statusHandler) {

}

IScript::Object* ProxyStub::StaticCreate(const String& entry) {
	return this;
}