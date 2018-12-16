#include "RenderFlowComponent.h"
#include "RenderStage.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;

RenderFlowComponent::RenderFlowComponent() {}

TObject<IReflect>& RenderFlowComponent::operator () (IReflect& reflect) {
	ReflectClass(RenderFlowComponent);

	return *this;
}

void RenderFlowComponent::AddNode(RenderStage* stage) {
	Graph<RenderStage>::AddNode(stage);
}

void RenderFlowComponent::RemoveNode(RenderStage* stage) {
	// removes all symbols related with stage ...
	for (std::map<String, RenderStage::Port*>::const_iterator it = stage->GetPortMap().begin(); it != stage->GetPortMap().end(); ++it) {
		RenderStage::Port* port = it->second;
		for (std::set<String>::const_iterator it = port->publicSymbols.begin(); it != port->publicSymbols.end(); ++it) {
			symbolMap.erase(*it);
		}

		port->publicSymbols.clear();
	}

	Graph<RenderStage>::RemoveNode(stage);
}

RenderStage::Port* RenderFlowComponent::ImportSymbol(const String& symbol) {
	std::map<String, std::pair<RenderStage*, String> >::const_iterator s = symbolMap.find(symbol);
	if (s != symbolMap.end()) {
		return (*s->second.first)[s->second.second];
	} else {
		return nullptr;
	}
}

bool RenderFlowComponent::ExportSymbol(const String& symbol, RenderStage* renderStage, const String& port) {
	RenderStage::Port* p = (*renderStage)[port];
	if (p != nullptr) {
		std::map<String, std::pair<RenderStage*, String> >::const_iterator s = symbolMap.find(symbol);

		if (s != symbolMap.end()) {
			// remove old entry
			RenderStage::Port* port = (*s->second.first)[s->second.second];

			if (port != nullptr) {
				port->publicSymbols.erase(symbol);
			}
		}

		p->publicSymbols.insert(port);
		symbolMap[symbol] = std::make_pair(renderStage, port);
		return true;
	} else {
		return false;
	}
}

struct CallBatch {
public:
	typedef IRenderStage Node;
	CallBatch(std::vector<std::vector<Node*> >& r) : result(r) {}
	// once item
	bool operator () (Node* stage) {
		batch.push_back(stage);
		return true;
	}

	// once batch
	bool operator () () {
		result.push_back(batch);
		batch.clear();
		return true;
	}

	std::vector<Node*> batch;
	std::vector<ZRenderFlow::Milestone>& result;
};

void RenderFlowComponent::UpdateCacheStages() {
	std::vector<ZRenderFlow::Milestone> result;
	CallBatch batch(result);
	IterateTopological(batch, batch);

	std::swap(result, cachedRenderStages);
}
