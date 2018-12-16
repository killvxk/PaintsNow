#include "IScript.h"

namespace PaintsNow {
	IScript::MetaLibrary ScriptLibrary;
	IScript::MetaMethod ScriptMethod;
	IScript::MetaVariable ScriptVariable;
	IScript::MetaVariableIterator ScriptVariableIterator;
	IScript::MetaRemoteEntry ScriptWrapper;
	IScript::Request::LocalStart beginlocal;
	IScript::Request::LocalEnd endlocal;
	IScript::Request::TableStart begintable;
	IScript::Request::TableEnd endtable;
	IScript::Request::Key key("");
	IScript::BaseDelegate delegate(nullptr);
	IScript::Request::Nil nil;
	IScript::Request::Global global;
	IScript::Request::Local local;
	IScript::Request::Ref ref;
	IScript::Request::Skip skip(0);
	IScript::Request::Sync Sync;
	IScript::Request::Async Async;
}

using namespace PaintsNow;

IScript::Object::Object() {}

IScript::Object::~Object() {
	// printf("OBJECT DELETED %p\n", this);
}

void IScript::Object::ScriptUninitialize(Request& request) {
	ReleaseObject();
}

void IScript::Object::ScriptInitialize(Request& request) {}

IScript::Library::Library(IThread& threadApi) : ISyncObject(threadApi) {
}

IScript::Library::~Library() {}

void IScript::Library::ScriptUninitialize(IScript::Request& request) {
	// do not release object by default
}

void IScript::Request::QueryInterface(const TWrapper<void, IScript::Request&, IReflectObject&, const Request::Ref&>& callback, IReflectObject& target, const Request::Ref& g) {
	// exports no interfaces by default
	callback(*this, target, g);
}

IScript::MetaLibrary::MetaLibrary(const String& n) : name(n) {}
TObject<IReflect>& IScript::MetaLibrary::operator () (IReflect& reflect) {
	ReflectClass(MetaLibrary);
	if (reflect.IsReflectProperty()) {
		ReflectProperty(name);
	}

	return *this;
}

IScript::MetaLibrary IScript::MetaLibrary::operator = (const String& value) {
	return MetaLibrary(value);
}


class Register : public IReflect {
public:
	Register(IScript::Request& req) : IReflect(true, true), request(req) {}

	virtual void ProcessProperty(IReflectObject& s, Unique typeID, const char* name, void* base, void* ptr, const MetaChainBase* meta) {
		static Unique typedBaseType = UniqueType<IScript::MetaLibrary>::Get();
		
		if (!s.IsBaseObject() && s.QueryInterface(UniqueType<IScript::Library>()) != nullptr) {
			for (const MetaChainBase* t = meta; t != nullptr; t = t->GetNext()) {
				const MetaNodeBase* node = t->GetNode();
				if (!node->IsBaseObject() && node->GetUnique() == typedBaseType) {
					const IScript::MetaLibrary* entry = static_cast<const IScript::MetaLibrary*>(node);
					request << key(entry->name.empty() ? name : entry->name) << static_cast<IScript::Library&>(s);
				}
			}
		}
	}

	virtual void ProcessMethod(Unique typeID, const char* name, const TProxy<>* p, const Param& retValue, const std::vector<Param>& params, const MetaChainBase* meta) {
		static Unique typedBaseType = UniqueType<IScript::MetaMethod::TypedBase>::Get();
		for (const MetaChainBase* t = meta; t != nullptr; t = t->GetNext()) {
			const MetaNodeBase* node = t->GetNode();
			if (!node->IsBaseObject() && node->GetUnique() == typedBaseType) {
				const IScript::MetaMethod::TypedBase* entry = static_cast<const IScript::MetaMethod::TypedBase*>(node);
				entry->Register(request, name);
			}
		}
	}

	IScript::Request& request;
};

bool IScript::Library::Initialize() {
	return true;
}

void IScript::Library::Uninitialize() {

}

void IScript::Library::Require(IScript::Request& request) {
	request << begintable;
	request << key("__delegate__") << delegate(this);
	Register reg(request);
	(*this)(reg);
	request << endtable;
}


struct InspectCustomStructure : public IReflect {
	InspectCustomStructure(IScript::Request& r, IReflectObject& obj) : request(r), IReflect(true, false) {
		request << begintable;
		String name, count;
		InspectCustomStructure::FilterType(obj.GetUnique().info->typeName, name, count);

		request << key("Type") << name;
		request << key("Optional") << false;
		request << key("Pointer") << false;
		request << key("List") << false;
		request << key("Fields") << begintable;
		obj(*this);
		request << endtable;
		request << endtable;
	}

