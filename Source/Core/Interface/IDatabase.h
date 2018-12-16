
// IDatabase.h -- Database player interface
// By PaintDream (paintdream@paintdream.com)
// 2014-12-1
//

#ifndef __IDATABASE_H__
#define __IDATABASE_H__

#include "../PaintsNow.h"
#include "../Common/ZType.h"
#include "../Interface/IReflect.h"
#include "IDevice.h"
#include <string>

namespace PaintsNow {
	class IDatabase : public IDevice {
	public:
		virtual ~IDatabase();
		class Database {};

		// class MetaData : public IIterator {};
		typedef IIterator MetaData;
		virtual Database* Connect(const String& target, const String& username, const String& password) = 0;
		virtual void Close(Database* database) = 0;
		virtual MetaData* Execute(Database* database, const String& statementTemplate, MetaData* postData) = 0;
	};
}

#endif // __IDATABASE_H__
