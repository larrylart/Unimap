
#ifndef _OBSERVER_HARDWARE_H
#define _OBSERVER_HARDWARE_H

// wx headers
#include "wx/wxprec.h"

// hardware types enum
enum
{
	HARDWARE_TYPE_CAMERA=0,
	HARDWARE_TYPE_TELESCOPE,
	HARDWARE_TYPE_CAMERA_LENS,
	HARDWARE_TYPE_EYEPIECE,
	HARDWARE_TYPE_BARLOW_LENS,
	HARDWARE_TYPE_FOCAL_REDUCER,
	HARDWARE_TYPE_LMOUNT,
	HARDWARE_TYPE_FOCUSER,
	HARDWARE_TYPE_TFILTER
};

typedef struct 
{
	wxString type;
	unsigned char id;
} DefHarwareType;

// hardware design type 
typedef struct 
{
	wxString type_label;
	char type_string[255];
	unsigned char id;
	unsigned char has_type;

} DefHarwareDesignType;

static DefHarwareType g_vectHardwareTypes[10] =
{
	{ wxT("Camera"), HARDWARE_TYPE_CAMERA },
	{ wxT("Telescope"), HARDWARE_TYPE_TELESCOPE },
	{ wxT("Camera Lens"), HARDWARE_TYPE_CAMERA_LENS },
	{ wxT("Eye Piece"), HARDWARE_TYPE_EYEPIECE },
	{ wxT("Barlow Lens"), HARDWARE_TYPE_BARLOW_LENS },
	{ wxT("Focal Reducer"), HARDWARE_TYPE_FOCAL_REDUCER },
	{ wxT("Mount"), HARDWARE_TYPE_LMOUNT },
	{ wxT("Focuser"), HARDWARE_TYPE_FOCUSER },
	{ wxT("Filters"), HARDWARE_TYPE_TFILTER }
};

// structure type: setup
typedef struct 
{
	wxString name;

	int type;
	char type_other[255];
	// setup elements
	int camera;
	int telescope;
	int mount;
	int filter;
	int focuser;

} DefHarwareSetup;

// structure type: TLens setup
typedef struct 
{
	// main lens
	int nLensType;
	int nLensOptics;
	int nLensBrand;
	int nLensName;
	int nLensNameOwn;
	// barlow lens
	unsigned char bBarlow;
	int nBarlowLensBrand;
	int nBarlowLensName;
	int nBarlowLensNameOwn;
	// focal reducer
	unsigned char bFocalReducer;
	int nFocalReducerBrand;
	int nFocalReducerName;
	int nFocalReducerNameOwn;
	// eyepiece
	unsigned char bEyepiece;
	int nEyepieceType;
	int nEyepieceBrand;
	int nEyepieceName;
	int nEyepieceNameOwn;
	// camera lens
	unsigned char bCameraLens;
	int nCameraLensOptics;
	int nCameraLensBrand;
	int nCameraLensName;
	int nCameraLensNameOwn;

} DefTLensSetup;

// local includes
#include "../camera/camera.h"
#include "../telescope/telescope.h"

// external classes
class CObserverSite;
class CObserver;

// class:	CObserverHardware
///////////////////////////////////////////////////////
class CObserverHardware
{
// public methods
public:
	CObserverHardware( );
	~CObserverHardware( );

	void ResetAll( );
	void ClearAll( );

	void CopyTo( CObserverHardware* pDst );

