#include "pch.h"
#include "KeybHook.h"

extern HINSTANCE g_hInstance;

const WCHAR szWindowClass[] = L"RADKEYLOCK";

BOOL RegisterKeyboardHook(HWND hWnd)
{
    HWND hRegWnd = FindWindow(szWindowClass, nullptr);

    if (hRegWnd == NULL)
    {
        TCHAR FileName[MAX_PATH];
        GetModuleFileName(g_hInstance, FileName, ARRAYSIZE(FileName));
        WCHAR* pFN = PathFindFileNameW(FileName);
        wcscpy_s(pFN, ARRAYSIZE(FileName) - (pFN - FileName), L"RadKeyLock.exe");

        STARTUPINFO si = {};
        PROCESS_INFORMATION pi = {};
        if (CreateProcess(nullptr,   // No module name (use command line)
            FileName,       // Command line
            NULL,           // Process handle not inheritable
            NULL,           // Thread handle not inheritable
            FALSE,          // Set handle inheritance to FALSE
            0,              // No creation flags
            NULL,           // Use parent's environment block
            NULL,           // Use parent's starting directory 
            &si,
            &pi))
        {
            WaitForInputIdle(pi.hProcess, INFINITE);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);

            hRegWnd = FindWindow(szWindowClass, nullptr);
        }
    }

    if (hRegWnd != NULL)
        PostMessage(hRegWnd, WM_HOOK_KEYB_REG, (WPARAM) hWnd, TRUE);

    return hRegWnd != NULL;
}

BOOL UnregisterKeyboardHook(HWND hWnd)
{
    HWND hRegWnd = FindWindow(szWindowClass, nullptr);
    if (hRegWnd != NULL)
        PostMessage(hRegWnd, WM_HOOK_KEYB_REG, (WPARAM) hWnd, FALSE);

    return TRUE;
}
