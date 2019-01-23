

#ifndef _PROCESS_BLUR_DLG_H
#define _PROCESS_BLUR_DLG_H

// wx includes
#include "wx/wxprec.h"
// ELX
#include <elx/image/ImageVariant.h>
#include <elx/image/ImageImpl.h>
#include <elx/image/Pixels.h>

// local includes
#include "process_base_dlg.h"

// defines
#define IMAGE_PROCESS_TYPE_BLUR_SMOOTH		0
#define IMAGE_PROCESS_TYPE_BLUR_MEAN		1
#define IMAGE_PROCESS_TYPE_BLUR_LOWPASS		2
#define IMAGE_PROCESS_TYPE_BLUR_GAUSSIAN	3
#define IMAGE_PROCESS_TYPE_BLUR_BILATERAL	4
#define IMAGE_PROCESS_TYPE_BLUR_SELECTIVE	5
#define IMAGE_PROCESS_TYPE_BLUR_FFTLOW		6

using namespace eLynx;
using namespace eLynx::Image;

// external classes
class CAstroImgView;

class CProcessBlurDlg : public CImageProcBaseDlg
{
public:
	CProcessBlurDlg( wxWindow *parent, int nType, int nViewType=0, const wxString& strTitle = wxT("") );
	~CProcessBlurDlg( );

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


