#include "IDeferredObject.h"

using namespace PaintsNow;

IDeferredObject::IDeferredObject() : isModified(true), isReleased(false) {}
IDeferredObject::~IDeferredObject() {
	// assert(!isModified);
	// assert(isReleased);
}

void IDeferredObject::Modify() {
	isModified = true;
}

void IDeferredObject::ReleaseObject() {
	isReleased = true;
}

bool IDeferredObject::IsValid() const {
	return !isModified && !isReleased;
}

void IDeferredObject::Update() {
	isModified = false;
	Synchronize();
}
