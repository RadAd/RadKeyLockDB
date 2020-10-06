#include <Windows.h>
#include <algorithm>
#include <vector>

const WCHAR szWindowClass[] = L"RADKEYLOCK";

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    MyRegisterClass(hInstance);
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = {};

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wcex.lpszClassName = szWindowClass;

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd = CreateWindowEx(0, szWindowClass, TEXT("Rad Key Lock"), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 15, 40, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
        return FALSE;

    //ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

HHOOK hHook = NULL;
std::vector<HWND> hHookWnds;
#define WM_HOOK_KEYB        (WM_USER + 421)
#define WM_HOOK_KEYB_REG    (WM_USER + 422)

LRESULT CALLBACK keyboardll_hook(const int code, const WPARAM wParam, const LPARAM lParam)
{
    if (code == HC_ACTION)
    {
        const KBDLLHOOKSTRUCT* kbdStruct = (KBDLLHOOKSTRUCT*) lParam;

        hHookWnds.erase(std::remove_if(hHookWnds.begin(), hHookWnds.end(), [](HWND hHookWnd)
            {
                return !IsWindow(hHookWnd);
            }), hHookWnds.end());

        std::for_each(hHookWnds.begin(), hHookWnds.end(), [&kbdStruct](HWND hHookWnd)
            {
                PostMessage(hHookWnd, WM_HOOK_KEYB, kbdStruct->vkCode, kbdStruct->flags);
            });
    }

    return CallNextHookEx(hHook, code, wParam, lParam);
}

struct KeyData
{
    const int vk;
    bool down;
    bool lock;

    void init()
    {
        SHORT keystate = GetKeyState(vk);
        down = keystate & 0x80;
        lock = keystate & 0x01;
    }
};

KeyData Keys[] = { { VK_SCROLL }, { VK_CAPITAL }, { VK_NUMLOCK } };

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        LRESULT ret = DefWindowProc(hWnd, message, wParam, lParam);
        //SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        hHookWnds.push_back(hWnd);
        std::for_each(std::begin(Keys), std::end(Keys), [](KeyData& kd) { kd.init(); });
        hHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardll_hook, NULL, 0);
        return ret;
    }
    break;
    case WM_HOOK_KEYB:
    {
        auto it = std::find_if(std::begin(Keys), std::end(Keys), [&wParam](const KeyData& kd)
            {
                return kd.vk == wParam;
            });
        //if (std::find(std::begin(Keys), std::end(Keys), wParam) != std::end(Keys))
        if (it != std::end(Keys))
        {
            const bool bDown = !(lParam & LLKHF_UP);
            if (!it->down && bDown)
                it->lock = !it->lock;
            it->down = bDown;
            InvalidateRect(hWnd, nullptr, FALSE);
        }
    }
    break;
    case WM_HOOK_KEYB_REG:
    {
        if ((BOOL) lParam)
        {
            auto it = std::find(hHookWnds.begin(), hHookWnds.end(), (HWND) wParam);
            if (it == hHookWnds.end())
                hHookWnds.push_back((HWND) wParam);
        }
        else
        {
            auto it = std::find(hHookWnds.begin(), hHookWnds.end(), (HWND) wParam);
            if (it != hHookWnds.end())
                hHookWnds.erase(it);
        }
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
