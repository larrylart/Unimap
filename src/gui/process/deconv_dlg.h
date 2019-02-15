////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _PROCESS_DECONVOLUTION_DLG_H
#define _PROCESS_DECONVOLUTION_DLG_H

// wx includes
#include "wx/wxprec.h"
// ELX
#include <elx/image/ImageVariant.h>
#include <elx/image/ImageImpl.h>
#include <elx/image/Pixels.h>

// local includes
#include "process_base_dlg.h"

// defines
#define IMAGE_PROCESS_TYPE_DECONV_RLUCY		0
#define IMAGE_PROCESS_TYPE_DECONV_WIENER	1
#define IMAGE_PROCESS_TYPE_DECONV_DCTEM		2

using namespace eLynx;
using namespace eLynx::Image;

// external classes
class CAstroImgView;

class CProcessDeconvolutionDlg : public CImageProcBaseDlg
{
public:
	CProcessDeconvolutionDlg( wxWindow *parent, int nType, int nViewType=0, const wxString& strTitle = wxT("") );
	~CProcessDeconvolutionDlg( );

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


