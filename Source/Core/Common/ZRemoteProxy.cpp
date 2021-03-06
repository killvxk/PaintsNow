#include "ZRemoteProxy.h"
#include "ZMemoryStream.h"
#include <iterator>

using namespace PaintsNow;

enum { UNIQUE_GLOBAL = 0, UNIQUE_END = 1 };
enum { GLOBAL_INTERFACE_CREATE = 0, GLOBAL_INTERFACE_QUERY };

TableImpl::TableImpl() {
	mapPart = new std::map<String, Variant>();
}

TableImpl::TableImpl(const TableImpl& rhs) {
	if (&rhs != this) {
		mapPart = nullptr;
		*this = rhs;
	}
}

TableImpl& TableImpl::operator = (const TableImpl& rhs) {
	if (&rhs != this) {
		if (mapPart == nullptr) {
			mapPart = new std::map<String, Variant>();
		}

		*mapPart = *rhs.mapPart;
	}

	return *this;
}

TableImpl::~TableImpl() {
	delete mapPart;
}

void Value<TableImpl>::Reflect(IReflect& reflect) {
	if (reflect.IsReflectProperty()) {
		ReflectProperty(IterateVector(value.arrayPart));
		uint64_t count = value.mapPart->size();
		ReflectProperty(count);
		if (value.mapPart->size() == 0) { // Read
			for (size_t i = 0; i < count; i++) {
				Variant v;
				String key;
				reflect.OnProperty(key, "Key", this, nullptr);
				reflect.OnProperty(v, "Value", this, nullptr);
				(*value.mapPart)[key] = v;
			}
		} else {
			for (std::map<String, Variant>::const_iterator it = value.mapPart->begin(); it != value.mapPart->end(); ++it) {
				reflect.OnProperty(it->first, "Key", this, nullptr);
				reflect.OnProperty(it->second, "Value", this, nullptr);
			}
		}
	}
}

ZRemoteProxy::Request::Packet::Packet() : object(0), procedure(0), callback(0), response(false) {}

TObject<IReflect>& ZRemoteProxy::Request::Packet::operator () (IReflect& reflect) {
	ReflectClass(Packet)[Constructable];
	if (reflect.IsReflectProperty()) {
		ReflectProperty(object);
		ReflectProperty(procedure);
		ReflectProperty(callback);
		ReflectProperty(response);
		ReflectProperty(deferred);
		ReflectProperty(IterateVector(vars));
		ReflectProperty(IterateVector(localDelta));
		ReflectProperty(IterateVector(remoteDelta));
	}

	return *this;
}

TObject<IReflect>& ZRemoteProxy::operator() (IReflect& reflect) {
	ReflectClass(ZRemoteProxy);

	return *this;
}

class MethodInspector : public IReflect {
public:
	MethodInspector(IScript::Object* o, ZRemoteProxy::ObjectInfo& info) : IReflect(false, true), object(o), objectInfo(info) {
	}

	~MethodInspector() {
		for (size_t i = 0; i < objectInfo.collection.size(); i++) {
			objectInfo.collection[i].method = Wrap(&objectInfo.collection[i], &ZRemoteProxy::ObjectInfo::Entry::CallFilter);
			objectInfo.collection[i].index = (long)i;
		}

		objectInfo.needQuery = false;
	}

	virtual void ProcessProperty(IReflectObject& s, Unique typeID, const char* name, void* base, void* ptr, const MetaChainBase* meta) {}
	virtual void ProcessMethod(Unique typeID, const char* name, const TProxy<>* p, const Param& retValue, const std::vector<Param>& params, const MetaChainBase* meta) {
		static Unique typedBaseType = UniqueType<IScript::MetaMethod::TypedBase>::Get();
		for (const MetaChainBase* t = meta; t != nullptr; t = t->GetNext()) {
			const MetaNodeBase* node = t->GetNode();
			if (!node->IsBaseObject() && node->GetUnique() == typedBaseType) {
				const IScript::MetaMethod::TypedBase* entry = static_cast<const IScript::MetaMethod::TypedBase*>(node);
				objectInfo.collection.push_back(ZRemoteProxy::ObjectInfo::Entry());
				ZRemoteProxy::ObjectInfo::Entry& t = objectInfo.collection.back();
				t.name = entry->name.empty() ? name : entry->name;
				t.retValue = retValue;
				t.params = params;
				t.wrapper = entry->CreateWrapper();
				t.obj = object;
			}
		}
	}

	IScript::Object* object;
	ZRemoteProxy::ObjectInfo& objectInfo;
};

ZRemoteProxy::ZRemoteProxy(IThread& threadApi, ITunnel& t, const TFactoryBase<IScript::Object>& creator, const String& entry, const TWrapper<void, IScript::Request&, bool, STATUS, const String&>& sh) : IScript(threadApi), tunnel(t), defaultRequest(*this, nullptr, statusHandler), objectCreator(creator), statusHandler(sh), dispatcher(nullptr), dispThread(nullptr) {
	SetEntry(entry);
}

void ZRemoteProxy::Cleanup() {
	entry = "";

	if (dispatcher != nullptr) {
		tunnel.DeactivateDispatcher(dispatcher);
		dispatcher = nullptr;
	}

	// Wait for thread proc end
	if (dispThread != nullptr) {
		threadApi.Wait(dispThread);
		threadApi.DeleteThread(dispThread); 
		dispThread = nullptr;
	}
}

ZRemoteProxy::~ZRemoteProxy() {
	Cleanup();
}

