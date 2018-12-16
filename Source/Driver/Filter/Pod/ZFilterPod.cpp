#include "ZFilterPod.h"
#include "../../../Core/Template/TObject.h"
#include <string>
#include <stack>
#include <cstdio>
#include <cassert>
#include <sstream>

extern "C" {
#include "Core/Pod.h"
}

using namespace PaintsNow;

class FilterPodImpl : public IStreamBase, public IReflect {
public:
	FilterPodImpl(IStreamBase& streamBase);
	virtual ~FilterPodImpl();

	virtual bool Read(void* p, size_t& len);
	virtual bool Write(const void* p, size_t& len);
	virtual bool Transfer(IStreamBase& stream, size_t& len);
	virtual bool WriteDummy(size_t& len);
	virtual void Flush();
	virtual bool Seek(IStreamBase::SEEK_OPTION option, long offset);

	// object writing/reading routine
	virtual bool Write(IReflectObject& a, Unique type, void* ptr, size_t length);
	virtual bool Read(IReflectObject& a, Unique type, void* ptr, size_t length);

	IStreamBase& GetStream() const;

public:
	// IReflect
	virtual void ProcessProperty(IReflectObject& s, Unique typeID, const char* name, void* base, void* ptr, const MetaChainBase* meta);
	virtual void ProcessMethod(Unique typeID, const char* name, const TProxy<>* p, const Param& retValue, const std::vector<Param>& params, const MetaChainBase* meta);
	virtual void Class(IReflectObject& host, Unique id, const char* name, const char* path, const MetaChainBase* meta);

	IReflectObjectComplex* CreateObjectFromMeta(const String& metaData);
	String GetMetaFromObject(IReflectObjectComplex& object);
	void OnSubClass(IReflectObject& s, const char* name);

	struct Type {
		Type();
		Pod* pod;
		IIterator* iterator;
	};

	const Type& GetRegisteredType(IReflectObject& s, Unique unique);
	void Register(IReflectObject& s);
	void ReadTree();
	void WriteTree();
	void SyncTree();

	void MovePointer(int64_t offset);
	int64_t GetPointer() const;

private:
	Type& NewType(Unique id, const char* name, bool& created);
	std::map<Unique, Type> mapTypes;
	Type* current;
	PodRoot* root;
	PodRoot* readRoot;
	int64_t offset;

protected:
	IStreamBase& stream;
};

FilterPodImpl::Type::Type() : pod(nullptr), iterator(nullptr) {}

// Pod adapters

static void* LocateHandlerString(void* locateContext, void** pointer, PodSize* count, void* base, char* metaData, uint32_t* metaLength, const Pod** dynamicType, void* context) {
	String& str = *reinterpret_cast<String*>(base);
	if (*count != (PodSize)-1) { // write
		str.resize((size_t)*count);
	} else {
		*count = (PodSize)(str.size());
	}

	return const_cast<char*>(str.data());
}

static void* LocateHandlerPointer(void* locateContext, void** pointer, PodSize* count, void* base, char* metaData, uint32_t* metaLength, const Pod** dynamicType, void* context) {
	assert(base != nullptr);
	assert(metaLength != nullptr);
	assert(dynamicType != nullptr);
	assert(context != nullptr);

	FilterPodImpl* filterPod = reinterpret_cast<FilterPodImpl*>(context);
	IReflectObjectComplex*& object = *reinterpret_cast<IReflectObjectComplex**>(base);
	bool read = *count != -1;
	*count = 1;
	*pointer = nullptr;

	if (read) {
		object = filterPod->CreateObjectFromMeta(String(metaData, *metaLength));
	} else if (object != nullptr) {
		String metaString = filterPod->GetMetaFromObject(*object);
		assert(!metaString.empty());
		if (metaString.empty()) return nullptr; // not supported
		assert(metaString.length() < MAX_META_LENGTH);
		memcpy(metaData, metaString.c_str(), metaString.length());
		*metaLength = (uint32_t)metaString.length();
	} else {
		// empty pointer
		*metaLength = 0;
	}

	// make object type registered
	if (object != nullptr) {
		*dynamicType = filterPod->GetRegisteredType(*object, object->GetUnique()).pod;
	}

	return object;
}

