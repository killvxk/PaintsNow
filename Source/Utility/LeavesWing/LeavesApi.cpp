#include "LeavesApi.h"

using namespace PaintsNow;
using namespace PaintsNow::NsLeavesWing;

LeavesApi::~LeavesApi() {}

TObject<IReflect>& LeavesApi::operator () (IReflect& reflect) {
	ReflectClass(LeavesApi);
	if (reflect.IsReflectMethod()) {
		ReflectMethod(RegisterFactory);
		ReflectMethod(UnregisterFactory);
		ReflectMethod(QueryFactory);
	}

	return *this;
}