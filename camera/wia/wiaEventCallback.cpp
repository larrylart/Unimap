
// system headers
#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>
#include <objbase.h>
#include <atlbase.h>
#include <sti.h>
#include <gdiplus.h>

#include "../camera_wia.h"
#include "wiaEventCallback.h"

// CEventCallback::CEventCallback
//////////////////////////////////////////////////////////////////////////
CEventCallback::CEventCallback()
{
    m_cRef = 0;
    m_nNumDevices = 0;
}

// CEventCallback::QueryInterface
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CEventCallback::QueryInterface(REFIID iid, LPVOID *ppvObj)
{
    if (ppvObj == NULL)
    {
	    return E_POINTER;
    }

    if (iid == IID_IUnknown)
    {
	    *ppvObj = (IUnknown *) this;
    }
    else if (iid == IID_IWiaEventCallback)
    {
	    *ppvObj = (IWiaEventCallback *) this;
    }
    else
    {
        *ppvObj = NULL;
        return E_NOINTERFACE;
    }

	AddRef();
    return S_OK;
}

// CEventCallback::AddRef
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(ULONG) CEventCallback::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

// CEventCallback::Release
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(ULONG) CEventCallback::Release()
{
    LONG cRef = InterlockedDecrement(&m_cRef);

    if (cRef == 0)
    {
        delete this;
    }

    return cRef;
}

// CEventCallback::ImageEventCallback
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CEventCallback::ImageEventCallback(
    LPCGUID ,
    BSTR    ,
    BSTR    ,
    BSTR    ,
    DWORD   ,
    BSTR    ,
    ULONG*  ,
    ULONG   
)
{
	return CCameraWia::WiaGetNumDevices( NULL, &m_nNumDevices );
}

// CEventCallback::Register
//////////////////////////////////////////////////////////////////////////
HRESULT CEventCallback::Register()
{
    HRESULT hr;

    // Create a connection to the local WIA device manager

    CComPtr<IWiaDevMgr> pWiaDevMgr;

    hr = pWiaDevMgr.CoCreateInstance(CLSID_WiaDevMgr);

    if (FAILED(hr))
    {
        return hr;
    }

    // Register the callback interface
    hr = pWiaDevMgr->RegisterEventCallbackInterface(
        0,
        NULL,
        &WIA_EVENT_DEVICE_CONNECTED,
        this,
        &m_pConnectEventObject
    );

    if (FAILED(hr))
    {
        return hr;
    }

    hr = pWiaDevMgr->RegisterEventCallbackInterface(
        0,
        NULL,
        &WIA_EVENT_DEVICE_DISCONNECTED,
        this,
        &m_pDisconnectEventObject
    );

    if (FAILED(hr))
    {
        return hr;
    }

    // Get the current count of all the WIA devices on the system
    hr = CCameraWia::WiaGetNumDevices(pWiaDevMgr, &m_nNumDevices);

    if (FAILED(hr))
    {
        return hr;
    }

    return S_OK;
}

// CEventCallback::GetNumDevices
//////////////////////////////////////////////////////////////////////////
ULONG CEventCallback::GetNumDevices() const
{
    return m_nNumDevices;
}

