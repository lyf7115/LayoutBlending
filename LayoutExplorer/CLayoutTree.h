#pragma once
#include<regex>
#include"CNode.h"
#include <assert.h>
#include <unordered_map>

class CLayoutTree
{
public:
	CLayoutTree();
	CLayoutTree(CNode* root_);

	~CLayoutTree();

	//insert node to contruct tree
	void InsertNode(CNode* node, CNode* parent = nullptr);
	void DeleteNode(CNode* node);
	void BreadthFirstSearch();
	void DestroyAllData();

	//get root
	CNode* GetRoot() { return root; };

	void ReadFromFile(std::string fileName);
public:
	std::vector<CNode*> m_allNodes;
	std::unordered_map<CNode*, int> ptr_idx_map;
	std::unordered_map<int, CNode*> idx_ptr_map;

private:
	CNode* root;

private:
	int StringToInteger(std::string str);
	NODE_PRESENT_TYPE SetNodePresentType(int flag);

};