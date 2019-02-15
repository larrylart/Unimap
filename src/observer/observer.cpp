////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// system libs
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// wx includes
#include "wx/wxprec.h"
#include <wx/regex.h>
#include <wx/string.h>
#include <wx/tokenzr.h>

// local includes
#include "../util/func.h"

// main header
#include "observer.h"

////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CObserver
// Purpose:	Initialize my dialog
// Input:	pointer to reference window 
// Output:	nothing
////////////////////////////////////////////////////////////////////
CObserver::CObserver( )
{
	m_strFirstName = wxT( "" );
	m_strMiddleName = wxT( "" );
	m_strLastName = wxT( "" );
	m_strNickName = wxT( "" );
	m_nBirthday = 0.0;
	m_strPhotoFile = wxT( "" );
	m_strPhotoLogo = wxT( "" );

	m_strProfession = wxT( "" );
	m_srtHobbies = wxT( "" );
	m_strWebsite = wxT( "" );

	m_strMobile = wxT( "" );
	m_strPhone = wxT( "" );
	m_strFax = wxT( "" );
	m_strEmail = wxT( "" );

	m_strMessengerYahoo = wxT( "" );
	m_strMessengerAol = wxT( "" );
	m_strMessengerMsn = wxT( "" );
	m_strMessengerSkype = wxT( "" );
	m_strMessengerGTalk = wxT( "" );
	m_strMessengerIcq = wxT( "" );

	m_strAddress = wxT( "" );
	m_strZipCode = wxT( "" );
	m_strCity = wxT( "" );
	m_strProvince = wxT( "" );
	m_strCountry = wxT( "" );

	m_nLatitude = 0.0;
	m_nLongitude = 0.0;
	m_nAltitude = 0.0;

	m_vectSite = NULL;
	m_nSite =0;
	m_pHardware = NULL;

	m_nUniqId = 0;
	m_vectRelation = NULL;
	m_nRelation =0;
	// set defaults
	m_nDefaultSite = 0;
	m_nDefaultSetup = 0;
	m_nDefaultTelescope = 0;
	m_nDefaultCamera = 0;
	m_nDefaultMount = 0;
	m_nDefaultFocuser = 0;
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CObserver
// Purpose:	destroy my dialog
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CObserver::~CObserver( ) 
{
	int i=0;
	// save my stuff
	Save( );
	// delete allocated
	FreeSites( );
	if( m_pHardware ) delete( m_pHardware );
	if( m_vectRelation ) free( m_vectRelation );
}

////////////////////////////////////////////////////////////////////
void CObserver::Save( ) 
{
	// save my stuff
	SaveProfile( );
	SaveSites( );
	SaveHardware( );
	SaveSetups( );
}

////////////////////////////////////////////////////////////////////
void CObserver::Init( ) 
{
	// load main profile
	LoadProfile( );
	// load sites
	LoadSites( );
	// load hardware
	m_pHardware = new CObserverHardware( );
	LoadHardware( );
	// load setups
	LoadSetups( );

	return;
}

////////////////////////////////////////////////////////////////////
int CObserver::LoadProfile( ) 
{
	FILE* pFile = NULL;
	wxChar strLine[2000];
	// regex :: profile
	wxRegEx reProfileFirstName = wxT( "^FirstName=(.*)" );
	wxRegEx reProfileMiddleName = wxT( "^MiddleName=(.*)" );
	wxRegEx reProfileLastName = wxT( "^LastName=(.*)" );
	wxRegEx reProfileNickName = wxT( "^NickName=(.*)" );
	wxRegEx reProfileBirthday = wxT( "^Birthday=(.*)" );
	wxRegEx reProfileProfession = wxT( "^Profession=(.*)" );
	wxRegEx reProfileHobbies = wxT( "^Hobbies=(.*)" );
	wxRegEx reProfileWebsite = wxT( "^Website=(.*)" );
	wxRegEx reProfilePhoto = wxT( "^PhotoFile=(.*)" );
	// address
	wxRegEx reAddress = wxT( "^Address=(.*)" );
	wxRegEx reAddrZipcode = wxT( "^ZipCode=(.*)" );
	wxRegEx reAddrCity = wxT( "^City=(.*)" );
	wxRegEx reAddrProvince = wxT( "^Province=(.*)" );
	wxRegEx reAddrCountry = wxT( "^Country=(.*)" );
	wxRegEx reAddrLat = wxT( "^Latitude=(.*)" );
	wxRegEx reAddrLon = wxT( "^Longitude=(.*)" );
	wxRegEx reAddrAlt = wxT( "^Altitude=(.*)" );
	// contact
	wxRegEx reCntMobile = wxT( "^Mobile=(.*)" );
	wxRegEx reCntPhone = wxT( "^Phone=(.*)" );
	wxRegEx reCntFax = wxT( "^Fax=(.*)" );
	wxRegEx reCntEmail = wxT( "^Email=(.*)" );
	wxRegEx reCntIMYahoo = wxT( "^IMYahoo=(.*)" );
	wxRegEx reCntIMSkype = wxT( "^IMSkype=(.*)" );
	wxRegEx reCntIMGTalk = wxT( "^IMGTalk=(.*)" );
	wxRegEx reCntIMMsn = wxT( "^IMMsn=(.*)" );
	wxRegEx reCntIMAol = wxT( "^IMAol=(.*)" );
	wxRegEx reCntIMIcq = wxT( "^IMIcq=(.*)" );
	// defaults
	wxRegEx reDefSite = wxT( "^DefSite=([0-9]+)" );
	wxRegEx reDefSetup = wxT( "^DefSetup=([0-9]+)" );
	wxRegEx reDefTel = wxT( "^DefTel=([0-9]+)" );
	wxRegEx reDefCam = wxT( "^DefCam=([0-9]+)" );
	wxRegEx reDefMount = wxT( "^DefMount=([0-9]+)" );
	wxRegEx reDefFocuser = wxT( "^DefFocuser=([0-9]+)" );

	// open local file to read from
	pFile = wxFopen( wxT(OBSERVER_CONFIG_FILE), wxT("r") );
	if( !pFile ) return( 0 );

	while( !feof( pFile ) )
	{
		// clear buffers
//		bzero( strLine, 255 );
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pFile );
		//  if less then 3 skip
		if( wxStrlen( strLine ) < 3 ) continue; 

//		wxString strWxLine = strLine;

		// PROFILE :: check which pattern
		if( reProfileFirstName.Matches( strLine ) )
			m_strFirstName = reProfileFirstName.GetMatch( strLine, 1 ).Trim(0).Trim(1);
		else if( reProfileMiddleName.Matches( strLine ) )
			m_strMiddleName = reProfileMiddleName.GetMatch( strLine, 1 ).Trim(0).Trim(1);
		else if( reProfileLastName.Matches( strLine ) )
			m_strLastName = reProfileLastName.GetMatch( strLine, 1 ).Trim(0).Trim(1);
		else if( reProfileNickName.Matches( strLine ) )
			m_strNickName = reProfileNickName.GetMatch( strLine, 1 ).Trim(0).Trim(1);
		else if( reProfileBirthday.Matches( strLine ) )
			reProfileBirthday.GetMatch( strLine, 1 ).Trim(0).Trim(1).ToDouble( &m_nBirthday );
		else if( reProfileProfession.Matches( strLine ) )
			m_strProfession = reProfileProfession.GetMatch( strLine, 1 ).Trim(0).Trim(1);
		else if( reProfileHobbies.Matches( strLine ) )
			m_srtHobbies = reProfileHobbies.GetMatch( strLine, 1 ).Trim(0).Trim(1);
		else if( reProfileWebsite.Matches( strLine ) )
			m_strWebsite = reProfileWebsite.GetMatch( strLine, 1 ).Trim(0).Trim(1);
		else if( reProfilePhoto.Matches( strLine ) )
			m_strPhotoFile = reProfilePhoto.GetMatch( strLine, 1 ).Trim(0).Trim(1);
		// PROFILE :: address
		else if( reAddress.Matches( strLine ) )
			m_strAddress = reAddress.GetMatch( strLine, 1 ).Trim(0).Trim(1);
		else if( reAddrZipcode.Matches( strLine ) )
			m_strZipCode = reAddrZipcode.GetMatch( strLine, 1 ).Trim(0).Trim(1);
		else if( reAddrCity.Matches( strLine ) )
			m_strCity = reAddrCity.GetMatch( strLine, 1 ).Trim(0).Trim(1);
		else if( reAddrProvince.Matches( strLine ) )
			m_strProvince = reAddrProvince.GetMatch( strLine, 1 ).Trim(0).Trim(1);
		else if( reAddrCountry.Matches( strLine ) )
			m_strCountry = reAddrCountry.GetMatch( strLine, 1 ).Trim(0).Trim(1);
		else if( reAddrLat.Matches( strLine ) )
			reAddrLat.GetMatch( strLine, 1 ).Trim(0).Trim(1).ToDouble( &m_nLatitude );
		else if( reAddrLon.Matches( strLine ) )
			reAddrLon.GetMatch( strLine, 1 ).Trim(0).Trim(1).ToDouble( &m_nLongitude );
		else if( reAddrAlt.Matches( strLine ) )
			reAddrAlt.GetMatch( strLine, 1 ).Trim(0).Trim(1).ToDouble( &m_nAltitude );
		// PROFILE :: contact
		else if( reCntMobile.Matches( strLine ) )
			m_strMobile = reCntMobile.GetMatch( strLine, 1 ).Trim(0).Trim(1);
		else if( reCntPhone.Matches( strLine ) )
			m_strPhone = reCntPhone.GetMatch( strLine, 1 ).Trim(0).Trim(1);
		else if( reCntFax.Matches( strLine ) )
			m_strFax = reCntFax.GetMatch( strLine, 1 ).Trim(0).Trim(1);
		else if( reCntEmail.Matches( strLine ) )
			m_strEmail = reCntEmail.GetMatch( strLine, 1 ).Trim(0).Trim(1);
		else if( reCntIMYahoo.Matches( strLine ) )
			m_strMessengerYahoo = reCntIMYahoo.GetMatch( strLine, 1 ).Trim(0).Trim(1);
		else if( reCntIMSkype.Matches( strLine ) )
			m_strMessengerSkype = reCntIMSkype.GetMatch( strLine, 1 ).Trim(0).Trim(1);
		else if( reCntIMGTalk.Matches( strLine ) )
			m_strMessengerGTalk = reCntIMGTalk.GetMatch( strLine, 1 ).Trim(0).Trim(1);
		else if( reCntIMMsn.Matches( strLine ) )
			m_strMessengerMsn = reCntIMMsn.GetMatch( strLine, 1 ).Trim(0).Trim(1);
		else if( reCntIMAol.Matches( strLine ) )
			m_strMessengerAol = reCntIMAol.GetMatch( strLine, 1 ).Trim(0).Trim(1);
		else if( reCntIMIcq.Matches( strLine ) )
			m_strMessengerIcq = reCntIMIcq.GetMatch( strLine, 1 ).Trim(0).Trim(1);
		// :: DEFAULTS
		else if( reDefSite.Matches( strLine ) )
			m_nDefaultSite = wxAtoi( reDefSite.GetMatch( strLine, 1 ).Trim(0).Trim(1) );
		else if( reDefSetup.Matches( strLine ) )
			m_nDefaultSetup = wxAtoi( reDefSetup.GetMatch( strLine, 1 ).Trim(0).Trim(1) );
		else if( reDefTel.Matches( strLine ) )
			m_nDefaultTelescope = wxAtoi( reDefTel.GetMatch( strLine, 1 ).Trim(0).Trim(1) );
		else if( reDefCam.Matches( strLine ) )
			m_nDefaultCamera = wxAtoi( reDefCam.GetMatch( strLine, 1 ).Trim(0).Trim(1) );
		else if( reDefMount.Matches( strLine ) )
			m_nDefaultMount = wxAtoi( reDefMount.GetMatch( strLine, 1 ).Trim(0).Trim(1) );
		else if( reDefFocuser.Matches( strLine ) )
			m_nDefaultFocuser = wxAtoi( reDefFocuser.GetMatch( strLine, 1 ).Trim(0).Trim(1) );

	}
	// close my file
	fclose( pFile );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CObserver::SaveProfile( ) 
{
	FILE* pFile = NULL;

	// open local file to write
	pFile = wxFopen( wxT(OBSERVER_CONFIG_FILE), wxT("w") );
	if( !pFile ) return( 0 );

	fprintf( pFile, "\n" );

	// SAVE :: PROFILE
	wxFprintf( pFile, wxT("[Profile]\n") );
	wxFprintf( pFile, wxT("FirstName=%s\n"), m_strFirstName );
	wxFprintf( pFile, wxT("MiddleName=%s\n"), m_strMiddleName );
	wxFprintf( pFile, wxT("LastName=%s\n"), m_strLastName );
	wxFprintf( pFile, wxT("NickName=%s\n"), m_strNickName );
	wxFprintf( pFile, wxT("Birthday=%lf\n"), m_nBirthday );
	wxFprintf( pFile, wxT("Profession=%s\n"), m_strProfession );
	wxFprintf( pFile, wxT("Hobbies=%s\n"), m_srtHobbies );
	wxFprintf( pFile, wxT("Website=%s\n"), m_strWebsite );
	wxFprintf( pFile, wxT("PhotoFile=%s\n"), m_strPhotoFile );
	// address part
	wxFprintf( pFile, wxT("\n[Address]\n") );
	wxFprintf( pFile, wxT("Address=%s\n"), m_strAddress );
	wxFprintf( pFile, wxT("ZipCode=%s\n"), m_strZipCode );
	wxFprintf( pFile, wxT("City=%s\n"), m_strCity );
	wxFprintf( pFile, wxT("Province=%s\n"), m_strProvince );
	wxFprintf( pFile, wxT("Country=%s\n"), m_strCountry );
	wxFprintf( pFile, wxT("Latitude=%lf\n"), m_nLatitude );
	wxFprintf( pFile, wxT("Longitude=%lf\n"), m_nLongitude );
	wxFprintf( pFile, wxT("Altitude=%lf\n"), m_nAltitude );
	// contact part
	wxFprintf( pFile, wxT("\n[Contact]\n") );
	wxFprintf( pFile, wxT("Mobile=%s\n"), m_strMobile );
	wxFprintf( pFile, wxT("Phone=%s\n"), m_strPhone );
	wxFprintf( pFile, wxT("Fax=%s\n"), m_strFax );
	wxFprintf( pFile, wxT("Email=%s\n"), m_strEmail );
	wxFprintf( pFile, wxT("IMYahoo=%s\n"), m_strMessengerYahoo );
	wxFprintf( pFile, wxT("IMSkype=%s\n"), m_strMessengerSkype );
	wxFprintf( pFile, wxT("IMGTalk=%s\n"), m_strMessengerGTalk );
	wxFprintf( pFile, wxT("IMMsn=%s\n"), m_strMessengerMsn );
	wxFprintf( pFile, wxT("IMAol=%s\n"), m_strMessengerAol );
	wxFprintf( pFile, wxT("IMIcq=%s\n"), m_strMessengerIcq );
	// defaults
	wxFprintf( pFile, wxT("\n[Defaults]\n") );
	wxFprintf( pFile, wxT("DefSite=%d\n"), m_nDefaultSite );
	wxFprintf( pFile, wxT("DefSetup=%d\n"), m_nDefaultSetup );
	wxFprintf( pFile, wxT("DefTel=%d\n"), m_nDefaultTelescope );
	wxFprintf( pFile, wxT("DefCam=%d\n"), m_nDefaultCamera );
	wxFprintf( pFile, wxT("DefMount=%d\n"), m_nDefaultMount );
	wxFprintf( pFile, wxT("DefFocuser=%d\n"), m_nDefaultFocuser );

	// close my file
	fclose( pFile );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
void CObserver::FreeSites( ) 
{
	if( m_vectSite )
	{
		for(int i=0; i<m_nSite; i++) delete( m_vectSite[i] );
		free( m_vectSite );
	}
	m_vectSite = NULL;
	m_nSite = 0;
}

////////////////////////////////////////////////////////////////////
int CObserver::LoadSites( ) 
{
	FILE* pFile = NULL;
	wxChar strLine[2000];
	// regex
	wxRegEx reSiteName = wxT( "^\\[(.+)\\]" );
	wxRegEx reCity = wxT( "^City=(.*)" );
	wxRegEx reProvince = wxT( "^Province=(.*)" );
	wxRegEx reCountry = wxT( "^Country=(.*)" );
	wxRegEx reLat = wxT( "^Latitude=(.*)" );
	wxRegEx reLon = wxT( "^Longitude=(.*)" );
	wxRegEx reAlt = wxT( "^Altitude=(.*)" );
	wxRegEx reNotes = wxT( "^Notes=(.*)" );

	// free current alllocation
	FreeSites( );

	// open local file to read from
	pFile = wxFopen( wxT(OBSERVER_CONFIG_SITES_FILE), wxT("r") );
	if( !pFile ) return( 0 );

	while( !feof( pFile ) )
	{
		// clear buffers
//		bzero( strLine, 255 );
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pFile );

		//  if less then 3 skip
		if( wxStrlen( strLine ) < 3 ) continue; 

//		wxString strWxLine(strLine,wxConvUTF8);

		// PROFILE :: check which pattern
		if( reSiteName.Matches( strLine ) )
		{
			if( m_vectSite ==  NULL )
				// allocate first record
				m_vectSite = (CObserverSite**) calloc( 1, sizeof(CObserverSite*) );
			else
				// re-allocate 
				m_vectSite = (CObserverSite**) _recalloc( m_vectSite, m_nSite+1, sizeof(CObserverSite*) );

			m_vectSite[m_nSite] = new CObserverSite();
			m_vectSite[m_nSite]->m_strSiteName = reSiteName.GetMatch( strLine, 1 ).Trim(0).Trim(1);
			m_nSite++;

		} else if( reCity.Matches( strLine ) )
			m_vectSite[m_nSite-1]->m_strCity = reCity.GetMatch( strLine, 1 ).Trim(0).Trim(1);
		else if( reProvince.Matches( strLine ) )
			m_vectSite[m_nSite-1]->m_strProvince = reProvince.GetMatch( strLine, 1 ).Trim(0).Trim(1);
		else if( reCountry.Matches( strLine ) )
			m_vectSite[m_nSite-1]->m_strCountry = reCountry.GetMatch( strLine, 1 ).Trim(0).Trim(1);
		else if( reNotes.Matches( strLine ) )
			m_vectSite[m_nSite-1]->m_strSiteNotes = reNotes.GetMatch( strLine, 1 ).Trim(0).Trim(1);
		else if( reLat.Matches( strLine ) )
			reLat.GetMatch( strLine, 1 ).Trim(0).Trim(1).ToDouble( &(m_vectSite[m_nSite-1]->m_nLatitude) );
		else if( reLon.Matches( strLine ) )
			reLon.GetMatch( strLine, 1 ).Trim(0).Trim(1).ToDouble( &(m_vectSite[m_nSite-1]->m_nLongitude) );
		else if( reAlt.Matches( strLine ) )
			reAlt.GetMatch( strLine, 1 ).Trim(0).Trim(1).ToDouble( &(m_vectSite[m_nSite-1]->m_nAltitude) );
	
	}
	// close my file
	fclose( pFile );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CObserver::SaveSites( ) 
{
	FILE* pFile = NULL;
	int i=0;

	// open local file to write
	pFile = wxFopen( wxT(OBSERVER_CONFIG_SITES_FILE), wxT("w") );
	if( !pFile ) return( 0 );

	// for all sites defined
	for( i=0; i<m_nSite; i++ )
	{
		wxFprintf( pFile, wxT("\n") );

		// SAVE :: SITE
		wxFprintf( pFile, wxT("[%s]\n"), m_vectSite[i]->m_strSiteName );
		wxFprintf( pFile, wxT("City=%s\n"), m_vectSite[i]->m_strCity );
		wxFprintf( pFile, wxT("Province=%s\n"), m_vectSite[i]->m_strProvince );
		wxFprintf( pFile, wxT("Country=%s\n"), m_vectSite[i]->m_strCountry );
		wxFprintf( pFile, wxT("Latitude=%lf\n"), m_vectSite[i]->m_nLatitude );
		wxFprintf( pFile, wxT("Longitude=%lf\n"), m_vectSite[i]->m_nLongitude );
		wxFprintf( pFile, wxT("Altitude=%lf\n"), m_vectSite[i]->m_nAltitude );
		wxFprintf( pFile, wxT("Notes=%s\n"), m_vectSite[i]->m_strSiteNotes );
	}

	// close my file
	fclose( pFile );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CObserver::LoadHardware( ) 
{
	FILE* pFile = NULL;
	wxChar strLine[2000];
	int nCamId=0, nTelId=0, nCamLensId=0, nBLensId=0, nFRedId=0,
		nEyepiece=0, nMount=0, nFocuser=0, nTFilter=0, nHardwareType=0;
	long nVarLong=0;
	// regex 
	wxRegEx reHardwareName = wxT( "^\\(([0-9]+)\\)\\[(.+)\\]" );
	wxRegEx reType = wxT( "^Type=(.*)" );
	wxRegEx reBrand = wxT( "^Brand=(.*)" );
	// regex :: camera
	wxRegEx reSensorWidth = wxT( "^SensorWidth=(.*)" );
	wxRegEx reSensorHeight = wxT( "^SensorHeight=(.*)" );
	wxRegEx reSensorPixelSize = wxT( "^SensorPixelSize=(.*)" );
	// regex :: telescope
	wxRegEx reFocalLength = wxT( "^FocalLength=(.*)" );
	wxRegEx reClearAperture = wxT( "^ClearAperture=(.*)" );
	wxRegEx reController = wxT( "^Controller=(.*)" );
	// regex :: camera lens
//	wxRegEx reFocalLength = wxT( "^FocalLength=(.*)" );
	wxRegEx reAperture = wxT( "^Aperture=(.*)" );
	wxRegEx reViewAngle = wxT( "^ViewAngle=(.*)" );
	// regex :: barllow lens
	wxRegEx reMagnification = wxT( "^Magnification=(.*)" );
	wxRegEx reElements = wxT( "^Elements=(.*)" );
	wxRegEx reBarrel = wxT( "^Barrel=(.*)" );
	// regex :: barllow lens
	wxRegEx reFocalRatio = wxT( "^FocalRatio=(.*)" );
	// regex :: Mounts
	wxRegEx reCapacity = wxT( "^Capacity=(.*)" );
	wxRegEx rePEM = wxT( "^PEM=(.*)" );
	// regex :: Focuser
	wxRegEx reDrawtube = wxT( "^Drawtube=(.*)" );
	wxRegEx reWeight = wxT( "^Weight=(.*)" );
	// regex :: telescope filters
	wxRegEx reBandpass = wxT( "^Bandpass=(.*)" );
	wxRegEx reTransmission = wxT( "^Transmission=(.*)" );

	// open local file to read from
	pFile = wxFopen( wxT(OBSERVER_CONFIG_HARDWARE_FILE), wxT("r") );
	if( !pFile ) return( 0 );

	while( !feof( pFile ) )
	{
		// clear buffers
//		bzero( strLine, 255 );
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pFile );

		//  if less then 3 skip
		if( wxStrlen( strLine ) < 3 ) continue; 

//		wxString strWxLine(strLine,wxConvUTF8);

		// HARDWARE :: type/name
		if( reHardwareName.Matches( strLine ) )
		{
			// get type and add by type
			nHardwareType = atoi( reHardwareName.GetMatch( strLine, 1 ).Trim(0).Trim(1).ToAscii() );
			// add by hardware type
			if( nHardwareType == HARDWARE_TYPE_CAMERA )
			{
				DefCamera cam;
				//memset( &cam, 0, sizeof(DefCamera) );
				wxString strName = reHardwareName.GetMatch( strLine, 2 ).Trim(0).Trim(1);
				cam.name = strName;
				//cam.h_type = (unsigned char) nHardwareType;
				nCamId = m_pHardware->AddCamera( cam );

			} else if( nHardwareType == HARDWARE_TYPE_TELESCOPE )
			{
				DefTelescope tel;
				//memset( &tel, 0, sizeof(DefTelescope) );
				wxString strName = reHardwareName.GetMatch( strLine, 2 ).Trim(0).Trim(1);
				tel.name = strName;
				//tel.h_type = (unsigned char) nHardwareType;
				nTelId = m_pHardware->AddTelescope( tel );

			} else if( nHardwareType == HARDWARE_TYPE_CAMERA_LENS )
			{
				DefCameraLens cam_lens;
				//memset( &cam_lens, 0, sizeof(DefCameraLens) );
				wxString strName = reHardwareName.GetMatch( strLine, 2 ).Trim(0).Trim(1);
				cam_lens.name = strName;
				//cam_lens.h_type = (unsigned char) nHardwareType;
				nCamLensId = m_pHardware->AddCameraLens( cam_lens );

			} else if( nHardwareType == HARDWARE_TYPE_BARLOW_LENS )
			{
				DefBarlowLens barlow_lens;
				//memset( &barlow_lens, 0, sizeof(DefBarlowLens) );
				wxString strName = reHardwareName.GetMatch( strLine, 2 ).Trim(0).Trim(1);
				barlow_lens.name = strName;
				//barlow_lens.h_type = (unsigned char) nHardwareType;
				nBLensId = m_pHardware->AddBarlowLens( barlow_lens );

			} else if( nHardwareType == HARDWARE_TYPE_FOCAL_REDUCER )
			{
				DefFocalReducer focal_reducer;
				//memset( &focal_reducer, 0, sizeof(DefFocalReducer) );
				wxString strName = reHardwareName.GetMatch( strLine, 2 ).Trim(0).Trim(1);
				focal_reducer.name = strName;
				//barlow_lens.h_type = (unsigned char) nHardwareType;
				nFRedId = m_pHardware->AddFocalReducer( focal_reducer );

			} else if( nHardwareType == HARDWARE_TYPE_EYEPIECE )
			{
				DefEyepiece eyepiece;
				//memset( &eyepiece, 0, sizeof(DefEyepiece) );
				wxString strName = reHardwareName.GetMatch( strLine, 2 ).Trim(0).Trim(1);
				eyepiece.name = strName;
				//eyepiece.h_type = (unsigned char) nHardwareType;
				nEyepiece = m_pHardware->AddEyepiece( eyepiece );

			} else if( nHardwareType == HARDWARE_TYPE_LMOUNT )
			{
				DefLMount mount;
				//memset( &mount, 0, sizeof(DefLMount) );
				wxString strName = reHardwareName.GetMatch( strLine, 2 ).Trim(0).Trim(1);
				mount.name = strName;
				//mount.type = (unsigned char) ???nHardwareType;
				nMount = m_pHardware->AddLMount( mount );

			} else if( nHardwareType == HARDWARE_TYPE_FOCUSER )
			{
				DefFocuser focuser;
				//memset( &focuser, 0, sizeof(DefFocuser) );
				wxString strName = reHardwareName.GetMatch( strLine, 2 ).Trim(0).Trim(1);
				focuser.name = strName;
				//focuser.type = (unsigned char) ???nHardwareType;
				nFocuser = m_pHardware->AddFocuser( focuser );

			} else if( nHardwareType == HARDWARE_TYPE_TFILTER )
			{
				DefTFilter filter;
				//memset( &filter, 0, sizeof(DefTFilter) );
				wxString strName = reHardwareName.GetMatch( strLine, 2 ).Trim(0).Trim(1);
				filter.name = strName;
				//filter.type = (unsigned char) ???nHardwareType;
				nTFilter = m_pHardware->AddTFilter( filter );
			}

		///////////////
		// :: CAMERA
		} else if( nHardwareType == HARDWARE_TYPE_CAMERA )
		{
			if( wxStrncmp( strLine, wxT("Type="), 5 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("Type=%u\n"), &(m_pHardware->m_vectCamera[nCamId].type) ) )
						m_pHardware->m_vectCamera[nCamId].type = 0;

			} else if( reBrand.Matches( strLine ) )
			{
				m_pHardware->m_vectCamera[nCamId].brand = reBrand.GetMatch( strLine, 1 ).Trim(0).Trim(1);

			} else if( reSensorWidth.Matches( strLine ) )
			{
				if( !reSensorWidth.GetMatch( strLine, 1 ).Trim(0).Trim(1).ToDouble( &(m_pHardware->m_vectCamera[nCamId].sensor_width) ) )
					m_pHardware->m_vectCamera[nCamId].sensor_width = 0.0;

			} else if( reSensorHeight.Matches( strLine ) )
			{
				if( !reSensorHeight.GetMatch( strLine, 1 ).Trim(0).Trim(1).ToDouble( &(m_pHardware->m_vectCamera[nCamId].sensor_height) ) )
					m_pHardware->m_vectCamera[nCamId].sensor_height = 0.0;

			} else if( reSensorPixelSize.Matches( strLine ) )
			{
				if( !reSensorPixelSize.GetMatch( strLine, 1 ).Trim(0).Trim(1).ToDouble( &(m_pHardware->m_vectCamera[nCamId].sensor_psize) ) )
					m_pHardware->m_vectCamera[nCamId].sensor_psize = 0.0;
			}

		///////////////
		// :: TELESCOPE
		} else if( nHardwareType == HARDWARE_TYPE_TELESCOPE )
		{
			if( wxStrncmp( strLine, wxT("Type="), 5 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("Type=%u\n"), &(m_pHardware->m_vectTelescope[nTelId].type) ) )
						m_pHardware->m_vectTelescope[nTelId].type = 0;

			} else if( reBrand.Matches( strLine ) )
			{
				m_pHardware->m_vectTelescope[nTelId].brand = reBrand.GetMatch( strLine, 1 ).Trim(0).Trim(1);

			} else if( reFocalLength.Matches( strLine ) )
			{
				if( !reFocalLength.GetMatch( strLine, 1 ).Trim(0).Trim(1).ToDouble( &(m_pHardware->m_vectTelescope[nTelId].focal_length) ) )
					m_pHardware->m_vectTelescope[nTelId].focal_length = 0.0;
	
			} else if( reClearAperture.Matches( strLine ) )
			{
				if( !reClearAperture.GetMatch( strLine, 1 ).Trim(0).Trim(1).ToDouble( &(m_pHardware->m_vectTelescope[nTelId].aperture) ) )
					m_pHardware->m_vectTelescope[nTelId].aperture = 0.0;

			} else if( reController.Matches( strLine ) )
			{
				m_pHardware->m_vectTelescope[nTelId].controller = reController.GetMatch( strLine, 1 ).Trim(0).Trim(1);
			}

		///////////////
		// :: CAMERA LENS
		} else if( nHardwareType == HARDWARE_TYPE_CAMERA_LENS )
		{
			if( wxStrncmp( strLine, wxT("Type="), 5 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("Type=%u\n"), &( m_pHardware->m_vectCameraLens[nCamLensId].type) ) )
						 m_pHardware->m_vectCameraLens[nCamLensId].type = 0;

			} else if( reBrand.Matches( strLine ) )
			{
				m_pHardware->m_vectCameraLens[nCamLensId].brand = reBrand.GetMatch( strLine, 1 ).Trim(0).Trim(1);

			} else if( reFocalLength.Matches( strLine ) )
			{
				if( !reFocalLength.GetMatch( strLine, 1 ).Trim(0).Trim(1).ToDouble( &(m_pHardware->m_vectCameraLens[nCamLensId].focal_length) ) )
					m_pHardware->m_vectCameraLens[nCamLensId].focal_length = 0.0;

			} else if( reAperture.Matches( strLine ) )
			{
				if( !reAperture.GetMatch( strLine, 1 ).Trim(0).Trim(1).ToDouble( &(m_pHardware->m_vectCameraLens[nCamLensId].aperture) ) )
					m_pHardware->m_vectCameraLens[nCamLensId].aperture = 0.0;

			} else if( reViewAngle.Matches( strLine ) )
			{
				if( !reViewAngle.GetMatch( strLine, 1 ).Trim(0).Trim(1).ToDouble( &(m_pHardware->m_vectCameraLens[nCamLensId].view_angle) ) )
					m_pHardware->m_vectCameraLens[nCamLensId].view_angle = 0.0;
			}

		////////////////////
		// :: BARLLOW LENS
		} else if( nHardwareType == HARDWARE_TYPE_BARLOW_LENS )
		{
			if( reBrand.Matches( strLine ) )
			{
				m_pHardware->m_vectBarlowLens[nBLensId].brand = reBrand.GetMatch( strLine, 1 ).Trim(0).Trim(1);
			
			} else if( reMagnification.Matches( strLine ) )
			{
				if( !reMagnification.GetMatch( strLine, 1 ).Trim(0).Trim(1).ToDouble( &(m_pHardware->m_vectBarlowLens[nBLensId].magnification) ) )
					m_pHardware->m_vectBarlowLens[nBLensId].magnification = 0.0;
			
			} else if( reElements.Matches( strLine ) )
			{
				if( !reElements.GetMatch( strLine, 1 ).Trim(0).Trim(1).ToLong( &nVarLong ) )
					m_pHardware->m_vectBarlowLens[nBLensId].elements = 0;
				else
					m_pHardware->m_vectBarlowLens[nBLensId].elements = (int) nVarLong;
			
			} else if( reBarrel.Matches( strLine ) )
			{
				if( !reBarrel.GetMatch( strLine, 1 ).Trim(0).Trim(1).ToDouble( &(m_pHardware->m_vectBarlowLens[nBLensId].barrel) ) )
					m_pHardware->m_vectBarlowLens[nBLensId].barrel = 0.0;
			}

		////////////////////
		// :: FOCAL REDUCER
		} else if( nHardwareType == HARDWARE_TYPE_FOCAL_REDUCER )
		{
			if( reBrand.Matches( strLine ) )
			{
				m_pHardware->m_vectFocalReducer[nFRedId].brand = reBrand.GetMatch( strLine, 1 ).Trim(0).Trim(1);
			
			} else if( reFocalRatio.Matches( strLine ) )
			{
				if( !reFocalRatio.GetMatch( strLine, 1 ).Trim(0).Trim(1).ToDouble( &(m_pHardware->m_vectFocalReducer[nFRedId].focal_ratio) ) )
					m_pHardware->m_vectFocalReducer[nFRedId].focal_ratio = 0.0;
			
			} else if( reElements.Matches( strLine ) )
			{
				if( !reElements.GetMatch( strLine, 1 ).Trim(0).Trim(1).ToLong( &nVarLong ) )
					m_pHardware->m_vectFocalReducer[nFRedId].elements = 0;
				else
					m_pHardware->m_vectFocalReducer[nFRedId].elements = (int) nVarLong;
			
			} else if( reBarrel.Matches( strLine ) )
			{
				if( !reBarrel.GetMatch( strLine, 1 ).Trim(0).Trim(1).ToDouble( &(m_pHardware->m_vectFocalReducer[nFRedId].barrel) ) )
					m_pHardware->m_vectFocalReducer[nFRedId].barrel = 0.0;
			}

		////////////////////
		// :: EYEPIECE
		} else if( nHardwareType == HARDWARE_TYPE_EYEPIECE )
		{
			if( wxStrncmp( strLine, wxT("Type="), 5 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("Type=%u\n"), &(m_pHardware->m_vectEyepiece[nEyepiece].type) ) )
						m_pHardware->m_vectEyepiece[nEyepiece].type = 0;

			} else if( reBrand.Matches( strLine ) )
			{
				m_pHardware->m_vectEyepiece[nEyepiece].brand = reBrand.GetMatch( strLine, 1 ).Trim(0).Trim(1);
			
			} else if( reFocalLength.Matches( strLine ) )
			{
				if( !reFocalLength.GetMatch( strLine, 1 ).Trim(0).Trim(1).ToDouble( &(m_pHardware->m_vectEyepiece[nEyepiece].focal_length) ) )
					m_pHardware->m_vectEyepiece[nEyepiece].focal_length = 0.0;
			
			} else if( reElements.Matches( strLine ) )
			{
				if( !reElements.GetMatch( strLine, 1 ).Trim(0).Trim(1).ToLong( &nVarLong ) )
					m_pHardware->m_vectEyepiece[nEyepiece].elements = 0;
				else
					m_pHardware->m_vectEyepiece[nEyepiece].elements = (int) nVarLong;
			
			} else if( reBarrel.Matches( strLine ) )
			{
				if( !reBarrel.GetMatch( strLine, 1 ).Trim(0).Trim(1).ToDouble( &(m_pHardware->m_vectEyepiece[nEyepiece].barrel) ) )
					m_pHardware->m_vectEyepiece[nEyepiece].barrel = 0.0;
			}

		////////////////////
		// :: MOUNT
		} else if( nHardwareType == HARDWARE_TYPE_LMOUNT )
		{
			if( wxStrncmp( strLine, wxT("Type="), 5 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("Type=%u\n"), &(m_pHardware->m_vectLMount[nMount].type) ) )
						m_pHardware->m_vectLMount[nMount].type = 0;

			} else if( reBrand.Matches( strLine ) )
			{
				m_pHardware->m_vectLMount[nMount].brand = reBrand.GetMatch( strLine, 1 ).Trim(0).Trim(1);
			
			} else if( reController.Matches( strLine ) )
			{
				m_pHardware->m_vectLMount[nMount].controller = reController.GetMatch( strLine, 1 ).Trim(0).Trim(1);			

			} else if( reCapacity.Matches( strLine ) )
			{
				if( !reCapacity.GetMatch( strLine, 1 ).Trim(0).Trim(1).ToDouble( &(m_pHardware->m_vectLMount[nMount].capacity) ) )
					m_pHardware->m_vectLMount[nMount].capacity = 0.0;
		
			} else if( rePEM.Matches( strLine ) )
			{
				if( !rePEM.GetMatch( strLine, 1 ).Trim(0).Trim(1).ToDouble( &(m_pHardware->m_vectLMount[nMount].pem) ) )
					m_pHardware->m_vectLMount[nMount].pem = 0.0;
			}

		////////////////////
		// :: FOCUSER
		} else if( nHardwareType == HARDWARE_TYPE_FOCUSER )
		{
			if( wxStrncmp( strLine, wxT("Type="), 5 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("Type=%u\n"), &(m_pHardware->m_vectFocuser[nFocuser].type) ) )
						m_pHardware->m_vectFocuser[nFocuser].type = 0;

			} else if( reBrand.Matches( strLine ) )
			{
				m_pHardware->m_vectFocuser[nFocuser].brand = reBrand.GetMatch( strLine, 1 ).Trim(0).Trim(1);
			
			} else if( reController.Matches( strLine ) )
			{
				m_pHardware->m_vectFocuser[nFocuser].controller = reController.GetMatch( strLine, 1 ).Trim(0).Trim(1);			

			} else if( reDrawtube.Matches( strLine ) )
			{
				if( !reDrawtube.GetMatch( strLine, 1 ).Trim(0).Trim(1).ToDouble( &(m_pHardware->m_vectFocuser[nFocuser].drawtube) ) )
					m_pHardware->m_vectFocuser[nFocuser].drawtube = 0.0;
		
			} else if( reWeight.Matches( strLine ) )
			{
				if( !reWeight.GetMatch( strLine, 1 ).Trim(0).Trim(1).ToDouble( &(m_pHardware->m_vectFocuser[nFocuser].weight) ) )
					m_pHardware->m_vectFocuser[nFocuser].weight = 0.0;
			}

		////////////////////
		// :: TELESCOPE FILTER
		} else if( nHardwareType == HARDWARE_TYPE_TFILTER )
		{
			if( wxStrncmp( strLine, wxT("Type="), 5 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("Type=%u\n"), &(m_pHardware->m_vectTFilter[nTFilter].type) ) )
						m_pHardware->m_vectTFilter[nTFilter].type = 0;

			} else if( reBrand.Matches( strLine ) )
			{
				m_pHardware->m_vectTFilter[nTFilter].brand = reBrand.GetMatch( strLine, 1 ).Trim(0).Trim(1);
			
			} else if( reBandpass.Matches( strLine ) )
			{
				m_pHardware->m_vectTFilter[nTFilter].band_pass = reBandpass.GetMatch( strLine, 1 ).Trim(0).Trim(1);			

			} else if( reTransmission.Matches( strLine ) )
			{
				if( !reTransmission.GetMatch( strLine, 1 ).Trim(0).Trim(1).ToDouble( &(m_pHardware->m_vectTFilter[nTFilter].transmission) ) )
					m_pHardware->m_vectTFilter[nTFilter].transmission = 0.0;
		
			} else if( reClearAperture.Matches( strLine ) )
			{
				if( !reClearAperture.GetMatch( strLine, 1 ).Trim(0).Trim(1).ToDouble( &(m_pHardware->m_vectTFilter[nTFilter].clear_aperture) ) )
					m_pHardware->m_vectTFilter[nTFilter].clear_aperture = 0.0;
			}

		}
	}
	// close my file
	fclose( pFile );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CObserver::SaveHardware( ) 
{
	FILE* pFile = NULL;
	int i=0;

	// open local file to write
	pFile = wxFopen( wxT(OBSERVER_CONFIG_HARDWARE_FILE), wxT("w") );
	if( !pFile ) return( 0 );

	/////////////////////////////
	// Save CAMERAS
	for( i=0; i<m_pHardware->m_vectCamera.size(); i++ )
	{
		wxFprintf( pFile, wxT("\n(%d)[%s]\n"), HARDWARE_TYPE_CAMERA, m_pHardware->m_vectCamera[i].name );
		wxFprintf( pFile, wxT("Type=%u\n"), m_pHardware->m_vectCamera[i].type );
		wxFprintf( pFile, wxT("Brand=%s\n"), m_pHardware->m_vectCamera[i].brand );
		wxFprintf( pFile, wxT("SensorWidth=%.4lf\n"), m_pHardware->m_vectCamera[i].sensor_width );
		wxFprintf( pFile, wxT("SensorHeight=%.4lf\n"), m_pHardware->m_vectCamera[i].sensor_height );
		wxFprintf( pFile, wxT("SensorPixelSize=%.4lf\n"), m_pHardware->m_vectCamera[i].sensor_psize );
	}

	/////////////////////////////
	// Save TELESCOPES
	for( i=0; i<m_pHardware->m_vectTelescope.size(); i++ )
	{
		wxFprintf( pFile, wxT("\n(%d)[%s]\n"), HARDWARE_TYPE_TELESCOPE, m_pHardware->m_vectTelescope[i].name );
		wxFprintf( pFile, wxT("Type=%u\n"), m_pHardware->m_vectTelescope[i].type );
		wxFprintf( pFile, wxT("Brand=%s\n"), m_pHardware->m_vectTelescope[i].brand );
		wxFprintf( pFile, wxT("FocalLength=%.4lf\n"), m_pHardware->m_vectTelescope[i].focal_length );
		wxFprintf( pFile, wxT("ClearAperture=%.4lf\n"), m_pHardware->m_vectTelescope[i].aperture );
		wxFprintf( pFile, wxT("Controller=%s\n"), m_pHardware->m_vectTelescope[i].controller );

	}

	/////////////////////////////
	// Save CAMERA LENSES
	for( i=0; i<m_pHardware->m_vectCameraLens.size(); i++ )
	{
		wxFprintf( pFile, wxT("\n(%d)[%s]\n"), HARDWARE_TYPE_CAMERA_LENS, m_pHardware->m_vectCameraLens[i].name );
		wxFprintf( pFile, wxT("Type=%u\n"), m_pHardware->m_vectCameraLens[i].type );
		wxFprintf( pFile, wxT("Brand=%s\n"), m_pHardware->m_vectCameraLens[i].brand );
		wxFprintf( pFile, wxT("FocalLength=%.4lf\n"), m_pHardware->m_vectCameraLens[i].focal_length );
		wxFprintf( pFile, wxT("Aperture=%.4lf\n"), m_pHardware->m_vectCameraLens[i].aperture );
		wxFprintf( pFile, wxT("ViewAngle=%.4lf\n"), m_pHardware->m_vectCameraLens[i].view_angle );
	}

	/////////////////////////////
	// Save BARLOW LENSES
	for( i=0; i<m_pHardware->m_vectBarlowLens.size(); i++ )
	{
		wxFprintf( pFile, wxT("\n(%d)[%s]\n"), HARDWARE_TYPE_BARLOW_LENS, m_pHardware->m_vectBarlowLens[i].name );
		wxFprintf( pFile, wxT("Brand=%s\n"), m_pHardware->m_vectBarlowLens[i].brand );
		wxFprintf( pFile, wxT("Magnification=%.4lf\n"), m_pHardware->m_vectBarlowLens[i].magnification );
		wxFprintf( pFile, wxT("Elements=%d\n"), m_pHardware->m_vectBarlowLens[i].elements );
		wxFprintf( pFile, wxT("Barrel=%.4lf\n"), m_pHardware->m_vectBarlowLens[i].barrel );
	}

	/////////////////////////////
	// Save FOCAL REDUCER
	for( i=0; i<m_pHardware->m_vectFocalReducer.size(); i++ )
	{
		wxFprintf( pFile, wxT("\n(%d)[%s]\n"), HARDWARE_TYPE_FOCAL_REDUCER, m_pHardware->m_vectFocalReducer[i].name );
		wxFprintf( pFile, wxT("Brand=%s\n"), m_pHardware->m_vectFocalReducer[i].brand );
		wxFprintf( pFile, wxT("FocalRatio=%.4lf\n"), m_pHardware->m_vectFocalReducer[i].focal_ratio );
		wxFprintf( pFile, wxT("Elements=%d\n"), m_pHardware->m_vectFocalReducer[i].elements );
		wxFprintf( pFile, wxT("Barrel=%.4lf\n"), m_pHardware->m_vectFocalReducer[i].barrel );
	}

	/////////////////////////////
	// Save EYEPIECE
	for( i=0; i<m_pHardware->m_vectEyepiece.size(); i++ )
	{
		wxFprintf( pFile, wxT("\n(%d)[%s]\n"), HARDWARE_TYPE_EYEPIECE, m_pHardware->m_vectEyepiece[i].name );
		wxFprintf( pFile, wxT("Type=%u\n"), m_pHardware->m_vectEyepiece[i].type );
		wxFprintf( pFile, wxT("Brand=%s\n"), m_pHardware->m_vectEyepiece[i].brand );
		wxFprintf( pFile, wxT("FocalLength=%.4lf\n"), m_pHardware->m_vectEyepiece[i].focal_length );
		wxFprintf( pFile, wxT("Elements=%d\n"), m_pHardware->m_vectEyepiece[i].elements );
		wxFprintf( pFile, wxT("Barrel=%.4lf\n"), m_pHardware->m_vectEyepiece[i].barrel );
	}

	/////////////////////////////
	// Save LMOUNTS
	for( i=0; i<m_pHardware->m_vectLMount.size(); i++ )
	{
		wxFprintf( pFile, wxT("\n(%d)[%s]\n"), HARDWARE_TYPE_LMOUNT, m_pHardware->m_vectLMount[i].name );
		wxFprintf( pFile, wxT("Type=%u\n"), m_pHardware->m_vectLMount[i].type );
		wxFprintf( pFile, wxT("Brand=%s\n"), m_pHardware->m_vectLMount[i].brand );
		wxFprintf( pFile, wxT("Controller=%s\n"), m_pHardware->m_vectLMount[i].controller );
		wxFprintf( pFile, wxT("Capacity=%.4lf\n"), m_pHardware->m_vectLMount[i].capacity );
		wxFprintf( pFile, wxT("PEM=%.4lf\n"), m_pHardware->m_vectLMount[i].pem );
	}

	/////////////////////////////
	// Save FOCUSER
	for( i=0; i<m_pHardware->m_vectFocuser.size(); i++ )
	{
		wxFprintf( pFile, wxT("\n(%d)[%s]\n"), HARDWARE_TYPE_FOCUSER, m_pHardware->m_vectFocuser[i].name );
		wxFprintf( pFile, wxT("Type=%u\n"), m_pHardware->m_vectFocuser[i].type );
		wxFprintf( pFile, wxT("Brand=%s\n"), m_pHardware->m_vectFocuser[i].brand );
		wxFprintf( pFile, wxT("Controller=%s\n"), m_pHardware->m_vectFocuser[i].controller );
		wxFprintf( pFile, wxT("Drawtube=%.4lf\n"), m_pHardware->m_vectFocuser[i].drawtube );
		wxFprintf( pFile, wxT("Weight=%.4lf\n"), m_pHardware->m_vectFocuser[i].weight );
	}

	/////////////////////////////
	// Save TELESCOPE FILTERS
	for( i=0; i<m_pHardware->m_vectTFilter.size(); i++ )
	{
		wxFprintf( pFile, wxT("\n(%d)[%s]\n"), HARDWARE_TYPE_TFILTER, m_pHardware->m_vectTFilter[i].name );
		wxFprintf( pFile, wxT("Type=%u\n"), m_pHardware->m_vectTFilter[i].type );
		wxFprintf( pFile, wxT("Brand=%s\n"), m_pHardware->m_vectTFilter[i].brand );
		wxFprintf( pFile, wxT("Bandpass=%s\n"), m_pHardware->m_vectTFilter[i].band_pass );
		wxFprintf( pFile, wxT("ClearAperture=%.4lf\n"), m_pHardware->m_vectTFilter[i].clear_aperture );
		wxFprintf( pFile, wxT("Transmission=%.4lf\n"), m_pHardware->m_vectTFilter[i].transmission );
	}

	// close my file
	fclose( pFile );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CObserver::LoadSetups( ) 
{
	FILE* pFile = NULL;
	wxChar strLine[2000];
	int nSetupId=0;
	// regex
	wxRegEx reSetupName = wxT( "^\\[(.+)\\]" );
	wxRegEx reType = wxT( "^Type=(.*)" );
	wxRegEx reCamera = wxT( "^Camera=(.*)" );
	wxRegEx reTelescope = wxT( "^Telescope=(.*)" );
	wxRegEx reMount = wxT( "^Mount=(.*)" );
	wxRegEx reFilter = wxT( "^Filter=(.*)" );
	wxRegEx reFocuser = wxT( "^Focuser=(.*)" );

	// open local file to read from
	pFile = wxFopen( wxT(OBSERVER_CONFIG_SETUPS_FILE), wxT("r") );
	if( !pFile ) return( 0 );

	while( !feof( pFile ) )
	{
		// clear buffers
//		bzero( strLine, 255 );
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pFile );

		//  if less then 3 skip
		if( wxStrlen( strLine ) < 3 ) continue; 

//		wxString strWxLine(strLine,wxConvUTF8);

		// PROFILE :: check which pattern
		if( reSetupName.Matches( strLine ) )
		{
			DefHarwareSetup setup;
			wxString strName = reSetupName.GetMatch( strLine, 1 ).Trim(0).Trim(1);
			setup.name = strName;
			nSetupId = m_pHardware->AddSetup( setup );

		} else if( reType.Matches( strLine ) )
			m_pHardware->m_vectSetup[nSetupId].type = wxAtoi( reType.GetMatch( strLine, 1 ).Trim(0).Trim(1) );
		else if( reTelescope.Matches( strLine ) )
			m_pHardware->m_vectSetup[nSetupId].telescope = wxAtoi( reTelescope.GetMatch( strLine, 1 ).Trim(0).Trim(1) );
		else if( reCamera.Matches( strLine ) )
			m_pHardware->m_vectSetup[nSetupId].camera = wxAtoi( reCamera.GetMatch( strLine, 1 ).Trim(0).Trim(1) );
		else if( reMount.Matches( strLine ) )
			m_pHardware->m_vectSetup[nSetupId].mount = wxAtoi( reMount.GetMatch( strLine, 1 ).Trim(0).Trim(1) );
		else if( reFilter.Matches( strLine ) )
			m_pHardware->m_vectSetup[nSetupId].filter = wxAtoi( reFilter.GetMatch( strLine, 1 ).Trim(0).Trim(1) );
		else if( reFocuser.Matches( strLine ) )
			m_pHardware->m_vectSetup[nSetupId].filter = wxAtoi( reFocuser.GetMatch( strLine, 1 ).Trim(0).Trim(1) );
	}
	// close my file
	fclose( pFile );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CObserver::SaveSetups( ) 
{
	FILE* pFile = NULL;
	int i=0;

	// open local file to write
	pFile = wxFopen( wxT(OBSERVER_CONFIG_SETUPS_FILE), wxT("w") );
	if( !pFile ) return( 0 );

	/////////////////////////////
	// Save SETUPS
	for( i=0; i<m_pHardware->m_vectSetup.size(); i++ )
	{
		wxFprintf( pFile, wxT("\n[%s]\n"), m_pHardware->m_vectSetup[i].name );
		wxFprintf( pFile, wxT("Type=%d\n"), m_pHardware->m_vectSetup[i].type );
		wxFprintf( pFile, wxT("Camera=%d\n"), m_pHardware->m_vectSetup[i].camera );
		wxFprintf( pFile, wxT("Telescope=%d\n"), m_pHardware->m_vectSetup[i].telescope );
		wxFprintf( pFile, wxT("Mount=%d\n"), m_pHardware->m_vectSetup[i].mount );
		wxFprintf( pFile, wxT("Filter=%d\n"), m_pHardware->m_vectSetup[i].filter );
		wxFprintf( pFile, wxT("Focuser=%d\n"), m_pHardware->m_vectSetup[i].focuser );
	}

	// close my file
	fclose( pFile );

	return( 1 );
}
