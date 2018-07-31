#ifndef ImageGraphicsView_H
#define ImageGraphicsView_H

#include <QGraphicsView>
#include <QObject>
#include <QWheelEvent>

class ImageGraphicsView : public QGraphicsView
{
    Q_OBJECT
public: 
        ImageGraphicsView(QWidget *parent = 0);
        void zoom(qreal factor);
protected:
        virtual void wheelEvent(QWheelEvent *event);
private:
        bool isTouched;
};

#endif // ImageGraphicsView_H 