const TFactoryBase<IScript::Object>& ZRemoteProxy::GetObjectCreator() const {
	return objectCreator;
}

bool ZRemoteProxy::ThreadProc(IThread::Thread* thread, size_t context) {
	ITunnel::Dispatcher* disp = dispatcher;
	do {
		ITunnel::Listener* listener = nullptr;
		if (!entry.empty()) {
			listener = tunnel.OpenListener(disp, Wrap(this, &ZRemoteProxy::HandleEvent), Wrap(this, &ZRemoteProxy::OnConnection), entry);
			if (listener == nullptr) {
				tunnel.CloseDispatcher(disp);
				HandleEvent(ITunnel::ABORT);
				return false;
			}

			tunnel.ActivateListener(listener);
		}

		tunnel.ActivateDispatcher(disp);

		if (listener != nullptr) {
			tunnel.DeactivateListener(listener);
			tunnel.CloseListener(listener);
		}
	} while (!entry.empty());

	tunnel.CloseDispatcher(disp);
	return false;
}

void ZRemoteProxy::HandleEvent(ITunnel::EVENT event) {}

void ZRemoteProxy::SetEntry(const String& e) {
	entry = e;
}

void ZRemoteProxy::Reset() {
	String orgEntry = entry;
	Cleanup();
	entry = orgEntry;
	Run();
}

bool ZRemoteProxy::Run() {
	assert(dispatcher == nullptr);
	dispatcher = tunnel.OpenDispatcher();

	if (dispThread == nullptr) {
		dispThread = threadApi.NewThread(Wrap(this, &ZRemoteProxy::ThreadProc), 0, false);
	}

	return dispThread != nullptr;
}

void ZRemoteProxy::Reconnect(IScript::Request& request) {
	ZRemoteProxy::Request& req = static_cast<ZRemoteProxy::Request&>(request);
	req.Reconnect();
}

void ZRemoteProxy::Request::Run() {
	host.tunnel.ActivateConnection(connection);
}

const ITunnel::Handler ZRemoteProxy::OnConnection(ITunnel::Connection* connection) {
	ZRemoteProxy::Request* request = new ZRemoteProxy::Request(*this, connection, statusHandler);
	request->Attach(connection);
	return Wrap(request, &Request::OnConnection);
}

IScript::Request* ZRemoteProxy::NewRequest(const String& entry) {
	Request* request = new ZRemoteProxy::Request(*this, nullptr, statusHandler);
	request->manually = true;
	assert(dispatcher != nullptr);

	ITunnel::Connection* c = tunnel.OpenConnection(dispatcher, Wrap(request, &Request::OnConnection), entry);
	if (c != nullptr) {
		request->Attach(c);
		request->Run();
		return request;
	} else {
		request->ReleaseObject();
		return nullptr;
	}
}

IScript::Request& ZRemoteProxy::GetDefaultRequest() {
	return defaultRequest;
}

IScript::Request& ZRemoteProxy::Request::CleanupIndex() {
	idx = initCount;
	return *this;
}

static size_t count = 0;
ZRemoteProxy::ObjectInfo::ObjectInfo() : refCount(0), needQuery(true) {
	count++;
}

ZRemoteProxy::ObjectInfo::~ObjectInfo() {
	for (size_t i = 0; i < collection.size(); i++) {
		delete collection[i].wrapper;
	}
	count--;
}

void ZRemoteProxy::Request::Attach(ITunnel::Connection* c) {
	connection = c;
}

void ZRemoteProxy::Request::PostPacket(Packet& packet) {

	/*
	// register objects if exists
	for (size_t i = packet.start; i < packet.vars.size(); i++) {
		Variant& var = packet.vars[i];
		if (var->QueryValueUnique() == UniqueType<IScript::Object*>::Get()) {
			Request::Value<IScript::Object*>& v = static_cast<Request::Value<IScript::Object*>&>(*var.Get());
			// inspect routines
			std::map<IScript::Object*, ObjectInfo>::iterator it = removedRemoteObjects.find(v.value);
			if (it == activeObjects.end()) {
				ObjectInfo& info = (activeObjects[v.value] = ObjectInfo());
				MethodInspector inspector(info);
				(*v.value)(inspector);
			}
		}
	}*/

	// swap local <=> remote
	for (std::map<IScript::Object*, long>::iterator it = remoteObjectRefDelta.begin(); it != remoteObjectRefDelta.end(); ++it) {
		packet.localDelta.push_back(std::make_pair((uint64_t)it->first, it->second));
	}

	for (std::map<IScript::Object*, long>::iterator is = localObjectRefDelta.begin(); is != localObjectRefDelta.end(); ++is) {
		packet.remoteDelta.push_back(std::make_pair((uint64_t)is->first, is->second));
	}

	// use filter 
	// *connection << packet;
	ZMemoryStream stream(0x1000, true);
	stream << packet;
	ITunnel::Packet state;
	state.header.length = (ITunnel::PacketSizeType)stream.GetTotalLength();
	host.tunnel.WriteConnectionPacket(connection, stream.GetBuffer(), (uint32_t)stream.GetTotalLength(), state);
	host.tunnel.Flush(connection);
	remoteObjectRefDelta.clear();
	localObjectRefDelta.clear();
}

