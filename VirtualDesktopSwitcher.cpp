#include <windows.h>
#include <iostream>
#include <fstream>

typedef DWORD(*GetCurrentDesktopNumberFunc)();
typedef DWORD(*GetDesktopCountFunc)();
typedef DWORD(*GoToDesktopNumberFunc)(DWORD id);

HMODULE hModule = NULL;
GetCurrentDesktopNumberFunc GetCurrentDesktopNumber = NULL;
GetDesktopCountFunc GetDesktopCount = NULL;
GoToDesktopNumberFunc GoToDesktopNumber = NULL;

void errorExit(const char* reason)
{
    std::ofstream fLog("logs.log");
    fLog << "[ERROR] " << reason << std::endl;
    exit(1);
}

bool unloadDll()
{
    return FreeLibrary(hModule);
}

bool loadDll()
{
    hModule = LoadLibrary(TEXT("VirtualDesktopAccessor.dll"));
    if (hModule == NULL)
        errorExit("Failed to load the DLL.");

    GetCurrentDesktopNumber = (GetCurrentDesktopNumberFunc)GetProcAddress(hModule, "GetCurrentDesktopNumber");
    if (GetCurrentDesktopNumber == NULL) {
        errorExit("Failed to locate GetCurrentDesktopNumber.");
    }
    GetDesktopCount = (GetDesktopCountFunc)GetProcAddress(hModule, "GetDesktopCount");
    if (GetDesktopCount == NULL) {
        errorExit("Failed to locate GetDesktopCount.");
    }
    GoToDesktopNumber = (GoToDesktopNumberFunc)GetProcAddress(hModule, "GoToDesktopNumber");
    if (GoToDesktopNumber == NULL) {
        errorExit("Failed to locate GoToDesktopNumber.");
    }
    return true;
}

int CALLBACK WinMain(
    _In_  HINSTANCE hInstance,
    _In_  HINSTANCE hPrevInstance,
    _In_  LPSTR lpCmdLine,
    _In_  int nCmdShow
) 
{
    loadDll();

    DWORD currentId = GetCurrentDesktopNumber();
    if (currentId > 0)
    {
        GoToDesktopNumber(currentId - 1);
    }
    else
    {
        DWORD totalCount = GetDesktopCount();
        if (totalCount <= 1)
            return 0;
        else
            GoToDesktopNumber(totalCount - 1);
    }

    unloadDll();
    return 0;
}