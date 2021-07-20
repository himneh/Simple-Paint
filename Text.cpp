#include "Text.h"
#include <fstream>

WCHAR* Text::getText() {
	return _text;
}

int Text::getLength() {
	return _length;
}

int Text::getX() {
	return _coorX;
}

int Text::getY() {
	return _coorY;
}

DWORD Text::getColor() {
	return _color;
}

int Text::getSize() {
	return _size;
}

void Text::setText(WCHAR* value) {
	_text = value;
}

void Text::setLength(int value) {
	_length = value;
}
void Text::setX(int value) {
	_coorX = value;
}

void Text::setY(int value) {
	_coorY = value;
}

void Text::setColor(DWORD value) {
	_color = value;
}

void Text::setSize(int value) {
	_size = value;
}

Text::Text(WCHAR* text, int len, int xx, int yy, DWORD cl, int sz) {
	setText(text);
	setLength(len);
	setX(xx);
	setY(yy);
	setColor(cl);
	setSize(sz);
}

//Text::~Text() {
//	delete[] _text;
//}

Text Text::parse(wstring buffer) {
	auto tokens = WTokenizer::split(buffer, L" //0"); 
		
	wstring tam = tokens[0];
	WCHAR* text_ = new WCHAR[tam.size() + 1];
	for (int i = 0; i < tam.size() + 1; i++) {
		text_[i] = tam[i];
	}
	wstring tmp = tokens[1];

	auto tkens = WTokenizer::split(tmp, L" "); 

	int len_ = stoi(tkens[0]);
	int x_ = stoi(tkens[1]);
	int y_ = stoi(tkens[2]);
	DWORD cl_ = stoul(tkens[3]);
	int sz_ = stoi(tkens[4]);

	Text txt(text_, len_, x_, y_, cl_, sz_);
	return txt;
}

wstring Text::toString() {
	wstringstream builder;
	builder << getText() << L" //0" << getLength() << " " << getX() << " " << getY() << " " << getColor() << " " << getSize();
	wstring result = builder.str();
	return result;
}