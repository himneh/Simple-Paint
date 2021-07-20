#include "Ellipse.h"
#include "IShapes.h"
#include "Point.h"
#include "Rectangle.h"
#include "Line.h"
#include "Tokenizer.h"

ostream& operator<<(ostream& out, IShape& shape)
{
    return shape.print(out);
}

vector<wstring> Tokenizer::split(wstring haystack, wstring  needle) {
    vector<wstring> result;

    int startPos = 0;
    size_t foundPos = 0;
    while (true) {
        foundPos = haystack.find(needle, startPos);

        if (foundPos != string::npos) {
            wstring token = haystack.substr(startPos, foundPos - startPos);
            result.push_back(token);
            startPos = foundPos + needle.length();
        }
        else {
            wstring token = haystack.substr(startPos, haystack.length() - startPos);
            result.push_back(token);
            break;
        }
    }
    return result;
}

vector<wstring> WTokenizer::split(wstring haystack, wstring  needle) {
    vector<wstring> result;

    int startPos = 0;
    size_t foundPos = 0;
    while (true) {
        foundPos = haystack.find(needle, startPos);

        if (foundPos != string::npos) {
            wstring token = haystack.substr(startPos, foundPos - startPos);
            result.push_back(token);
            startPos = foundPos + needle.length();
        }
        else {
            wstring token = haystack.substr(startPos, haystack.length() - startPos);
            result.push_back(token);
            break;
        }
    }
    return result;
}



//---------------
int Point::getX() {
    return _x;
}

int Point::getY() {
    return _y;
}

void Point::setX(int value) {
    _x = value;
}

void Point::setY(int value) {
    _y = value;
}

Point::Point() {
    setX(0);
    setY(0);
}

Point::Point(int x, int y) {
    setX(x);
    setY(y);
}

double Point::calDistance(const Point& a, const Point& b) {
    int dx = a._x - b._x;
    int dy = a._y - b._y;
    double result = sqrt(dx * dx + dy * dy);
    return result;
}

ostream& operator<<(ostream& out, const Point& point) {
    out << point._x << "," << point._y;
    return out;
}

Point Point::parse(wstring buffer) {
    vector<wstring> tokens = Tokenizer::split(buffer, L",");
    int x = stoi(tokens[0]);
    int y = stoi(tokens[1]);
    Point answer(Point(x, y));
    return answer;
}

wstring Point::toString() {
    wstringstream builder;
    builder << getX() << L"," << getY();
    return builder.str();
}

//---------------
wstring Line::Type() {
    return L"Line";
}

Point Line::getTop() {
    return _start;
}

Point Line::getBottom() {
    return _end;
}

DWORD Line::getColor() {
    return _color;
}

int Line::getSize() {
    return _size;
}

void Line::setColor(DWORD value) {
    _color = value;
}

void Line::setSize(int value) {
    _size = value;
}

void Line::setStart(Point value) {
    _start = value;
}

void Line::setEnd(Point value) {
    _end = value;
}

Line::Line(Point x, Point y, DWORD cl, int sz) {
    setStart(x);
    setEnd(y);
    setColor(cl);
    setSize(sz);
}

double Line::length() {
    return Point::calDistance(_start, _end);
}

bool Line::isSelected(int x, int y) {
    if (((_start.getX() < _end.getX()) && (_start.getY() < _end.getY())) || ((_start.getX() > _end.getX()) && (_start.getY() > _end.getY()))) {
        int max_x = max(_start.getX(), _end.getX());
        int min_x = min(_start.getX(), _end.getX());

        int max_y = max(_start.getY(), _end.getY());
        int min_y = min(_start.getY(), _end.getY());

        if (min_x <= x && x <= max_x)
            if (min_y <= y && y <= max_y) return true;
        return false;
    }
    if (((_start.getX() > _end.getX()) && (_start.getY() < _end.getY())) || ((_start.getX() < _end.getX()) && (_start.getY() > _end.getY()))) {
        int max_x = max(_start.getX(), _end.getX());
        int min_x = min(_start.getX(), _end.getX());

        int max_y = max(_start.getY(), _end.getY());
        int min_y = min(_start.getY(), _end.getY());

        if (min_x <= x && x <= max_x)
            if (min_y <= y && y <= max_y) return true;
        return false;
    }
    return false;
}

ostream& Line::print(ostream& out) {
    out << _start << " " << _end << " " << _color << " " << _size;
    return out;
}

Line Line::parse(wstring buffer) {
    auto tokens = Tokenizer::split(buffer, L" ");
    Point a(Point::parse(tokens[0]));
    Point b(Point::parse(tokens[1]));
    DWORD colr = stoul(tokens[2]);
    int shize = stoi(tokens[3]);


    Line result(Line(a, b, colr, shize));
    return result;
}

wstring Line::toString() {
    wstringstream builder;
    builder << _start.toString() << L" " << _end.toString()
        << L" " << to_wstring(getColor()) << L" " << to_wstring(getSize());
    return builder.str();
}

//-----------------------------
wstring Rectanglee::Type() {
    return L"Rectangle";
}

Point Rectanglee::getTop() {
    return _topLeft;
}

Point Rectanglee::getBottom() {
    return _rightBottom;
}

int Rectanglee::getSize() {
    return _size;
}

DWORD Rectanglee::getColor() {
    return _color;
}

void Rectanglee::setSize(int value) {
    _size = value;
}

void Rectanglee::setColor(DWORD value) {
    _color = value;
}

