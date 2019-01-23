
#ifndef _OBSERVER_CFG_DLG_H
#define _OBSERVER_CFG_DLG_H

#include "wx/wxprec.h"
#include <wx/datectrl.h>
#include <wx/notebook.h>

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// gui ids
enum
{
	// profile
	wxID_PROFILE_BIRTHDAY			= 11100,
	wxID_PROFILE_PHOTO_SET_BUTTON,
	// address
	wxID_OBSERVER_CFG_CONTACT_MAP,
	wxID_OBSERVER_CFG_CONTACT_GEOIP,
	wxID_OBSERVER_CFG_CONTACT_GPS,
	wxID_OBSERVER_CFG_CITY,
	wxID_OBSERVER_CFG_PROVINCE,
	wxID_OBSERVER_CFG_COUNTRY,
	wxID_OBSERVER_CFG_LATITUDE,
	wxID_OBSERVER_CFG_LONGITUDE,
	wxID_OBSERVER_CFG_ALTITUDE,
	// contact
	wxID_PROFILE_CONTACT_MESSENGER_SELECT,
	// sites
	wxID_OBSERVER_CFG_SITES_NAME_SELECT,
	wxID_OBSERVER_CFG_SITES_NAME_ADD,
	wxID_OBSERVER_CFG_SITES_NAME_DELETE,
	wxID_OBSERVER_CFG_SITES_NAME_RENAME,
	wxID_OBSERVER_CFG_SITE_CITY,
	wxID_OBSERVER_CFG_SITE_PROVINCE,
	wxID_OBSERVER_CFG_SITE_COUNTRY,
	wxID_OBSERVER_CFG_SITE_LATITUDE,
	wxID_OBSERVER_CFG_SITE_LONGITUDE,
	wxID_OBSERVER_CFG_SITE_ALTITUDE,
	wxID_OBSERVER_CFG_SITE_NOTES,
	wxID_OBSERVER_CFG_SITE_MAP,
	wxID_OBSERVER_CFG_SITE_GEOIP,
	wxID_OBSERVER_CFG_SITE_GPS,
	// hardware
	wxID_OBSERVER_CFG_HARDWARE_NAME_TYPE,
	wxID_OBSERVER_CFG_HARDWARE_NAME_SELECT,
	wxID_OBSERVER_CFG_HARDWARE_NAME_ADD,
	wxID_OBSERVER_CFG_HARDWARE_NAME_DELETE,
	// setups
	wxID_OBSERVER_CFG_SETUPS_NAME_SELECT,
	wxID_OBSERVER_CFG_SETUPS_NAME_ADD,
	wxID_OBSERVER_CFG_SETUPS_NAME_DELETE,
	wxID_OBSERVER_CFG_SETUP_NAME_RENAME
};

// defines
#define DEFAULT_LOAD_IMAGE_FILE_TYPE "All Images (*.jpg;*.bmp;*.tif;*.png;*.tga;*.gif)|*.jpg;*.bmp;*.tif;*.png;*.tga;*.gif|JPEG (*.jpg)|*.jpg|BPM (*.bmp)|*.bmp|TIFF (*.tif)|*.tif|GIF (*.gif)|*.gif|TGA (*.tga)|*.tga|PNG (*.png)|*.png"
#define DEFAULT_OBSERVER_SITE_NAME "Default"

// local headers
#include "../land/earth.h"
#include "../observer/observer.h"
#include "../camera/camera.h"
#include "../telescope/telescope.h"

// external classes
class CUnimapWorker;

// class:	CObserverCfgDlg
///////////////////////////////////////////////////////
class CObserverCfgDlg : public wxDialog
{
// public methods
public:
    CObserverCfgDlg( wxWindow *parent );
	virtual ~CObserverCfgDlg( );

	void SetConfig( CObserver* pObserver, CCamera* pCamera, CTelescope* pTelescope, CUnimapWorker* pUnimapWorker );

// public data
public:
	
	CObserver*		m_pObserver;
	CCamera*		m_pCamera;
	CTelescope*		m_pTelescope;
	CLocationEarth* m_pEarth;

	CUnimapWorker* m_pUnimapWorker;

	// :: profile
	wxTextCtrl*	m_pFirstName;
	wxTextCtrl*	m_pMidName;
	wxTextCtrl*	m_pLastName;
	wxTextCtrl*	m_pNickName;
	wxDatePickerCtrl* m_pBirthday;
	wxTextCtrl*	m_pProfession;
	wxTextCtrl*	m_pHobbies;
	wxTextCtrl*	m_pWebsite;
	// profile :: photo
	wxString m_strProfilePhotoFile;
	wxStaticBitmap* m_pProfilePhoto;
	wxButton*	m_pSetPhotoButton;

