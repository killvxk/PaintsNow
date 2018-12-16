#include "ZMeta.h"

using namespace PaintsNow;

void ZMeta::SetMetaData(const String& key, const String& value) {
	metaData[key] = value;
}

void ZMeta::ClearMetaData(const String& key) {
	metaData.erase(key);
}

const String& ZMeta::GetMetaData(const String& key) const {
	static const String dummy = "";
	std::map<String, String>::const_iterator it = metaData.find(key);
	if (it != metaData.end()) {
		return it->second;
	} else {
		return dummy;
	}
}