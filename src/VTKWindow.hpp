#ifndef VTKWindow_H
#define VTKWindow_H

#include "ui_VTKWindow.h"

#include <QMainWindow>

class VTKWindow : public QMainWindow, private Ui::VTKWindow 
{
    Q_OBJECT

public:
    VTKWindow();

public slots:
    virtual void slotExit();
};

#endif

