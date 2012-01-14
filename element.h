#ifndef _ELEMENT_
#define _ELEMENT_

#include <QtCore/QStringList>
#include <QtGui/QPaintDevice>
#include <QtGui/QFont>
#include <QtGui/QPixmap>

class Page;

class Element
{
  public:

    Element()
    {
        current_position = 0;
        element_number = 0;
    }

    virtual ~Element()
    {
    }

        // Called before anything else in layout
    void configure(qint64 y, Page * p)
    {
        current_position = y;
        page = p;
    }

        // Should probably come up with some more general interface here
    virtual QString hitTest(qint64 /*x*/, qint64 /*y*/)
    { return ""; }
    
    virtual qint64 height() = 0;
        // offset maps screen to virtual coordinate
    virtual void render(qint64 offset) = 0;
    
    qint64 position()  { return current_position; }
    void setNumber(qint64 n) { element_number = n; }
    qint64 number() { return element_number; }
    
  protected:

    qint64 current_position;
    qint64 element_number;
    Page * page;

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

class Word
{
  public:

    Word()
    {
        is_italic=false;
        is_bold=false;
        x=0;
        y=0;
        lx=0;
        ly=0;
        w=0;
        h=0;
    }

    bool is_italic;
    bool is_bold;
    QString text;
        // For drawing purposes
    qint64 x,y;
        // Bounding box
    qint64 lx,ly, w, h;
    int index;    // The xth word of the paragraph as in stringfragment
    
};


class ParagraphElement : public Element
{
  public:

    ParagraphElement()
    {
        cached_height = -1;
    }
    
    void addFragment(StringFragment s)
    {
        fragments.push_back(s);
    }

    int numFragments()
    {
        return fragments.size();
    }

    QList<StringFragment> & getFragments()
    {
        return fragments;
    }
    
    virtual qint64 height();
    virtual void render(qint64 offset);

    QList<Word> & getWords()
    {
        return words;
    }

    Word getWord(qint64 x, qint64 y);
    
  protected:
    
    QList<StringFragment> fragments;
    QList<Word> words;
    qint64 cached_height;
    
};

class PictureElement : public Element
{
  public:

    PictureElement(QImage & i)
    {
        pixmap = QPixmap::fromImage(i);
    }
        
    virtual qint64 height();
    virtual void render(qint64 offset);
    
  protected:

    QPixmap pixmap;

};

class PagebreakElement : public Element
{
  public:
    
    virtual qint64 height();
    virtual void render(qint64) {}
    
};

#endif
