
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
#include <wia.h>
#include <Strmif.h>
#include <Dshow.h>

// WX includes
#include "wx/wxprec.h"
#include <wx/regex.h>

// Local headers
#include "../util/func.h"
#include "../observer/observer_hardware.h"
#include "camera.h"
#include "gdiplus_init.h"
//#include "ProgressDlg.h"
#include "wia/wiaEventCallback.h"
#include "wia/wiaDataCallback.h"

// Main headers
#include "camera_wia.h"

// name spaces to use
using namespace Gdiplus;

// Method to get an image class id by name
////////////////////////////////////////////////////////////////////
int GetEncoderClsid( const WCHAR* format, CLSID* pClsid )
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   ImageCodecInfo* pImageCodecInfo = NULL;

   GetImageEncodersSize(&num, &size);
   if(size == 0) return -1;  // Failure

   pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }    
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}

// Method to file extension by image class id
////////////////////////////////////////////////////////////////////
int GetEncoderFileExt( wxString& strExt, CLSID* pClsid, GUID nFormatID )
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes
   char strTmp[255];
   wxRegEx reExt( wxT("^\\*\\.([^;]+);") );

   ImageCodecInfo* pImageCodecInfo = NULL;

   GetImageEncodersSize(&num, &size);
   if(size == 0) return -1;  // Failure

   pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
		if( IsEqualGUID( pImageCodecInfo[j].FormatID, nFormatID ) )
		{
			wcstombs( strTmp, pImageCodecInfo[j].FilenameExtension, 255 );
			wxString strWxTmp(strTmp,wxConvUTF8);
			if( reExt.Matches( strWxTmp ) ) 
			{
				// Success
				*pClsid = pImageCodecInfo[j].Clsid;

				wxString strWxExt = reExt.GetMatch( strWxTmp, 1 ).Trim(0).Trim(1).Lower();
				strExt = strWxExt;

				free(pImageCodecInfo);
				return( j );
			}
		}    
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}

// class:	WiaWrap	
//////////////////////////////////////////////////////////////////////////
CCameraWia::CCameraWia( CCamera* pCamera )
{
	m_pCamera = pCamera;
	m_bConnected = 0;
}

//////////////////////////////////////////////////////////////////////////
CCameraWia::~CCameraWia( )
{
	if( m_pEventCallback ) delete( m_pEventCallback );
	if( m_pWiaDevMgr ){ m_pWiaDevMgr.Release(); m_pWiaDevMgr=NULL; }
	if( m_pGdiplusInit ) delete( m_pGdiplusInit );
}

//////////////////////////////////////////////////////////////////////////
int CCameraWia::Connect( )
{
	if( m_bConnected ) return( 1 );

	HRESULT hr=0;

	m_pGdiplusInit = new CGdiplusInit();

	m_pEventCallback = new CEventCallback();
	if (m_pEventCallback != NULL) m_pEventCallback->Register();

	m_pWiaDevMgr.CoCreateInstance(CLSID_WiaDevMgr);
	// check if ok
	if( FAILED(hr) ) 
		m_bConnected = 0;
	else 
		m_bConnected = 1;

	return( 1 );
}

// ReadPropertyLong
//////////////////////////////////////////////////////////////////////////
HRESULT CCameraWia::ReadPropertyLong( IWiaPropertyStorage* pWiaPropertyStorage, const PROPSPEC* pPropSpec, LONG* plResult )
{
    PROPVARIANT PropVariant;

    HRESULT hr = pWiaPropertyStorage->ReadMultiple( 1, pPropSpec, &PropVariant );

    // Generally, the return value should be checked against S_FALSE.
    // If ReadMultiple returns S_FALSE, it means the property name or ID
    // had valid syntax, but it didn't exist in this property set, so
    // no properties were retrieved, and each PROPVARIANT structure is set 
    // to VT_EMPTY. But the following switch statement will handle this case
    // and return E_FAIL. So the caller of ReadPropertyLong does not need
    // to check for S_FALSE explicitly.
    if( SUCCEEDED(hr) )
    {
        switch (PropVariant.vt)
        {
            case VT_I1:
            {
                *plResult = (LONG) PropVariant.cVal;
                hr = S_OK;
                break;
            }

            case VT_UI1:
            {
                *plResult = (LONG) PropVariant.bVal;
                hr = S_OK;
                break;
            }

            case VT_I2:
            {
                *plResult = (LONG) PropVariant.iVal;
                hr = S_OK;
                break;
            }

            case VT_UI2:
            {
                *plResult = (LONG) PropVariant.uiVal;
                hr = S_OK;
                break;
            }

            case VT_I4:
            {
                *plResult = (LONG) PropVariant.lVal;
                hr = S_OK;
                break;
            }

            case VT_UI4:
            {
                *plResult = (LONG) PropVariant.ulVal;
                hr = S_OK;
                break;
            }

            case VT_INT:
            {
                *plResult = (LONG) PropVariant.intVal;
                hr = S_OK;
                break;
            }

            case VT_UINT:
            {
                *plResult = (LONG) PropVariant.uintVal;
                hr = S_OK;
                break;
            }

            case VT_R4:
            {
                *plResult = (LONG) (PropVariant.fltVal + 0.5);
                hr = S_OK;
                break;
            }

            case VT_R8:
            {
                *plResult = (LONG) (PropVariant.dblVal + 0.5);
                hr = S_OK;
                break;
            }

            default:
            {
                hr = E_FAIL;
                break;
            }
        }
    }

    PropVariantClear(&PropVariant);

    return( hr );
}

