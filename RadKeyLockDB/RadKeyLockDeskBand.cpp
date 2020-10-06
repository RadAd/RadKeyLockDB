#include "pch.h"
#include "RadKeyLockDeskBand.h"

enum
{
    IDM_RECONNECT_OFFSET,
};

CRadKeyLockDeskBand::CRadKeyLockDeskBand()
    : m_nBandID(0)
    , m_dwViewMode(DBIF_VIEWMODE_NORMAL)
    , m_bRequiresSave(false)
    , m_bCompositionEnabled(TRUE)
{
}

HRESULT CRadKeyLockDeskBand::UpdateDeskband()
{
    ATL::CComPtr<IInputObjectSite> spInputSite;
    HRESULT hr = GetSite(IID_IInputObjectSite, reinterpret_cast<void**>(&spInputSite));

    if (SUCCEEDED(hr))
    {
        ATL::CComQIPtr<IOleCommandTarget> spOleCmdTarget;
        spOleCmdTarget = spInputSite;

        if (spOleCmdTarget)
        {
            // m_nBandID must be `int' or bandID variant must be explicitly
            // set to VT_I4, otherwise IDeskBand::GetBandInfo won't
            // be called by the system.
            ATL::CComVariant bandID(m_nBandID);

            hr = spOleCmdTarget->Exec(&CGID_DeskBand, DBID_BANDINFOCHANGED, OLECMDEXECOPT_DODEFAULT, &bandID, NULL);
            ATLASSERT(SUCCEEDED(hr));

            if (SUCCEEDED(hr))
                m_wndRadKeyLock.Invalidate();
        }
    }

    return hr;
}

HRESULT STDMETHODCALLTYPE CRadKeyLockDeskBand::SetSite(
    /* [in] */ IUnknown* pUnkSite)
{
    HRESULT hr = __super::SetSite(pUnkSite);

    if (SUCCEEDED(hr) && pUnkSite) // pUnkSite is NULL when band is being destroyed
    {
        ATL::CComQIPtr<IOleWindow> spOleWindow = pUnkSite;

        if (spOleWindow)
        {
            HWND hwndParent = NULL;
            hr = spOleWindow->GetWindow(&hwndParent);

            if (SUCCEEDED(hr))
            {
                m_wndRadKeyLock.Create(hwndParent, static_cast<IDeskBand*>(this), pUnkSite);

                if (!m_wndRadKeyLock.IsWindow())
                    hr = E_FAIL;
            }
        }
    }

    return hr;
}

HRESULT STDMETHODCALLTYPE CRadKeyLockDeskBand::UIActivateIO(
    /* [in] */ BOOL fActivate,
    /* [unique][in] */ PMSG /*pMsg*/)
{
    ATLTRACE(ATL::atlTraceCOM, 2, _T("IInputObject::UIActivateIO (%s)\n"), (fActivate ? _T("TRUE") : _T("FALSE")));

    if (fActivate)
        m_wndRadKeyLock.SetFocus();

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CRadKeyLockDeskBand::HasFocusIO()
{
    ATLTRACE(ATL::atlTraceCOM, 2, _T("IInputObject::HasFocusIO\n"));

    return (m_wndRadKeyLock.HasFocus() ? S_OK : S_FALSE);
}

HRESULT STDMETHODCALLTYPE CRadKeyLockDeskBand::TranslateAcceleratorIO(
    /* [in] */ PMSG /*pMsg*/)
{
    ATLTRACE(ATL::atlTraceCOM, 2, _T("IInputObject::TranslateAcceleratorIO\n"));

    return S_FALSE;
}

HRESULT STDMETHODCALLTYPE CRadKeyLockDeskBand::QueryContextMenu(
    /* [in] */ HMENU hMenu,
    /* [in] */ UINT indexMenu,
    /* [in] */ UINT idCmdFirst,
    /* [in] */ UINT /*idCmdLast*/,
    /* [in] */ UINT uFlags)
{
    ATLTRACE(ATL::atlTraceCOM, 2, _T("IContextMenu::QueryContextMenu\n"));

    if (CMF_DEFAULTONLY & uFlags)
        return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);

    ATL::CString sCaption;
    ATLVERIFY(sCaption.LoadString(IDS_RECONNECT));

    ::InsertMenu(hMenu, indexMenu, MF_STRING | MF_BYPOSITION, idCmdFirst + (UINT) IDM_RECONNECT_OFFSET, sCaption);

    return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, IDM_RECONNECT_OFFSET + 1);
}

