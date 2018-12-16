#include "IReflect.h"
#include "IStreamBase.h"
using namespace PaintsNow;


size_t IUniqueInfo::GetSize() const {
	return size;
}

String IUniqueInfo::GetSubName() const {
	String s = typeName;
	size_t t = s.rfind(':');
	if (t != String::npos) {
		return s.substr(t + 1);
	} else {
		t = s.rfind(' ');
		if (t != String::npos) {
			return s.substr(t + 1);
		} else {
			return s;
		}
	}
}

bool IUniqueInfo::IsInteger() {
	if (intState == IUniqueInfo::TBD) {
		static Unique intUniques[] = {
			UniqueType<char>::Get(),
			UniqueType<unsigned char>::Get(),
			UniqueType<short>::Get(),
			UniqueType<unsigned short>::Get(),
			UniqueType<int>::Get(),
			UniqueType<unsigned int>::Get(),
			UniqueType<long>::Get(),
			UniqueType<unsigned long>::Get(),
			UniqueType<int8_t>::Get(),
			UniqueType<uint8_t>::Get(),
			UniqueType<int16_t>::Get(),
			UniqueType<uint16_t>::Get(),
			UniqueType<int32_t>::Get(),
			UniqueType<uint32_t>::Get(),
			UniqueType<int64_t>::Get(),
			UniqueType<uint64_t>::Get(),
		};

		intState = IUniqueInfo::NOT_INTEGER;
		for (size_t i = 0; i < sizeof(intUniques) / sizeof(intUniques[0]); i++) {
			if (this == intUniques[i].info) {
				intState = IUniqueInfo::IS_INTEGER;
				break;
			}
		}
	}

	return intState == IUniqueInfo::IS_INTEGER;
}



IReflectObject* IUniqueInfo::Create() const {
	assert(creator);
	return creator();
}

IUniqueAllocator* IUniqueInfo::GetAllocator() const {
	return allocator;
}

Unique::Unique(IUniqueInfo* f) : info(f) {}

bool Unique::operator != (const Unique& unique) const {
	return !(*this == unique);
}

bool Unique::operator == (const Unique& unique) const {
	assert(info != nullptr);
	if (info == unique.info) {
		return true;
	} else if (info->allocator == unique.info->allocator) {
		return false;
	} else {
#ifdef _MSC_VER
		// compatibility
		size_t lhs = info->typeName.length();
		size_t rhs = unique.info->typeName.length();
		const char* pl = info->typeName.c_str();
		const char* pr = unique.info->typeName.c_str();
		if (lhs < rhs) {
			std::swap(lhs, rhs);
			std::swap(pl, pr);
		}

		static const char ext[] = ",class PaintsNow::Void";
		return strncmp(pl, pr, rhs - 1) == 0 && (pr[rhs - 1] == pl[rhs - 1] || (pr[rhs - 1] == '>' && strncmp(pl + rhs - 1, ext, sizeof(ext) - 1) == 0));
#else
		return info->typeName == unique.info->typeName /* && info->GetSize() == unique->GetSize() */;
#endif
	}
}

bool Unique::operator < (const Unique& unique) const {
	// assert(info->GetAllocator() == unique.info->GetAllocator());
	return info->typeName < unique.info->typeName;
	// if (res > 0 || res < 0) return res < 0;
	// else return info->GetSize() < unique->GetSize();
}

const IUniqueInfo* Unique::operator -> () const {
	return info;
}

IUniqueInfo* Unique::operator -> () {
	return info;
}

IReflectObject::IReflectObject() {}
IReflectObject::~IReflectObject() {}

void IReflectObject::ReleaseObject() {
	delete this;
}

bool IReflectObject::IsIterator() const {
	return false;
}

TObject<IReflect>& IReflectObject::operator () (IReflect& reflect) {
	assert(false); // not allowed. must override
	return *this;
}

bool IReflectObject::IsBaseObject() const {
	return true;
}


