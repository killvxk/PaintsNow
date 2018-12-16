#include "ZDatabaseSqlite.h"
#include "../../../Core/Common/ZDynamicObject.h"

using namespace PaintsNow;

// MetaData

class DatabaseSqliteImpl : public IDatabase::Database {
public:
	sqlite3* handle;
	ZDynamicUniqueAllocator uniqueAllocator;
};

ZDatabaseSqlite::ZDatabaseSqlite() {}
ZDatabaseSqlite::~ZDatabaseSqlite() {}

IDatabase::Database* ZDatabaseSqlite::Connect(const String& target, const String& username, const String& password) {
	sqlite3* handle;
	if (SQLITE_OK == sqlite3_open(target.c_str(), &handle)) {
		DatabaseSqliteImpl* impl = new DatabaseSqliteImpl();
		impl->handle = handle;
		return impl;
	} else {
		return nullptr;
	}
}

void ZDatabaseSqlite::Close(Database* database) {
	DatabaseSqliteImpl* impl = static_cast<DatabaseSqliteImpl*>(database);
	if (impl->handle != nullptr) {
		sqlite3_close(impl->handle);
		delete impl;
	}
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

class QueryMetaData : public IDatabase::MetaData {
public:
	QueryMetaData(ZDynamicUniqueAllocator& allocator, sqlite3* h, sqlite3_stmt* s) : uniqueAllocator(allocator), handle(h), dynamicObject(nullptr), stmt(s), count(0), inited(true) {
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			count = sqlite3_column_count(stmt);
			std::vector<ZDynamicInfo::Field> fields(count);

			for (int i = 0; i < count; i++) {
				const char* name = sqlite3_column_name(stmt, i);
				if (name == nullptr) name = "";
				int t = sqlite3_column_type(stmt, i);

				ZDynamicInfo::Field& field = fields[i];
				field.name = name;

				static ZDynamicInfo::MemController mc = {
					StringCreator, StringDeletor, StringAssigner
				};

				switch (t) {
				case SQLITE_INTEGER:
					field.type = UniqueType<int>::Get();
					sets.push_back(&QueryMetaData::SetValueInt);
					break;
				case SQLITE_FLOAT:
					field.type = UniqueType<double>::Get();
					sets.push_back(&QueryMetaData::SetValueFloat);
					break;
				case SQLITE_TEXT:
					field.type = UniqueType<String>::Get();
					field.controller = &mc;
					sets.push_back(&QueryMetaData::SetValueText);
					break;
				}
			}

			Unique unique = uniqueAllocator.AllocFromDescriptor("QueryMetaDataInstance", fields);
			dynamicObject = static_cast<ZDynamicObject*>(unique->Create());
		}
	}

	void SetValueText(int i) {
		String s = (const char*)sqlite3_column_text(stmt, i);
		dynamicObject->Set(dynamicObject->GetDynamicInfo()->fields[i], &s);
	}

	void SetValueFloat(int i) {
		double value = sqlite3_column_double(stmt, i);
		dynamicObject->Set(dynamicObject->GetDynamicInfo()->fields[i], &value);
	}

	void SetValueInt(int i) {
		int value = sqlite3_column_int(stmt, i);
		dynamicObject->Set(dynamicObject->GetDynamicInfo()->fields[i], &value);;
	}

	virtual ~QueryMetaData() {
		if (dynamicObject != nullptr)
			dynamicObject->ReleaseObject();
		sqlite3_finalize(stmt);
	}

	virtual IIterator* New() const {
		assert(false);
		return nullptr;
	}

	virtual void Attach(void* base) {
		assert(false);
	}

	virtual void Initialize(size_t count) {
		assert(false);
	}

	virtual Unique GetPrototypeUnique() const {
		assert(false);
		return Unique();
	}

	virtual size_t GetTotalCount() const {
		assert(false);
		return 0;
	}

	virtual void* GetHost() const {
		return nullptr;
	}

	virtual void* Get() {
		return dynamicObject;
	}

	virtual const IReflectObject& GetPrototype() const {
		return *dynamicObject;
	}

	virtual bool Next() {
		if (count == 0) return false;

		if (inited || sqlite3_step(stmt) == SQLITE_ROW) {
			inited = false;
			for (int i = 0; i < count; i++) {
				(this->*sets[i])(i);
			}

			return true;
		} else {
			return false;
		}
	}

	virtual bool IsLayoutLinear() const {
		return false;
	}

	virtual bool IsLayoutPinned() const {
		return true;
	}

private:
	ZDynamicUniqueAllocator& uniqueAllocator;
	sqlite3* handle;
	sqlite3_stmt* stmt;
	typedef void (QueryMetaData::*Set)(int i);
	std::vector<Set> sets;
	ZDynamicObject* dynamicObject;

	int count;
	bool inited;
};

