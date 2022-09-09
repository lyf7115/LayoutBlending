#pragma once
#include "CLayoutTree.h"
#include "CHungarianAlgorithm.h"
#include <iostream>
#include <algorithm>
#include <set>
#include <vector>
#include <queue>
#include <unordered_map>
#include "CCompoundNode.h"

class pairhash {
public:
	template <typename T, typename U>
	std::size_t operator()(const std::pair<T, U>& x) const
	{
		return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
	}
};

class CNodeMatch
{
public:
	CNodeMatch() {};
	~CNodeMatch() {};

	int HungaryCost(std::vector<CNode*>& nodeSet1, std::vector<CNode*>& nodeSet2, std::set<std::pair<int, int>>& crtCsp);
	int ComputeCost(CNode* n1, CNode* n2);
	void ComputeCsp(CNode* tree1, CNode* tree2);

	void GetLeafNodes(CNode* node, std::vector<CNode*>& leaf_nodes);
	void AssignNodeIndex();

	void CreateCompoundTree();		//上边那种建CompoundTree的方式有问题，不能处理下沉
	 
	int getCost() { return two_tree_cost; }
	CompoundNode* GetCombineTree() { return comb_tree; }
	void SetLayoutTree(CLayoutTree* m_firstLayout, CLayoutTree* m_secondLayout);

private:
	int two_tree_cost;

	CNode* tree0;
	CNode* tree1;
	CLayoutTree* first_layout;
	CLayoutTree* second_layout;
	CompoundNode* comb_tree;

	std::unordered_map<std::pair<int, int>, std::set<std::pair<int, int>>, pairhash> recordCorrespondence;
	std::unordered_map<std::pair<int, int>, int, pairhash> recordCost;

private:
	int LeafToNULLCost(CNode* node);
	int LeafToLeafCost(CNode* n1, CNode* n2);

	CompoundNode* ReturnParentNode(int index1, int index2);
	CompoundNode* ReturnRightParentNode(int index2);
	CompoundNode* ReturnLeftParentNode(int index1);

	bool PairExistedInCombineTree(std::pair<int, int>& thisPair, CompoundNode* combine_tree_root_);
	bool OnlyPairFirstExistedInCombineTree(std::pair<int, int>& thisPair, CompoundNode* combine_tree_root_);
	bool PairDoNotExistedInCombineTree(std::pair<int, int>& thisPair, CompoundNode* combine_tree_root_);
	bool OnlyPairSecondExistedInCombineTree(std::pair<int, int>& thisPair, CompoundNode* combine_tree_root_);
};