#pragma once
#include<iostream>
#include <vector>
#include <set>

enum NODE_TYPE { LEAF, NONLEAF };
enum EQUAL_SPACE_MODE { HORIZONTAL, VERTICAL};
enum NODE_PRESENT_TYPE { PICTURE, TEXT, PADDING, NONLABEL, TITLE};

struct EqualSpace
{
	std::vector<int> node_index;
	EQUAL_SPACE_MODE equal_space_mode;
};

class CNode
{
public:
	CNode(int index_, NODE_TYPE node_type_,  int x_, int y_, int width_, int height_) : index(index_), node_type(node_type_),
		x(x_), y(y_), width(width_), height(height_),parent(nullptr) {};

	//~CNode();

public:
	//id for each node 
	int index;

	//node type: LEAF, NONLEAF
	NODE_TYPE node_type;   

	//position
	int x;
	int y;

	//size
	int width;
	int height;

	//semantic infor, only leaf node have semantic infor
	NODE_PRESENT_TYPE node_present_type;

	CNode* parent;
	//store children
	std::vector<CNode*> children;   

	//pair relation for Equal Size
	std::set<std::pair<int, int>> equal_size_node;

	//group relation for Align and Equal Space
	EqualSpace equal_space_node;
	std::vector<int> left_align_node;
	std::vector<int> right_align_node;
	std::vector<int> top_align_node;
	std::vector<int> bottom_align_node;
};