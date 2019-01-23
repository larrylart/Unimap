
#ifndef _CATALOG_VIEW_DLG_H
#define _CATALOG_VIEW_DLG_H

#include "wx/wxprec.h"
#include <wx/listctrl.h>

enum
{
	wxID_CATALOG_VIEW_FIND_ROWS_BUTTON = 2600,
	wxID_CATALOG_VIEW_FIND_ROWS_ENTRY,
	wxID_CATALOG_VIEW_FIND_COLS_TYPE,
	wxID_CATALOG_VIEW_PAGE_SIZE,
	wxID_CATALOG_VIEW_PAGE_BACK,
	wxID_CATALOG_VIEW_PAGE_NEXT,
	wxID_CATALOG_VIEW_SELECT_PAGE_NO
};

// defines
#define DEFAULT_NO_OF_CAT_ROWS_PER_PAGE			100

// catalog table definitions
typedef struct 
{
	char label[50];
	unsigned char type;
	int size;

} CatTableCol;

////////////////////////
// DEFINE :: EXO-PLANETS
static CatTableCol vectExoPlanet[11] = 
{ 
	{"Planet", 0, 100}, {"Mass", 1, 80}, {"Radius", 1, 80}, {"Period", 1, 80}, {"SM-Axis", 1, 80},
	{"Eccentricity", 1, 80}, {"Inclination", 1, 80}, {"Ang-Dist", 1, 80}, {"Disc. Year", 2, 60},
	{"RA", 1, 80}, {"Dec", 1, 80}
};
////////////////////////
// DEFINE :: STARS
static CatTableCol vectStars[11] = 
{
	{"Name", 2, 120}, {"RA", 1, 80}, {"DEC", 1, 80}, {"Mag", 1, 80}
};
////////////////////////
// DEFINE :: DSO
static CatTableCol vectHeadDso[11] = 
{
	{"Name", 2, 120}, {"RA", 1, 80}, {"DEC", 1, 80}, {"Mag", 1, 80}, 
	{"A", 1, 80}, {"B", 1, 80}, {"PA", 1, 80}
};
////////////////////////
// DEFINE :: RADIO
static CatTableCol vectHeadRadio[11] = 
{
	{"No", 2, 40}, {"Name", 2, 120}, {"RA", 1, 80}, {"DEC", 1, 80}, {"Freq.", 1, 80}, 
	{"Flux", 1, 80}, {"Int. Flux", 1, 80}, {"Points", 2, 80}, 
	{"Slope", 2, 80}, {"Abscissa", 2, 80} 
};
////////////////////////
// DEFINE :: XGAMMA
static CatTableCol vectHeadXGamma[30] = 
{
	{"No", 2, 40}, {"Name", 2, 120}, {"RA", 1, 80}, {"DEC", 1, 80}, {"Count", 1, 80},			// 1-5
	{"Bg-Count", 1, 80}, {"Exp. Time", 1, 80}, {"Flux", 1, 80}, {"Start Time", 2, 80},			// 6-9
	{"Duration", 1, 80}, {"Burst Date/Time", 1, 80}, {"Flux I", 1, 80}, {"Flux II", 1, 80},		// 10-13
	{"Flux III", 1, 80}, {"Flux IV", 1, 80}, {"Flux V", 1, 80}, {"Flux VI", 1, 80},				// 14-17
	{"Count I", 1, 80}, {"Count II", 1, 80}, {"Count III", 1, 80}, {"Count IV", 1, 80},			// 18-21
	{"Src Type", 2, 80}, {"Time Det.", 1, 80}, {"Int. No.", 1, 80}, {"Fluence", 1, 80},			// 22-25
	{"Peak Flux", 1, 80}, {"Gamma", 1, 80}, {"No Det.", 1, 80}									// 26-28
};
////////////////////////
// DEFINE :: SUPERNOVAS
static CatTableCol vectHeadSupernovas[20] = 
{
	{"No", 2, 40}, {"Name", 2, 120}, {"RA", 1, 80}, {"DEC", 1, 80}, {"Date Max", 5, 80},	// 1-5
	{"Date Disc", 5, 80}, {"Discoverer", 1, 80}, {"Mag", 1, 80}, {"Max. Mag", 1, 80},		// 6-9
	{"SN Type", 1, 80}, {"Gal. X", 2, 80}, {"Gal. Y", 2, 80}, {"Galaxy", 1, 80},			// 10-13
	{"Method", 2, 80}, {"Prog Code", 1, 80}, {"Unconf", 2, 80}								// 14-16
};
////////////////////////
// DEFINE :: BLACK HOLES
static CatTableCol vectHeadBlackholes[15] = 
{
	{"No", 2, 40}, {"Name", 2, 120}, {"RA", 1, 80}, {"DEC", 1, 80}, {"Mass", 1, 80},			// 1-5
	{"Mass HBeta", 1, 80}, {"Mass MGII", 1, 80}, {"Mass CIV", 1, 80}, {"Luminosity", 1, 80},	// 6-9
	{"Radio Lum", 1, 80}, {"XRay Lum", 1, 80}, {"Redshift", 1, 80}, {"Target Type", 2, 80}		// 10-13
};
////////////////////////
// DEFINE :: MULTIPLE/DOUBLE STARS
static CatTableCol vectHeadMStars[25] = 
{
	{"No", 2, 40}, {"Name", 2, 120}, {"RA", 1, 80}, {"DEC", 1, 80}, {"Mag", 1, 80},					// 1-5
	{"Mag 2", 1, 80}, {"Comp", 0, 80}, {"Spec Type", 0, 80}, {"Pos Ang", 1, 80},					// 6-9
	{"Pos Ang 2", 1, 80}, {"pm RA", 2, 80}, {"pm DEC", 1, 80}, {"pm RA 2", 2, 80},					// 10-13
	{"pm DEC 2", 1, 80}, {"pm Notes", 1, 80}, {"Sep", 1, 80}, {"Sep 2", 1, 80}, {"No Obs", 2, 80},	// 14-18
	{"Obs Date", 1, 80}, {"Obs Date 2", 1, 80}, {"Discoverer", 0, 80}, {"Notes", 0, 80}				// 19-22
};
////////////////////////
// DEFINE :: ASTEROIDS
static CatTableCol vectHeadAST[25] = 
{
	{"No", 2, 40}, {"Name", 2, 120}, {"RA", 1, 80}, {"DEC", 1, 80}, {"Ast No", 1, 80},			// 1-5
	{"Comp Name", 0, 80}, {"Abs Mag H", 1, 80}, {"Slope Mag", 1, 80}, {"Color Index", 1, 80},	// 6-9
	{"Iras Diam", 1, 80}, {"Iras Class", 0, 80}, {"No Arc", 2, 80}, {"No Obs", 2, 80},			// 10-13
	{"Epoch", 1, 80}, {"Mean Anomaly", 1, 80}, {"Arg Perih", 1, 80}, {"Lon Asc Node", 1, 80},	// 14-17
	{"Date Orb Comp", 1, 80}, {"Update Date", 1, 80}, {"Inclination", 1, 80},					// 18-20
	{"Eccentricity", 1, 80}, {"SMaj Axis", 1, 80}												// 21-22
};
////////////////////////
// DEFINE :: COMETS
static CatTableCol vectHeadCOM[20] = 
{
	{"No", 2, 40}, {"Name", 2, 120}, {"RA", 1, 80}, {"DEC", 1, 80}, {"Note Date", 0, 80},			// 1-5
	{"IAU Code", 0, 80}, {"Comp Name", 0, 80}, {"Epoch", 5, 80}, {"Rel Effect", 3, 80},				// 6-9
	{"Perih Date", 5, 80}, {"Perih Dist", 1, 80}, {"Arg Perih", 1, 80}, {"Lon Orb Node", 1, 80},	// 10-13
	{"Inclination", 1, 80}, {"Eccentricity", 1, 80}													// 14-15
};
////////////////////////
// DEFINE :: AES
static CatTableCol vectHeadAES[25] = 
{
	{"No", 2, 40}, {"Name", 2, 120}, {"Orb Type", 2, 80}, {"Obj Type", 2, 80}, {"Sat Type", 2, 80},			// 1-5
	{"Norad No", 2, 80}, {"Class", 2, 80}, {"COSPAR", 2, 80}, {"Eph Type", 2, 80},							// 6-9
	{"Epoch", 1, 80}, {"Decay", 1, 80}, {"MMnd Deriv", 1, 80}, {"Drag", 1, 80},								// 10-13
	{"Elem No", 2, 80},	{"Inclination", 1, 80}, {"Asc Node RA", 1, 80}, {"Eccentricity", 1, 80},			// 14-17
	{"Arg Of Perigee", 1, 80}, {"Mean Anomaly", 1, 80}, {"Mean Motion", 1, 80}, {"Orb No Epoch", 2, 80}		// 18-21						
};

