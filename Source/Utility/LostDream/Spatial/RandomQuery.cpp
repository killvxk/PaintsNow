#include "../../../Core/Template/TKdTree.h"
#include "../../MythForest/Unit.h"
#include "Spatial.h"
#include <vector>

using namespace PaintsNow;
using namespace PaintsNow::NsLostDream;
using namespace PaintsNow::NsMythForest;

inline Box BuildBoundingFromVertex(const ZFloat3& first, const ZFloat3& second) {
	Box box(first, first);
	Include(box, second);
	return box;
}

const size_t M = 6;
typedef TKdTree<Box, M> Tree;

inline Box BuildBoundingBoxRandomly() {
	return BuildBoundingFromVertex(ZFloat3((float)rand(), (float)rand(), (float)rand()), ZFloat3((float)rand(), (float)rand(), (float)rand()));
}

struct Queryer {
	size_t count;
	bool operator () (const Box& b, Tree& tree) {
		assert(history.find(&tree) == history.end());
		if (Overlap(b, tree.GetKey()))
			count++;
		history.insert(&tree);
		return true;
	}

	std::set<Tree*> history;
};

inline size_t FastQuery(Tree*& root, const Box& box) {
	Queryer q;
	q.count = 0;
	assert(root->GetParent() == nullptr);
	Tree* newRoot = root->Query(box, q);
	if (newRoot != nullptr) {
		root = newRoot;
	}

	return q.count;
}

inline size_t LinearSearch(Tree* root, std::vector<Tree>& nodes, const Box& box) {
	size_t count = 0;
	for (size_t i = 0; i < nodes.size(); i++) {
		if (nodes[i].GetParent() != nullptr || &nodes[i] == root)
			count += Overlap(nodes[i].GetKey(), box);
	}

	return count;
}

bool RandomQuery::Initialize() {
	return true;
}

bool RandomQuery::Run(int randomSeed, int length) {
	std::vector<Tree> nodes(length * 4096);
	srand(randomSeed);

	// initialize data
	for (size_t i = 0; i < nodes.size(); i++) {
		nodes[i] = Tree(BuildBoundingBoxRandomly(), rand() % Box::size);
	}

	// link data
	// select root
	Tree* root = &nodes[rand() % nodes.size()];

	for (size_t j = 0; j < nodes.size(); j++) {
		if (root != &nodes[j]) {
			root->Attach(&nodes[j]);
		}
	}

	// random detach data
	for (size_t k = 0; k < nodes.size() / 10; k++) {
		size_t index = rand() % nodes.size();
		// printf("REMOVE ITEM: %d", index);
		Tree* toDetach = &nodes[index];
		Tree* newRoot = toDetach->Detach();
		if (newRoot != nullptr) {
			root = newRoot;
			if (k & 1) {
				root->Attach(toDetach);
			}
		}
	}
	/*
	size_t detachedCount = 0;
	for (size_t t = 0; t < nodes.size(); t++) {
		if (nodes[t].GetParent() == nullptr && &nodes[t] != root) {
			printf("DETEACED ITEM: %d", (int)t);
		}
		detachedCount += nodes[t].GetParent() == nullptr;
	}

	printf("DEtached: %d\n", (int)detachedCount - 1);

	for (size_t m = 0; m < nodes.size(); m++) {
		nodes[m].CheckCycle();
	}*/

	// random select
	for (size_t n = 0; n < (size_t)(10 * length); n++) {
		Box box = BuildBoundingBoxRandomly();
		if (n == 5) {
			int b = 0;
		}
		size_t searchCount = LinearSearch(root, nodes, box);
		size_t queryCount = FastQuery(root, box);

		if (queryCount != searchCount) {
			printf("Unmatched result, %d got, %d expected.\n", (int)queryCount, (int)searchCount);
			return false;
		}
	}


	return true;
}

void RandomQuery::Summary() {
}


TObject<IReflect>& RandomQuery::operator ()(IReflect& reflect) {
	ReflectClass(RandomQuery);

	return *this;
}