#include "ObjectDumper.h"
#include "Tunnel.h"

using namespace PaintsNow;
using namespace PaintsNow::NsRayForce;

ObjectDumper::ObjectDumper(IScript::Request& r, Tunnel& t, const std::map<Unique, Type>& m) :  ZScriptReflect(r, false, m), request(r), tunnel(t) {
}

void ObjectDumper::ProcessProperty(IReflectObject& s, Unique typeID, const char* name, void* base, void* ptr, const MetaChainBase* meta) {
	// TODO: place code here
	ZScriptReflect::ProcessProperty(s, typeID, name, base, ptr, meta);
}

void ObjectDumper::ProcessMethod(Unique typeID, const char* name, const TProxy<>* p, const IReflect::Param& retValue, const std::vector<IReflect::Param>& params, const MetaChainBase* meta) {
	if (!read) {
		Proxy& proxy = tunnel.NewProxy(p);
		request << key(name) << request.Adapt(Wrap(&proxy, &Proxy::OnCall));

		String extraKey = String("meta$") + name;
		// generate parameter info
		request << key(extraKey.c_str()) << begintable
			<< key("retval") << begintable
				<< key("type") << GetType(retValue).name
			<< endtable
		<< key("params") << begintable;

		for (size_t i = 0; i < params.size(); i++) {
			request << begintable
				<< key("type") << GetType(params[i]).name
				<< key("name") << params[i].name
				<< endtable;

			/*
			if (!params[i].desc.empty()) {
				const String& object = "<Object>";
				size_t pos = type.find(object);
				assert(pos != String::npos);
				type.replace(pos, object.size(), params[i].desc);
			}*/
		}

		request << endtable << endtable;
	}
}

