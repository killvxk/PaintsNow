#include "ZBatchRanger.h"
using namespace PaintsNow;

ZBatchRanger::DrawCall::DrawCall() : indexBuffer(nullptr), instanceCount(0), from(0), to(0) {}

ZBatchRanger::CommandCapture::CommandCapture() : hashValue(0) {}

bool ZBatchRanger::CommandCapture::operator == (const ZBatchRanger::CommandCapture& rhs) const {
	return indexBuffer == rhs.indexBuffer
		&& from == rhs.from && to == rhs.to
		&& unmergableData == rhs.unmergableData
		&& dataBuffer == rhs.dataBuffer;
}

bool ZBatchRanger::CommandCapture::operator < (const ZBatchRanger::CommandCapture& rhs) const {
	int result = (int)(indexBuffer - rhs.indexBuffer);
	if (result != 0) {
		return result < 0;
	}

	result = from - rhs.from;
	if (result != 0) {
		return result < 0;
	}

	result = to - rhs.to;
	if (result != 0) {
		return result < 0;
	}

	unmergableData.compare(rhs.unmergableData);
	if (result != 0) {
		return result < 0;
	}

	return dataBuffer.compare(rhs.dataBuffer) < 0;
}

ZBatchRanger::ZBatchRanger(ZPipeline& pipeline) : basePipeline(pipeline) {
	std::vector<ArgumentsHandler>& argsHandler = basePipeline.argsHandler;
	assert(!argsHandler.empty());
	std::vector<std::pair<ArgumentsHandler, uint32_t> > mergableDataHandlers;
	std::vector<std::pair<ArgumentsHandler, uint32_t> > unmergableDataHandlers;
	uint32_t unmergableDataLength = 0, mergableDataLength = 0;

	for (size_t i = 0; i < argsHandler.size(); i++) {
		const ArgumentsHandler& handler = argsHandler[i];
		VAR_TYPE varType = handler.var->GetType();
		if (IShaderVariableBase::IsBuffer(varType) || IShaderVariableBase::IsTexture(varType)) {
			unmergableDataHandlers.push_back(std::make_pair(handler, (uint32_t)-1));
			unmergableDataHandlers.back().first.offset = unmergableDataLength;
			unmergableDataLength += handler.dataLength;
		} else {
			mergableDataHandlers.push_back(std::make_pair(handler, (uint32_t)-1));
			mergableDataHandlers.back().first.offset = mergableDataLength;
			mergableDataLength += handler.dataLength;
		}
	}

	std::swap(this->mergableDataHandlers, mergableDataHandlers);
	std::swap(this->unmergableDataHandlers, unmergableDataHandlers);
}

void ZBatchRanger::CleanupQueue(IRender& render, CommandQueue& commandQueue) const {
	CleanupBuffers(render, commandQueue.managedBuffers);
}

void ZBatchRanger::DoCapture(IRender& render, CommandCapture& commandCapture) const {
	size_t mergableDataLength = mergableDataHandlers.empty() ? 0 : mergableDataHandlers.back().first.offset + mergableDataHandlers.back().first.dataLength;
	size_t unmergableDataLength = unmergableDataHandlers.empty() ? 0 : unmergableDataHandlers.back().first.offset + unmergableDataHandlers.back().first.dataLength;

	// fast capture
	CommandCapture& snapshot = commandCapture;
	snapshot.unmergableData.resize(unmergableDataLength);
	snapshot.dataBuffer.resize(mergableDataLength);

	for (size_t i = 0; i < mergableDataHandlers.size(); i++) {
		const ArgumentsHandler& handler = mergableDataHandlers[i].first;
		handler.exporter(handler.var, const_cast<char*>(snapshot.dataBuffer.data()) + handler.offset);
	}

	for (size_t j = 0; j < unmergableDataHandlers.size(); j++) {
		const ArgumentsHandler& handler = unmergableDataHandlers[j].first;
		handler.exporter(handler.var, const_cast<char*>(snapshot.unmergableData.data()) + handler.offset);
	}

	snapshot.indexBuffer = basePipeline.GetIndexBuffer();
	basePipeline.GetIndexRange(snapshot.from, snapshot.to);
	snapshot.hashValue = HashBuffer(snapshot.unmergableData.data(), snapshot.unmergableData.size())
		^ HashBuffer(&snapshot.indexBuffer, sizeof(snapshot.indexBuffer))
		^ HashBuffer(&snapshot.from, sizeof(snapshot.from))
		^ HashBuffer(&snapshot.to, sizeof(snapshot.to));
}

