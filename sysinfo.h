
#ifndef _HARDWARE_SYSINFO_H
#define _HARDWARE_SYSINFO_H

class CSystemInfo
{
// public methods
public:
	CSystemInfo( ){}
	~CSystemInfo( ){}

	bool GetHardwareId( wxString& strId );

	// sys info
#if defined(WIN32)
	bool GetWMIInfo( wxString system, wxString data, wxString &res, wxString nameSpace = wxT("root\\cimv2") );

//	bool GetWMIInfo( wxString system, Array <wxString> &data, Array <Value> *ret[], wxString nameSpace = wxT("root\\cimv2") );
//	bool GetWMIInfo( wxString system, wxString data, Value &res, wxString nameSpace = "root\\cimv2" );

	bool GetSystemInfo( wxString &manufacturer, wxString &productName, wxString &version, int &numberOfProcessors, wxString &mbSerial );
/*
	void GetBiosInfo(String &biosVersion, Date &biosReleaseDate, String &biosSerial);
	bool GetProcessorInfo(int number, String &vendor, String &identifier, String &architecture, int &speed);
	String GetMacAddress();
	String GetHDSerial();
	bool GetVideoInfo( Array <Value> &name, Array <Value> &description, Array <Value> &videoProcessor, 
						Array <Value> &ram, Array <Value> &videoMode);
	bool GetPackagesInfo( Array <Value> &name, Array <Value> &version, Array <Value> &vendor, 
			Array <Value> &installDate, Array <Value> &caption, Array <Value> &description, Array <Value> &state );
	double GetCpuTemperature();
*/

#endif

};

#endif

