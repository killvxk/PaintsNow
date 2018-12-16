#include "Tiny.h"

using namespace PaintsNow;

TObject<IReflect>& Tiny::operator () (IReflect& reflect) {
	ReflectClass(Tiny);
	if (reflect.IsReflectProperty()) {
		// static_assert(sizeof(flag) == sizeof(uint32_t), "Unsupported compiler.");
		ReflectProperty(flag)[Runtime];
	}

	return *this;
}

TAtomic<Tiny::FLAG>& Tiny::Flag() {
	return flag;
}

Tiny::FLAG Tiny::Flag() const {
	return flag.load();
}

size_t Tiny::ReportMemoryUsage() const {
	return sizeof(*this);
}

Tiny::Tiny(FLAG fl) : flag(fl) {}
Tiny* Tiny::Duplicate() const {
	assert(false);
	return nullptr;
}
