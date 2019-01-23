
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

// wx includes
#include "wx/wxprec.h"
#include <wx/dialog.h>

// ELX
#include <elx/core/CoreMacros.h>
#include <elx/math/TransfertFunction.h>
#include <elx/image/AbstractImageManager.h>
#include <elx/image/ImageFileManager.h>
#include <elx/math/MathCore.h>
#include <elx/image/ImageVariant.h>
#include <elx/image/ImageOperatorsImpl.h>
#include <elx/image/PixelIterator.h>

// local headers
#include "../astro_img_view.h"

// main header
#include "gradient_dlg.h"

BEGIN_EVENT_TABLE( CProcessGradientDlg, CImageProcBaseDlg )
END_EVENT_TABLE( )
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CProcessGradientDlg
// Purpose:	Initialize my 3d view config dialog 
// Input:	pointer to reference window 
// Output:	nothing
////////////////////////////////////////////////////////////////////
CProcessGradientDlg::CProcessGradientDlg( wxWindow *parent, int nType, int nViewType, 
										const wxString& strTitle ) : CImageProcBaseDlg( parent, strTitle, nViewType )
{
	m_nType = nType;
	Create( );
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CProcessGradientDlg
// Purpose:	destroy my  dialog
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CProcessGradientDlg::~CProcessGradientDlg( ) 
{

}

////////////////////////////////////////////////////////////////////
// Method:	Create
// Class:	CProcessGradientDlg
// Purpose:	create  dialog by type
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CProcessGradientDlg::Create( )
{
	if( m_nType == IMAGE_PROCESS_TYPE_GRADIENT_REMOVE )
	{
		SetTitle( wxT("Gradient :: Removal") );
		wxString vectMethods[2] = {wxT("Linear"), wxT("Quadratic") };
		int vectValues[8] = {GM_Linear, GM_Quadratic};
		AddSelector( "Method:", 8, vectMethods, vectValues );

	} else if( m_nType == IMAGE_PROCESS_TYPE_GRADIENT_ROTATIONAL )
	{
		SetTitle( wxT("Gradient :: Rotational") );
		AddSlider( "Radial Shift", 0, 200, 0.0, 10.0, 0.0 );
		AddSlider( "Rotational Shift", 0, 200, 0.0, 360.0, 5.0 );
	}

	Layout( );
	Fit( );
	Refresh( );
}

////////////////////////////////////////////////////////////////////
// Method:	Process
// Class:	CProcessGradientDlg
// Purpose:	process brightness contrast gamma dialog
// Input:	elx image
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CProcessGradientDlg::Process( ImageVariant& rElxImage )
{
	if( m_nType == IMAGE_PROCESS_TYPE_GRADIENT_REMOVE )
	{
		ImageVariant* _pImage1;
		boost::shared_ptr<ImageVariant> _spImage2;
		uint32 iChannelMask=CM_All;
		/// ???
		bool _isImage = true;

		// EGradientMethod: GM_Linear, GM_Quadratic  
		EGradientMethod method = GM_Linear;
		Math::Point2dList points;
		bool isImage;
		// todo: see how do I actualy get these gradient point
		//GetSettings(method, points, isImage);
		Math::Point2iList realPoints;
		const uint32 width = rElxImage.GetWidth();
		const uint32 height = rElxImage.GetHeight();
		const uint32 size = points.size(); 
		for(uint32 i = 0; i < size; ++i)
		{
			uint32 x = uint32(points[i]._x*width);
			uint32 y = uint32(points[i]._y*height);
			if( x < width && y < height )
				realPoints.push_back( Math::Point2i( int(points[i]._x*width),int(points[i]._y*height)) );
		} 

		// Create a dummy image variant to hold a background    
		_spImage2.reset(new ImageVariant(rElxImage.GetPixelFormat(), 1, 1));

		////////////////////////
		// call here to remove gradient - methods: GM_Linear, GM_Quadratic  
		bool result = rElxImage.RemoveGradient( *_spImage2.get(), realPoints, (EGradientMethod) m_vectChoiceSelected[0], iChannelMask ); //, iNotifier);
		if (result)
		{
			_pImage1 = &rElxImage;
			if (!_isImage)
			{
				boost::shared_ptr<AbstractImage> temp1 = _pImage1->GetImpl();
				boost::shared_ptr<AbstractImage> temp2 = _spImage2->GetImpl();
				_pImage1->Assign(temp2);
				_spImage2->Assign(temp1);
			}
		}

	} else if( m_nType == IMAGE_PROCESS_TYPE_GRADIENT_ROTATIONAL )
		rElxImage.ApplyRotationalGradient( (double) rElxImage.GetWidth()/2.0, (double) rElxImage.GetHeight()/2.0, 
											m_vectSliderCurrentReal[0], m_vectSliderCurrentReal[1], CM_All );
}
