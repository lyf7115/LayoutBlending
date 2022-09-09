#pragma once
#include "CNodeMatching.h"
#include "CQuadraticProgramming.h"
#include <fstream>

enum TRY_ALIGN{FIND,TRY_NEXT,NO_CHILDREN};

class Transfer
{
public:
	Transfer();
	~Transfer();

	void SetCombineTreeRoot(CompoundNode* root_node);
	void CalDeleteValue(CompoundNode* root_node);
	void DeterInterVariable(double slider_value);
	void TransferAndSolve(double slider_value);		//迁移关系并且求解

private:
	void GetCombineTreeAttri();
	int DetectNodeLayer(CompoundNode* node);
	int GetTreeHeight(CompoundNode* node);


	bool LeftAlignLeftTree(CompoundNode* root_node);
	bool LeftAlignRightTree(CompoundNode* root_node);
	bool RightAlignLeftTree(CompoundNode* root_node);
	bool RightAlignRightTree(CompoundNode* root_node);
	bool TopAlignLeftTree(CompoundNode* root_node);
	bool TopAlignRightTree(CompoundNode* root_node);
	bool BottomAlignLeftTree(CompoundNode* root_node);
	bool BottomAlignRightTree(CompoundNode* root_node);
	bool EqualSpacingLeftTree(CompoundNode* root_node);
	bool EqualSpacingRightTree(CompoundNode* root_ndoe);
	bool EqualSizeLeftTree(CompoundNode* root_ndoe);
	bool EqualSizeRightTree(CompoundNode* root_ndoe);

private:
	CompoundNode* root_combine_tree;
	double layer_interval;		//记录树两层之间的间隔值
	int combine_tree_height;
};