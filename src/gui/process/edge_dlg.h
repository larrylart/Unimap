////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////


#ifndef _PROCESS_EDGE_DLG_H
#define _PROCESS_EDGE_DLG_H

// wx includes
#include "wx/wxprec.h"
// ELX
#include <elx/image/ImageVariant.h>
#include <elx/image/ImageImpl.h>
#include <elx/image/Pixels.h>

// local includes
#include "process_base_dlg.h"

// defines
#define IMAGE_PROCESS_TYPE_EDGE_GRADIENT		0
#define IMAGE_PROCESS_TYPE_EDGE_ROBERSCROSS		1
#define IMAGE_PROCESS_TYPE_EDGE_LAPLACIAN		2	
#define IMAGE_PROCESS_TYPE_EDGE_MARRHILDRETH	3
#define IMAGE_PROCESS_TYPE_EDGE_CANNY			4

using namespace eLynx;
using namespace eLynx::Image;

// external classes
class CAstroImgView;

class CProcessEdgeDlg : public CImageProcBaseDlg
{
public:
	CProcessEdgeDlg( wxWindow *parent, int nType, int nViewType=0, const wxString& strTitle = wxT("") );
	~CProcessEdgeDlg( );

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


