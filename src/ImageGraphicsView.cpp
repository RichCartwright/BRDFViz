#include "ImageGraphicsView.hpp"
#include <cmath>

ImageGraphicsView::ImageGraphicsView(QWidget *parent) : QGraphicsView(parent)
{

}
void ImageGraphicsView::wheelEvent (QWheelEvent *event)
{
    int numSteps = event->delta() / 15 / 8;

    if (numSteps == 0)
    {
        event->ignore();
        return;
    }
    qreal sc = pow(1.1, numSteps);
    this->zoom(sc); 
    event->accept();
}

void ImageGraphicsView::zoom(qreal factor)
{
    scale(factor, factor);
}