	virtual void ProcessProperty(IReflectObject& s, Unique typeID, const char* name, void* base, void* ptr, const MetaChainBase* meta) {
		static Unique typedBaseType = UniqueType<IScript::MetaVariable::TypedBase>::Get();
		for (const MetaChainBase* t = meta; t != nullptr; t = t->GetNext()) {
			const MetaNodeBase* node = t->GetNode();
			if (!node->IsBaseObject() && node->GetUnique() == typedBaseType) {
				IScript::MetaVariable::TypedBase* entry = const_cast<IScript::MetaVariable::TypedBase*>(static_cast<const IScript::MetaVariable::TypedBase*>(node));
				request << key(entry->name.empty() ? name : entry->name);
				InspectCustomStructure::ProcessMember(request, typeID, s.IsIterator(), false);
			}
		}
	}

	virtual void ProcessMethod(Unique typeID, const char* name, const TProxy<>* p, const Param& retValue, const std::vector<Param>& params, const MetaChainBase* meta) {}



	static bool FilterType(const String& name, String& ret, String& count) {
		// parse name
		if (name == UniqueType<String>::Get()->typeName) {
			ret = "String";
			return true;
		} else if (name.find("std::") == 0) {
			ret = "vector";
			return false;
		} else {
			ret = "";
			bool inword = false;
			for (size_t i = 0; i < name.length(); i++) {
				char ch = name[i];
				if (isalnum(ch) || ch == '_') {
					if (!inword) ret = "";
					ret += ch;
					inword = true;
				} else if (ch == ':') {
					ret += '_';
					inword = true;
				} else {
					inword = false;
				}
			}
		}

		static const String prefix = "PaintsNow__";
		String::size_type pns = ret.find(prefix);
		if (pns != String::npos) {
			ret = ret.substr(pns + prefix.length());
		}

		static const String typePrefix = "TType";
		String::size_type pos = name.find(typePrefix);
		String::size_type end = pos;
		if (pos != String::npos) {
			pos += typePrefix.length();
			end += typePrefix.length();
			while (end < name.length() && isdigit(name[end])) end++;
		}

		count = "";
		if (end != String::npos) {
			count = name.substr(pos, end - pos);
		}

		return true;
	}

	static void ProcessMember(IScript::Request& request, Unique type, bool isIterator, bool optional) {
		// filter TType
		String typeName = type.info->typeName;

		bool isDelegate = typeName.find("PaintsNow::BaseDelegate") != 0 || typeName.find("PaintsNow::Delegate<") != 0;
		String name, count;
		bool subfield = InspectCustomStructure::FilterType(typeName, name, count);

		if (type.info->creator) {
			IReflectObject* obj = type.info->Create();
			if (obj != nullptr) {
				InspectCustomStructure(request, *obj);
				obj->ReleaseObject();
			} else {
				request << begintable;
				request << key("Pointer") << false;
				request << key("Optional") << false;
				request << key("List") << false;
				request << key("Fields") << begintable << endtable;
				request << key("Type") << "ErrorType";
				request << endtable;
				assert(false);
			}
		} else {
			request << begintable;
			request << key("Optional") << (isDelegate && optional);
			request << key("Pointer") << count.empty();
			request << key("List") << isIterator;
			request << key("Fields") << begintable;

			if (subfield) {
				int c = atoi(count.c_str());
				for (int i = 0; i < c; i++) {
					request << name;
				}
			}

			request << endtable;
			request << key("Type") << (name + count);
			request << endtable;
		}
	}

	IScript::Request& request;
};

struct InspectProcs : public IReflect {
	InspectProcs(IScript::Request& r, IReflectObject& obj) : request(r), IReflect(false, true) {
		request.DoLock();
		request << begintable;
		obj(*this);
		request << endtable;
		request.UnLock();
	}

