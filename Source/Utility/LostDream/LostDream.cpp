#include "LostDream.h"
#include <ctime>

using namespace PaintsNow;
using namespace PaintsNow::NsLostDream;

LostDream::Qualifier::~Qualifier() {}

LostDream::~LostDream() {
	for (std::list<std::pair<Qualifier*, int> >::const_iterator p = qualifiers.begin(); p != qualifiers.end(); ++p) {
		delete p->first;
	}
}

bool LostDream::RegisterQualifier(const TFactoryBase<Qualifier>& q, int count) {
	Qualifier* qualifier = q.Create();
	if (!qualifier->Initialize()) {
		printf("Qualifier initialize failed!\n");
		delete qualifier;
		return false;
	}

	qualifiers.push_back(std::make_pair(qualifier, count));
	return true;
}

bool LostDream::RunQualifiers(bool stopOnError, int initRandomSeed, int length) {
	for (std::list<std::pair<Qualifier*, int> >::const_iterator p = qualifiers.begin(); p != qualifiers.end(); ++p) {
		int count = p->second;
		Qualifier* q = p->first;
		srand(initRandomSeed++);
		printf("Start qualifier %s\n", q->GetUnique().info->typeName.c_str());
		
		for (int i = 0; i < count; i++) {
			printf("Pass %d\n", i);
			if (!q->Run(rand(), length) && stopOnError) {
				printf("Stopped On Error.\n");
				return false;
			}
		}

		q->Summary();
	}

	return true;
}