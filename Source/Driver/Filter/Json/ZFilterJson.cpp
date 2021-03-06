#include "ZFilterJson.h"
#define JSON_HAS_INT64
#include "Core/json.h"
#include "../../../Core/Common/ZDynamicObject.h"

using namespace PaintsNow;
using namespace Json;

class FilterJsonImpl : public IStreamBase {
public:
	FilterJsonImpl(IStreamBase& streamBase);

	virtual void Flush();
	virtual bool Read(void* p, size_t& len);
	virtual bool Write(const void* p, size_t& len);
	virtual bool Transfer(IStreamBase& stream, size_t& len);
	virtual bool WriteDummy(size_t& len);
	virtual bool Seek(IStreamBase::SEEK_OPTION option, long offset);

	// object writing/reading routine
	virtual bool Write(IReflectObject& s, void* ptr, size_t length);
	virtual bool Read(IReflectObject& s, void* ptr, size_t length);

protected:
	IStreamBase& stream;
};

void FilterJsonImpl::Flush() {
	stream.Flush();
}

bool FilterJsonImpl::Read(void* p, size_t& len) {
	assert(false);
	return stream.Read(p, len);
}

bool FilterJsonImpl::Write(const void* p, size_t& len) {
	assert(false);
	return stream.Write(p, len);
}

bool FilterJsonImpl::Transfer(IStreamBase& s, size_t& len) {
	assert(false);
	return stream.Transfer(s, len);
}

bool FilterJsonImpl::Seek(SEEK_OPTION option, long offset) {
	assert(false);
	return stream.Seek(option, offset);
}

template <bool read>
class Exchanger : public IReflect {
public:
	Exchanger(Json::Value& v) : IReflect(true, false), root(v) {}
	// IReflect
	void OnValue(IReflectObject& s, Unique typeID, Json::Value& v, void* base) {
		static Unique strType = UniqueType<String>::Get();
		static Unique floatType = UniqueType<float>::Get();
		static Unique doubleType = UniqueType<double>::Get();
		static Unique int32Type = UniqueType<int32_t>::Get();
		static Unique uint32Type = UniqueType<uint32_t>::Get();
		static Unique int64Type = UniqueType<int64_t>::Get();
		static Unique uint64Type = UniqueType<uint64_t>::Get();
		static Unique boolType = UniqueType<bool>::Get();

		if (!s.IsBaseObject()) {
			if (s.IsIterator()) {
				IIterator& it = static_cast<IIterator&>(s);
				if (read) {
					if (v.isArray()) {
						it.Initialize(v.size());
					}
				} else {
					v = Json::Value(Json::arrayValue);
				}

				if (v.isArray()) {
					size_t index = 0;
					Unique id = it.GetPrototypeUnique();
					IReflectObject& prototype = const_cast<IReflectObject&>(it.GetPrototype());
					if (prototype.IsBaseObject()) {
						while (it.Next()) {
							OnValue(prototype, id, v[(Json::ArrayIndex)index++], it.Get());
						}
					} else {
						while (it.Next()) {
							void* ptr = it.Get();
							OnValue(*reinterpret_cast<IReflectObject*>(ptr), id, v[(Json::ArrayIndex)index++], ptr);
						}
					}
				}
			} else {
				Exchanger sub(v);
				s(sub);
			}
		} else if (typeID == strType) {
			String& target = *reinterpret_cast<String*>(base);
			if (read) {
				target = v.isString() ? v.asString() : "";
			} else {
				v = std::string(target.c_str(), target.length());
			}
		} else if (typeID == floatType) {
			float& target = *reinterpret_cast<float*>(base);
			if (read) {
				target = v.isDouble() ? (float)v.asDouble() : 0;
			} else {
				v = target;
			}
		} else if (typeID == doubleType) {
			double& target = *reinterpret_cast<double*>(base);
			if (read) {
				target = v.isDouble() ? v.asDouble() : 0;
			} else {
				v = target;
			}
		} else if (typeID == int32Type || typeID == uint32Type) {
			int32_t& target = *reinterpret_cast<int32_t*>(base);
			if (read) {
				target = v.isInt() ? v.asInt() : 0;
			} else {
				v = target;
			}
		} else if (typeID == int64Type || typeID == uint64Type) {
			int64_t& target = *reinterpret_cast<int64_t*>(base);
			if (read) {
				target = v.isInt64() ? v.asInt64() : 0;
			} else {
				v = (Json::Value::Int64)target;
			}
		} else if (typeID == boolType) {
			bool& target = *reinterpret_cast<bool*>(base);
			if (read) {
				target = v.isBool() ? v.asBool() : false;
			} else {
				v = target;
			}
		}
	}

	virtual void ProcessProperty(IReflectObject& s, Unique typeID, const char* name, void* base, void* ptr, const MetaChainBase* meta) {
		if (!root.isMember(name)) {
			return;
		}

		Json::Value& v = root[name];
		OnValue(s, typeID, v, ptr);
	}

	virtual void ProcessMethod(Unique typeID, const char* name, const TProxy<>* p, const Param& retValue, const std::vector<Param>& params, const MetaChainBase* meta) {}

private:
	Json::Value& root;
};

bool FilterJsonImpl::Write(IReflectObject& s, void* ptr, size_t length) {
	Json::Value v;
	Exchanger<false> exchanger(v);
	s(exchanger);

#if UNIFY_STRING_LAYOUT
	stream << v.toStyledString().c_str();
#else
	stream << v.toStyledString();
#endif

	return true;
}

