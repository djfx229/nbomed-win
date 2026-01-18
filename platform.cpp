#include "platform.h"

#include <string>
#include <windows.h>

PlatformWindow::PlatformWindow()
        : currentHdc(NULL),
          inputState(0),
          m_gridDimension(NULL),
          handleWindow(NULL) {
    setlocale(LC_ALL, "");
    m_gridDimension = new GridDimension();
    m_gridDimension->tileSizeW = 24;
    m_gridDimension->tileSizeH = 24;
    m_gridDimension->tileCountW = 20;
    m_gridDimension->tileCountH = 18;
}

void PlatformWindow::needRedrawing() {
    Log::v(L"DisplayOutputImpl: needRedrawing");
    RedrawWindow(handleWindow, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
}

void PlatformWindow::drawString(RectPixels* rect, String text) {
    Log::v(L"DisplayOutputImpl: drawString");

    int tileW = getGridDimension()->tileSizeW;
    int tileH = getGridDimension()->tileSizeH;

    for (size_t charPositionX = 0; charPositionX < text.length(); ++charPositionX) {
        int x = rect->x + (charPositionX * tileW);
        RECT nativeRect = {
            x,
            rect->y,
            x + tileW,
            rect->y + tileH
        };

        wchar_t buffer[2] = { text[charPositionX], L'\0' };
        DrawTextW(currentHdc, buffer, -1, &nativeRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
}

void PlatformWindow::fillColor(RectPixels *rect, Color color) {
    RECT nativeRect = {
        rect->x,
        rect->y,
        rect->x + rect->width,
        rect->y + rect->height
    };
    DWORD hexValue = getValueForColor(color);
    COLORREF nativeColor = RGB(
        (hexValue >> 16) & 0xFF,
        (hexValue >> 8) & 0xFF,
        (hexValue & 0xFF)
    );
    HBRUSH hBrush = CreateSolidBrush(nativeColor);
    FillRect(currentHdc, &nativeRect, hBrush);
    DeleteObject(hBrush);
}

DWORD PlatformWindow::getValueForColor(Color color) {
    switch (color) {
        case COLOR_SCREEN_BACKGROUND:
            return 0x94d9f8;
        case COLOR_TEXT_PLAIN:
            return 0x000000;
        case COLOR_TEXT_SELECTED:
            return 0xbff9fc;
        case COLOR_TEXT_BACKGROUND:
            return 0xeb6683;
        case COLOR_RANDOM:
            int r = rand() % 256;
            int g = rand() % 256;
            int b = rand() % 256;
            return static_cast<DWORD>((r << 16) | (g << 8) | b);
    }
    return 0x94d9f8;
}

GridDimension* PlatformWindow::getGridDimension() const {
    return m_gridDimension;
}

void PlatformWindow::show(PlatformEventsListener *listener) {
    HINSTANCE hInst = GetModuleHandle(NULL);
    int nCmdShow = SW_SHOW;
    LPCSTR CLASS_NAME = "SampleWindowClass";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInst;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    RegisterClass(&wc);

    const int windowWidth = m_gridDimension->getScreenWidth();
    const int windowHeight = m_gridDimension->getScreenHeight();

    const HWND hwnd = CreateWindowExA(0, CLASS_NAME, "NboMED", (WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME | WS_MAXIMIZEBOX)),
                                CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight,
                                NULL, NULL, hInst, this);

    if (hwnd == NULL) return;

    handleWindow = hwnd;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    listener->onStart();

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        ButtonsState nextInputState = 0;
        if (GetAsyncKeyState(VK_SPACE) & 0x8000) nextInputState |= BUTTON_PLAY_OR_PAUSE;
        if (GetAsyncKeyState('L') & 0x8000) nextInputState |= BUTTON_SELECT;
        if (GetAsyncKeyState('K') & 0x8000) nextInputState |= BUTTON_A;
        if (GetAsyncKeyState('J') & 0x8000) nextInputState |= BUTTON_B;
        if (GetAsyncKeyState('W') & 0x8000) nextInputState |= BUTTON_D_PAD_UP;
        if (GetAsyncKeyState('S') & 0x8000) nextInputState |= BUTTON_D_PAD_DOWN;
        if (GetAsyncKeyState('A') & 0x8000) nextInputState |= BUTTON_D_PAD_LEFT;
        if (GetAsyncKeyState('D') & 0x8000) nextInputState |= BUTTON_D_PAD_RIGHT;

        if (nextInputState != inputState) {
            inputState = nextInputState;
            listener->onChangeButtonsState(inputState);
        }
    }

    listener->onFinish();
}

LRESULT CALLBACK PlatformWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // https://devblogs.microsoft.com/oldnewthing/20140203-00/
    PlatformWindow *pThis; // our "this" pointer will go here
    if (uMsg == WM_NCCREATE) {
        LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
        pThis = static_cast<PlatformWindow *>(lpcs->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    } else {
        pThis = reinterpret_cast<PlatformWindow *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    switch (uMsg) {
        case WM_PAINT: {
            if (pThis) {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                pThis->currentHdc = hdc;
                pThis->handleWindow = hwnd;

                SetBkMode(hdc, TRANSPARENT);
                pThis->onHostStartDraw(pThis);
                EndPaint(hwnd, &ps);
            }

            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        default: ;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
