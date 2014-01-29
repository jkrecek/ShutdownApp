#include "winwindow.h"
#include <iostream>
#include <tlhelp32.h>
#include "helper.h"
#include "defines.h"
#include "sha512.h"

#ifdef __WIN32
    #include <direct.h>
    #define GetCurrentDir _getcwd
    #include "../res/resources.h"
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
 #endif

const static char* appName = "NOSA-PCClient";
const static char* serverExecutable = "NOSA-Server.exe";
const static char* winName = "NOSA PC Client";

std::map<HWND, WinWindow* > WindowHolder::windowMap;

WinWindow::WinWindow(HINSTANCE instance, int nCmdShow)
    : m_instance(instance), m_defaultFont(GetStockObject(DEFAULT_GUI_FONT)), m_config(Configuration::loadFile(CONFIG_FILENAME))
{
    WNDCLASSEX wc;

    //Step 1: Registering the Window Class
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = CS_VREDRAW;
    wc.lpfnWndProc   = WindowHolder::WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = m_instance;
    wc.hIcon         = LoadIcon(m_instance, MAKEINTRESOURCE(APP_ICON));
    wc.hIconSm       = LoadIcon(m_instance, MAKEINTRESOURCE(APP_ICON));
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = appName;

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        exit(1);
    }

    WindowHolder::setTemp(this, true);

    setWindow(CreateWindowEx(
        WS_EX_CLIENTEDGE, appName, winName,
        WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 400, 215,
        HWND_DESKTOP, NULL, m_instance, NULL
    ));

    if (m_window == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!\r\n", "Error!", MB_ICONEXCLAMATION | MB_OK);
        exit(1);
    }

    WindowHolder::windowMap[m_window] = this;
    WindowHolder::setTemp(this, false);

    SendMessage(m_window, WM_SETFONT, (WPARAM)m_defaultFont, MAKELPARAM(TRUE, 0));

    ShowWindow(m_window, nCmdShow);
    UpdateWindow(m_window);
}

WPARAM WinWindow::loop()
{
    MSG Msg;

    while(GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        if (!IsDialogMessage(m_window, &Msg)) {
            TranslateMessage(&Msg);
            DispatchMessage(&Msg);
        }
    }

    return Msg.wParam;
}

LRESULT WinWindow::Proc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_CREATE:
            onCreate();
            break;
        case WM_COMMAND:
        {
            if (onCommand(wParam, lParam))
                return 0;
            else
                break;
        }
        case WM_CLOSE:
            DestroyWindow(m_window);
            break;
        default:
            break;
    }

    return DefWindowProc(m_window, msg, wParam, lParam);
}

void WinWindow::onCreate()
{
/*
    addText(LABEL_ADDRESS, "Address:", 10, 15, 60);
    addEdit(EDIT_ADDRESS, "", 70, 10, 150);

    addText(LABEL_USERNAME, "Username:", 10, 41, 60);
    addEdit(EDIT_USERNAME, "", 70, 36, 150);

    addText(LABEL_PASSWORD, "Password:", 10, 67, 60);
    addEdit(EDIT_PASSWORD, "", 70, 62, 150, 24, ES_PASSWORD);
*/

    addText(LABEL_ADDRESS, "Address:", 10, 45, 60);
    addEdit(EDIT_ADDRESS, "", 70, 40, 150);

    addText(LABEL_USERNAME, "Username:", 10, 71, 60);
    addEdit(EDIT_USERNAME, "", 70, 66, 150);

    addText(LABEL_PASSWORD, "Password:", 10, 97, 60);
    addEdit(EDIT_PASSWORD, "", 70, 92, 150, 24, ES_PASSWORD);

    addButton(BTN_SAVE, "Save", 250, 10, 120, 25);
    addButton(BTN_RUN_WIN, "Run in new window", 250, 38, 120, 25);
    addButton(BTN_RUN_SERVICE, "Run as service", 250, 66, 120, 25);
    addButton(BTN_RUN_START, "Run on start", 250, 94, 120, 25);
    addButton(BTN_KILL, "Kill", 250, 122, 120, 26);
    addButton(BTN_QUIT, "Quit", 250, 150, 120, 26);

    SendMessage(m_objects[EDIT_ADDRESS], WM_SETTEXT, 0, (LPARAM)m_config->getString("REMOTE_ADDRESS", "").c_str());
    SendMessage(m_objects[EDIT_USERNAME], WM_SETTEXT, 0, (LPARAM)m_config->getString("USER", "").c_str());
    SendMessage(m_objects[EDIT_PASSWORD], WM_SETTEXT, 0, (LPARAM)m_config->getString("PASS", "").c_str());
}