static void StringCreator(void* buffer) {
	new (buffer) String();
}

static void StringDeletor(void* buffer) {
	reinterpret_cast<String*>(buffer)->~String();
}

static void StringAssigner(void* dst, const void* src) {
	*reinterpret_cast<String*>(dst) = *reinterpret_cast<const String*>(src);
}

static ZDynamicInfo::MemController mcString = {
	StringCreator, StringDeletor, StringAssigner
};

static Unique ParseUniqueObject(ZDynamicObjectWrapper& wrapper, const Value& value);
static std::pair<Unique, ZDynamicInfo::MemController*> ParseUniqueValue(ZDynamicObjectWrapper& wrapper, const Value& value) {
	std::pair<Unique, ZDynamicInfo::MemController*> res;
	res.second = nullptr;

	if (value.isBool()) {
		res.first = UniqueType<bool>::Get();
	} else if (value.isInt()) {
		res.first = UniqueType<int>::Get();
	} else if (value.isDouble()) {
		res.first = UniqueType<double>::Get();
	} else if (value.isString()) {
		res.first = UniqueType<String>::Get();
		res.second = &mcString;
	} else if (value.isArray()) {
		res.first = UniqueType<ZDynamicVector>::Get();
	} else if (value.isObject()) {
		res.first = ParseUniqueObject(wrapper, value);
	}

	return res;
}

static Unique ParseUniqueObject(ZDynamicObjectWrapper& wrapper, const Value& value) {
	// enumerate attributes ...
	assert(value.isObject());
	std::vector<ZDynamicInfo::Field> fields;
	size_t count = 0;
	for (Value::const_iterator it = value.begin(); it != value.end(); ++it) {
		ZDynamicInfo::Field& field = fields[count++];

		field.name = it.name();
		std::pair<Unique, ZDynamicInfo::MemController*> info = ParseUniqueValue(wrapper, *it);
		field.type = info.first;
		field.controller = info.second;
		field.reflectable = !!(it->isObject() || it->isArray());
	}

	return wrapper.uniqueAllocator.AllocFromDescriptor("JSON", fields);
}

static void ParseDynamicObject(ZDynamicObjectWrapper& wrapper, const Value& value, ZDynamicObject* object);

template <class T, class F>
void ParseDynamicValue(ZDynamicObjectWrapper& wrapper, const Value& v, T* object, const F& field) {
	if (v.isBool()) {
		bool data = v.asBool();
		object->Set(field, &data);
	} if (v.isInt()) {
		int data = v.asInt();
		object->Set(field, &data);
	} else if (v.isDouble()) {
		double data = v.asDouble();
		object->Set(field, &data);
	} else if (v.isString()) {
		String data = v.asCString();
		object->Set(field, &data);
	} else if (v.isArray()) {
		ZDynamicVector* vec = reinterpret_cast<ZDynamicVector*>(object->Get(field));
		size_t size = v.size();
		// set by first element type
		if (size != 0) {
			const Value& f = v[0];
			std::pair<Unique, ZDynamicInfo::MemController*> res = ParseUniqueValue(wrapper, f);
			vec->Reinit(res.first, res.second, size, f.isArray() || f.isObject());
		}
	} else if (v.isObject()) {
		ParseDynamicObject(wrapper, v, reinterpret_cast<ZDynamicObject*>(object->Get(field)));
	}
}

static void ParseDynamicObject(ZDynamicObjectWrapper& wrapper, const Value& value, ZDynamicObject* object) {
	Unique unique = object->GetDynamicInfo();
	assert(unique->GetAllocator() == &wrapper.uniqueAllocator);
	const std::vector<ZDynamicInfo::Field>& fields = object->GetDynamicInfo()->fields;

	size_t i = 0;
	for (Value::const_iterator it = value.begin(); it != value.end(); ++it) {
		ParseDynamicValue(wrapper, *it, object, fields[i++]);
	}
}

bool FilterJsonImpl::Read(IReflectObject& s, void* ptr, size_t length) {
	// check dynamic object
	assert(!s.IsBaseObject());
	String str;
	if (stream >> str) {
		Reader reader;
		Value document;
#ifdef UNIFY_STRING_LAYOUT
		reader.parse(str.c_str(), str.size(), document, false);
#else
		reader.parse(str, document, false);
#endif

		if (document.isObject()) {
			if (s.GetUnique() == UniqueType<ZDynamicObjectWrapper>::Get()) {
				ZDynamicObjectWrapper& wrapper = static_cast<ZDynamicObjectWrapper&>(s);
				assert(wrapper.dynamicObject == nullptr);
				Unique unique = ParseUniqueObject(wrapper, document);
				ZDynamicObject* object = static_cast<ZDynamicObject*>(unique->Create());
				ParseDynamicObject(wrapper, document, object);
				wrapper.dynamicObject = object;
			} else {
				Exchanger<true> exchanger(document);
				s(exchanger);
			}

			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

bool FilterJsonImpl::WriteDummy(size_t& len) {
	return stream.WriteDummy(len);
}

FilterJsonImpl::FilterJsonImpl(IStreamBase& streamBase) : stream(streamBase) {}

IStreamBase* ZFilterJson::CreateFilter(IStreamBase& streamBase) {
	return new FilterJsonImpl(streamBase);
}