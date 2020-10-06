#pragma once

#define WM_HOOK_KEYB		(WM_USER + 421)
#define WM_HOOK_KEYB_REG    (WM_USER + 422)

BOOL RegisterKeyboardHook(HWND hWnd);
BOOL UnregisterKeyboardHook(HWND hWnd);
