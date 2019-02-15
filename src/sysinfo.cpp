////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// system headers
// :: wmi
#define _WIN32_DCOM
#include <iostream>
using namespace std;
#include <comdef.h>
#include <Wbemidl.h>
# pragma comment(lib, "wbemuuid.lib")

// wx
#include "wx/wxprec.h"
#include <wx/utils.h>

// local headers
#include "util/func.h"

// main header
#include "sysinfo.h"

/////////////////////////////////////////////////////////////////////
// Hardware Info
#if defined(WIN32) 

////////////////////////////////////////////////////////////////////
bool CSystemInfo::GetHardwareId( wxString& strId )
{
	int bStatus=0;

	strId=wxT("");

	wxString vSerial;
	// try to get mother board serial
	bStatus = GetWMIInfo( wxT("Win32_BaseBoard"), wxT("SerialNumber"), vSerial );
	// if not try to get bios serial
	if( !bStatus || vSerial.IsEmpty() )
		bStatus = GetWMIInfo( wxT("Win32_BIOS"), wxT("SerialNumber"), vSerial );
	// if not try to get the mac address
	if( !bStatus || vSerial.IsEmpty() )
		bStatus = GetWMIInfo( wxT("Win32_NetworkAdapterConfiguration"), wxT("MacAddress"), vSerial ); 
	// if not try to get the hdd serial
	if( !bStatus || vSerial.IsEmpty() )
		bStatus = GetWMIInfo( wxT("Win32_PhysicalMedia"), wxT("SerialNumber"), vSerial );

	/////////////////
	// check for exceptions like: ____, To Be Filled By O.E.M., OEM, None, N/A, xxxxxxxxxxx, Base Board Serial Number
	if( vSerial.CmpNoCase( wxT("____") ) == 0 || 
		vSerial.CmpNoCase( wxT("To Be Filled By O.E.M.") ) == 0 ||
		vSerial.CmpNoCase( wxT("OEM") ) == 0 ||
		vSerial.CmpNoCase( wxT("None") ) == 0 ||
		vSerial.CmpNoCase( wxT("N/A") ) == 0 ||
		vSerial.CmpNoCase( wxT("xxxxxxxxxxx") ) == 0 ||
		vSerial.CmpNoCase( wxT("Base Board Serial Number") ) == 0  ||
		vSerial.CmpNoCase( wxT("Base Board Serial#") ) == 0 || 
		vSerial.CmpNoCase( wxT("MB-1234567890") ) == 0 ) bStatus = 0;

	// at last if still emmpty generatea n unique id from time
	if( !bStatus || vSerial.IsEmpty() )
		vSerial.Printf( wxT("%lf"), GetDTime() );

	if( bStatus && !vSerial.IsEmpty() )
		strId = vSerial;

	return( bStatus );
}

////////////////////////////////////////////////////////////////////
bool CSystemInfo::GetWMIInfo( wxString system, wxString data, wxString &res, wxString nameSpace )
{
    HRESULT hres;

    // Initialize COM
//	hres =  CoInitializeEx(0, COINIT_MULTITHREADED); 
//	if( FAILED(hres) ) return 1;
	hres = CoInitialize(NULL);
	if( hres != S_OK && hres != S_FALSE ) return(0);

/*    // Set general COM security levels 
    hres =  CoInitializeSecurity(
        NULL, 
        -1,                          // COM authentication
        NULL,                        // Authentication services
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
        RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
        NULL,                        // Authentication info
        EOAC_NONE,                   // Additional capabilities 
        NULL                         // Reserved
        );
                      
    if (FAILED(hres))
    {
        CoUninitialize();
        return(0);                  
    }
*/
	hres = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT,
			RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, 0);
	if (hres != S_OK && hres != RPC_E_TOO_LATE) 
	{
		CoUninitialize();
		return(0);
	}
    
    // Obtain the initial locator to WMI 
    IWbemLocator *pLoc = NULL;

    hres = CoCreateInstance(
        CLSID_WbemLocator,             
        0, 
        CLSCTX_INPROC_SERVER, 
        IID_IWbemLocator, (LPVOID *) &pLoc);
 
    if (FAILED(hres))
    {
        CoUninitialize();
        return(0);               
    }

    // Connect to WMI through the IWbemLocator::ConnectServer method

    IWbemServices *pSvc = NULL;
	
    // Connect to the root\cimv2 namespace with
    // the current user and obtain pointer pSvc
    // to make IWbemServices calls.
    hres = pLoc->ConnectServer(
         _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
         NULL,                    // User name. NULL = current user
         NULL,                    // User password. NULL = current
         0,                       // Locale. NULL indicates current
         NULL,                    // Security flags.
         0,                       // Authority (e.g. Kerberos)
         0,                       // Context object 
         &pSvc                    // pointer to IWbemServices proxy
         );
    
    if (FAILED(hres))
    {
        pLoc->Release();     
        CoUninitialize();
        return(0);                
    }

