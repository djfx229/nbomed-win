#ifndef NBOMED_PLATFORM_H
#define NBOMED_PLATFORM_H
#include <string>
#include <windows.h>

#include "nbomed-core/application.h"
#include "nbomed-core/display.h"
#include "nbomed-core/common.h"

class PlatformWindow : public DisplayOutput, public IDisplayCanvas {
    HDC currentHdc;
    HWND handleWindow;
    ButtonsState inputState;
    GridDimension *m_gridDimension;

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static DWORD getValueForColor(Color color);

public:
    PlatformWindow();

    void drawString(RectPixels *rect, String text);

    void fillColor(RectPixels *rect, Color color);

    void needRedrawing();

    GridDimension *getGridDimension() const;

    void show(PlatformEventsListener *listener);
};

#endif //NBOMED_PLATFORM_H