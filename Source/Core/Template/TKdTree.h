#ifndef __TKDTREE_H__
#define __TKDTREE_H__


#include "../PaintsNow.h"
#include "../Common/ZType.h"
#include <algorithm>
#include <cassert>

namespace PaintsNow
{
	template <class K, size_t M>
	class TKdTree {
	public:
		typedef uint16_t KeyType;
		typedef int16_t CountType;
		typedef TKdTree Type;
		TKdTree(const K& k = K(), const KeyType i = 0) : key(k), parentNode(nullptr), leftNode(nullptr), rightNode(nullptr), keyIndex(i),  visitCounter(0) {}

		inline void Attach(TKdTree* tree) {
			assert(tree != nullptr && tree != this);
			assert(tree->leftNode == nullptr && tree->rightNode == nullptr && tree->parentNode == nullptr);
			CheckCycle();
			Merge(tree);
			CheckCycle();
		}

		inline TKdTree* Detach() {
			CheckCycle();
			TKdTree* newRoot = nullptr;
			if (LightDetach(newRoot)) {
				CheckCycle();
				return newRoot;
			}

			CheckCycle();
			assert(leftNode != nullptr && rightNode != nullptr);
			// printf("Detach (%6.6f, %6.6f, %6.6f) \n", leftNode->FindMaximal(keyIndex)->GetValue(keyIndex), GetValue(keyIndex), rightNode->FindMinimal(keyIndex)->GetValue(keyIndex));

			TKdTree* p = visitCounter > 0 ? rightNode->FindMinimal(keyIndex) : leftNode->FindMaximal(keyIndex);
			CheckCycle();
			assert(p != nullptr);
			newRoot = p;

			TKdTree* temp = nullptr;
			p->Detach();
			assert(p->parentNode == nullptr);
			assert(p->leftNode == nullptr && p->rightNode == nullptr);

			CheckCycle();
			if (parentNode != nullptr) {
				TKdTree** pp = parentNode->leftNode == this ? &parentNode->leftNode : &parentNode->rightNode;
				*pp = newRoot;
				newRoot = nullptr;
			}

			if (leftNode != nullptr) {
				leftNode->parentNode = p;
			}

			if (rightNode != nullptr) {
				rightNode->parentNode = p;
			}

			std::swap(links, p->links);
			p->CheckCycle();
			CheckCycle();

			return newRoot;
		}

		template <class Q>
		inline TKdTree* Query(const K& targetKey, Q& queryer) {
			// ranged queryer
			TKdTree* newRoot = nullptr;
			TKdTree* oldParent = parentNode;
			if (queryer(targetKey, *this)) {
				CheckCycle();
				if (leftNode != nullptr && key.OverlapLeft(targetKey, keyIndex)) {
					leftNode->Query(targetKey, queryer);
					VisitLeft();
				}
				CheckCycle();

				if (rightNode != nullptr && key.OverlapRight(targetKey, keyIndex)) {
					rightNode->Query(targetKey, queryer);
					VisitRight();
				}
				CheckCycle();

				if ((leftNode != nullptr && visitCounter < -(long)M) || (rightNode != nullptr && visitCounter > (long)M) || oldParent == nullptr) {
					newRoot = PlaceDown();
					visitCounter = 0;
				}

				CheckCycle();
			}

			return oldParent != nullptr ? oldParent : newRoot;
		}

		inline const K& GetKey() const {
			return key;
		}

		inline TKdTree* GetParent() const {
			return parentNode;
		}

		inline void CheckCycle() {
			assert(leftNode != this && rightNode != this);
			if (leftNode != nullptr) {
				assert(leftNode->parentNode == this);
				assert(!leftNode->key.Compare(key, keyIndex));
			}

			if (rightNode != nullptr) {
				assert(rightNode->parentNode == this);
				assert(!key.Compare(rightNode->key, keyIndex));
			}

			if (parentNode != nullptr) {
				assert(parentNode->leftNode == this || parentNode->rightNode == this);
			}
		}

