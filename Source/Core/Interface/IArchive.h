// IArchive.h -- Archive format supporting
// By PaintDream (paintdream@paintdream.com)
// 2014-12-1
//

#ifndef __IARCHIVE_H__
#define __IARCHIVE_H__


#include "../PaintsNow.h"
#include "../Template/TProxy.h"
#include "../Common/ZMeta.h"
#include <string>

namespace PaintsNow {
	class IStreamBase;
	class IArchive : public ZMeta {
	public:
		virtual ~IArchive();
		virtual IStreamBase* Open(const String& uri, bool write, size_t& length, uint64_t* lastModifiedTime = nullptr) = 0;
		virtual void Query(const String& uri, const TWrapper<void, bool, const String&>& wrapper) const = 0;
		virtual bool IsReadOnly() const = 0;
		virtual bool Delete(const String& uri) = 0;
	};
}



#endif // __IARCHIVE_H__
