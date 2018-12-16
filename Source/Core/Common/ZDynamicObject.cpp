#include "ZDynamicObject.h"
#include <sstream>
using namespace PaintsNow;

ZDynamicUniqueAllocator::ZDynamicUniqueAllocator() {}

ZDynamicInfo::Field::Field() : controller(nullptr) {
	reflectable = 0;
	offset = 0;
}

IReflectObject* ZDynamicInfo::Create() const {
	assert(Alignment(sizeof(ZDynamicObject)) >= sizeof(size_t));
	char* buffer = new char[sizeof(ZDynamicObject) + size];
	ZDynamicObject* proxy = new (buffer) ZDynamicObject(const_cast<ZDynamicInfo*>(this));
	return proxy;
}

const ZDynamicInfo::Field* ZDynamicInfo::operator [] (const String& key) const {
	std::map<String, uint32_t>::const_iterator it = mapNameToField.find(key);
	return it == mapNameToField.end() ? (ZDynamicInfo::Field*)nullptr : &fields[it->second];
}

ZDynamicObject::ZDynamicObject(ZDynamicInfo* info) : dynamicInfo(info) {
	std::vector<ZDynamicInfo::Field>& fields = dynamicInfo->fields;
	char* base = (char*)this + sizeof(*this);
	for (size_t i = 0; i < fields.size(); i++) {
		ZDynamicInfo::Field& field = fields[i];
		char* ptr = base + field.offset;
		if (field.type->GetAllocator() == dynamicInfo->GetAllocator()) {
			// dynamic-created class?
			assert(field.controller == nullptr);
			new (ptr) ZDynamicObject(dynamicInfo);
		} else if (field.controller != nullptr) {
			field.controller->Creator(ptr);
		}
	}
}

ZDynamicObject::~ZDynamicObject() {
	std::vector<ZDynamicInfo::Field>& fields = dynamicInfo->fields;
	char* base = (char*)this + sizeof(*this);
	for (size_t i = 0; i < fields.size(); i++) {
		ZDynamicInfo::Field& field = fields[i];
		char* ptr = base + field.offset;
		if (field.type->GetAllocator() == dynamicInfo->GetAllocator()) {
			assert(field.controller == nullptr);
			ZDynamicObject* proxy = reinterpret_cast<ZDynamicObject*>(ptr);
			proxy->~ZDynamicObject();
		} else if (field.controller != nullptr) {
			field.controller->Deletor(ptr);
		}
	}
}

void ZDynamicObject::ReleaseObject() {
	// call destructor manually
	this->~ZDynamicObject();
	delete[] (char*)this;
}

ZDynamicVector::Iterator::Iterator(ZDynamicVector* vec) : base(vec), i(0) {}

void ZDynamicVector::Iterator::Initialize(size_t c) {
	i = 0;
	base->Reinit(base->unique, base->memController, c, base->reflectable);
}

size_t ZDynamicVector::Iterator::GetTotalCount() const {
	return base->count;
}

Unique ZDynamicVector::Iterator::GetPrototypeUnique() const {
	return base->unique;
}

static IReflectObject dummyObject((int)0);

const IReflectObject& ZDynamicVector::Iterator::GetPrototype() const {
	return base->count == 0 || !base->reflectable ? dummyObject : *reinterpret_cast<const IReflectObject*>(base->buffer);
}

void* ZDynamicVector::Iterator::Get() {
	return (char*)base->buffer + (i - 1) * base->unique->GetSize();
}

bool ZDynamicVector::Iterator::Next() {
	if (i >= base->count) {
		return false;
	}

	i++;
	return true;
}

IIterator* ZDynamicVector::Iterator::New() const {
	return new ZDynamicVector::Iterator(base);
}

void ZDynamicVector::Iterator::Attach(void* p) {
	base = reinterpret_cast<ZDynamicVector*>(p);
	i = 0;
}

bool ZDynamicVector::Iterator::IsLayoutLinear() const {
	return true;
}

bool ZDynamicVector::Iterator::IsLayoutPinned() const {
	return false;
}

void* ZDynamicVector::Iterator::GetHost() const {
	return base;
}


