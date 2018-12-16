// LeavesApi.h
// By PaintDream (paintdream@paintdream.com)
// 2016-1-2
//

#ifndef __LEAVESAPI_H__
#define __LEAVESAPI_H__

#include "../../Core/Interface/IReflect.h"
#include "../../Core/Template/TFactory.h"
#include "../../Core/Interface/IFrame.h"
#include "../../Core/Interface/IAudio.h"
#include "../../Core/Interface/INetwork.h"
#include "../../Core/Interface/IRandom.h"
#include "../../Core/Interface/IRender.h"
#include "../../Core/Interface/IArchive.h"
#include "../../Core/Interface/IFontBase.h"
#include "../../Core/Interface/IScript.h"
#include "../../Core/Interface/IImage.h"
#include "../../Core/Interface/IDatabase.h"
#include "../../Core/Interface/IAsset.h"
#include "../../Core/Interface/ITimer.h"

#include <string>


namespace PaintsNow {
	namespace NsLeavesWind {
		class LeavesWind;
	}
	namespace NsLeavesWing {
		class LeavesApi : public IReflectObjectComplex {
		public:
			virtual ~LeavesApi();

			virtual TObject<IReflect>& operator () (IReflect& reflect);
			virtual void RegisterFactory(const String& factoryEntry, const String& name, const TWrapper<void*, const String&>* factoryBase) = 0;
			virtual void UnregisterFactory(const String& factoryEntry, const String& name) = 0;
			virtual void QueryFactory(const String& factoryEntry, const TWrapper<void, const String&, const TWrapper<void*, const String&>*>& callback) = 0;
			virtual void WriteString(String& target, const String& source) const = 0;

			enum RUNTIME_STATE {
				RUNTIME_INITIALIZE,
				// RUNTIME_RESET,
				RUNTIME_UNINITIALIZE
			};
			virtual void RegisterRuntimeHook(const TWrapper<void, NsLeavesWind::LeavesWind*, RUNTIME_STATE>& proc) = 0;
		};
	}
}

#endif // __LEAVESAPI_H__