static void* DirectLocateHandler(void* locateContext, void** pointer, PodSize* count, void* base, char* metaData, uint32_t* metaLength, const Pod** dynamicType, void* context) {
	*count = 1;
	*pointer = nullptr;

	return base;
}

static void* StreamLocateHandler(void* locateContext, void** pointer, PodSize* count, void* base, char* metaData, uint32_t* metaLength, const Pod** dynamicType, void* context) {
	FilterPodImpl::Type* type = (FilterPodImpl::Type*)locateContext;
	IIterator* it = type->iterator->New();
	it->Attach(base);
	if (*count != -1) {
		// read
		it->Initialize((size_t)*count);
	} else {
		// write
		*count = (PodSize)it->GetTotalCount();
	}

	// check linearness
	if (it->IsLayoutLinear() && it->GetPrototype().IsBaseObject()) {
		// can copy directly
		void* obj = nullptr;
		if (it->Next()) {
			// get first element...
			obj = it->Get();
			it->ReleaseObject();
		} 

		return obj;
	} else {
		*pointer = (void*)it;
		return nullptr; // before first one, returns nullptr.
	}
}

static void* StreamIterateHandler(void* locateContext, void** pointer, void* context) {
	IIterator* iterator = *(IIterator**)pointer;
	assert(iterator != nullptr);

	if (iterator->Next()) {
		return iterator->Get();
	} else {
		iterator->ReleaseObject();
		return nullptr;
	}
}

static int StreamWriterFile(const PodStream* stream, const void* base, PodSize size, void* context) {
	FilterPodImpl* p = (FilterPodImpl*)context;
	IStreamBase& s = p->GetStream();

	size_t len = (size_t)size;
	int ret = s.WriteBlock(base, len) ? POD_SUCCESS : POD_ERROR_STREAM;
	p->MovePointer(len);

	return ret;
}

static int StreamReaderFile(const PodStream* stream, void* base, PodSize size, void* context) {
	FilterPodImpl* p = (FilterPodImpl*)context;
	IStreamBase& s = p->GetStream();
	size_t len = (size_t)size;
	int ret = s.ReadBlock(base, len) ? POD_SUCCESS : POD_ERROR_STREAM;

	p->MovePointer(len);
	assert(ret == POD_SUCCESS);
	return ret;
}

static PodSize StreamLocaterFile(const PodStream* stream, void* context) {
	FilterPodImpl* p = (FilterPodImpl*)context;
	// IStreamBase* s = p->GetStream();

	return (PodSize)p->GetPointer();
}

static int StreamSeekerFile(const PodStream* stream, uint8_t direct, PodSize step, void* context) {
	FilterPodImpl* p = (FilterPodImpl*)context;
	IStreamBase& s = p->GetStream();

	long t = direct ? (long)step : (long)-(int64_t)step;
	p->MovePointer(t);
	return s.Seek(IStreamBase::CUR, t) ? POD_SUCCESS : POD_ERROR_STREAM;
}

static PodStream streamAdapter = { StreamWriterFile, StreamReaderFile, StreamSeekerFile, StreamLocaterFile };

// currently we do not process methods, so set the 2nd option should be set to false
FilterPodImpl::FilterPodImpl(IStreamBase& streamBase) : stream(streamBase), IReflect(true, false), current(nullptr), offset(0) {
	PodInit();
	root = PodCreateRoot();
	readRoot = PodCreateRoot();
}

void FilterPodImpl::MovePointer(int64_t delta) {
	offset += delta;
}

IStreamBase& FilterPodImpl::GetStream() const {
	return stream;
}

int64_t FilterPodImpl::GetPointer() const {
	return offset;
}

FilterPodImpl::~FilterPodImpl() {
	/*
	for (std::map<Unique, Type>::iterator it = mapTypes.begin(); it != mapTypes.end(); ++it) {
		PodDelete((*it).second.pod);
	}*/

	PodDeleteRoot(readRoot);
	PodDeleteRoot(root);
	PodExit();
}

void FilterPodImpl::Flush() {
	stream.Flush();
}

bool FilterPodImpl::Read(void* p, size_t& len) {
	assert(false);
	return stream.Read(p, len);
}

