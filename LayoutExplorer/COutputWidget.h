#pragma once
#include <qwidget.h>
#include <QSvgRenderer>
#include <Qpainter>
#include <iostream>
#include "CLayoutTree.h"
#include "CCompoundNode.h"

class OutputWidget : public QWidget
{
	//for using 'signal' and 'slot'
	Q_OBJECT

public:
	OutputWidget(QWidget *parent);
	~OutputWidget();

	void paintEvent(QPaintEvent* event);

public:
	CompoundNode* output_widget_tree;
	bool draw_layout_state;	

private:
	void DrawGrid(QPainter& paint);
	void DrawFrame(QPainter& paint);
	void DrawLayout(QPainter& painter);
	void DrawLeafNode(QPainter& paint, CompoundNode* node);

};