// external classes
class CSky;

// class:	CCatalogViewDlg
///////////////////////////////////////////////////////
class CCatalogViewDlg : public wxDialog
{
// methods
public:
    CCatalogViewDlg( wxWindow *parent, CSky* pSky, const wxString& title );
	~CCatalogViewDlg( );

	void SetCatalogTable( int nCatDefId, int nCatId );
	void InitCatalogTablePage( );
	void SetCatalogTablePage( );
	int GetCatalogColumnRowDataAsStr( int nCatId, int col, int row, wxString& strData );

/////////////
// ::DATA

	// :: Vars
	CSky*	m_pSky;
	CatTableCol* m_vectHeader;
	int		m_nCatCols;
	unsigned long m_nCatRows;
	int		m_nCatRowsPerPage;
	int		m_nCatId;
	int		m_nCatViewPage;
	int		m_nCatPages;

	// :: GUI :: find
	wxButton*		m_pFindRowsButton;
	wxTextCtrl*		m_pFindTextEntry;
	wxChoice*		m_pFindCols;
	// :: GUI :: page size
	wxChoice*		m_pCatalogPageSize;
	// :: GUI :: navigation
	wxButton*		m_pPrevButton;
	wxButton*		m_pNextButton;
	wxChoice*		m_pCatalogPage;

	wxListCtrl*		m_pCatalogTable;

private:
	void OnDoFind( wxCommandEvent& event );
	void OnPageSetup( wxCommandEvent& pEvent );
	void OnNavPage( wxCommandEvent& event );
	void SetNavPage( int nId );

	DECLARE_EVENT_TABLE()
};

#endif
