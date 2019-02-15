////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _TELESCOPE_H
#define _TELESCOPE_H

// system includes
#include <vector>
// wx
#include "wx/wxprec.h"

// local headers
#include "../util/folders.h"

// defines :: ini files
#define DEFAULT_TELESCOPE_BRAND_DEF_FILE		unMakeAppPath(wxT("data/hardware/telescope/telescope_brands.ini"))
#define DEFAULT_EYEPIECE_BRAND_DEF_FILE			unMakeAppPath(wxT("data/hardware/eyepiece/eyepiece_brands.ini"))
#define DEFAULT_BARLOW_LENS_BRAND_DEF_FILE		unMakeAppPath(wxT("data/hardware/barlow_lens/barlow_lens_brands.ini"))
#define DEFAULT_FOCAL_REDUCER_BRAND_DEF_FILE	unMakeAppPath(wxT("data/hardware/focal_reducer/focal_reducer_brands.ini"))
#define DEFAULT_LMOUNTS_BRAND_DEF_FILE			unMakeAppPath(wxT("data/hardware/mounts/mount_brands.ini"))
#define DEFAULT_FOCUSER_BRAND_DEF_FILE			unMakeAppPath(wxT("data/hardware/focuser/focuser_brands.ini"))
#define DEFAULT_TFILTER_BRAND_DEF_FILE			unMakeAppPath(wxT("data/hardware/filters/filter_brands.ini"))
// instrument ini file
#define DEFAULT_TELESCOPE_INI_FILE				unMakeAppPath(wxT("cfg/telescope.ini"))

// enum telescope hardware piece types
enum
{
	TELESCOPE_HARDWARE_MAIN_SCOPE = 0,
	TELESCOPE_HARDWARE_BARLOW_LENS,
	TELESCOPE_HARDWARE_FOCAL_REDUCER,
	TELESCOPE_HARDWARE_EYEPIECE,
	TELESCOPE_HARDWARE_LMOUNT,
	TELESCOPE_HARDWARE_FOCUSER,
	TELESCOPE_HARDWARE_TFILTER
};

// 11 :: telescope types
static int g_nTelescopeTypes = 12;
static DefHarwareDesignType g_vectTelescopeTypes[15] = 
{ 
	{ wxT("All"), "All", 0, 1 },
	{ wxT("Achromatic Refractor"), "AchromaticRefractor", 1, 0 },
	{ wxT("Apochromatic Refractor"), "ApochromaticRefractor", 2, 0 },
	{ wxT("Newtonian Dob/Eq"), "NewtonianDobEq", 3, 0 },
	{ wxT("Schmidt-Newtonian"), "SchmidtNewtonian", 4, 0 },
	{ wxT("Schmidt-Cassegrain"), "SchmidtCassegrain", 5, 0 },
	{ wxT("Maksutov-Newtonian"), "MaksutovNewtonian", 6, 0 },
	{ wxT("Maksutov-Cassegrain"), "MaksutovCassegrain", 7, 0 },
	{ wxT("Maksutov-Gregorian"), "MaksutovGregorian", 8, 0 },
	{ wxT("Ritchey-Chretien"), "RitcheyChretien", 9, 0 },
	// new design from meade ???
	{ wxT("Advanced Coma-Free"), "AdvancedComaFree", 10, 0 },	
	{ wxT("Other/Unknown"), "Other", 11, 0 }
 };

// 14 :: http://www.aoe.com.au/eyepiece_types.html
static int g_nTelescopeEyepieceTypes = 15;
static DefHarwareDesignType g_vectTelescopeEyepieceTypes[20] = 
{ 
	{ wxT("All"), "All", 0, 1 },
	{ wxT("Huyghens"), "Huyghens", 1, 0 },
	{ wxT("Ramsden"), "Ramsden", 2, 0 },
	{ wxT("Kellner"), "Kellner", 3, 0 },
	{ wxT("Orthoscopic"), "Orthoscopic", 4, 0 },
	{ wxT("Plossl"), "Plossl", 5, 0 },
	{ wxT("Erfle"), "Erfle", 6, 0 },
	{ wxT("Lanthanum"), "Lanthanum", 7, 0 },
	{ wxT("Ultra-Wide"), "UltraWide", 8, 0 },
	{ wxT("Hybrid"), "Hybrid", 9, 0 },
	{ wxT("Panoptic"), "Panoptic", 10, 0 },
	{ wxT("Radian"), "Radian", 11, 0 },
	{ wxT("Nagler"), "Nagler", 12, 0 },
	{ wxT("Konig"), "Konig", 13, 0 },
	{ wxT("Reticle"), "Reticle", 14, 0 }
};

