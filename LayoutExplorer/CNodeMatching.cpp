#include"CNodeMatching.h"

int CNodeMatch::HungaryCost(std::vector<CNode*>& nodeSet1, std::vector<CNode*>& nodeSet2, std::set<std::pair<int, int>>& crtCsp)
{
    std::vector<std::vector<int>> costMat;
    std::vector<std::vector<std::set<std::pair<int, int>>>> cspMat;
    std::vector<int> rowVec;
    std::vector<std::set<std::pair<int, int>>> rowCspVec;

    rowVec.resize(nodeSet1.size() + nodeSet2.size(), 0);
    rowCspVec.resize(nodeSet1.size() + nodeSet2.size(), std::set<std::pair<int, int>>());

    costMat.resize(nodeSet1.size() + nodeSet2.size(), rowVec);
    cspMat.resize(nodeSet1.size() + nodeSet2.size(), rowCspVec);

    for (int i = 0; i < nodeSet1.size(); ++i)
    {
        CNode* ni = nodeSet1[i];
        for (int j = 0; j < nodeSet2.size(); ++j)
        {
            CNode* nj = nodeSet2[j];
            costMat[i][j] = ComputeCost(ni, nj);
			
            cspMat[i][j] = recordCorrespondence[std::pair<int, int>(ni->index, nj->index)];
        }
    }

    for (int i = nodeSet1.size(); i < nodeSet1.size() + nodeSet2.size(); ++i)
    {
        for (int j = 0; j < nodeSet2.size(); ++j)
        {
            CNode* nj = nodeSet2[j];
            costMat[i][j] = ComputeCost(nullptr, nj);
            cspMat[i][j] = recordCorrespondence[std::pair<int, int>(-nj->index, nj->index)];
        }
    }

    for (int i = 0; i < nodeSet1.size(); ++i)
    {
        CNode* ni = nodeSet1[i];
        for (int j = nodeSet2.size(); j < nodeSet1.size() + nodeSet2.size(); ++j)
        {
            costMat[i][j] = ComputeCost(ni, nullptr);
            cspMat[i][j] = recordCorrespondence[std::pair<int, int>(ni->index, -ni->index)];
        }
    }

    CHungarianAlgorithm hung_algo;
    std::vector<int> HungaryCsp;
    int thisCost = hung_algo.Solve(costMat, HungaryCsp);

    for (int i = 0; i < HungaryCsp.size(); ++i)
    {
        crtCsp.insert(cspMat[i][HungaryCsp[i]].begin(), cspMat[i][HungaryCsp[i]].end());
    }

    return thisCost;
}

int CNodeMatch::LeafToNULLCost(CNode* node)
{
	double thisCost = std::sqrt((node->width * node->width + node->height * node->height) * 1.0);
	thisCost *= 1.5;
	return thisCost;
}

int CNodeMatch::LeafToLeafCost(CNode* n1, CNode* n2)
{
	//for different semantic infor
	if (n1->node_present_type != n2->node_present_type)
	{
		return 1e+9;
	}

	double leafToleaf_cost = 0;
	double centerDif[2];

	centerDif[0] = n1->x + 0.5*n1->width - n2->x - 0.5*n2->width;
	centerDif[1] = n1->y + 0.5*n1->height - n2->y - 0.5*n2->height;

	leafToleaf_cost = std::sqrt(centerDif[0] * centerDif[0] + centerDif[1] * centerDif[1]);
	leafToleaf_cost += std::sqrt((n1->width - n2->width) * (n1->width - n2->width) + (n1->height - n2->height) * (n1->height - n2->height));

	return (int)leafToleaf_cost;
}

bool CNodeMatch::PairExistedInCombineTree(std::pair<int, int>& thisPair, CompoundNode * combine_tree_root_)
{
	if (combine_tree_root_)
	{
		std::queue<CompoundNode*> temp_queue;
		temp_queue.push(combine_tree_root_);
		while (!temp_queue.empty())
		{
			CompoundNode* front_node = temp_queue.front();
			temp_queue.pop();

			if (front_node->tree0_node_id == thisPair.first && front_node->tree1_node_id == thisPair.second)
			{
				return true;
			}

			for (int i = 0; i < front_node->children.size(); i++)
			{
				temp_queue.push(front_node->children[i]);
			}
		}
	}
	return false;
}

