#include "IImage.h"
#include "Interfaces.h"

using namespace PaintsNow;

int IImage::GetPixelSize(IRender::DATA_TYPE dataType, IRender::LAYOUT_TYPE layout) {
	int count = sizeof(float), size = 4;
	switch (dataType) {
		case IRender::FLOAT: size = sizeof(float); break;
		case IRender::FLOAT_HALF: size = sizeof(float) / 2; break;
		case IRender::UNSIGNED_BYTE: size = sizeof(unsigned char); break;
		case IRender::UNSIGNED_SHORT: size = sizeof(unsigned short); break;
		case IRender::UNSIGNED_INT: size = sizeof(unsigned int); break;
	}
	switch (layout) {
		case IRender::LAYOUT_R: count = 1; break;
		case IRender::LAYOUT_RG: count = 2; break;
		case IRender::LAYOUT_RGB: count = 3; break;
		case IRender::LAYOUT_RGBA: count = 4; break;
		case IRender::LAYOUT_BGRA: count = 4; break;
	}

	return size * count;
}

IImage::~IImage() {}