// telescope mount types
static int g_nTelescopeMountTypes = 5;
static DefHarwareDesignType g_vectTelescopeMountTypes[9] =
{
	{ wxT("All"), "All", 0, 1 },
	{ wxT("Altazimuth"), "Altazimuth", 1, 0 },
	{ wxT("Dobsonian"), "Dobsonian", 2, 0 },
	{ wxT("German Equatorial"), "GermanEquatorial", 3, 0 },
	{ wxT("Equatorial Fork"), "EquatorialFork", 4, 0 }
};

// focuser types
static int g_nTelescopeFocuserTypes = 4;
static DefHarwareDesignType g_vectTelescopeFocuserTypes[9] =
{
	{ wxT("All"), "All", 0, 1 },
	{ wxT("Crayford"), "Crayford", 1, 0 },
	{ wxT("Rear Cell"), "RearCell", 2, 0 },
	{ wxT("Rack and Pinion"), "RackAndPinion", 3, 0 }
};

// 20 :: http://www.universetoday.com/guide-to-space/telescopes/telescope-filters/
static int g_nTelescopeFilterTypes = 21;
static DefHarwareDesignType g_vectTelescopeFilterTypes[25] =
{
	{ wxT("All"), "All", 0, 1 },
	{ wxT("Luminance"), "Luminance", 1, 0 },
	{ wxT("H-Alpha CCD"), "HAlphaCCD", 2, 0 },
	{ wxT("H-Beta CCD"), "HBetaCCD", 3, 0 },
	{ wxT("Neutral Density"), "NeutralDensity", 4, 0 },
	{ wxT("Oxygen III (OIII)"), "OxygenIII", 5, 0 },
	{ wxT("RGB Set"), "RGBSet", 6, 0 },
	{ wxT("Sulfur II (SII)"), "SulfurII", 7, 0 },
	{ wxT("Light Pollution (LPR)"), "LightPollution", 8, 0 },
	{ wxT("UBVRI Set"), "UBVRISet", 9, 0 },
	{ wxT("UV"), "UV", 10, 0 },
	{ wxT("IR"), "IR", 11, 0 },
	{ wxT("Broadband"), "Broadband", 12, 0 },
	{ wxT("Color Correction"), "ColorCorrection", 13, 0 },
	{ wxT("Color"), "Color", 14, 0 },
	{ wxT("Contrast Boosting"), "ContrastBoosting", 15, 0 },
	{ wxT("Narrowband"), "Narrowband", 16, 0 },
	{ wxT("Polarizing"), "Polarizing", 17, 0 },
	{ wxT("Sky Light"), "SkyLight", 18, 0 },
	{ wxT("Anti-Fringing"), "AntiFringing", 19, 0 },
	{ wxT("White Light Solar"), "WhiteLightSolar", 20, 0 }
};

// default :: setups type
static int g_nSetupsTypes = 3;
static wxString g_vectSetupsTypes[9] = 
{
	wxT("Visual Observation"),
	wxT("Astro-Photography"),
	wxT("Solar Observation")
};

