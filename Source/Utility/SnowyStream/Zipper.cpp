#include "Zipper.h"

using namespace PaintsNow;
using namespace PaintsNow::NsSnowyStream;

Zipper::Zipper(IArchive* a, IStreamBase* stream) : archive(a), streamBase(stream) {}
Zipper::~Zipper() {
	delete archive;
	streamBase->ReleaseObject();
}