void ZBatchRanger::CleanupBuffers(IRender& render, std::vector<IRender::Buffer*>& buffers) {
	for (size_t i = 0; i < buffers.size(); i++) {
		render.DeleteBuffer(buffers[i]);
	}

	buffers.clear();
}


void ZBatchRanger::WriteMergableDataBuffer(String& target, IRender& render, std::vector<IRender::Buffer*>& buffers, const std::vector<String>& mergedData, const String& dataBuffer, const std::vector<std::pair<ArgumentsHandler, uint32_t> >& mergableDataHandlers) {
	for (size_t i = 0, k = 0; i < mergableDataHandlers.size(); i++) {
		const std::pair<ArgumentsHandler, uint32_t>& handler = mergableDataHandlers[i];
		// keep
		if (handler.second != (uint32_t)-1) {
			memcpy(const_cast<char*>(target.data()) + handler.second, dataBuffer.data() + handler.first.offset, handler.first.dataLength);
		} else {
			// create instanced buffer
			IRender::Buffer* buffer = render.CreateBuffer(IRender::VERTEX_INSTANCED);
			render.InitBufferDataEx(buffer, mergedData[k++]);
			memcpy(const_cast<char*>(target.data()) + handler.second, &buffer, sizeof(IRender::Buffer*));
			buffers.push_back(buffer);
		}
	}
}

void ZBatchRanger::WriteUnmergableDataBuffer(String& target, const String& unmergedData, const std::vector<std::pair<ArgumentsHandler, uint32_t> >& unmergableDataHandlers) {
	for (size_t i = 0; i < unmergableDataHandlers.size(); i++) {
		const std::pair<ArgumentsHandler, uint32_t>& handler = unmergableDataHandlers[i];
		memcpy(const_cast<char*>(target.data()) + handler.second, unmergedData.data() + handler.first.offset, handler.first.dataLength);
	}
}

