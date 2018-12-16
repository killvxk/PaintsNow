// ZMeta.h
// By PaintDream (paintdream@paintdream.com)
// 2014-12-22
//

#ifndef __ZMETA_H__
#define __ZMETA_H__

#include "../PaintsNow.h"
#include "ZType.h"
#include "../Interface/IDevice.h"
#include <string>
#include <map>

namespace PaintsNow {
	class ZMeta : public IDevice {
	public:
		virtual ~ZMeta() {}
		virtual void ClearMetaData(const String& key);
		virtual void SetMetaData(const String& key, const String& value);
		virtual const String& GetMetaData(const String& key) const;

	private:
		std::map<String, String> metaData;
	};
}

#endif // __ZMETA_H__