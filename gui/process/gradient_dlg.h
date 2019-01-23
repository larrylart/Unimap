

#ifndef _PROCESS_GRADIENT_DLG_H
#define _PROCESS_GRADIENT_DLG_H

// wx includes
#include "wx/wxprec.h"
// ELX
#include <elx/image/ImageVariant.h>
#include <elx/image/ImageImpl.h>
#include <elx/image/Pixels.h>

// local includes
#include "process_base_dlg.h"

// defines
#define IMAGE_PROCESS_TYPE_GRADIENT_REMOVE		0
#define IMAGE_PROCESS_TYPE_GRADIENT_ROTATIONAL	1

using namespace eLynx;
using namespace eLynx::Image;

// external classes
class CAstroImgView;

class CProcessGradientDlg : public CImageProcBaseDlg
{
public:
	CProcessGradientDlg( wxWindow *parent, int nType, int nViewType=0, const wxString& strTitle = wxT("") );
	~CProcessGradientDlg( );

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


