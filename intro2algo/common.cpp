#include "common.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

string utf8_encode(const wstring& wstr)
{
    if (wstr.empty()) return string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    
    return strTo;
}

string utf8_encode(const _TCHAR* str)
{
    if (sizeof(_TCHAR) == sizeof(char))
        return string(reinterpret_cast<char*>(const_cast<_TCHAR*>(str)));
    
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, str, _tcslen(str), NULL, 0, NULL, NULL);
    string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, str, _tcslen(str), &strTo[0], size_needed, NULL, NULL);

    return strTo;
}


wstring utf8_decode(const string& str)
{
    if (str.empty()) return wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    
    return wstrTo;
}

