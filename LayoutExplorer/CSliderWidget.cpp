#include "CSliderWidget.h"

SliderWidget::SliderWidget(QWidget* parent)
	: QWidget(parent)
{
	
}

SliderWidget::~SliderWidget()
{

}

void SliderWidget::paintEvent(QPaintEvent* event)
{
	QPainter paint(this);
	DrawFrame(paint);
}

void SliderWidget::DrawFrame(QPainter& paint)
{
	paint.save();
	paint.setPen(Qt::white);
	paint.setBrush(QBrush(Qt::white));
	QRect this_rect = this->rect(); 
	paint.drawRect(this_rect);
	paint.restore();
}