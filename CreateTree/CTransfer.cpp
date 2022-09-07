#include"CTransfer.h"

Transfer::Transfer()
{
	combine_tree_height = 0;
}

Transfer::~Transfer()
{
}

void Transfer::SetCombineTreeRoot(CompoundNode* root_node)
{
	root_combine_tree = root_node;
	return;
}

void Transfer::DeterInterVariable(double slider_value)
{
	double slider_value1 = slider_value + 0.01;
	double slider_value2 = 1 - slider_value + 0.01;

	/*double slider_value1 = slider_value * 2 + 0.01;
	double slider_value2 = 2 - slider_value * 2 + 0.01;*/

	//when delete nodes,from the bottom up	
	std::vector<CompoundNode*> combine_tree_nodes;
	std::queue<CompoundNode*> temp_queue;

	if (root_combine_tree != nullptr)
	{
		temp_queue.push(root_combine_tree);

		while (!temp_queue.empty())
		{
			CompoundNode* node = temp_queue.front();
			temp_queue.pop();			

			combine_tree_nodes.push_back(node);

			for (auto iter = node->children.begin(); iter != node->children.end(); iter++)
			{
				temp_queue.push(*iter);
			}
		}
	}

	//when delete a leafnode, please check whether this leafnode's ancestors should be deleted.
	for (int i = combine_tree_nodes.size() - 1; i >= 0; i--)
	{
		if (combine_tree_nodes[i] == nullptr)
		{			
			continue;
		}

		if (combine_tree_nodes[i]->operation_state == OPERSTATE::DELETE && combine_tree_nodes[i]->node_corres_type == NODE_CORRES_TYPE::A
				&& slider_value1 >= combine_tree_nodes[i]->operation_value && combine_tree_nodes[i]->children.size() == 0)
		{						
			CompoundNode* delete_node_parent = combine_tree_nodes[i]->parent;
			std::vector<CompoundNode*> acestor_nodes;

			CompoundNode* temp_acestor_node = delete_node_parent;
			while (temp_acestor_node != root_combine_tree)
			{
				if (!(temp_acestor_node->tree0_node != nullptr && temp_acestor_node->tree1_node != nullptr))
				{
					acestor_nodes.push_back(temp_acestor_node);
				}
				temp_acestor_node = temp_acestor_node->parent;
			}

			root_combine_tree->DeleteNode(combine_tree_nodes[i]);
			combine_tree_nodes[i] = nullptr;	
			
			for (int j = 0; j < acestor_nodes.size(); j++)
			{
				CompoundNode* each_node = acestor_nodes[j];				
				if (each_node->children.size() == 0)
				{
					//need delete		
					for (int k = 0;k < combine_tree_nodes.size();k++)
					{						
						if (combine_tree_nodes[k] == each_node)
						{
							combine_tree_nodes[k] = nullptr;
						}
					}
					root_combine_tree->DeleteNode(each_node);									
				}
			}
		}
		else if (combine_tree_nodes[i]->operation_state == OPERSTATE::DELETE && combine_tree_nodes[i]->node_corres_type == NODE_CORRES_TYPE::B
					&& slider_value2 >= combine_tree_nodes[i]->operation_value && combine_tree_nodes[i]->children.size() == 0)
		{			
			CompoundNode* delete_node_parent = combine_tree_nodes[i]->parent;
			std::vector<CompoundNode*> acestor_nodes;

			CompoundNode* temp_acestor_node = delete_node_parent;
			while (temp_acestor_node != root_combine_tree)
			{
				if (!(temp_acestor_node->tree0_node != nullptr && temp_acestor_node->tree1_node != nullptr))
				{
					acestor_nodes.push_back(temp_acestor_node);
				}
				temp_acestor_node = temp_acestor_node->parent;
			}

			root_combine_tree->DeleteNode(combine_tree_nodes[i]);
			combine_tree_nodes[i] = nullptr;
			
			for (int j = 0; j < acestor_nodes.size(); j++)
			{
				CompoundNode* each_node = acestor_nodes[j];				
				if (each_node->children.size() == 0)
				{
					for (int k = 0; k < combine_tree_nodes.size(); k++)
					{
						if (combine_tree_nodes[k] == each_node)
						{
							combine_tree_nodes[k] = nullptr;
						}
					}					
					root_combine_tree->DeleteNode(each_node);			
				}
			}
		}
		else
		{
			continue;
		}
	}
	return;
}

