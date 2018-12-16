#include "Remembery.h"

using namespace PaintsNow;
using namespace PaintsNow::NsRemembery;

Remembery::Remembery(IThread& threadApi, IDatabase& db) : IScript::Library(threadApi), databaseFactory(db) {}
Remembery::~Remembery() {}


TObject<IReflect>& Remembery::operator () (IReflect& reflect) {
	ReflectClass(Remembery)[Interface(IScript::Library)];
	if (reflect.IsReflectMethod()) {
		ReflectMethod(RequestCreateDatabase)[ScriptMethod = "CreateDatabase"];
		ReflectMethod(RequestExecute)[ScriptMethod = "Execute"];
		ReflectMethod(RequestStep)[ScriptMethod = "Step"];
	}

	return *this;
}


void Remembery::RequestCreateDatabase(IScript::Request& request, String& target, String& username, String& password) {
	METHOD_ENTRY(Remembery::RequestCreateDatabase);
	// try to create one
	IDatabase::Database* database = databaseFactory.Connect(target, username, password);
	if (database != nullptr) {
		TShared<Hive> hive = TShared<Hive>::Make(databaseFactory, database);
		request.DoLock();
		request << delegate(hive());
		request.UnLock();
	} else {
		request.Error("Remembery::CreateDatabase(target, username, password) : invalid target, username or password.");
	}
}

void Remembery::RequestExecute(IScript::Request& request, IScript::Delegate<Hive> hive, String& sql, HoneyData& honeyData) {
	METHOD_ENTRY(Remembery::RequestExecute);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(hive);

	TShared<Honey> honey = hive->Execute(sql, honeyData);
	if (honey) {
		request.DoLock();
		request << delegate(honey());
		request.UnLock();
	}
}

void Remembery::RequestStep(IScript::Request& request, IScript::Delegate<Honey> honey, uint32_t count) {
	METHOD_ENTRY(Remembery::RequestStep);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(honey);

	request.DoLock();
	for (uint32_t i = 0; i < count; i++) {
		if (!honey->Step()) break;
		honey->WriteLine(request);
	}
	request.UnLock();
}
