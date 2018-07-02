/********************************************************************************
** Form generated from reading UI file 'AboutMe.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABOUTME_H
#define UI_ABOUTME_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>

QT_BEGIN_NAMESPACE

class Ui_AboutMe
{
public:
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *AboutMe)
    {
        if (AboutMe->objectName().isEmpty())
            AboutMe->setObjectName(QStringLiteral("AboutMe"));
        AboutMe->resize(400, 300);
        buttonBox = new QDialogButtonBox(AboutMe);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(30, 240, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        retranslateUi(AboutMe);
        QObject::connect(buttonBox, SIGNAL(accepted()), AboutMe, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), AboutMe, SLOT(reject()));

        QMetaObject::connectSlotsByName(AboutMe);
    } // setupUi

    void retranslateUi(QDialog *AboutMe)
    {
        AboutMe->setWindowTitle(QApplication::translate("AboutMe", "About", 0));
    } // retranslateUi

};

namespace Ui {
    class AboutMe: public Ui_AboutMe {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUTME_H
