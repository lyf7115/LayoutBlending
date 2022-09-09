#include "LayoutExplorer.h"

LayoutExplorer::LayoutExplorer(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	m_firstLayout = nullptr;
	m_secondLayout = nullptr;

	combine_tree = nullptr;
	combine_tree_copy = nullptr;

	comb_tree_root = nullptr;
	comb_tree_root_copy = nullptr;

	ui.horizontalSlider->setRange(0.0, 1.02);

	connect(ui.horizontalSlider, &QSlider::sliderMoved, this, &LayoutExplorer::MovedAndCreate);
	connect(ui.horizontalSlider, &QSlider::sliderMoved, this, &LayoutExplorer::OutputWidgetRedraw);
}

LayoutExplorer::~LayoutExplorer()
{
	if (m_firstLayout != nullptr)
	{
		delete m_firstLayout;
		m_firstLayout = nullptr;
	}

	if (m_secondLayout != nullptr)
	{
		delete m_secondLayout;
		m_secondLayout = nullptr;
	}

	if (combine_tree != nullptr)
	{
		delete combine_tree;
		combine_tree = nullptr;
	}

	if (combine_tree_copy != nullptr)
	{
		delete combine_tree_copy;
		combine_tree_copy = nullptr;
	}
}

void LayoutExplorer::OutputWidgetRedraw()
{
	//active paintEvent，repaint
	ui.output_widget->repaint();
}

void LayoutExplorer::OpenNextLayout()
{
	static const QString defaultPath("DEFAULT_LAYOUT_FILE_PATH");
	QSettings thisSetting;
	QString path = thisSetting.value(defaultPath).toString();
	QString filter = "(*.lay);;(*.*)";

	QString file_name = QFileDialog::getOpenFileName(this, tr("Open layout"), path, filter);
	std::string fileName = file_name.toStdString();

	if (fileName.length() > 0)
	{
		if (m_uploadLayout.size() == 2)
		{
			ui.output_widget->draw_layout_state = false;
			for (int i = 0; i < 2; i++)
			{
				if (m_uploadLayout[i] != nullptr)
				{
					delete m_uploadLayout[i];
					m_uploadLayout[i] = nullptr;
				}
			}
			m_uploadLayout.clear();
		}

		m_uploadLayout.push_back((new CLayoutTree()));
		m_uploadLayout.back()->ReadFromFile(fileName);

		std::string name = fileName;
		QDir crtDir;
		size_t found = name.find_last_of('/');
		name = name.substr(0, found);
		QString nameQ = QString::fromStdString(name);
		thisSetting.setValue(defaultPath, crtDir.absoluteFilePath(nameQ));
	}
	return;
}

void LayoutExplorer::Compute()
{
	if (m_uploadLayout.size() == 2)
	{
		m_firstLayout = m_uploadLayout[0];
		m_secondLayout = m_uploadLayout[1];

		CNodeMatch nodematching;
		nodematching.SetLayoutTree(m_firstLayout, m_secondLayout);
		nodematching.ComputeCsp(m_firstLayout->GetRoot(), m_secondLayout->GetRoot());
		std::cout << "two tree corres cost = " << nodematching.getCost() << std::endl;

		//Create Compound Tree
		nodematching.CreateCompoundTree();
		comb_tree_root = nodematching.GetCombineTree();

		nodematching.AssignNodeIndex();
		layout_handler.SetCombineTreeRoot(comb_tree_root);
		layout_handler.CalDeleteValue(comb_tree_root);
		std::cout << "compute finish" << std::endl;

		combine_tree = new CCompoundTree(comb_tree_root);
	}
	else
	{
		std::cout << "input layout file < 2" << std::endl;
	}
}

void LayoutExplorer::MovedAndCreate()
{
	if (comb_tree_root == nullptr)
	{
		return;
	}

	CopyTree();
	DeterInterVar(ui.horizontalSlider->value());
	layout_handler.TransferAndSolve(ui.horizontalSlider->value());
	SetCompoundNodeLabel();

	ui.output_widget->draw_layout_state = true;
	ui.output_widget->output_widget_tree = comb_tree_root_copy;

	return;
}