// ReadPropertyGuid
//////////////////////////////////////////////////////////////////////////
HRESULT CCameraWia::ReadPropertyGuid( IWiaPropertyStorage* pWiaPropertyStorage, const PROPSPEC* pPropSpec, GUID* pguidResult )
{
    PROPVARIANT PropVariant;

    HRESULT hr = pWiaPropertyStorage->ReadMultiple( 1, pPropSpec, &PropVariant );

    // Generally, the return value should be checked against S_FALSE.
    // If ReadMultiple returns S_FALSE, it means the property name or ID
    // had valid syntax, but it didn't exist in this property set, so
    // no properties were retrieved, and each PROPVARIANT structure is set 
    // to VT_EMPTY. But the following switch statement will handle this case
    // and return E_FAIL. So the caller of ReadPropertyGuid does not need
    // to check for S_FALSE explicitly.
    if (SUCCEEDED(hr))
    {
        switch (PropVariant.vt)
        {
            case VT_CLSID:
            {
                *pguidResult = *PropVariant.puuid; 
                hr = S_OK;
                break;
            }

            case VT_BSTR:
            {
                hr = CLSIDFromString(PropVariant.bstrVal, pguidResult);
                break;
            }

            case VT_LPWSTR:
            {
                hr = CLSIDFromString(PropVariant.pwszVal, pguidResult);
                break;
            }

            case VT_LPSTR:
            {
                WCHAR wszGuid[MAX_GUID_STRING_LEN];
				mbstowcs_s(NULL, wszGuid, MAX_GUID_STRING_LEN, PropVariant.pszVal, MAX_GUID_STRING_LEN);

                wszGuid[MAX_GUID_STRING_LEN - 1] = L'\0';
                hr = CLSIDFromString(wszGuid, pguidResult);
                break;
            }

            default:
            {
                hr = E_FAIL;
                break;
            }
        }
    }

    PropVariantClear( &PropVariant );

    return hr;
}

// DefaultProgressCallback
//////////////////////////////////////////////////////////////////////////
HRESULT CALLBACK CCameraWia::DefaultProgressCallback( LONG lStatus, LONG lPercentComplete, PVOID  pParam )
{
/*	larry ... here implement my own call back
	CProgressDlg *pProgressDlg = (CProgressDlg *) pParam;
    if( pProgressDlg == NULL ) return( E_POINTER );
    // If the user has pressed the cancel button, abort transfer
    if (pProgressDlg->Cancelled()) return( S_FALSE );
*/

    // Form the message text
//	g_hInstance = 0;
	UINT uID;

    switch (lStatus)
    {
        case IT_STATUS_TRANSFER_FROM_DEVICE:
		{
			uID = IDS_STATUS_TRANSFER_FROM_DEVICE;
            break;
		}

        case IT_STATUS_PROCESSING_DATA:
		{
            uID = IDS_STATUS_PROCESSING_DATA;
            break;
		}

        case IT_STATUS_TRANSFER_TO_CLIENT:
		{
            uID = IDS_STATUS_TRANSFER_TO_CLIENT;
            break;
		}

		default:
			return( E_INVALIDARG );
    }		

    TCHAR szFormat[DEFAULT_STRING_SIZE] = _T("%d");
//    LoadString(g_hInstance, uID, szFormat, COUNTOF(szFormat));

    TCHAR szStatusText[DEFAULT_STRING_SIZE];
    _sntprintf_s(szStatusText, COUNTOF(szStatusText), COUNTOF(szStatusText) - 1, szFormat, lPercentComplete);
    szStatusText[COUNTOF(szStatusText) - 1] = _T('\0');

	// Update the progress bar values
//    pProgressDlg->SetMessage(szStatusText);
//    pProgressDlg->SetPercent(lPercentComplete);

    return( S_OK );
}