bool FilterPodImpl::Write(const void* p, size_t& len) {
	assert(false);
	return stream.WriteBlock(p, len);
}

bool FilterPodImpl::Transfer(IStreamBase& s, size_t& len) {
	assert(false);
	return stream.Transfer(s, len);
}

bool FilterPodImpl::Seek(SEEK_OPTION option, long offset) {
	assert(false);
	return stream.Seek(option, offset);
}

void FilterPodImpl::WriteTree() {
	PodWriteSpecRoot(root, &streamAdapter, this);
}

void FilterPodImpl::ReadTree() {
	PodParseSpecRoot(readRoot, &streamAdapter, this);
}

void FilterPodImpl::SyncTree() {
	PodSyncRoot(root, readRoot);
}

const FilterPodImpl::Type& FilterPodImpl::GetRegisteredType(IReflectObject& s, Unique unique) {
	std::map<Unique, Type>::iterator p = mapTypes.find(unique);
	if (p == mapTypes.end()) {
		// Not registered
		Register(s);

		return mapTypes[unique];
	} else {
		assert(p->second.pod != nullptr);
		return p->second;
	}
}

bool FilterPodImpl::Write(IReflectObject& s, Unique unique, void* ptr, size_t length) {
	const Type& type = GetRegisteredType(s, unique);
	PodWriteData(type.pod, &streamAdapter, ptr, this);
	return true;
}

bool FilterPodImpl::WriteDummy(size_t& len) {
	return stream.WriteDummy(len);
}

bool FilterPodImpl::Read(IReflectObject& s, Unique unique, void* ptr, size_t length) {
	GetRegisteredType(s, unique);
	PodParseData(root, &streamAdapter, ptr, this);
	return true;
}

void FilterPodImpl::OnSubClass(IReflectObject& s, const char* name) {
	Type* save = current;
	current = nullptr; // if some class do not use ReflectClass before other Reflect macros, use nullptr to assert them
	s(*this);
	// now current is sub type ...
	assert(current != nullptr);
	current = save;
}

IReflectObjectComplex* FilterPodImpl::CreateObjectFromMeta(const String& metaData) {
	IUniqueInfo* info = GetGlobalUniqueAllocator()->Get(metaData.c_str());
	return info == nullptr ? nullptr : static_cast<IReflectObjectComplex*>(info->Create());
}

String FilterPodImpl::GetMetaFromObject(IReflectObjectComplex& object) {
	return object.GetUnique().info->typeName;
}

