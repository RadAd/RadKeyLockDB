#include "pch.h"
#include "RadKeyLockWnd.h"
#include "GdiUtils.h"
#include "VisualStyle.h"

#include <algorithm>

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

KeyData g_Keys[] = { { VK_SCROLL }, { VK_CAPITAL }, { VK_NUMLOCK } };

CRadKeyLockWnd::CRadKeyLockWnd()
    : m_pDeskBand(NULL)
    , m_fHasFocus(FALSE)
    , m_ptrVisualStyle(CVisualStyle::Create())
    , m_bRegistered(false)
{
}

CRadKeyLockWnd::~CRadKeyLockWnd()
{
}

BOOL CRadKeyLockWnd::Create(
    HWND hwndParent,
    LPUNKNOWN pDeskBand,
    LPUNKNOWN pInputObjectSite)
{
    if (!__super::Create(hwndParent))
        return FALSE;

    ATLASSERT(pDeskBand);
    m_pDeskBand = pDeskBand;

    ATLASSERT(pInputObjectSite);
    m_spInputObjectSite = pInputObjectSite;

    std::for_each(std::begin(g_Keys), std::end(g_Keys), [](KeyData& kd) { kd.init(); });

    m_bRegistered = RegisterKeyboardHook(m_hWnd);

    return TRUE;
}

POINTL CRadKeyLockWnd::CalcMinimalSize() const
{
    const POINTL pt = {
        ::GetSystemMetrics(SM_CXMIN),
        ::GetSystemMetrics(SM_CYMIN)
    };

    return pt;
}

POINTL CRadKeyLockWnd::CalcIdealSize() const
{
    if (!IsWindow()) return CalcMinimalSize();

    const POINTL pt = {
        15,
        ::GetSystemMetrics(SM_CYMIN)
    };

    return pt;
}

BOOL CRadKeyLockWnd::HasFocus() const
{
    return m_fHasFocus;
}

BOOL CRadKeyLockWnd::Register()
{
    m_bRegistered = RegisterKeyboardHook(m_hWnd);
    Invalidate();
    return m_bRegistered;
}

LRESULT CRadKeyLockWnd::OnFocus(
    UINT uMsg,
    WPARAM /*wParam*/,
    LPARAM /*lParam*/,
    BOOL& /*bHandled*/)
{
    m_fHasFocus = (uMsg == WM_SETFOCUS);

    if (m_spInputObjectSite)
        m_spInputObjectSite->OnFocusChangeIS(m_pDeskBand, m_fHasFocus);

    return 0L;
}

LRESULT CRadKeyLockWnd::OnDestroy(
    UINT /*uMsg*/,
    WPARAM /*wParam*/,
    LPARAM /*lParam*/,
    BOOL& /*bHandled*/)
{
    UnregisterKeyboardHook(m_hWnd);

    return 0;
}

LRESULT CRadKeyLockWnd::OnEraseBackground(
    UINT /*uMsg*/,
    WPARAM /*wParam*/,
    LPARAM /*lParam*/,
    BOOL& /*bHandled*/)
{
    return 1;
}

LRESULT CRadKeyLockWnd::OnPaint(
    UINT /*uMsg*/,
    WPARAM /*wParam*/,
    LPARAM /*lParam*/,
    BOOL& /*bHandled*/)
{
    DoubleBufferPaint dbuffPaint(m_hWnd);

    Paint(dbuffPaint.GetDC(), dbuffPaint.GetPaintRect());

    return 0;
}

LRESULT CRadKeyLockWnd::OnThemeChanged(
    UINT /*uMsg*/,
    WPARAM /*wParam*/,
    LPARAM /*lParam*/,
    BOOL& /*bHandled*/)
{
    // re-create theme style
    m_ptrVisualStyle.Free();
    m_ptrVisualStyle.Attach(CVisualStyle::Create());

    return 0;
}

LRESULT CRadKeyLockWnd::OnHookKeyboard(
    UINT /*uMsg*/,
    WPARAM wParam,
    LPARAM lParam,
    BOOL& /*bHandled*/)
{
    auto it = std::find_if(std::begin(g_Keys), std::end(g_Keys), [&wParam](const KeyData& kd)
        {
            return kd.vk == wParam;
        });

    if (it != std::end(g_Keys))
    {
        const bool bDown = !(lParam & LLKHF_UP);
        if (!it->down && bDown)
            it->lock = !it->lock;
        it->down = bDown;
        Invalidate();
    }

    return 0;
}

void CRadKeyLockWnd::Paint(HDC hdc, const RECT& rcPaint) const
{
    m_ptrVisualStyle->DrawBackground(m_hWnd, hdc, rcPaint);

    RECT rcClient = { 0 };
    GetClientRect(&rcClient);
    InflateRect(&rcClient, 0, -5);

    GDIPtr<HPEN> hBorderPen(CreatePen(PS_SOLID, 0, m_ptrVisualStyle->GetTextColor()));
    GDIPtr<HBRUSH> hEnabledBrush(CreateSolidBrush(RGB(255, 255, 255)));
    GDIPtr<HBRUSH> hDisabledBrush(CreateSolidBrush(RGB(128, 128, 128)));
    GDIPtr<HBRUSH> hErrorBrush(CreateSolidBrush(RGB(128, 0, 0)));

    const LONG spacing = 2;
    const LONG h = (Height(rcClient) + spacing) / (LONG) ARRAYSIZE(g_Keys);

    SelectObject(hdc, hBorderPen);

    RECT rcBox(rcClient);
    rcBox.bottom = rcBox.top + h - spacing;
    for (const KeyData& k : g_Keys)
    {
        if (!m_bRegistered)
            SelectObject(hdc, hErrorBrush);
        else
            SelectObject(hdc, k.lock ? hEnabledBrush : hDisabledBrush);
        Rectangle(hdc, rcBox);

        OffsetRect(&rcBox, 0, h);
    }

    SelectObject(hdc, GetStockObject(WHITE_PEN));
    SelectObject(hdc, GetSysColorBrush(COLOR_WINDOW));
}
