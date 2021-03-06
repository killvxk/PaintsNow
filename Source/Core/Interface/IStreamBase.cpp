#include "IStreamBase.h"

using namespace PaintsNow;

const int MAX_INTEGER_SIZE = 64;


inline bool WriteBaseObject(IStreamBase& stream, Unique typeID, void* ptr, size_t size) {
	static Unique strType = UniqueType<String>::Get();
	static Unique pointerType = UniqueType<IReflectObjectComplex*>::Get();
	bool result = true;
	if (typeID == pointerType) {
		IReflectObjectComplex*& object = *reinterpret_cast<IReflectObjectComplex**>(ptr);
		stream << ((object != nullptr) ? object->GetUnique()->typeName : "");
		if (object != nullptr) {
			return *object >> stream;
		} else {
			return true;
		}
	} else if (typeID == strType) {
		String* str = reinterpret_cast<String*>(ptr);
		if (stream << (uint64_t)str->size()) {
			if (!str->empty()) {
				size_t s = str->size();
				if (!stream.WriteBlock(str->data(), s)) {
					result = false;
				}
			}
		} else {
			result = false;
		}
	} else {
		if (IsStdEndian() || !typeID.info->IsInteger()) {
			if (!stream.WriteBlock(ptr, size)) {
				result = false;
			}
		} else {
			char buffer[MAX_INTEGER_SIZE];
			assert(size < MAX_INTEGER_SIZE);
			for (size_t i = 0; i < size; i++) {
				buffer[i] = ((char*)ptr)[size - i - 1];
			}

			if (!stream.WriteBlock(buffer, size)) {
				result = false;
			}
		}
	}

	return result;
}

inline bool ReadBaseObject(IStreamBase& stream, Unique typeID, void* ptr, size_t size) {
	static Unique strType = UniqueType<String>::Get();
	static Unique pointerType = UniqueType<IReflectObjectComplex*>::Get();
	bool result = true; 
	if (typeID == pointerType) {
		IReflectObjectComplex*& object = *reinterpret_cast<IReflectObjectComplex**>(ptr);
		String typeName;
		stream >> typeName;
		if (typeName.size() != 0) {
			IUniqueInfo* info = GetGlobalUniqueAllocator()->Get(typeName.c_str());
			assert(info != nullptr);
			if (info != nullptr) {
				object = static_cast<IReflectObjectComplex*>(info->Create());
				assert(!object->IsBaseObject());
				*object << stream;
				return true;
			} else {
				return false; // object not found
			}
		} else {
			return true;
		}
	} else if (typeID == strType) {
		uint64_t length;
		if (stream >> length) {
			String* str = reinterpret_cast<String*>(ptr);
			if (length < (size_t)-1) {
				str->resize((size_t)length);
				if (length != 0) {
					size_t s = (size_t)length;
					if (!stream.ReadBlock(const_cast<char*>(str->data()), s)) {
						result = false;
					}
				}
			} else {
				result = false;
			}
		} else {
			result = false;
		}
	} else {
		if (!stream.ReadBlock(ptr, size)) {
			typeID.info->Endianize(ptr);
			result = false;
		}
	}

	return result;
}


// this class is not mult-thread safe
template <bool read>
class Reflect : public IReflect {
public:
	Reflect(IStreamBase& s) : IReflect(true, false), stream(s), result(true) {}
	virtual void ProcessProperty(IReflectObject& s, Unique typeID, const char* name, void* base, void* ptr, const MetaChainBase* meta) {
		if (!result)
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

		size_t size = typeID->size;
		if (s.IsBaseObject()) {
			if (read) {
				if (!ReadBaseObject(stream, typeID, ptr, size)) {
					result = false;
				}
			} else {
				if (!WriteBaseObject(stream, typeID, ptr, size)) {
					result = false;
				}
			}
		} else if (s.IsIterator()) {
			IIterator& it = static_cast<IIterator&>(s);
			uint64_t count;
			if (read) {
				if (stream >> count) {
					it.Initialize((size_t)count);
				} else {
					result = false;
					return;
				}
			} else {
				count = (uint64_t)it.GetTotalCount();
				if (!(stream << count)) {
					result = false;
					return;
				}
			}

			Unique type = it.GetPrototypeUnique();
			Unique uniquePointer = UniqueType<IReflectObjectComplex*>::Get();
			bool isPointer = type == uniquePointer;
			bool isInteger = type->IsInteger();

			if (it.GetPrototype().IsBaseObject()) {
				while (it.Next()) {
					void* ptr = it.Get();
					size_t length = type->GetSize();
					if (!isPointer && (IsStdEndian() || !isInteger)) {
						length *= (size_t)count;
						if (read) {
							if (!stream.ReadBlock(ptr, length)) {
								result = false;
								return;
							}
						} else {
							if (!stream.WriteBlock(ptr, length)) {
								result = false;
								return;
							}
						}

						break; // fast fast fast
					} else {
						// the same as object(*this); but more faster.
						if (read) {
							if (!ReadBaseObject(stream, type, ptr, length)) {
								result = false;
							}
						} else {
							if (!WriteBaseObject(stream, type, ptr, length)) {
								result = false;
							}
						}
					}
				}
			} else {
				while (it.Next()) {
					(*reinterpret_cast<IReflectObject*>(it.Get()))(*this);
				}
			}
		} else {
			s(*this); // continue to reflect sub fields
		}
	}
	virtual void ProcessMethod(Unique typeID, const char* name, const TProxy<>* p, const IReflect::Param& retValue, const std::vector<IReflect::Param>& params, const MetaChainBase* meta) {}
	
	bool GetResult() const { return result; }

private:
	IStreamBase& stream;
	bool result;
};

IStreamBase::~IStreamBase() {}


bool IStreamBase::WriteForward(const IReflectObject& a, Unique type, void* ptr, size_t length) {
	if (a.IsBaseObject()) {
		return WriteBaseObject(*this, type, ptr, length);
	} else {
		return const_cast<IReflectObject&>(a) >> *this;
	}
}

bool IStreamBase::ReadForward(const IReflectObject& a, Unique type, void* ptr, size_t length) {
	if (a.IsBaseObject()) {
		return ReadBaseObject(*this, type, ptr, length);
	} else {
		return const_cast<IReflectObject&>(a) << *this;
	}
}

bool IStreamBase::Write(IReflectObject& a, Unique type, void* ptr, size_t length) {
	assert(!a.IsBaseObject());
	Reflect<false> reflect(*this);
	reflect.ProcessProperty(a, type, "(nullptr)", ptr, ptr, nullptr);
	return reflect.GetResult();
}

bool IStreamBase::Read(IReflectObject& a, Unique type, void* ptr, size_t length) {
	assert(!a.IsBaseObject());
	Reflect<true> reflect(*this);
	reflect.ProcessProperty(a, type, "(nullptr)", ptr, ptr, nullptr);
	return reflect.GetResult();
}