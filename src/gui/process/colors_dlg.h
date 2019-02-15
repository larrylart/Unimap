////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////


#ifndef _PROCESS_COLORS_DLG_H
#define _PROCESS_COLORS_DLG_H

// wx includes
#include "wx/wxprec.h"
// ELX
#include <elx/image/ImageVariant.h>
#include <elx/image/ImageImpl.h>
#include <elx/image/Pixels.h>

// local includes
#include "process_base_dlg.h"

// defines
#define IMAGE_PROCESS_TYPE_BCG				10
#define IMAGE_PROCESS_TYPE_MIDTONE			11
#define IMAGE_PROCESS_TYPE_BALANCE			12
#define IMAGE_PROCESS_TYPE_COLORIZE			13
#define IMAGE_PROCESS_TYPE_DESATURATE		14
#define IMAGE_PROCESS_TYPE_HUESATURATION	15

using namespace eLynx;
using namespace eLynx::Image;

// external classes
class CAstroImgView;

class CProcessColorsDlg : public CImageProcBaseDlg
{
public:
	CProcessColorsDlg( wxWindow *parent, int nType, int nViewType=0, const wxString& strTitle = wxT("") );
	~CProcessColorsDlg( );

	// specific construction methods
	void Create( );

	void Process( ImageVariant& rElxImage );

// data
public:

// protected methods
protected:
	DECLARE_EVENT_TABLE()
};

#endif