// structure type: telescope
typedef struct 
{
	wxString name;
	unsigned char type;

	// manufacturer - made by - linke to the company/country where this was made
	wxString manufacturer;
	// brand - the original brand under which this was made
	wxString brand;

	// Clear Aperture 
	double aperture;
	// Focal Length Focal Ratio
	double focal_length;
	// Focal Ratio
	int focal_ratio;

	// Angular Field of View - 0.6 ° 
	double field_of_view;

	// Optical Coating
	// Ultra-High Transmission Coatings
	// Viewfinder - 8x50 - Finderscope 
	double finder_scope;
	// Primary, Secondary Mirrors - material - Pyrex® glass
	// Correcting Plate/Lens - clear water white glass
	// Telescope Mounting - fork-type; double tine - Mount 
	int mount;
	// Optical Tube material/construction - Aluminum 

	// Power Supply - Car battery adapter 
	// Highest Useful Magnification
	// Lowest Useful Magnification
	// Limiting Stellar Magnitude 
	// Resolution (Rayleigh) - Resolution (Dawes) 
	// Photographic Resolution 
	// Light Gathering Power 

	// Optical Design 
	
	// Telescope Weight 

	// Motor Drive 
	// Motor Resolution 
	// Computer Hand Control 
	wxString controller;

	// Tracking Rates 
	// Tracking Modes 
	// Alignment Procedures 
	// Software Precision - 24bit, 0.08 calculations 
	// Periodic Error Correction - Permanently Programmable 
	// Communication Ports 
	// Motor Ports 

	// Power Requirements - Power Requirements - Idle Current  : 340 mA 
	// Power Requirements - Slew one axis  : 640 mA 
	// Power Requirements - Slew both axes  : 900 mA 

	// GPS  : Internal 16 channel 

	// Gear  : 5.625 hard anodized gear in both axes, 180 tooth 
	// Bearings  : 9.8 azimuth bearing 

} DefTelescope;

// structure type: telescope brand
typedef struct 
{
	// brand - the original brand under which this was made
	wxString brand_name;
	wxString brand_ini_file;

	std::vector<DefTelescope> vectData;
//	int nData;
//	int nDataAllocated;

	// types
	unsigned char vectTypes[50];
	unsigned char nTypes;

} DefTelescopeBrand;

////////////////////////////////////////////////////////////////////
// structure type: eyepiece
typedef struct 
{
	wxString name;
	unsigned char type;

	// brand - made by
	wxString manufacturer;
	// brand - the original brand under which this was made
	wxString brand;

	// Barrel - outside tube diameter
	double barrel;

	// Eyepiece focal length
	double focal_length;
	// Dia. - diameter - 36mm
	// Coatings 
	// Weight - 45g
	// Design - Plossl, Panoptic, Radian, Nagler (4,2,5), Konig, Ortho, Erfle, Brandon, Kellner, Hybrid
	unsigned char design;
	// Parfocal
	// Eye Relief (mm)
	// Field Stop
	// Apparent Field (degrees)
	double apparent_field;
	// no of Elements
	int elements;
	// Body material 
 
} DefEyepiece;

// structure type: eyepiece brand
typedef struct 
{
	// brand - the original brand under which this was made
	wxString brand_name;
	wxString brand_ini_file;

	std::vector<DefEyepiece> vectData;
//	int nData;
//	int nDataAllocated;

	// types
	unsigned char vectTypes[50];
	unsigned char nTypes;

} DefEyepieceBrand;

////////////////////////////////////////////////////////////////////
// structure type: barlow lens
typedef struct 
{
	wxString name;
//	unsigned char h_type;

	// brand - made by
	wxString manufacturer;
	// brand - the original brand under which this was made
	wxString brand;

	double magnification;
	int elements;
	// Barrel - outside tube diameter
	double barrel;

} DefBarlowLens;

// structure type: barlow lens brand
typedef struct 
{
	// brand - the original brand under which this was made
	wxString brand_name;
	wxString brand_ini_file;

	std::vector<DefBarlowLens> vectData;
//	int nData;
//	int nDataAllocated;

} DefBarlowLensBrand;

////////////////////////////////////////////////////////////////////
// structure type: Focal Reducer
typedef struct 
{
	wxString name;

	// brand - made by
	wxString manufacturer;
	// brand - the original brand under which this was made
	wxString brand;

	double focal_ratio;
	double clear_aperture;
	int elements;
	double barrel;
	// price
	float price;
	int	currency_id;

} DefFocalReducer;

// structure type: Focal Reducer brand
typedef struct 
{
	// brand - the original brand under which this was made
	wxString brand_name;
	wxString brand_ini_file;

	std::vector<DefFocalReducer> vectData;
//	int nData;
//	int nDataAllocated;

} DefFocalReducerBrand;