//    cout << "Connected to ROOT\\CIMV2 WMI namespace" << endl;


    // Set security levels on the proxy 
    hres = CoSetProxyBlanket(
       pSvc,                        // Indicates the proxy to set
       RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
       RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
       NULL,                        // Server principal name 
       RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
       RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
       NULL,                        // client identity
       EOAC_NONE                    // proxy capabilities 
    );

    if (FAILED(hres))
    {
        pSvc->Release();
        pLoc->Release();     
        CoUninitialize();
		return(0);            
    }

    // Use the IWbemServices pointer to make requests of WMI 

    // For example - Win32_BaseBoard
	wxString strQuery=wxT("");
	strQuery.Printf( wxT("SELECT * FROM %s"), system );
    IEnumWbemClassObject* pEnumerator = NULL;
    hres = pSvc->ExecQuery(
        bstr_t("WQL"), 
		bstr_t(strQuery.ToAscii()),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
        NULL,
        &pEnumerator);
    
    if( FAILED(hres) )
    {
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return(0);             
    }

    // Get the data from the query 
    IWbemClassObject* pclsObj=NULL;
    ULONG uReturn = 0;
   
    while( pEnumerator )
    {
        HRESULT hr = pEnumerator->Next( WBEM_INFINITE, 1, &pclsObj, &uReturn );

        if( 0 == uReturn ) break;

        VARIANT vtProp;

        // Get the value of the Name property
		//LPCWSTR strData = ;
		hr = pclsObj->Get( (LPCWSTR) data.wc_str(), 0, &vtProp, 0, 0 );

		// check exception
		if( vtProp.vt == VT_NULL || vtProp.vt == VT_EMPTY ) continue;

		res = vtProp.bstrVal;
		res.Trim(0).Trim(1);

//        wcout << "Name : " << vtProp.bstrVal << endl;
        VariantClear(&vtProp);

//        pclsObj->Release();
    }

    // Cleanup  
    pSvc->Release();
    pLoc->Release();
    pEnumerator->Release();
	pclsObj->Release();
    CoUninitialize();

    return(1);
}

