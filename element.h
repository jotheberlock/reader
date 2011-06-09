#ifndef _ELEMENT_
#define _ELEMENT_

#include <QtCore/QStringList>
#include <QtGui/QPaintDevice>
#include <QtGui/QFont>
#include <QtGui/QPixmap>

class Element
{
  public:

    virtual bool render(QPaintDevice *, int x,int y, int w, int h,
                        int & dropout) = 0;
    virtual QRect size(int w) = 0;
    
};

class ParagraphElement : public Element
{
  public:

    ParagraphElement(QString t)
    {
        text=t.split(' ');
        font = QFont("Times New Roman", 20);
    }
    
    void setText(QString t) 
    {
        text=t.split(' ');
    }

    virtual QRect size(int w);
    virtual bool render(QPaintDevice *, int x,int y, int w, int h,
                        int & dropout);

  protected:

    QStringList text;
    QFont font;
    
};

class PictureElement : public Element
{
  public:

    PictureElement(QImage & i)
    {
        pixmap = QPixmap::fromImage(i);
    }

    virtual QRect size(int w);
    virtual bool render(QPaintDevice *, int x,int y, int w, int h,
                        int & dropout);

  protected:

    QPixmap pixmap;

};


#endif
