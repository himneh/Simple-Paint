#pragma once
#include <sstream>
#include <string>
#include "framework.h"
#include "IShapes.h"
#include "Tokenizer.h"

using namespace std;

class Text {
private:
	WCHAR* _text;
	int _length;
	int _coorX;
	int _coorY;
	DWORD _color;
	int _size;
public:
	Text() {}
	~Text() {}
	WCHAR* getText();
	int getLength();
	int getX();
	int getY();
	DWORD getColor();
	int getSize();

	void setText(WCHAR*);
	void setLength(int);
	void setX(int);
	void setY(int);
	void setColor(DWORD);
	void setSize(int);

	Text(WCHAR*, int, int, int, DWORD, int);

	static Text parse(wstring);

	wstring toString();
};