// WiaGetNumDevices
//////////////////////////////////////////////////////////////////////////
HRESULT CCameraWia::WiaGetNumDevices( IWiaDevMgr *pSuppliedWiaDevMgr, ULONG *pulNumDevices )
{
    HRESULT hr;

    // Validate and initialize output parameters
    if (pulNumDevices == NULL) return( E_POINTER );

    *pulNumDevices = 0;

    // Create a connection to the local WIA device manager
    CComPtr<IWiaDevMgr> pWiaDevMgr = pSuppliedWiaDevMgr;

    if( pWiaDevMgr == NULL )
    {
        hr = pWiaDevMgr.CoCreateInstance(CLSID_WiaDevMgr);
		// check if ok
        if( FAILED(hr) ) return( hr );
    }

    // Get a list of all the WIA devices on the system
    CComPtr<IEnumWIA_DEV_INFO> pIEnumWIA_DEV_INFO;
	// use : WIA_DEVINFO_ENUM_ALL
    hr = pWiaDevMgr->EnumDeviceInfo( 0, &pIEnumWIA_DEV_INFO );
	// check result
    if( FAILED(hr) ) return( hr );

    // Get the number of WIA devices
    ULONG celt;
    hr = pIEnumWIA_DEV_INFO->GetCount(&celt);

	// check result
    if( FAILED(hr) ) return hr;

	// set number of devices
    *pulNumDevices = celt;

	pWiaDevMgr.Release();

    return( S_OK );

}

// WiaGetDevices
//////////////////////////////////////////////////////////////////////////
HRESULT CCameraWia::DirectShowGetDevices( CCamera* pCamera, int bLookForCameras )
{
    HRESULT hr;

    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
 
    // enumerate all video capture devices
     ICreateDevEnum *pCreateDevEnum=0;
     hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
                           IID_ICreateDevEnum, (void**)&pCreateDevEnum);
 
	if( hr != NOERROR )
	{
		CoUninitialize();
		return(hr);
	}
 
	IEnumMoniker *pEm=0;
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEm, 0);
	if( hr != NOERROR )
	{
		CoUninitialize();
		return(hr);
	}
 
	pEm->Reset();
	ULONG cFetched;
	IMoniker *pM;
 
	while( hr = pEm->Next(1, &pM, &cFetched), hr==S_OK )
	{
		IPropertyBag *pBag=0;
 
		hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if( SUCCEEDED(hr) )
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if(hr == NOERROR)
			{
				wxString FriendlyName=wxT("");
				wxString DeviceId=wxT("");
				wxString DeviceVendor=wxT("");
				FriendlyName = W2T(var.bstrVal);

				hr = pBag->Read(L"DevicePath", &var, 0); 
				if (SUCCEEDED(hr)) DeviceId = W2T(var.bstrVal);
 
				/////////////////////////////
				// add device
				int nWiaType=3;
				int bConnected=1;
				int bCanTakePicture=1;
				pCamera->AddHWDevice( DeviceId, FriendlyName, DeviceVendor, nWiaType, bConnected, bCanTakePicture );

				SysFreeString(var.bstrVal);
 
				// Add the Device's DevicePath to the NEW Video Device Path ComboBox!
				hr = pBag->Read(L"DevicePath", &var, 0); 

				if (SUCCEEDED(hr)) 
				{ 
					// The device path is not intended for display. 
					//CString FriendlyPath = W2T(var.bstrVal);
					//pComboRApath->AddString(FriendlyPath); // W2T(var.bstrVal));
				}
			} 

			SysFreeString(var.bstrVal);
 
			//ASSERT(gcap.rgpmVideoMenu[uIndex] == 0);
			//gcap.rgpmVideoMenu[uIndex] = pM;
			//pM->AddRef();
		}
		pBag->Release();

	}
 
	pM->Release();
	//uIndex++;
 
    CoUninitialize();
 
	return(hr);
}