void LayoutExplorer::BatchCreateLay()
{
	/*for (int i = 0;i <= 100;i++)
	{
		double slider_val = i * 1.0 / 100.0;
		CopyTree();
		DeterInterVar(slider_val);
		layout_handler.TransferAndSolve(slider_val);
		SetCompoundNodeLabel();
		SaveGenerLayoutForBatch(i);
	}*/

	/*for (int i = 0; i <= 200; i++)
	{
		double slider_val = i * 1.0 / 200.0;
		CopyTree();
		DeterInterVar(slider_val);
		layout_handler.TransferAndSolve(slider_val);
		SetCompoundNodeLabel();
		SaveGenerLayoutForBatch(i);
	}*/

	std::vector<double> input_coefficient = { 0.0,0.25,0.5,0.75,1.0 };
	for (int i = 0; i < input_coefficient.size(); i++)
	{
		double slider_val = input_coefficient[i];
		CopyTree();
		DeterInterVar(slider_val);
		layout_handler.TransferAndSolve(slider_val);
		SetCompoundNodeLabel();
		SaveGenerLayoutForBatch(i);

		ui.output_widget->output_widget_tree = comb_tree_root_copy;
		BatchSaveLayoutAsSvg(i);
	}

	return;
}

void LayoutExplorer::SaveGenerLayout()
{
	std::vector<int> transfer_index;

	transfer_index.resize(500, 0);
	int index_counter = 1;

	QString fileName;
	fileName = QFileDialog::getSaveFileName(this, "Save", "", "Lay (*.lay)");

	std::fstream File_output;
	File_output.open(fileName.toStdString(), std::fstream::out);
	File_output << "node attribute" << std::endl;

	CompoundNode* root_display_layout = comb_tree_root_copy;
	int node_number = 0, equal_size_number = 0, equal_space_number = 0, left_align_number = 0, right_align_number = 0, top_align_number = 0, bottom_align_number = 0;

	if (root_display_layout)
	{
		std::queue<CompoundNode*> temp_queue;
		temp_queue.push(root_display_layout);

		while (!temp_queue.empty())
		{
			CompoundNode* node = temp_queue.front();
			temp_queue.pop();

			node_number++;
			equal_size_number += node->equal_size_node.size();
			if (node->equal_space_node.node_index.size() > 2)
			{
				equal_space_number++;
			}
			if (node->left_align_node.size() >= 2)
			{
				left_align_number++;
			}
			if (node->right_align_node.size() >= 2)
			{
				right_align_number++;
			}
			if (node->top_align_node.size() >= 2)
			{
				top_align_number++;
			}
			if (node->bottom_align_node.size() >= 2)
			{
				bottom_align_number++;
			}

			transfer_index[node->index] = index_counter;
			index_counter++;

			for (auto iter = node->children.begin(); iter != node->children.end(); iter++)
				temp_queue.push(*iter);
		}
	}

	File_output << node_number << std::endl;

	if (root_display_layout)
	{
		std::queue<CompoundNode*> temp_queue;
		temp_queue.push(root_display_layout);
		while (!temp_queue.empty())
		{
			CompoundNode* node = temp_queue.front();
			temp_queue.pop();

			if (node->index == 0)
			{
				if (node->children.size() == 0)
				{
					File_output << transfer_index[node->index] << " " << 0 << " " << 0 << " " << (int)node->x << " " << (int)node->y << " " << (int)node->width << " " << (int)node->height << " " << NodePresentTypeToInteger(node) << " " << 0 << std::endl;
				}
				else
				{
					File_output << transfer_index[node->index] << " " << 0 << " " << 0 << " " << (int)node->x << " " << (int)node->y << " " << (int)node->width << " " << (int)node->height << " " << 0 << " " << 0 << std::endl;
				}
			}
			else
			{
				if (node->children.size() == 0)
				{
					File_output << transfer_index[node->index] << " " << 0 << " " << 0 << " " << (int)node->x << " " << (int)node->y << " " << (int)node->width << " " << (int)node->height << " " << NodePresentTypeToInteger(node) << " " << transfer_index[node->parent->index] << std::endl;
				}
				else
				{
					File_output << transfer_index[node->index] << " " << 0 << " " << 0 << " " << (int)node->x << " " << (int)node->y << " " << (int)node->width << " " << (int)node->height << " " << 0 << " " << transfer_index[node->parent->index] << std::endl;
				}
			}

			for (auto iter = node->children.begin(); iter != node->children.end(); iter++)
				temp_queue.push(*iter);
		}
	}

	File_output << "equal size" << std::endl;
	File_output << equal_size_number << std::endl;
	if (root_display_layout)
	{
		std::queue<CompoundNode*> temp_queue;
		temp_queue.push(root_display_layout);
		while (!temp_queue.empty())
		{
			CompoundNode* node = temp_queue.front();
			temp_queue.pop();
			if (node->equal_size_node.size() > 0)
			{
				for (auto each_pair : node->equal_size_node)
				{
					File_output << transfer_index[each_pair.first] << " " << transfer_index[each_pair.second] << " " << transfer_index[node->index] << std::endl;
				}
			}
			for (auto iter = node->children.begin(); iter != node->children.end(); iter++)
				temp_queue.push(*iter);
		}
	}

	File_output << "equal space" << std::endl;
	File_output << equal_space_number << std::endl;
	if (root_display_layout)
	{
		std::queue<CompoundNode*> temp_queue;
		temp_queue.push(root_display_layout);

		while (!temp_queue.empty())
		{
			CompoundNode* node = temp_queue.front();
			temp_queue.pop();
			if (node->equal_space_node.node_index.size() > 2)
			{
				for (int i = 0; i < node->equal_space_node.node_index.size(); i++)
				{
					File_output << transfer_index[node->equal_space_node.node_index[i]] << " ";
				}

				if (node->top_align_node.size() > 1 || node->bottom_align_node.size() > 1)
				{
					File_output << 0 << " ";
				}
				else
				{
					File_output << 1 << " ";
				}

				File_output << transfer_index[node->index] << std::endl;
			}
			for (auto iter = node->children.begin(); iter != node->children.end(); iter++)
				temp_queue.push(*iter);
		}
	}

	File_output << "left align" << std::endl;
	File_output << left_align_number << std::endl;
	if (root_display_layout)
	{
		std::queue<CompoundNode*> temp_queue;
		temp_queue.push(root_display_layout);

		while (!temp_queue.empty())
		{
			CompoundNode* node = temp_queue.front();
			temp_queue.pop();

			if (node->left_align_node.size() >= 2)
			{
				for (int i = 0; i < node->left_align_node.size(); i++)
				{
					File_output << transfer_index[node->left_align_node[i]] << " ";
				}
				File_output << transfer_index[node->index] << std::endl;
			}

			for (auto iter = node->children.begin(); iter != node->children.end(); iter++)
				temp_queue.push(*iter);
		}
	}

	File_output << "right align" << std::endl;
	File_output << right_align_number << std::endl;
	if (root_display_layout)
	{
		std::queue<CompoundNode*> temp_queue;
		temp_queue.push(root_display_layout);
		while (!temp_queue.empty())
		{
			CompoundNode* node = temp_queue.front();
			temp_queue.pop();
			if (node->right_align_node.size() >= 2)
			{
				for (int i = 0; i < node->right_align_node.size(); i++)
				{
					File_output << transfer_index[node->right_align_node[i]] << " ";
				}
				File_output << transfer_index[node->index] << std::endl;
			}
			for (auto iter = node->children.begin(); iter != node->children.end(); iter++)
				temp_queue.push(*iter);
		}
	}

	File_output << "top align" << std::endl;
	File_output << top_align_number << std::endl;
	if (root_display_layout)
	{
		std::queue<CompoundNode*> temp_queue;
		temp_queue.push(root_display_layout);
		while (!temp_queue.empty())
		{
			CompoundNode* node = temp_queue.front();
			temp_queue.pop();
			if (node->top_align_node.size() >= 2)
			{
				for (int i = 0; i < node->top_align_node.size(); i++)
				{
					File_output << transfer_index[node->top_align_node[i]] << " ";
				}
				File_output << transfer_index[node->index] << std::endl;
			}
			for (auto iter = node->children.begin(); iter != node->children.end(); iter++)
				temp_queue.push(*iter);
		}
	}

	int bottom_align_counter = 0;
	File_output << "bottom align" << std::endl;
	File_output << bottom_align_number << std::endl;
	if (root_display_layout)
	{
		std::queue<CompoundNode*> temp_queue;
		temp_queue.push(root_display_layout);
		while (!temp_queue.empty())
		{
			CompoundNode* node = temp_queue.front();
			temp_queue.pop();

			if (node->bottom_align_node.size() >= 2)
			{
				bottom_align_counter++;
				if (bottom_align_counter == bottom_align_number)
				{
					for (int i = 0; i < node->bottom_align_node.size(); i++)
					{
						File_output << transfer_index[node->bottom_align_node[i]] << " ";
					}
					File_output << transfer_index[node->index];
				}
				else
				{
					for (int i = 0; i < node->bottom_align_node.size(); i++)
					{
						File_output << transfer_index[node->bottom_align_node[i]] << " ";
					}
					File_output << transfer_index[node->index] << std::endl;
				}
			}
			for (auto iter = node->children.begin(); iter != node->children.end(); iter++)
				temp_queue.push(*iter);
		}
	}
	File_output.close();

	return;
}

