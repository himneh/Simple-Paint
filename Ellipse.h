#pragma once
#include <string>
#include "IShapes.h"
#include "Point.h"
#include "Tokenizer.h"

class Ellipsee : public IShape {
private:
    Point _topLeft;
    Point _rightBottom;
    int _size;
    DWORD _color;
public:
    Ellipsee() {
        _topLeft = Point(0, 0);
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

    Ellipsee(Point a, Point b, DWORD cl, int sz);

    bool isSelected(int x, int y);

    ostream& print(ostream&);

    static Ellipsee parse(wstring buffer);

    wstring toString();
};