/*
////////////////////////////////////////////////////////////////////
bool CSystemInfo::GetWMIInfo( wxString system, Array <wxString> &data, Array <Value> *ret[], wxString nameSpace = wxT("root\\cimv2") )
{
	HRESULT hRes;
	
	hRes = CoInitialize(NULL);
	if( hRes != S_OK && hRes != S_FALSE ) return false;

	hRes = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT,
			RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, 0);
	if (hRes != S_OK && hRes != RPC_E_TOO_LATE) 
	{
		CoUninitialize();
		return false;
	}
	IWbemLocator* pIWbemLocator = NULL;
	if (CoCreateInstance(CLSID_WbemAdministrativeLocator, NULL, 
		CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, IID_IUnknown, (void **)&pIWbemLocator) != S_OK) 
	{
		CoUninitialize();
		return false;
	}
	
	BSTR bstrNamespace = SysAllocString(nameSpace.ToWString());
	IWbemServices* pWbemServices = NULL;
	if (pIWbemLocator->ConnectServer(bstrNamespace, NULL, NULL, NULL, 0, NULL, NULL,
		&pWbemServices) != S_OK) 
	{
		CoUninitialize();
		return false;
	}
	SysFreeString(bstrNamespace);
	
	hRes = CoSetProxyBlanket(pWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, 
			RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
    if (FAILED(hRes)) {
        pWbemServices->Release();
        pIWbemLocator->Release();     
        CoUninitialize();
        return false;
    }
	
	IEnumWbemClassObject* pEnumerator = NULL;
	String query;
	query << "Select * from " << system;
	WCHAR wquery[1024*sizeof(WCHAR)];
	MultiByteToWideChar(CP_UTF8, 0, query, -1, wquery, sizeof(wquery)/sizeof(wquery[0]));
	BSTR strQuery = SysAllocString(wquery);
	BSTR strQL = SysAllocString(L"WQL");
	hRes = pWbemServices->ExecQuery(strQL, strQuery, WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
	if( hRes != S_OK ) 
	{
        pWbemServices->Release();
        pIWbemLocator->Release();     
        CoUninitialize();
        return false;
    }
	SysFreeString(strQuery);
	SysFreeString(strQL);

    IWbemClassObject *pClassObject;
    ULONG uReturn = 0;
    int row = 0;
    bool rt = false;
	while (pEnumerator) 
	{
        hRes = pEnumerator->Next(WBEM_INFINITE, 1, &pClassObject, &uReturn);
       	if(0 == uReturn) 
		{
       		if( rt )
       			break;
       		else 
			{
	   			pIWbemLocator->Release();
				pWbemServices->Release();
				pEnumerator->Release();
				CoUninitialize(); 
	            return false;
       		}
       	}
		if(hRes != S_OK) 
		{
	        pWbemServices->Release();
	        pIWbemLocator->Release(); 
	        pEnumerator->Release(); 
	        pClassObject->Release();   
	        CoUninitialize();
	        return false;
	    }
		for (int col = 0; col < data.GetCount(); ++col) 
		{
			VARIANT vProp;
			BSTR strClassProp = SysAllocString(data[col].ToWString());
	        hRes = pClassObject->Get(strClassProp, 0, &vProp, 0, 0);
	        if( hRes != S_OK )
			{
		        pWbemServices->Release();
		        pIWbemLocator->Release(); 
		        pEnumerator->Release(); 
		        pClassObject->Release();   
		        CoUninitialize();
		        return false;
		    }
			SysFreeString(strClassProp);        
			ret[col]->Add(GetVARIANT(vProp));
			rt = true;
		}
		row++;
    }
	pIWbemLocator->Release();
	pWbemServices->Release();
	pEnumerator->Release();
	pClassObject->Release();
	CoUninitialize(); 
	
	return true;
}

////////////////////////////////////////////////////////////////////
bool CSystemInfo::GetWMIInfo( String system, String data, Value &res, String nameSpace = "root\\cimv2" ) 
{
	Array <Value> arrRes;
	Array <Value> *arrResP[1];
	arrResP[0] = &arrRes;
	Array <String> arrData;
	arrData.Add(data);
	bool ret = GetWMIInfo(system, arrData, arrResP, nameSpace);
	if( ret ) res = arrRes[0];
	return ret;
}
*/