void LayoutExplorer::BatchSaveLayoutAsSvg(int order)
{
	QString fileName;
	//对于批处理设一个固定的输出路径
	QString prefix = "C:/Users/Administrator/Desktop/SIGA2022/SameStructureWithoutSameLabel/change_part_labels/";

	QString num_to_qstring = QString::number(order, 10);
	QString houzui = ".svg";
	fileName = prefix + num_to_qstring + houzui;

	if (fileName.length() > 0)
	{
		QSvgGenerator generator;
		generator.setFileName(fileName);
		generator.setSize(QSize(ui.output_widget->width(), ui.output_widget->height()));
		generator.setViewBox(QRect(ui.output_widget->x(), ui.output_widget->y(), ui.output_widget->width(), ui.output_widget->height()));
		generator.setTitle("SVG Generator Example Drawing");
		QPainter painter;
		painter.begin(&generator);
		this->window()->render(&painter);
		painter.end();
	}
	return;
}

int LayoutExplorer::NodePresentTypeToInteger(CompoundNode * input_node)
{
	if (input_node == nullptr)
	{
		return -1;
	}

	if (input_node->node_present_type == NODE_PRESENT_TYPE::NONLABEL)
	{
		return 0;
	}
	else if (input_node->node_present_type == NODE_PRESENT_TYPE::PICTURE)
	{
		return 1;
	}
	else if (input_node->node_present_type == NODE_PRESENT_TYPE::TEXT)
	{
		return 2;
	}
	else if (input_node->node_present_type == NODE_PRESENT_TYPE::PADDING)
	{
		return 3;
	}
	else if (input_node->node_present_type == NODE_PRESENT_TYPE::TITLE)
	{
		return 4;
	}
	else
	{
		std::cout << "need add node label" << std::endl;
	}

	return -1;
}

