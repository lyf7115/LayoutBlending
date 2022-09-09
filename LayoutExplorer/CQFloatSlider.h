#pragma once
#include <QSlider>

class QFloatSlider : public QSlider
{
	Q_OBJECT

public:
	QFloatSlider(QWidget* pParent = NULL);

	void setRange(double Min, double Max);
	void setMinimum(double Min);
	double minimum() const;
	void setMaximum(double Max);
	double maximum() const;
	double value() const;

public slots:
	void setValue(int value);
	void setValue(double Value, bool BlockSignals = false);

private slots:

signals:
	void valueChanged(double Value);
	//void rangeChanged(double Min, double Max);

private:
	double	m_Multiplier;
};
