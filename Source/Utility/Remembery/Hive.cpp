#include "Hive.h"

using namespace PaintsNow;
using namespace PaintsNow::NsRemembery;

Hive::Hive(IDatabase& db, IDatabase::Database* dbInst) : base(db), database(dbInst) {}

Hive::~Hive() {
	base.Close(database);
}

TShared<Honey> Hive::Execute(const String& sql, HoneyData& honey) {
	IDatabase::MetaData* meta = base.Execute(database, sql, &honey);
	return meta == nullptr ? nullptr : TShared<Honey>::Make(meta);
}

TObject<IReflect>& Hive::operator () (IReflect& reflect) {
	ReflectClass(Hive);
	return *this;
}