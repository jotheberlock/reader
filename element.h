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

class StringFragment
{
  public:

    StringFragment()
    {
        is_italic = false;
        is_bold = false;
    }

    StringFragment(QString t, bool i = false, bool b = false)
    {
        text=t.split(' '); is_italic=i; is_bold=b;
    }
    
    QStringList text;
    bool is_italic;
    bool is_bold;
    
};

class ParagraphElement : public Element
{
  public:

    ParagraphElement()
    {
        font = QFont("Times New Roman", 12);
    }
    
    virtual QRect size(int w);
    virtual bool render(QPaintDevice *, int x,int y, int w, int h,
                        int & dropout);

    void addFragment(StringFragment s)
    {
        fragments.push_back(s);
    }

    int numFragments()
    {
        return fragments.size();
    }
    
  protected:

    QList<StringFragment> fragments;
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
