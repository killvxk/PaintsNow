#include "RPC.h"
#include "../../../Driver/Network/LibEvent/ZNetworkLibEvent.h"
#include "../../../Driver/Thread/Pthread/ZThreadPthread.h"
#include "../../../Driver/Network/LibEvent/ZNetworkLibEvent.h"
#ifdef _WIN32
#include "../../../Driver/Tunnel/WinPipe/ZWinPipe.h"
#endif
#include "../../../Core/Common/ZRemoteProxy.h"

using namespace PaintsNow;
using namespace PaintsNow::NsLostDream;

class Foo : public IScript::Library {
public:
	Foo(IThread& threadApi) : IScript::Library(threadApi) {}

	virtual TObject<IReflect>& operator () (IReflect& reflect) {
		ReflectClass(Foo)[Interface(IScript::Library)];
		if (reflect.IsReflectMethod()) {
			ReflectMethod(RequestNewFoo)[ScriptMethod = "NewFoo"];
		}

		return *this;
	}


	void RequestNewFoo(IScript::Request& request, int a, String& str) {

	}
};


class PrefabFactory : public TFactoryBase<IScript::Object> {
public:
	PrefabFactory(IThread& threadApi) : TFactoryBase<IScript::Object>(Wrap(this, &PrefabFactory::CreateObject)), thread(threadApi) {}
	class Prefab : public IScript::Object {
	public:
		virtual TObject<IReflect>& operator () (IReflect& reflect) {
			ReflectClass(Prefab);
			if (reflect.IsReflectMethod()) {
				ReflectMethod(Test)[ScriptMethod];
			}

			return *this;
		}

		void Test(IScript::Request& request, String& message, int value) {
			printf("Message: %s : %d\n", message.c_str(), value);
		}

		virtual void ScriptUninitialize(IScript::Request& request) {}
	};

	IScript::Object* CreateObject(const String& entry) const {
		printf("Hello %s \n", entry.c_str());
		return new Prefab();
	}

	IThread& thread;
};

void Then(IScript::Request& request) {
	IScript::Request::TableStart ns;
	request >> ns;
	for (size_t i = 0; i < ns.count; i++) {
		String name;
		request >> begintable;
		request >> key("Name") >> name;
		printf("Name: %s\n", name.c_str());
		IScript::Request::TableStart ts;
		request >> key("Arguments") >> ts;
		for (size_t j = 0; j < ts.count; j++) {
			request >> name;
			printf("\tArg[%d]: %s\n", (int)j, name.c_str());
		}
		request >> endtable;
		request >> endtable;
	}
	request >> endtable;
}

void After(IScript::Request& request, IScript::Request::Ref prefab) {
	request.DoLock();
	request.Push();
	request.Call(Sync, prefab, 0, "abcasdfwhaodkjfo;asld;asdgihaosdfi");
	request.Pop();
	request.UnLock();
}


struct MyPrefab : public IReflectObjectComplex {
public:
	virtual TObject<IReflect>& operator () (IReflect& reflect) {
		ReflectClass(MyPrefab);
		if (reflect.IsReflectProperty()) {
			ReflectProperty(Test)[ScriptWrapper];
		}
		return *this;
	}
	TWrapper<void, IScript::Request&, const IScript::Request::AutoWrapperBase&, const String&, int> Test;
} myPrefab;


void OnFinish(IScript::Request& request) {
	printf("FINISH!\n");
}

void OnQueryPrefab(IScript::Request& request, IReflectObject& inter, const IScript::Request::Ref& ref) {
	assert(&myPrefab == &inter);
	String s = "Hello, World";

	request.DoLock();
	request.Push();
	myPrefab.Test(request, request.Adapt(Wrap(OnFinish)), s, 1234);
	request.Pop();
	request.UnLock();

	/*
	request.DoLock();
	request.Push();
	request.Call(Sync, ref, 0, "abcasdfwhaodkjfo;asld;asdgihaosdfi");
	request.Pop();
	request.UnLock();*/
}

void OnQuery(IScript::Request& request, IReflectObject& inter, const IScript::Request::Ref& ref) {
	ZRemoteFactory& testInterface = static_cast<ZRemoteFactory&>(inter);
	IScript::Request::Ref prefab;
	request.Push();

	// You can choose either sync or async call.
	// testInterface.CreateObject(request, IScript::Request::Adapt(Wrap(After)), "Prefab");
	testInterface.CreateObject(request, Sync, "Prefab");
	request >> prefab;

	request.QueryInterface(Wrap(OnQueryPrefab), myPrefab, prefab);
	request.Pop();

	/*
	if (prefab) {
	request.Push();
	request.Call(Sync, prefab, 0, "abcasdfwhaodkjfo;asld;asdgihaosdfi");
	request.Pop();
	}*/
}

void OnConnect(IScript::Request& request, bool isServer, ZRemoteProxy::STATUS status, const String& info) {
	// sync call
	printf("Connecting ... %d, %d\n", isServer, status);
}

void TestRPC() {
}

bool RPC::Initialize() {
	return true;
}

bool RPC::Run(int randomSeed, int length) {
	ZThreadPthread uniqueThreadApi;
	Foo foo(uniqueThreadApi);
	IScript::Library* module = foo.QueryInterface(UniqueType<IScript::Library>());
	IScript::Object* object = foo.QueryInterface(UniqueType<IScript::Object>());

	ZNetworkLibEvent network(uniqueThreadApi);
#ifdef _WIN32
	ZWinPipe winPipe(1, 4096);
	ITunnel& tunnel = winPipe;
#else
	ITunnel& tunnel = network;
#endif

	const PrefabFactory factory(uniqueThreadApi);

	ZRemoteProxy serverProxy(uniqueThreadApi, tunnel, factory, "127.0.0.1:16384", Wrap(OnConnect));
	serverProxy.Run();

	for (int i = 0; i < length; i++) {
		ZRemoteProxy clientProxy(uniqueThreadApi, tunnel, factory, "127.0.0.1:16385");
		clientProxy.Run();
		// async create request
		IScript::Request* r = clientProxy.NewRequest("127.0.0.1:16384");
		if (r == nullptr) {
			printf("Unable to connect RPC server. Stop.\n");
			break;
		}

		IScript::Request& request = *r;
		request.DoLock();
		request.Push();
		IScript::Request::Ref global = request.Load("Global", "Initialize");
		ZRemoteFactory testInterface;
		request.QueryInterface(Wrap(OnQuery), testInterface, global);
		// parse result
		request.Pop();
		request.UnLock();
		getchar();

		// Close connection
		r->ReleaseObject();
	}

	return true;
}

void RPC::Summary() {}

TObject<IReflect>& RPC::operator () (IReflect& reflect) {
	ReflectClass(RPC);

	return *this;
}