#pragma once
#include <qwidget.h>
#include <Qpainter>
#include <iostream>

class SliderWidget : public QWidget
{
	Q_OBJECT

public:
	SliderWidget(QWidget* parent);
	~SliderWidget();

	void paintEvent(QPaintEvent* event);

	void DrawFrame(QPainter& paint);
};

