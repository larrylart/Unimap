////////////////////////////////////////////////////////////////////
// Name:		Star definition
// File:		star.h
// Purpose:		definition of star/dso objects
//
// Created by:	Larry Lart on 04-05-2004
// Updated by:	Larry Lart on 12-01-2010 - unicode conversion
//
////////////////////////////////////////////////////////////////////

#ifndef _SKY_STAR_H
#define _SKY_STAR_H

// WX includes
#include "wx/wxprec.h"

// catalog define object types - use this also for general catalog type ?
#define CAT_OBJECT_TYPE_STAR		0
#define CAT_OBJECT_TYPE_MESSIER		1
#define CAT_OBJECT_TYPE_NGC			2
#define CAT_OBJECT_TYPE_IC			3
#define CAT_OBJECT_TYPE_TYCHO		4
#define CAT_OBJECT_TYPE_USNO		5
#define CAT_OBJECT_TYPE_SAO			6
#define CAT_OBJECT_TYPE_PGC			7
#define CAT_OBJECT_TYPE_MGC			8
#define CAT_OBJECT_TYPE_MCG			9
#define CAT_OBJECT_TYPE_HIPPARCOS	10
#define CAT_OBJECT_TYPE_NOMAD		11
#define CAT_OBJECT_TYPE_2MASS		12
#define CAT_OBJECT_TYPE_GSC			13
// dso next
#define CAT_OBJECT_TYPE_DSO			14
#define CAT_OBJECT_TYPE_UGC			15
// radio
#define CAT_OBJECT_TYPE_RADIO		16
#define CAT_OBJECT_TYPE_SPECFIND	17
#define CAT_OBJECT_TYPE_VLSS		18
#define CAT_OBJECT_TYPE_NVSS		19
#define CAT_OBJECT_TYPE_MSL			20
// x-ray/gamma
#define CAT_OBJECT_TYPE_XGAMMA			21
#define CAT_OBJECT_TYPE_ROSAT			22
#define CAT_OBJECT_TYPE_BATSE			23
#define CAT_OBJECT_TYPE_INTEGRAL		24
#define CAT_OBJECT_TYPE_BEPPOSAX_GRBM	25
#define CAT_OBJECT_TYPE_2XMMi			26
#define CAT_OBJECT_TYPE_HEAO_A1			27
// supernovas
#define CAT_OBJECT_TYPE_SUPERNOVA		28
#define CAT_OBJECT_TYPE_ASC				29
#define CAT_OBJECT_TYPE_SSC				30
// blackholes
#define CAT_OBJECT_TYPE_BLACKHOLE		40
#define CAT_OBJECT_TYPE_SDSS_DR5		41
#define CAT_OBJECT_TYPE_RXE_AGN			42
// multiple/double stars
#define CAT_OBJECT_TYPE_MSTARS			50
#define CAT_OBJECT_TYPE_WDS				51
#define CAT_OBJECT_TYPE_CCDM			52
// extra-solar planets
#define CAT_OBJECT_TYPE_EXOPLANET		53
#define CAT_OBJECT_TYPE_EXOPLANET_EU	54
// asteroids
#define CAT_OBJECT_TYPE_ASTEROID		55
#define CAT_OBJECT_TYPE_OMP				56
// comets
#define CAT_OBJECT_TYPE_COMET			57
#define CAT_OBJECT_TYPE_COCOM			58
// artificial earth satellite
#define CAT_OBJECT_TYPE_AES				59
#define CAT_OBJECT_TYPE_CELESTRAK		60

