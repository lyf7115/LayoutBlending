#include"CLayoutTree.h"
#include<queue>
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>

CLayoutTree::CLayoutTree()
{
	root = nullptr;
}

CLayoutTree::CLayoutTree(CNode* root_)
{
	root = root_;
}

CLayoutTree::~CLayoutTree()
{
	DestroyAllData();
}

void CLayoutTree::InsertNode(CNode* node, CNode* parent)
{
	if (node)
	{
		if (!parent)
		{
			if (root)
			{
				parent = root;
			}
			else
			{
				root = node;
				return;
			}
		}
		node->parent = parent;
		parent->children.push_back(node);
		parent->node_type = NODE_TYPE::NONLEAF;
	}
	else
	{
		return;
	}
}

void CLayoutTree::DeleteNode(CNode* node)
{
	if (node == nullptr)
	{
		return;
	}

	if (node->parent != nullptr)
	{
		node->parent->children.erase(std::find(node->parent->children.begin(), node->parent->children.end(), node));
		if (node->parent->children.size() == 0)
		{
			node->parent->node_type = NODE_TYPE::LEAF;
		}
		node->parent = nullptr;
	}

	auto pointer = node;

	while (true)
	{
		while (!pointer->children.empty())
		{
			pointer = pointer->children.back();
		}
		auto pointer_parent = pointer->parent;

		delete pointer;
		pointer = nullptr;
		pointer = pointer_parent;

		if (pointer == nullptr)
		{
			break;
		}

		auto back_iterator = pointer->children.end() - 1;

		pointer->children.erase(back_iterator);
	}

	node = nullptr;

	return;
}

void CLayoutTree::BreadthFirstSearch()
{	
	if (root)
	{
		std::queue<CNode*> temp_queue;
		temp_queue.push(root);
		while (!temp_queue.empty())
		{
			CNode* node = temp_queue.front();
			temp_queue.pop();

			std::cout << "node index = " << node->index;
			for (auto iter = node->children.begin(); iter != node->children.end(); iter++)
				temp_queue.push(*iter);
		}
	}
	else
	{
		return;
	}
}

void CLayoutTree::DestroyAllData()
{
	DeleteNode(root);
}