void ZRemoteProxy::Request::ApplyDelta(std::map<IScript::Object*, ObjectInfo>& info, const std::vector<std::pair<uint64_t, long> >& delta, bool retrieve) {
	for (std::vector<std::pair<uint64_t, long> >::const_iterator it = delta.begin(); it != delta.end(); ++it) {
		const std::pair<uint64_t, long>& value = *it;
		// merge edition
		IScript::Object* object = reinterpret_cast<IScript::Object*>(value.first);
		std::map<IScript::Object*, ObjectInfo>::iterator p = info.find(object);
		if (p != info.end()) {
			if ((p->second.refCount += value.second) <= 0) {
				// destroy
				IScript::Object* object = reinterpret_cast<IScript::Object*>(value.first);
				info.erase(p);
				if (retrieve) { // local object
					object->ReleaseObject();
				}
			}
		} else {
			ObjectInfo& x = info[object];
			if (retrieve) { // local object
				MethodInspector ins(object, x);
				(*object)(ins);
			}

			x.refCount = value.second;
		}
	}
}

void ZRemoteProxy::Request::DoLock() {
	host.threadApi.DoLock(requestLock);
	lockCount++;
	// assert(lockCount == 1);
}

void ZRemoteProxy::Request::UnLock() {
	lockCount--;
	host.threadApi.UnLock(requestLock);
}

void ZRemoteProxy::Request::ProcessPacket(Packet& packet) {
	DoLock();
	ApplyDelta(localActiveObjects, packet.localDelta, true);
	ApplyDelta(remoteActiveObjects, packet.remoteDelta, false);
	packet.localDelta.clear();
	packet.remoteDelta.clear();

	// lookup wrapper
	IScript::Request::AutoWrapperBase* wrapper = nullptr;
	bool needFree = false;

	if (packet.response) {
		if (packet.deferred) {
			IScript::Request::AutoWrapperBase* temp = reinterpret_cast<IScript::Request::AutoWrapperBase*>(packet.callback);
			std::set<IScript::Request::AutoWrapperBase*>::iterator it = localCallbacks.find(temp);
			if (it != localCallbacks.end()) {
				wrapper = temp;
				localCallbacks.erase(it);
				needFree = true;
			}
		}
	} else {
		uint64_t proc = packet.procedure;
		if (packet.object == UNIQUE_GLOBAL) {
			if (proc < globalRoutines.collection.size()) {
				wrapper = globalRoutines.collection[(size_t)proc].wrapper;
			}
		} else {
			// retrieve object
			IScript::Object* object = reinterpret_cast<IScript::Object*>(packet.object);
			std::map<IScript::Object*, ObjectInfo>::iterator it = localActiveObjects.find(object);
			if (it != localActiveObjects.end()) {
				if (proc < it->second.collection.size()) {
					wrapper = it->second.collection[(size_t)proc].wrapper;
				}
			}
		}
	}

	// write values back
	if (!packet.deferred && (wrapper == nullptr || wrapper->IsSync())) {
		idx = (int)buffer.size();
		std::copy(packet.vars.begin(), packet.vars.end(), std::back_inserter(buffer));
		host.threadApi.Signal(syncCallSem, false);
	} else if (wrapper != nullptr) {
		assert(!wrapper->IsSync());
		int argCount = (int)packet.vars.size();
		// push vars
		int saveIdx = idx;
		int saveInitCount = initCount;
		int saveTableLevel = tableLevel;
		idx = initCount = tableLevel = 0;
		std::swap(packet.vars, buffer);
		// free lock
		UnLock();
		wrapper->Execute(*this);
		DoLock();

		// pop vars
		// write results back
		std::swap(packet.vars, buffer);
		std::vector<Variant> v;
		std::copy(packet.vars.begin() + argCount, packet.vars.end(), std::back_inserter(v));
		std::swap(packet.vars, v);

		idx = saveIdx;
		initCount = saveInitCount;
		tableLevel = saveTableLevel;

		// write back?
		if (!packet.response) {
			packet.response = true;
			PostPacket(packet);
		}
	}

	if (needFree) {
		delete wrapper;
	}

	UnLock();
}

void ZRemoteProxy::Request::Process() {
	ITunnel::PacketSizeType bufferLength = CHUNK_SIZE;
	String current;
	current.resize(bufferLength);

	while (host.tunnel.ReadConnectionPacket(connection, const_cast<char*>(current.data()), bufferLength, state)) {
		size_t len = bufferLength;
		if (!stream.WriteBlock(current.data(), len)) {
			stream.Clear();
			break;
		}

		if (state.cursor == state.header.length) {
			stream.Seek(IStreamBase::BEGIN, 0);
			Request::Packet packet;
			if (!(stream >> packet)) {
				return;
			}

			ProcessPacket(packet);
			stream.Clear();
		}

		bufferLength = CHUNK_SIZE;
	}
}

void ZRemoteProxy::Request::OnConnection(ITunnel::EVENT event) {
	// { CONNECTED, TIMEOUT, READ, WRITE, CLOSE, ERROR }
	switch (event) {
	case ITunnel::CONNECTED:
		if (statusHandler)
			statusHandler(*this, !manually, ZRemoteProxy::CONNECTED, "Connection established");
		break;
	case ITunnel::TIMEOUT:
		if (statusHandler)
			statusHandler(*this, !manually, ZRemoteProxy::TIMEOUT, "Connection timeout");
		break;
	case ITunnel::READ:
		// Handle for new call
		Process();
		break;
	case ITunnel::WRITE:
		break;
	case ITunnel::CLOSE:
	case ITunnel::ABORT:
		if (statusHandler) {
			if (event == ITunnel::CLOSE) {
				statusHandler(*this, !manually, ZRemoteProxy::CLOSED, "Connection closed");
			} else {
				statusHandler(*this, !manually, ZRemoteProxy::ABORTED, "Connection aborted");
			}
		}

		host.tunnel.CloseConnection(connection);
		connection = nullptr;
		if (!manually) {
			delete this;
		}
		break;
	}
}