bool CNodeMatch::OnlyPairFirstExistedInCombineTree(std::pair<int, int>& thisPair, CompoundNode* combine_tree_root_)
{
	bool first_flag = true;
	bool second_flag = false;
	bool third_flag = false;
	if (combine_tree_root_)
	{
		std::queue<CompoundNode*> temp_queue;
		temp_queue.push(combine_tree_root_);
		while (!temp_queue.empty())
		{
			CompoundNode* front_node = temp_queue.front();
			temp_queue.pop();
			if (front_node->tree0_node_id == thisPair.first && front_node->tree1_node_id == thisPair.second)
			{
				first_flag = false;
			}
			if (front_node->tree0_node_id == thisPair.first)
			{
				second_flag = true;
			}
			if (front_node->tree1_node_id == thisPair.second)
			{
				third_flag = true;
			}
			for (int i = 0; i < front_node->children.size(); i++)
			{
				temp_queue.push(front_node->children[i]);
			}
		}
	}
	if (first_flag == true && second_flag == true && third_flag == false)
	{
		return true;
	}
	return false;
}

bool CNodeMatch::PairDoNotExistedInCombineTree(std::pair<int, int>& thisPair, CompoundNode * combine_tree_root_)
{
	bool first_flag = true;
	bool second_flag = true;
	bool third_flag = true;

	if (combine_tree_root_)
	{
		std::queue<CompoundNode*> temp_queue;
		temp_queue.push(combine_tree_root_);
		while (!temp_queue.empty())
		{
			CompoundNode* front_node = temp_queue.front();
			temp_queue.pop();

			if (front_node->tree0_node_id == thisPair.first && front_node->tree1_node_id == thisPair.second)
			{
				first_flag = false;
			}

			if (front_node->tree0_node_id == thisPair.first)
			{
				second_flag = false;
			}

			if (front_node->tree1_node_id == thisPair.second)
			{
				third_flag = false;
			}

			for (int i = 0; i < front_node->children.size(); i++)
			{
				temp_queue.push(front_node->children[i]);
			}
		}
	}

	if (first_flag && second_flag && third_flag)
	{
		return true;
	}
	return false;
}

bool CNodeMatch::OnlyPairSecondExistedInCombineTree(std::pair<int, int>& thisPair, CompoundNode * combine_tree_root_)
{
	bool first_flag = true;
	bool second_flag = false;
	bool third_flag = false;
	if (combine_tree_root_)
	{
		std::queue<CompoundNode*> temp_queue;
		temp_queue.push(combine_tree_root_);
		while (!temp_queue.empty())
		{
			CompoundNode* front_node = temp_queue.front();
			temp_queue.pop();
			if (front_node->tree0_node_id == thisPair.first && front_node->tree1_node_id == thisPair.second)
			{
				first_flag = false;
			}
			if (front_node->tree0_node_id == thisPair.first)
			{
				second_flag = true;
			}
			if (front_node->tree1_node_id == thisPair.second)
			{
				third_flag = true;
			}
			for (int i = 0; i < front_node->children.size(); i++)
			{
				temp_queue.push(front_node->children[i]);
			}
		}
	}
	if (first_flag == true && second_flag == false && third_flag == true)
	{
		return true;
	}
	return false;
}

CompoundNode* CNodeMatch::ReturnParentNode(int index1, int index2)
{
	if (comb_tree)
	{
		std::queue<CompoundNode*> temp_queue;
		temp_queue.push(comb_tree);
		while (!temp_queue.empty())
		{
			CompoundNode* front_node = temp_queue.front();
			temp_queue.pop();
			if (front_node->tree0_node_id == index1 && front_node->tree1_node_id == index2)
			{
				return front_node;
			}
			for (int i = 0; i < front_node->children.size(); i++)
			{
				temp_queue.push(front_node->children[i]);
			}
		}
	}
	return nullptr;
}

CompoundNode * CNodeMatch::ReturnRightParentNode(int index2)
{
	CompoundNode* return_node = nullptr;

	if (comb_tree)
	{
		std::queue<CompoundNode*> temp_queue;
		temp_queue.push(comb_tree);
		while (!temp_queue.empty())
		{
			CompoundNode* front_node = temp_queue.front();
			temp_queue.pop();
			if (front_node->tree1_node_id == index2)
			{
				return_node = front_node;
			}
			for (int i = 0; i < front_node->children.size(); i++)
			{
				temp_queue.push(front_node->children[i]);
			}
		}
	}
	return return_node;
}