////////////////////////////////////////////////////////////////////
bool CSystemInfo::GetSystemInfo( wxString &manufacturer, wxString &productName, 
									wxString &version, int &numberOfProcessors, wxString &mbSerial ) 
{ 
	manufacturer = wxT("");
	wxString vmanufacturer;
	if( GetWMIInfo( wxT("Win32_ComputerSystem"), wxT("manufacturer"), vmanufacturer ) ) 
		manufacturer = vmanufacturer.Trim(0).Trim(1);
//	if( manufacturer.IsEmpty() ) 
//		manufacturer = FromSystemCharset(GetWinRegString("SystemManufacturer", "HARDWARE\\DESCRIPTION\\System\\BIOS", HKEY_LOCAL_MACHINE));
	if( manufacturer.IsEmpty() ) 
	{
//		StringParse fileData = LoadFile(AppendFileName(GetSystemFolder(), "oeminfo.ini"));
//		fileData.GoAfter("Manufacturer=");
//		manufacturer = fileData.GetText("\r\n");
	}
	productName = wxT("");
	wxString vproductName;
	if( GetWMIInfo( wxT("Win32_ComputerSystem"), wxT("model"), vproductName ) ) 
		productName = vproductName.Trim(0).Trim(1);
//	if( productName.IsEmpty() )
//		productName = FromSystemCharset(GetWinRegString("SystemProductName", "HARDWARE\\DESCRIPTION\\System\\BIOS", HKEY_LOCAL_MACHINE));
//	if( productName.IsEmpty() )
//		productName = FromSystemCharset(GetWinRegString("Model", "SOFTWARE\\Microsoft\\PCHealth\\HelpSvc\\OEMInfo", HKEY_LOCAL_MACHINE));
	
//	version = FromSystemCharset(GetWinRegString("SystemVersion", "HARDWARE\\DESCRIPTION\\System\\BIOS", HKEY_LOCAL_MACHINE));
	numberOfProcessors = wxAtoi( wxGetenv( wxT("NUMBER_OF_PROCESSORS") ) );
	wxString vmbSerial;
	if( GetWMIInfo( wxT("Win32_BaseBoard"), wxT("SerialNumber"), vmbSerial ) ) 
		mbSerial = vmbSerial.Trim(0).Trim(1);
	else
		return(0);

	return(1);
}

