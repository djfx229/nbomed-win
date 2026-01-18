#include "nbomed-core/common.h"

#include <iostream>

static void print(const String& level, const String& text) {
    wprintf(L"[NboMED %ls] %ls \n", level.c_str(), text.c_str());
}

void Log::v(const String& text) {
    print(L"VERBOSE", text);
}

void Log::d(const String& text) {
    print(L"DEBUG", text);
}

void Log::w(const String& text) {
    print(L"WARNING", text);
}

void Log::e(const String& text) {
    print(L"ERROR", text);
}