	virtual void ProcessProperty(IReflectObject& s, Unique typeID, const char* name, void* base, void* ptr, const MetaChainBase* meta) {}
	virtual void ProcessMethod(Unique typeID, const char* name, const TProxy<>* p, const Param& retValue, const std::vector<Param>& params, const MetaChainBase* meta) {
		// convert params ...
		while (meta != nullptr) {
			const MetaNodeBase* node = meta->GetNode();
			static Unique ScriptMethodUnique = UniqueType<IScript::MetaMethod::TypedBase>::Get();
			if (node->GetUnique() == ScriptMethodUnique) {
				const IScript::MetaMethod::TypedBase* entry = static_cast<const IScript::MetaMethod::TypedBase*>(node);
				request << key(entry->name.empty() ? name : entry->name) << begintable << key("Params") << begintable;
				for (size_t i = 1; i < params.size(); i++) {
					InspectCustomStructure::ProcessMember(request, params[i].type, false, true);
				}

				request << endtable;
				request << endtable;
				break;
			}

			meta = meta->GetNext();
		}
	}


	IScript::Request& request;
};

void IScript::Library::InspectObject(Request& request, IReflectObject& object) {
	InspectProcs(request, object);
}

void IScript::Library::RequestInspect(Request& request) {
	InspectObject(request, *this);
}

TObject<IReflect>& IScript::Library::operator () (IReflect& reflect) {
	ReflectClass(Library);

	if (reflect.IsReflectMethod()) {
		ReflectMethod(RequestInspect)[ScriptMethod = "Inspect"];
	}

	return *this;
}

IScript::MetaVariable::MetaVariable(const String& k) : name(k) {}
IScript::MetaVariable::~MetaVariable() {}
IScript::MetaVariable IScript::MetaVariable::operator = (const String& k) {
	return MetaVariable(k);
}

IScript::MetaVariableIterator IScript::MetaVariableIterator::operator = (const String& k) {
	return MetaVariableIterator(k);
}

TObject<IReflect>& IScript::MetaVariable::operator () (IReflect& reflect) {
	ReflectClass(MetaVariable)[Constructable];

	return *this;
}

TObject<IReflect>& IScript::MetaVariable::TypedBase::operator () (IReflect& reflect) {
	ReflectClass(TypedBase);

	return *this;
}


template <bool read>
class Serializer : public IReflect {
public:
	Serializer(IScript::Request& req) : IReflect(true, false), request(req) {}
	virtual void ProcessProperty(IReflectObject& s, Unique typeID, const char* name, void* base, void* ptr, const MetaChainBase* meta) {
		static Unique typedBaseType = UniqueType<IScript::MetaVariable::TypedBase>::Get();
		for (const MetaChainBase* t = meta; t != nullptr; t = t->GetNext()) {
			const MetaNodeBase* node = t->GetNode();
			if (!node->IsBaseObject() && node->GetUnique() == typedBaseType) {
				IScript::MetaVariable::TypedBase* entry = const_cast<IScript::MetaVariable::TypedBase*>(static_cast<const IScript::MetaVariable::TypedBase*>(node));

				if (s.IsIterator()) {
					IScript::Request::TableStart ts;
					IIterator& it = static_cast<IIterator&>(s);

					if (read) {
						request >> key(entry->name.empty() ? name : entry->name) >> ts;
						it.Initialize((size_t)ts.count);
					} else {
						request << key(entry->name.empty() ? name : entry->name) << begintable;
					}

					if (it.GetPrototype().IsBaseObject()) {
						while (it.Next()) {
							void* ptr = it.Get();
							if (read) {
								entry->Read(request, false, name, ptr);
							} else {
								entry->Write(request, false, name, ptr);
							}
						}
					} else {
						while (it.Next()) {
							(*reinterpret_cast<IReflectObject*>(it.Get()))(*this);
						}
					}

					if (read) {
						request >> endtable;
					} else {
						request << endtable;
					}
				} else {
					if (read) {
						entry->Read(request, true, name, nullptr);
					} else {
						entry->Write(request, true, name, nullptr);
					}
				}
			}
		}
	}

	virtual void ProcessMethod(Unique typeID, const char* name, const TProxy<>* p, const Param& retValue, const std::vector<Param>& params, const MetaChainBase* meta) {}


	IScript::Request& request;
	size_t itemCount;
};


IScript::BaseDelegate::BaseDelegate(Object* p, bool n) {
	alignment1 = 0;
	alignment2 = 0;
	ptr = p;
	isNative = n;
	isNewRegistered = false;
}

IScript::BaseDelegate::~BaseDelegate() {}

bool IScript::BaseDelegate::IsNative() const {
	return isNative;
}

IScript::BaseDelegate IScript::BaseDelegate::operator() (Object* p) {
	assert(IsNative());
	return BaseDelegate(p, true);
}