/*
////////////////////////////////////////////////////////////////////
void CSystemInfo::GetBiosInfo( String &biosVersion, Date &biosReleaseDate, String &biosSerial ) 
{ 
	// Alternative is "wmic bios get name" and "wmic bios get releasedate"
	String strBios = FromSystemCharset(GetWinRegString("BIOSVersion", "HARDWARE\\DESCRIPTION\\System\\BIOS", HKEY_LOCAL_MACHINE));	
	if (strBios.IsEmpty())
		strBios = FromSystemCharset(GetWinRegString("SystemBiosVersion", "HARDWARE\\DESCRIPTION\\System", HKEY_LOCAL_MACHINE));	
	for (int i = 0; i < strBios.GetLength(); ++i) {
		if (strBios[i] == '\0')
			biosVersion.Cat(". ");
		else
			biosVersion.Cat(strBios[i]);
	}
	String strDate = FromSystemCharset(GetWinRegString("BIOSReleaseDate", "HARDWARE\\DESCRIPTION\\System\\BIOS", HKEY_LOCAL_MACHINE));
	if (strDate.IsEmpty())
		strDate = FromSystemCharset(GetWinRegString("SystemBiosDate", "HARDWARE\\DESCRIPTION\\System", HKEY_LOCAL_MACHINE));
	int lang = GetCurrentLanguage();
	SetLanguage(LNG_ENGLISH);
	if (!StrToDate(biosReleaseDate, strDate))
		biosReleaseDate = Null;
	SetLanguage(lang);
	Value vmbSerial;
	if (GetWMIInfo("Win32_BIOS", "SerialNumber", vmbSerial)) 
		biosSerial = TrimBoth(vmbSerial);	
}

////////////////////////////////////////////////////////////////////
bool CSystemInfo::GetProcessorInfo(int number, String &vendor, String &identifier, String &architecture, int &speed)	
{
	String strReg = Format("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\%d", number);
	vendor = FromSystemCharset(GetWinRegString("VendorIdentifier", strReg, HKEY_LOCAL_MACHINE));	
	identifier = FromSystemCharset(GetWinRegString("ProcessorNameString", strReg, HKEY_LOCAL_MACHINE));	
	architecture = FromSystemCharset(GetWinRegString("Identifier", strReg, HKEY_LOCAL_MACHINE));	
	speed = GetWinRegInt("~MHz", strReg, HKEY_LOCAL_MACHINE);
	
	return true;
}

////////////////////////////////////////////////////////////////////
String CSystemInfo::GetMacAddress() 
{
	Value vmac;
	if (!GetWMIInfo("Win32_NetworkAdapterConfiguration", "MacAddress", vmac)) 
		return false;
	String mac = TrimBoth(vmac);	
	if (mac.GetCount() == 17)
		return mac;
	return Null;
}
////////////////////////////////////////////////////////////////////
String CSystemInfo::GetHDSerial()
{
	Value vmbSerial;
	if (!GetWMIInfo("Win32_PhysicalMedia", "SerialNumber", vmbSerial)) 
		return false;
	String serial = TrimBoth(vmbSerial);	
	if (serial.GetCount() > 0)
		return serial;
	return Null;
}

////////////////////////////////////////////////////////////////////
bool CSystemInfo::GetVideoInfo( Array <Value> &name, Array <Value> &description, Array <Value> &videoProcessor, 
					Array <Value> &ram, Array <Value> &videoMode ) 
{
	Array <Value> *res[5];
	res[0] = &name;
	res[1] = &description;
	res[2] = &videoProcessor;
	res[3] = &ram;
	res[4] = &videoMode;
	Array <String> data;
	data.Add("Name");
	data.Add("Description");
	data.Add("VideoProcessor");
	data.Add("AdapterRAM");
	data.Add("VideoModeDescription");
	if (!GetWMIInfo("Win32_VideoController", data, res))
		return false;
	
	for (int row = 0; row < ram.GetCount(); ++row) 
		ram[row] = (atoi(ram[row].ToString()) + 524288)/1048576;
	return true;
}

////////////////////////////////////////////////////////////////////
bool CSystemInfo::GetPackagesInfo(Array <Value> &name, Array <Value> &version, Array <Value> &vendor, 
				Array <Value> &installDate, Array <Value> &caption, Array <Value> &description, Array <Value> &state)
{
	Array <Value> *res[7];
	res[0] = &name;
	res[1] = &version;
	res[2] = &vendor;
	res[3] = &installDate;	
	res[4] = &caption;	
	res[5] = &description;	
	res[6] = &state;	
	Array <String> data;
	data.Add("Name");
	data.Add("Version");
	data.Add("Vendor");
	data.Add("InstallDate2");
	data.Add("Caption");
	data.Add("Description");
	data.Add("InstallState");
	if (!GetWMIInfo("Win32_Product", data, res))
		return false;
	
	for (int i = 0; i < installDate.GetCount(); ++i) {
		String sdate = installDate[i];
		Time t(atoi(sdate.Left(4)), atoi(sdate.Mid(4, 2)), atoi(sdate.Mid(6, 2)), 
			   atoi(sdate.Mid(8, 2)), atoi(sdate.Mid(10, 2)), atoi(sdate.Mid(12, 2)));
		installDate[i] = t;
		int istate = state[i];
		switch (istate) {
		case -6:	state[i] = "Bad Configuration";	break;
		case -2:	state[i] = "Invalid Argument";	break;
		case -1:	state[i] = "Unknown Package";	break;
		case 1:		state[i] = "Advertised";		break;
		case 2:		state[i] = "Absent";			break;
		case 5:		state[i] = "Ok";				break;
		default: 	return false;
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////
double CSystemInfo::GetCpuTemperature() 
{
	Value data;
	if (GetWMIInfo("MSAcpi_ThermalZoneTemperature", "CurrentTemperature", data, "root\\wmi"))
		return (double(data) - 2732.) / 10.;
	if (GetWMIInfo("Win32_TemperatureProbe", "CurrentReading", data))
		return data;
	return Null;
}
*/

