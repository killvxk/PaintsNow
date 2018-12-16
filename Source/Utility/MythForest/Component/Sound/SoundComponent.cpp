#include "SoundComponent.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;
using namespace PaintsNow::NsSnowyStream;

SoundComponent::SoundComponent(IAudio& a, TShared<AudioResource> resource, IScript::Request::Ref r) : audio(a), audioResource(resource), callback(r) {
	source = audio.CreateSource();
	audio.SetSourceBuffer(source, resource->GetAudioBuffer());
}

void SoundComponent::ScriptUninitialize(IScript::Request& request) {
	request.DoLock();
	if (callback) {
		request.Dereference(callback);
	}
	request.UnLock();

	Component::ScriptUninitialize(request);
}

bool SoundComponent::IsOnline() const {
	return callback;
}

int64_t SoundComponent::GetDuration() const {
	return 0;
}

TObject<IReflect>& SoundComponent::operator () (IReflect& reflect) {
	ReflectClass(SoundComponent);

	if (reflect.IsReflectMethod()) {
		ReflectMethod(GetDuration);
		ReflectMethod(Seek);
		ReflectMethod(Play);
		ReflectMethod(Pause);
		ReflectMethod(Stop);
		ReflectMethod(Rewind);
		ReflectMethod(IsOnline);
		ReflectMethod(IsPlaying);
	}

	return *this;
}

bool SoundComponent::IsPlaying() const {
	return !!(Flag() & Tiny::TINY_ACTIVATED);
}

void SoundComponent::Step(IScript::Request& request) {
	if (stepWrapper) {
		size_t t = stepWrapper();
		if (t == (size_t)-1) {
			Flag() &= ~Tiny::TINY_ACTIVATED;
		}

		if (t != 0 && callback) {
			request.DoLock();
			request.Push();
			request << (t == (size_t)-1 ? (int64_t)0 : (int64_t)t);
			request.Call(Sync, callback);
			request.Pop();
			request.UnLock();
		}
	}
}

SoundComponent::~SoundComponent() {
	audio.DeleteSource(source);
}

void SoundComponent::Play() {
	Flag() |= TINY_ACTIVATED;
	audio.Play(source);
}

void SoundComponent::Pause() {
	Flag() &= ~TINY_ACTIVATED;
	audio.Pause(source);
}

void SoundComponent::Stop() {
	Flag() &= ~TINY_ACTIVATED;
	audio.Stop(source);
}

void SoundComponent::Seek(double time) {
	// buffer->decoder->Seek(IStreamBase::BEGIN, (long)(time * buffer->decoder->GetSampleRate()));
	// audio.Seek(source, IStreamBase::CUR, time);
}

void SoundComponent::Rewind() {
	audio.Rewind(source);
}
