// File.h
// By PaintDream (paintdream@paintdream.com)
// 2015-1-7
//

#ifndef __FILE_H__
#define __FILE_H__

#include "../../Core/Object/Tiny.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		class File : public SharedTiny {
		public:
			File(IStreamBase* stream, size_t fileSize, uint64_t lastModifiedTime);
			virtual ~File();

			virtual TObject<IReflect>& operator () (IReflect& reflect);
			IStreamBase* GetStream() const;
			IStreamBase* Detach();
			size_t GetLength() const;
			uint64_t GetLastModifiedTime() const;
			void Close();

		private:
			IStreamBase* stream;
			uint64_t lastModifiedTime;
			size_t length;
		};
	}
}

#endif // __FILE_H__