////////////////////////////////////////////////////////////////////
// structure type: Telescope Mount
typedef struct 
{
	wxString name;
	unsigned char type;

	// brand - made by
	wxString manufacturer;
	// brand - the original brand under which this was made
	wxString brand;

	// Periodic Error Maximum
	double pem;
	// load capacity in kg
	double capacity;
	// electronic controller 
	wxString controller;

} DefLMount;

// structure type: Telescope Mount brand
typedef struct 
{
	// brand - the original brand under which this was made
	wxString brand_name;
	wxString brand_ini_file;

	std::vector<DefLMount> vectData;
//	int nData;
//	int nDataAllocated;

	// types
	unsigned char vectTypes[50];
	unsigned char nTypes;

} DefLMountBrand;

////////////////////////////////////////////////////////////////////
// structure type: Focuser
typedef struct 
{
	wxString name;
	unsigned char type;

	// brand - made by
	wxString manufacturer;
	// brand - the original brand under which this was made
	wxString brand;

	double drawtube;
	wxString controller;
	// weight in grams
	double weight;

} DefFocuser;

// structure type: Focuser brand
typedef struct 
{
	// brand - the original brand under which this was made
	wxString brand_name;
	wxString brand_ini_file;

	std::vector<DefFocuser> vectData;
//	int nData;
//	int nDataAllocated;

	// types
	unsigned char vectTypes[50];
	unsigned char nTypes;

} DefFocuserBrand;

////////////////////////////////////////////////////////////////////
// structure type: telescope filter
typedef struct 
{
	wxString name;
	unsigned char type;

	// brand - made by
	wxString manufacturer;
	// brand - the original brand under which this was made
	wxString brand;

	double clear_aperture;
	wxString band_pass;
	double transmission;
	double barrel;

} DefTFilter;

// structure type: Telescope filter brand
typedef struct 
{
	// brand - the original brand under which this was made
	wxString brand_name;
	wxString brand_ini_file;

	std::vector<DefTFilter> vectData;
//	int nData;
//	int nDataAllocated;

	// types
	unsigned char vectTypes[50];
	unsigned char nTypes;

} DefTFilterBrand;

////////////////////
// telescope driver types eunm
enum
{
	TELESCOPE_DRV_TYPE_NONE = 0,
	TELESCOPE_DRV_TYPE_MEADE_LX200,
	TELESCOPE_DRV_TYPE_CELESTRON_NEXSTAR
};

// external classes
class CAscom;
class CTelescopeLx200;
class CTelescopeNexStar;

// class:	CTelescope
///////////////////////////////////////////////////////
class CTelescope
{
// public methods
public:
	CTelescope( );
	~CTelescope( );

	/////////////////
	// method to select and control hardware
	int SetTelescope( );
	int Connect( );
	int Disconnect( );
	int IsConnected(){ return( m_bIsConnected ); }
	// get data
	int GetPosition( double& ra, double& dec );
	int GotoPosition( double ra, double dec, int bHighPrec=0 );

	/////////////////////////////
	// TELESCOPE
	// movement handlers
	int StartMovingEast();
	int StartMovingWest();
	int StartMovingNorth();
	int StartMovingSouth();
	int StopSlew();

	////////////////////////////
	// FOCUSER
	int SetFocusSpeed( long nSpeed );
	int StartFocusIn( long nSpeed=0 );
	int StartFocusOut( long nSpeed=0 );
	int StopFocus( );
	// absolute focus positioning
	int FocusIn( long nUnits );
	int FocusOut( long nUnits );
	int FocusAt( long nPosition );

	// save instruments
	int SaveConfig( );
	int LoadConfig( );

	///////////////
	// handlers to load hardware from ini files
	int LoadHardwareDef( int nHType, int nType=-1, int nBrand=-1 );
	int LoadHardwareIniByBrandId( int nHType, int nBrandId );
	int GetHardwareTypeIdByString( int nHType, const wxString& strType );
	int HasBrandHardwareType( int nHType, int nBrand, int nType );
	
