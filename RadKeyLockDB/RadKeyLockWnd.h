#pragma once
#include "KeybHook.h"

class CVisualStyle;

class CRadKeyLockWnd :
    public ATL::CWindowImpl<CRadKeyLockWnd>
{
public:
    CRadKeyLockWnd();
    ~CRadKeyLockWnd();

    BOOL Create(
        HWND hwndParent,
        LPUNKNOWN pDeskBand,
        LPUNKNOWN pInputObjectSite);

    POINTL CalcMinimalSize() const;
    POINTL CalcIdealSize() const;
    BOOL HasFocus() const;

    BOOL Register();

BEGIN_MSG_MAP(CRadKeyLockWnd)
    MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
    MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
    MESSAGE_HANDLER(WM_PAINT, OnPaint)
    MESSAGE_HANDLER(WM_SETFOCUS, OnFocus)
    MESSAGE_HANDLER(WM_KILLFOCUS, OnFocus)
    MESSAGE_HANDLER(WM_THEMECHANGED, OnThemeChanged)
    MESSAGE_HANDLER(WM_HOOK_KEYB, OnHookKeyboard)
END_MSG_MAP()

private:
    LRESULT OnFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnThemeChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnHookKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    void Paint(HDC hdc, const RECT& rcPaint) const;

private:
    IUnknown* m_pDeskBand;
    BOOL m_fHasFocus;
    ATL::CComQIPtr<IInputObjectSite> m_spInputObjectSite;

    ATL::CAutoPtr<CVisualStyle> m_ptrVisualStyle;

    bool m_bRegistered;
};