//branch nodes will not be assigned delete value
void Transfer::CalDeleteValue(CompoundNode* root_node)
{
	std::vector<CompoundNode*> only_exit_left;
	std::vector<CompoundNode*> only_exit_right;

	std::vector<int> left_delete_nodes_code;
	std::vector<int> right_delete_nodes_code;

	std::vector<CompoundNode*> left_delete_nodes;
	std::vector<CompoundNode*> right_delete_nodes;

	if (root_node)
	{
		std::queue<CompoundNode*> temp_queue;
		temp_queue.push(root_node);

		while (!temp_queue.empty())
		{
			CompoundNode* node = temp_queue.front();
			temp_queue.pop();

			if (node->tree0_node != nullptr && node->tree1_node != nullptr)
			{
				node->operation_state = OPERSTATE::CORRESED;
				node->node_corres_type = NODE_CORRES_TYPE::T_AB;
			}
			else if (node->tree0_node != nullptr && node->tree1_node == nullptr)
			{
				node->operation_state = OPERSTATE::DELETE;
				node->node_corres_type = NODE_CORRES_TYPE::A;
				if (node->children.size() == 0)
				{					
					only_exit_left.push_back(node);
				}
			}
			else if (node->tree0_node == nullptr && node->tree1_node != nullptr)
			{
				node->operation_state = OPERSTATE::DELETE;
				node->node_corres_type = NODE_CORRES_TYPE::B;
				if (node->children.size() == 0)
				{					
					only_exit_right.push_back(node);
				}
			}			

			for (auto iter = node->children.begin(); iter != node->children.end(); iter++)
				temp_queue.push(*iter);
		}
	}

	int left_delete_nodes_num = only_exit_left.size();
	int right_delete_nodes_num = only_exit_right.size();

	left_delete_nodes_code.resize(left_delete_nodes_num, 0);
	right_delete_nodes_code.resize(right_delete_nodes_num, 0);

	for (int i = 0; i < left_delete_nodes_num; i++)
	{
		int layer = 0;
		int order = 0;
		int delete_code = 0;

		layer = DetectNodeLayer(only_exit_left[i]);

		CompoundNode* node_parent = only_exit_left[i]->parent;
		for (int j=0;j<node_parent->children.size();j++)
		{
			if (node_parent->children[j] == only_exit_left[i])
			{
				order = j + 1;
				break;
			}
		}

		delete_code = 100 * layer + order;
		left_delete_nodes_code[i] = delete_code;
	}

	for (int i = 0; i < right_delete_nodes_num; i++)
	{
		int layer = 0;
		int order = 0;
		int delete_code = 0;

		layer = DetectNodeLayer(only_exit_right[i]);

		CompoundNode* node_parent = only_exit_right[i]->parent;
		for (int j = 0; j < node_parent->children.size(); j++)
		{
			if (node_parent->children[j] == only_exit_right[i])
			{
				order = j + 1;
				break;
			}
		}

		delete_code = 100 * layer + order;
		right_delete_nodes_code[i] = delete_code;
	}

	int counter1 = 0;
	int counter2 = 0;

	while (counter1 < left_delete_nodes_num)
	{
		int min_num = *std::min_element(left_delete_nodes_code.begin(), left_delete_nodes_code.end());
		for (int i=0;i<left_delete_nodes_code.size();i++)
		{
			if (left_delete_nodes_code[i] == min_num)
			{
				counter1++;
				left_delete_nodes_code[i] = 1e+9;
				left_delete_nodes.push_back(only_exit_left[i]);
			}
		}
	}

	while (counter2 < right_delete_nodes_num)
	{
		int min_num = *std::min_element(right_delete_nodes_code.begin(), right_delete_nodes_code.end());
		for (int i = 0; i < right_delete_nodes_code.size(); i++)
		{
			if (right_delete_nodes_code[i] == min_num)
			{
				counter2++;
				right_delete_nodes_code[i] = 1e+9;
				right_delete_nodes.push_back(only_exit_right[i]);
			}
		}
	}

	for (int i = 0; i < left_delete_nodes.size(); i++)
	{
		left_delete_nodes[i]->operation_value = int((1.0 / left_delete_nodes_num) * (left_delete_nodes_num - i) * 10000 + 0.5) / 10000.0;		
	}	

	for (int i = 0; i < right_delete_nodes.size(); i++)
	{
		right_delete_nodes[i]->operation_value = int((1.0 / right_delete_nodes_num) * (right_delete_nodes_num - i) * 10000 + 0.5) / 10000.0;
	}

	return;
}

