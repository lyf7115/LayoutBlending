#pragma once

#include <iostream>
#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include <Qdebug>
#include "CSliderWidget.h"
#include "COutputWidget.h"
#include <qfiledialog.h>
#include <Eigen/Core>
#include <Eigen/Dense>
#include "CNodeMatching.h"
#include "CLayoutTree.h"
#include "CCompoundNode.h"
#include "CCompoundTree.h"
#include "qsvggenerator.h"
#include "CTransfer.h"
#include <QtCore/QObject>
#include <QtGui/QtGui>

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = Q_NULLPTR);
	~MainWindow();

public slots:
	void OutputWidgetRedraw();

	//open each layout file
	void OpenNextLayout();

	void Compute();
	void MovedAndCreate();
	void SaveGenerLayout();
	void SaveLayoutAsSvg();
	void BatchCreateLay();

private:
	void CopyTree();
	void DeterInterVar(double slider_value);
	void SaveGenerLayoutForBatch(int order);
	void SetCompoundNodeLabel();
	void BatchSaveLayoutAsSvg(int order);
	int NodePresentTypeToInteger(CompoundNode* input_node);

private:
	Ui::MainWindowClass ui;

	Transfer layout_handler;
	CLayoutTree* m_firstLayout;
	CLayoutTree* m_secondLayout;

	CCompoundTree* combine_tree;
	CCompoundTree* combine_tree_copy;

	CompoundNode* comb_tree_root;
	CompoundNode* comb_tree_root_copy;

	std::vector<CLayoutTree*> m_uploadLayout;		
};
