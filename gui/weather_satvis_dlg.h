

#ifndef _WEATHER_SATVIS_DIALOG_H
#define _WEATHER_SATVIS_DIALOG_H

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

enum
{
	wxID_SAT_IMAGE_SOURCE = 21800,
	wxID_SAT_IMAGE_VIEW_ZONE,
	wxID_SAT_IMAGE_VIS_TYPE
};

//////////////
// zones :: eurometeo - Segment map ? use or not ?
static wxString vectZonesEeumetsat[20] =
{
	wxT("Italy and Central Mediterranean"),	// 1:SEGMENT14
	wxT("Spain and Western Mediterranean"),	// 2:SEGMENT5
	wxT("France and Western Europe"),		// 3:SEGMENT6
	wxT("Germany and Central Europe"),		// 4:SEGMENT15
	wxT("Great Britain and North Sea"),		// 5:SEGMENT4
	wxT("Iceland"),							// 6:SEGMENT2
	wxT("North Atlantic"),					// 7:SEGMENT3
	wxT("Scandinavia and Baltic Sea"),		// 8:SEGMENT7
	wxT("Eastern Europe"),					// 9:SEGMENT16
	wxT("Greece and Aegean Sea"),			// 10:SEGMENT17
	wxT("Turkey and Eastern Mediterranean"),// 11:SEGMENT18
	wxT("Israel and Middle East"),			// 12:SEGMENT12
	wxT("Egypt"),							// 13:SEGMENT11
	wxT("Libya"),							// 14:SEGMENT10
	wxT("Northern Africa"),					// 15:SEGMENT9
	wxT("Morocco"),							// 16:SEGMENT8
	wxT("Canary and Azores Islands")		// 17:SEGMENT1
};
static int nZonesEeumetsat = 17;
// zones :: fvalk.com
static wxString vectZonesFvalk[10] =
{
	wxT("Europe"),					// 1
	wxT("Europe, Africa"),			// 2
	wxT("Central Asia"),			// 3
	wxT("East Asia, Australia"),	// 4
	wxT("The Americas"),			// 5
	wxT("Western US")				// 6
};
static int nZonesFvalk = 6;
// zones :: meteoam.it
static wxString vectZonesMeteoamIt[5] =
{
	wxT("Europe")
};
static int nZonesMeteoamIt = 1;

// external class
class CConfigMain;
class CDynamicImgView;
class CUnimapWorker;

// class:	CWeatherVisDlg
///////////////////////////////////////////////////////
class CWeatherVisDlg : public wxDialog
{
public:
	CWeatherVisDlg( wxWindow *parent, const wxString& strTitle, CUnimapWorker* pUnimapWorker );
	~CWeatherVisDlg( );

	void SetOptions( );
	int SetConfig( CConfigMain* pMainConfig );
	int FetchRemoteImage( );
	int FetchRemoteImageDlg( );
	void SetRemoteImage( );

// public data
public:
	CUnimapWorker* m_pUnimapWorker;

	CDynamicImgView* m_pImgView;

	wxStaticText*	m_pImgDateTime;
	wxChoice*		m_pSatSource;
	wxChoice*		m_pViewZone;
	wxChoice*		m_pVisType;

	int m_nSourceType;
	int m_nVisType;
	int m_nZoneId;
	int m_bIsRemoteImage;
	double m_nImgDateTime;

	wxString m_strImgFile;

protected:
	void OnSelectVisZone( wxCommandEvent& pEvent );
	void OnSelectSatSource( wxCommandEvent& pEvent );

	DECLARE_EVENT_TABLE()

};

#endif