const TObject<IReflect>& IReflectObject::operator () (IReflect& reflect) const {
	const TObject<IReflect>& t = *this;
	(const_cast<TObject<IReflect>&>(t))(reflect);
	return *this;
}

IReflect::IReflect(bool a, bool m) : isReflectProperty(a), isReflectMethod(m) {}

IReflect::~IReflect() {}

bool IReflect::IsReflectProperty() const {
	return isReflectProperty;
}

bool IReflect::IsReflectMethod() const {
	return isReflectMethod;
}


class Generic : public IReflect {
public:
	Generic(const String& k, Unique u, IReflectObject& object) : IReflect(true, false), key(k), targetUnique(u), target(nullptr) {}

	virtual void ProcessProperty(IReflectObject& s, Unique typeID, const char* name, void* base, void* ptr, const MetaChainBase* meta) {
		if (typeID == targetUnique && (key.empty() || key == name)) {
			if (s.IsBaseObject()) {
				target = ptr;
			} else {
				target = &s;
			}
		}
	}

	void* GetTarget() const { return target; }
	virtual void ProcessMethod(Unique typeID, const char* name, const TProxy<>* p, const Param& retValue, const std::vector<Param>& params, const MetaChainBase* meta) {}
	

private:
	const String& key;
	Unique targetUnique;
	void* target;
};

void* IReflectObject::InspectEx(Unique unique, const String& key) {
	Generic g(key, unique, *this);
	(*this)(g);

	return g.GetTarget();
}

Inspector::Inspector(const IReflectObject& r) : IReflect(true, false) {
	(const_cast<IReflectObject&>(r))(*this);
}

void Inspector::ProcessProperty(IReflectObject& s, Unique typeID, const char* name, void* base, void* ptr, const MetaChainBase* meta) {
	entries[typeID][name] = s.IsBaseObject() ? ptr : &s;
}

void Inspector::ProcessMethod(Unique typeID, const char* name, const TProxy<>* p, const IReflect::Param& retValue, const std::vector<IReflect::Param>& params, const MetaChainBase* meta) {}

void* Inspector::Find(Unique unique, const String& filter) const {
	std::map<Unique, std::map<String, void*> >::const_iterator it = entries.find(unique);
	if (it != entries.end()) {
		const std::map<String, void*>& n = it->second;
		if (filter.empty()) {
			return n.empty() ? nullptr : n.begin()->second;
		} else {
			const std::map<String, void*>::const_iterator& p = n.find(filter);
			return p == n.end() ? nullptr : p->second;
		}
	} else {
		return nullptr;
	}
}

IUniqueAllocator* IReflectObject::GetUniqueAllocator() const {
	return GetGlobalUniqueAllocator();
}

bool IReflectObject::operator << (IStreamBase& stream) {
	return stream.Read(const_cast<IReflectObject&>(*this), GetUnique(), const_cast<IReflectObject*>(this), sizeof(*this));
}

bool IReflectObject::operator >> (IStreamBase& stream) const {
	return stream.Write(const_cast<IReflectObject&>(*this), GetUnique(), const_cast<IReflectObject*>(this), sizeof(*this));
}

/*
size_t IReflect::GetUniqueLength(Unique id) {
	return *(reinterpret_cast<size_t*>(id));
}*/

bool IReflectObjectComplex::IsBaseObject() const {
	return false;
}


TObject<IReflect>& IReflectObjectComplex::operator () (IReflect& reflect) {
	return *this; // no operations
}


// IIterator
IIterator::IIterator() {}
IIterator::~IIterator() {}

bool IIterator::IsBaseObject() const {
	return false;
}

bool IIterator::IsIterator() const {
	return true;
}

#ifdef _MSC_VER
#include <atlbase.h>
#endif

