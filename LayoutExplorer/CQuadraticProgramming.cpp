#include "CQuadraticProgramming.h"

Eigen::VectorXd SolveFunction(Eigen::MatrixXd H, Eigen::MatrixXd A, Eigen::VectorXd b, Eigen::VectorXd c)
{
	Eigen::MatrixXd H_Inverse, A_HInver_ATrans, Inverse_A_HInver_ATrans, A_HInver, G, B, C;
	Eigen::VectorXd result, lam, fval;

	H_Inverse = H.inverse();

	A_HInver_ATrans = A * H_Inverse * A.transpose();

	//generalized inverse matrix
	double epsilon = std::numeric_limits<double>::epsilon();
	Eigen::JacobiSVD< Eigen::MatrixXd > svd(A_HInver_ATrans, Eigen::ComputeFullU | Eigen::ComputeFullV);
	double tolerance = epsilon * std::max(A_HInver_ATrans.cols(), A_HInver_ATrans.rows()) * svd.singularValues().array().abs()(0);
	Inverse_A_HInver_ATrans = svd.matrixV() * (svd.singularValues().array().abs() > tolerance).select(svd.singularValues().array().inverse(), 0).matrix().asDiagonal() * svd.matrixU().adjoint();

	A_HInver = A * H_Inverse;

	C = -(Inverse_A_HInver_ATrans);

	G = H_Inverse - H_Inverse * A.transpose() * Inverse_A_HInver_ATrans * A_HInver;
	B = Inverse_A_HInver_ATrans * A_HInver;

	result = B.transpose() * b - G * c;
	lam = B * c - C * b;
	fval = 0.5 * result.transpose() * H * result + c.transpose() * result;

	return result;
}

bool CompoundCompareY(CompoundNode* a, CompoundNode* b)
{
	return a->y < b->y;
}

bool CompoundCompareX(CompoundNode* a, CompoundNode* b)
{
	return a->x < b->x;
}

