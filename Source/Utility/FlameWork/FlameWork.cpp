#include "FlameWork.h"

using namespace PaintsNow;
using namespace PaintsNow::NsFlameWork;

FlameWork::FlameWork(IThread& threadApi, IScript& ns) : IScript::Library(threadApi), nativeScript(ns) {}

void FlameWork::RequestCompileNativeCode(IScript::Request& request, String& code) {
	METHOD_ENTRY(FlameWork::RequestCompileNativeCode);
	TShared<Native> native = TShared<Native>::Make(nativeScript);
	if (native->Compile(code)) {
		request.DoLock();
		request << delegate(native());
		request.UnLock();
	}
}

void FlameWork::RequestExecuteNative(IScript::Request& request, IScript::Delegate<Native> native, String& entry, std::vector<String>& params) {
	METHOD_ENTRY(FlameWork::RequestExecuteNative);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(native);
	native->Execute(request, entry, params);
}

TObject<IReflect>& FlameWork::operator () (IReflect& reflect) {
	ReflectClass(FlameWork)[Interface(IScript::Library)];
	if (reflect.IsReflectProperty()) {
		ReflectMethod(RequestCompileNativeCode)[ScriptMethod = "CompileNativeCode"];
		ReflectMethod(RequestExecuteNative)[ScriptMethod = "ExecuteNative"];
	}

	return *this;
}
