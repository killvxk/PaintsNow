#include "Annotation.h"

using namespace PaintsNow;
using namespace NsLostDream;

class Inner : public IReflectObjectComplex {
public:
	Inner() {}
	virtual ~Inner() {}

	virtual TObject<IReflect>& operator () (IReflect& reflect) {
		ReflectClass(Inner)[Constructable];
		if (reflect.IsReflectProperty()) {
			ReflectProperty(info);
			ReflectProperty(value);
			ReflectProperty(db);
		}

		return *this;
	}

	String info;
	int value;
	double db;
};

class InsightMe : public IReflectObjectComplex {
public:
	InsightMe(int x = 0, char h = 0, char p = 0) : a(x), c(h) {
		memset(array, p, sizeof(array));
	}

	virtual TObject<IReflect>& operator () (IReflect& reflect) {
		ReflectClass(InsightMe)[Constructable];
		if (reflect.IsReflectProperty()) {
			ReflectProperty(a);
			ReflectProperty(c)[Note = "Test"];
			ReflectProperty(IterateArray(array));
			ReflectProperty(text);
			ReflectProperty(inner);
			ReflectProperty(IterateArray(arrInner));
			ReflectProperty(IterateVector(vecValues));
			ReflectProperty(IterateVector(vecInner));
		}

		if (reflect.IsReflectMethod()) {
			ReflectMethod(func)[Note = "Hello"][Note = "Abc"];
		}

		return *this;
	}

	void func(int a, char& b, short* c, uint32_t, double d) {}

	int a;
	char c;
	short array[4];
	String text;
	Inner inner;
	std::vector<int> vecValues;
	std::vector<Inner> vecInner;
	Inner arrInner[10];
};

class OldInsightMe : public IReflectObjectComplex {
public:
	OldInsightMe(int x = 0, char h = 0, char p = 0) : a(x), c(h) {
		memset(array, p, sizeof(array));
	}

	TObject<IReflect>& operator () (IReflect& reflect) {
		ReflectClass(OldInsightMe)[Constructable];
		if (reflect.IsReflectProperty()) {
			ReflectProperty(a);
			ReflectProperty(c)[Note = "Test"];
			ReflectProperty(IterateArray(array));
			ReflectProperty(inner);


			ReflectProperty(IterateArray(arrInner));
			ReflectProperty(IterateVector(vecValues));
			ReflectProperty(IterateVector(vecInner));
		}

		if (reflect.IsReflectMethod()) {
			ReflectMethod(func)[Note = "Hello"][Note = "Abc"];
		}

		return *this;
	}

	void func(int a, char& b, short* c, uint32_t, double d) {}
private:
	int a;
	char c;
	short array[4];
	Inner inner;

	std::vector<int> vecValues;
	std::vector<Inner> vecInner;
	Inner arrInner[10];
};

void TestFunc(int a, String b) {
	//	int d = 0;
}

int TestFunc2(char a, float b) {
	//	int c = 0;
	return 0;
}

struct Clos {
	Clos() : state("hahahahah") {}
	~Clos() {
		printf("State: %s\n", state.c_str());

	}
	void TestFunc3(int c, String d) {
		printf("State: %s\n", state.c_str());
	}

	String state;
};

static void Test() {
#if !defined(_MSC_VER) || _MSC_VER > 1200
	TWrapper<int, int> p = WrapClosure([](int a)->int {
		printf("%d\n", a);
		return a;
	});

	int m = p(1);
	TWrapper<int, int> k = WrapClosure([m](int a)->int {
		printf("%d\n", a + m);
		return a - m;
	});

	k(2);
#endif

	TWrapper<void, int, String> func = TestFunc;
	TWrapper<int, char, float> func2 = TestFunc2;
	Clos* cl = new Clos();
	TWrapper<void, int, String> cc = WrapClosure(cl, &Clos::TestFunc3);
	delete cl;
	cc(2, "haha");

	InsightMe me(1, 1, 1);
	me.vecValues.push_back(1314);
	Inner inner;
	inner.info = "test";
	me.vecInner.push_back(inner);
	me.vecValues.push_back(333);
	me.arrInner[1] = inner;
	me.inner.value = 54321;
	me.inner.db = 123.0;
	me.inner.info = "hello";
	Creatable<Inner>();
	Creatable<OldInsightMe>();
	Creatable<InsightMe>();
	OldInsightMe old(2, 2, 2);

	func(1, "2");
	func2('a', 0.1f);
}


bool Annotation::Initialize() {
	return true;
}

bool Annotation::Run(int randomSeed, int length) {
	Test();
	return true;
}

void Annotation::Summary() {}

TObject<IReflect>& Annotation::operator () (IReflect& reflect) {
	ReflectClass(Annotation);

	return *this;
}