void Transfer::GetCombineTreeAttri()
{
	combine_tree_height = GetTreeHeight(root_combine_tree);
	layer_interval = 1.0 / (combine_tree_height - 1);
	return;
}

int Transfer::GetTreeHeight(CompoundNode* node)
{
	if (!node)
	{
		return 0;
	}

	if (node->children.size() == 0)
	{
		return 1;
	}

	int max_depth = 0;
	for (auto node_ : node->children)
	{
		int height = GetTreeHeight(node_);
		max_depth = (height > max_depth) ? height : max_depth;
	}
	return max_depth + 1;
}

int Transfer::DetectNodeLayer(CompoundNode* node)
{
	if (node == nullptr)
	{
		return 0;
	}
	int result = 1;
	CompoundNode* node_parent = node->parent;
	while (node_parent != nullptr)
	{
		result++;
		node_parent = node_parent->parent;
	}
	return result;
}

void Transfer::TransferAndSolve(double slider_value)
{
	if (root_combine_tree != nullptr)
	{
		std::queue<CompoundNode*> temp_queue;
		temp_queue.push(root_combine_tree);

		while (!temp_queue.empty())
		{
			CompoundNode* node = temp_queue.front();
			temp_queue.pop();
			for (int i = 0; i < node->children.size(); ++i)
			{
				temp_queue.push(node->children[i]);
			}

			if ((slider_value < 0.5 && node->tree0_node != nullptr))
			{
				if (LeftAlignLeftTree(node) && RightAlignLeftTree(node))
				{	
					EqualSpacingLeftTree(node);
					//std::cout << "side find in left" << std::endl;
				}
				else if (TopAlignLeftTree(node) && BottomAlignLeftTree(node))
				{	
					EqualSpacingLeftTree(node);
					//std::cout << "up down find in left" << std::endl;
				}
				else if (LeftAlignRightTree(node)&& RightAlignRightTree(node))
				{	
					EqualSpacingRightTree(node);
					//std::cout << "side find in right" << std::endl;
				}
				else if (TopAlignRightTree(node)&& BottomAlignRightTree(node))
				{	
					EqualSpacingRightTree(node);
					//std::cout<< "up down find in right" << std::endl;
				}
				EqualSizeLeftTree(node);
			}
			else if ((slider_value < 0.5 && node->tree0_node == nullptr))
			{
				if (LeftAlignRightTree(node) && RightAlignRightTree(node))
				{
					EqualSpacingRightTree(node);
					//std::cout << "side find in right" << std::endl;
				}
				else if (TopAlignRightTree(node) && BottomAlignRightTree(node))
				{
					EqualSpacingRightTree(node);
					//std::cout << "up down find in right" << std::endl;
				}				
				EqualSizeRightTree(node);
			}
			else if ((slider_value >= 0.5 && node->tree1_node != nullptr))
			{
				if (LeftAlignRightTree(node) && RightAlignRightTree(node))
				{
					EqualSpacingRightTree(node);
					//std::cout << "side find in right" << std::endl;
				}
				else if (TopAlignRightTree(node) && BottomAlignRightTree(node))
				{
					EqualSpacingRightTree(node);
					//std::cout << "up down find in right" << std::endl;
				}
				else if (LeftAlignLeftTree(node) && RightAlignLeftTree(node))
				{
					EqualSpacingLeftTree(node);
					//std::cout << "side find in left" << std::endl;
				}
				else if (TopAlignLeftTree(node) && BottomAlignLeftTree(node))
				{
					EqualSpacingLeftTree(node);
					//std::cout << "up down find in left" << std::endl;
				}
				EqualSizeRightTree(node);
			}
			else if ((slider_value >= 0.5 && node->tree1_node == nullptr))
			{
				if (LeftAlignLeftTree(node) && RightAlignLeftTree(node))
				{
					EqualSpacingLeftTree(node);
					//std::cout << "side find in left" << std::endl;
				}
				else if (TopAlignLeftTree(node) && BottomAlignLeftTree(node))
				{
					EqualSpacingLeftTree(node);
					//std::cout << "up down find in left" << std::endl;
				}				
				EqualSizeLeftTree(node);
			}			

		}

		//Solve it!!!!!
		ConstructFunction(root_combine_tree, slider_value);		
	}
	return;
}