CompoundNode * CNodeMatch::ReturnLeftParentNode(int index1)
{
	CompoundNode* return_node = nullptr;
	if (comb_tree)
	{
		std::queue<CompoundNode*> temp_queue;
		temp_queue.push(comb_tree);
		while (!temp_queue.empty())
		{
			CompoundNode* front_node = temp_queue.front();
			temp_queue.pop();
			if (front_node->tree0_node_id == index1)
			{
				return_node = front_node;
			}
			for (int i = 0; i < front_node->children.size(); i++)
			{
				temp_queue.push(front_node->children[i]);
			}
		}
	}
	return return_node;
}

//Recursive Part
int CNodeMatch::ComputeCost(CNode* n1, CNode* n2)
{
    if (n1 == nullptr)
    {	
        if (recordCost.find(std::pair<int, int>(-n2->index, n2->index)) != recordCost.end())
        {
            return recordCost[std::pair<int, int>(-n2->index, n2->index)];
        }

        recordCorrespondence[std::pair<int, int>(-n2->index, n2->index)].insert(std::pair<int, int>(-n2->index, n2->index));

        if (n2->node_type == NODE_TYPE::LEAF)
        {
            int thisCost = LeafToNULLCost(n2);
            recordCost[std::pair<int, int>(-n2->index, n2->index)] = thisCost;
            return thisCost;
        }
		
		//process child node
        int totalCost = 0;
        for (auto ptr : n2->children)
        {
            totalCost += ComputeCost(nullptr, ptr);
            recordCorrespondence[std::pair<int, int>(-n2->index, n2->index)].insert(
                recordCorrespondence[std::pair<int, int>(-ptr->index, ptr->index)].begin(),
                recordCorrespondence[std::pair<int, int>(-ptr->index, ptr->index)].end());
        }

        recordCost[std::pair<int, int>(-n2->index, n2->index)] = totalCost;

        return totalCost;
    }
    else if (n2 == nullptr)
    {
        if (recordCost.find(std::pair<int, int>(n1->index, -n1->index))!=recordCost.end())
        {
            return recordCost[std::pair<int, int>(n1->index, -n1->index)];
        }

        recordCorrespondence[std::pair<int, int>(n1->index, -n1->index)].insert(std::pair<int, int>(n1->index, -n1->index));

        if (n1->node_type == NODE_TYPE::LEAF)
        {
            int thisCost = LeafToNULLCost(n1);
            recordCost[std::pair<int, int>(n1->index, -n1->index)] = thisCost;			

            return thisCost;
        }

        int totalCost = 0;
        for (auto ptr : n1->children)
        {
            totalCost += ComputeCost(ptr, nullptr);
            recordCorrespondence[std::pair<int, int>(n1->index, -n1->index)].insert(
                recordCorrespondence[std::pair<int, int>(ptr->index, -ptr->index)].begin(),
                recordCorrespondence[std::pair<int, int>(ptr->index, -ptr->index)].end());
        }
        recordCost[std::pair<int, int>(n1->index, -n1->index)] = totalCost;		

        return totalCost;
    }

    if (recordCost.find(std::pair<int, int>(n1->index, n2->index)) != recordCost.end())
    {
        return recordCost[std::pair<int, int>(n1->index, n2->index)];
    }

    recordCorrespondence[std::pair<int, int>(n1->index, n2->index)].insert(std::pair<int, int>(n1->index, n2->index));

    if (n1->node_type == NODE_TYPE::LEAF && n2->node_type == NODE_TYPE::LEAF)
    {
        int thisCost = LeafToLeafCost(n1, n2);
        recordCost[std::pair<int, int>(n1->index, n2->index)] = thisCost;
        return thisCost;
    }
	else if (n1->node_type == NODE_TYPE::LEAF && n2->node_type == NODE_TYPE::NONLEAF)
	{
		std::vector<CNode*> nodeSet1, nodeSet2;

		nodeSet1 = { n1 };
		nodeSet2 = n2->children;
		std::set<std::pair<int, int>> newCsp;

		int thisCost = HungaryCost(nodeSet1, nodeSet2, newCsp);

		bool without_corres = true;
		for (auto each_pair : newCsp)
		{
			if (each_pair.first >= 0 && each_pair.second >= 0)
			{
				without_corres = false;
				break;
			}
		}
		if (without_corres == true)
		{
			thisCost = 1e+9;
		}

		recordCorrespondence[std::pair<int, int>(n1->index, n2->index)].insert(newCsp.begin(), newCsp.end());
		recordCost[std::pair<int, int>(n1->index, n2->index)] = thisCost;

		return thisCost;
	}
    else if (n1->node_type == NODE_TYPE::NONLEAF && n2->node_type == NODE_TYPE::LEAF)
    {	
        std::vector<CNode*> nodeSet1, nodeSet2;

        nodeSet1 = n1->children;
        nodeSet2 = { n2 };
        std::set<std::pair<int, int>> newCsp;

        int thisCost = HungaryCost(nodeSet1, nodeSet2, newCsp);

		bool without_corres = true;
		for (auto each_pair : newCsp)
		{
			if (each_pair.first >=0 && each_pair.second >= 0)
			{
				without_corres = false;
				break;
			}			
		}			
		if (without_corres == true)
		{
			thisCost = 1e+9;
		}

        recordCorrespondence[std::pair<int, int>(n1->index, n2->index)].insert(newCsp.begin(), newCsp.end());
        recordCost[std::pair<int, int>(n1->index, n2->index)] = thisCost;

        return thisCost;
    }
	else if (n1->node_type == NODE_TYPE::NONLEAF && n2->node_type == NODE_TYPE::NONLEAF)
	{
		std::vector<CNode*> nodeSet1, nodeSet2;

		// case 1: children to children correspondence
		nodeSet1 = n1->children;
		nodeSet2 = n2->children;
		std::set<std::pair<int, int>> newCspCase1;
		int case1Cost = HungaryCost(nodeSet1, nodeSet2, newCspCase1);

		// case 2: n1 to n2's children
		nodeSet1 = { n1 };
		nodeSet2 = n2->children;
		std::set<std::pair<int, int>> newCspCase2;
		int case2Cost = HungaryCost(nodeSet1, nodeSet2, newCspCase2);

		// case 3: n1's children to n2
		nodeSet1 = n1->children;
		nodeSet2 = { n2 };
		std::set<std::pair<int, int>> newCspCase3;
		int case3Cost = HungaryCost(nodeSet1, nodeSet2, newCspCase3);

		int minimalCost = std::min(case1Cost, std::min(case2Cost, case3Cost));

		if (case1Cost == minimalCost)
		{
			bool without_corres = true;
			for (auto each_pair : newCspCase1)
			{
				if (each_pair.first >= 0 && each_pair.second >= 0)
				{
					without_corres = false;
					break;
				}
			}
			if (without_corres == true)
			{
				minimalCost = 1e+9;
			}
			recordCorrespondence[std::pair<int, int>(n1->index, n2->index)].insert(newCspCase1.begin(), newCspCase1.end());			
		}
		else if (case2Cost == minimalCost)
		{
			recordCorrespondence[std::pair<int, int>(n1->index, n2->index)].insert(newCspCase2.begin(), newCspCase2.end());			
		}
		else
		{
			recordCorrespondence[std::pair<int, int>(n1->index, n2->index)].insert(newCspCase3.begin(), newCspCase3.end());			
		}

		recordCost[std::pair<int, int>(n1->index, n2->index)] = minimalCost;

		return minimalCost;
	}

	return 0;
}

