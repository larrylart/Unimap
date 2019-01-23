
#ifndef _CAMERA_WIA_H
#define _CAMERA_WIA_H

// system includes
#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>
#include <objbase.h>
#include <atlbase.h>
#include <sti.h>
#include <vector>
#include <wia.h>

// wx
#include "wx/wxprec.h"

// Local headers
//#include "../../observer/observer_hardware.h"
//#include "../camera.h"


// defines from resources
#define IDS_STATUS_TRANSFER_FROM_DEVICE 5
#define IDS_STATUS_PROCESSING_DATA      6
#define IDS_STATUS_TRANSFER_TO_CLIENT   7
#define IDS_ERROR_GET_IMAGE_DLG         8
// from afx
#define COUNTOF(x) ( sizeof(x) / sizeof(*x) )
#define DEFAULT_STRING_SIZE 256

//MAX_GUID_STRING_LEN
#ifndef MAX_GUID_STRING_LEN
#define MAX_GUID_STRING_LEN 39
#endif 

// external classes
class CGdiplusInit;
class CCamera;
class CEventCallback;

static HINSTANCE g_hInstance;

////////////////////////////
// :: type defs
// The ProgressCallback function is an application-defined callback function
// used with the WiaGetImage function.
typedef HRESULT (CALLBACK *PFNPROGRESSCALLBACK)( LONG   lStatus, LONG   lPercentComplete, PVOID  pParam );

// class CCameraWia
//////////////////////////////////////////////////////////////////////////
class CCameraWia
{
// public methods
public:
	CCameraWia( CCamera* pCamera );
	~CCameraWia();

	void SetDownloadPath( const wxString& strPath ){ m_strDownloadPath = strPath; }
	int Connect();

	static HRESULT WiaGetDevices( IWiaDevMgr *pSuppliedWiaDevMgr, CCamera* pCamera, int bLookForCameras=1 );
	static HRESULT WiaGetDevices2( IWiaDevMgr2 *pSuppliedWiaDevMgr, CCamera* pCamera, int bLookForCameras=1 );
	static HRESULT DirectShowGetDevices( CCamera* pCamera, int bLookForCameras=1 );

	HRESULT WiaTakePicture( wxString strDevId, IWiaDevMgr* pSuppliedWiaDevMgr=NULL, const wxString& strPath=wxT("") );
	static HRESULT CALLBACK DefaultProgressCallback( LONG lStatus, LONG lPercentComplete, PVOID pParam );

// public data
public:
	CCamera* m_pCamera;
	int m_bConnected;

	CGdiplusInit* m_pGdiplusInit;

	// connection to the local WIA device manager
    CComPtr<IWiaDevMgr> m_pWiaDevMgr;

	// WIA
	CComPtr<CEventCallback>  m_pEventCallback;

	// other
	wxString m_strDownloadPath;
	

// public methods
public:

	// The WiaGetImage function displays one or more dialog boxes that enable a 
	// user to acquire multiple images from a WIA device and return them in an
	// array of IStream interfaces. This method combines the functionality of 
	// SelectDeviceDlg, DeviceDlg and idtGetBandedData API's to completely 
	// encapsulate image acquisition within a single function call.
	HRESULT WiaGetImage( HWND hWndParent, LONG lDeviceType, LONG lFlags, LONG lIntent,
						IWiaDevMgr* pSuppliedWiaDevMgr, IWiaItem* pSuppliedItemRoot,
						PFNPROGRESSCALLBACK pfnProgressCallback, PVOID pProgressCallbackParam,
						GUID* pguidFormat, LONG* plCount, IStream*** pppStream );


	// The WiaGetNumDevices function returns the number of WIA devices on
	// the system.
	static HRESULT WiaGetNumDevices( IWiaDevMgr* pSuppliedWiaDevMgr, ULONG* pulNumDevices );

	// The ReadPropertyLong function reads a long integer value from a WIA 
	// property storage
	HRESULT ReadPropertyLong( IWiaPropertyStorage* pWiaPropertyStorage, const PROPSPEC* pPropSpec, LONG* plResult );

	// The ReadPropertyGuid function reads a GUID value from a WIA property storage
	HRESULT ReadPropertyGuid( IWiaPropertyStorage* pWiaPropertyStorage, const PROPSPEC* pPropSpec, GUID* pguidResult );

};

//////////////////////////////////////////////////////////////////////////
// CComPtrArray stores an array of COM interface pointers and performs
// reference counting through AddRef and Release methods.  
// CComPtrArray can be used with WiaGetImage and DeviceDlg functions 
// to provide automatic deallocation of the output arrays.
template <class T>
class CComPtrArray
{
public:
    CComPtrArray()
    {
        m_pArray = NULL;
        m_nCount = 0;
    }

	// Initializes the array pointer and the count to zero.
    explicit CComPtrArray(int nCount)
    {
        m_pArray = (T **) CoTaskMemAlloc(nCount * sizeof(T *));

        m_nCount = m_pArray == NULL ? 0 : nCount;

        for (int i = 0; i < m_nCount; ++i) 
        {
            m_pArray[i] = NULL;
        }
    }

	//Allocates the array for with CoTaskMemAlloc for nCount items and 
   // initializes the interface pointers to NULL
    CComPtrArray(const CComPtrArray& rhs)
    {
        Copy(rhs);
    }

    ~CComPtrArray() 
    {
        Clear();
    }

    CComPtrArray &operator =(const CComPtrArray& rhs)
    {
        if (this != &rhs)
        {
            Clear();
            Copy(rhs);
        }

        return *this;
    }

    operator T **()
    {
        return m_pArray;
    }

    bool operator!()
    {
        return m_pArray == NULL;
    }

    T ***operator&()
    {
        return &m_pArray;
    }

    LONG &Count()
    {
        return m_nCount;
    }

	void Clear()
	{
        if (m_pArray != NULL) 
        {
            for (int i = 0; i < m_nCount; ++i) 
            {
                if (m_pArray[i] != NULL) 
                {
                    m_pArray[i]->Release();
                }
            }

            CoTaskMemFree(m_pArray);

            m_pArray = NULL;
            m_nCount = 0;
        }
	}

	// Allocates a new array with CoTaskMemAlloc, copies the interface 
	// pointers and call AddRef() on the copied pointers.
    void Copy(const CComPtrArray& rhs)
    {
        m_pArray = NULL;
        m_nCount = 0;

        if (rhs.m_pArray != NULL)
        {
            m_pArray = (T**) CoTaskMemAlloc(rhs.m_nCount * sizeof(T *));

            if (m_pArray != NULL)
            {
                m_nCount = rhs.m_nCount;

                for (int i = 0; i < m_nCount; ++i)
                {
                    m_pArray[i] = rhs.m_pArray[i];

                    if (m_pArray[i] != NULL)
                    {
                        m_pArray[i]->AddRef();
                    }
                }
            }
        }
    }

private:
    T    **m_pArray;
    LONG  m_nCount;
};


#endif 