ValueBase::ValueBase() {
}

ValueBase::~ValueBase() {
}

TObject<IReflect>& ZRemoteProxy::Request::operator () (IReflect& reflect) {
	ReflectClass(Request);
	if (reflect.IsReflectMethod()) {
		ReflectMethod(RequestCreateObject)[ScriptMethod = "CreateObject"];
		ReflectMethod(RequestQueryObject)[ScriptMethod = "QueryObject"];
	}

	return *this;
}

Variant::~Variant() {
	ReleaseObject();
}

Variant::Variant(const Variant& var) : value(nullptr) {
	*this = var;
}

Variant& Variant::operator = (const Variant& var) {
	if (&var != this) {
		// add reference
		size_t* ref2 = (size_t*)((char*)var.value - sizeof(size_t));
		assert(*ref2 < 0x100);
		if (value != nullptr) {
			ReleaseObject();
		}

		value = var.value;
		AddRef();
	}

	return *this;
}

// Request Apis

ZRemoteProxy::Request::Request(ZRemoteProxy& h, ITunnel::Connection* c, const TWrapper<void, IScript::Request&, bool, STATUS, const String&>& sh) : host(h), stream(CHUNK_SIZE, true), statusHandler(sh), idx(0), initCount(0), tableLevel(0), connection(c), manually(false), lockCount(0) {
	requestLock = host.threadApi.NewLock();
	syncCallSem = host.threadApi.NewEvent();
	MethodInspector inspector((IScript::Object*)UNIQUE_GLOBAL, globalRoutines);
	(*this)(inspector);
}

void ZRemoteProxy::Request::Reconnect() {
	String sip, dip;
	host.tunnel.GetConnectionInfo(connection, sip, dip);
	stream.Seek(IStreamBase::BEGIN, 0);

	ITunnel::Connection* c = host.tunnel.OpenConnection(host.dispatcher, Wrap(this, &Request::OnConnection), dip);
	if (c != nullptr) {
		std::swap(c, connection);
		host.tunnel.CloseConnection(c);
	}
}

ZRemoteProxy::Request::~Request() {
	if (manually && connection != nullptr) {
		host.tunnel.CloseConnection(connection);
	}

	for (std::map<IScript::Object*, ObjectInfo>::iterator it = localActiveObjects.begin(); it != localActiveObjects.end(); ++it) {
		(it->first)->ReleaseObject();
	}

	for (std::set<IScript::Request::AutoWrapperBase*>::iterator p = localCallbacks.begin(); p != localCallbacks.end(); ++p) {
		delete *p;
	}

	for (std::set<IScript::BaseDelegate*>::iterator q = localReferences.begin(); q != localReferences.end(); ++q) {
		delete *q;
	}

	for (std::set<IReflectObject*>::iterator t = tempObjects.begin(); t != tempObjects.end(); ++t) {
		delete *t;
	}

	if (!manually) {
		//host.activeRequests.erase(this);
	}

	host.threadApi.DeleteEvent(syncCallSem);
	host.threadApi.DeleteLock(requestLock);
}

IScript* ZRemoteProxy::Request::GetScript() {
	return &host;
}

int ZRemoteProxy::Request::GetCount() {
	return (int)buffer.size();
}

void ZRemoteProxy::Request::Debug(std::vector<StackInfo>& debugInfos, std::vector<LocalInfo>& localInfos, const Ref* func) {
	assert(false); // not implemented
}

IScript::Request::TYPE ZRemoteProxy::Request::GetCurrentType() {
	Variant& var = buffer[buffer.size() - 1];
	assert(false); // not implemented
	return NIL;
}

std::vector<IScript::Request::Key> ZRemoteProxy::Request::Enumerate() {
	return std::vector<IScript::Request::Key>(); // not allowed
}

static int IncreaseTableIndex(std::vector<Variant>& buffer, int count = 1) {
	Variant& var = buffer[buffer.size() - 1];
	static Unique intType = UniqueType<int>::Get();
	assert(var->QueryValueUnique() == intType);
	int& val = static_cast<Value<int>*>(var.Get())->value;

	if (count == 0) {
		val = 0;
	} else {
		val += count;
	}

	return val - 1;
}

template <class C>
inline void Write(ZRemoteProxy::Request& request, C& value) {
	std::vector<Variant>& buffer = request.buffer;
	int& tableLevel = request.tableLevel;
	int& idx = request.idx;
	String& key = request.key;

	if (tableLevel != 0) {
		Variant& arr = buffer[buffer.size() - 2];
		static Unique tableType = UniqueType<TableImpl>::Get();
		assert(arr.Get()->QueryValueUnique() == tableType);

		TableImpl& table = (static_cast<Value<TableImpl>*>(arr.Get()))->value;
		if (key.empty()) {
			int index = IncreaseTableIndex(buffer);
			table.arrayPart.push_back(Variant(value));
		} else {
			(*table.mapPart)[key] = Variant(value);
		}

		key = "";
	} else {
		buffer.push_back(Variant(value));
	}
}


