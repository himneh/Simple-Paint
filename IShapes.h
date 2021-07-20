#pragma once
#include <vector>
#include <string>
#include "Point.h"

#include "framework.h"

using namespace std;

class IShape {
public:
    virtual wstring toString() = 0;
    virtual wstring Type() = 0;
    virtual bool isSelected(int, int) = 0;

    virtual ostream& print(ostream& out) { return out; }
    friend ostream& operator<<(ostream& out, IShape& shape);

    virtual Point getTop() = 0;
    virtual Point getBottom() = 0;

    virtual DWORD getColor() = 0;
    virtual int getSize() = 0;
};
