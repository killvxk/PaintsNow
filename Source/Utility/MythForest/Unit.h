// Unit.h
// By PaintDream (paintdream@paintdream.com)
// 2018-7-27
//

#ifndef __UNIT_H__
#define __UNIT_H__

#include "../../Core/Object/Tiny.h"

namespace PaintsNow {
	namespace NsMythForest {
		class Box : public ZFloat3Pair {
		public:
			enum { size = 6 };
			Box(const ZFloat3Pair& pair);
			Box(const ZFloat3& minPoint = ZFloat3(), const ZFloat3& maxPoint = ZFloat3());

			float operator [] (size_t index) const;
			bool Compare(const Box& target, uint16_t index);
			bool OverlapLeft(const Box& target, uint16_t index);
			bool OverlapRight(const Box& target, uint16_t index);
		};

		class Unit : public SharedTiny {
		public:
			virtual Unit* Raycast(ZFloat3& intersection, ZFloat3& normal, const ZFloat3Pair& ray, IReflectObject* metaInfo) const;
			void SetWarpIndex(uint32_t warpIndex);
			uint32_t GetWarpIndex() const;

			enum {
				UNIT_WARP_BITS = 8,
				UNIT_WARP_INDEX_BEGIN = TINY_CUSTOM_BEGIN,
				UNIT_WARP_INDEX_END = TINY_CUSTOM_BEGIN << UNIT_WARP_BITS,
				UNIT_CUSTOM_BEGIN = UNIT_WARP_INDEX_END
			};
		};

#define CHECK_THREAD(unit) \
		(MUST_CHECK_REFERENCE_ONCE); \
		if (engine.GetCurrentWarpIndex() != unit->GetWarpIndex()) { \
			request.Error("Threading routine failed on " #unit); \
			return; \
		}
	}
}


#endif // __UNIT_H__