IScript::Request& ZRemoteProxy::Request::operator << (const TableStart&) {
	assert(lockCount != 0);
	TableImpl impl;
	Variant var(impl);

	if (tableLevel != 0) {
		Variant& arr = buffer[buffer.size() - 2];
		static Unique tableType = UniqueType<TableImpl>::Get();
		assert(arr.Get()->QueryValueUnique() == tableType);

		TableImpl& table = (static_cast<Value<TableImpl>*>(arr.Get()))->value;
		if (key.empty()) {
			int index = IncreaseTableIndex(buffer);
			table.arrayPart.push_back(var);
		} else {
			(*table.mapPart)[key] = var;
		}

		key = "";
	} else {
		buffer.push_back(var);
	}

	key = "";
	// load table
	buffer.push_back(var);
	buffer.push_back(Variant((int)0));
	tableLevel++;
	return *this;
}

template <class C>
inline void Read(ZRemoteProxy::Request& request, C& value) {
	std::vector<Variant>& buffer = request.buffer;
	int& tableLevel = request.tableLevel;
	int& idx = request.idx;
	String& key = request.key;

	if (tableLevel != 0) {
		Variant& arr = buffer[buffer.size() - 2];
		static Unique tableType = UniqueType<TableImpl>::Get();
		assert(arr.Get()->QueryValueUnique() == tableType);

		TableImpl& table = (static_cast<Value<TableImpl>*>(arr.Get()))->value;
		if (key.empty()) {
			int index = IncreaseTableIndex(buffer);
			Variant* var = &table.arrayPart[index];
			value = static_cast<Value<C>*>(var->Get())->value;
		} else {
			std::map<String, Variant>::iterator it = table.mapPart->find(key);
			if (it != table.mapPart->end()) {
				value = static_cast<Value<C>*>(it->second.Get())->value;
			}
		}

		key = "";
	} else {
		if (idx < (int)buffer.size()) {
			Variant* var = &buffer[idx++];

			assert(var->Get()->QueryValueUnique() == UniqueType<C>::Get());
			if (var->Get()->QueryValueUnique() == UniqueType<C>::Get()) {
				value = static_cast<Value<C>*>(var->Get())->value;
			}
		}
	}
}



IScript::Request& ZRemoteProxy::Request::operator >> (TableStart& ts) {
	assert(lockCount != 0);

	if (tableLevel == 0) {
		if (idx == (int)buffer.size()) {
			// create empty table
			TableImpl impl;
			Variant var(impl);
			buffer.push_back(var);
		}

		buffer.push_back(buffer[idx++]);
	} else {
		Variant& arr = buffer[buffer.size() - 2];
		static Unique tableType = UniqueType<TableImpl>::Get();
		assert(arr.Get()->QueryValueUnique() == tableType);
		TableImpl& table = (static_cast<Value<TableImpl>*>(arr.Get()))->value;
		if (key.empty()) {
			int index = IncreaseTableIndex(buffer);
			buffer.push_back(table.arrayPart[index]);
		} else {
			std::map<String, Variant>::iterator it = table.mapPart->find(key);
			if (it != table.mapPart->end()) {
				buffer.push_back(it->second);
			} else {
				TableImpl impl;
				Variant var(impl);
				buffer.push_back(var);
			}
		}
	}

	key = "";
	buffer.push_back(Variant((int)0));
	Variant& v = buffer[buffer.size() - 2];
	static Unique tableType = UniqueType<TableImpl>::Get();
	assert(v.Get()->QueryValueUnique() == tableType);
	TableImpl& t = static_cast<Value<TableImpl>*>(v.Get())->value;
	ts.count = t.arrayPart.size();
	tableLevel++;

	return *this;
}

IScript::Request& ZRemoteProxy::Request::Push() {
	assert(lockCount != 0);
	assert(key.empty());
	buffer.push_back(Variant(idx));
	buffer.push_back(Variant(tableLevel));
	buffer.push_back(Variant(initCount));
	initCount = (int)buffer.size();
	idx = initCount;
	tableLevel = 0;

	return *this;
}

IScript::Request& ZRemoteProxy::Request::Pop() {
	assert(lockCount != 0);
	assert(key.empty());
	assert(tableLevel == 0);

	size_t org = initCount;

	initCount = (static_cast<Value<int>*>(buffer[org - 1].Get()))->value;
	tableLevel = (static_cast<Value<int>*>(buffer[org - 2].Get()))->value;
	idx = (static_cast<Value<int>*>(buffer[org - 3].Get()))->value;

	buffer.resize(org - 3);
	return *this;
}

void ZRemoteProxy::Request::RequestCreateObject(IScript::Request& request, String& url, String& mode) {
	IScript::BaseDelegate d(host.objectCreator(url));
	request.DoLock();
	request << d;
	request.UnLock();
}

void ZRemoteProxy::Request::RequestQueryObject(IScript::Request& request, IScript::BaseDelegate base) {
	// fetch object
	ObjectInfo& info = base.IsNative() ? localActiveObjects[base.ptr] : globalRoutines;
	request.DoLock();
	request << begintable;

	for (size_t i = 0; i < info.collection.size(); i++) {
		request << begintable;
		const ObjectInfo::Entry& entry = info.collection[i];
		request << IScript::Request::Key("Name") << entry.name;
		request << IScript::Request::Key("Arguments") << begintable;
		for (size_t j = 1; j < entry.params.size(); j++) {
			request << entry.params[j].type->typeName;
		}
		request << endtable;
		request << endtable;
	}

	request << endtable;
	request.UnLock();
}

