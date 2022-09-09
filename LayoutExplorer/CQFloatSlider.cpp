#include "CQFloatSlider.h"


QFloatSlider::QFloatSlider(QWidget* pParent /*= NULL*/) :
	QSlider(pParent),
	m_Multiplier(10000.0)
{
	connect(this, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));

	setSingleStep(1);

	setOrientation(Qt::Horizontal);
	setFocusPolicy(Qt::NoFocus);
}

void QFloatSlider::setValue(int Value)
{
	emit valueChanged((double)Value / m_Multiplier);
}

void QFloatSlider::setValue(double Value, bool BlockSignals)
{
	QSlider::blockSignals(BlockSignals);

	QSlider::setValue(Value * m_Multiplier);

	if (!BlockSignals)
		emit valueChanged(Value);

	QSlider::blockSignals(false);
}

void QFloatSlider::setRange(double Min, double Max)
{
	QSlider::setRange(Min * m_Multiplier, Max * m_Multiplier);

	emit rangeChanged(Min, Max);
}

void QFloatSlider::setMinimum(double Min)
{
	QSlider::setMinimum(Min * m_Multiplier);

	emit rangeChanged(minimum(), maximum());
}

double QFloatSlider::minimum() const
{
	return QSlider::minimum() / m_Multiplier;
}

void QFloatSlider::setMaximum(double Max)
{
	QSlider::setMaximum(Max * m_Multiplier);

	emit rangeChanged(minimum(), maximum());
}

double QFloatSlider::maximum() const
{
	return QSlider::maximum() / m_Multiplier;
}

double QFloatSlider::value() const
{
	int Value = QSlider::value();
	return (double)Value / m_Multiplier;
}