// sky object types stars, galaxy etc
#define SKY_OBJECT_TYPE_NONE			0
#define SKY_OBJECT_TYPE_STAR			1
#define SKY_OBJECT_TYPE_DSO				2	// - unknown general dso object
#define SKY_OBJECT_TYPE_NEBULA			3
#define SKY_OBJECT_TYPE_CLUSTER			4
#define SKY_OBJECT_TYPE_GALAXY			6
#define SKY_OBJECT_TYPE_SUPERNOVA		7
#define SKY_OBJECT_TYPE_BLACKHOLE		8
// alike sky objects - from detection classfication
#define SKY_OBJECT_TYPE_LIKE_A_STAR		9
#define SKY_OBJECT_TYPE_LIKE_A_GALAXY	10
#define SKY_OBJECT_TYPE_LIKE_A_RAY		11
// other types from GSC
#define SKY_OBJECT_TYPE_BLEND			12
#define SKY_OBJECT_TYPE_NON_STAR		13
#define SKY_OBJECT_TYPE_UNCLASSIFIED	14
#define SKY_OBJECT_TYPE_DEFECT			15
// radio type
#define SKY_OBJECT_TYPE_RADIO			16
// x-ray/gamma
#define SKY_OBJECT_TYPE_XGAMMA			17
#define SKY_OBJECT_TYPE_XRAY			18
#define SKY_OBJECT_TYPE_GAMMA			19
// multiple/double stars
#define SKY_OBJECT_TYPE_MSTARS			20
// extra-solar planet
#define SKY_OBJECT_TYPE_EXOPLANET		21
//  asteroid
#define SKY_OBJECT_TYPE_ASTEROID		22
// COMET
#define SKY_OBJECT_TYPE_COMET			23
// artificial  earth sattelite
#define SKY_OBJECT_TYPE_AES				24

// star states
#define OBJECT_STATE_SELECTED			1
#define OBJECT_STATE_MOUSE_OVER			2
#define OBJECT_STATE_CUSTOM_SHOW		4
#define OBJECT_STATE_CUSTOM_HIDE		8

////////////////////////////////////
// Just a star definition
///////////////////////////////////
typedef struct
{
	// catalogs numbers
	unsigned char cat_type;
	unsigned int zone_no;
	unsigned long cat_no;
	unsigned char comp_no;
	char* cat_name;
	// object type - dso - star etc ...
	unsigned char type;
	// match flag 
	unsigned char match;

	// name
//	char* name;

	// image coord
	double x;
	double y;
	int cx;
	int cy;
	// image area coord
	unsigned char x_a;
	unsigned char y_a;

	// ra/dec - coordinates
	double ra;
	double dec;
	///////////////////
	// visual brigthness and mag
	double vmag;
	double mag;
	// Full width at half maximum
	double fwhm;
	// integrated mes. flux
	double flux;

	// ellipse params
	double a;
	double b;
	double kronfactor;
	double theta;

	// other measurment vars
	double distance;

	////////////////////////
	// object state - for later processing
	int state;
	// id - used for matching 
	unsigned long img_id;
	// catalog originating id
	unsigned long cat_id;

} StarDef;

////////////////////////////////
// star basic details definition
typedef struct
{
	double parallax;
	double radial_velocity;
	int spectral_type;

} StarBasicDetailsDef;

/////////////////////////
// basic star definition for catalog - to use for matching
typedef struct
{
	char* cat_name;

	// catalogs numbers
	unsigned long cat_no;
	// zone number
	unsigned int zone_no;
	// component number
	unsigned char comp_no;
	// type of number
	unsigned char cat_type;
	// ra/dec - coordinates
	double ra;
	double dec;
	// magnitude
	double mag;

	// object type - dso - star etc ...
	unsigned char type;

} StarDefCatBasic;

/////////////////////////////////////////////////
// basic definition for NGC-IC DSO catalog
typedef struct
{
	// catalogs numbers
	unsigned long cat_no;
	// ra/dec - coordinates
	double ra;
	double dec;
	// magnitude
	double mag;
	// object params
	double a;
	double b;
	double pa;
	// mesier catalog no
	unsigned char messier_no;

} DsoDefCatNgcIc;

/////////////////////////////////////////////////
// basic definition DSO ! for PGC GALAXY catalog
typedef struct
{
	// catalogs numbers
	unsigned long cat_no;
	// ra/dec - coordinates
	double ra;
	double dec;
	// magnitude
	double mag;
	// object params
	double a;
	double b;
	double pa;
	// types
	unsigned char cat_type;
	unsigned char type;

} DsoDefCatBasic;

/////////////////////////////////////////////////
// basic definition for Messier DSO catalog
typedef struct
{
	// catalogs numbers
	unsigned char cat_no;
	// ra/dec - coordinates
	double ra;
	double dec;
	// magnitude
	double mag;

} DsoDefCatMessier;


/////////////////////////////////////////////////
// star common name structure
typedef struct
{
	// pointer to name - to be alocated
	wxChar*	name;
	// ra/dec - coordinates
	double ra;
	double dec;

} StarCommonName;

//////////////////////////////////////////////////////////////
// METHODS


#endif