// WiaGetDevices
//////////////////////////////////////////////////////////////////////////
HRESULT CCameraWia::WiaGetDevices( IWiaDevMgr *pSuppliedWiaDevMgr, CCamera* pCamera, int bLookForCameras )
{
    HRESULT hr;

    // Create a connection to the local WIA device manager
    CComPtr<IWiaDevMgr> pWiaDevMgr = pSuppliedWiaDevMgr;

	// check if we have a dev manager already else create
    if( pWiaDevMgr == NULL )
    {
        hr = pWiaDevMgr.CoCreateInstance(CLSID_WiaDevMgr);
		// check if ok
        if( FAILED(hr) ) return( hr );
    }

    // Get a list of all the WIA devices on the system
    CComPtr<IEnumWIA_DEV_INFO> pIEnumWIA_DEV_INFO;
	// use : WIA_DEVINFO_ENUM_ALL
    hr = pWiaDevMgr->EnumDeviceInfo( WIA_DEVINFO_ENUM_ALL, &pIEnumWIA_DEV_INFO );
	// check result
    if( FAILED(hr) ) return( hr );

    // Get the number of WIA devices
    ULONG celt;
    hr = pIEnumWIA_DEV_INFO->GetCount( &celt );
	if( celt <= 0 ) return( 0 );

	/////////////////
	// prepare to investigate infos
	IWiaPropertyStorage* pProp;
	ULONG ulFetched = NULL;  

	PROPSPEC PropSpec[8];   
	PROPVARIANT PropVar[8];   
   
	memset(PropVar, 0, sizeof(PropVar));   

	PropSpec[0].ulKind = PRSPEC_PROPID;   
	PropSpec[0].propid = WIA_DIP_DEV_ID;   
	PropSpec[1].ulKind = PRSPEC_PROPID;   
	PropSpec[1].propid = WIA_DIP_DEV_TYPE;   
	PropSpec[2].ulKind = PRSPEC_PROPID;   
	PropSpec[2].propid = WIA_DPV_IMAGES_DIRECTORY;   
	PropSpec[3].ulKind = PRSPEC_PROPID;   
	PropSpec[3].propid = WIA_DIP_DEV_NAME;   
	PropSpec[4].ulKind = PRSPEC_PROPID;   
	PropSpec[4].propid = WIA_DIP_DEV_DESC;   
	PropSpec[5].ulKind = PRSPEC_PROPID;   
	PropSpec[5].propid = WIA_DIP_VEND_DESC; 
	PropSpec[6].ulKind = PRSPEC_PROPID;   
	PropSpec[6].propid = WIA_DIP_STI_GEN_CAPABILITIES; 
	PropSpec[7].ulKind = PRSPEC_PROPID;   
	PropSpec[7].propid = WIA_DPA_CONNECT_STATUS; 

	long nDevType = 0;
	while( pIEnumWIA_DEV_INFO->Next( 1, &pProp, &ulFetched) == S_OK )
	{
		// check if end
		if( !ulFetched ) break;

		memset(PropVar, 0, sizeof(PropVar));
		hr = pProp->ReadMultiple(sizeof(PropSpec)/sizeof(PropSpec[0]), PropSpec, PropVar);   

		// for the name use PropVar[1].BSTR - uniq name - the other is port specific ?? or maybe

		// we need first to exclude type scanner - known on all windows from 2000
		nDevType = GET_STIDEVICE_TYPE( PropVar[1].lVal );

		// types StiDeviceTypeDefault=0, StiDeviceTypeScanner=1, StiDeviceTypeDigitalCamera=2, StiDeviceTypeStreamingVideo=3
		if( nDevType == StiDeviceTypeScanner && bLookForCameras ) continue;
		int nWiaType = nDevType;

		//////////////
		// here we get device capabilities
		// create device from devid string
		IWiaItem *ppWiaItemRoot;
		int bCanTakePicture=0;
		int bConnected=0;
		hr = pWiaDevMgr->CreateDevice( PropVar[0].bstrVal, &ppWiaItemRoot );
		if( SUCCEEDED(hr) )
		{
			bConnected = 1;
			// get list of capabilities - could | WIA_DEVICE_EVENTS
			IEnumWIA_DEV_CAPS* ppIEnumWIA_DEV_CAPS;
			ppWiaItemRoot->EnumDeviceCapabilities( WIA_DEVICE_COMMANDS, &ppIEnumWIA_DEV_CAPS );
			// process capabilities - guid=WIA_CMD_TAKE_PICTURE  bstrCommandline, ulFlags=WIA_ACTION_EVENT
			WIA_DEV_CAP pCaps;
			while( ppIEnumWIA_DEV_CAPS->Next( 1, &pCaps, &ulFetched) == S_OK )
			{
				if( pCaps.guid == WIA_CMD_TAKE_PICTURE )
				{
					bCanTakePicture=1;
					break;
				}
			}
			// delete here
			ppWiaItemRoot->Release();
			ppIEnumWIA_DEV_CAPS->Release();
		}

		/////////////////////////////
		// add device
		pCamera->AddHWDevice( PropVar[0].bstrVal, PropVar[3].bstrVal, PropVar[5].bstrVal, nWiaType, bConnected, bCanTakePicture );
	}

	PropVariantClear( PropVar );
//	pIEnumWIA_DEV_INFO->Release();
	pWiaDevMgr.Release();

	return( hr );
}

