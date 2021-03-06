// Zipper.h
// By PaintDream (paintdream@paintdream.com)
// 2018-8-4
//

#ifndef __ZIPPER_H__
#define __ZIPPER_H__

#include "../../Core/Object/Tiny.h"
#include "../../Core/Interface/IArchive.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		class Zipper : public SharedTiny {
		public:
			Zipper(IArchive* archive, IStreamBase* streamBase);
			virtual ~Zipper();

		protected:
			IArchive* archive;
			IStreamBase* streamBase;
		};
	}
}


#endif // __ZIPPER_H__