IScript::Request::Ref ZRemoteProxy::Request::Load(const String& script, const String& pa) {
	// OK! return pseudo remote object proxy
	if (script == "Global") {
		IScript::BaseDelegate* d = new IScript::BaseDelegate((IScript::Object*)UNIQUE_GLOBAL);
		localReferences.insert(d);

		return IScript::Request::Ref((size_t)d);
	} else {
		return IScript::Request::Ref(0);
	}
}

IScript::Request& ZRemoteProxy::Request::operator << (const Nil& nil) {
	assert(lockCount != 0);
	Write(*this, nil);
	return *this;
}

IScript::Request& ZRemoteProxy::Request::operator << (const Global&) {
	assert(lockCount != 0);
	assert(false);
	/*
	buffer.push_back(Variant((IDispatch*)host.globalObject));
	buffer.push_back(Variant(0));
	tableLevel++;*/
	return *this;
}

IScript::Request& ZRemoteProxy::Request::operator << (const IScript::Request::Local &) {
	assert(false); // not implemented
	return *this;
}

IScript::Request& ZRemoteProxy::Request::operator << (const Key& k) {
	assert(lockCount != 0);
	assert(key.empty());
	key = k.GetKey();
	return *this;
}

IScript::Request& ZRemoteProxy::Request::operator >> (const Key& k) {
	assert(lockCount != 0);
	key = k.GetKey();
	return *this;
}

IScript::Request& ZRemoteProxy::Request::operator << (const LocalInfo& k) {
	assert(lockCount != 0);
	return *this;
}

IScript::Request& ZRemoteProxy::Request::operator >> (const LocalInfo& k) {
	assert(lockCount != 0);
	return *this;
}


IScript::Request& ZRemoteProxy::Request::operator << (double value) {
	assert(lockCount != 0);
	Write(*this, value);
	return *this;
}

IScript::Request& ZRemoteProxy::Request::operator >> (double& value) {
	assert(lockCount != 0);
	Read(*this, value);

	return *this;
}

IScript::Request& ZRemoteProxy::Request::operator << (const String& str) {
	assert(lockCount != 0);
	Write(*this, str);

	return *this;
}

IScript::Request& ZRemoteProxy::Request::operator >> (String& str) {
	assert(lockCount != 0);
	Read(*this, str);

	return *this;
}

IScript::Request& ZRemoteProxy::Request::operator << (const char* str) {
	assert(lockCount != 0);
	assert(str != nullptr);
	return *this << String(str);
}

IScript::Request& ZRemoteProxy::Request::operator >> (const char*& str) {
	assert(lockCount != 0);
	assert(false); // Not allowed
	return *this;
}

IScript::Request& ZRemoteProxy::Request::operator << (bool b) {
	assert(lockCount != 0);
	Write(*this, b);
	return *this;
}

IScript::Request& ZRemoteProxy::Request::operator >> (bool& b) {
	assert(lockCount != 0);
	Read(*this, b);
	return *this;
}

inline IScript::BaseDelegate Reverse(const IScript::BaseDelegate& value) {
	return IScript::BaseDelegate(value.ptr, !value.IsNative());
}

IScript::Request& ZRemoteProxy::Request::operator << (const BaseDelegate& value) {
	assert(lockCount != 0);
	IScript::BaseDelegate rev = Reverse(value);
	Write(*this, rev);

	return *this;
}

IScript::Request& ZRemoteProxy::Request::operator >> (BaseDelegate& value) {
	assert(lockCount != 0);
	value.ptr = nullptr;
	Read(*this, value);
	// value = Reverse(value);

	return *this;
}

IScript::Request& ZRemoteProxy::Request::operator << (const AutoWrapperBase& wrapper) {
	assert(lockCount != 0);
	assert(false); // not supported
	// const AutoWrapperBase* ptr = &wrapper;
	// Write(*this, ptr);
	return *this;
}

IScript::Request& ZRemoteProxy::Request::operator << (int64_t u) {
	assert(lockCount != 0);
	Write(*this, u);
	return *this;
}

IScript::Request& ZRemoteProxy::Request::operator >> (int64_t& u) {
	assert(lockCount != 0);
	Read(*this, u);

	return *this;
}

void ZRemoteProxy::Request::DebugPrint() {}

IScript::Request& ZRemoteProxy::Request::MoveVariables(IScript::Request& target, size_t count) {
	assert(lockCount != 0);
	assert(false); // not supported

	return *this;
}


IScript::Request& ZRemoteProxy::Request::operator << (const TableEnd&) {
	assert(lockCount != 0);
	buffer.resize(buffer.size() - 2);
	tableLevel--;
	return *this;
}

IScript::Request& ZRemoteProxy::Request::operator >> (const TableEnd&) {
	assert(lockCount != 0);
	buffer.resize(buffer.size() - 2);
	tableLevel--;

	return *this;
}

bool ZRemoteProxy::Request::IsValid(const BaseDelegate& d) {
	return d.ptr != nullptr;
}

IScript::Request& ZRemoteProxy::Request::operator >> (Ref& ref) {
	assert(lockCount != 0);
	BaseDelegate d;
	*this >> d;
	ref = MakeReference(d);
	return *this;
}

IScript::Request& ZRemoteProxy::Request::operator << (const Ref& ref) {
	assert(lockCount != 0);

	*this << *reinterpret_cast<BaseDelegate*>(ref.value);
	return *this;
}


