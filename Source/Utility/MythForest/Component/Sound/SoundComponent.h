// Source.h
// By PaintDream (paintdream@paintdream.com)
// 2015-5-5
//

#ifndef __SOUNDCOMPONENT_H__
#define __SOUNDCOMPONENT_H__

#include "../../Component.h"
#include "../../../SnowyStream/SnowyStream.h"
#include "../../../SnowyStream/Resource/AudioResource.h"

namespace PaintsNow {
	namespace NsMythForest {
		class SoundComponent : public Component {
		public:
			SoundComponent(IAudio& audio, TShared<NsSnowyStream::AudioResource> audioResource, IScript::Request::Ref callback);
			virtual ~SoundComponent();
			virtual TObject<IReflect>& operator () (IReflect& reflect);

			void Step(IScript::Request& request);
			int64_t GetDuration() const;
			void Seek(double time);
			void Play();
			void Pause();
			void Stop();
			void Rewind();
			virtual void ScriptUninitialize(IScript::Request& request);
			bool IsOnline() const;
			bool IsPlaying() const;

		protected:
			IAudio& audio;
			IAudio::Source* source;
			TShared<NsSnowyStream::AudioResource> audioResource;
			TWrapper<size_t> stepWrapper;
			IScript::Request::Ref callback;
		};
	}
}


#endif // __SOUNDCOMPONENT_H__