bool ZBatchRanger::CompilePipeline(IRender& render, std::vector<CommandCapture*> sortedCommandCaptures, ZPipeline& batchedPipeline) const {
	// must be exactly the same
	assert(batchedPipeline.GetUnique() == basePipeline.GetUnique());

	// restruct target pipelines ...
	std::vector<ArgumentsHandler>& argsHandler = basePipeline.argsHandler;
	std::vector<ArgumentsHandler>& batchedArgsHandler = batchedPipeline.argsHandler;

	assert(!argsHandler.empty());
	assert(batchedArgsHandler.size() == argsHandler.size());
	std::vector<bool> mergedIndicesBitset(mergableDataHandlers.size(), false);
	// collect requested batched fields
	for (size_t t = 0; t < batchedArgsHandler.size(); t++) {
		ArgumentsHandler& source = argsHandler[t];
		ArgumentsHandler& target = batchedArgsHandler[t];
		if (source.var->GetType() == IShaderVariableBase::INPUT_UNIFORM && target.var->GetType() == IShaderVariableBase::INPUT_INSTANCED_BUFFER) {
			mergedIndicesBitset[t] = true;
		}
	}

	// compute data-related batched fields
	for (size_t k = 0; k < sortedCommandCaptures.size(); k++) {
		CommandCapture* lastSnapshot = sortedCommandCaptures[0];
		CommandCapture* currentSnapshot = sortedCommandCaptures[k];
		// mergable?
		if (*currentSnapshot == *lastSnapshot) {
			// compute merge indices
			for (size_t k = 0; k < mergableDataHandlers.size(); k++) {
				const ArgumentsHandler& handler = mergableDataHandlers[k].first;
				if (!mergedIndicesBitset[k] && memcmp(currentSnapshot->dataBuffer.data() + handler.offset, lastSnapshot->dataBuffer.data() + handler.offset, handler.dataLength) != 0) {
					mergedIndicesBitset[k] = true;
				}
			}
		} else {
			// go for next
			assert(k != 0);
			lastSnapshot = currentSnapshot;
		}
	}

	bool updated = false;
	for (size_t i = 0; i < argsHandler.size(); i++) {
		const ArgumentsHandler& handler = argsHandler[i];
		VAR_TYPE varType = handler.var->GetType();
		ArgumentsHandler& batchedHandler = batchedArgsHandler[i];
		IShaderVariableBase* batchedVariable = batchedHandler.var;

		if (!IShaderVariableBase::IsBuffer(varType) && !IShaderVariableBase::IsTexture(varType)) {
			if (mergedIndicesBitset[i] && batchedVariable->GetType() != IShaderVariableBase::INPUT_INSTANCED_BUFFER) {
				// ok, it is mergable
				batchedVariable->SetType(IShaderVariableBase::INPUT_INSTANCED_BUFFER); // use instancing~
				updated = true;
			}
		}
	}

	// refresh batched infos ...
	return updated;
}

void ZBatchRanger::CompileQueue(IRender& render, CommandQueue& commandQueue, ZPipeline& batchedPipeline) const {
	std::vector<ArgumentsHandler>& argsHandler = basePipeline.argsHandler;
	std::vector<ArgumentsHandler>& batchedArgsHandler = batchedPipeline.argsHandler;
	std::vector<std::pair<ArgumentsHandler, uint32_t> >& mergableDataHandlers = commandQueue.mergableDataHandlers;
	std::vector<std::pair<ArgumentsHandler, uint32_t> >& unmergableDataHandlers = commandQueue.unmergableDataHandlers;

	// copy std handlers
	mergableDataHandlers = this->mergableDataHandlers;
	unmergableDataHandlers = this->unmergableDataHandlers;

	// find corelated mergable handlers
	uint32_t batchedTotalDataLength = 0;
	std::vector<bool> mergedIndicesBitset(mergableDataHandlers.size(), false);
	// collect requested batched fields
	for (size_t t = 0; t < batchedArgsHandler.size(); t++) {
		ArgumentsHandler& source = argsHandler[t];
		ArgumentsHandler& target = batchedArgsHandler[t];
		if (source.var->GetType() == IShaderVariableBase::INPUT_UNIFORM && target.var->GetType() == IShaderVariableBase::INPUT_INSTANCED_BUFFER) {
			mergedIndicesBitset[t] = true;
		}
	}

	for (size_t i = 0, mergedIndex = 0, unmergedIndex = 0; i < argsHandler.size(); i++) {
		const ArgumentsHandler& handler = argsHandler[i];
		VAR_TYPE varType = handler.var->GetType();
		ArgumentsHandler& batchedHandler = batchedArgsHandler[i];
		IShaderVariableBase* batchedVariable = batchedHandler.var;

		if (!IShaderVariableBase::IsBuffer(varType) && !IShaderVariableBase::IsTexture(varType)) {
			mergableDataHandlers[mergedIndex++].second = batchedTotalDataLength;
			if (mergedIndicesBitset[i]) {
				// ok, it is mergable
				batchedVariable->SetType(IShaderVariableBase::INPUT_INSTANCED_BUFFER); // use instancing~
				batchedTotalDataLength += (uint32_t)batchedVariable->GetDataLength();
			} else {
				batchedTotalDataLength += handler.dataLength;
			}
		} else {
			unmergableDataHandlers[unmergedIndex++].second = batchedTotalDataLength;
			batchedTotalDataLength += handler.dataLength;
		}
	}


	std::vector<uint32_t> mergedIndices;
	for (size_t n = 0; n < mergedIndicesBitset.size(); n++) {
		if (mergedIndicesBitset[n]) {
			mergedIndices.push_back((uint32_t)n);
		}
	}

	std::swap(commandQueue.mergedIndices, mergedIndices);
	commandQueue.batchedTotalDataLength = batchedTotalDataLength;
}