bool ZRemoteProxy::Request::Call(const AutoWrapperBase& wrapper, const Request::Ref& ref) {
	assert(lockCount != 0);
	// parse function index
	if (buffer.empty())
		return false;

	const Variant& var = buffer[initCount];
	static Unique int64Type = UniqueType<int64_t>::Get();
	if (!(var.Get()->QueryValueUnique() == int64Type)) {
		return false;
	}

	// pack arguments
	const Variant& callProcIdx = buffer[initCount];
	if (!(callProcIdx.Get()->QueryValueUnique() == int64Type)) {
		return false;
	}

	BaseDelegate* d = reinterpret_cast<BaseDelegate*>(ref.value);
	assert(d == nullptr || (size_t)d->ptr < UNIQUE_END || !d->IsNative());
	Packet packet;
	packet.deferred = !wrapper.IsSync();
	packet.object = d == nullptr ? 0 : (size_t)d->ptr;
	packet.procedure = (uint64_t)(static_cast<Value<int64_t>*>(callProcIdx.Get()))->value;
	IScript::Request::AutoWrapperBase* cb = wrapper.Copy();
	packet.callback = (uint64_t)cb;
	std::vector<Variant>::iterator from = buffer.begin() + initCount + 1;
#if defined(_MSC_VER) && _MSC_VER <= 1200
	std::copy(from, buffer.end(), std::back_inserter(packet.vars));
#else
	std::move(from, buffer.end(), std::back_inserter(packet.vars));
#endif
	buffer.erase(from, buffer.end());
	localCallbacks.insert(cb);
	PostPacket(packet);

	if (wrapper.IsSync()) {
		// Must wait util return, timeout or error
		// free locks
		host.threadApi.Wait(syncCallSem, requestLock);
	}

	return true;
}

IScript::Request& ZRemoteProxy::Request::operator >> (const Skip& skip) {
	assert(lockCount != 0);
	if (tableLevel != 0) {
		if (key.empty()) {
			IncreaseTableIndex(buffer, skip.count);
		}
	} else {
		idx += skip.count;
	}

	return *this;
}

IScript::Request::Ref ZRemoteProxy::Request::ReferenceEx(const IScript::BaseDelegate* base) {
	IScript::Object* ptr = base->ptr;
	if (ptr != nullptr) {
		std::map<IScript::Object*, long>& delta = base->IsNative() ? localObjectRefDelta : remoteObjectRefDelta;
		std::map<IScript::Object*, ObjectInfo>& info = base->IsNative() ? localActiveObjects : remoteActiveObjects;
		info[ptr].refCount++;
		delta[ptr]++;
	}

	IScript::BaseDelegate* p = new IScript::BaseDelegate(*base);
	localReferences.insert(p);
	return (size_t)p;
}

void ZRemoteProxy::Request::DereferenceEx(IScript::BaseDelegate* base) {
	IScript::Object* ptr = base->ptr;
	if (ptr != nullptr) {
		std::map<IScript::Object*, long>& delta = base->IsNative() ? localObjectRefDelta : remoteObjectRefDelta;
		std::map<IScript::Object*, ObjectInfo>& info = base->IsNative() ? localActiveObjects : remoteActiveObjects;
		info[ptr].refCount--;
		delta[ptr]--;
	}

	localReferences.erase(base);
	delete base;
}

IScript::Request::Ref ZRemoteProxy::Request::ReferenceWithDebugInfo(const BaseDelegate& d, const String& tag, int line) {
	assert(lockCount != 0);
	return ReferenceEx(&d);
}

IScript::Request::Ref ZRemoteProxy::Request::ReferenceWithDebugInfo(const Ref& ref, const String& tag, int line) {
	assert(lockCount != 0);
	return ReferenceEx(reinterpret_cast<IScript::BaseDelegate*>(ref.value));
}

IScript::Request::TYPE ZRemoteProxy::Request::GetReferenceType(const Ref& d) {
	return IScript::Request::OBJECT;
}

void ZRemoteProxy::Request::Dereference(Ref& ref) {
	assert(lockCount != 0);
	DereferenceEx(reinterpret_cast<IScript::BaseDelegate*>(ref.value));
}

const char* ZRemoteProxy::GetFileExt() const {
	return "rpc";
}

class ReflectRoutines : public IReflect {
public:
	// input source
	ReflectRoutines(IScript::Request& request, const IScript::BaseDelegate& d, ZRemoteProxy::ObjectInfo& objInfo);
	virtual void ProcessProperty(IReflectObject& s, Unique typeID, const char* name, void* base, void* ptr, const MetaChainBase* meta);
	virtual void ProcessMethod(Unique typeID, const char* name, const TProxy<>* p, const Param& retValue, const std::vector<Param>& params, const MetaChainBase* meta);

private:
	std::map<String, TWrapper<std::pair<IScript::Request::Ref, size_t>, IScript::Request&, bool>*> mapNameToWrapper;
	ZRemoteProxy::ObjectInfo& objectInfo;
};

ReflectRoutines::ReflectRoutines(IScript::Request& request, const IScript::BaseDelegate& d, ZRemoteProxy::ObjectInfo& objInfo) : IReflect(true, false), objectInfo(objInfo) {
	// read request

	if (objectInfo.needQuery) {
		IScript::Request::TableStart ns;
		request >> ns;
		objectInfo.collection.resize(ns.count);

		for (size_t i = 0; i < ns.count; i++) {
			String name;
			request >> begintable;
			request >> IScript::Request::Key("Name") >> name;
			ZRemoteProxy::ObjectInfo::Entry& entry = objectInfo.collection[i];
			entry.index = i;
			entry.name = name;
			entry.obj = d;
			entry.method = Wrap(&entry, &ZRemoteProxy::ObjectInfo::Entry::CallFilter);
			mapNameToWrapper[name] = &entry.method;

			/*
			printf("Name: %s\n", name.c_str());
			IScript::Request::TableStart ts;
			request >> key("Arguments") >> ts;
			for (size_t j = 0; j < ts.count; j++) {
			request >> name;
			printf("\tArg[%d]: %s\n", (int)j, name.c_str());
			}*/
			request >> endtable;
		}
		request >> endtable;
		objectInfo.needQuery = false;
	} else {
		for (size_t i = 0; i < objectInfo.collection.size(); i++) {
			ZRemoteProxy::ObjectInfo::Entry& entry = objectInfo.collection[i];
			mapNameToWrapper[entry.name] = &entry.method;
		}
	}
}



