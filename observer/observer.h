
#ifndef _OBSERVER_H
#define _OBSERVER_H

// init file
#define OBSERVER_CONFIG_FILE			"./cfg/observer.ini"
#define OBSERVER_CONFIG_SITES_FILE		"./cfg/observer_sites.ini"
#define OBSERVER_CONFIG_HARDWARE_FILE	"./cfg/observer_hardware.ini"
#define OBSERVER_CONFIG_SETUPS_FILE		"./cfg/observer_setups.ini"

// local includes
#include "observer_site.h"
#include "observer_hardware.h"

// external classes

// class:	CObserver
///////////////////////////////////////////////////////
class CObserver
{
// public methods
public:
	CObserver( );
	~CObserver( );

	void	Init( );
	void	Save( );

	int		LoadProfile( );	
	int		SaveProfile( );
	int		LoadSites( );	
	int		SaveSites( );
	void	FreeSites( );
	int		LoadHardware( );	
	int		SaveHardware( );
	int		LoadSetups( );
	int		SaveSetups( );

	wxString GetOberverFullName( ){ wxString strRet; strRet.Printf( wxT("%s %s"), m_strFirstName, m_strLastName ); return( strRet ); };

// public data
public:
	/////////////
	// :: observer profile fields
	wxString	m_strFirstName;
	wxString	m_strMiddleName;
	wxString	m_strLastName;
	wxString	m_strNickName;
	double	m_nBirthday;
	// photo
	wxString	m_strPhotoFile;
	wxString	m_strPhotoLogo;
	// profession
	wxString	m_strProfession;
	// hobbies
	wxString	m_srtHobbies;
	// website
	wxString	m_strWebsite;

	///////////
	// contact
	wxString	m_strMobile;
	wxString	m_strPhone;
	wxString	m_strFax;
	wxString	m_strEmail;
	// :: messenger contacts
	wxString	m_strMessengerYahoo;
	wxString	m_strMessengerSkype;
	wxString	m_strMessengerGTalk;
	wxString	m_strMessengerMsn;
	wxString	m_strMessengerAol;
	wxString	m_strMessengerIcq;

	/////////
	// :: ADDRESS - this goes under site ???
	wxString	m_strAddress;
	wxString	m_strZipCode;
	wxString	m_strCity;
	wxString	m_strProvince;
	wxString	m_strCountry;
	// :: latitude/longitude - most important
	double	m_nLatitude;
	double	m_nLongitude;
	double	m_nAltitude;

	// :: sites
	CObserverSite**	m_vectSite;
	int				m_nSite;

	// :: hardware
	CObserverHardware*	m_pHardware;

	/////////////////
	// SPECIAL LINKING PROPS
	unsigned long	m_nUniqId;
	// relation to others
	unsigned long*	m_vectRelation;
	int				m_nRelation;

	//////////////////////
	// defaults
	int m_nDefaultSite;
	int m_nDefaultSetup;
	int m_nDefaultTelescope;
	int m_nDefaultCamera;
	int m_nDefaultMount;
	int m_nDefaultFocuser;

};

#endif