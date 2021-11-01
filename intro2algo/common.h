#pragma once
#include <tchar.h>
#include <string>
using std::string, std::wstring;

// Convert a wide Unicode string to an UTF8 string
string utf8_encode(const wstring& wstr);
string utf8_encode(const _TCHAR wstr);

// Convert an UTF8 string to a wide Unicode String
wstring utf8_decode(const string& str);