void ReflectRoutines::ProcessProperty(IReflectObject& s, Unique typeID, const char* name, void* base, void* ptr, const MetaChainBase* meta) {
	if (s.IsBaseObject()) {
		static Unique wrapperType = UniqueType<IScript::MetaRemoteEntry>::Get();
		if (typeID.info->size == sizeof(TWrapper<void>)) {
			for (const MetaChainBase* p = meta; p != nullptr; p = p->GetNext()) {
				const MetaNodeBase* node = p->GetNode();
				if (!node->IsBaseObject() && node->GetUnique() == wrapperType) {
					const IScript::MetaRemoteEntry* wrapper = static_cast<const IScript::MetaRemoteEntry*>(node);
					TWrapper<void>& routineBase = *reinterpret_cast<TWrapper<void>*>(ptr);
					routineBase = wrapper->wrapper;
					TWrapper<std::pair<IScript::Request::Ref, size_t>, IScript::Request&, bool>* host = mapNameToWrapper[wrapper->name.empty() ? name : wrapper->name];
					routineBase.proxy.host = reinterpret_cast<IHost*>(host);
				}
			}
		}
	}
}

void ReflectRoutines::ProcessMethod(Unique typeID, const char* name, const TProxy<>* p, const Param& retValue, const std::vector<Param>& params, const MetaChainBase* meta) {}

class QueryInterfaceCallback : public IReflectObject {
public:
	QueryInterfaceCallback(const TWrapper<void, IScript::Request&, IReflectObject&, const IScript::Request::Ref&>& c, const IScript::BaseDelegate& d, IReflectObject& o, ZRemoteProxy::ObjectInfo& objInfo) : object(o), bd(d), callback(c), objectInfo(objInfo) {}
	void Invoke(IScript::Request& request) {
		
		ZRemoteProxy::Request& r = static_cast<ZRemoteProxy::Request&>(request);
		r.DoLock();
		r.QueryObjectInterface(objectInfo, bd, callback, object);
		r.UnLock();

		callback(request, object, ref);
		r.tempObjects.erase(this);
		delete this;
	}

	IReflectObject& object;
	IScript::BaseDelegate bd;
	TWrapper<void, IScript::Request&, IReflectObject&, const IScript::Request::Ref&> callback;
	ZRemoteProxy::ObjectInfo& objectInfo;
};

void ZRemoteProxy::Request::QueryObjectInterface(ObjectInfo& objectInfo, const IScript::BaseDelegate& d, const TWrapper<void, IScript::Request&, IReflectObject&, const Request::Ref&>& callback, IReflectObject& target) {
	ReflectRoutines reflect(*this, d, objectInfo);
	target(reflect);
}

void ZRemoteProxy::Request::QueryInterface(const TWrapper<void, IScript::Request&, IReflectObject&, const Request::Ref&>& callback, IReflectObject& target, const Request::Ref& g) {
	IScript::Request& request = *this;
	// Check if the object is already cached.
	IScript::BaseDelegate* d = reinterpret_cast<IScript::BaseDelegate*>(g.value);
	assert(d != nullptr);
	if (d->ptr == (IScript::Object*)UNIQUE_GLOBAL) {
		QueryObjectInterface(globalRoutines, *d, callback, target);
		callback(*this, target, g);
	} else if (!d->IsNative()) {
		ObjectInfo& objectInfo = remoteActiveObjects[d->ptr];
		if (objectInfo.needQuery) {
			QueryInterfaceCallback* cb = new QueryInterfaceCallback(callback, *d, target, objectInfo);
			tempObjects.insert(cb);
			IScript::BaseDelegate d((IScript::Object*)UNIQUE_GLOBAL);
			IScript::Request::Ref ref((size_t)&d);

			request.Push();
			request.Call(IScript::Request::Adapt(Wrap(cb, &QueryInterfaceCallback::Invoke)), ref, GLOBAL_INTERFACE_QUERY, g);
			request.Pop();
		} else {
			QueryObjectInterface(objectInfo, *d, callback, target);
		}
	}
}

void ZRemoteFactory::Initialize(IScript::Request& request, const TWrapper<void, IScript::Request&, IReflectObject&, const IScript::Request::Ref&>& callback) {
	globalRef = request.Load("Global", "Initialize");
	request.QueryInterface(callback, *this, globalRef);
}

TObject<IReflect>& ZRemoteFactory::operator () (IReflect& reflect) {
	ReflectClass(ZRemoteFactory);
	if (reflect.IsReflectProperty()) {
		ReflectProperty(CreateObject)[ScriptWrapper];
		ReflectProperty(QueryObject)[ScriptWrapper];
		ReflectProperty(globalRef);
	}

	return *this;
}

IScript* ZRemoteProxy::NewScript() const {
	return nullptr; // Not supported
}