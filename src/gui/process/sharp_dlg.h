////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////


#ifndef _PROCESS_SHARP_DLG_H
#define _PROCESS_SHARP_DLG_H

// wx includes
#include "wx/wxprec.h"
// ELX
#include <elx/image/ImageVariant.h>
#include <elx/image/ImageImpl.h>
#include <elx/image/Pixels.h>

// local includes
#include "process_base_dlg.h"

// defines
#define IMAGE_PROCESS_TYPE_SHARP_SHARPEN		0
#define IMAGE_PROCESS_TYPE_SHARP_UNSHARP		1
#define IMAGE_PROCESS_TYPE_SHARP_UNSHARPMASK	2
#define IMAGE_PROCESS_TYPE_SHARP_LAPGAUS		3
#define IMAGE_PROCESS_TYPE_SHARP_EMBOSS			4

using namespace eLynx;
using namespace eLynx::Image;

// external classes
class CAstroImgView;

class CProcessSharpDlg : public CImageProcBaseDlg
{
public:
	CProcessSharpDlg( wxWindow *parent, int nType, int nViewType=0, const wxString& strTitle = wxT("") );
	~CProcessSharpDlg( );

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


