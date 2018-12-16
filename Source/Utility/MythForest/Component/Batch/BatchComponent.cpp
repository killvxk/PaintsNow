#include "BatchComponent.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;


BatchComponent::BatchComponent(TShared<NsSnowyStream::ShaderResource> shader) : baseShaderResource(shader), batchRanger(shader->GetPipeline()) {
	Flag() |= TINY_UNIQUE | TINY_MODIFIED; // inited as modified
}

BatchComponent::~BatchComponent() {}

void BatchComponent::FlushCapture(IRender& render) {
	// commit async work
}

void BatchComponent::DoCapture(IRender& render, TShared<RenderableComponent> renderableComponent) {
	assert(Verify(Flag(), Tiny::TINY_ACTIVATED | Tiny::TINY_UPDATING));
	// save ...
	/*
	ZBatchRanger::CommandCapture commandCapture;
	batchRanger.DoCapture(render, commandCapture);
	commandCaptures.push_back(std::move(commandCapture));*/
}

void BatchComponent::BeginCapture(IRender& render) {
	Cleanup(render);
	Flag() |= Tiny::TINY_UPDATING;
}

void BatchComponent::Cleanup(IRender& render) {
	batchRanger.CleanupQueue(render, commandQueue);
}

void BatchComponent::EndCapture(IRender& render) {
	ZBatchRanger::CommandQueue commandQueue;
	NsSnowyStream::ShaderResource* dup = static_cast<NsSnowyStream::ShaderResource*>(baseShaderResource->Duplicate());
	batchRanger.CompileQueue(render, commandQueue, batchedShaderResource->GetPipeline());
	// load existing resource or create new
	NsSnowyStream::ShaderResource* batched = dup->CreateShadowInstance();
	batchedShaderResource = batched;

	dup->ReleaseObject();
	batched->ReleaseObject();

	std::vector<ZBatchRanger::CommandCapture*> sortedCommandCaptures(commandCaptures.size());

	for (size_t i = 0; i < commandCaptures.size(); i++) {
		sortedCommandCaptures[i] = &commandCaptures[i];
	}

	std::sort(sortedCommandCaptures.begin(), sortedCommandCaptures.end(), ZBatchRanger::SortCapturePointers());
	batchRanger.CommitQueueCaptures(render, commandQueue, sortedCommandCaptures, batchedShaderResource->GetPipeline());

	Flag() &= ~(TINY_UPDATING | TINY_MODIFIED);
}

bool BatchComponent::UpdateRenderData() {
	return true;
}

void BatchComponent::RenderInstance(IRender& render, IRender::Target* renderTarget, Instance& instance) {
	if (batchedShaderResource) {
		batchRanger.InvokeRenderQueue(render, commandQueue, batchedShaderResource->GetPipeline());
	}
}

TShared<NsSnowyStream::ShaderResource> BatchComponent::GetBaseShaderResource() const {
	return baseShaderResource;
}