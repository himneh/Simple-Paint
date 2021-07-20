#pragma once
#include <string>
#include <sstream>
#include "Tokenizer.h"

using namespace std;

class Point {
private:
    int _x;
    int _y;
public:
    int getX();
    int getY();
    void setX(int);
    void setY(int);

    Point();
    Point(int x, int y);
  

    static double calDistance(const Point& a, const Point& b);

    friend ostream& operator<<(ostream& out, const Point& point);

    static Point parse(wstring buffer);

    wstring toString();
};