void IReflect::RegisterBuiltinTypes(bool useStdintType) {
	IReflect& reflect = *this;
	ReflectBuiltinType(String);
#ifdef _MSC_VER
	ReflectBuiltinType(IDispatch*);
	ReflectBuiltinType(IUnknown*);
	ReflectBuiltinType(SAFEARRAY);
	ReflectBuiltinType(BSTR);
	ReflectBuiltinType(VARIANT);
	ReflectBuiltinType(VARIANT_BOOL);
	ReflectBuiltinType(HMODULE);
	ReflectBuiltinType(HWND);
	ReflectBuiltinType(HGLRC);
	ReflectBuiltinType(HDC);
	ReflectBuiltinType(HGLOBAL);
	ReflectBuiltinType(HGDIOBJ);
	ReflectBuiltinType(HRESULT);
	ReflectBuiltinType(PROC);
#endif
	ReflectBuiltinType(void*);

	if (useStdintType) {
		ReflectBuiltinType(size_t);
		ReflectBuiltinType(int8_t);
		ReflectBuiltinType(uint8_t);
		ReflectBuiltinType(int16_t);
		ReflectBuiltinType(uint16_t);
		ReflectBuiltinType(int32_t);
		ReflectBuiltinType(uint32_t);
		ReflectBuiltinType(long);
		ReflectBuiltinType(unsigned long);
	} else {
		ReflectBuiltinType(bool);
		ReflectBuiltinType(signed char);
		ReflectBuiltinType(char);
		ReflectBuiltinType(unsigned char);
		ReflectBuiltinType(signed int);
		ReflectBuiltinType(int);
		ReflectBuiltinType(unsigned int);
		ReflectBuiltinType(signed short);
		ReflectBuiltinType(short);
		ReflectBuiltinType(unsigned short);
		ReflectBuiltinType(signed long);
		ReflectBuiltinType(long);
		ReflectBuiltinType(unsigned long);
	}

	ReflectBuiltinType(int64_t);
	ReflectBuiltinType(uint64_t);
	ReflectBuiltinType(float);
	ReflectBuiltinType(double);

	ReflectBuiltinType(ZInt2);
	ReflectBuiltinType(ZInt3);
	ReflectBuiltinType(ZInt4);
	ReflectBuiltinType(ZFloat2);
	ReflectBuiltinType(ZFloat2Pair);
	ReflectBuiltinType(ZFloat3);
	ReflectBuiltinType(ZFloat3Pair);
	ReflectBuiltinType(ZFloat4);
	ReflectBuiltinType(ZFloat4Pair);
	ReflectBuiltinType(ZDouble2);
	ReflectBuiltinType(ZDouble2Pair);
	ReflectBuiltinType(ZDouble3);
	ReflectBuiltinType(ZDouble3Pair);
	ReflectBuiltinType(ZDouble4);
	ReflectBuiltinType(ZDouble4Pair);
	ReflectBuiltinType(ZMatrixFloat3x3);
	ReflectBuiltinType(ZMatrixFloat4x4);
}

void IReflect::Class(IReflectObject& host, Unique id, const char* name, const char*  path, const MetaChainBase* meta) {
	while (meta != nullptr) {
		MetaNodeBase* node = const_cast<MetaNodeBase*>(meta->GetNode());
		assert(node != nullptr);
		assert(!node->IsBaseObject());
		(*node)(*this);
		meta = meta->GetNext();
	}
}


class ReflectQueryType : public IReflect {
public:
	ReflectQueryType(IReflectObject& object);
	operator Unique () const {
		return type;
	}

public:
	virtual void ProcessProperty(IReflectObject& s, Unique typeID, const char* name, void* base, void* ptr, const MetaChainBase* meta);
	virtual void ProcessMethod(Unique typeID, const char* name, const TProxy<>* p, const Param& retValue, const std::vector<Param>& params, const MetaChainBase* meta);
	virtual void Class(IReflectObject& host, Unique id, const char* name, const char* path, const MetaChainBase* meta);

private:
	Unique type;
};

// ReflectQueryType

ReflectQueryType::ReflectQueryType(IReflectObject& object) : IReflect(false, false) {
	object(*this);
}

