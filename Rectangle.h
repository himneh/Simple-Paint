#pragma once
#include <string>
#include "IShapes.h"
#include "Point.h"
#include "Tokenizer.h"

class Rectanglee : public IShape {
private:
    Point _topLeft;
    Point _rightBottom;
    int _size;
    DWORD _color;
public:
    Rectanglee() {
        _topLeft = Point(0,0);
        _rightBottom = Point(0, 0);
        _size = 0;
        _color = 0;
    }

    wstring Type();

    Point getTop();
    Point getBottom();
    DWORD getColor();
    int getSize();

    void setTop(Point x);
    void setBottom(Point x);
    void setColor(DWORD);
    void setSize(int);

    Rectanglee(Point a, Point b, DWORD cl, int sz);

    bool isSelected(int x, int y);

    ostream& print(ostream&);

    static Rectanglee parse(wstring buffer);

    wstring toString();
};