TObject<IReflect>& ZDynamicObject::operator () (IReflect& reflect) {
	reflect.Class(*this, dynamicInfo, dynamicInfo->typeName.c_str(), "PaintsNow", nullptr);

	if (reflect.IsReflectProperty()) {
		char* base = (char*)this + sizeof(*this);
		std::vector<ZDynamicInfo::Field>& fields = dynamicInfo->fields;
		for (size_t i = 0; i < fields.size(); i++) {
			ZDynamicInfo::Field& field = fields[i];
			char* ptr = base + field.offset;
			IReflectObject* reflectObject = reinterpret_cast<IReflectObject*>(ptr);
			if (field.type == UniqueType<ZDynamicVector>::Get()) {
				ZDynamicVector::Iterator iterator(static_cast<ZDynamicVector*>(reflectObject));
				reflect.ProcessProperty(iterator, field.type, field.name.c_str(), base, ptr, nullptr);
			} else {
				reflect.ProcessProperty(field.reflectable ? dummyObject : *reflectObject, field.type, field.name.c_str(), base, ptr, nullptr);
			}
		}
	}

	return *this;
}

IUniqueInfo* ZDynamicUniqueAllocator::Alloc(const String& name, size_t size) {
	assert(false);
	return nullptr;
}

IUniqueInfo* ZDynamicUniqueAllocator::Get(const String& name) {
	std::map<String, ZDynamicInfo>::iterator it = mapType.find(name);
	return it == mapType.end() ? nullptr : &it->second;
}

IUniqueInfo* ZDynamicUniqueAllocator::AllocFromDescriptor(const String& name, const std::vector<ZDynamicInfo::Field>& descriptors) {
	// Compose new name
	String desc;
	String allNames;
	size_t maxSize = (size_t)-1;
	std::vector<ZDynamicInfo::Field> fields = descriptors;
	std::vector<size_t> layout;

	for (size_t k = 0; k < fields.size(); k++) {
		if (k != 0) desc += ", ";
		ZDynamicInfo::Field& p = fields[k];
		IUniqueInfo* info = p.type.info;
		assert(maxSize >= info->size);
		maxSize = Min(maxSize, info->size); // Must be sorted descending order by size

		desc += p.name + ": " + info->typeName;
		allNames += info->typeName;

		if (!layout.empty()) {
			for (size_t i = 0; i < layout.size(); i++) {
				size_t& offset = layout[i];

				// enough
				if (i < layout.size() - 1) {
					if (Alignment(offset) >= info->size && offset + info->size <= layout[i + 1]) {
						p.offset = offset;
						offset += info->size;
						break;
					}
				} else {
					// last one but no fit
					p.offset = offset + Alignment(info->size) - Alignment(offset);
					layout.push_back(p.offset + info->size);
				}
			}
		} else {
			p.offset = 0;
			layout.push_back(info->size);
		}
	}

	std::stringstream ss;
	ss << name.c_str() << "{" << descriptors.size() << "-" << HashBuffer(allNames.c_str(), allNames.size()) << "-" << HashBuffer(desc.c_str(), desc.size()) << "}";

	String newName = ss.str();
	std::map<String, ZDynamicInfo>::iterator it = mapType.find(newName);
	if (it != mapType.end()) {
		// assert(false); // Performance warning: should check it before calling me!
		return &it->second;
	} else {
		ZDynamicInfo& info = mapType[name];
		info.allocator = this;
		info.typeName = newName;
		std::sort(fields.begin(), fields.end());
		for (size_t i = 0; i < fields.size(); i++) {
			info.mapNameToField[fields[i].name] = (uint32_t)i;
		}

		info.fields = std::move(fields);
		info.size = fields.empty() ? sizeof(size_t) : fields.back().offset + sizeof(size_t) - Alignment(fields.back().offset);
		info.intState = IUniqueInfo::NOT_INTEGER;

		return &info;
	}
}

ZDynamicInfo* ZDynamicObject::GetDynamicInfo() const {
	return dynamicInfo;
}

ZDynamicObject& ZDynamicObject::operator = (const ZDynamicObject& rhs) {
	if (dynamicInfo == rhs.dynamicInfo && this != &rhs) {
		std::vector<ZDynamicInfo::Field>& fields = dynamicInfo->fields;
		const char* rbase = (const char*)&rhs + sizeof(*this);
		for (size_t i = 0; i < fields.size(); i++) {
			ZDynamicInfo::Field& field = fields[i];
			Set(field, rbase);
		}
	}

	return *this;
}

