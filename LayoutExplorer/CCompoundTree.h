#pragma once
#include "CCompoundNode.h"

class CCompoundTree
{
public:
	CCompoundTree(CompoundNode* root) : comTreeRoot(root) {};
	~CCompoundTree();

private:
	CompoundNode* comTreeRoot;
};