class MapperSqlite : public IReflect {
public:
	MapperSqlite(sqlite3_stmt* s, const std::vector<String>& n) : IReflect(true, false), stmt(s), names(n), counter(0) {}
	virtual void ProcessProperty(IReflectObject& s, Unique typeID, const char* name, void* base, void* ptr, const MetaChainBase* meta) {
		static Unique intType = UniqueType<int>::Get();
		static Unique strType = UniqueType<String>::Get();
		static Unique cstrType = UniqueType<const char*>::Get();
		static Unique doubleType = UniqueType<double>::Get();

		size_t offset = (const char*)ptr - (const char*)base;
		if (typeID == intType) {
			setters.push_back(std::make_pair(&MapperSqlite::SetValueInt, offset));
		} else if (typeID == strType) {
			setters.push_back(std::make_pair(&MapperSqlite::SetValueString, offset));
		} else if (typeID == cstrType) {
			setters.push_back(std::make_pair(&MapperSqlite::SetValueText, offset));
		} else if (typeID == doubleType) {
			setters.push_back(std::make_pair(&MapperSqlite::SetValueFloat, offset));
		} else {
			assert(false);
		}

		counter++;
	}
	virtual void ProcessMethod(Unique typeID, const char* name, const TProxy<>* p, const Param& retValue, const std::vector<Param>& params, const MetaChainBase* meta) {}

	void SetValueString(int i, const void* base) {
		sqlite3_bind_text(stmt, i, ((const String*)base)->c_str(), (int)((const String*)base)->length(), SQLITE_TRANSIENT);
	}

	void SetValueText(int i, const void* base) {
		sqlite3_bind_text(stmt, i, (const char*)base, (int)strlen((const char*)base), SQLITE_TRANSIENT);
	}

	void SetValueFloat(int i, const void* base) {
		sqlite3_bind_double(stmt, i, *(const double*)base);
	}

	void SetValueInt(int i, const void* base) {
		sqlite3_bind_int(stmt, i, *(const int*)base);
	}

	sqlite3_stmt* stmt;
	const std::vector<String>& names;
	typedef void (MapperSqlite::*Set)(int i, const void* base);
	std::vector<std::pair<Set, size_t> > setters;
	int counter;
};

class WriterSqlite : public IReflect {
public:
	WriterSqlite(MapperSqlite& r) : IReflect(true, false), reflect(r), i(0) {}

	virtual void ProcessProperty(IReflectObject& object, Unique typeID, const char* name, void* base, void* ptr, const MetaChainBase* meta) {
		MapperSqlite::Set s = reflect.setters[i].first;
		(reflect.*s)(i, ptr);
		i++;
	}
	virtual void ProcessMethod(Unique typeID, const char* name, const TProxy<>* p, const Param& retValue, const std::vector<Param>& params, const MetaChainBase* meta) {}


private:
	MapperSqlite& reflect;
	int i;
};

IDatabase::MetaData* ZDatabaseSqlite::Execute(Database* database, const String& statementTemplate, IDatabase::MetaData* postData) {
	DatabaseSqliteImpl* impl = static_cast<DatabaseSqliteImpl*>(database);
	sqlite3_stmt* stmt;
	// replace all $() variables statements with ? and records the order
	String converted = statementTemplate;
	bool instr = false;
	bool instrDouble = false;
	bool inVar = false;
	String varname;
	std::vector<String> names;

	size_t j = 0;
	for (size_t i = 0; i < converted.size(); i++) {
		bool skip = false;
		switch (converted[i]) {
		case '\'':
			instr = !instr;
			break;
		case '"':
			instrDouble = !instrDouble;
			break;
		case '{':
			varname = "";
			inVar = true;
			skip = true;
			break;
		case '}':
			names.push_back(varname);
			inVar = false;
			skip = true;
			converted[j++] = '?';
			break;
		default:
			if (!instr && !instrDouble && inVar) {
				varname += converted[i];
				skip = true;
			}
			break;
		}

		if (!skip) {
			converted[j++] = converted[i];
		}
	}

	converted[j++] = '\0';

	if (sqlite3_prepare_v2(impl->handle, converted.c_str(), -1, &stmt, 0) != SQLITE_OK) {
		fprintf(stderr, "\nerror %s\n", sqlite3_errmsg(impl->handle));
		return nullptr;
	} else {
		if (postData != nullptr) {
			// bind data
			const IReflectObject& prototype = postData->GetPrototype();
			MapperSqlite reflect(stmt, names);
			prototype(reflect);

			if (postData->IsLayoutPinned()) {
				std::vector<std::pair<MapperSqlite::Set, size_t> >& setters = reflect.setters;
				while (postData->Next()) {
					// Pinned data, fast write
					for (size_t i = 0; i < setters.size(); i++) {
						MapperSqlite::Set s = setters[i].first;
						(reflect.*s)((int)i, (const char*)&prototype + setters[i].second);
					}
				}
			} else {
				assert(!postData->GetPrototype().IsBaseObject());
				while (postData->Next()) {
					WriterSqlite writer(reflect);
					(*reinterpret_cast<IReflectObject*>(postData->Get()))(writer);
				}
			}
		}

		return new QueryMetaData(impl->uniqueAllocator, impl->handle, stmt);
	}
}
