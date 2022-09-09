#pragma once
#include"CNode.h"

enum OPERSTATE { CORRESED, DELETE };
enum NODE_CORRES_TYPE { T_AB, A, B};	//A:first tree   B:second_tree

class CompoundNode
{
public:
	CompoundNode() : tree0_node(nullptr), tree1_node(nullptr),parent(nullptr),tree0_node_id(-20000),tree1_node_id(-20000) {};
	~CompoundNode(){};

	void DeleteNode(CompoundNode* node);

	CompoundNode* DeepCopy();
		
	static void DeepDestroy(CompoundNode* aNode);

public:
	CNode* tree0_node;
	CNode* tree1_node;

	int tree0_node_id;
	int tree1_node_id;

	int index;
	OPERSTATE operation_state;
	//delete threshhold
	double operation_value;
	NODE_CORRES_TYPE node_corres_type;
	NODE_PRESENT_TYPE node_present_type;

	double x, y, width, height;
	std::set<std::pair<int, int>> equal_size_node;
	EqualSpace equal_space_node;
	std::vector<int> left_align_node;
	std::vector<int> right_align_node;
	std::vector<int> top_align_node;
	std::vector<int> bottom_align_node;

	CompoundNode* parent;
	std::vector<CompoundNode*> children;
};