bool Transfer::LeftAlignLeftTree(CompoundNode* Node)
{
	if (Node->tree0_node)
	{
		if (Node->tree0_node->left_align_node.size() > 0)
		{	
			if (Node->children.size() < 2)
			{
				return false;
			}
			for (int i = 0; i < Node->children.size(); ++i)
			{	
				Node->left_align_node.push_back(Node->children[i]->index);				
			}
			return true;
		}
	}
	return false;
}

bool Transfer::LeftAlignRightTree(CompoundNode* Node)
{
	if (Node->tree1_node)
	{
		if (Node->tree1_node->left_align_node.size() > 0)
		{	
			if (Node->children.size() < 2)
			{
				return false;
			}
			for (int i = 0; i < Node->children.size(); ++i)
			{
				Node->left_align_node.push_back(Node->children[i]->index);
			}
			return true;
		}
	}
	return false;
}

bool Transfer::RightAlignLeftTree(CompoundNode* Node)
{
	if (Node->tree0_node)
	{
		if (Node->tree0_node->right_align_node.size() > 0)
		{	
			if (Node->children.size() < 2)
			{
				return false;
			}
			for (int i = 0; i < Node->children.size(); ++i)
			{
				Node->right_align_node.push_back(Node->children[i]->index);
			}			
			return true;
		}
	}
	return false;
}

bool Transfer::RightAlignRightTree(CompoundNode* Node)
{
	if (Node->tree1_node)
	{	
		if (Node->tree1_node->right_align_node.size() > 0)
		{	
			if (Node->children.size() < 2)
			{
				return false;
			}
			for (int i = 0; i < Node->children.size(); ++i)
			{
				Node->right_align_node.push_back(Node->children[i]->index);
			}
			return true;
		}
	}
	return false;
}

bool Transfer::TopAlignLeftTree(CompoundNode* Node)
{
	if (Node->tree0_node)
	{
		if (Node->tree0_node->top_align_node.size() > 0)
		{	
			if (Node->children.size() < 2)
			{
				return false;
			}
			for (int i = 0; i < Node->children.size(); ++i)
			{
				Node->top_align_node.push_back(Node->children[i]->index);
			}
			return true;
		}
	}
	return false;
}

bool Transfer::TopAlignRightTree(CompoundNode* Node)
{
	if (Node->tree1_node)
	{
		if (Node->tree1_node->top_align_node.size() > 0)
		{	
			if (Node->children.size() < 2)
			{
				return false;
			}
			for (int i = 0; i < Node->children.size(); ++i)
			{
				Node->top_align_node.push_back(Node->children[i]->index);
			}
			return true;
		}
	}
	return false;
}