	// hardware handle methods
	// :: camera
	int AddCamera( DefCamera& src );
	int DeleteCamera( int id );
	wxString GetCameraFullName( int nId ){ wxString strRet; strRet.Printf( wxT("%s %s"), m_vectCamera[nId].brand, m_vectCamera[nId].name ); return( strRet ); }
	// :: camera lens
	int AddCameraLens( DefCameraLens& src );
	int DeleteCameraLens( int id );
	int IsCameraLens( int id ){ if( id >=0 && id < m_vectCameraLens.size() ) return(1); else return(0); }
	wxString GetCameraLensFullName( int nId ){ wxString strRet; strRet.Printf( wxT("%s %s"), m_vectCameraLens[nId].brand, m_vectCameraLens[nId].name ); return( strRet ); }
	// :: telescope
	int AddTelescope( DefTelescope& src );
	int DeleteTelescope( int id );
	int IsTelescope( int id ){ if( id >=0 && id < m_vectTelescope.size() ) return(1); else return(0); }
	wxString GetTelescopeFullName( int nId ){ wxString strRet; strRet.Printf( wxT("%s %s"), m_vectTelescope[nId].brand, m_vectTelescope[nId].name ); return( strRet ); }
	// :: barlow lens
	int AddBarlowLens( DefBarlowLens& src );
	int DeleteBarlowLens( int id );
	wxString GetBarlowLensFullName( int nId ){ wxString strRet; strRet.Printf( wxT("%s %s"), m_vectBarlowLens[nId].brand, m_vectBarlowLens[nId].name ); return( strRet ); }
	// :: focal reducer
	int AddFocalReducer( DefFocalReducer& src );
	int DeleteFocalReducer( int id );
	wxString GetFocalReducerFullName( int nId ){ wxString strRet; strRet.Printf( wxT("%s %s"), m_vectFocalReducer[nId].brand, m_vectFocalReducer[nId].name ); return( strRet ); }
	// :: eyepiece
	int AddEyepiece( DefEyepiece& src );
	int DeleteEyepiece( int id );
	int IsEyepiece( int id ){ if( id >=0 && id < m_vectEyepiece.size() ) return(1); else return(0); }
	wxString GetEyepieceFullName( int nId ){ wxString strRet; strRet.Printf( wxT("%s %s"), m_vectEyepiece[nId].brand, m_vectEyepiece[nId].name ); return( strRet ); }
	// :: telescope/lens mount
	int AddLMount( DefLMount& src );
	int DeleteLMount( int id );
	wxString GetLMountFullName( int nId ){ wxString strRet; strRet.Printf( wxT("%s %s"), m_vectLMount[nId].brand, m_vectLMount[nId].name ); return( strRet ); }
	// :: focuser
	int AddFocuser( DefFocuser& src );
	int DeleteFocuser( int id );
	wxString GetFocuserFullName( int nId ){ wxString strRet; strRet.Printf( wxT("%s %s"), m_vectFocuser[nId].brand, m_vectFocuser[nId].name ); return( strRet ); }
	// :: telescope filter
	int AddTFilter( DefTFilter& src );
	int DeleteTFilter( int id );
	wxString GetTFilterFullName( int nId ){ wxString strRet; strRet.Printf( wxT("%s %s"), m_vectTFilter[nId].brand, m_vectTFilter[nId].name ); return( strRet ); }

	// SETUPS
	int AddSetup( DefHarwareSetup& src );
	int DeleteSetup( int id );

// public data
public:

	///////////////////
	// camera - struct: type, ccd type, lens?
	std::vector<DefCamera> m_vectCamera;
//	int m_nCamera;
//	int m_nCameraAllocated;

	////////////////////////
	// telescope - struct , type, focal, aperture, optics etc
	std::vector<DefTelescope> m_vectTelescope;
//	int m_nTelescope;
//	int m_nTelescopeAllocated;

	////////////////////////
	// camera lens: focal, aperture, lens type, distortion
	std::vector<DefCameraLens> m_vectCameraLens;
//	int m_nCameraLens;
//	int m_nCameraLensAllocated;

	///////////////////////////
	// eye piece: this should be a struct name, mag factor, angle of view etc
	std::vector<DefEyepiece> m_vectEyepiece;
//	int m_nEyepiece;
//	int m_nEyepieceAllocated;

	std::vector<DefBarlowLens> m_vectBarlowLens;
//	int m_nBarlowLens;
//	int m_nBarlowLensAllocated;

	std::vector<DefFocalReducer> m_vectFocalReducer;
//	int m_nFocalReducer;
//	int m_nFocalReducerAllocated;

	/////////////////
	// telescope filter
	std::vector<DefTFilter> m_vectTFilter;
//	int m_nTFilter;
//	int m_nTFilterAllocated;

	/////////////////
	// mount - struct
	std::vector<DefLMount> m_vectLMount;
//	int m_nLMount;
//	int m_nLMountAllocated;

	/////////////////
	// telescope focuser
	std::vector<DefFocuser> m_vectFocuser;
//	int m_nFocuser;
//	int m_nFocuserAllocated;

	///////////////////////////////////////////////////////
	// DEFINE SETUPS USED - as a cobination of the stuff above
	std::vector<DefHarwareSetup>	m_vectSetup;
//	int					m_nSetup;
//	int					m_nSetupAllocated;
};

#endif