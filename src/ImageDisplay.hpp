#include <QGraphicsItem>
#include <QTimer>

// Small class, might as well rock it in the header
class ImageDisplay : public QGraphicsItem 
{
public:
    ImageDisplay(int x, int y, int xf, int yf) : QGraphicsItem(NULL)
    {
        xres = x, yres = y;
        xframe = xf, yframe = yf;

        image = QImage(xres, yres, QImage::Format_ARGB32);
        image.fill(QColor(0.0, 0.0, 0.0)); 
    } 
   
    void UpdateImage(int x, int y, double *colour, int xf, int yf)
    {
        xframe = xf, yframe = yf;
        image.setPixel(x, y, qRgba( colour[0], 
                                    colour[1], 
                                    colour[2], 
                                    255));
        update(); 
    }

private:
    QImage image;
    int xres = 0;
    int yres = 0;
    // Get the XY of the frame its being rendered in 
    int xframe = 0;
    int yframe = 0;
    // We need to know how many multisamples
    virtual QRectF boundingRect() const
    {
        return QRectF(0, 0, xres, yres);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
    {
        painter->drawImage(0,0,image); 
    }
};
