#include "Honey.h"

using namespace PaintsNow;
using namespace NsRemembery;

namespace PaintsNow {
	namespace NsRemembery {
		IScript::Request& operator << (IScript::Request& request, HoneyData& honey) {
			return request;
		}

		IScript::Request& operator >> (IScript::Request& request, HoneyData& honey) {
			honey.Attach(&request);
			return request;
		}
	}
}

Honey::Honey(IDatabase::MetaData* data) : metaData(data) {
	assert(data != nullptr);
	(*data)(resolver); // resolve schema
}

bool Honey::Step() {
	return metaData->Next();
}

void Honey::WriteLine(IScript::Request& request) {
	request << begintable;
	for (size_t j = 0; j < resolver.setters.size(); j++) {
		std::pair<SchemaResolver::Set, size_t>& setter = resolver.setters[j];
		setter.first(request, const_cast<char*>(reinterpret_cast<const char*>(metaData)) + setter.second);
	}
	request << endtable;
}

HoneyData::HoneyData() : index(0), count(0), request(nullptr) {

}

HoneyData::~HoneyData() {
	if (request != nullptr) {
		request->DoLock();
		request->Dereference(tableRef);
		request->UnLock();
	}

	if (dynamicObject != nullptr) {
		dynamicObject->ReleaseObject();
	}
}

bool HoneyData::IsLayoutLinear() const {
	return false;
}

void* HoneyData::GetHost() const {
	return nullptr;
}

Unique HoneyData::GetPrototypeUnique() const {
	assert(false);
	return Unique();
}

IIterator* HoneyData::New() const {
	assert(false);
	return nullptr;
}


SchemaResolver::SchemaResolver() : IReflect(true, false) {}
void SchemaResolver::ProcessProperty(IReflectObject& s, Unique typeID, const char* name, void* base, void* ptr, const MetaChainBase* meta) {
	size_t offset = (const char*)ptr - (const char*)base;
	if (typeID == UniqueType<int>::Get()) {
		setters.push_back(std::make_pair(&SchemaResolver::SetValueInt, offset));
	} else if (typeID == UniqueType<String>::Get()) {
		setters.push_back(std::make_pair(&SchemaResolver::SetValueString, offset));
	} else if (typeID == UniqueType<const char*>::Get()) {
		setters.push_back(std::make_pair(&SchemaResolver::SetValueText, offset));
	} else if (typeID == UniqueType<double>::Get()) {
		setters.push_back(std::make_pair(&SchemaResolver::SetValueFloat, offset));
	} else {
		assert(false);
	}
}

void SchemaResolver::SetValueString(IScript::Request& request, char* base) {
	request << ((String*)base)->c_str();
}

void SchemaResolver::SetValueText(IScript::Request& request, char* base) {
	request << *(const char**)base;
}

void SchemaResolver::SetValueFloat(IScript::Request& request, char* base) {
	request << *(double*)base;
}

void SchemaResolver::SetValueInt(IScript::Request& request, char* base) {
	request << *(int*)base;
}

void SchemaResolver::ProcessMethod(Unique typeID, const char* name, const TProxy<>* p, const Param& retValue, const std::vector<Param>& params, const MetaChainBase* meta) {}

static void StringCreator(void* buffer) {
	new (buffer) String();
}

static void StringDeletor(void* buffer) {
	reinterpret_cast<String*>(buffer)->~String();
}

static void StringAssigner(void* dst, const void* src) {
	*reinterpret_cast<String*>(dst) = *reinterpret_cast<const String*>(src);
}


void HoneyData::Attach(void* base) {
	assert(request == nullptr);
	this->request = reinterpret_cast<IScript::Request*>(base);
	IScript::Request& request = *this->request;
	IScript::Request::Ref tableRef;
	// fetch first line
	IScript::Request::TableStart ts;
	request >> ts;

	// get schema
	if (ts.count != 0) {
		request >> begintable;
		std::vector<IScript::Request::Key> keys = request.Enumerate();
		std::vector<ZDynamicInfo::Field> fields(keys.size());

		for (size_t i = 0; i < keys.size(); i++) {
			const String& key = keys[i].GetKey();
			ZDynamicInfo::Field& field = fields[i];
			field.name = key;
			static ZDynamicInfo::MemController mc = {
				StringCreator, StringDeletor, StringAssigner
			};


			switch (keys[i].GetType()) {
				case IScript::Request::INTEGER:
					field.type = UniqueType<int>::Get();
					sets.push_back(&HoneyData::SetInteger);
					break;
				case IScript::Request::NUMBER:
					field.type = UniqueType<double>::Get();
					sets.push_back(&HoneyData::SetFloat);
					break;
				case IScript::Request::STRING:
				default:
					field.type = UniqueType<String>::Get();
					field.controller = &mc;
					sets.push_back(&HoneyData::SetString);
					break;
			}
		}
		request >> endtable;

		Unique unique = uniqueAllocator.AllocFromDescriptor("HoneyDataInstance", fields);
		dynamicObject = static_cast<ZDynamicObject*>(unique->Create());
	}

	request >> endtable;
	count = ts.count;
	index = 0;
}

void HoneyData::SetFloat(size_t i) {
	double db;
	*request >> db;
	dynamicObject->Set(dynamicObject->GetDynamicInfo()->fields[i], &db);
}

void HoneyData::SetString(size_t i) {
	String str;
	*request >> str;
	dynamicObject->Set(dynamicObject->GetDynamicInfo()->fields[i], &str);
}

void HoneyData::SetInteger(size_t i) {
	int v;
	*request >> v;
	dynamicObject->Set(dynamicObject->GetDynamicInfo()->fields[i], &v);
}

void HoneyData::Initialize(size_t count) {
	assert(false);
}

size_t HoneyData::GetTotalCount() const {
	return count;
}

void* HoneyData::Get() {
	return dynamicObject;
}

const IReflectObject& HoneyData::GetPrototype() const {
	return *dynamicObject;
}

bool HoneyData::IsLayoutPinned() const {
	return false;
}

bool HoneyData::Next() {
	if (index < count) {
		*request >> begintable;
		for (size_t n = 0; n < sets.size(); n++) {
			(this->*sets[n])(n);
		}
		*request >> endtable;
		index++;
		return true;
	} else {
		return false;
	}
}