void CNodeMatch::ComputeCsp(CNode* tree1, CNode* tree2)
{
    this->tree0 = tree1;
    this->tree1 = tree2;
	two_tree_cost = ComputeCost(this->tree0, this->tree1);

	return;
}

void CNodeMatch::AssignNodeIndex()
{
	int start_index = 0;

	if (comb_tree)
	{
		std::queue<CompoundNode*> temp_queue;
		temp_queue.push(comb_tree);
		while (!temp_queue.empty())
		{
			CompoundNode *node = temp_queue.front();
			temp_queue.pop();

			node->index = start_index;
			start_index++;

			for (auto iter = node->children.begin(); iter != node->children.end(); iter++)
				temp_queue.push(*iter);
		}
	}

	std::cout << "************** check CombineTree infor **************" << std::endl;
	if (comb_tree)
	{
		std::queue<CompoundNode*> temp_queue;
		temp_queue.push(comb_tree);
		while (!temp_queue.empty())
		{
			CompoundNode* node = temp_queue.front();
			temp_queue.pop();

			if (node == comb_tree)
			{
				std::cout << "node index = " << node->index << "  tree0 index = " << node->tree0_node_id << "   tree1 index = " << node->tree1_node_id << std::endl;
			}
			else
			{
				std::cout << "node index = " << node->index << "  tree0 index = " << node->tree0_node_id << "   tree1 index = " << node->tree1_node_id << "   parent index = " << node->parent->index<<std::endl;
			}

			for (auto iter = node->children.begin(); iter != node->children.end(); iter++)
				temp_queue.push(*iter);
		}
	}
	std::cout << "************** check CombineTree infor **************" << std::endl;

	return;
}