// WiaGetDevices2
//////////////////////////////////////////////////////////////////////////
HRESULT CCameraWia::WiaGetDevices2( IWiaDevMgr2 *pSuppliedWiaDevMgr, CCamera* pCamera, int bLookForCameras )
{
    HRESULT hr;

    // Create a connection to the local WIA device manager
    CComPtr<IWiaDevMgr2> pWiaDevMgr = pSuppliedWiaDevMgr;

	// check if we have a dev manager already else create
    if( pWiaDevMgr == NULL )
    {
        hr = pWiaDevMgr.CoCreateInstance(CLSID_WiaDevMgr2, NULL, CLSCTX_LOCAL_SERVER );
		// check if ok
        if( FAILED(hr) ) return( hr );
    }

    // Get a list of all the WIA devices on the system
    CComPtr<IEnumWIA_DEV_INFO> pIEnumWIA_DEV_INFO;
	// use : WIA_DEVINFO_ENUM_ALL
    hr = pWiaDevMgr->EnumDeviceInfo( WIA_DEVINFO_ENUM_ALL, &pIEnumWIA_DEV_INFO );
	// check result
    if( FAILED(hr) ) return( hr );

    // Get the number of WIA devices
    ULONG celt;
    hr = pIEnumWIA_DEV_INFO->GetCount( &celt );
	if( celt <= 0 ) return( 0 );

	/////////////////
	// prepare to investigate infos
	IWiaPropertyStorage* pProp;
	ULONG ulFetched = NULL;  

	PROPSPEC PropSpec[8];   
	PROPVARIANT PropVar[8];   
   
	memset(PropVar, 0, sizeof(PropVar));   

	PropSpec[0].ulKind = PRSPEC_PROPID;   
	PropSpec[0].propid = WIA_DIP_DEV_ID;   
	PropSpec[1].ulKind = PRSPEC_PROPID;   
	PropSpec[1].propid = WIA_DIP_DEV_TYPE;   
	PropSpec[2].ulKind = PRSPEC_PROPID;   
	PropSpec[2].propid = WIA_DPV_IMAGES_DIRECTORY;   
	PropSpec[3].ulKind = PRSPEC_PROPID;   
	PropSpec[3].propid = WIA_DIP_DEV_NAME;   
	PropSpec[4].ulKind = PRSPEC_PROPID;   
	PropSpec[4].propid = WIA_DIP_DEV_DESC;   
	PropSpec[5].ulKind = PRSPEC_PROPID;   
	PropSpec[5].propid = WIA_DIP_VEND_DESC; 
	PropSpec[6].ulKind = PRSPEC_PROPID;   
	PropSpec[6].propid = WIA_DIP_STI_GEN_CAPABILITIES; 
	PropSpec[7].ulKind = PRSPEC_PROPID;   
	PropSpec[7].propid = WIA_DPA_CONNECT_STATUS; 

	long nDevType = 0;
	while( pIEnumWIA_DEV_INFO->Next( 1, &pProp, &ulFetched) == S_OK )
	{
		// check if end
		if( !ulFetched ) break;

		memset(PropVar, 0, sizeof(PropVar));
		hr = pProp->ReadMultiple(sizeof(PropSpec)/sizeof(PropSpec[0]), PropSpec, PropVar);   

		// for the name use PropVar[1].BSTR - uniq name - the other is port specific ?? or maybe

		// we need first to exclude type scanner - known on all windows from 2000
		nDevType = GET_STIDEVICE_TYPE( PropVar[1].lVal );

		// types StiDeviceTypeDefault=0, StiDeviceTypeScanner=1, StiDeviceTypeDigitalCamera=2, StiDeviceTypeStreamingVideo=3
		if( nDevType == StiDeviceTypeScanner && bLookForCameras ) continue;
		int nWiaType = nDevType;

		//////////////
		// here we get device capabilities
		// create device from devid string
		IWiaItem2 *ppWiaItemRoot;
		int bCanTakePicture=0;
		int bConnected=0;
		hr = pWiaDevMgr->CreateDevice( 0, PropVar[0].bstrVal, &ppWiaItemRoot );
		if( SUCCEEDED(hr) )
		{
			bConnected = 1;
			// get list of capabilities - could | WIA_DEVICE_EVENTS
			IEnumWIA_DEV_CAPS* ppIEnumWIA_DEV_CAPS;
			ppWiaItemRoot->EnumDeviceCapabilities( WIA_DEVICE_COMMANDS, &ppIEnumWIA_DEV_CAPS );
			// process capabilities - guid=WIA_CMD_TAKE_PICTURE  bstrCommandline, ulFlags=WIA_ACTION_EVENT
			WIA_DEV_CAP pCaps;
			while( ppIEnumWIA_DEV_CAPS->Next( 1, &pCaps, &ulFetched) == S_OK )
			{
				if( pCaps.guid == WIA_CMD_TAKE_PICTURE )
				{
					bCanTakePicture=1;
					break;
				}
			}
			// delete here
			ppWiaItemRoot->Release();
			ppIEnumWIA_DEV_CAPS->Release();
		}

		/////////////////////////////
		// add device
		pCamera->AddHWDevice( PropVar[0].bstrVal, PropVar[3].bstrVal, PropVar[5].bstrVal, nWiaType, bConnected, bCanTakePicture );
	}

	PropVariantClear( PropVar );
//	pIEnumWIA_DEV_INFO->Release();
	pWiaDevMgr.Release();

	return( hr );
}

