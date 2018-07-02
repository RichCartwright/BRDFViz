/********************************************************************************
** Form generated from reading UI file 'VTKWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VTKWINDOW_H
#define UI_VTKWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>
#include "QVTKOpenGLWidget.h"

QT_BEGIN_NAMESPACE

class Ui_VTKWindow
{
public:
    QAction *actionOpenFile;
    QAction *actionExit;
    QAction *actionPrint;
    QAction *actionHelp;
    QAction *actionSave;
    QAction *actionAbout;
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QTabWidget *viewWidget;
    QWidget *pointCloudViewTab;
    QGridLayout *gridLayout_2;
    QVTKOpenGLWidget *qvtkWidget;
    QWidget *tab_2;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuView;
    QMenu *menuAbout;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *VTKWindow)
    {
        if (VTKWindow->objectName().isEmpty())
            VTKWindow->setObjectName(QStringLiteral("VTKWindow"));
        VTKWindow->resize(710, 485);
        actionOpenFile = new QAction(VTKWindow);
        actionOpenFile->setObjectName(QStringLiteral("actionOpenFile"));
        actionOpenFile->setEnabled(true);
        actionExit = new QAction(VTKWindow);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        actionPrint = new QAction(VTKWindow);
        actionPrint->setObjectName(QStringLiteral("actionPrint"));
        actionHelp = new QAction(VTKWindow);
        actionHelp->setObjectName(QStringLiteral("actionHelp"));
        actionSave = new QAction(VTKWindow);
        actionSave->setObjectName(QStringLiteral("actionSave"));
        actionAbout = new QAction(VTKWindow);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        centralwidget = new QWidget(VTKWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        viewWidget = new QTabWidget(centralwidget);
        viewWidget->setObjectName(QStringLiteral("viewWidget"));
        viewWidget->setTabPosition(QTabWidget::East);
        pointCloudViewTab = new QWidget();
        pointCloudViewTab->setObjectName(QStringLiteral("pointCloudViewTab"));
        gridLayout_2 = new QGridLayout(pointCloudViewTab);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        qvtkWidget = new QVTKOpenGLWidget(pointCloudViewTab);
        qvtkWidget->setObjectName(QStringLiteral("qvtkWidget"));

        gridLayout_2->addWidget(qvtkWidget, 0, 0, 1, 1);

        viewWidget->addTab(pointCloudViewTab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        viewWidget->addTab(tab_2, QString());

        gridLayout->addWidget(viewWidget, 0, 0, 1, 1);

        VTKWindow->setCentralWidget(centralwidget);
        menuBar = new QMenuBar(VTKWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 710, 25));
        menuBar->setDefaultUp(false);
        menuBar->setNativeMenuBar(false);
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuView = new QMenu(menuBar);
        menuView->setObjectName(QStringLiteral("menuView"));
        menuAbout = new QMenu(menuBar);
        menuAbout->setObjectName(QStringLiteral("menuAbout"));
        VTKWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(VTKWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        VTKWindow->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuView->menuAction());
        menuBar->addAction(menuAbout->menuAction());
        menuFile->addAction(actionOpenFile);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuAbout->addAction(actionAbout);

        retranslateUi(VTKWindow);

        viewWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(VTKWindow);
    } // setupUi

    void retranslateUi(QMainWindow *VTKWindow)
    {
        VTKWindow->setWindowTitle(QApplication::translate("VTKWindow", "BRDFViz", 0));
        actionOpenFile->setText(QApplication::translate("VTKWindow", "Open Scene...", 0));
        actionExit->setText(QApplication::translate("VTKWindow", "Exit", 0));
        actionPrint->setText(QApplication::translate("VTKWindow", "Print", 0));
        actionHelp->setText(QApplication::translate("VTKWindow", "Help", 0));
        actionSave->setText(QApplication::translate("VTKWindow", "Save", 0));
        actionAbout->setText(QApplication::translate("VTKWindow", "About", 0));
        viewWidget->setTabText(viewWidget->indexOf(pointCloudViewTab), QApplication::translate("VTKWindow", "Point Cloud View", 0));
        viewWidget->setTabText(viewWidget->indexOf(tab_2), QApplication::translate("VTKWindow", "TODO", 0));
        menuFile->setTitle(QApplication::translate("VTKWindow", "File", 0));
        menuView->setTitle(QApplication::translate("VTKWindow", "View", 0));
        menuAbout->setTitle(QApplication::translate("VTKWindow", "About", 0));
    } // retranslateUi

};

namespace Ui {
    class VTKWindow: public Ui_VTKWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VTKWINDOW_H
