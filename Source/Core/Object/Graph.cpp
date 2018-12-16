#include "Graph.h"
#include <iostream>
using namespace PaintsNow;

class Entity : public GraphNode<SharedTiny, IReflectObjectComplex> {
public:
	Entity(const String& n) : name(n) {
		ReflectNodePorts();
	}

	virtual TObject<IReflect>& operator () (IReflect& reflect) {
		ReflectClass(Entity);

		if (reflect.IsReflectProperty()) {
			ReflectProperty(In);
			ReflectProperty(Out);
		}

		return *this;
	}

	Port In, Out;
	String name;
};

static bool OptimizeHandlerEx(Entity* t) {
	return t->name == "Alpha";
}

static bool IterateNodeEx(Entity* t) {
	std::cout << "Node " << t->name.c_str() << std::endl;
	return true;
}

static bool Finish() {
	return true;
}

int testex(void) {
	typedef Graph<Entity> G;
	G g;
	TShared<Entity> alpha = TShared<Entity>::Make("Alpha");
	TShared<Entity> beta = TShared<Entity>::Make("Beta");
	TShared<Entity> gamma = TShared<Entity>::Make("Gamma");
	TShared<Entity> theta = TShared<Entity>::Make("Theta");
	TShared<Entity> sigma = TShared<Entity>::Make("Sigma");

	g.AddNode(alpha());
	g.AddNode(beta());
	g.AddNode(gamma());
	g.AddNode(theta());
	g.AddNode(sigma());

	(*gamma())["Out"]->Link((*beta())["In"]);
	(*alpha())["Out"]->Link((*beta())["In"]);
	(*alpha())["Out"]->Link((*gamma())["In"]);
	(*gamma())["Out"]->Link((*theta())["In"]);
	(*theta())["Out"]->Link((*alpha())["In"]);

	bool success = g.IterateTopological(IterateNodeEx, Finish);
	std::cout << "Sorting1 " << (success ? "Success!" : "Failed!") << std::endl;
	g.Optimize(OptimizeHandlerEx);

	(*theta())["Out"]->UnLink((*alpha())["In"]);
	success = g.IterateTopological(IterateNodeEx, Finish);
	std::cout << "Sorting2 " << (success ? "Success!" : "Failed!") << std::endl;
	g.IterateTopological(IterateNodeEx, Finish);

	return 0;
}