void CLayoutTree::ReadFromFile(std::string fileName)
{
	DestroyAllData();

	std::ifstream infile;
	infile.open(fileName.data());
	assert(infile.is_open());

	int nodes_number = 0;
	int equal_size_num = 0;
	int equal_space_num = 0;
	int left_align_num = 0;
	int right_align_num = 0;
	int top_align_num = 0;
	int bottom_align_num = 0;

	std::vector<int> each_line_data;

	std::string split_string;
	std::string each_line_string;
	while (getline(infile, each_line_string))
	{
		if (each_line_string == "node attribute")
		{
			getline(infile, each_line_string);

			nodes_number = StringToInteger(each_line_string);

			for (int i = 0; i < nodes_number; i++)
			{
				getline(infile, each_line_string);
				std::istringstream record(each_line_string);

				//split string
				while (record >> split_string)
				{
					each_line_data.push_back(StringToInteger(split_string));
				}

				if (each_line_data.back() == 0)
				{
					root = new CNode(each_line_data[0], NODE_TYPE::LEAF, each_line_data[3],each_line_data[4], each_line_data[5], each_line_data[6]);

					ptr_idx_map[root] = root->index;
					idx_ptr_map[root->index] = root;
					
					root->node_present_type = SetNodePresentType(each_line_data[7]);
				}
				else
				{
					CNode* non_root = new CNode(each_line_data[0], NODE_TYPE::LEAF, each_line_data[3],each_line_data[4], each_line_data[5], each_line_data[6]);

					ptr_idx_map[non_root] = non_root->index;
					idx_ptr_map[non_root->index] = non_root;

					non_root->node_present_type = SetNodePresentType(each_line_data[7]);

					this->InsertNode(non_root, idx_ptr_map[each_line_data[8]]);
				}
				each_line_data.clear();
			}
		}
		else if (each_line_string == "equal size")
		{
			getline(infile, each_line_string);
			equal_size_num = StringToInteger(each_line_string);

			for (int i = 0; i < equal_size_num; i++)
			{
				getline(infile, each_line_string);
				std::istringstream record(each_line_string);

				while (record >> split_string)
				{
					each_line_data.push_back(StringToInteger(split_string));
				}

				std::pair<int, int> temp_pair(each_line_data[0], each_line_data[1]);
				idx_ptr_map[each_line_data[2]]->equal_size_node.insert(temp_pair);
				each_line_data.clear();
			}
		}
		else if (each_line_string == "equal space")
		{
			getline(infile, each_line_string);
			equal_space_num = StringToInteger(each_line_string);		

			for (int i = 0; i < equal_space_num; i++)
			{
				getline(infile, each_line_string);
				std::istringstream record(each_line_string);

				while (record >> split_string)
				{
					each_line_data.push_back(StringToInteger(split_string));
				}

				int vec_size = each_line_data.size();
				
				for (int j = 0; j < vec_size - 2; j++)
				{
					idx_ptr_map[each_line_data.back()]->equal_space_node.node_index.push_back(each_line_data[j]);
				}
				
				//vertical equal space; horizontal equal space
				if (each_line_data[vec_size-2] == 1)
				{
					idx_ptr_map[each_line_data.back()]->equal_space_node.equal_space_mode = EQUAL_SPACE_MODE::VERTICAL;
				}
				else
				{
					idx_ptr_map[each_line_data.back()]->equal_space_node.equal_space_mode = EQUAL_SPACE_MODE::HORIZONTAL;
				}

				each_line_data.clear();
			}
		}
		else if (each_line_string == "left align")
		{
			getline(infile, each_line_string);
			left_align_num = StringToInteger(each_line_string);

			for (int i = 0; i < left_align_num; i++)
			{
				getline(infile, each_line_string);
				std::istringstream record(each_line_string);

				while (record >> split_string)
				{
					each_line_data.push_back(StringToInteger(split_string));
				}

				int vec_size = each_line_data.size();

				for (int j = 0; j < vec_size - 1; j++)
				{
					idx_ptr_map[each_line_data.back()]->left_align_node.push_back(each_line_data[j]);
				}

				each_line_data.clear();
			}
		}
		else if (each_line_string == "right align")
		{
			getline(infile, each_line_string);
			right_align_num = StringToInteger(each_line_string);

			for (int i = 0; i < right_align_num; i++)
			{
				getline(infile, each_line_string);
				std::istringstream record(each_line_string);

				while (record >> split_string)
				{
					each_line_data.push_back(StringToInteger(split_string));
				}

				int vec_size = each_line_data.size();

				for (int j = 0; j < vec_size - 1; j++)
				{
					idx_ptr_map[each_line_data.back()]->right_align_node.push_back(each_line_data[j]);
				}

				each_line_data.clear();
			}
		}
		else if (each_line_string == "top align")
		{
			getline(infile, each_line_string);
			top_align_num = StringToInteger(each_line_string);

			for (int i = 0; i < top_align_num; i++)
			{
				getline(infile, each_line_string);
				std::istringstream record(each_line_string);

				while (record >> split_string)
				{
					each_line_data.push_back(StringToInteger(split_string));
				}

				int vec_size = each_line_data.size();

				for (int j = 0; j < vec_size - 1; j++)
				{
					idx_ptr_map[each_line_data.back()]->top_align_node.push_back(each_line_data[j]);
				}

				each_line_data.clear();
			}
		}
		else if (each_line_string == "bottom align")
		{
			getline(infile, each_line_string);
			bottom_align_num = StringToInteger(each_line_string);

			for (int i = 0; i < bottom_align_num; i++)
			{
				getline(infile, each_line_string);
				std::istringstream record(each_line_string);

				while (record >> split_string)
				{
					each_line_data.push_back(StringToInteger(split_string));
				}

				int vec_size = each_line_data.size();

				for (int j = 0; j < vec_size - 1; j++)
				{
					idx_ptr_map[each_line_data.back()]->bottom_align_node.push_back(each_line_data[j]);
				}

				each_line_data.clear();
			}
		}
		else
		{
			std::cout << "candinate relation" << std::endl;
		}
	}
	infile.close();

	return;
}

int CLayoutTree::StringToInteger(std::string str)
{
	int number = 0;
	std::stringstream ss;
	ss << str;
	ss >> number;
	return number;
}

NODE_PRESENT_TYPE CLayoutTree::SetNodePresentType(int flag)
{
	if (flag == 0)
	{
		return NODE_PRESENT_TYPE::NONLABEL;
	}
	else if (flag == 1)
	{
		return NODE_PRESENT_TYPE::PICTURE;
	}
	else if (flag == 2)
	{
		return NODE_PRESENT_TYPE::TEXT;
	}
	else if (flag == 3)
	{
		return NODE_PRESENT_TYPE::PADDING;
	}
	else if (flag == 4)
	{
		return NODE_PRESENT_TYPE::TITLE;
	}
	else
	{
		std::cout << "SetNodePresentType call here" << std::endl;
	}
	return NODE_PRESENT_TYPE();
}