void CNodeMatch::CreateCompoundTree()
{
	std::unordered_map<int, CNode*> first_idx_ptr_map;
	std::unordered_map<int, CNode*> second_idx_ptr_map;

	first_idx_ptr_map = first_layout->idx_ptr_map;
	second_idx_ptr_map = second_layout->idx_ptr_map;

	//new comb tree root
	comb_tree = new CompoundNode();	
	comb_tree->tree0_node = tree0;
	comb_tree->tree1_node = tree1;
	comb_tree->tree0_node_id = tree0->index;
	comb_tree->tree1_node_id = tree1->index;

	std::set<std::pair<int, int>> validCsp = recordCorrespondence[std::pair<int, int>(comb_tree->tree0_node_id, comb_tree->tree1_node_id)];

	std::cout << "************** check corres infor **************" << std::endl;
	for (auto i : validCsp)
	{
		std::cout << i.first << "    " << i.second << std::endl;
	}
	std::cout << "************** check corres infor **************" << std::endl;

	std::queue<CNode*> temp_queue;
	temp_queue.push(tree0);
	while (!temp_queue.empty())
	{
		CNode* front_node = temp_queue.front();
		temp_queue.pop();

		std::set<std::pair<int, int>> pair_first_with_front_node_index;

		for (auto i : validCsp)
		{
			if (i.first == front_node->index)
			{
				pair_first_with_front_node_index.insert(i);
			}
		}

		std::set<std::pair<int, int>> pair_first_with_front_node_index_copy = pair_first_with_front_node_index;

		while (pair_first_with_front_node_index_copy.size() != 0)
		{
			pair_first_with_front_node_index = pair_first_with_front_node_index_copy;

			//pair_first_with_front_node_index has two cases, node->node OR node->null
			for (auto j : pair_first_with_front_node_index)
			{
				//node->node
				if (j.first > 0 && j.second > 0)
				{					
					if (PairExistedInCombineTree(j, comb_tree) == true)
					{
						//for example: <1,1>
						pair_first_with_front_node_index_copy.erase(pair_first_with_front_node_index_copy.find(j));
						continue;
					}

					if (PairDoNotExistedInCombineTree(j, comb_tree))
					{
						//this pair doesn't exit in CombineTree,pair first and pair second don't exit in CombineTree too.
						//serach parent node
						int first_node_parent_index = first_idx_ptr_map[j.first]->parent->index;
						int second_node_parent_index = second_idx_ptr_map[j.second]->parent->index;
						CompoundNode* temp_parent_node = ReturnParentNode(first_node_parent_index, second_node_parent_index);

						if (temp_parent_node != nullptr)
						{
							CompoundNode* new_node = new CompoundNode();
							new_node->tree0_node_id = j.first;
							new_node->tree1_node_id = j.second;
							new_node->tree0_node = first_idx_ptr_map[new_node->tree0_node_id];
							new_node->tree1_node = second_idx_ptr_map[new_node->tree1_node_id];
							new_node->parent = temp_parent_node;
							temp_parent_node->children.push_back(new_node);			
							pair_first_with_front_node_index_copy.erase(pair_first_with_front_node_index_copy.find(j));
						}
					}
					else if (OnlyPairFirstExistedInCombineTree(j, comb_tree))
					{
						//this pair doesn't exit in CombineTree,and only pair first exits in CombineTree.
						int second_node_parent_index = second_idx_ptr_map[j.second]->parent->index;
						CompoundNode* temp_parent_node = ReturnRightParentNode(second_node_parent_index);
						if (temp_parent_node != nullptr)
						{
							CompoundNode* new_node = new CompoundNode();
							new_node->tree0_node_id = j.first;
							new_node->tree1_node_id = j.second;
							new_node->tree0_node = first_idx_ptr_map[new_node->tree0_node_id];
							new_node->tree1_node = second_idx_ptr_map[new_node->tree1_node_id];
							new_node->parent = temp_parent_node;
							temp_parent_node->children.push_back(new_node);							
							pair_first_with_front_node_index_copy.erase(pair_first_with_front_node_index_copy.find(j));
						}
					}
					else if (OnlyPairSecondExistedInCombineTree(j, comb_tree))
					{
						//this pair doesn't exit in CombineTree,and only pair second exits in CombineTree.												
						int first_node_parent_index = first_idx_ptr_map[j.first]->parent->index;
						CompoundNode* temp_parent_node = ReturnLeftParentNode(first_node_parent_index);
						if (temp_parent_node != nullptr)
						{
							CompoundNode* new_node = new CompoundNode();
							new_node->tree0_node_id = j.first;
							new_node->tree1_node_id = j.second;
							new_node->tree0_node = first_idx_ptr_map[new_node->tree0_node_id];
							new_node->tree1_node = second_idx_ptr_map[new_node->tree1_node_id];
							new_node->parent = temp_parent_node;
							temp_parent_node->children.push_back(new_node);							
							pair_first_with_front_node_index_copy.erase(pair_first_with_front_node_index_copy.find(j));
						}
					}
				}
				else
				{
					//node->null
					if (PairExistedInCombineTree(j, comb_tree) == true)
					{
						//this pair has exit in CombineTree						
						pair_first_with_front_node_index_copy.erase(pair_first_with_front_node_index_copy.find(j));
						continue;
					}

					int first_node_parent_index = first_idx_ptr_map[j.first]->parent->index;
					CompoundNode* temp_parent_node = ReturnLeftParentNode(first_node_parent_index);
					if (temp_parent_node != nullptr)
					{
						CompoundNode* new_node = new CompoundNode();
						new_node->tree0_node_id = j.first;
						new_node->tree1_node_id = -new_node->tree0_node_id;

						new_node->tree0_node = first_idx_ptr_map[new_node->tree0_node_id];
						new_node->tree1_node = nullptr;

						new_node->parent = temp_parent_node;
						temp_parent_node->children.push_back(new_node);						
						pair_first_with_front_node_index_copy.erase(pair_first_with_front_node_index_copy.find(j));
					}
				}
			}

		}

		for (int i = 0; i < front_node->children.size(); i++)
		{
			temp_queue.push(front_node->children[i]);
		}
	}

	//for null->node case
	std::set<std::pair<int, int>> remaining_pair;

	for (auto each_pair : validCsp)
	{
		if (each_pair.first < 0 && each_pair.second > 0)
		{
			remaining_pair.insert(each_pair);
		}
	}

	std::set<std::pair<int, int>> remaining_pair_copy = remaining_pair;
	while (remaining_pair_copy.size() != 0)
	{
		remaining_pair = remaining_pair_copy;
		for (auto each_pair : remaining_pair)
		{
			if (PairExistedInCombineTree(each_pair, comb_tree) == true)
			{
				//this pair has exit in CombineTree
				remaining_pair_copy.erase(remaining_pair_copy.find(each_pair));
				continue;
			}

			int second_node_parent_index = second_idx_ptr_map[each_pair.second]->parent->index;
			CompoundNode* temp_parent_node = ReturnRightParentNode(second_node_parent_index);
			if (temp_parent_node != nullptr)
			{
				CompoundNode* new_node = new CompoundNode();
				new_node->tree0_node_id = -each_pair.second;
				new_node->tree1_node_id = each_pair.second;

				new_node->tree0_node = nullptr;
				new_node->tree1_node = second_idx_ptr_map[new_node->tree1_node_id];

				new_node->parent = temp_parent_node;
				temp_parent_node->children.push_back(new_node);								
				remaining_pair_copy.erase(remaining_pair_copy.find(each_pair));				
			}
		}
	}
	return;
}

void CNodeMatch::SetLayoutTree(CLayoutTree* m_firstLayout, CLayoutTree* m_secondLayout)
{
	first_layout = m_firstLayout;
	second_layout = m_secondLayout;

	return;
}

void CNodeMatch::GetLeafNodes(CNode* node, std::vector<CNode*>& leaf_nodes)
{
	if (node)
	{
		if (node->children.size() == 0)
		{
			leaf_nodes.push_back(node);
		}
		for (int i = 0; i < node->children.size(); i++)
		{
			GetLeafNodes(node->children[i], leaf_nodes);
		}
		return;
	}
	else
	{
		std::cout << "this node == nullptr" << std::endl;
		return;
	}
}