#ifndef VTKWindow_H
#define VTKWindow_H

#include "ui_VTKWindow.h"
#include "ui_AboutMe.h"
#include <QMainWindow>
#include <QDialog>

class VTKWindow : public QMainWindow, private Ui::VTKWindow 
{
    Q_OBJECT

public:
    VTKWindow();

protected slots:
    virtual void slotExit();
    virtual void slotAbout();
    virtual void slotOpen();
};



// About me class
class AboutMeDialog : public QDialog, private Ui::AboutMe
{
    Q_OBJECT

public:
    AboutMeDialog(QMainWindow *parent = Q_NULLPTR);
};
#endif

