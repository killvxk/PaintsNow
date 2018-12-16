#include "TextResource.h"

using namespace PaintsNow;
using namespace PaintsNow::NsSnowyStream;

TextResource::TextResource(ResourceManager& manager, const ResourceManager::UniqueLocation& uniqueID) : DeviceResourceBase<IArchive>(manager, uniqueID) {}
TextResource::~TextResource() {}

uint64_t TextResource::GetMemoryUsage() const {
	return 0;
}

void TextResource::Attach(IArchive& archive) {
}

void TextResource::Detach(IArchive& archive) {
}

void TextResource::Upload(IArchive& archive) {

}

void TextResource::Download(IArchive& archive) {

}

bool TextResource::LoadExternalResource(IStreamBase& streamBase, size_t length) {
	text.resize(length);
	return streamBase.ReadBlock(const_cast<char*>(text.c_str()), length);
}

TObject<IReflect>& TextResource::operator () (IReflect& reflect) {
	ReflectClass(TextResource)[Interface(ResourceBase)];
	if (reflect.IsReflectProperty()) {
		// serializable
		ReflectProperty(text);
	}

	return *this;
}