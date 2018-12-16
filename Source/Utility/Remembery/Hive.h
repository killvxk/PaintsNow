// Hive.h
// By PaintDream (paintdream@paintdream.com)
// 2015-12-31
//

#ifndef __HIVE_H__
#define __HIVE_H__

#include "../../Core/Object/Tiny.h"
#include "Honey.h"

namespace PaintsNow {
	namespace NsRemembery {
		class Hive : public SharedTiny {
		public:
			Hive(IDatabase& base, IDatabase::Database* database);
			virtual ~Hive();

			virtual TObject<IReflect>& operator () (IReflect& reflect);
			TShared<Honey> Execute(const String& sql, HoneyData& honeyData);

		private:
			IDatabase& base;
			IDatabase::Database* database;
		};
	}
}


#endif // __HIVE_H__