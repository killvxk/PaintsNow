#include "SoundComponentModule.h"
#include "../../Engine.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;
using namespace PaintsNow::NsSnowyStream;

SoundComponentModule::SoundComponentModule(Engine& engine) : ModuleImpl<SoundComponent>(engine) {}
SoundComponentModule::~SoundComponentModule() {}

TObject<IReflect>& SoundComponentModule::operator () (IReflect& reflect) {
	ReflectClass(SoundComponentModule)[Interface(Module)];
	if (reflect.IsReflectMethod()) {
		ReflectMethod(RequestCreateSource)[ScriptMethod = "CreateSource"];
		ReflectMethod(RequestGetSourceDuration)[ScriptMethod = "GetSourceDuration"];
		ReflectMethod(RequestMoveSource)[ScriptMethod = "MoveSource"];
		ReflectMethod(RequestSeekSource)[ScriptMethod = "SeekSource"];
		ReflectMethod(RequestPlaySource)[ScriptMethod = "PlaySource"];
		ReflectMethod(RequestPauseSource)[ScriptMethod = "PauseSource"];
		ReflectMethod(RequestStopSource)[ScriptMethod = "StopSource"];
		ReflectMethod(RequestRewindSource)[ScriptMethod = "RewindSource"];
		ReflectMethod(RequestIsSourcePaused)[ScriptMethod = "IsSourcePaused"];
	}

	return *this;
}

void SoundComponentModule::RequestCreateSource(IScript::Request& request, String path, IScript::Request::Ref callback) {
	METHOD_ENTRY(SoundComponentModule::RequestCreateSource);
	CHECK_REFERENCES_WITH_TYPE(callback, IScript::Request::FUNCTION);

	AudioResource* audioResource = engine.snowyStream.CreateReflectedResource(UniqueType<AudioResource>(), path);
	if (audioResource != nullptr) {
		TShared<SoundComponent> soundComponent = TShared<SoundComponent>::Make(engine.interfaces.audio, audioResource, callback);
		request.DoLock();
		request << delegate(soundComponent());
		request.UnLock();
		audioResource->ReleaseObject();
	} else {
		request.Error(String("SoundComponentModule::RequestCreateSource: invalid path '") + path + "'");
	}
}

void SoundComponentModule::RequestMoveSource(IScript::Request& request, IScript::Delegate<SoundComponent> source, ZFloat3& pos) {
	METHOD_ENTRY(SoundComponentModule::RequestMoveSource);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(source);
}

void SoundComponentModule::RequestSeekSource(IScript::Request& request, IScript::Delegate<SoundComponent> source, double time) {
	METHOD_ENTRY(SoundComponentModule::RequestSeekSource);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(source);
	source->Seek(time);
}

void SoundComponentModule::RequestGetSourceDuration(IScript::Request& request, IScript::Delegate<SoundComponent> source) {
	METHOD_ENTRY(SoundComponentModule::RequestGetSourceDuration);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(source);
	int64_t duration = source->GetDuration();
	request.DoLock();
	request << duration;
	request.UnLock();
}

void SoundComponentModule::RequestPlaySource(IScript::Request& request, IScript::Delegate<SoundComponent> source) {
	METHOD_ENTRY(SoundComponentModule::RequestPlaySource);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(source);
	source->Play();
}

void SoundComponentModule::RequestPauseSource(IScript::Request& request, IScript::Delegate<SoundComponent> source) {
	METHOD_ENTRY(SoundComponentModule::RequestPauseSource);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(source);
	source->Pause();
}

void SoundComponentModule::RequestStopSource(IScript::Request& request, IScript::Delegate<SoundComponent> source) {
	METHOD_ENTRY(SoundComponentModule::RequestStopSource);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(source);
	source->Stop();
}

void SoundComponentModule::RequestRewindSource(IScript::Request& request, IScript::Delegate<SoundComponent> source) {
	METHOD_ENTRY(SoundComponentModule::RequestRewindSource);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(source);
	source->Rewind();
}

void SoundComponentModule::RequestIsSourcePaused(IScript::Request& request, IScript::Delegate<SoundComponent> source) {
	assert(false);
}