bool Transfer::BottomAlignLeftTree(CompoundNode* Node)
{
	if (Node->tree0_node)
	{
		if (Node->tree0_node->bottom_align_node.size() > 0)
		{	
			if (Node->children.size() < 2)
			{
				return false;
			}
			for (int i = 0; i < Node->children.size(); ++i)
			{
				Node->bottom_align_node.push_back(Node->children[i]->index);
			}
			return true;
		}
	}
	return false;
}

bool Transfer::BottomAlignRightTree(CompoundNode* Node)
{
	if (Node->tree1_node)
	{
		if (Node->tree1_node->bottom_align_node.size() > 0)
		{	
			if (Node->children.size() < 2)
			{
				return false;
			}
			for (int i = 0; i < Node->children.size(); ++i)
			{
				Node->bottom_align_node.push_back(Node->children[i]->index);
			}
			return true;
		}
	}
	return false;
}

bool Transfer::EqualSpacingLeftTree(CompoundNode* Node)
{
	if (Node->tree0_node)
	{		
		if (Node->children.size() <= 2)
		{
			return false;
		}
		for (int i = 0; i < Node->children.size(); ++i)
		{
			int idx0 = Node->children[i]->index;
			Node->equal_space_node.node_index.push_back(idx0);			
		}
		if (Node->left_align_node.size() >= 2 || Node->right_align_node.size() >= 2)
		{
			Node->equal_space_node.equal_space_mode = EQUAL_SPACE_MODE::VERTICAL;
		}
		else
		{
			Node->equal_space_node.equal_space_mode = EQUAL_SPACE_MODE::HORIZONTAL;
		}

		return true;
	}
	return false;
}

bool Transfer::EqualSpacingRightTree(CompoundNode* Node)
{
	if (Node->tree1_node)
	{
		if (Node->children.size() <= 2)
		{
			return false;
		}
		for (int i = 0; i < Node->children.size(); ++i)
		{
			int idx0 = Node->children[i]->index;
			Node->equal_space_node.node_index.push_back(idx0);			
		}
		if (Node->left_align_node.size() >= 2 || Node->right_align_node.size() >= 2)
		{
			Node->equal_space_node.equal_space_mode = EQUAL_SPACE_MODE::VERTICAL;
		}
		else
		{
			Node->equal_space_node.equal_space_mode = EQUAL_SPACE_MODE::HORIZONTAL;
		}
		return true;
	}
	return false;
}

bool Transfer::EqualSizeLeftTree(CompoundNode* Node)
{
	if (Node->tree0_node)
	{
		for (auto each_pair : Node->tree0_node->equal_size_node)
		{
			int corres1_in_combine = -1;
			int corres2_in_combine = -1;

			for (int i = 0; i < Node->children.size(); i++)
			{
				if (Node->children[i]->tree0_node && Node->children[i]->tree0_node->index == each_pair.first)
				{
					corres1_in_combine = Node->children[i]->index;
				}

				if (Node->children[i]->tree0_node && Node->children[i]->tree0_node->index == each_pair.second)
				{
					corres2_in_combine = Node->children[i]->index;
				}
			}

			if (corres1_in_combine != -1 && corres2_in_combine != -1)
			{
				Node->equal_size_node.insert(std::pair<int, int>(corres1_in_combine, corres2_in_combine));
			}
		}
	}
	return true;
}

bool Transfer::EqualSizeRightTree(CompoundNode* Node)
{
	if (Node->tree1_node)
	{
		for (auto each_pair : Node->tree1_node->equal_size_node)
		{
			int corres1_in_combine = -1;
			int corres2_in_combine = -1;

			for (int i = 0; i < Node->children.size(); i++)
			{
				if (Node->children[i]->tree1_node&&Node->children[i]->tree1_node->index == each_pair.first)
				{
					corres1_in_combine = Node->children[i]->index;
				}

				if (Node->children[i]->tree1_node && Node->children[i]->tree1_node->index == each_pair.second)
				{
					corres2_in_combine = Node->children[i]->index;
				}
			}

			if (corres1_in_combine != -1 && corres2_in_combine != -1)
			{
				Node->equal_size_node.insert(std::pair<int, int>(corres1_in_combine, corres2_in_combine));
			}
		}
	}
	return true;
}

