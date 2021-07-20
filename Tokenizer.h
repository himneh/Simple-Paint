#pragma once
#include <vector>
#include <string>

using namespace std;

class Tokenizer {
public:
    static vector<wstring> split(wstring haystack, wstring  needle);
};

class WTokenizer {
public:
    static vector<wstring> split(wstring haystack, wstring  needle);
};