void ReflectQueryType::ProcessProperty(IReflectObject& s, Unique typeID, const char* name, void* base, void* ptr, const MetaChainBase* meta) {}

void ReflectQueryType::ProcessMethod(Unique typeID, const char* name, const TProxy<>* p, const IReflect::Param& retValue, const std::vector<IReflect::Param>& params, const MetaChainBase* meta) {}

void ReflectQueryType::Class(IReflectObject& host, Unique id, const char* name, const char* path, const MetaChainBase* meta) {
	type = id;
}

Unique IReflectObject::GetUnique() const {
	ReflectQueryType query(const_cast<IReflectObject&>(*this));
	assert(((Unique)query).info != nullptr); // no class provided.
	return query;
}


IUniqueAllocator::~IUniqueAllocator() {}

UniqueAllocator::UniqueAllocator(IThread& t) : thread(t) {
	lock = thread.NewLock();
}

UniqueAllocator::~UniqueAllocator() {
	thread.DeleteLock(lock);
}

IUniqueInfo* UniqueAllocator::Get(const String& key) {
	thread.DoLock(lock);
	std::map<String, IUniqueInfo>::iterator it = mapType.find(key);
	IUniqueInfo* info = it == mapType.end() ? nullptr : &(*it).second;
	thread.UnLock(lock);

	return info;
}

IUniqueInfo* UniqueAllocator::Alloc(const String& key, size_t size) {
	thread.DoLock(lock);
	std::map<String, IUniqueInfo>::iterator it = mapType.find(key);
	IUniqueInfo* ret = nullptr;
	if (it == mapType.end()) {
		IUniqueInfo info;
		info.typeName = key;
		info.size = size;
		info.allocator = this;
		info.intState = IUniqueInfo::TBD;
		ret = &(mapType[key] = info);
	} else {
		ret = &(mapType[key]);
	}

	thread.UnLock(lock);
	return ret;
}


IThread& UniqueAllocator::GetThreadApi() {
	return thread;
}

namespace PaintsNow {
	static IUniqueAllocator* theIUniqueAllocator = nullptr;
	IUniqueAllocator* GetGlobalUniqueAllocator() {
		assert(theIUniqueAllocator != nullptr);
		return theIUniqueAllocator;
	}

	void SetGlobalUniqueAllocator(IUniqueAllocator* alloc) {
		assert(theIUniqueAllocator == nullptr || theIUniqueAllocator == alloc);
		theIUniqueAllocator = alloc;
	}

	MetaNote Note("");
	MetaConstructable Constructable;
	MetaRuntime Runtime;
}

MetaNote::MetaNote(const String& v) : value(v) {}

MetaNote MetaNote::operator = (const String& value) {
	return MetaNote(value);
}

TObject<IReflect>& MetaNote::operator () (IReflect& reflect) {
	ReflectClass(MetaNote);
	if (reflect.IsReflectProperty()) {
		ReflectProperty(value);
	}

	return *this;
}

/*
#include <Windows.h>
DWORD syncObjectCount = 0;*/

ISyncObject::~ISyncObject() {
	threadApi.DeleteLock(lock);
	// ::InterlockedDecrement(&syncObjectCount);
}

ISyncObject::ISyncObject(IThread& thread) : threadApi(thread) {
	lock = threadApi.NewLock();
	// ::InterlockedIncrement(&syncObjectCount);
}

void ISyncObject::DoLock() {
	threadApi.DoLock(lock);
}

void ISyncObject::UnLock() {
	threadApi.UnLock(lock);
}

bool ISyncObject::TryLock() {
	return threadApi.TryLock(lock);
}

size_t ISyncObject::GetLockCount() const {
	return threadApi.GetLockCount(lock);
}

IThread& ISyncObject::GetThreadApi() {
	return threadApi;
}

TObject<IReflect>& MetaRuntime::operator () (IReflect& reflect) {
	ReflectClass(MetaRuntime);
	return *this;
}