void ConstructFunction(CompoundNode* node, double& slider_value)
{
	const double pos_weight=1.0;

	/*const int min_height = 100;
	const int min_width = 100;*/

	//sometimes min_height and min_width should be smaller to avoid elements' overlapping	
	const int min_height = 50;
	const int min_width = 50;
	const int min_interval = 20;

	double root_x = node->tree0_node->x*(1.0 - slider_value) + node->tree1_node->x*slider_value;
	double root_y = node->tree0_node->y*(1.0 - slider_value) + node->tree1_node->y*slider_value;
	double root_width = node->tree0_node->width*(1.0 - slider_value) + node->tree1_node->width*slider_value;
	double root_height = node->tree0_node->height*(1.0 - slider_value) + node->tree1_node->height*slider_value;

	std::vector<CompoundNode*> allnodes;
	std::queue<CompoundNode*> compound_q;
	std::unordered_map<CompoundNode*, int> ptr_idx_map;
	std::unordered_map<int, CompoundNode*> idx_ptr_map;
	
	int counts_equal_size_node = 0;
	int counts_left_align_node = 0;
	int counts_right_align_node = 0;
	int counts_top_align_node = 0;
	int counts_bottom_align_node = 0;

	compound_q.push(node);
	while (!compound_q.empty())
	{	
		CompoundNode* front_node = compound_q.front();
		
		idx_ptr_map[front_node->index] = front_node;
		ptr_idx_map[front_node] = allnodes.size();

		allnodes.push_back(front_node);
		compound_q.pop();
		
		for (auto ptr : allnodes.back()->children)
		{
			compound_q.push(ptr);
		}

		counts_equal_size_node += allnodes.back()->equal_size_node.size();

		if (allnodes.back()->left_align_node.size() > 0)
		{
			counts_left_align_node += allnodes.back()->left_align_node.size() - 1;
		}

		if (allnodes.back()->right_align_node.size() > 0)
		{
			counts_right_align_node += allnodes.back()->right_align_node.size()-1;
		}		

		if (allnodes.back()->top_align_node.size() > 0)
		{
			counts_top_align_node += allnodes.back()->top_align_node.size()-1;
		}
		
		if (allnodes.back()->bottom_align_node.size() > 0)
		{
			counts_bottom_align_node += allnodes.back()->bottom_align_node.size()-1;
		}		
	}

	// +4: for setting root's geomtry attributes
	int constraint_num = counts_equal_size_node * 2 + counts_left_align_node + counts_right_align_node
		+ counts_top_align_node + counts_bottom_align_node + 4;

	Eigen::MatrixXd H(allnodes.size() * 4, allnodes.size() * 4);
	H.setIdentity();	

	for (int i = 0; i < allnodes.size(); i++)
	{
		if (allnodes[i]->node_corres_type == NODE_CORRES_TYPE::T_AB)
		{			
			H(i * 4, i * 4) = 2 * H(i * 4, i * 4) * pos_weight;
			H(i * 4 + 1, i * 4 + 1) = 2 * H(i * 4 + 1, i * 4 + 1) * pos_weight;
			H(i * 4 + 2, i * 4 + 2) = 2 * H(i * 4 + 2, i * 4 + 2);
			H(i * 4 + 3, i * 4 + 3) = 2 * H(i * 4 + 3, i * 4 + 3);
		}
		else if (allnodes[i]->node_corres_type == NODE_CORRES_TYPE::A)
		{			
			H(i * 4, i * 4) = 2 * (1 - slider_value) * H(i * 4, i * 4) * pos_weight;
			H(i * 4 + 1, i * 4 + 1) = 2 * (1 - slider_value) * H(i * 4 + 1, i * 4 + 1) * pos_weight;
			H(i * 4 + 2, i * 4 + 2) = 2 * H(i * 4 + 2, i * 4 + 2);
			H(i * 4 + 3, i * 4 + 3) = 2 * H(i * 4 + 3, i * 4 + 3);
		}
		else if (allnodes[i]->node_corres_type == NODE_CORRES_TYPE::B)
		{		
			H(i * 4, i * 4) = 2 * slider_value * H(i * 4, i * 4) * pos_weight;
			H(i * 4 + 1, i * 4 + 1) = 2 * slider_value * H(i * 4 + 1, i * 4 + 1) * pos_weight;
			H(i * 4 + 2, i * 4 + 2) = 2 * H(i * 4 + 2, i * 4 + 2);
			H(i * 4 + 3, i * 4 + 3) = 2 * H(i * 4 + 3, i * 4 + 3);
		}
	}
	
	Eigen::VectorXd b(constraint_num);
	b.setZero();

	Eigen::MatrixXd A(constraint_num, 4 * allnodes.size());
	A.setZero();

	int row_count = 0;

	int root_order = ptr_idx_map[idx_ptr_map[node->index]];
	int x_location_root = root_order * 4 + 1 - 1;
	int y_location_root = root_order * 4 + 2 - 1;
	int width_location_root = root_order * 4 + 3 - 1;
	int height_location_root = root_order * 4 + 4 - 1;

	A(row_count, x_location_root) = 1;
	row_count++;
	b(0) = root_x;

	A(row_count, y_location_root) = 1;
	row_count++;
	b(1) = root_y;

	A(row_count, width_location_root) = 1;
	row_count++;
	b(2) = root_width;

	A(row_count, height_location_root) = 1;
	row_count++;
	b(3) = root_height;

	//Equal size
	for (int i = 0; i < allnodes.size(); i++)
	{	
		for (auto each_pair : allnodes[i]->equal_size_node)
		{
			int order_first = ptr_idx_map[idx_ptr_map[each_pair.first]];
			int order_second = ptr_idx_map[idx_ptr_map[each_pair.second]];

			int width_location_first = order_first * 4 + 3 - 1;
			int width_location_second = order_second * 4 + 3 - 1;

			A(row_count, width_location_first) = 1;
			A(row_count, width_location_second) = -1;

			row_count++;

			int height_location_first = order_first * 4 + 4 - 1;
			int height_location_second = order_second * 4 + 4 - 1;

			A(row_count, height_location_first) = 1;
			A(row_count, height_location_second) = -1;

			row_count++;
		}
	}
	
	//left_align
	for (int i = 0; i < allnodes.size(); i++)
	{
		if (allnodes[i]->left_align_node.size() > 0)
		{
			for (int j = 0; j < allnodes[i]->left_align_node.size()-1; j++)
			{
				int order_first = ptr_idx_map[idx_ptr_map[allnodes[i]->left_align_node[j]]];
				int order_second = ptr_idx_map[idx_ptr_map[allnodes[i]->left_align_node[j + 1]]];
				int x_location_first = order_first * 4 + 1 - 1;
				int x_location_second = order_second * 4 + 1 - 1;

				A(row_count, x_location_first) = 1;
				A(row_count, x_location_second) = -1;

				row_count++;
			}
		}	
	}

	//right_align
	for (int i = 0; i < allnodes.size(); i++)
	{
		if (allnodes[i]->right_align_node.size() > 0)
		{
			for (int j = 0; j < allnodes[i]->right_align_node.size() - 1; j++)
			{
				int order_first = ptr_idx_map[idx_ptr_map[allnodes[i]->right_align_node[j]]];
				int order_second = ptr_idx_map[idx_ptr_map[allnodes[i]->right_align_node[j + 1]]];

				int x_location_first = order_first * 4 + 1 - 1;
				int x_location_second = order_second * 4 + 1 - 1;
				int width_location_first = order_first * 4 + 3 - 1;
				int width_location_second = order_second * 4 + 3 - 1;

				A(row_count, x_location_first) = 1;
				A(row_count, x_location_second) = -1;
				A(row_count, width_location_first) = 1;
				A(row_count, width_location_second) = -1;

				row_count++;
			}
		}		
	}

	//top align
	for (int i = 0; i < allnodes.size(); i++)
	{	
		if (allnodes[i]->top_align_node.size() > 0)
		{
			for (int j = 0; j < allnodes[i]->top_align_node.size() - 1; j++)
			{
				int order_first = ptr_idx_map[idx_ptr_map[allnodes[i]->top_align_node[j]]];
				int order_second = ptr_idx_map[idx_ptr_map[allnodes[i]->top_align_node[j + 1]]];
				int y_location_first = order_first * 4 + 2 - 1;
				int y_location_second = order_second * 4 + 2 - 1;

				A(row_count, y_location_first) = 1;
				A(row_count, y_location_second) = -1;

				row_count++;
			}
		}		
	}
	
	//bottom align
	for (int i = 0; i < allnodes.size(); i++)
	{
		if (allnodes[i]->bottom_align_node.size() > 0)
		{
			for (int j = 0; j < allnodes[i]->bottom_align_node.size() - 1; j++)
			{
				int order_first = ptr_idx_map[idx_ptr_map[allnodes[i]->bottom_align_node[j]]];
				int order_second = ptr_idx_map[idx_ptr_map[allnodes[i]->bottom_align_node[j + 1]]];

				int y_location_first = order_first * 4 + 2 - 1;
				int y_location_second = order_second * 4 + 2 - 1;
				int h_location_first = order_first * 4 + 4 - 1;
				int h_location_second = order_second * 4 + 4 - 1;

				A(row_count, y_location_first) = 1;
				A(row_count, y_location_second) = -1;
				A(row_count, h_location_first) = 1;
				A(row_count, h_location_second) = -1;

				row_count++;
			}
		}		
	}
	
	Eigen::VectorXd c(4 * allnodes.size());
	for (int i = 0; i < allnodes.size(); ++i)
	{
		//store position and size infor
		std::vector<int> position_size_one;
		std::vector<int> position_size_two;
		
		if (allnodes[i]->node_corres_type == NODE_CORRES_TYPE::T_AB)
		{
			position_size_one.push_back(allnodes[i]->tree0_node->x);
			position_size_one.push_back(allnodes[i]->tree0_node->y);
			position_size_one.push_back(allnodes[i]->tree0_node->width);
			position_size_one.push_back(allnodes[i]->tree0_node->height);

			position_size_two.push_back(allnodes[i]->tree1_node->x);
			position_size_two.push_back(allnodes[i]->tree1_node->y);
			position_size_two.push_back(allnodes[i]->tree1_node->width);
			position_size_two.push_back(allnodes[i]->tree1_node->height);
		}
		else if (allnodes[i]->node_corres_type == NODE_CORRES_TYPE::A)
		{
			position_size_one.push_back(allnodes[i]->tree0_node->x);
			position_size_one.push_back(allnodes[i]->tree0_node->y);
			position_size_one.push_back(allnodes[i]->tree0_node->width);
			position_size_one.push_back(allnodes[i]->tree0_node->height);

			position_size_two.push_back(0);
			position_size_two.push_back(0);
			position_size_two.push_back(min_width);
			position_size_two.push_back(min_height);
		}
		else
		{
			position_size_one.push_back(0);
			position_size_one.push_back(0);
			position_size_one.push_back(min_width);
			position_size_one.push_back(min_height);

			position_size_two.push_back(allnodes[i]->tree1_node->x);
			position_size_two.push_back(allnodes[i]->tree1_node->y);
			position_size_two.push_back(allnodes[i]->tree1_node->width);
			position_size_two.push_back(allnodes[i]->tree1_node->height);
		}

		if (allnodes[i]->node_corres_type == NODE_CORRES_TYPE::T_AB)
		{
			c(4 * i + 0) = (-2 * (1 - slider_value) * position_size_one[0] - 2 * slider_value * position_size_two[0]) * pos_weight;
			c(4 * i + 1) = (-2 * (1 - slider_value) * position_size_one[1] - 2 * slider_value * position_size_two[1]) * pos_weight;
			c(4 * i + 2) = -2 * (1 - slider_value) * position_size_one[2] - 2 * slider_value * position_size_two[2];
			c(4 * i + 3) = -2 * (1 - slider_value) * position_size_one[3] - 2 * slider_value * position_size_two[3];
		}
		else if (allnodes[i]->node_corres_type == NODE_CORRES_TYPE::A)
		{
			for (int j = 0; j < 4; j++)
			{
				if (j < 2)
				{
					c(4 * i + j) = (-2 * (1 - slider_value) * position_size_one[j]) * pos_weight;
				}
				else
				{
					c(4 * i + j) = -2 * (1 - slider_value) * position_size_one[j] - 2 * slider_value * position_size_two[j];
				}
			}
		}
		else
		{
			for (int j = 0; j < 4; j++)
			{
				if (j < 2)
				{
					c(4 * i + j) = (-2 * slider_value * position_size_two[j]) * pos_weight;
				}
				else
				{
					c(4 * i + j) = -2 * (1 - slider_value) * position_size_one[j] - 2 * slider_value * position_size_two[j];
				}
			}
		}
	}

	clock_t start_solve, end_solve;
	double total_spent_time;
	start_solve = clock();

	//first solve
	auto solve_result = SolveFunction(H, A, b, c);

	for (int i = 0; i < solve_result.rows() / 4; i++)
	{	
		allnodes[i]->x = solve_result(4 * i);
		allnodes[i]->y = solve_result(4 * i + 1);
		allnodes[i]->width = solve_result(4 * i + 2);
		allnodes[i]->height = solve_result(4 * i + 3);
	}
	
	std::vector<std::pair<int, int>> A_expand_pair0;
	std::vector<std::pair<int, int>> A_expand_pair1;
	std::vector<std::pair<int, int>> A_expand_pair2;
	std::vector<std::pair<int, int>> A_expand_pair3;
	
	std::vector<int> b_expand;

	for (int i = 0; i < allnodes.size(); ++i)
	{
		int left_bound_idx = 0;
		int right_bound_idx = 0;
		int top_bound_idx = 0;
		int bottom_bound_idx = 0;

		for (int j = 0; j < allnodes[i]->children.size(); ++j)
		{			
			if (allnodes[i]->children[j]->x < allnodes[i]->children[left_bound_idx]->x)
			{
				left_bound_idx = j;
			}
			if (allnodes[i]->children[j]->y < allnodes[i]->children[top_bound_idx]->y)
			{
				top_bound_idx = j;
			}			
			if (allnodes[i]->children[j]->x >= allnodes[i]->children[right_bound_idx]->x)
			{
				right_bound_idx = j;
			}			
			if (allnodes[i]->children[j]->y >= allnodes[i]->children[bottom_bound_idx]->y)
			{
				bottom_bound_idx = j;
			}
		}

		if (!allnodes[i]->children.empty())
		{	
			//std::cout << "left" << left_bound_idx << std::endl;
			//std::cout << "right" << right_bound_idx << std::endl;
			//std::cout << "top" << top_bound_idx << std::endl;
			//std::cout << "bottom" << bottom_bound_idx << std::endl;

			left_bound_idx = ptr_idx_map[allnodes[i]->children[left_bound_idx]];
			right_bound_idx = ptr_idx_map[allnodes[i]->children[right_bound_idx]];
			top_bound_idx = ptr_idx_map[allnodes[i]->children[top_bound_idx]];
			bottom_bound_idx = ptr_idx_map[allnodes[i]->children[bottom_bound_idx]];

			A_expand_pair0.emplace_back(4 * left_bound_idx, 1);
			A_expand_pair1.emplace_back(4 * left_bound_idx + 2, 0);
			A_expand_pair2.emplace_back(4 * i, -1);
			A_expand_pair3.emplace_back(4 * i+2, 0);
			b_expand.push_back(0);

			A_expand_pair0.emplace_back(4 * right_bound_idx, 1);
			A_expand_pair1.emplace_back(4 * right_bound_idx+ 2, 1);
			A_expand_pair2.emplace_back(4 * i, -1);
			A_expand_pair3.emplace_back(4 * i + 2, -1);
			b_expand.push_back(0);

			A_expand_pair0.emplace_back(4 * top_bound_idx+1, 1);
			A_expand_pair1.emplace_back(4 * top_bound_idx + 3, 0);
			A_expand_pair2.emplace_back(4 * i+1, -1);
			A_expand_pair3.emplace_back(4 * i + 3, 0);
			b_expand.push_back(0);

			A_expand_pair0.emplace_back(4 * bottom_bound_idx + 1, 1);
			A_expand_pair1.emplace_back(4 * bottom_bound_idx + 3, 1);
			A_expand_pair2.emplace_back(4 * i + 1, -1);
			A_expand_pair3.emplace_back(4 * i + 3, -1);
			b_expand.push_back(0);

		}
	}
		
	int A_old_rows = A.rows();
	A.conservativeResize(A.rows() + A_expand_pair0.size(), Eigen::NoChange_t::NoChange);
	for (int i = A_old_rows; i < A.rows(); i++)
	{
		for (int j = 0; j < A.cols(); j++)
		{
			A(i, j) = 0;
		}
	}
	for (int i = 0; i < A_expand_pair0.size(); ++i)
	{	
		A(i + A_old_rows, A_expand_pair0[i].first) = A_expand_pair0[i].second;
		A(i + A_old_rows, A_expand_pair1[i].first) = A_expand_pair1[i].second;
		A(i + A_old_rows, A_expand_pair2[i].first) = A_expand_pair2[i].second;
		A(i + A_old_rows, A_expand_pair3[i].first) = A_expand_pair3[i].second;

	}
	
	int b_old_rows = b.rows();
	b.conservativeResize(b.rows() + b_expand.size());
	for (int i = b_old_rows; i < b.rows(); i++)
	{
		b(i) = b_expand[i - b_old_rows];
	}

	A_expand_pair0.clear();
	A_expand_pair1.clear();
	A_expand_pair2.clear();
	A_expand_pair3.clear();

	b_expand.clear();

	//add interval constraints
	for (int i = 0; i < allnodes.size(); ++i)
	{
		int final_interval = min_interval;
		
		if (allnodes[i]->top_align_node.size() > 1 || allnodes[i]->bottom_align_node.size() > 1)
		{
			std::vector<CompoundNode*> child_nodes = allnodes[i]->children;

			std::sort(child_nodes.begin(), child_nodes.end(), CompoundCompareX);	

			for (int j = 1; j < child_nodes.size(); ++j)
			{
				int thisIdx = ptr_idx_map[child_nodes[j]];
				int leftNodeIdx = ptr_idx_map[child_nodes[j - 1]];
				
				A_expand_pair0.emplace_back(4 * leftNodeIdx, -1);
				A_expand_pair1.emplace_back(4 * leftNodeIdx + 2, -1);
				A_expand_pair2.emplace_back(4 * thisIdx, 1);
				b_expand.push_back(final_interval);
				row_count++;
				
			}
		}
		else if (allnodes[i]->left_align_node.size() > 1 || allnodes[i]->right_align_node.size() > 1)
		{			
			std::vector<CompoundNode*> child_nodes = allnodes[i]->children;
			std::sort(child_nodes.begin(), child_nodes.end(), CompoundCompareY);
			
			for (int j = 1; j < child_nodes.size(); ++j)
			{
				int thisIdx = ptr_idx_map[child_nodes[j]];
				int upNodeIdx = ptr_idx_map[child_nodes[j - 1]];
				
				A_expand_pair0.emplace_back(4 * upNodeIdx + 1, -1);
				A_expand_pair1.emplace_back(4 * upNodeIdx + 3, -1);
				A_expand_pair2.emplace_back(4 * thisIdx + 1, 1);
				b_expand.push_back(final_interval);

				row_count++;				
			}
		}
		else
		{
			//std::cout << "Not reach Here." << std::endl;
		}
	}
	
	A_old_rows = A.rows();
	A.conservativeResize(A.rows() + A_expand_pair0.size(), Eigen::NoChange_t::NoChange);

	for (int i = A_old_rows; i < A.rows(); i++)
	{
		for (int j = 0; j < A.cols(); j++)
		{
			A(i, j) = 0;
		}
	}
	for (int i = 0; i < A_expand_pair0.size(); ++i)
	{
		A(i + A_old_rows, A_expand_pair0[i].first) = A_expand_pair0[i].second;
		A(i + A_old_rows, A_expand_pair1[i].first) = A_expand_pair1[i].second;
		A(i + A_old_rows, A_expand_pair2[i].first) = A_expand_pair2[i].second;
	}

	b_old_rows = b.rows();
	b.conservativeResize(b.rows() + b_expand.size());
	for (int i = b_old_rows; i < b.rows(); i++)
	{
		b(i) = b_expand[i - b_old_rows];
	}

	auto final_solve_result = SolveFunction(H, A, b, c);
	
	for (int i = 0; i < final_solve_result.rows() / 4; i++)
	{
		allnodes[i]->x = final_solve_result(4 * i);
		allnodes[i]->y = final_solve_result(4 * i + 1);
		allnodes[i]->width = final_solve_result(4 * i + 2);
		allnodes[i]->height = final_solve_result(4 * i + 3);
	}

	std::vector<int> less_height_thresh_vector;
	std::vector<int> less_width_thresh_vector;
	for (int i = 0; i < final_solve_result.rows() / 4; i++)
	{
		if (int(final_solve_result(4 * i + 3) + 0.5) < min_height)
		{
			less_height_thresh_vector.push_back(i);
		}
		if (int(final_solve_result(4 * i + 2) + 0.5) < min_width)
		{
			less_width_thresh_vector.push_back(i);
		}
	}

	do
	{	
		A_old_rows = A.rows();
		A.conservativeResize(A.rows() + less_width_thresh_vector.size() + less_height_thresh_vector.size(),Eigen::NoChange_t::NoChange);
		for (int i = A.rows() - (less_width_thresh_vector.size() + less_height_thresh_vector.size()); i < A.rows(); i++)
		{
			for (int j = 0; j < A.cols(); j++)
			{
				A(i, j) = 0;
			}
		}

		b.conservativeResize(b.rows() + less_width_thresh_vector.size() + less_height_thresh_vector.size());
		for (int i = b.rows() - (less_width_thresh_vector.size() + less_height_thresh_vector.size()); i < b.rows(); i++)
		{
			b(i) = 0;
		}
		for (int i = 0; i < less_height_thresh_vector.size(); i++)
		{
			A(i+A_old_rows, 4 * less_height_thresh_vector[i] + 3) = 1;
			b(i + A_old_rows) = min_height;
		}

		for (int i = 0; i < less_width_thresh_vector.size(); i++)
		{
			A(i + A_old_rows+ less_height_thresh_vector.size(), 4 * less_width_thresh_vector[i] + 2) = 1;
			b(i + A_old_rows + less_height_thresh_vector.size()) = min_width;
		}

		final_solve_result = SolveFunction(H, A, b, c);

		less_width_thresh_vector.clear();
		less_height_thresh_vector.clear();

		for (int i = 0; i < final_solve_result.rows() / 4; i++)
		{
			if (int(final_solve_result(4 * i + 3) + 0.5) < min_height)
			{
				less_height_thresh_vector.push_back(i);
			}
			if (int(final_solve_result(4 * i + 2) + 0.5) < min_width)
			{
				less_width_thresh_vector.push_back(i);
			}
		}		
	} while (less_width_thresh_vector.size()+ less_height_thresh_vector.size() > 0);

	for (int i = 0; i < final_solve_result.rows() / 4; i++)
	{
		allnodes[i]->x = final_solve_result(4 * i);
		allnodes[i]->y = final_solve_result(4 * i + 1);
		allnodes[i]->width = final_solve_result(4 * i + 2);
		allnodes[i]->height = final_solve_result(4 * i + 3);
	}

	end_solve = clock();
	total_spent_time = (double)(start_solve - end_solve) / CLOCKS_PER_SEC;
	//std::cout << "each solve spend time = " << total_spent_time<<std::endl;

	return;
}