#endif

////////////////////////////////////////////////////////////////////
// POSIX PLATFORM 
////////////////////////////////////////////////////////////////////

#if defined (PLATFORM_POSIX)
void GetSystemInfo(String &manufacturer, String &productName, String &version, int &numberOfProcessors, String &mbSerial)
{
	manufacturer = LoadFile_Safe("/sys/devices/virtual/dmi/id/board_vendor");
	productName = LoadFile_Safe("/sys/devices/virtual/dmi/id/board_name");
	version = LoadFile_Safe("/sys/devices/virtual/dmi/id/product_version");
	mbSerial = LoadFile_Safe("/sys/devices/virtual/dmi/id/board_serial");
	if (mbSerial.IsEmpty()) 
		mbSerial = FormatInt(gethostid());	

	StringParse cpu(LoadFile_Safe("/proc/cpuinfo"));	
	numberOfProcessors = 1;
	while (cpu.GoAfter("processor")) {
		cpu.GoAfter(":");
		numberOfProcessors = atoi(cpu.GetText()) + 1;
	} 
}
void GetBiosInfo(String &biosVersion, Date &biosReleaseDate, String &biosSerial)
{
	String biosVendor = LoadFile_Safe("/sys/devices/virtual/dmi/id/bios_vendor");
	biosVersion = LoadFile_Safe("/sys/devices/virtual/dmi/id/bios_version");
	StrToDate(biosReleaseDate, LoadFile_Safe("/sys/devices/virtual/dmi/id/bios_date"));
	biosSerial = LoadFile_Safe("/sys/devices/virtual/dmi/id/chassis_serial");
}
bool GetProcessorInfo(int number, String &vendor, String &identifier, String &architecture, int &speed)	
{
	StringParse cpu(LoadFile_Safe("/proc/cpuinfo"));
	
	int cpuNumber;
	do {
		if (!cpu.GoAfter("processor", ":"))
			return false;
		String sCpu = cpu.GetText();
		if (sCpu == "")
			return false;
		cpuNumber = atoi(sCpu);
	} while (cpuNumber != number);
		
	cpu.GoAfter("vendor_id", ":");
	vendor = cpu.GetText();
	cpu.GoAfter("cpu family", ":");
	String family = cpu.GetText();		// 6 means 686
	cpu.GoAfter("model", ":");
	String model = cpu.GetText();
	cpu.GoAfter("model name", ":");
	identifier = cpu.GetText("\n");
	cpu.GoAfter("stepping", ":");
	String stepping = cpu.GetText();
	architecture = Sys("uname -m");		// CPU type
	architecture << " Family " << family << " Model " << model << " Stepping " << stepping;		// And 64 bits ?? uname -m
	cpu.GoAfter_Init("cpu MHz", ":");
	speed = cpu.GetInt();
}
double GetCpuTemperature() {
/* This is only if acpi package is present
	StringParse data = Sys("acpi -V");	
	data.GoAfter("Thermal", ",");
	return data.GetDouble();
*/
	FindFile ff;
	if(ff.Search("/proc/acpi/thermal_zone/*")) {
		do {
			if (ff.IsDirectory()) {
				String name = ff.GetName();
				if (name != "." && name != "..") {
					StringParse str = LoadFile_Safe(AppendFileName(AppendFileName("/proc/acpi/thermal_zone", name), "temperature"));			
					str.GoAfter("temperature:");
					return str.GetDouble();
				}
			}
		} while(ff.Next());
	}
	return Null;
}
String GetMacAddress() {
	StringParse data = Sys("ifconfig -a");
	
	data.GoAfter("eth0");
	String line = TrimBoth(data.GetLine());
	int pos = line.ReverseFind(' ');
	if (pos == -1)
		return Null;
	
	String ret = line.Mid(pos+1);
	if (ret.GetCount() == 17)
		return ret;
	return Null;
}
#endif
