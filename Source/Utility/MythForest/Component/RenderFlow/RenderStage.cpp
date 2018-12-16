#include "RenderStage.h"
#include "RenderPort/RenderPortRenderTarget.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;

RenderStage::RenderStage() {}

class OutputScanner : public IReflect {
public:
	OutputScanner() : IReflect(true, false) {}
	virtual void ProcessProperty(IReflectObject& s, Unique typeID, const char* name, void* base, void* ptr, const MetaChainBase* meta) override {
		if (!s.IsBaseObject()) {
			IShaderVariableBase* var = s.QueryInterface(UniqueType<IShaderVariableBase>());
			
			if (var != nullptr) {
				if (var->GetType() == IShaderVariableBase::OUTPUT_TARGET) {
					outputs.push_back(name);
				}
			} else {
				RenderPort* port = s.QueryInterface(UniqueType<RenderPortRenderTarget>());
				if (port != nullptr) {
					outputs.push_back(name);
				}

				(s)(*this);
			}
		}
	}

	virtual void ProcessMethod(Unique typeID, const char* name, const TProxy<>* p, const Param& retValue, const std::vector<Param>& params, const MetaChainBase* meta) override {}

	std::vector<String> outputs;
};

// get pipeline outputs
// it's a debug function, do no use it in release
bool RenderStage::ValidateTargets(ZPipeline& pipeline) {
	std::vector<String> self, pipe;
	OutputScanner scannerSelf, scannerPipe;
	(*this)(scannerSelf);
	(pipeline)(scannerPipe);

	if (self.size() != pipe.size())
		return false;

	for (size_t i = 0; i < self.size(); i++) {
		if (self[i] != pipe[i]) {
			return false;
		}
	}

	return true;
}