//void Transfer::SaveGenerLayout(CompoundNode* combine_tree_root, std::string fileName)
//{
//	//注意：保存时要对index进行重新排列
//	std::vector<int> transfer_index;
//	transfer_index.resize(200, 0);
//	int index_counter = 1;
//
//
//	std::fstream File_output;
//	File_output.open(fileName, std::fstream::out);
//	File_output << "node attribute" << std::endl;
//
//	//注意得用层次遍历输出文件
//	int node_number = 0, equal_size_number = 0, equal_space_number = 0, left_align_number = 0, right_align_number = 0, top_align_number = 0, bottom_align_number = 0;
//
//	if (combine_tree_root)
//	{
//		std::queue<CompoundNode*> temp_queue;
//		temp_queue.push(combine_tree_root);
//		while (!temp_queue.empty())
//		{
//			CompoundNode* node = temp_queue.front();
//			temp_queue.pop();
//
//			node_number++;
//			equal_size_number += node->equal_size_node.size();
//			if (node->equal_space_node.size() > 2)
//			{
//				equal_space_number++;
//			}
//			left_align_number += node->left_align_node.size();
//			right_align_number += node->right_align_node.size();
//			top_align_number += node->top_align_node.size();
//			bottom_align_number += node->bottom_align_node.size();
//
//			transfer_index[node->index] = index_counter;
//			index_counter++;
//
//			for (auto iter = node->children.begin(); iter != node->children.end(); iter++)
//				temp_queue.push(*iter);
//		}
//	}
//	File_output << node_number << std::endl;
//
//	if (combine_tree_root)
//	{
//		std::queue<CompoundNode*> temp_queue;
//		temp_queue.push(combine_tree_root);
//		while (!temp_queue.empty())
//		{
//			CompoundNode* node = temp_queue.front();
//			temp_queue.pop();
//
//			if (node->index == 0)
//			{
//				File_output << transfer_index[node->index] << " " << 0 << " " << 0 << " " << (int)node->x << " " << (int)node->y << " " << (int)node->width << " " << (int)node->height << " " << 0 << std::endl;
//			}
//			else
//			{
//				File_output << transfer_index[node->index] << " " << 0 << " " << 0 << " " << (int)node->x << " " << (int)node->y << " " << (int)node->width << " " << (int)node->height << " " << transfer_index[node->parent->index] << std::endl;
//			}
//
//			for (auto iter = node->children.begin(); iter != node->children.end(); iter++)
//				temp_queue.push(*iter);
//		}
//	}
//
//	File_output << "equal size" << std::endl;
//	File_output << equal_size_number << std::endl;
//	if (combine_tree_root)
//	{
//		std::queue<CompoundNode*> temp_queue;
//		temp_queue.push(combine_tree_root);
//		while (!temp_queue.empty())
//		{
//			CompoundNode* node = temp_queue.front();
//			temp_queue.pop();
//			if (node->equal_size_node.size() > 0)
//			{
//				for (auto each_pair : node->equal_size_node)
//				{
//					File_output << transfer_index[each_pair.first] << " " << transfer_index[each_pair.second] << " " << transfer_index[node->index] << std::endl;
//				}
//			}
//			for (auto iter = node->children.begin(); iter != node->children.end(); iter++)
//				temp_queue.push(*iter);
//		}
//	}
//
//	File_output << "equal space" << std::endl;
//	File_output << equal_space_number << std::endl;
//	if (combine_tree_root)
//	{
//		std::queue<CompoundNode*> temp_queue;
//		temp_queue.push(combine_tree_root);
//		while (!temp_queue.empty())
//		{
//			CompoundNode* node = temp_queue.front();
//			temp_queue.pop();
//			if (node->equal_space_node.size() > 2)
//			{
//				for (int i = 0; i < node->equal_space_node.size(); i++)
//				{
//					if (i != node->equal_space_node.size() - 1 && i != node->equal_space_node.size() - 2)
//					{
//						File_output << transfer_index[node->equal_space_node[i]] << " ";
//					}
//					else if (i == node->equal_space_node.size() - 2)
//					{
//						File_output << node->equal_space_node[i] << " ";
//					}
//					else
//					{
//						File_output << transfer_index[node->index];
//					}
//				}
//				File_output << std::endl;
//			}
//			for (auto iter = node->children.begin(); iter != node->children.end(); iter++)
//				temp_queue.push(*iter);
//		}
//	}
//
//	File_output << "left align" << std::endl;
//	File_output << left_align_number << std::endl;
//	if (root_combine_tree)
//	{
//		std::queue<CompoundNode*> temp_queue;
//		temp_queue.push(root_combine_tree);
//		while (!temp_queue.empty())
//		{
//			CompoundNode* node = temp_queue.front();
//			temp_queue.pop();
//			if (node->left_align_node.size() > 0)
//			{
//				for (auto each_pair : node->left_align_node)
//				{
//					File_output << transfer_index[each_pair.first] << " " << transfer_index[each_pair.second] << " " << transfer_index[node->index] << std::endl;
//				}
//			}
//			for (auto iter = node->children.begin(); iter != node->children.end(); iter++)
//				temp_queue.push(*iter);
//		}
//	}
//
//	File_output << "right align" << std::endl;
//	File_output << right_align_number << std::endl;
//	if (combine_tree_root)
//	{
//		std::queue<CompoundNode*> temp_queue;
//		temp_queue.push(combine_tree_root);
//		while (!temp_queue.empty())
//		{
//			CompoundNode* node = temp_queue.front();
//			temp_queue.pop();
//			if (node->right_align_node.size() > 0)
//			{
//				for (auto each_pair : node->right_align_node)
//				{
//					File_output << transfer_index[each_pair.first] << " " << transfer_index[each_pair.second] << " " << transfer_index[node->index] << std::endl;
//				}
//			}
//			for (auto iter = node->children.begin(); iter != node->children.end(); iter++)
//				temp_queue.push(*iter);
//		}
//	}
//
//	File_output << "top align" << std::endl;
//	File_output << top_align_number << std::endl;
//	if (root_combine_tree)
//	{
//		std::queue<CompoundNode*> temp_queue;
//		temp_queue.push(root_combine_tree);
//		while (!temp_queue.empty())
//		{
//			CompoundNode* node = temp_queue.front();
//			temp_queue.pop();
//			if (node->top_align_node.size() > 0)
//			{
//				for (auto each_pair : node->top_align_node)
//				{
//					File_output << transfer_index[each_pair.first] << " " << transfer_index[each_pair.second] << " " << transfer_index[node->index] << std::endl;
//				}
//			}
//			for (auto iter = node->children.begin(); iter != node->children.end(); iter++)
//				temp_queue.push(*iter);
//		}
//	}
//
//	int bottom_align_counter = 0;
//	File_output << "bottom align" << std::endl;
//	File_output << bottom_align_number << std::endl;
//	if (root_combine_tree)
//	{
//		std::queue<CompoundNode*> temp_queue;
//		temp_queue.push(root_combine_tree);
//		while (!temp_queue.empty())
//		{
//			CompoundNode* node = temp_queue.front();
//			temp_queue.pop();
//			if (node->bottom_align_node.size() > 0)
//			{
//				for (auto each_pair : node->bottom_align_node)
//				{
//					bottom_align_counter++;
//					if (bottom_align_counter == bottom_align_number)
//					{
//						File_output << transfer_index[each_pair.first] << " " << transfer_index[each_pair.second] << " " << transfer_index[node->index];
//					}
//					else
//					{
//						File_output << transfer_index[each_pair.first] << " " << transfer_index[each_pair.second] << " " << transfer_index[node->index] << std::endl;
//					}
//				}
//			}
//			for (auto iter = node->children.begin(); iter != node->children.end(); iter++)
//				temp_queue.push(*iter);
//		}
//	}
//	File_output.close();
//}