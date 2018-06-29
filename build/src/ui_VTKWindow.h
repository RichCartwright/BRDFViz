/********************************************************************************
** Form generated from reading UI file 'VTKWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VTKWINDOW_H
#define UI_VTKWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
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
    QWidget *centralwidget;
    QVTKOpenGLWidget *qvtkWidget;

    void setupUi(QMainWindow *SimpleView)
    {
        if (SimpleView->objectName().isEmpty())
            SimpleView->setObjectName(QStringLiteral("SimpleView"));
        SimpleView->resize(541, 583);
        actionOpenFile = new QAction(SimpleView);
        actionOpenFile->setObjectName(QStringLiteral("actionOpenFile"));
        actionOpenFile->setEnabled(true);
        actionExit = new QAction(SimpleView);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        actionPrint = new QAction(SimpleView);
        actionPrint->setObjectName(QStringLiteral("actionPrint"));
        actionHelp = new QAction(SimpleView);
        actionHelp->setObjectName(QStringLiteral("actionHelp"));
        actionSave = new QAction(SimpleView);
        actionSave->setObjectName(QStringLiteral("actionSave"));
        centralwidget = new QWidget(SimpleView);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        qvtkWidget = new QVTKOpenGLWidget(centralwidget);
        qvtkWidget->setObjectName(QStringLiteral("qvtkWidget"));
        qvtkWidget->setGeometry(QRect(10, 30, 511, 531));
        SimpleView->setCentralWidget(centralwidget);

        retranslateUi(SimpleView);

        QMetaObject::connectSlotsByName(SimpleView);
    } // setupUi

    void retranslateUi(QMainWindow *SimpleView)
    {
        SimpleView->setWindowTitle(QApplication::translate("VTKWindow", "BRDFViz", Q_NULLPTR));
        actionOpenFile->setText(QApplication::translate("VTKWindow", "Open File...", Q_NULLPTR));
        actionExit->setText(QApplication::translate("VTKWindow", "Exit", Q_NULLPTR));
        actionPrint->setText(QApplication::translate("VTKWindow", "Print", Q_NULLPTR));
        actionHelp->setText(QApplication::translate("VTKWindow", "Help", Q_NULLPTR));
        actionSave->setText(QApplication::translate("VTKWindow", "Save", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class VTKWindow: public Ui_VTKWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VTKWINDOW_H
