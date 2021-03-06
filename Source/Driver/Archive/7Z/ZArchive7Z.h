// ZArchive7Z -- 7z archiver
// By PaintDream (paintdream@paintdream.com)
//

#ifndef __ZARCHIVE7Z_H__
#define __ZARCHIVE7Z_H__

#include "../../../Core/Common/ZType.h"
#include "../../../Core/Interface/IArchive.h"
#include "../../../Core/Interface/IStreamBase.h"
#include <map>
#include <string>

extern "C" {
	#include "../../Filter/LZMA/Core/7z.h"
	#include "../../Filter/LZMA/Core/7zFile.h"
	#include "../../Filter/LZMA/Core/7zCrc.h"
	#include "../../Filter/LZMA/Core/7zAlloc.h"
	#include "../../Filter/LZMA/Core/7zBuf.h"
}

namespace PaintsNow {
	class ZArchive7Z final : public IArchive {
	public:
		ZArchive7Z(IStreamBase& stream, size_t len);
		virtual ~ZArchive7Z();

		virtual IStreamBase* Open(const String& uri, bool write, size_t& length, uint64_t* lastModifiedTime = nullptr);
		virtual void Query(const String& uri, const TWrapper<void, bool, const String&>& wrapper) const;
		virtual bool IsReadOnly() const;
		virtual bool Delete(const String& uri);

		static int main(int argc, char* argv[]);

		IStreamBase& GetStream();
		int64_t GetPos() const;
		void SetPos(int64_t s);
		int64_t GetLength() const;

	private:
		bool Open();

	private:
		IStreamBase& stream;
		CFileInStream archiveStream;
		CLookToRead lookStream;
		CSzArEx db;
		ISzAlloc allocImp;
		ISzAlloc allocTempImp;
		int64_t pos;
		int64_t size;
		bool opened;
		std::map<String, std::pair<UInt32, bool> > mapPathToID;
	};
}

#endif // __ZARCHIVE7Z_H__