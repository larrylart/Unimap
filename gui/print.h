
#ifndef _IMG_PRINTOUT
#define _IMG_PRINTOUT

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

// external classes
class CAstroImgView;

class CImgPrintout : public wxPrintout
{
public:
	CImgPrintout( CAstroImgView* pImgView=NULL, wxPageSetupDialogData* pPageSetupData=NULL, 
		const wxChar *title = _T("Astro-Image Printout") );

	bool OnPrintPage( int page );
	bool HasPage( int page );
	bool OnBeginDocument( int startPage, int endPage );
	void GetPageInfo( int *minPage, int *maxPage, int *selPageFrom, int *selPageTo );

	void DrawPage( );
	void DrawPageOne( );
	void DrawPageTwo( );
	bool WritePageHeader(wxPrintout *printout, wxDC *dc, const wxChar *text, float mmToLogical);

// data
	CAstroImgView*			m_pImgView;
	wxPageSetupDialogData*	m_pPageSetupData;


};

#endif
