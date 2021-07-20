#pragma once
#include <string>
#include "IShapes.h"
#include "Point.h"
#include "Tokenizer.h"

class Line : public IShape {
private:
    Point _start;
    Point _end;
    int _size;
    DWORD _color;
public:
    Line() {
        _start = Point(0, 0);
        _end = Point(0, 0);
        _size = 0;
        _color = 0;
    }

    Point getTop();
    Point getBottom();
    DWORD getColor();
    int getSize();

    void setStart(Point);
    void setEnd(Point);
    void setColor(DWORD);
    void setSize(int);

    wstring Type();

    bool isSelected(int x, int y);

    Line(Point x, Point y, DWORD cl, int sz);

    double length();

    ostream& print(ostream&);
    static Line parse(wstring buffer);

    wstring toString();
};