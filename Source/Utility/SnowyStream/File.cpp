#include "File.h"

using namespace PaintsNow;
using namespace PaintsNow::NsSnowyStream;

File::File(IStreamBase* s, size_t len, uint64_t modTime) : stream(s), length(len), lastModifiedTime(modTime) {}

File::~File() {
	if (stream != nullptr)
		Close();
}

TObject<IReflect>& File::operator () (IReflect& reflect) {
	ReflectClass(File);
	return *this;
}

IStreamBase* File::GetStream() const {
	return stream;
}

size_t File::GetLength() const {
	return length;
}

uint64_t File::GetLastModifiedTime() const {
	return lastModifiedTime;
}

void File::Close() {
	if (stream != nullptr) {
		stream->ReleaseObject();
		stream = nullptr;
	}
}

IStreamBase* File::Detach() {
	IStreamBase* s = stream;
	stream = nullptr;
	return s;
}