void ZDynamicObject::Set(const ZDynamicInfo::Field& field, const void* value) {
	char* buffer = (char*)this + sizeof(*this) + field.offset;
	if (buffer == value) return;

	if (field.type->GetAllocator() == dynamicInfo->GetAllocator()) {
		const ZDynamicObject* src = reinterpret_cast<const ZDynamicObject*>(value);
		ZDynamicObject* dst = reinterpret_cast<ZDynamicObject*>(buffer);
		*dst = *src;
	} else if (field.controller != nullptr) {
		field.controller->Assigner(buffer, value);
	} else {
		memcpy(buffer, value, field.type->GetSize());
	}
}

void* ZDynamicObject::Get(const ZDynamicInfo::Field& field) const {
	return (char*)this + sizeof(*this) + field.offset;
}

ZDynamicObjectWrapper::ZDynamicObjectWrapper(ZDynamicUniqueAllocator& allocator) : uniqueAllocator(uniqueAllocator), dynamicObject(nullptr) {}

ZDynamicObjectWrapper::~ZDynamicObjectWrapper() {
	if (dynamicObject != nullptr) {
		dynamicObject->ReleaseObject();
	}
}

TObject<IReflect>& ZDynamicObjectWrapper::operator () (IReflect& reflect) {
	ReflectClass(ZDynamicObjectWrapper);
	return *this;
}

ZDynamicVector::ZDynamicVector(Unique type, ZDynamicInfo::MemController* mc, size_t c, bool r) : unique(type), memController(mc), reflectable(r), count((uint32_t)c), buffer(nullptr) {
	Init();
}

void ZDynamicVector::Init() {
	size_t size = unique->GetSize();
	if (count != 0) {
		assert(buffer == nullptr);
		buffer = new char[size * count];

		// initialize them
		if (memController != nullptr) {
			char* base = reinterpret_cast<char*>(buffer);
			for (size_t k = 0; k < count; k++) {
				memController->Creator(base + k * size);
			}
		}
	}
}

ZDynamicVector::~ZDynamicVector() {
	Cleanup();
}

void ZDynamicVector::Reinit(Unique u, ZDynamicInfo::MemController* mc, size_t n, bool r) {
	Cleanup();
	reflectable = r;
	unique = u;
	memController = mc;	
	count = n;
	Init();
}

void ZDynamicVector::Set(size_t i , const void* value) {
	memController->Assigner(reinterpret_cast<char*>(buffer) + i * unique->GetSize(), value);
}

void* ZDynamicVector::Get(size_t i) const {
	return reinterpret_cast<char*>(buffer) + i * unique->GetSize();
}

void ZDynamicVector::Cleanup() {
	if (buffer != nullptr) {
		if (memController != nullptr) {
			size_t size = unique->GetSize();
			char* base = reinterpret_cast<char*>(buffer);
			for (size_t k = 0; k < count; k++) {
				memController->Deletor(base + k * size);
			}
		}

		delete[] reinterpret_cast<char*>(buffer);
		buffer = nullptr;
	}
}

ZDynamicVector& ZDynamicVector::operator = (const ZDynamicVector& vec) {
	if (this != &vec) {
		Cleanup();
		// copy info
		assert(buffer == nullptr);
		unique = vec.unique;
		memController = vec.memController;
		count = vec.count;

		size_t size = unique->GetSize();
		if (count != 0) {
			buffer = new char[size * count];

			// initialize them
			if (memController != nullptr) {
				char* base = reinterpret_cast<char*>(buffer);
				const char* src = reinterpret_cast<const char*>(vec.buffer);
				for (size_t k = 0; k < count; k++) {
					memController->Creator(base + k * size);
					memController->Assigner(base + k * size, src + k * size);
				}
			}
		}
	}

	return *this;
}

void ZDynamicVector::VectorCreator(void* buffer) {
	new (buffer) ZDynamicVector(UniqueType<int>::Get(), nullptr, 0, false);
}

void ZDynamicVector::VectorDeletor(void* buffer) {
	reinterpret_cast<ZDynamicVector*>(buffer)->~ZDynamicVector();
}

void ZDynamicVector::VectorAssigner(void* dst, const void* src) {
	*reinterpret_cast<ZDynamicVector*>(dst) = *reinterpret_cast<const ZDynamicVector*>(src);
}

ZDynamicInfo::MemController& ZDynamicVector::GetVectorController() {
	static ZDynamicInfo::MemController controller = {
		&ZDynamicVector::VectorCreator,
		&ZDynamicVector::VectorDeletor,
		&ZDynamicVector::VectorAssigner,
	};

	return controller;
}