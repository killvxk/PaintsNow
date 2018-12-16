// IMemory -- Basic memory allocator interface
// By PaintDream (paintdream@paintdream.com)
// 2014-12-3
//

#ifndef __IMEMORY_H__
#define __IMEMORY_H__

#include "../PaintsNow.h"

namespace PaintsNow {
	class IMemory {
	public:
		virtual ~IMemory();
		// virtual void* Alloc(size_t size) = 0;
		// virtual void Free(void* data, size_t size) = 0;
	};
}

#endif // __IMEMORY_H__