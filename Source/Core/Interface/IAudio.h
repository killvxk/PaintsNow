// IAudio.h -- Audio player interface
// By PaintDream (paintdream@paintdream.com)
// 2014-12-1
//

#ifndef __IAUDIO_H__
#define __IAUDIO_H__


#include "../PaintsNow.h"
#include "IFilterBase.h"
#include "../Common/ZType.h"
#include "IDevice.h"

namespace PaintsNow {
	class IAudio : public IDevice {
	public:
		class Buffer {};
		class Source {};
		virtual ~IAudio();

		virtual Buffer* CreateBuffer() = 0;

		class Decoder : public IStreamBase {
		public:
			enum FORMAT {
				MONO8 = 0, 
				MONO16 = 1,
				STEREO8 = 2,
				STEREO16 = 3
			};

			virtual size_t GetSampleRate() const = 0;
			virtual size_t GetSampleCount() const = 0; // returns negetive if unknown
			virtual FORMAT GetFormat() const = 0;
		};

		virtual void SetBufferStream(Buffer* buffer, Decoder& stream, bool online) = 0;
		virtual void DeleteBuffer(Buffer* buffer) = 0;

		virtual Source* CreateSource() = 0;
		virtual void DeleteSource(Source* sourceHandle) = 0;
		virtual void SetSourcePosition(Source* sourceHandle, const ZFloat3& position) = 0;
		virtual void SetSourceVolume(Source* sourceHandle, float volume) = 0;
		virtual TWrapper<size_t> SetSourceBuffer(Source* sourceHandle, const Buffer* buffer) = 0;
		virtual void SetListenerPosition(const ZFloat3& position) = 0;
		virtual void Play(Source* sourceHandle) = 0;
		virtual void Pause(Source* sourceHandle) = 0;
		virtual void Rewind(Source* sourceHandle) = 0;
		virtual void Stop(Source* sourceHandle) = 0;
		// virtual void Seek(Source* sourceHandle, IStreamBase::SEEK_OPTION option, long offset) = 0;
	};
}

#endif // __IAUDIO_H__
