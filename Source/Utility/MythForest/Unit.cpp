#include "Unit.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;

Box::Box(const ZFloat3Pair& box) : ZFloat3Pair(box) {}
Box::Box(const ZFloat3& minPoint, const ZFloat3& maxPoint) : ZFloat3Pair(minPoint, maxPoint) {}

float Box::operator [] (size_t index) const {
	assert(index < size);
	const float* buffer = reinterpret_cast<const float*>(this);
	return buffer[index];
}

bool Box::Compare(const Box& target, uint16_t index) {
	return target[index] < (*this)[index];
}

bool Box::OverlapLeft(const Box& target, uint16_t index) {
	if (index < size / 2) {
		return true;
	} else {
		return !((*this)[index] < target[index - size / 2]);
	}
}

bool Box::OverlapRight(const Box& target, uint16_t index) {
	if (index < size / 2) {
		return !(target[index + size / 2] < (*this)[index]);
	} else {
		return true;
	}
}

Unit* Unit::Raycast(ZFloat3& intersection, ZFloat3& normal, const ZFloat3Pair& ray, IReflectObject* metaInfo) const {
	return nullptr;
}

void Unit::SetWarpIndex(uint32_t warpIndex) {
	FLAG old, value;

	do {
		old = value = flag.load(std::memory_order_acquire);
		value = (value & ~(UNIT_WARP_INDEX_END - UNIT_WARP_INDEX_BEGIN)) | (warpIndex << Log2((uint32_t)UNIT_WARP_INDEX_BEGIN));
	} while (!flag.compare_exchange_weak(old, value, std::memory_order_release));
}

uint32_t Unit::GetWarpIndex() const {
	return (flag.load(std::memory_order_relaxed) & (UNIT_WARP_INDEX_END - UNIT_WARP_INDEX_BEGIN)) >> Log2((uint32_t)UNIT_WARP_INDEX_BEGIN);
}