void Rectanglee::setTop(Point x) {
    _topLeft = x;
}

void Rectanglee::setBottom(Point x) {
    _rightBottom = x;
}

Rectanglee::Rectanglee(Point a, Point b, DWORD cl, int sz) {
    setTop(a);
    setBottom(b);
    setSize(sz);
    setColor(cl);
}

wstring Rectanglee::toString() {
    wstringstream builder;
    builder << _topLeft.toString() << L" " << _rightBottom.toString()
        << L" " << to_wstring(getColor()) << L" " << to_wstring(getSize());
    return builder.str();
}

Rectanglee Rectanglee::parse(wstring buffer) {
    auto tokens = Tokenizer::split(buffer, L" ");
    Point a(Point::parse(tokens[0]));
    Point b(Point::parse(tokens[1]));
    DWORD colr = stoul(tokens[2]);
    int shize = stoi(tokens[3]);
    Rectanglee result(Rectanglee(a, b, colr, shize));
    return result;
}

bool Rectanglee::isSelected(int x, int y) {
    if (((_topLeft.getX() < _rightBottom.getX()) && (_topLeft.getY() < _rightBottom.getY())) || ((_topLeft.getX() > _rightBottom.getX()) && (_topLeft.getY() > _rightBottom.getY()))) {
        int max_x = max(_topLeft.getX(), _rightBottom.getX());
        int min_x = min(_topLeft.getX(), _rightBottom.getX());

        int max_y = max(_topLeft.getY(), _rightBottom.getY());
        int min_y = min(_topLeft.getY(), _rightBottom.getY());

        if (min_x <= x && x <= max_x)
            if (min_y <= y && y <= max_y) return true;
        return false;
    }
    if (((_topLeft.getX() > _rightBottom.getX()) && (_topLeft.getY() < _rightBottom.getY())) || ((_topLeft.getX() < _rightBottom.getX()) && (_topLeft.getY() > _rightBottom.getY()))) {
        int max_x = max(_topLeft.getX(), _rightBottom.getX());
        int min_x = min(_topLeft.getX(), _rightBottom.getX());

        int max_y = max(_topLeft.getY(), _rightBottom.getY());
        int min_y = min(_topLeft.getY(), _rightBottom.getY());

        if (min_x <= x && x <= max_x)
            if (min_y <= y && y <= max_y) return true;
        return false;
    }
    return false;
}

ostream& Rectanglee::print(ostream& out) {
    out << _topLeft << " " << _rightBottom << _color << " " << _size;
    return out;
}

wstring Ellipsee::Type() {
    return L"Ellipse";
}

Point Ellipsee::getTop() {
    return _topLeft;
}

Point Ellipsee::getBottom() {
    return _rightBottom;
}

int Ellipsee::getSize() {
    return _size;
}

DWORD Ellipsee::getColor() {
    return _color;
}

void Ellipsee::setColor(DWORD value) {
    _color = value;
}

void Ellipsee::setSize(int value) {
    _size = value;
}

void Ellipsee::setTop(Point x) {
    _topLeft = x;
}

void Ellipsee::setBottom(Point x) {
    _rightBottom = x;
}

Ellipsee::Ellipsee(Point a, Point b, DWORD cl, int sz) {
    setTop(a);
    setBottom(b);
    setColor(cl);
    setSize(sz);
}

wstring Ellipsee::toString() {
    wstringstream builder;
    builder << _topLeft.toString() << L" " << _rightBottom.toString()
        << L" " << to_wstring(getColor()) << L" " << to_wstring(getSize());
    return builder.str();
}

bool Ellipsee::isSelected(int x, int y) {
    if (((_topLeft.getX() < _rightBottom.getX()) && (_topLeft.getY() < _rightBottom.getY())) || ((_topLeft.getX() > _rightBottom.getX()) && (_topLeft.getY() > _rightBottom.getY()))) {
        int max_x = max(_topLeft.getX(), _rightBottom.getX());
        int min_x = min(_topLeft.getX(), _rightBottom.getX());

        int max_y = max(_topLeft.getY(), _rightBottom.getY());
        int min_y = min(_topLeft.getY(), _rightBottom.getY());

        if (min_x <= x && x <= max_x)
            if (min_y <= y && y <= max_y) return true;
        return false;
    }
    if (((_topLeft.getX() > _rightBottom.getX()) && (_topLeft.getY() < _rightBottom.getY())) || ((_topLeft.getX() < _rightBottom.getX()) && (_topLeft.getY() > _rightBottom.getY()))) {
        int max_x = max(_topLeft.getX(), _rightBottom.getX());
        int min_x = min(_topLeft.getX(), _rightBottom.getX());

        int max_y = max(_topLeft.getY(), _rightBottom.getY());
        int min_y = min(_topLeft.getY(), _rightBottom.getY());

        if (min_x <= x && x <= max_x)
            if (min_y <= y && y <= max_y) return true;
        return false;
    }
    return false;
}

ostream& Ellipsee::print(ostream& out) {
    out << _topLeft << " " << _rightBottom;
    return out;
}

Ellipsee Ellipsee::parse(wstring buffer) {
    auto tokens = Tokenizer::split(buffer, L" ");
    Point a(Point::parse(tokens[0]));
    Point b(Point::parse(tokens[1]));
    DWORD colr = stoul(tokens[2]);
    int shize = stoi(tokens[3]);
    Ellipsee result(Ellipsee(a, b, colr, shize));
    return result;
}