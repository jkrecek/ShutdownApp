#include <windows.h>

#include "winwindow.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WinWindow window(hInstance, nCmdShow);
    return window.loop();
}