// WiaTakePicyure
//////////////////////////////////////////////////////////////////////////
HRESULT CCameraWia::WiaTakePicture( wxString strDevId, IWiaDevMgr *pSuppliedWiaDevMgr, const wxString& strPath )
{
	// check if connected
	if( !m_bConnected ) return( 0 );
	// check if specific path
	if( !strPath.IsEmpty() ) m_strDownloadPath = strPath;


    HRESULT hr;
	CComBSTR bstrDevId( strDevId );

    // Create a connection to the local WIA device manager
    CComPtr<IWiaDevMgr> pWiaDevMgr = pSuppliedWiaDevMgr;

	// check if we have a dev manager already else create
	if( m_pWiaDevMgr && pWiaDevMgr == NULL )
	{
		pWiaDevMgr = m_pWiaDevMgr;

	} if( pWiaDevMgr == NULL )
    {
        hr = pWiaDevMgr.CoCreateInstance(CLSID_WiaDevMgr);
		// check if ok
        if( FAILED(hr) ) return( hr );
    }

	// create device from devid string
	IWiaItem *ppWiaItemRoot;

	hr = pWiaDevMgr->CreateDevice( bstrDevId, &ppWiaItemRoot );
	if( FAILED(hr) ) return( hr );

	IWiaItem *ppIWiaItem = NULL; 
	ppWiaItemRoot->DeviceCommand( 0, &WIA_CMD_TAKE_PICTURE, &ppIWiaItem ); 

	// create stream for of pictures
	CComPtrArray<IStream> ppStream;
	// Create the data callback interface
//	CComPtr<CProgressDlg> pProgressDlg;

	PFNPROGRESSCALLBACK  pfnProgressCallback;
	PVOID pProgressCallbackParam;
	pfnProgressCallback = &CCameraWia::DefaultProgressCallback;

	// larry :: here insert dlg
	//pProgressDlg = new CProgressDlg( NULL );
	pProgressCallbackParam = NULL; //(CProgressDlg *) pProgressDlg;

	// Create the data callback interface
	CComPtr<CDataCallback> pDataCallback = new CDataCallback( pfnProgressCallback, pProgressCallbackParam,
																&(ppStream.Count()), &ppStream );

	if( pDataCallback == NULL ) return( E_OUTOFMEMORY );

	////////////////////////
	// Start the transfer of the selected items
	// Get the interface pointers
	CComQIPtr<IWiaPropertyStorage> pWiaPropertyStorage(ppIWiaItem);
	if( pWiaPropertyStorage == NULL ) return( E_NOINTERFACE );

	CComQIPtr<IWiaDataTransfer> pIWiaDataTransfer(ppIWiaItem);
	if( pIWiaDataTransfer == NULL ) return E_NOINTERFACE;

	/////////////
	// Set the transfer type
	PROPSPEC specTymed;
	specTymed.ulKind = PRSPEC_PROPID;
	specTymed.propid = WIA_IPA_TYMED;

	PROPVARIANT varTymed;
	varTymed.vt = VT_I4;
	varTymed.lVal = TYMED_CALLBACK;

	hr = pWiaPropertyStorage->WriteMultiple( 1, &specTymed, &varTymed, WIA_IPA_FIRST );
	PropVariantClear( &varTymed );
	if( FAILED(hr) ) return( hr );

	// If there is no transfer format specified, use the device default
	GUID guidFormat = GUID_NULL;
	GUID* pguidFormat = NULL;

	if( pguidFormat == NULL ) pguidFormat = &guidFormat;

	if( *pguidFormat == GUID_NULL )
	{
		PROPSPEC specPreferredFormat;
		specPreferredFormat.ulKind = PRSPEC_PROPID;
		specPreferredFormat.propid = WIA_IPA_PREFERRED_FORMAT;

		hr = ReadPropertyGuid( pWiaPropertyStorage, &specPreferredFormat, pguidFormat );
		if( FAILED(hr) ) return( hr );
	}

	///////////////////
	// Set the transfer format
	PROPSPEC specFormat;
	specFormat.ulKind = PRSPEC_PROPID;
	specFormat.propid = WIA_IPA_FORMAT;

	PROPVARIANT varFormat;
	varFormat.vt = VT_CLSID;
	varFormat.puuid = (CLSID *) CoTaskMemAlloc(sizeof(CLSID));
	if( varFormat.puuid == NULL ) return( E_OUTOFMEMORY );

	*varFormat.puuid = *pguidFormat;
	hr = pWiaPropertyStorage->WriteMultiple( 1, &specFormat, &varFormat, WIA_IPA_FIRST );
	PropVariantClear( &varFormat );
	if( FAILED(hr) ) return ( hr );

	//////////////////////////////////
	// Read the transfer buffer size from the device, default to 64K
	LONG nBufferSize;
	PROPSPEC specBufferSize;
	specBufferSize.ulKind = PRSPEC_PROPID;
	specBufferSize.propid = WIA_IPA_BUFFER_SIZE;

	hr = ReadPropertyLong( pWiaPropertyStorage, &specBufferSize, &nBufferSize );
	// if failed increase buffer size
	if( FAILED(hr) ) nBufferSize = 64 * 1024;

	// Choose double buffered transfer for better performance
	WIA_DATA_TRANSFER_INFO WiaDataTransferInfo = { 0 };

	WiaDataTransferInfo.ulSize        = sizeof(WIA_DATA_TRANSFER_INFO);
	WiaDataTransferInfo.ulBufferSize  = 2 * nBufferSize;
	WiaDataTransferInfo.bDoubleBuffer = TRUE;

	// Start the transfer
	hr = pIWiaDataTransfer->idtGetBandedData( &WiaDataTransferInfo, pDataCallback );

	if( FAILED(hr) || hr == S_FALSE ) return( hr );

	// image enconder
	CLSID m_imgClsid;
	wxString strFilePath;
	wxString strFileExt;
	//////////////////////////
    // SAVE IMAGES FROM MY STREAM
    for( int i = 0; i < ppStream.Count(); ++i )
    {
		Gdiplus::Image m_Image(ppStream[i]);

		if( GetEncoderFileExt( strFileExt, &m_imgClsid, *pguidFormat ) == -1 )
		{
			GetEncoderClsid( L"image/jpeg", &m_imgClsid );
			strFilePath.Printf( wxT("%s%d.jpg"), m_strDownloadPath, GetUniqTimeId( ) );

		} else
		{
			strFilePath.Printf( wxT("%s%d.%s"), m_strDownloadPath, GetUniqTimeId( ), strFileExt );
		}

		//WCHAR strImg[500];
		//mbstowcs( strImg, strFilePath, 500 );

		//m_Image.Save( strImg, &m_pngClsid );
		m_Image.Save( strFilePath, &m_imgClsid );

		// tell camera that there is a new frame
		m_pCamera->AddImageFrame( strFilePath );
    }

	// check overall  result
    if( FAILED(hr) ) return hr;

	// release 
//	delete( ppStream );
	ppIWiaItem->Release();
	pDataCallback.Release();
	ppWiaItemRoot->Release();
	if( pWiaDevMgr != m_pWiaDevMgr ) pWiaDevMgr.Release();

    return S_OK;
}