void LayoutExplorer::SaveGenerLayoutForBatch(int order)
{
	std::vector<int> transfer_index;
	transfer_index.resize(500, 0);
	int index_counter = 1;

	QString fileName;
	QString prefix = "C:/Users/Administrator/Desktop/SIGA2022/SameStructureWithoutSameLabel/change_part_labels/";

	QString num_to_qstring = QString::number(order, 10);
	QString houzui = ".lay";
	fileName = prefix + num_to_qstring + houzui;

	std::fstream File_output;
	File_output.open(fileName.toStdString(), std::fstream::out);
	File_output << "node attribute" << std::endl;

	CompoundNode* root_display_layout = comb_tree_root_copy;
	int node_number = 0, equal_size_number = 0, equal_space_number = 0, left_align_number = 0, right_align_number = 0, top_align_number = 0, bottom_align_number = 0;

	if (root_display_layout)
	{
		std::queue<CompoundNode*> temp_queue;
		temp_queue.push(root_display_layout);

		while (!temp_queue.empty())
		{
			CompoundNode* node = temp_queue.front();
			temp_queue.pop();

			node_number++;
			equal_size_number += node->equal_size_node.size();
			if (node->equal_space_node.node_index.size() > 2)
			{
				equal_space_number++;
			}
			if (node->left_align_node.size() >= 2)
			{
				left_align_number++;
			}
			if (node->right_align_node.size() >= 2)
			{
				right_align_number++;
			}
			if (node->top_align_node.size() >= 2)
			{
				top_align_number++;
			}
			if (node->bottom_align_node.size() >= 2)
			{
				bottom_align_number++;
			}

			transfer_index[node->index] = index_counter;
			index_counter++;

			for (auto iter = node->children.begin(); iter != node->children.end(); iter++)
				temp_queue.push(*iter);
		}
	}

	File_output << node_number << std::endl;

	if (root_display_layout)
	{
		std::queue<CompoundNode*> temp_queue;
		temp_queue.push(root_display_layout);

		while (!temp_queue.empty())
		{
			CompoundNode* node = temp_queue.front();
			temp_queue.pop();

			if (node->index == 0)
			{
				if (node->children.size() == 0)
				{
					File_output << transfer_index[node->index] << " " << 0 << " " << 0 << " " << (int)node->x << " " << (int)node->y << " " << (int)node->width << " " << (int)node->height << " " << NodePresentTypeToInteger(node) << " " << 0 << std::endl;
				}
				else
				{
					File_output << transfer_index[node->index] << " " << 0 << " " << 0 << " " << (int)node->x << " " << (int)node->y << " " << (int)node->width << " " << (int)node->height << " " << 0 << " " << 0 << std::endl;
				}
			}
			else
			{
				if (node->children.size() == 0)
				{
					File_output << transfer_index[node->index] << " " << 0 << " " << 0 << " " << (int)node->x << " " << (int)node->y << " " << (int)node->width << " " << (int)node->height << " " << NodePresentTypeToInteger(node) << " " << transfer_index[node->parent->index] << std::endl;
				}
				else
				{
					File_output << transfer_index[node->index] << " " << 0 << " " << 0 << " " << (int)node->x << " " << (int)node->y << " " << (int)node->width << " " << (int)node->height << " " << 0 << " " << transfer_index[node->parent->index] << std::endl;
				}
			}

			for (auto iter = node->children.begin(); iter != node->children.end(); iter++)
				temp_queue.push(*iter);
		}
	}

	File_output << "equal size" << std::endl;
	File_output << equal_size_number << std::endl;
	if (root_display_layout)
	{
		std::queue<CompoundNode*> temp_queue;
		temp_queue.push(root_display_layout);

		while (!temp_queue.empty())
		{
			CompoundNode* node = temp_queue.front();
			temp_queue.pop();
			if (node->equal_size_node.size() > 0)
			{
				for (auto each_pair : node->equal_size_node)
				{
					File_output << transfer_index[each_pair.first] << " " << transfer_index[each_pair.second] << " " << transfer_index[node->index] << std::endl;
				}
			}
			for (auto iter = node->children.begin(); iter != node->children.end(); iter++)
				temp_queue.push(*iter);
		}
	}

	File_output << "equal space" << std::endl;
	File_output << equal_space_number << std::endl;
	if (root_display_layout)
	{
		std::queue<CompoundNode*> temp_queue;
		temp_queue.push(root_display_layout);

		while (!temp_queue.empty())
		{
			CompoundNode* node = temp_queue.front();
			temp_queue.pop();

			if (node->equal_space_node.node_index.size() > 2)
			{
				for (int i = 0; i < node->equal_space_node.node_index.size(); i++)
				{
					File_output << transfer_index[node->equal_space_node.node_index[i]] << " ";
				}

				if (node->top_align_node.size() > 1 || node->bottom_align_node.size() > 1)
				{
					File_output << 0 << " ";
				}
				else
				{
					File_output << 1 << " ";
				}

				File_output << transfer_index[node->index] << std::endl;
			}
			for (auto iter = node->children.begin(); iter != node->children.end(); iter++)
				temp_queue.push(*iter);
		}
	}

	File_output << "left align" << std::endl;
	File_output << left_align_number << std::endl;
	if (root_display_layout)
	{
		std::queue<CompoundNode*> temp_queue;
		temp_queue.push(root_display_layout);
		while (!temp_queue.empty())
		{
			CompoundNode* node = temp_queue.front();
			temp_queue.pop();
			if (node->left_align_node.size() >= 2)
			{
				for (int i = 0; i < node->left_align_node.size(); i++)
				{
					File_output << transfer_index[node->left_align_node[i]] << " ";
				}
				File_output << transfer_index[node->index] << std::endl;
			}
			for (auto iter = node->children.begin(); iter != node->children.end(); iter++)
				temp_queue.push(*iter);
		}
	}

	File_output << "right align" << std::endl;
	File_output << right_align_number << std::endl;
	if (root_display_layout)
	{
		std::queue<CompoundNode*> temp_queue;
		temp_queue.push(root_display_layout);
		while (!temp_queue.empty())
		{
			CompoundNode* node = temp_queue.front();
			temp_queue.pop();
			if (node->right_align_node.size() >= 2)
			{
				for (int i = 0; i < node->right_align_node.size(); i++)
				{
					File_output << transfer_index[node->right_align_node[i]] << " ";
				}
				File_output << transfer_index[node->index] << std::endl;
			}
			for (auto iter = node->children.begin(); iter != node->children.end(); iter++)
				temp_queue.push(*iter);
		}
	}

	File_output << "top align" << std::endl;
	File_output << top_align_number << std::endl;
	if (root_display_layout)
	{
		std::queue<CompoundNode*> temp_queue;
		temp_queue.push(root_display_layout);

		while (!temp_queue.empty())
		{
			CompoundNode* node = temp_queue.front();
			temp_queue.pop();
			if (node->top_align_node.size() >= 2)
			{
				for (int i = 0; i < node->top_align_node.size(); i++)
				{
					File_output << transfer_index[node->top_align_node[i]] << " ";
				}
				File_output << transfer_index[node->index] << std::endl;
			}
			for (auto iter = node->children.begin(); iter != node->children.end(); iter++)
				temp_queue.push(*iter);
		}
	}

	int bottom_align_counter = 0;
	File_output << "bottom align" << std::endl;
	File_output << bottom_align_number << std::endl;
	if (root_display_layout)
	{
		std::queue<CompoundNode*> temp_queue;
		temp_queue.push(root_display_layout);
		while (!temp_queue.empty())
		{
			CompoundNode* node = temp_queue.front();
			temp_queue.pop();

			if (node->bottom_align_node.size() >= 2)
			{
				bottom_align_counter++;
				if (bottom_align_counter == bottom_align_number)
				{
					for (int i = 0; i < node->bottom_align_node.size(); i++)
					{
						File_output << transfer_index[node->bottom_align_node[i]] << " ";
					}
					File_output << transfer_index[node->index];
				}
				else
				{
					for (int i = 0; i < node->bottom_align_node.size(); i++)
					{
						File_output << transfer_index[node->bottom_align_node[i]] << " ";
					}
					File_output << transfer_index[node->index] << std::endl;
				}


			}
			for (auto iter = node->children.begin(); iter != node->children.end(); iter++)
				temp_queue.push(*iter);
		}
	}

	File_output.close();

	return;
}