	// telescope
	int AddTelescopeBrandDef( const wxString& strName );
	int AddTelescopeDef( int nBrand, const wxString& strName );
	int IsTelescopeDef( int nBrand, int nName );
	void UnloadTelescopeBrandsDef( );
	void UnloadTelescopeDef( int nBrand );

	// barlow lens
	int AddBarlowLensBrandDef( const wxString& strName );
	int AddBarlowLensDef( int nBrand, const wxString& strName );
	void UnloadBarlowLensBrandsDef( );
	void UnloadBarlowLensDef( int nBrand );

	// focal reducer
	int AddFocalReducerBrandDef( const wxString& strName );
	int AddFocalReducerDef( int nBrand, const wxString& strName );
	void UnloadFocalReducerBrandsDef( );
	void UnloadFocalReducerDef( int nBrand );

	// eyepiece
	int AddEyepieceBrandDef( const wxString& strName );
	int AddEyepieceDef( int nBrand, const wxString& strName );
	int IsEyepieceDef( int nBrand, int nName );
	void UnloadEyepieceBrandsDef( );
	void UnloadEyepieceDef( int nBrand );

	// mount
	int AddLMountBrandDef( const wxString& strName );
	int AddLMountDef( int nBrand, const wxString& strName );
	void UnloadLMountBrandsDef( );
	void UnloadLMountDef( int nBrand );

	// focuser
	int AddFocuserBrandDef( const wxString& strName );
	int AddFocuserDef( int nBrand, const wxString& strName );
	void UnloadFocuserBrandsDef( );
	void UnloadFocuserDef( int nBrand );

	// telescope filter
	int AddTFilterBrandDef( const wxString& strName );
	int AddTFilterDef( int nBrand, const wxString& strName );
	void UnloadTFilterBrandsDef( );
	void UnloadTFilterDef( int nBrand );

	// gui set handlers
	void SetGuiSelect( int nHType, wxChoice* pType, wxChoice* pBrand, wxChoice* pName, 
						int nType, int nBrand, int nName, std::vector<int> &vectBrandId, std::vector<int> &vectNameId );
	void SetGuiNameSelect( int nHType, wxChoice* pName, int nType, int nBrand, int nName, std::vector<int> &vectNameId );

// public data
public:
	int m_nTelescopeDrvType;
	// hardware interfaces objects
	CAscom* m_pAscom;
	CTelescopeLx200*	m_pTelescopeDrvLx200;
	CTelescopeNexStar*	m_pTelescopeDrvNexStar;
	// port setup
	int m_nSerialPortNo;
	int m_nSerialPortBaud;

	// flags
	int m_bIsConnected;

	// current data
	double m_nCurrentRa;
	double m_nCurrentDec;

	// telescope properties
	int m_bUseAscom;
	wxString m_strAscomDriverName;

	// telescope
	std::vector<DefTelescopeBrand>	m_vectTelescopeBrandDef;
//	int m_nTelescopeBrandDef;
//	int m_nTelescopeBrandDefAllocated;

	// barlow lens
	std::vector<DefBarlowLensBrand>	m_vectBarlowLensBrandDef;
//	int m_nBarlowLensBrandDef;
//	int m_nBarlowLensBrandDefAllocated;

	// focal reducer
	std::vector<DefFocalReducerBrand> m_vectFocalReducerBrandDef;
//	int m_nFocalReducerBrandDef;
//	int m_nFocalReducerBrandDefAllocated;

	// eyepiece
	std::vector<DefEyepieceBrand> m_vectEyepieceBrandDef;
//	int m_nEyepieceBrandDef;
//	int m_nEyepieceBrandDefAllocated;

	// telescope/lens mount - guider
	std::vector<DefLMountBrand> m_vectLMountBrandDef;
//	int m_nLMountBrandDef;
//	int m_nLMountBrandDefAllocated;

	// focuser
	std::vector<DefFocuserBrand> m_vectFocuserBrandDef;
//	int m_nFocuserBrandDef;
//	int m_nFocuserBrandDefAllocated;

	// telescope filter
	std::vector<DefTFilterBrand> m_vectTFilterBrandDef;
//	int m_nTFilterBrandDef;
//	int m_nTFilterBrandDefAllocated;

};

#endif

