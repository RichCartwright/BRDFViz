#include <QGraphicsItem>
#include <QTimer>

class ImageDisplay : public QGraphicsItem 
{
public:
    ImageDisplay(int x, int y) : QGraphicsItem(NULL)
    {
        xres = x, yres = y;

        image = QImage(xres, yres, QImage::Format_ARGB32);

        image.fill(QColor(0.0, 0.0, 0.0)); 
        /*
        static int red = 0;
        static int green = 0;
        static int blue = 0;

        image.fill(QColor(red++%255, green++%255, blue++%255));

        QTimer* pTimer = new QTimer;
        QObject::connect(pTimer, &QTimer::timeout, [=]()
        {
            image.fill(QColor(red++%255, green++%255, blue++%255));
            update();
        }); 

        pTimer->start(1000 / 30);
        */
    } 
   
    void UpdateImage(int x, int y, double *colour)
    {
        image.setPixel(x, y, qRgba(colour[0], colour[1], colour[2], 255)); 
        update(); 
    }
private:
    QImage image;
    int xres = 0;
    int yres = 0;
    virtual QRectF boundingRect() const
    {
        return QRectF(0, 0, xres, yres);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
    {
        painter->drawImage(0,0,image); 
    }
};