IScript::IScript(IThread& api) : ISyncObject(api), debugMask(DEBUG_LINE) {
}

IScript::~IScript() {
}

IScript::Request::~Request() {}
void IScript::Request::DoLock() {
	GetScript()->DoLock();
}

void IScript::Request::UnLock() {
	GetScript()->UnLock();
}

IScript::Request::Skip::Skip(int k) : count(k) {}
IScript::Request::Skip IScript::Request::Skip::operator () (int k) {
	return IScript::Request::Skip(k);
}

IScript::Request::Ref::Ref(size_t i) : value(i) {}

IScript::Request::Key::Key(const String& k, IScript::Request::TYPE t) : type(t), key(k) {}
IScript::Request::Key IScript::Request::Key::operator () (const String& k) {
	return Key(k);
}


/*
IScript* IScript::Request::operator -> () {
	return GetScript();
}*/

IScript::Request& IScript::Request::operator >> (Void*& t) {
	int64_t v = 0;
	(*this) >> v;
	t = (Void*)v;
	return *this;
}

IScript::Request& IScript::Request::operator << (Void* t) {
	(*this) << (int64_t)t;
	return *this;
}

#ifdef _MSC_VER
IScript::Request& IScript::Request::operator >> (long& t) {
	int64_t v = 0;
	(*this) >> v;
	t = (long)v;
	return *this;
}

IScript::Request& IScript::Request::operator >> (unsigned long& t) {
	int64_t v = 0;
	(*this) >> v;
	t = (unsigned long)v;
	return *this;
}

IScript::Request& IScript::Request::operator << (long t) {
	(*this) << (int64_t)t;
	return *this;
}

IScript::Request& IScript::Request::operator << (unsigned long t) {
	(*this) << (int64_t)t;
	return *this;
}
#endif

IScript::Request& IScript::Request::operator >> (int8_t& t) {
	int64_t v = 0;
	(*this) >> v;
	t = (int8_t)v;
	return *this;
}

IScript::Request& IScript::Request::operator << (int8_t t) {
	(*this) << (int64_t)t;
	return *this;
}

IScript::Request& IScript::Request::operator >> (int16_t& t) {
	int64_t v = 0;
	(*this) >> v;
	t = (int16_t)v;
	return *this;
}

IScript::Request& IScript::Request::operator << (int16_t t) {
	(*this) << (int64_t)t;
	return *this;
}

IScript::Request& IScript::Request::operator >> (int32_t& t) {
	int64_t v = 0;
	(*this) >> v;
	t = (int32_t)v;
	return *this;
}

IScript::Request& IScript::Request::operator << (int32_t t) {
	(*this) << (int64_t)t;
	return *this;
}

IScript::Request& IScript::Request::operator >> (uint8_t& t) {
	uint64_t v = 0;
	(*this) >> v;
	t = (uint8_t)v;
	return *this;
}


IScript::Request& IScript::Request::operator << (uint8_t t) {
	(*this) << (uint64_t)t;
	return *this;
}

IScript::Request& IScript::Request::operator >> (uint16_t& t) {
	uint64_t v = 0;
	(*this) >> v;
	t = (uint16_t)v;
	return *this;
}

IScript::Request& IScript::Request::operator << (uint16_t t) {
	(*this) << (uint64_t)t;
	return *this;
}

IScript::Request& IScript::Request::operator >> (uint32_t& t) {
	uint64_t v = 0;
	(*this) >> v;
	t = (uint32_t)v;
	return *this;
}

IScript::Request& IScript::Request::operator << (uint32_t t) {
	(*this) << (uint64_t)t;
	return *this;
}

IScript::Request& IScript::Request::operator >> (uint64_t& t) {
	int64_t v = 0;
	(*this) >> v;
	t = (uint64_t)v;
	return *this;
}

IScript::Request& IScript::Request::operator << (uint64_t t) {
	(*this) << (int64_t)t;
	return *this;
}


IScript::Request& IScript::Request::operator << (Library& module) {
	module.Require(*this);
	return *this;
}


IScript::Request& IScript::Request::operator >> (Void&) {
	return *this;
}

IScript::Request& IScript::Request::operator << (const Void&) {
	return *this;
}

IScript::Request& IScript::Request::operator >> (float& value) {
	double db = 0;
	*this >> db;
	value = (float)db;
	return *this;
}

IScript::Request& IScript::Request::operator << (const PackBase& object) {
	String data;
	if (object.Serialize(data)) {
		*this << data;
	}

	return *this;
}