bool WinWindow::onCommand(WPARAM handle, LPARAM lParam)
{
    switch(handle)
    {
        case BTN_SAVE:
            doSave();
            break;
        case BTN_RUN_WIN:
            doRun(false);
            break;
        case BTN_RUN_SERVICE:
            doRun(true);
            break;
        case BTN_RUN_START:
            doRunOnStart();
            break;
        case BTN_QUIT:
            DestroyWindow(m_window);
            break;
        case BTN_KILL:
            doTerminateAll();
            break;
        default:
            return false;
    }

    return true;
}

void WinWindow::addButton(int id, const char* currentText, int x, int y, int width, int height)
{
    addItem(id, "Button", currentText, x, y, width, height, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_MULTILINE);
}

void WinWindow::addText(int id, const char* currentText, int x, int y, int width, int height)
{
    addItem(id, "Static", currentText, x, y, width, height, WS_CHILD | WS_VISIBLE);
}

void WinWindow::addEdit(int id, const char *currentText, int x, int y, int width, int height, int additionalFlags)
{
    addItem(id, "Edit", currentText, x, y, width, height, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT | WS_TABSTOP | additionalFlags);
}

void WinWindow::addItem(int id, const char* type, const char *currentText, int x, int y, int width, int height, int flags)
{
    HWND item = CreateWindowEx(0, type, TEXT(currentText), flags, x, y, width, height, m_window, (HMENU)id, GetModuleHandle(NULL), NULL );

    SendMessage(item, WM_SETFONT, (WPARAM)m_defaultFont, MAKELPARAM(TRUE, 0));
    m_objects[id] = item;
}

void WinWindow::doSave()
{
    if (const char* error = checkValues())
    {
        MessageBox(NULL, error, "Error!",  MB_ICONERROR | MB_OK);
        return;
    }

    m_config->setValue("REMOTE_ADDRESS", getText(EDIT_ADDRESS));
    m_config->setValue("USER", getText(EDIT_USERNAME));
    m_config->setValue("PASS", Sha512(getText(EDIT_PASSWORD)).to_string());

    m_config->save();

    MessageBox(NULL, "Server connection info successfully saved.", "Success!",  MB_ICONINFORMATION | MB_OK);
}

void WinWindow::doRun(bool asService)
{
    if (processCount(serverExecutable) > 0)
    {
        MessageBox(NULL, "Server executable is already running. ", "Error!",  MB_ICONERROR | MB_OK);
        return;
    }

    std::string runPath = getExecutablePath(asService);
    if (runPath != "")
        if (startProgram(runPath))
            MessageBox(NULL, "Server successfully started.", "Success",  MB_ICONINFORMATION | MB_OK);
}

void WinWindow::doRunOnStart()
{
    std::string runPath = getExecutablePath(true);

    HKEY hKey;
    LONG result = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_WRITE, &hKey);
    if (result == ERROR_SUCCESS)
    {
        if ((result = RegSetValueExA( hKey, "NOSA-Server", 0, REG_SZ, (BYTE*)runPath.c_str(), runPath.length())) == ERROR_SUCCESS)
            MessageBox(NULL, "Now NOSA-Server service will launch of computer start.", "Done!", MB_ICONINFORMATION | MB_OK);
        else
            MessageBox(NULL, "You don't have sufficient rights to edit your registry. Please try run this application with admin privilege.", "Error!",  MB_ICONERROR | MB_OK);
    }
    else
        MessageBox(NULL, "Your registry has wrong format!", "Error!", MB_ICONERROR | MB_OK);

    RegCloseKey(hKey);
    return;
}

