// Remembery.h
// By PaintDream (paintdream@paintdream.com)
// 2015-6-15
//

#ifndef __REMEMBERY_H__
#define __REMEMBERY_H__

#include "Hive.h"

namespace PaintsNow {
	namespace NsRemembery {
		class Remembery : public IScript::Library {
		public:
			Remembery(IThread& threadApi, IDatabase& databaseFactory);
			virtual ~Remembery();
			virtual TObject<IReflect>& operator () (IReflect& reflect);

			void RequestCreateDatabase(IScript::Request& request, String& path, String& username, String& password);
			void RequestExecute(IScript::Request& request, IScript::Delegate<Hive> hive, String& sql, HoneyData& honeyData);
			void RequestStep(IScript::Request& request, IScript::Delegate<Honey> honey, uint32_t count);

		private:
			IDatabase& databaseFactory;
		};
	}
}

#endif // __REMEMBERY_H__