	// :: contact
	wxTextCtrl*	m_pCntAddrStreet;
	wxTextCtrl* m_pCntZipCode;
	wxChoice*	m_pCntCity;
	wxChoice*	m_pCntProvince;
	wxChoice*	m_pCntCountry;
	wxTextCtrl* m_pCntLatitude;
	wxTextCtrl* m_pCntLongitude;
	wxTextCtrl* m_pCntAltitude;
	// other
	wxTextCtrl*	m_pMobile;
	wxTextCtrl*	m_pPhone;
	wxTextCtrl*	m_pFax;
	wxTextCtrl*	m_pEmail;
	wxChoice*	m_pMessengerType;
	wxTextCtrl*	m_pMessenger;
	wxString	m_vectMessenger[10];
	int			m_nSelectedMessenger;

	// :: sites
	CObserverSite**	m_vectSite;
	int				m_nSite;
	int				m_nSelectSite;
	wxChoice*	m_pSiteNameSelect;
	wxButton*	m_pSiteNameButtonAdd;
	wxButton*	m_pSiteNameButtonDelete;
	wxChoice*	m_pSiteCity;
	wxChoice*	m_pSiteProvince;
	wxChoice*	m_pSiteCountry;
	wxTextCtrl* m_pSiteLatitude;
	wxTextCtrl* m_pSiteLongitude;
	wxTextCtrl* m_pSiteAltitude;
	wxTextCtrl* m_pSiteNotes;

	// :: hardware
	CObserverHardware*	m_pHardware;
//	int					m_nSelectHardware; ???
	wxFlexGridSizer* sizerHardwareTail;
	wxChoice*	m_pHardwareNameType;
	wxChoice*	m_pHardwareNameSelect;
	wxButton*	m_pHardwareNameButtonAdd;
	wxButton*	m_pHardwareNameButtonDelete;
	// :: hardware :: labels
	wxStaticText* m_pHardwareLabel1;
	wxStaticText* m_pHardwareLabel2;
	wxStaticText* m_pHardwareLabel3;
	wxStaticText* m_pHardwareLabel4;
	wxStaticText* m_pHardwareLabel5;
	// :: hardware :: free text entry fields
	wxTextCtrl* m_pHardwareEntry1;
	wxTextCtrl* m_pHardwareEntry2;
	wxTextCtrl* m_pHardwareEntry3;
	wxTextCtrl* m_pHardwareEntry4;
	// :: hardware :: select fields
	wxChoice*	m_pHardwareSelect1;

	// :: setup
	int			m_nSelectedSetup;
	wxChoice*	m_pSetupNameSelect;
	wxButton*	m_pSetupNameButtonAdd;
	wxButton*	m_pSetupNameButtonDelete;
	// properties
	wxChoice*	m_pSetupType;
	wxChoice*	m_pSetupTelescope;
	wxChoice*	m_pSetupMount;
	wxChoice*	m_pSetupCamera;
	wxChoice*	m_pSetupFilter;
	wxChoice*	m_pSetupFocuser;

	// :: defaults
	wxChoice*	m_pDefaultSite;
	wxChoice*	m_pDefaultSetup;
	wxChoice*	m_pDefaultTelescope;
	wxChoice*	m_pDefaultCamera;
	wxChoice*	m_pDefaultMount;
	wxChoice*	m_pDefaultFocuser;

// private methods
private:
	void SetLocByName( wxString& strCountry, wxString& strProvince, wxString& strCity, 
						wxChoice* pCountry, wxChoice* pProvince, wxChoice* pCity, int bCity );

	void OnProfileSetPhoto( wxCommandEvent& pEvent );
	void OnNotebookTab( wxNotebookEvent& pEvent );
	void OnSelectAddrLoc( wxCommandEvent& pEvent );
	void SetCity( int nType );
	void OnEartMapLocation( wxCommandEvent& pEvent );
	void OnGeoIpLocation( wxCommandEvent& pEvent );
	void OnSelectIMessType( wxCommandEvent& pEvent );
	// sites handlers
	void OnSelectSite( wxCommandEvent& pEvent );
	void OnSitesAdd( wxCommandEvent& pEvent );
	void OnSiteDelete( wxCommandEvent& pEvent );
	void OnSiteRename( wxCommandEvent& pEvent );
	void StoreSelectedSite( );
	// hardware handlers
	void ResetAllHardwareFields( );
	void OnSelectHardwareType( wxCommandEvent& pEvent );
	void OnSelectHardwareName( wxCommandEvent& pEvent );
	void SelectHardwareName( );
	void SelectHardwareType( int nType, int nSelect=0 );
	void OnHardwareAdd( wxCommandEvent& pEvent );
	void OnHardwareDelete( wxCommandEvent& pEvent );
	// setups handlers
	void OnSelectSetupName( wxCommandEvent& pEvent );
	void OnSetupAdd( wxCommandEvent& pEvent );
	void OnSetupDelete( wxCommandEvent& pEvent );
	void OnSetupRename( wxCommandEvent& pEvent );
	void GetSetupSelected( int nSetup=-1 );
	void StoreSelectedSetup( );
	// get defaults
	void GetDefaults( );

	// validator
	void OnValidate( wxCommandEvent& pEvent );

	DECLARE_EVENT_TABLE()
};

#endif


