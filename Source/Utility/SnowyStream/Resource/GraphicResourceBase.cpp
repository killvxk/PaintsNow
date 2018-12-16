#include "GraphicResourceBase.h"

using namespace PaintsNow;
using namespace PaintsNow::NsSnowyStream;

GraphicResourceBase::GraphicResourceBase(ResourceManager& manager, const ResourceManager::UniqueLocation& uniqueID) : DeviceResourceBase<IRender>(manager, uniqueID), statusNotifier(nullptr) {}

GraphicResourceBase::~GraphicResourceBase() {}

const IRender::NOTIFY_TYPE RES_MASK = IRender::WRITE_COMPLETE | IRender::ALL_COMPLETE;
void GraphicResourceBase::Attach(IRender& render) {
	assert(statusNotifier == nullptr);
	statusNotifier = render.CreateNotifier(Wrap(this, &GraphicResourceBase::OnResourceNotification), RES_MASK);
}

void GraphicResourceBase::OnResourceNotification(void* object, IRender::NOTIFY_TYPE notifyType) {
	assert((notifyType & RES_MASK) == RES_MASK);

	Flag() |= RESOURCE_UPLOADED;
}

void GraphicResourceBase::Detach(IRender& render) {
	assert(statusNotifier != nullptr);
	render.DeleteNotifier(statusNotifier);
	statusNotifier = nullptr;
}

TObject<IReflect>& GraphicResourceBase::operator () (IReflect& reflect) {
	ReflectClass(GraphicResourceBase)[Interface(ResourceBase)];
	if (reflect.IsReflectProperty()) {
		ReflectProperty(statusNotifier)[Runtime];
	}

	return *this;
}
