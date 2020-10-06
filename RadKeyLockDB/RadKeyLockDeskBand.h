#pragma once
#include "resource.h"       // main symbols

#include "RadKeyLockWnd.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// {67FA3799-6039-4017-BF51-50B205F34725}
extern const __declspec(selectany) CLSID CLSID_RadKeyLockDeskBand =
    { 0x67fa3799, 0x6039, 0x4017, { 0xbf, 0x51, 0x50, 0xb2, 0x5, 0xf3, 0x47, 0x25 } };

class CRadKeyLockDeskBand :
    public ATL::CComObjectRootEx<ATL::CComSingleThreadModel>,
    public ATL::CComCoClass<CRadKeyLockDeskBand, &CLSID_RadKeyLockDeskBand>,
    public ATL::IObjectWithSiteImpl<CRadKeyLockDeskBand>,
    public ATL::IPersistStreamInitImpl<CRadKeyLockDeskBand>,
    public IInputObject,
    public IContextMenu,
    public IDeskBand2
{
    typedef IPersistStreamInitImpl<CRadKeyLockDeskBand> IPersistStreamImpl;
public:
    CRadKeyLockDeskBand();

DECLARE_REGISTRY_RESOURCEID(IDR_RADKEYLOCKDB)

BEGIN_COM_MAP(CRadKeyLockDeskBand)
    COM_INTERFACE_ENTRY(IOleWindow)
    COM_INTERFACE_ENTRY(IDockingWindow)
    COM_INTERFACE_ENTRY(IDeskBand)
    COM_INTERFACE_ENTRY(IDeskBand2)
    COM_INTERFACE_ENTRY(IInputObject)
    COM_INTERFACE_ENTRY(IContextMenu)
    COM_INTERFACE_ENTRY(IObjectWithSite)
    COM_INTERFACE_ENTRY_IID(IID_IPersist, IPersistStreamImpl)
    COM_INTERFACE_ENTRY_IID(IID_IPersistStream, IPersistStreamImpl)
    COM_INTERFACE_ENTRY_IID(IID_IPersistStreamInit, IPersistStreamImpl)
END_COM_MAP()

BEGIN_CATEGORY_MAP(CRadKeyLockDeskBand)
    IMPLEMENTED_CATEGORY(CATID_DeskBand)
END_CATEGORY_MAP()

// IPersistStreamInitImpl requires property map.
BEGIN_PROP_MAP(CRadKeyLockDeskBand)
    // PROP_DATA_ENTRY("Value", m_Value, VT_UI4)
END_PROP_MAP()

public:
    // IObjectWithSite
    //
    STDMETHOD(SetSite)( 
        /* [in] */ IUnknown *pUnkSite);

    // IInputObject
    //
    STDMETHOD(UIActivateIO)( 
        /* [in] */ BOOL fActivate,
        /* [unique][in] */ MSG *pMsg);
        
    STDMETHOD(HasFocusIO)();

    STDMETHOD(TranslateAcceleratorIO)( 
        /* [in] */ MSG *pMsg);

    // IContextMenu
    //
    STDMETHOD(QueryContextMenu)(
        /* [in] */ HMENU hmenu,
        /* [in] */ UINT indexMenu,
        /* [in] */ UINT idCmdFirst,
        /* [in] */ UINT idCmdLast,
        /* [in] */ UINT uFlags);

    STDMETHOD(InvokeCommand)( 
        /* [in] */ CMINVOKECOMMANDINFO *pici);

    STDMETHOD(GetCommandString)( 
        /* [in] */ UINT_PTR idCmd,
        /* [in] */ UINT uType,
        /* [in] */ UINT *pReserved,
        /* [out] */ LPSTR pszName,
        /* [in] */ UINT cchMax);

    // IDeskBand
    //
    STDMETHOD(GetBandInfo)(
        /* [in] */ DWORD dwBandID,
        /* [in] */ DWORD dwViewMode,
        /* [out][in] */ DESKBANDINFO *pdbi);

    // IDeskBand2
    //
    STDMETHOD(CanRenderComposited)( 
        /* [out] */ BOOL *pfCanRenderComposited);
        
    STDMETHOD(SetCompositionState)( 
        /* [in] */ BOOL fCompositionEnabled);
        
    STDMETHOD(GetCompositionState)( 
        /* [out] */ BOOL *pfCompositionEnabled);
    
    // IOleWindow
    //
    STDMETHOD(GetWindow)( 
        /* [out] */ HWND *phwnd);

    STDMETHOD(ContextSensitiveHelp)( 
        /* [in] */ BOOL fEnterMode);

    // IDockingWindow
    //
    STDMETHOD(ShowDW)( 
        /* [in] */ BOOL fShow);

    STDMETHOD(CloseDW)( 
        /* [in] */ DWORD dwReserved);

    STDMETHOD(ResizeBorderDW)( 
        /* [in] */ LPCRECT prcBorder,
        /* [in] */ IUnknown *punkToolbarSite,
        /* [in] */ BOOL fReserved);

private:
    HRESULT UpdateDeskband();

public:
    bool m_bRequiresSave; // used by IPersistStreamInitImpl

private:
    int m_nBandID;
    DWORD m_dwViewMode;
    CRadKeyLockWnd m_wndRadKeyLock;
    BOOL m_bCompositionEnabled;
};

OBJECT_ENTRY_AUTO(CLSID_RadKeyLockDeskBand, CRadKeyLockDeskBand)
