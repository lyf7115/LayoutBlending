/********************************************************************************
** Form generated from reading UI file 'LayoutExplorer.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LAYOUTEXPLORER_H
#define UI_LAYOUTEXPLORER_H

#include <COutputWidget.h>
#include <CQfloatSlider.h>
#include <CSliderWidget.h>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LayoutExplorerClass
{
public:
    QAction *actionOpen_Next_Layout;
    QAction *actionSave_Layout;
    QAction *actionSave_Layout_As_Svg;
    QAction *actionCompute;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    OutputWidget *output_widget;
    SliderWidget *slider_widget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacerLeft;
    QFloatSlider *horizontalSlider;
    QSpacerItem *horizontalSpacerRight;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuEdit;
    QToolBar *mainToolBar;

    void setupUi(QMainWindow *LayoutExplorerClass)
    {
        if (LayoutExplorerClass->objectName().isEmpty())
            LayoutExplorerClass->setObjectName(QStringLiteral("LayoutExplorerClass"));
        LayoutExplorerClass->resize(1095, 872);
        actionOpen_Next_Layout = new QAction(LayoutExplorerClass);
        actionOpen_Next_Layout->setObjectName(QStringLiteral("actionOpen_Next_Layout"));
        QIcon icon;
        icon.addFile(QStringLiteral("Resources/ProjectIcon/OpenNextLayout.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpen_Next_Layout->setIcon(icon);
        actionSave_Layout = new QAction(LayoutExplorerClass);
        actionSave_Layout->setObjectName(QStringLiteral("actionSave_Layout"));
        actionSave_Layout_As_Svg = new QAction(LayoutExplorerClass);
        actionSave_Layout_As_Svg->setObjectName(QStringLiteral("actionSave_Layout_As_Svg"));
        actionCompute = new QAction(LayoutExplorerClass);
        actionCompute->setObjectName(QStringLiteral("actionCompute"));
        QIcon icon1;
        icon1.addFile(QStringLiteral("Resources/ProjectIcon/ComputeIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionCompute->setIcon(icon1);
        centralWidget = new QWidget(LayoutExplorerClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        output_widget = new OutputWidget(centralWidget);
        output_widget->setObjectName(QStringLiteral("output_widget"));

        verticalLayout->addWidget(output_widget);

        slider_widget = new SliderWidget(centralWidget);
        slider_widget->setObjectName(QStringLiteral("slider_widget"));
        horizontalLayout = new QHBoxLayout(slider_widget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacerLeft = new QSpacerItem(206, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacerLeft);

        horizontalSlider = new QFloatSlider(slider_widget);
        horizontalSlider->setObjectName(QStringLiteral("horizontalSlider"));
        horizontalSlider->setOrientation(Qt::Horizontal);

        horizontalLayout->addWidget(horizontalSlider);

        horizontalSpacerRight = new QSpacerItem(206, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacerRight);

        horizontalLayout->setStretch(0, 1);
        horizontalLayout->setStretch(1, 3);
        horizontalLayout->setStretch(2, 1);

        verticalLayout->addWidget(slider_widget);

        verticalLayout->setStretch(0, 90);
        verticalLayout->setStretch(1, 10);
        LayoutExplorerClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(LayoutExplorerClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1095, 23));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuEdit = new QMenu(menuBar);
        menuEdit->setObjectName(QStringLiteral("menuEdit"));
        LayoutExplorerClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(LayoutExplorerClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        LayoutExplorerClass->addToolBar(Qt::TopToolBarArea, mainToolBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuEdit->menuAction());
        menuFile->addAction(actionOpen_Next_Layout);
        menuFile->addAction(actionSave_Layout);
        menuFile->addAction(actionSave_Layout_As_Svg);
        menuEdit->addAction(actionCompute);
        mainToolBar->addAction(actionOpen_Next_Layout);
        mainToolBar->addAction(actionCompute);

        retranslateUi(LayoutExplorerClass);
        QObject::connect(actionOpen_Next_Layout, SIGNAL(triggered()), LayoutExplorerClass, SLOT(OpenNextLayout()));
        QObject::connect(actionSave_Layout, SIGNAL(triggered()), LayoutExplorerClass, SLOT(SaveGenerLayout()));
        QObject::connect(actionSave_Layout_As_Svg, SIGNAL(triggered()), LayoutExplorerClass, SLOT(SaveLayoutAsSvg()));
        QObject::connect(actionCompute, SIGNAL(triggered()), LayoutExplorerClass, SLOT(Compute()));

        QMetaObject::connectSlotsByName(LayoutExplorerClass);
    } // setupUi

    void retranslateUi(QMainWindow *LayoutExplorerClass)
    {
        LayoutExplorerClass->setWindowTitle(QApplication::translate("LayoutExplorerClass", "LayoutExplorer", Q_NULLPTR));
        actionOpen_Next_Layout->setText(QApplication::translate("LayoutExplorerClass", "Open Next Layout", Q_NULLPTR));
        actionSave_Layout->setText(QApplication::translate("LayoutExplorerClass", "Save Layout", Q_NULLPTR));
        actionSave_Layout_As_Svg->setText(QApplication::translate("LayoutExplorerClass", "Save Layout As Svg", Q_NULLPTR));
        actionCompute->setText(QApplication::translate("LayoutExplorerClass", "Compute", Q_NULLPTR));
        menuFile->setTitle(QApplication::translate("LayoutExplorerClass", "File", Q_NULLPTR));
        menuEdit->setTitle(QApplication::translate("LayoutExplorerClass", "Edit", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class LayoutExplorerClass: public Ui_LayoutExplorerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LAYOUTEXPLORER_H
