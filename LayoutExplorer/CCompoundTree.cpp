#include "CCompoundTree.h"


CCompoundTree::~CCompoundTree()
{
	CompoundNode::DeepDestroy(comTreeRoot);
}