// PipeMsg.h
// By PaintDream (paintdream@paintdream.com)
// 2018-2-10
//

#ifndef __PIPEMSG_H__
#define __PIPEMSG_H__

#ifdef _WIN32

#include "../LostDream.h"
#include "../../../Driver/Tunnel/WinPipe/ZWinPipe.h"

namespace PaintsNow {
	namespace NsLostDream {
		class PipeMsg : public LostDream::Qualifier {
		public:
			virtual bool Initialize();
			virtual bool Run(int randomSeed, int length);
			virtual void Summary();

			virtual TObject<IReflect>& operator () (IReflect& reflect);
			PipeMsg();
			enum { MESSAGE_LENGTH = 64 };
			void OnListenerEvent(ITunnel::EVENT event);
			void OnServerEvent(ITunnel::EVENT event);
			void OnClientEvent(ITunnel::EVENT event);
			const TWrapper<void, ITunnel::EVENT> OnConnection(ITunnel::Connection* connection);

			ZWinPipe pipe;
			ITunnel::Listener* listener;
			ITunnel::Connection* connection;
			ITunnel::Connection* serverConnection;
		};
	}
}

#endif // _WIN32

#endif // __PIPEMSG_H__
