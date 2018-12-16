// RayForce.h
// By PaintDream (paintdream@paintdream.com)
// 2015-6-15
//

#ifndef __RAYFORCE_H__
#define __RAYFORCE_H__

#include "../../Core/Interface/IScript.h"
#include "../SnowyStream/SnowyStream.h"
#include "Tunnel.h"
#include "Bridge.h"

namespace PaintsNow {
	namespace NsRayForce {
		class RayForce : public IScript::Library {
		public:
			RayForce(IThread& thread, NsSnowyStream::SnowyStream& snowyStream);
			virtual TObject<IReflect>& operator () (IReflect& reflect);
			void Register(Bridge* bridge);

		public:
			void RequestCreateObject(IScript::Request& request, String& path, String& data);
			void RequestQueryInterface(IScript::Request& request, IScript::Delegate<Tunnel> tunnel);

		protected:
			NsSnowyStream::SnowyStream& snowyStream;
			std::list<Bridge*> bridges;
		};
	}
}



#endif // __RAYFORCE_H__