// ZDatabaseSqlite.h
// By PaintDream (paintdream@paintdream.com)
// 2015-12-30
//

#ifndef __ZDATABSESQLITE_H__
#define __ZDATABSESQLITE_H__

#include "../../../Core/Interface/IDatabase.h"
extern "C" {
	#include "Core/sqlite3.h"
}

namespace PaintsNow {
	class ZDatabaseSqlite final : public IDatabase {
	public:
		ZDatabaseSqlite();
		virtual ~ZDatabaseSqlite();
		virtual Database* Connect(const String& target, const String& username, const String& password);
		virtual void Close(Database* database);
		virtual MetaData* Execute(Database* database, const String& statementTemplate, MetaData* postData);
	};
}


#endif // __ZDATABSESQLITE_H__