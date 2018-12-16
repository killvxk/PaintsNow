// IDeferredObject.h
// By PaintDream (paintdream@paintdream.com)
// 2015-10-6
//

#ifndef __IDEFERREDOBJECT_H__
#define __IDEFERREDOBJECT_H__

#include "IThread.h"

namespace PaintsNow {
	class IDeferredObject {
	public:
		IDeferredObject();
		virtual ~IDeferredObject();
		virtual void Modify();
		virtual void Update();
		virtual void ReleaseObject();
		virtual bool IsValid() const;

	protected:
		virtual void Synchronize() = 0;
		bool isModified;
		bool isReleased;
	};
}


#endif // __IDEFERREDOBJECT_H__