		void Validate() {
			CheckCycle();
			if (leftNode != nullptr) {
				leftNode->CheckCycle();
			}

			if (rightNode != nullptr) {
				rightNode->CheckCycle();
			}
		}

	protected:
		TKdTree* PlaceDown() {
			TKdTree* savedParent = parentNode;
			if (savedParent != nullptr) {
				assert(leftNode != nullptr || rightNode != nullptr);
				TKdTree** pp = this == savedParent->leftNode ? &savedParent->leftNode : &savedParent->rightNode;
				TKdTree* newRoot = Detach();
				assert(savedParent != parentNode);
				assert(*pp != nullptr);
				// savedParent->Validate();
				(*pp)->Attach(this);
				// savedParent->Validate();
			} else {
				TKdTree* newRoot = Detach();
				assert(newRoot != nullptr);
				newRoot->Attach(this);
				savedParent = newRoot;
			}

			return savedParent;
		}

		bool LightDetach(TKdTree*& newRoot) {
			newRoot = nullptr;
			CheckCycle();
			if (leftNode != nullptr) {
				if (rightNode != nullptr) {
					return false;
				} else {
					leftNode->parentNode = parentNode;
					newRoot = leftNode;
					leftNode = nullptr;
				}
			} else if (rightNode != nullptr) {
				rightNode->parentNode = parentNode;
				newRoot = rightNode;
				rightNode = nullptr;
			}

			if (parentNode != nullptr) {
				CheckCycle();
				TKdTree** pp = parentNode->leftNode == this ? &parentNode->leftNode : &parentNode->rightNode;
				*pp = newRoot;
				if (newRoot != nullptr) {
					newRoot->CheckCycle();
					newRoot = nullptr;
				}
				parentNode = nullptr;
			}
			CheckCycle();

			return true;
		}

		TKdTree* FindMinimal(KeyType index) {
			TKdTree* p = this;
			if (leftNode != nullptr) {
				TKdTree* compare = leftNode->FindMinimal(index);
				if (p->key.Compare(compare->key, index)) {
					p = compare;
				}
			}

			if (index != keyIndex && rightNode != nullptr) {
				TKdTree* compare = rightNode->FindMinimal(index);
				if (p->key.Compare(compare->key, index)) {
					p = compare;
				}
			}

			return p;
		}

		TKdTree* FindMaximal(KeyType index) {
			TKdTree* p = this;
			if (index != keyIndex && leftNode != nullptr) {
				TKdTree* compare = leftNode->FindMaximal(index);
				if (!p->key.Compare(compare->key, index)) {
					p = compare;
				}
			}

			if (rightNode != nullptr) {
				TKdTree* compare = rightNode->FindMaximal(index);
				if (!p->key.Compare(compare->key, index)) {
					p = compare;
				}
			}

			return p;
		}

		inline void VisitLeft() {
			if (visitCounter > 0) {
				visitCounter /= 2;
			} else {
				visitCounter--;
			}
		}

		inline void VisitRight() {
			if (visitCounter < 0) {
				visitCounter /= 2;
			} else {
				visitCounter++;
			}
		}

		inline void Merge(TKdTree* tree) {
			CheckCycle();
			assert(tree->parentNode == nullptr);
			bool left = key.Compare(tree->key, keyIndex);
			if (left) {
				VisitLeft();
			} else {
				VisitRight();
			}

			TKdTree** ptr = left ? &leftNode : &rightNode;
			if (*ptr == nullptr) {
				*ptr = tree;
				tree->parentNode = this;
			} else {
				(*ptr)->Merge(tree);
			}
		}

		K key;						// 0	: ZFloat3Pair => 24 Bytes
		union {
			struct {
				TKdTree* parentNode;		// 24	: Pointer => 4 Bytes (32bit mode), 8 Bytes (64bit mode)
				TKdTree* leftNode;			// 28(32)
				TKdTree* rightNode;			// 32(40)
				KeyType keyIndex;			// 36(48)
				CountType visitCounter;		// 38(50)
											// 40(52+4): Total => 40(52+4) Bytes
			};
			struct {
				TKdTree* data[4];
			} links;
		};
	};
}


#endif // __TKDTREE_H__