//
// WiaGetImage
//////////////////////////////////////////////////////////////////////////
HRESULT CCameraWia::WiaGetImage(
    HWND                 hWndParent,
    LONG                 lDeviceType,
    LONG                 lFlags,
    LONG                 lIntent,
    IWiaDevMgr          *pSuppliedWiaDevMgr,
    IWiaItem            *pSuppliedItemRoot,
    PFNPROGRESSCALLBACK  pfnProgressCallback,
    PVOID                pProgressCallbackParam,
    GUID                *pguidFormat,
    LONG                *plCount,
    IStream             ***pppStream
)
{
    HRESULT hr;

    // Validate and initialize output parameters

    if (plCount == NULL)
    {
        return E_POINTER;
    }

    if (pppStream == NULL)
    {
        return E_POINTER;
    }

    *plCount = 0;
    *pppStream = NULL;

    // Initialize the local root item variable with the supplied value.
    // If no value is supplied, display the device selection common dialog.

    CComPtr<IWiaItem> pItemRoot = pSuppliedItemRoot;

    if (pItemRoot == NULL)
    {
        // Initialize the device manager pointer with the supplied value
        // If no value is supplied, connect to the local device manager

        CComPtr<IWiaDevMgr> pWiaDevMgr = pSuppliedWiaDevMgr;

        if (pWiaDevMgr == NULL)
        {
            hr = pWiaDevMgr.CoCreateInstance(CLSID_WiaDevMgr);

            if (FAILED(hr))
            {
                return hr;
            }
        }
    
        // Display the device selection common dialog

        hr = pWiaDevMgr->SelectDeviceDlg(
            hWndParent,
            lDeviceType,
            lFlags,
            0,
            &pItemRoot
        );

        if (FAILED(hr) || hr == S_FALSE)
        {
            return hr;
        }
    }

    // Display the image selection common dialog 

    CComPtrArray<IWiaItem> ppIWiaItem;

    hr = pItemRoot->DeviceDlg(
        hWndParent,
        lFlags,
        lIntent,
        &ppIWiaItem.Count(),
        &ppIWiaItem
    );

    if (FAILED(hr) || hr == S_FALSE)
    {
        return hr;
    }

    // For ADF scanners, the common dialog explicitly sets the page count to one.
    // So in order to transfer multiple images, set the page count to ALL_PAGES
    // if the WIA_DEVICE_DIALOG_SINGLE_IMAGE flag is not specified, 

    if (!(lFlags & WIA_DEVICE_DIALOG_SINGLE_IMAGE))
    {
        // Get the property storage interface pointer for the root item

        CComQIPtr<IWiaPropertyStorage> pWiaRootPropertyStorage(pItemRoot);

        if (pWiaRootPropertyStorage == NULL)
        {
            return E_NOINTERFACE;
        }

        // Determine if the selected device is a scanner or not

        PROPSPEC specDevType;

        specDevType.ulKind = PRSPEC_PROPID;
        specDevType.propid = WIA_DIP_DEV_TYPE;

        LONG nDevType;

        hr = ReadPropertyLong(
            pWiaRootPropertyStorage, 
            &specDevType, 
            &nDevType
        );

        if (SUCCEEDED(hr) && (GET_STIDEVICE_TYPE(nDevType) == StiDeviceTypeScanner))
        {
            // Determine if the document feeder is selected or not

            PROPSPEC specDocumentHandlingSelect;

            specDocumentHandlingSelect.ulKind = PRSPEC_PROPID;
            specDocumentHandlingSelect.propid = WIA_DPS_DOCUMENT_HANDLING_SELECT;

            LONG nDocumentHandlingSelect;

            hr = ReadPropertyLong(
                pWiaRootPropertyStorage, 
                &specDocumentHandlingSelect, 
                &nDocumentHandlingSelect
            );

            if (SUCCEEDED(hr) && (nDocumentHandlingSelect & FEEDER))
            {
                PROPSPEC specPages;

                specPages.ulKind = PRSPEC_PROPID;
                specPages.propid = WIA_DPS_PAGES;

                PROPVARIANT varPages;
                    
                varPages.vt = VT_I4;
                varPages.lVal = ALL_PAGES;

                pWiaRootPropertyStorage->WriteMultiple(
                    1,
                    &specPages,
                    &varPages,
                    WIA_DPS_FIRST
                );
                
                PropVariantClear(&varPages);
            }
        }
    }

    // If a status callback function is not supplied, use the default.
    // The default displays a simple dialog with a progress bar and cancel button.

//    CComPtr<CProgressDlg> pProgressDlg;

    if (pfnProgressCallback == NULL)
    {
        pfnProgressCallback = DefaultProgressCallback;

 //       pProgressDlg = new CProgressDlg(hWndParent);

        pProgressCallbackParam = NULL; //(CProgressDlg *) pProgressDlg;
    }

    // Create the data callback interface

    CComPtr<CDataCallback> pDataCallback = new CDataCallback(
        pfnProgressCallback,
        pProgressCallbackParam,
        plCount, 
        pppStream
    );

    if (pDataCallback == NULL)
    {
        return E_OUTOFMEMORY;
    }

    // Start the transfer of the selected items

    for (int i = 0; i < ppIWiaItem.Count(); ++i)
    {
        // Get the interface pointers

        CComQIPtr<IWiaPropertyStorage> pWiaPropertyStorage(ppIWiaItem[i]);

        if (pWiaPropertyStorage == NULL)
        {
            return E_NOINTERFACE;
        }

        CComQIPtr<IWiaDataTransfer> pIWiaDataTransfer(ppIWiaItem[i]);

        if (pIWiaDataTransfer == NULL)
        {
            return E_NOINTERFACE;
        }

        // Set the transfer type

        PROPSPEC specTymed;

        specTymed.ulKind = PRSPEC_PROPID;
        specTymed.propid = WIA_IPA_TYMED;

        PROPVARIANT varTymed;

        varTymed.vt = VT_I4;
        varTymed.lVal = TYMED_CALLBACK;

        hr = pWiaPropertyStorage->WriteMultiple(
            1,
            &specTymed,
            &varTymed,
            WIA_IPA_FIRST
        );

        PropVariantClear(&varTymed);

        if (FAILED(hr))
        {
            return hr;
        }

        // If there is no transfer format specified, use the device default

        GUID guidFormat = GUID_NULL;

        if (pguidFormat == NULL)
        {
            pguidFormat = &guidFormat;
        }

        if (*pguidFormat == GUID_NULL)
        {
            PROPSPEC specPreferredFormat;

            specPreferredFormat.ulKind = PRSPEC_PROPID;
            specPreferredFormat.propid = WIA_IPA_PREFERRED_FORMAT;

            hr = ReadPropertyGuid(
                pWiaPropertyStorage,
                &specPreferredFormat,
                pguidFormat
            );

            if (FAILED(hr))
            {
                return hr;
            }
        }

        // Set the transfer format

        PROPSPEC specFormat;

        specFormat.ulKind = PRSPEC_PROPID;
        specFormat.propid = WIA_IPA_FORMAT;

        PROPVARIANT varFormat;

        varFormat.vt = VT_CLSID;
        varFormat.puuid = (CLSID *) CoTaskMemAlloc(sizeof(CLSID));

        if (varFormat.puuid == NULL)
        {
            return E_OUTOFMEMORY;
        }

        *varFormat.puuid = *pguidFormat;

        hr = pWiaPropertyStorage->WriteMultiple(
            1,
            &specFormat,
            &varFormat,
            WIA_IPA_FIRST
        );

        PropVariantClear(&varFormat);

        if (FAILED(hr))
        {
            return hr;
        }

        // Read the transfer buffer size from the device, default to 64K

        PROPSPEC specBufferSize;

        specBufferSize.ulKind = PRSPEC_PROPID;
        specBufferSize.propid = WIA_IPA_BUFFER_SIZE;

        LONG nBufferSize;

        hr = ReadPropertyLong(
            pWiaPropertyStorage, 
            &specBufferSize, 
            &nBufferSize
        );

        if (FAILED(hr))
        {
            nBufferSize = 64 * 1024;
        }

        // Choose double buffered transfer for better performance

        WIA_DATA_TRANSFER_INFO WiaDataTransferInfo = { 0 };

        WiaDataTransferInfo.ulSize        = sizeof(WIA_DATA_TRANSFER_INFO);
        WiaDataTransferInfo.ulBufferSize  = 2 * nBufferSize;
        WiaDataTransferInfo.bDoubleBuffer = TRUE;

        // Start the transfer

        hr = pIWiaDataTransfer->idtGetBandedData(
            &WiaDataTransferInfo,
            pDataCallback
        );

        if (FAILED(hr) || hr == S_FALSE)
        {
            return hr;
        }
    }

    return S_OK;
}