const char* WinWindow::getText(Field field)
{
    HWND handle = m_objects[field];
    char buffer[256];
    int len = SendMessage(handle, WM_GETTEXTLENGTH, 0, 0);
    SendMessage(handle, WM_GETTEXT, (WPARAM)len + 1, (LPARAM)buffer);
    return strdup(buffer);
}

std::string WinWindow::getExecutablePath(bool asService)
{
    char cCurrentPath[FILENAME_MAX];
    if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
    {
        MessageBox(NULL, "Couldn't find current directory.", "Error!",  MB_ICONERROR | MB_OK);
        return "";
    }

    std::string pathStart = cCurrentPath + std::string("\\");
    std::string hstart = pathStart + "hstart.exe";

    std::string path;

    if (asService)
    {
        if (!Helper::file_exists(hstart))
        {
            MessageBox(NULL, "Please place file hstart.exe to application directory.", "Error!",  MB_ICONERROR | MB_OK);
            return "";
        }

        path = "\"" + hstart + "\" /NOCONSOLE ";
    }
    else
        path = "";

    std::string server = pathStart + serverExecutable;
    if (!Helper::file_exists(server))
    {
        MessageBox(NULL, "Please place file NOSA-Server.exe to application directory. ", "Error!",  MB_ICONERROR | MB_OK);
        return "";
    }

    path += "\"" + server + "\"";

    return path.c_str();
}

bool WinWindow::startProgram(std::string executable)
{
    const char* fullExe = executable.c_str();;

    char* nextChar = (char*)memchr(fullExe + 1 , '\"', executable.length());
    int findLen = (int)nextChar - (int)fullExe - 1;
    char* file = (char*)malloc(findLen + 1);
    memcpy(file, fullExe + 1, findLen);
    file[findLen] = '\0';

    char* parameter = NULL;
    if (*(nextChar + 1) != '\0')
    {
        int restLen = executable.length() - findLen;
        parameter = (char*)malloc(restLen + 1);
        memcpy(parameter, nextChar + 1, restLen-2);
        parameter[restLen-2] = '\0';
    }

    HINSTANCE hRet = ShellExecute(NULL, "open", file, parameter, NULL, SW_SHOW);

    if ((LONG)hRet <= 32)
    {
        MessageBox(HWND_DESKTOP, "Unable to start program", "Error", MB_ICONERROR | MB_OK);
        return false;
    }

    return true;
}

int WinWindow::processCount(const char *executable)
{
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    int count = 0;
    if (Process32First(snapshot, &entry))
        while (Process32Next(snapshot, &entry))
            if (stricmp(entry.szExeFile, executable) == 0)
                ++count;

    CloseHandle(snapshot);
    return count;
}

void WinWindow::terminateProcesses(const char *executable)
{
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (Process32First(snapshot, &entry))
    {
        HANDLE hProcess;
        while (Process32Next(snapshot, &entry))
        {
            if (stricmp(entry.szExeFile, executable) == 0)
            {
                hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
                TerminateProcess(hProcess, 1);
                CloseHandle(hProcess);
            }
        }
    }

    CloseHandle(snapshot);
}

void WinWindow::doTerminateAll()
{
    terminateProcesses(serverExecutable);

    MessageBox(NULL, "All NOSA servers are beeing terminated.", "Done!", MB_ICONINFORMATION | MB_OK);
}

const char* WinWindow::checkValues()
{
    const char* address =  getText(EDIT_ADDRESS);
    const char* username = getText(EDIT_USERNAME);
    const char* password = getText(EDIT_PASSWORD);

    if (!address || !strlen(address) || !username || !strlen(username) || !password || !strlen(password))
        return "All fields must be filled before saving.";

    if (strlen(username) < 4)
        return "Your username must be at least 5 characters long.";

    if (strlen(password) < 5)
        return "Your username must be at least 5 characters long.";

    return NULL;
}