IScript::Request& IScript::Request::operator >> (PackBase& object) {
	String data;
	*this >> data;
	object.Deserialize(data);
	return *this;
}

IScript::Request& IScript::Request::operator >> (IReflectObject& object) {
	assert(!object.IsBaseObject());
	*this >> begintable;
	Serializer<true> s(*this);
	object(s);
	*this >> endtable;

	return *this;
}

IScript::Request& IScript::Request::operator << (const IReflectObject& object) {
	assert(!object.IsBaseObject());
	*this << begintable;
	Serializer<false> s(*this);
	(const_cast<IReflectObject&>(object))(s);
	*this << endtable;

	return *this;
}

IScript::Request& IScript::Request::operator << (const Skip& skip) {
	return *this >> skip;
}

void IScript::SetDebugHandler(const TWrapper<void, Request&, int, int>& handler, int mask) {
	debugHandler = handler;
	debugMask = mask;
}

bool IScript::IsTypeCompatible(Request::TYPE target, Request::TYPE source) const {
	return target == source;
}

void IScript::SetErrorHandler(const TWrapper<void, Request&, String&>& handler) {
	errorHandler = handler;
}

void IScript::SetDispatcher(const TWrapper<void, Request&, IHost*, size_t, const TWrapper<void, Request&>&>& disp) {
	dispatcher = disp;
}

const char* IScript::QueryUniformResource(const String & path, size_t & length) {
	return nullptr;
}

const TWrapper<void, IScript::Request&, IHost*, size_t, const TWrapper<void, IScript::Request&>&>& IScript::GetDispatcher() const {
	return dispatcher;
}

const TWrapper<void, IScript::Request&, int, int>& IScript::GetDebugHandler() const {
	return debugHandler;
}

void IScript::Request::Error(const String& msg) {
	const TWrapper<void, Request&, String&>& handler = GetScript()->errorHandler;
	if (handler) {
		handler(*this, const_cast<String&>(msg));
	}
}

/*
#include <Windows.h>

void IScript::DoLock() {
	DWORD id = ::GetCurrentThreadId();

	printf("Thread ID %d try to lock script\n", id);
	ISyncObject::DoLock();
	printf("Thread ID %d sucessfully locked script\n", id);
}

void IScript::UnLock() {
	DWORD id = ::GetCurrentThreadId();
	printf("Thread ID %d unlocked script\n", id);
	ISyncObject::UnLock();
}*/

void IScript::Reset() {
}

TObject<IReflect>& IScript::Request::Key::operator () (IReflect& reflect) {
	ReflectClass(Key)[Constructable];
	if (reflect.IsReflectProperty()) {
		ReflectProperty(key);
	}

	return *this;
}

void IScript::Request::Key::SetKey(const char* k) {
	key = k;
}

const char* IScript::Request::Key::GetKey() const {
	return key.c_str();
}

IScript::Request::TYPE IScript::Request::Key::GetType() const {
	return type;
}

void IScript::Request::Key::SetType(IScript::Request::TYPE t) {
	type = t;
}


bool IScript::Request::AutoWrapperBase::IsSync() const {
	return false;
}


bool IScript::Request::Sync::IsSync() const {
	return true;
}

void IScript::Request::Sync::Execute(Request& request) const {
	assert(false);
}


IScript::Request::AutoWrapperBase* IScript::Request::Sync::Copy() const {
	return nullptr;
}

bool IScript::Request::Async::IsSync() const {
	return false;
}

void IScript::Request::Async::Execute(Request& request) const {}

IScript::Request::AutoWrapperBase* IScript::Request::Async::Copy() const {
	return nullptr;
}

TObject<IReflect>& IScript::MetaRemoteEntry::operator () (IReflect& reflect) {
	ReflectClass(MetaRemoteEntry)[Constructable];
	return *this;
}

IScript::MetaMethod::MetaMethod(const String& k) : key(k) {}
IScript::MetaMethod::~MetaMethod() {}

IScript::MetaMethod IScript::MetaMethod::operator = (const String& key) {
	return MetaMethod(key);
}

TObject<IReflect>& IScript::MetaMethod::operator () (IReflect& reflect) {
	ReflectClass(MetaMethod)[Constructable];

	return *this;
}

TObject<IReflect>& IScript::MetaMethod::TypedBase::operator () (IReflect& reflect) {
	ReflectClass(TypedBase);

	return *this;
}