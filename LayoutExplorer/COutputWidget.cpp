#include "COutputWidget.h"
#include<queue>

OutputWidget::OutputWidget(QWidget *parent)
	: QWidget(parent)
{	
	output_widget_tree = nullptr;
	draw_layout_state = true;
}

OutputWidget::~OutputWidget()
{

}

void OutputWidget::paintEvent(QPaintEvent* event)
{
	QColor shape_color = QColor(45, 131, 209,100);

	QPainter paint(this);
	DrawFrame(paint);
	DrawGrid(paint);	
	if(draw_layout_state == true)
	{
		DrawLayout(paint);
	}
	
}

void OutputWidget::DrawFrame(QPainter& paint)
{
	paint.save();
	paint.setPen(Qt::white);
	paint.setBrush(QBrush(Qt::white));
	QRect this_rect = this->rect(); 
	paint.drawRect(this_rect);
	paint.restore();
}

void OutputWidget::DrawLayout(QPainter& painter)
{
	painter.save();
	QColor shape_color = QColor(45, 131, 209, 160);

	QPen thisPen;
	thisPen.setBrush(Qt::black);
	thisPen.setStyle(Qt::SolidLine);
	painter.setPen(thisPen);
	painter.setBrush(shape_color);

	if (output_widget_tree)
	{
		std::queue<CompoundNode*> temp_queue;
		temp_queue.push(output_widget_tree);
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
						DrawLeafNode(painter, node);
					}
					else
					{						
						std::cout << "have this case?,compound node with different label" << std::endl;
					}
				}
				else if(node->tree0_node != nullptr && node->tree1_node == nullptr)
				{					
					node->node_present_type = node->tree0_node->node_present_type;
					DrawLeafNode(painter, node);
				}
				else
				{					
					node->node_present_type = node->tree1_node->node_present_type;
					DrawLeafNode(painter, node);
				}
				
			}

			for (auto iter = node->children.begin(); iter != node->children.end(); iter++)
				temp_queue.push(*iter);
		}
	}
	painter.restore();
}

void OutputWidget::DrawLeafNode(QPainter& paint, CompoundNode* node)
{
	//for better visualization, giving a displacement
	double move = 50;

	QColor label_color[4] = {
		QColor(150, 150, 150, 30),  // padding
		QColor(45, 131, 109, 220),  // picture
		QColor(75, 151, 209, 100),  // text
		QColor(120, 120, 120, 100)  // title
	};

	QRect rect;
	QColor text_color;
	QPen thisPen;
	thisPen.setWidth(0);
	thisPen.setColor(QColor(255, 255, 255, 0));
	paint.setPen(thisPen);

	QSvgRenderer image_icon;
	image_icon.load(QString("./Resources/ContentImage/image.svg"));

	int font_size = 0;
	if (node->height >= 20.0 && node->height < 60.0)
	{
		font_size = int(node->height * 0.8);
	}
	else if (node->height >= 60.0)
	{
		font_size = 45;
	}
	else
	{
		font_size = int(node->height);
	}

	QFont font = paint.font();
	font.setPixelSize(font_size);
	font.setBold(true);
	font.setFamily("Arial");
	paint.setFont(font);
	rect = QRect(node->x + move, node->y + move, node->width, node->height);

	if (node->node_present_type == NODE_PRESENT_TYPE::PADDING)
	{
		paint.setBrush(label_color[0]);
		paint.drawRoundedRect(rect, 5, 5);
	}
	else if (node->node_present_type == NODE_PRESENT_TYPE::PICTURE)
	{
		paint.setBrush(label_color[1]);
		paint.drawRoundedRect(rect, 5, 5);
		if (node->width < 90 || node->height < 90)
		{
			image_icon.render(&paint, QRectF(rect.center().x() - 20, rect.center().y() - 20, 40, 40));
		}
		else
		{
			image_icon.render(&paint, QRectF(rect.center().x() - 40, rect.center().y() - 40, 80, 80));
		}
	}
	else if (node->node_present_type == NODE_PRESENT_TYPE::TEXT)
	{
		QRect text_rect;
		int count = 0;
		double text_width = node->width;
		double text_x = rect.x() + 5;

		paint.setBrush(label_color[2]);
		paint.drawRoundedRect(rect, 5, 5);

		for (double i = 0; i <= node->height - 18; i += 14) {
			count++;
			if (count % 8 == 0) {
				text_x = rect.x() + 5;
				text_width = node->width / 2;
			}
			else if (count % 8 == 1) {
				text_x = rect.x() + 15;
				text_width = node->width - 10;
			}
			else {
				text_x = rect.x() + 5;
				text_width = node->width;
			}
			text_rect = QRect(text_x, rect.y() + i + 6, text_width - 10, 6);
			paint.setBrush(QColor(60, 60, 60, 220));
			paint.drawRoundedRect(text_rect, 2, 2);
			if (count % 8 == 0) {
				i += 15;
			}
		}
	}
	else if (node->node_present_type == NODE_PRESENT_TYPE::TITLE)
	{
		paint.setBrush(label_color[3]);
		paint.drawRoundedRect(rect, 5, 5);
		text_color = QColor(30, 30, 30);
		paint.setPen(text_color);
		paint.drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, " Title");		
	}
	else
	{
		std::cout << "When drawing, do not have this label" << std::endl;
	}
	return;
}

void OutputWidget::DrawGrid(QPainter& paint)
{
	paint.save();
	QPen pen;
	pen.setWidth(1);

	QColor c(0, 0, 0, 70);

	pen.setColor(c);
	
	int grid_width = 100;

	paint.setPen(pen);

	paint.setRenderHint(QPainter::Antialiasing, false);
	QRect this_rect = this->rect();

	int total_width = this_rect.width();
	int total_height = this_rect.height();

	int start_point_ = 0;

	while (start_point_ <= total_height)
	{
		QPoint p[2];
		p[0].setX(0);
		p[1].setX(total_width);
		p[0].setY(start_point_);
		p[1].setY(start_point_);

		paint.drawLine(p[0], p[1]);
		start_point_ += grid_width;
	}

	start_point_ = 0;

	while (start_point_ <= total_width)
	{
		QPoint p[2];
		p[0].setX(start_point_);
		p[1].setX(start_point_);

		p[0].setY(0);
		p[1].setY(total_height);

		paint.drawLine(p[0], p[1]);
		start_point_ += grid_width;
	}

	paint.restore();
}