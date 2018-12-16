#include "Serialization.h"
#include "../../../Core/Common/ZMemoryStream.h"
using namespace PaintsNow;
using namespace PaintsNow::NsLostDream;

struct MyType : public IReflectObjectComplex {
	virtual TObject<IReflect>& operator () (IReflect& reflect) {
		ReflectClass(MyType);
		if (reflect.IsReflectProperty()) {
			ReflectProperty(hi);
		}

		return *this;
	}

	String hi;
};

struct MarshallInfo : public IReflectObjectComplex {
	MarshallInfo() : dynType(nullptr) {}
	virtual ~MarshallInfo() {
		if (dynType != nullptr) {
			dynType->ReleaseObject();
		}

		for (size_t i = 0; i < dynVector.size(); i++) {
			IReflectObjectComplex* c = dynVector[i];
			if (c != nullptr) {
				c->ReleaseObject();
			}
		}
	}
	virtual TObject<IReflect>& operator () (IReflect& reflect) {
		ReflectClass(MarshallInfo);
		if (reflect.IsReflectProperty()) {
			ReflectProperty(name);
			ReflectProperty(value);
			ReflectProperty(test);
			ReflectProperty(mytype);
			ReflectProperty(mytype2);
			ReflectProperty(IterateVector(arr));
			ReflectProperty(IterateVector(arrmytype));
			ReflectProperty(dynType);
			ReflectProperty(IterateVector(dynVector));
		}

		return *this;
	}
	String name;
	int value;
	double test;
	MyType mytype;
	MyType mytype2;
	IReflectObjectComplex* dynType;
	std::vector<int> arr;
	std::vector<MyType> arrmytype;
	std::vector<IReflectObjectComplex*> dynVector;
};

struct Another : public IReflectObjectComplex {
	virtual TObject<IReflect>& operator () (IReflect& reflect) {
		ReflectClass(Another);
		if (reflect.IsReflectProperty()) {
			ReflectProperty(magic);
		}

		return *this;
	}

	short magic;
};

struct Derived : public MarshallInfo {
public:
	virtual TObject<IReflect>& operator () (IReflect& reflect) {
		ReflectClass(Derived)[Interface(MarshallInfo)];
		if (reflect.IsReflectProperty()) {
			ReflectProperty(ch);
			ReflectProperty(extra);
		}

		return *this;
	}

	char ch;
	String extra;
};


#include "../../../Driver/Filter/Pod/ZFilterPod.h"

bool Serialization::Initialize() {
	Creatable<MyType>::Init();
	return true;
}

bool Serialization::Run(int randomSeed, int length) {
	MarshallInfo info;
	info.name = "HAHA";
	info.value = 123;
	info.test = 0.5;
	info.mytype.hi = "Hello, world!";
	info.mytype2.hi = "ALOHA";
	info.arr.push_back(1);
	info.arr.push_back(3);
	MyType s;
	s.hi = "In Container";

	MyType t;
	t.hi = "ArrString";
	info.arrmytype.push_back(t);
	info.arrmytype.push_back(t);
	MyType* d = new MyType();
	d->hi = "Dynamic!!";
	info.dynType = d;
	MyType* vec = new MyType();
	vec->hi = "Vectorized~!";
	info.dynVector.push_back(vec);

	Derived derived;
	derived.name = "Derived";
	derived.value = 456;
	derived.test = 0.3;
	derived.mytype.hi = "MyTYPE";
	derived.mytype2.hi = "HI2";
	derived.ch = 'a';
	derived.extra = "Extra";

	ZMemoryStream stream(0x1000, true);
	ZFilterPod pod;
	IStreamBase* filter = pod.CreateFilter(stream);
	// NoFilter filter;
	*filter << info;
	*filter << derived;
	filter->ReleaseObject();

	MarshallInfo target;
	Derived targetDerived;
	String str;
	stream.Seek(IStreamBase::BEGIN, 0);
	IStreamBase* filterAgain = pod.CreateFilter(stream);
	*filterAgain >> target;
	*filterAgain >> targetDerived;
	filterAgain->ReleaseObject();

	return true;
}

void Serialization::Summary() {

}

TObject<IReflect>& Serialization::operator () (IReflect& reflect) {
	ReflectClass(Serialization);

	return *this;
}