void LayoutExplorer::SetCompoundNodeLabel()
{
	if (comb_tree_root_copy)
	{
		std::queue<CompoundNode*> temp_queue;
		temp_queue.push(comb_tree_root_copy);

		while (!temp_queue.empty())
		{
			CompoundNode* node = temp_queue.front();
			temp_queue.pop();

			if (node->children.size() == 0)
			{
				if (node->tree0_node != nullptr && node->tree1_node != nullptr)
				{
					if (node->tree0_node->node_present_type == node->tree1_node->node_present_type)
					{
						node->node_present_type = node->tree0_node->node_present_type;
					}
					else
					{
						std::cout << "have this case?,compound node with different label" << std::endl;
					}
				}
				else if (node->tree0_node != nullptr && node->tree1_node == nullptr)
				{
					node->node_present_type = node->tree0_node->node_present_type;
				}
				else
				{
					node->node_present_type = node->tree1_node->node_present_type;
				}

				if (node->x < 0 || node->y < 0 || node->width < 0 || node->height < 0)
				{
					std::cout << "solve wrong...." << std::endl;
				}
			}

			for (auto iter = node->children.begin(); iter != node->children.end(); iter++)
				temp_queue.push(*iter);
		}
	}
	return;
}

void LayoutExplorer::SaveLayoutAsSvg()
{
	QString fileName = QFileDialog::getSaveFileName(this, "Save", "", "SVG (*.svg)");

	if (fileName.length() > 0)
	{
		QSvgGenerator generator;
		generator.setFileName(fileName);
		generator.setSize(QSize(ui.output_widget->width(), ui.output_widget->height()));
		generator.setViewBox(QRect(ui.output_widget->x(), ui.output_widget->y(), ui.output_widget->width(), ui.output_widget->height()));
		generator.setTitle("SVG Generator Example Drawing");
		QPainter painter;
		painter.begin(&generator);
		this->window()->render(&painter);
		painter.end();
	}
}

void LayoutExplorer::CopyTree()
{
	if (combine_tree_copy != nullptr)
	{
		delete combine_tree_copy;
		combine_tree_copy = nullptr;
	}

	comb_tree_root_copy = comb_tree_root->DeepCopy();
	combine_tree_copy = new CCompoundTree(comb_tree_root_copy);

	return;
}

void LayoutExplorer::DeterInterVar(double slider_value)
{
	layout_handler.SetCombineTreeRoot(comb_tree_root_copy);
	layout_handler.DeterInterVariable(slider_value);
	return;
}
