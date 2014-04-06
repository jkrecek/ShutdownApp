#ifndef WINWINDOW_H
#define WINWINDOW_H

#include "defaultwindow.h"
#include <windows.h>
#include <map>
#include <commctrl.h>
#include <list>
#include <string>
#include "configuration.h"

enum Field {
    BTN_SAVE = 1000,
    BTN_RUN_WIN,
    BTN_RUN_SERVICE,
    BTN_RUN_START,
    BTN_QUIT,
    BTN_KILL,

    LABEL_ADDRESS = 2000,
    LABEL_USERNAME,
    LABEL_PASSWORD,

    EDIT_ADDRESS = 3000,
    EDIT_USERNAME,
    EDIT_PASSWORD
};

class WinWindow : public DefaultWindow
{
public:
    WinWindow(HINSTANCE instance, int nCmdShow);
    WPARAM loop();

    LRESULT Proc(UINT msg, WPARAM wParam, LPARAM lParam);
    void addButton(int id, const char* currentText = NULL, int x = 0, int y = 0, int width = 100, int height = 24);
    void addText(int id, const char* currentText = NULL, int x = 0, int y = 0, int width = 100, int height = 24);
    void addEdit(int id, const char* currentText = NULL, int x = 0, int y = 0, int width = 100, int height = 24, int additionalFlags = 0);

    void setWindow(HWND window) { m_window = window; }

private:
    void onCreate();
    bool onCommand(WPARAM handle, LPARAM lParam);
    void addItem(int id, const char* type, const char *currentText, int x, int y, int width, int height, int flags);

    void doSave();
    void doRun(bool asService);
    void doRunOnStart();
    void doTerminateAll();

    const char* checkValues();

    std::string getExecutablePath(bool asService);

    const char* getText(Field field);
    bool startProgram(std::string executable);

    int processCount(const char* executable);
    uint terminateProcesses(const char* executable);

    Configuration* m_config;
    HGDIOBJ m_defaultFont;
    std::map<int, HWND> m_objects;
    HINSTANCE m_instance;
    HWND m_window;
};

struct WindowHolder
{
    static std::map<HWND, WinWindow* > windowMap;
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch(msg)
        {
            case WM_DESTROY:
                PostQuitMessage(0);
                break;
            default:
            {
                std::map<HWND, WinWindow *>::iterator it = windowMap.find(hwnd);
                if (it != windowMap.end()) {
                    return it->second->Proc(msg, wParam, lParam);
                } else {
                    it = windowMap.find(NULL);
                    if (it != windowMap.end()) {
                        it->second->setWindow(hwnd);
                        return it->second->Proc(msg, wParam, lParam);
                    }

                }

                break;
            }
        }

        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    static void setTemp(WinWindow* window, bool set)
    {
        if (set)
            windowMap[NULL] = window;
        else
            windowMap.erase(NULL);
    }

};

#endif // WINWINDOW_H