void ZBatchRanger::CommitQueueCaptures(IRender& render, CommandQueue& commandQueue, std::vector<CommandCapture*> sortedCommandCaptures, ZPipeline& batchedPipeline) const {
	if (sortedCommandCaptures.empty()) return;

	std::vector<std::pair<ArgumentsHandler, uint32_t> >& mergableDataHandlers = commandQueue.mergableDataHandlers;
	std::vector<std::pair<ArgumentsHandler, uint32_t> >& unmergableDataHandlers = commandQueue.unmergableDataHandlers;
	std::vector<DrawCall>& drawCalls = commandQueue.drawCalls;
	std::vector<IRender::Buffer*>& managedBuffers = commandQueue.managedBuffers;
	std::vector<uint32_t>& mergedIndices = commandQueue.mergedIndices;
	uint32_t batchedTotalDataLength = commandQueue.batchedTotalDataLength;

	// ok, then rearrange data
	std::vector<String> mergedData(mergedIndices.size());
	CommandCapture* lastSnapshot = sortedCommandCaptures[0];
	for (size_t k = 0; k < sortedCommandCaptures.size(); k++) {
		CommandCapture* currentSnapshot = sortedCommandCaptures[k];
		bool commit = true;
		if (*currentSnapshot == *lastSnapshot) {
			for (size_t k = 0; k < mergedIndices.size(); k++) {
				const ArgumentsHandler& handler = mergableDataHandlers[k].first;
				String data;
				data.resize(handler.dataLength);
				handler.exporter(handler.var, const_cast<char*>(data.data()));
				mergedData[k].append(data);
			}

			commit = false;
		}

		if (commit || k == sortedCommandCaptures.size() - 1) {
			// commit current and load next
			DrawCall drawCall = static_cast<DrawCall&>(*currentSnapshot);
			drawCall.dataBuffer.resize(batchedTotalDataLength);
			WriteMergableDataBuffer(drawCall.dataBuffer, render, managedBuffers, mergedData, currentSnapshot->dataBuffer, mergableDataHandlers);
			WriteUnmergableDataBuffer(drawCall.dataBuffer, currentSnapshot->unmergableData, unmergableDataHandlers);
			drawCalls.push_back(std::move(drawCall));
			mergedData = std::vector<String>(mergedIndices.size());
		}
	}

}

void ZBatchRanger::InvokeRenderQueue(IRender& render, CommandQueue& commandQueue, ZPipeline& batchedPipeline) const {
	std::vector<ArgumentsHandler>& batchedArgsHandler = batchedPipeline.argsHandler;
	std::vector<void*> lastValues(batchedArgsHandler.size(), nullptr);
	std::vector<DrawCall>& drawCalls = commandQueue.drawCalls;
	std::vector<IRender::Buffer*>& managedBuffers = commandQueue.managedBuffers;

	IRender::Program* program = batchedPipeline.GetProgram();
	for (size_t i = 0; i < drawCalls.size(); i++) {
		DrawCall& drawCall = drawCalls[i];
		for (size_t k = 0; k < batchedArgsHandler.size(); k++) {
			ArgumentsHandler& handler = batchedArgsHandler[k];
			handler.binder(render, program, handler.var, drawCall.dataBuffer.data() + handler.offset);
		}

		render.BindIndex(program, drawCall.indexBuffer, drawCall.from, drawCall.to, drawCall.instanceCount);
		render.FireRender(program);
	}
}

ZBatchRanger::~ZBatchRanger() {}