void FilterPodImpl::ProcessProperty(IReflectObject& s, Unique typeID, const char* name, void* base, void* ptr, const MetaChainBase* meta) {
	if (current == nullptr)
		return;

	static Unique metaRuntime = UniqueType<MetaRuntime>::Get();
	// check serialization chain
	while (meta != nullptr) {
		// runtime data can not be serialized
		if (meta->GetNode()->GetUnique() == metaRuntime) {
			return;
		}

		meta = meta->GetNext();
	}

	static const Unique uniqueString = UniqueType<String>::Get();
	static Unique uniquePointer = UniqueType<IReflectObjectComplex*>::Get();

	assert((const char*)ptr - (const char*)base != 0); // must have space for virtual table?
	// Process Attrib
	if (s.IsBaseObject()) {
		bool isString = typeID == uniqueString;
		bool isPointer = typeID == uniquePointer;
		if (isString || isPointer) {
			bool created = false;
			Type& type = NewType(typeID, typeID.info->typeName.c_str(), created);
			if (created) {
				PodSetHandler(type.pod, isString ? LocateHandlerString : LocateHandlerPointer, nullptr, isString ? (void*)&type : (void*)this);
			}

			PodInsert(current->pod, (const uint8_t*)name, (PodSize)((const char*)ptr - (const char*)base), 1, 0, isPointer ? 1 : 0, type.pod);
		} else {
			PodInsert(current->pod, (const uint8_t*)name, (PodSize)((const char*)ptr - (const char*)base), 0, typeID->IsInteger(), 0, POD_TYPE(typeID->size));
		}
	} else if (s.IsIterator()) {
		IIterator& iter = static_cast<IIterator&>(s);
		std::ostringstream os;
		os << name << "[" << (size_t)typeID.info << "]\0";
		bool created = false;
		Type& type = NewType(typeID, os.str().c_str(), created);
		if (created) {
			type.iterator = (static_cast<IIterator&>(s)).New();
			const IReflectObject& prototype = iter.GetPrototype();
			Unique subUnique = iter.GetPrototypeUnique();
			if (prototype.IsBaseObject()) {
				if (subUnique == uniquePointer) {
					Type& subType = NewType(subUnique, subUnique.info->typeName.c_str(), created);
					if (created) {
						PodSetHandler(subType.pod, LocateHandlerPointer, nullptr, (void*)this);
					}

					PodInsert(type.pod, (const uint8_t*)"[]", 0, 0, 0, 1, subType.pod);
				} else {
					PodInsert(type.pod, (const uint8_t*)"[]", 0, 0, subUnique->IsInteger(), 0, POD_TYPE(subUnique->size));
				}
			} else {
				Type& subType = NewType(subUnique, subUnique.info->typeName.c_str(), created);
				PodInsert(type.pod, (const uint8_t*)"[]", 0, 0, 0, 0, subType.pod);
			}
			PodSetHandler(type.pod, StreamLocateHandler, StreamIterateHandler, &type);
		}

		assert(type.pod->locateHandler == StreamLocateHandler);
		PodInsert(current->pod, (const uint8_t*)name, (PodSize)((const char*)iter.GetHost() - (const char*)base), 1, (static_cast<IIterator&>(s)).GetPrototypeUnique()->IsInteger(), 0, type.pod);
	} else {
		// cascade
		bool created = false;
		Type& type = NewType(typeID, typeID.info->typeName.c_str(), created);
		if (created) {
			OnSubClass(s, name);
		}

		if (type.pod->subList != nullptr) {
			PodSetHandler(type.pod, DirectLocateHandler, nullptr, &type);
			PodInsert(current->pod, (const uint8_t*)name, (PodSize)((const char*)ptr - (const char*)base), 0, 0, 0, type.pod);
		}
	}
}

void FilterPodImpl::Register(IReflectObject& object) {
	object(*this);
}

void FilterPodImpl::ProcessMethod(Unique typeID, const char* name, const TProxy<>* p, const Param& retValue, const std::vector<Param>& params, const MetaChainBase* meta) {
	// Not handled now
	assert(false);
}

FilterPodImpl::Type& FilterPodImpl::NewType(Unique id, const char* name, bool& created) {
	// if already registered, skip
	std::map<Unique, Type>::iterator p = mapTypes.find(id);
	if (p == mapTypes.end()) {
		// Create a new pod type
		Pod* pod = PodCreate(root, (const uint8_t*)name);
		// add entry
		Type& type = mapTypes[id];
		type.pod = pod;
		created = true;
		return type;
	} else {
		created = false;
		return p->second;
	}
}

void FilterPodImpl::Class(IReflectObject& host, Unique id, const char* name, const char* path, const MetaChainBase* meta) {
	bool created = false;
	current = &NewType(id, id.info->typeName.c_str(), created);
	if (created) {
		// write parent fields
		while (meta != nullptr) {
			// Record all metas
			MetaNodeBase* node = const_cast<MetaNodeBase*>(meta->GetNode());

			bool created = false;
			Unique u = node->GetUnique();
			const char* subName = u.info->typeName.c_str();
			if (mapTypes.find(u) == mapTypes.end()) {
				OnSubClass(*node, subName);
			}

			Type& type = NewType(u, subName, created);
			assert(!created); // created already in OnSubClass

			static Unique uniquePointer = UniqueType<IReflectObjectComplex*>::Get();
			bool isPointer = u == uniquePointer;
			assert(!isPointer);
			// PodSetHandler(type.pod, isPointer ? LocateHandlerPointer : DirectLocateHandler, nullptr, isPointer ? (void*)this : (void*)&type);
			PodSetHandler(type.pod, DirectLocateHandler, nullptr, &type);
			if (type.pod->subList != nullptr) {
				PodInsert(current->pod, (const uint8_t*)subName, 0, 0, 0, 0, type.pod);
			}
			meta = meta->GetNext();
		}
	}
}

IStreamBase* ZFilterPod::CreateFilter(IStreamBase& stream) {
	return new FilterPodImpl(stream);
}