HRESULT STDMETHODCALLTYPE CRadKeyLockDeskBand::InvokeCommand(
    /* [in] */ LPCMINVOKECOMMANDINFO pici)
{
    ATLTRACE(ATL::atlTraceCOM, 2, _T("IContextMenu::InvokeCommand\n"));

    if (!pici)
        return E_INVALIDARG;

    if (LOWORD(pici->lpVerb) == IDM_RECONNECT_OFFSET)
    {
        ATLASSERT(m_wndRadKeyLock.IsWindow());
        if (!m_wndRadKeyLock.Register())
            MessageBox(m_wndRadKeyLock, L"Error connecting", L"Rad Key Lock", MB_OK | MB_ICONERROR);
    }

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CRadKeyLockDeskBand::GetCommandString(
    /* [in] */ UINT_PTR /*idCmd*/,
    /* [in] */ UINT /*uType*/,
    /* [in] */ UINT* /*pReserved*/,
    /* [out] */ LPSTR /*pszName*/,
    /* [in] */ UINT /*cchMax*/)
{
    ATLTRACE(ATL::atlTraceCOM, 2, _T("IContextMenu::GetCommandString\n"));

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// IDeskBand

HRESULT STDMETHODCALLTYPE CRadKeyLockDeskBand::GetBandInfo(
    /* [in] */ DWORD dwBandID,
    /* [in] */ DWORD dwViewMode,
    /* [out][in] */ DESKBANDINFO* pdbi)
{
    ATLTRACE(ATL::atlTraceCOM, 2, _T("IDeskBand::GetBandInfo\n"));

    if (!pdbi) return E_INVALIDARG;

    m_nBandID = dwBandID;
    m_dwViewMode = dwViewMode;

    if (pdbi->dwMask & DBIM_MODEFLAGS)
    {
        pdbi->dwModeFlags = DBIMF_VARIABLEHEIGHT;
    }

    if (pdbi->dwMask & DBIM_MINSIZE)
    {
        pdbi->ptMinSize = m_wndRadKeyLock.CalcIdealSize();
    }

    if (pdbi->dwMask & DBIM_MAXSIZE)
    {
        // the band object has no limit for its maximum height
        pdbi->ptMaxSize.x = -1;
        pdbi->ptMaxSize.y = -1;
    }

    if (pdbi->dwMask & DBIM_INTEGRAL)
    {
        pdbi->ptIntegral.x = 1;
        pdbi->ptIntegral.y = 1;
    }

    if (pdbi->dwMask & DBIM_ACTUAL)
    {
        pdbi->ptActual = m_wndRadKeyLock.CalcIdealSize();
    }

    if (pdbi->dwMask & DBIM_TITLE)
    {
        if (dwViewMode == DBIF_VIEWMODE_FLOATING)
        {
            ATL::CString sDate;
            ATLVERIFY(sDate.LoadString(IDS_TITLE));
            lstrcpynW(pdbi->wszTitle, sDate, ARRAYSIZE(pdbi->wszTitle));
        }
        else
            pdbi->dwMask &= ~DBIM_TITLE; // do not show title
    }

    if (pdbi->dwMask & DBIM_BKCOLOR)
    {
        //Use the default background color by removing this flag.
        pdbi->dwMask &= ~DBIM_BKCOLOR;
    }

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CRadKeyLockDeskBand::CanRenderComposited(
    /* [out] */ BOOL* pfCanRenderComposited)
{
    if (pfCanRenderComposited)
        *pfCanRenderComposited = TRUE;

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CRadKeyLockDeskBand::SetCompositionState(
    /* [in] */ BOOL fCompositionEnabled)
{
    m_bCompositionEnabled = fCompositionEnabled;
    m_wndRadKeyLock.Invalidate();

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CRadKeyLockDeskBand::GetCompositionState(
    /* [out] */ BOOL* pfCompositionEnabled)
{
    if (pfCompositionEnabled)
        *pfCompositionEnabled = m_bCompositionEnabled;

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CRadKeyLockDeskBand::GetWindow(
    /* [out] */ HWND* phwnd)
{
    ATLTRACE(ATL::atlTraceCOM, 2, _T("IOleWindow::GetWindow\n"));

    if (phwnd) *phwnd = m_wndRadKeyLock;

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CRadKeyLockDeskBand::ContextSensitiveHelp(
    /* [in] */ BOOL /*fEnterMode*/)
{
    ATLTRACE(ATL::atlTraceCOM, 2, _T("IOleWindow::ContextSensitiveHelp\n"));

    ATLTRACENOTIMPL("IOleWindow::ContextSensitiveHelp");
}

HRESULT STDMETHODCALLTYPE CRadKeyLockDeskBand::ShowDW(
    /* [in] */ BOOL fShow)
{
    ATLTRACE(ATL::atlTraceCOM, 2, _T("IDockingWindow::ShowDW\n"));

    if (m_wndRadKeyLock)
        m_wndRadKeyLock.ShowWindow(fShow ? SW_SHOW : SW_HIDE);

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CRadKeyLockDeskBand::CloseDW(
    /* [in] */ DWORD /*dwReserved*/)
{
    ATLTRACE(ATL::atlTraceCOM, 2, _T("IDockingWindow::CloseDW\n"));

    if (m_wndRadKeyLock)
    {
        m_wndRadKeyLock.ShowWindow(SW_HIDE);
        m_wndRadKeyLock.DestroyWindow();
    }

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CRadKeyLockDeskBand::ResizeBorderDW(
    /* [in] */ LPCRECT prcBorder,
    /* [in] */ IUnknown* punkToolbarSite,
    /* [in] */ BOOL /*fReserved*/)
{
    ATLTRACE(ATL::atlTraceCOM, 2, _T("IDockingWindow::ResizeBorderDW\n"));

    if (!m_wndRadKeyLock) return S_OK;

    ATL::CComQIPtr<IDockingWindowSite> spDockingWindowSite = punkToolbarSite;

    if (spDockingWindowSite)
    {
        BORDERWIDTHS bw = { 0 };
        bw.top = bw.bottom = ::GetSystemMetrics(SM_CYBORDER);
        bw.left = bw.right = ::GetSystemMetrics(SM_CXBORDER);

        HRESULT hr = spDockingWindowSite->RequestBorderSpaceDW(
            static_cast<IDeskBand*>(this), &bw);

        if (SUCCEEDED(hr))
        {
            HRESULT hr = spDockingWindowSite->SetBorderSpaceDW(
                static_cast<IDeskBand*>(this), &bw);

            if (SUCCEEDED(hr))
            {
                m_wndRadKeyLock.MoveWindow(prcBorder);
                return S_OK;
            }
        }
    }

    return E_FAIL;
}
