

#ifndef _PROCESS_FILTERS_DLG_H
#define _PROCESS_FILTERS_DLG_H

// wx includes
#include "wx/wxprec.h"
// ELX
#include <elx/image/ImageVariant.h>
#include <elx/image/ImageImpl.h>
#include <elx/image/Pixels.h>

// local includes
#include "process_base_dlg.h"

// defines
#define IMAGE_PROCESS_TYPE_FILTER_MORPHOLOGICAL		0
#define IMAGE_PROCESS_TYPE_FILTER_DIGITAL			1
#define IMAGE_PROCESS_TYPE_PROCESS_DENOISING		2
#define IMAGE_PROCESS_TYPE_PROCESS_DEBLOOMER		3

using namespace eLynx;
using namespace eLynx::Image;

// external classes
class CAstroImgView;

class CProcessFiltersDlg : public CImageProcBaseDlg
{
public:
	CProcessFiltersDlg( wxWindow *parent, int nType, int nViewType=0, const wxString& strTitle = wxT("") );
	~CProcessFiltersDlg( );

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


