
// system includes
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

// include main header
#include "sextractor.h"

////////////////////////////////////////////////////////////////////
// Method:		InitPrefsStruct
// Class:		CSextractor
// Purpose:		init prefs struct 
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::InitPrefsStruct( prefstruct& refPrefs )
{
	int i = 0;

	refPrefs.command_line = NULL;
	refPrefs.ncommand_line = 0;				
	strcpy( refPrefs.prefs_name, "" );
	for( i=0; i<2; i++ ) refPrefs.image_name[i] = NULL;
	refPrefs.nimage_name = 0;				
	strcpy( refPrefs.cat_name, "" );
	
	// thresholding 
	for( i=0; i<2; i++ ) refPrefs.dthresh[i] = 0;		
	refPrefs.ndthresh = 0;			
	for( i=0; i<2; i++ ) refPrefs.thresh[i] = 0;
	refPrefs.nthresh = 0;				
//	BkgndTreshType	thresh_type[2];	
	refPrefs.nthresh_type = 0;			

	// extraction 
	refPrefs.dimage_flag = 0;				
	refPrefs.ext_minarea = 0;				
	refPrefs.deb_maxarea = 0;				
	refPrefs.filter_flag = 0;
	refPrefs.filter_id = -1;
	strcpy( refPrefs.filter_name, "" );
	for( i=0; i<2; i++ ) refPrefs.filter_thresh[i] = 0;
	refPrefs.nfilter_thresh = 0;				
	refPrefs.deblend_nthresh = 0;		
	refPrefs.deblend_mincont = 0;
	refPrefs.satur_level = 0;
	refPrefs.detect_type = CCD;

	// Flagging 
	for( i=0; i<MAXFLAG; i++ ) refPrefs.fimage_name[i] = NULL;
	refPrefs.nfimage_name = 0;			
//	DefFlagType		flag_type[MAXFLAG];	
	refPrefs.imaflag_size = 0;			
	refPrefs.imanflag_size = 0;			
	refPrefs.nimaisoflag = 0;		
	refPrefs.nimaflag = 0;		

	// cleaning 
	refPrefs.clean_flag = 0;				
	refPrefs.clean_param = 0;
				
	// Weighting 
	for( i=0; i<2; i++ ) refPrefs.wimage_name[i] = NULL;
	refPrefs.nwimage_name = 0;				
//  weightenum	weight_type[2];				
	refPrefs.nweight_type = 0;			
	refPrefs.weight_flag = 0;				
	refPrefs.dweight_flag = 0;				
	refPrefs.weightgain_flag = 0;
	
	// photometry 
	refPrefs.cat_type = CAT_NONE;			
//	refPrefs.apert_type = AUTO;	
	for( i=0; i<MAXNAPER; i++ ) refPrefs.apert[i] = 0;
	refPrefs.naper = 0;				
	refPrefs.flux_apersize = 0; 
	refPrefs.fluxerr_apersize = 0;	
	refPrefs.mag_apersize = 0; 
	refPrefs.magerr_apersize = 0;		
	for( i=0; i<2; i++ ) refPrefs.autoparam[i] = 0;		
	refPrefs.nautoparam = 0;				
	for( i=0; i<2; i++ ) refPrefs.petroparam[i] = 0;
	refPrefs.npetroparam = 0;			
	for( i=0; i<2; i++ ) refPrefs.autoaper[i] = 0;		
	refPrefs.nautoaper = 0;			
	refPrefs.mag_zeropoint = 0;		
	refPrefs.mag_gamma = 0;			
	refPrefs.gain = 0;		
	
	// S/G separation 
	refPrefs.pixel_scale = 0;		
	refPrefs.seeing_fwhm = 0;		
 	strcpy( refPrefs.nnw_name, "" );

	// background 
	refPrefs.back_origin = IMAGE;	
	strcpy( refPrefs.back_name, "" );
//  backenum	back_type[2];				
	refPrefs.nback_type = 0;				
	for( i=0; i<2; i++ ) refPrefs.back_val[i] = 0;			
	refPrefs.nback_val = 0;			
	for( i=0; i<2; i++ ) refPrefs.backsize[i] = 0;			
	refPrefs.nbacksize = 0;	
	for( i=0; i<2; i++ ) refPrefs.backfsize[i] = 0;		
	refPrefs.nbackfsize = 0;
	refPrefs.backfthresh = 0;
	refPrefs.pback_type = LOCAL;	
	refPrefs.pback_size = 0;
	
	// memory 
	refPrefs.clean_stacksize = 0;
	refPrefs.mem_pixstack = 0;
	refPrefs.mem_bufsize = 0;

	// catalog output 
	strcpy( refPrefs.param_name, "" );

	// miscellaneous 
	refPrefs.pipe_flag = 0;				

	refPrefs.verbose_type = QUIET;	
	refPrefs.xml_flag = 0;			
	strcpy( refPrefs.xml_name, "" );
	strcpy( refPrefs.xsl_name, "" );
	strcpy( refPrefs.sdate_start, "" );
	strcpy( refPrefs.stime_start, "" );
	strcpy( refPrefs.sdate_end, "" );
	strcpy( refPrefs.stime_end, "" );
	refPrefs.time_diff = 0;			

	// CHECK-images 
	refPrefs.check_flag = 0;			
//  checkenum    	check_type[MAXCHECK];		       
	refPrefs.ncheck_type = 0;			
	for( i=0; i<MAXCHECK; i++ ) refPrefs.check_name[i] = NULL;
	refPrefs.ncheck_name = 0;				
	for( i=0; i<MAXCHECK; i++ ) refPrefs.check[i] = NULL;

	// ASSOCiation
	refPrefs.assoc_flag = 0;							
	strcpy( refPrefs.assoc_name, "" );
	for( i=0; i<3; i++ ) refPrefs.assoc_param[i] = 0;			
	refPrefs.nassoc_param = 0;			
	for( i=0; i<2; i++ ) refPrefs.assoc_data[i] = 0;
	refPrefs.nassoc_data = 0;			
	refPrefs.assoc_type= ASSOC_FIRST;			
	refPrefs.assocselec_type = ASSOCSELEC_ALL;		       
	refPrefs.assoc_radius = 0;		
	refPrefs.assoc_size = 0;				
	strcpy( refPrefs.retina_name, "" );
	for( i=0; i<2; i++ ) refPrefs.vignetsize[i] = 0;
	for( i=0; i<2; i++ ) refPrefs.vigshiftsize[i] = 0;
	refPrefs.cleanmargin = 0;				
	strcpy( refPrefs.som_name, "" );
	refPrefs.somfit_flag = 0;			
	refPrefs.somfit_vectorsize = 0;	

	// masking
	refPrefs.mask_type = MASK_NONE;				
	refPrefs.blank_flag = 0;
	for( i=0; i<2; i++ ) refPrefs.weight_thresh[i] = 0;
	refPrefs.nweight_thresh = 0;				

	// interpolation 
//	DefInterpolationType	interp_type[2];			
	refPrefs.ninterp_type = 0;			
	for( i=0; i<2; i++ ) refPrefs.interp_xtimeout[i] = 0;
  	refPrefs.ninterp_xtimeout = 0;     	      
	for( i=0; i<2; i++ ) refPrefs.interp_ytimeout[i] = 0;
	refPrefs.ninterp_ytimeout = 0;

	// astrometry 
	refPrefs.world_flag = 0;			
	strcpy( refPrefs.coosys, "" );
	refPrefs.epoch = 0;					

	// growth curve 
	refPrefs.growth_flag = 0;			
	refPrefs.flux_growthsize = 0;      		
	refPrefs.mag_growthsize = 0;      		
	refPrefs.flux_radiussize = 0;     		
	refPrefs.growth_step = 0;			
	for( i=0; i<MAXNAPER; i++ ) refPrefs.flux_frac[i] = 0;
	refPrefs.nflux_frac = 0;  		

	// PSF-fitting 
	refPrefs.psf_flag = 0;			
 	refPrefs.dpsf_flag = 0;		
	for( i=0; i<2; i++ ) refPrefs.psf_name[i] = NULL;
	refPrefs.npsf_name = 0;		
	refPrefs.psf_npsfmax = 0;			
	refPrefs.psfdisplay_type = PSFDISPLAY_SPLIT;			
	refPrefs.psf_xsize = 0;
	refPrefs.psf_ysize = 0;
	refPrefs.psf_xwsize = 0;
	refPrefs.psf_ywsize = 0;		
	refPrefs.psf_alphassize = 0;
	refPrefs.psf_deltassize = 0;		
	refPrefs.psf_alpha2000size = 0;
	refPrefs.psf_delta2000size = 0;
	refPrefs.psf_alpha1950size = 0;
	refPrefs.psf_delta1950size = 0;
	refPrefs.psf_fluxsize = 0;			
	refPrefs.psf_fluxerrsize = 0;	
	refPrefs.psf_magsize = 0;		
	refPrefs.psf_magerrsize = 0;		
	refPrefs.pc_flag = 0;		
	refPrefs.pc_vectorsize = 0;		

	// customize 
	refPrefs.fitsunsigned_flag = 0;
	refPrefs.next = 0;

	// Multithreading 
	refPrefs.nthreads = 0;
 
	return;
}

////////////////////////////////////////////////////////////////////
// Method:		InitCatStruct
// Class:		CSextractor
// Purpose:		init cat struct 
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::InitCatStruct( sexcatstruct& refCat )
{
	refCat.ndetect = 0;				
	refCat.ntotal = 0;					
	refCat.nparam = 0;			
	
	// Misc. strings defining the extraction 
//  char		prefs_name[MAXCHAR];			
//  char		image_name[MAXCHAR];			
//  char		psf_name[MAXCHAR];			
//  char		nnw_name[MAXCHAR];			
//  char		filter_name[MAXCHAR];		
//  char		soft_name[MAXCHAR];	
	
	// time 
//  char		ext_date[16],ext_time[16];		
	refCat.ext_elapsed = 0;		
	
	// MEF 
	refCat.currext = 0;			
	refCat.next = 0;				

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		InitObjStruct
// Class:		CSextractor
// Purpose:		init object struct
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::InitObjStruct( objstruct&	refObj )
{
	refObj.number = 0;
	refObj.fdnpix = 0;			
	refObj.dnpix = 0;			
	refObj.npix = 0;			
	refObj.fdflux = 0;				
	refObj.dflux = 0;			
	refObj.flux = 0;			
	refObj.fluxerr = 0;			
	refObj.flux_prof = 0;			
	refObj.fluxerr_prof = 0;			
	// PIXTYPE
	refObj.fdpeak = 0;			
	refObj.dpeak = 0;			
	refObj.peak = 0;			

	refObj.peakx = 0;
	refObj.peaky = 0;		
	refObj.mx = 0;
	refObj.my = 0;			
	refObj.poserr_mx2 = 0; 
	refObj.poserr_my2 = 0;
	refObj.poserr_mxy = 0;			

	refObj.xmin = 0;
	refObj.xmax = 0;
	refObj.ymin = 0;
	refObj.ymax = 0;
	refObj.ycmin = 0;
	refObj.ycmax = 0;
	// PIXTYPE*
	refObj.blank = NULL;
	refObj.dblank = NULL; 	    
	// int*
	refObj.submap = NULL;
	// int
	refObj.subx = 0;
	refObj.suby = 0;
	refObj.subw = 0;
	refObj.subh = 0;	
	refObj.flag = 0;				
	refObj.wflag = 0;			

//  FLAGTYPE	imaflag[MAXFLAG];	

	refObj.singuflag = 0;	

//  int		imanflag[MAXFLAG];     	
	refObj.mx2 = 0;
	refObj.my2 = 0;
	refObj.mxy = 0;	
	refObj.a = 0;
	refObj.b = 0;
	refObj.theta = 0;
	refObj.abcor = 0;	
	refObj.cxx = 0;
	refObj.cyy = 0;
	refObj.cxy = 0;		
	refObj.firstpix = 0;	
	refObj.lastpix = 0;		
	refObj.bkg = 0;
	refObj.dbkg = 0;
	refObj.sigbkg = 0;	
	refObj.thresh = 0;		
	refObj.dthresh = 0;
	refObj.mthresh = 0;

//	int refObj.iso[NISO];		

	refObj.fwhm = 0;		

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		InitObjTwoStruct
// Class:		CSextractor
// Purpose:		init object2 struct
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::InitObjTwoStruct( obj2struct&	refObj )
{
	// photometric data 
	refObj.flux_iso;			
	refObj.fluxerr_iso = 0;			
	refObj.mag_iso = 0;			
	refObj.magerr_iso = 0;			
	refObj.flux_isocor = 0;			
	refObj.fluxerr_isocor = 0;		
	refObj.mag_isocor = 0;		
	refObj.magerr_isocor = 0;			
	refObj.kronfactor = 0;			
	refObj.flux_auto = 0;			
	refObj.fluxerr_auto = 0;		
	refObj.mag_auto = 0;			
	refObj.magerr_auto = 0;			
	refObj.petrofactor = 0;		
	refObj.flux_petro = 0;			
	refObj.fluxerr_petro = 0;		
	refObj.mag_petro = 0;			
	refObj.magerr_petro = 0;		
	refObj.flux_best = 0;			
	refObj.fluxerr_best = 0;		
	refObj.mag_best = 0;			
	refObj.magerr_best = 0;			

	refObj.flux_aper = NULL;			
	refObj.fluxerr_aper = NULL;		
	refObj.mag_aper = NULL;		
	refObj.magerr_aper = NULL;		

	refObj.flux_prof = 0;		
	refObj.fluxerr_prof = 0;		
	refObj.mag_prof = 0;			
	refObj.magerr_prof = 0;		
	refObj.flux_win = 0;		
	refObj.fluxerr_win = 0;		
	refObj.mag_win = 0;		
	refObj.magerr_win = 0;	
	
	// astrometric data 
	refObj.posx = 0;
	refObj.posy = 0;			
	refObj.mamaposx = 0;
	refObj.mamaposy = 0;		
	refObj.sposx = 0;
	refObj.sposy = 0;			
	refObj.poserr_a = 0; 
	refObj.poserr_b = 0;
	refObj.poserr_theta = 0;			
	refObj.poserr_cxx = 0; 
	refObj.poserr_cyy = 0;
	refObj.poserr_cxy = 0;			
	refObj.poserr_mx2w = 0; 
	refObj.poserr_my2w = 0;
	refObj.poserr_mxyw = 0;		
	refObj.poserr_aw = 0; 
	refObj.poserr_bw = 0;
	refObj.poserr_thetaw = 0;			
	refObj.poserr_thetas = 0;			
	refObj.poserr_theta2000 = 0;		
	refObj.poserr_theta1950 = 0;		
	refObj.poserr_cxxw = 0; 
	refObj.poserr_cyyw = 0;
	refObj.poserr_cxyw = 0;			
	refObj.mx2w = 0;
	refObj.my2w = 0;
	refObj.mxyw = 0;		
	refObj.peakxw = 0; 
	refObj.peakyw = 0;			
	refObj.mxw = 0; 
	refObj.myw = 0;			
	refObj.alphas = 0; 
	refObj.deltas = 0;			
	refObj.thetas = 0;				
	refObj.peakalphas = 0; 
	refObj.peakdeltas = 0;		
	refObj.peakalpha2000 = 0; 
	refObj.peakdelta2000 = 0;	
	refObj.peakalpha1950 = 0; 
	refObj.peakdelta1950 = 0;	
	refObj.alpha2000 = 0; 
	refObj.delta2000 = 0;		
	refObj.theta2000 = 0;			
	refObj.alpha1950 = 0; 
	refObj.delta1950 = 0;		
	refObj.theta1950 = 0;			
	refObj.aw = 0; 
	refObj.bw = 0;				
	refObj.thetaw = 0;				
	refObj.cxxw = 0;
	refObj.cyyw = 0;
	refObj.cxyw = 0;		
	refObj.npixw = 0; 
	refObj.fdnpixw = 0;			
	refObj.threshmu = 0;		
	refObj.maxmu = 0;				
	refObj.elong = 0;				
	refObj.ellip = 0;				
	refObj.polar = 0;			
	refObj.polarw = 0;			
	refObj.sprob = 0;			
	refObj.fwhmw = 0;				
	refObj.assoc = NULL;			
	refObj.assoc_number = 0;			
	refObj.vignet = NULL;			
	refObj.vigshift = NULL;			

	// Windowed measurements 
	refObj.winpos_x = 0;
	refObj.winpos_y = 0;		
	refObj.winposerr_mx2 = 0; 
	refObj.winposerr_my2 = 0;
	refObj.winposerr_mxy = 0;		
	refObj.winposerr_a = 0; 
	refObj.winposerr_b = 0;
	refObj.winposerr_theta = 0;		
	refObj.winposerr_cxx = 0; 
	refObj.winposerr_cyy = 0;
	refObj.winposerr_cxy = 0;		
	refObj.winposerr_mx2w = 0; 
	refObj.winposerr_my2w = 0;
	refObj.winposerr_mxyw = 0;		
	refObj.winposerr_aw = 0; 
	refObj.winposerr_bw = 0;
	refObj.winposerr_thetaw = 0;		
	refObj.winposerr_thetas = 0;		
	refObj.winposerr_theta2000 = 0;		
	refObj.winposerr_theta1950 = 0;		
	refObj.winposerr_cxxw = 0; 
	refObj.winposerr_cyyw = 0;
	refObj.winposerr_cxyw = 0;			
	refObj.win_mx2 = 0; 
	refObj.win_my2 = 0;
	refObj.win_mxy = 0;			
	refObj.win_a = 0; 
	refObj.win_b = 0;
	refObj.win_theta = 0;			
	refObj.win_polar = 0;			
	refObj.win_cxx = 0; 
	refObj.win_cyy = 0;
	refObj.win_cxy = 0;			
	refObj.win_mx2w = 0; 
	refObj.win_my2w = 0;
	refObj.win_mxyw = 0;			
	refObj.win_aw = 0; 
	refObj.win_bw = 0;
	refObj.win_thetaw = 0;			
	refObj.win_polarw = 0;			
	refObj.win_thetas = 0;		
	refObj.win_theta2000 = 0;		
	refObj.win_theta1950 = 0;		
	refObj.win_cxxw = 0; 
	refObj.win_cyyw = 0;
	refObj.win_cxyw = 0;			
	refObj.winpos_xw = 0; 
	refObj.winpos_yw = 0;		
	refObj.winpos_alphas = 0; 
	refObj.winpos_deltas = 0;	
	refObj.winpos_alpha2000 = 0; 
	refObj.winpos_delta2000 = 0;	
	refObj.winpos_alpha1950 = 0; 
	refObj.winpos_delta1950 = 0;	
	refObj.winpos_niter = 0;			
	refObj.win_flag = 0;			

 
	refObj.flux_somfit = 0;			
	refObj.fluxerr_somfit = 0;			
	refObj.mag_somfit = 0;			
	refObj.magerr_somfit = 0;			
	refObj.stderr_somfit = 0;			
	refObj.vector_somfit = NULL;			
	// Growth curves and stuff
	refObj.flux_growth = NULL;			
	refObj.flux_growthstep = 0;		
	refObj.mag_growth = NULL;			
	refObj.mag_growthstep = 0;			
	refObj.flux_radius = NULL;			
	refObj.hl_radius = 0;			
	// PSF-fitting 
	refObj.flux_psf = NULL;			
	refObj.fluxerr_psf = NULL;			
	refObj.mag_psf = NULL;			
	refObj.magerr_psf = NULL;			
	refObj.x_psf = NULL; 
	refObj.y_psf = NULL;			
	refObj.niter_psf = 0;			
	refObj.npsf = 0;				
	refObj.chi2_psf = 0;			
	refObj.xw_psf = 0; 
	refObj.yw_psf = 0;			
	refObj.alphas_psf = 0; 
	refObj.deltas_psf = 0;		
	refObj.alpha2000_psf = 0; 
	refObj.delta2000_psf = 0;	
	refObj.alpha1950_psf = 0; 
	refObj.delta1950_psf = 0;	
	refObj.poserrmx2_psf = 0; 
	refObj.poserrmy2_psf = 0;
	refObj.poserrmxy_psf = 0;			
	refObj.poserra_psf = 0; 
	refObj.poserrb_psf = 0;
	refObj.poserrtheta_psf = 0;		
	refObj.poserrcxx_psf = 0; 
	refObj.poserrcyy_psf = 0;
	refObj.poserrcxy_psf = 0;			
	refObj.poserrmx2w_psf = 0; 
	refObj.poserrmy2w_psf = 0;
	refObj.poserrmxyw_psf = 0;			
	refObj.poserraw_psf = 0; 
	refObj.poserrbw_psf = 0;
	refObj.poserrthetaw_psf = 0;		
	refObj.poserrthetas_psf = 0;		
	refObj.poserrtheta2000_psf = 0;		
	refObj.poserrtheta1950_psf = 0;		
	refObj.poserrcxxw_psf = 0; 
	refObj.poserrcyyw_psf = 0;
	refObj.poserrcxyw_psf = 0;			
	// PC-fitting 
	refObj.mx2_pc = 0;
	refObj.my2_pc = 0;
	refObj.mxy_pc = 0;		
	refObj.a_pc = 0;
	refObj.b_pc = 0;
	refObj.theta_pc = 0;		
	refObj.vector_pc = NULL;			
	refObj.gdposang = 0;			
	refObj.gdscale = 0;			
	refObj.gdaspect = 0;			
	refObj.gde1 = 0;
	refObj.gde2 = 0;			
	refObj.gbratio = 0;			
	refObj.gbposang = 0;			
	refObj.gbscale = 0;			
	refObj.gbaspect = 0;			
	refObj.gbe1 = 0;
	refObj.gbe2 = 0;			
	refObj.flux_galfit = 0;			
	refObj.fluxerr_galfit = 0;			
	refObj.mag_galfit = 0;			
	refObj.magerr_galfit = 0;			
	// MEF 
	refObj.ext_number = 0;			

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		AddHeadKeyOne
// Class:		CSextractor
// Purpose:		add emlement to key vector
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::AddHeadKeyOne( const char* strName="", const char* strComment="",
							void* pPtr=NULL, h_type nHtype=H_INT,
							t_type nTtype=T_BYTE, const char* strPrintf="",
							const char* strUnit="", const char* strVoucd="", 
							const char* strVounit="", int nNaxis=0,
							int* pNaxisn=NULL, int nNobj=0, 
							int nNbytes=0, long nPos=0 )
{
	strcpy( headkey1[m_nHeadKeyOne].name, strName );
	strcpy( headkey1[m_nHeadKeyOne].comment, strComment );
	headkey1[m_nHeadKeyOne].ptr = pPtr;
	headkey1[m_nHeadKeyOne].htype = nHtype;
	headkey1[m_nHeadKeyOne].ttype = nTtype;
	strcpy( headkey1[m_nHeadKeyOne].printf, strPrintf );
	strcpy( headkey1[m_nHeadKeyOne].unit, strUnit );
	strcpy( headkey1[m_nHeadKeyOne].voucd, strVoucd );
	strcpy( headkey1[m_nHeadKeyOne].vounit, strVounit );
	headkey1[m_nHeadKeyOne].naxis = nNaxis;
	headkey1[m_nHeadKeyOne].naxisn = pNaxisn;
	headkey1[m_nHeadKeyOne].nobj = nNobj;
	headkey1[m_nHeadKeyOne].nbytes = nNbytes;
	headkey1[m_nHeadKeyOne].pos = nPos;

	m_nHeadKeyOne++;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		InitHeadKeyOneList
// Class:		CSextractor
// Purpose:		initialize my head key list
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::InitHeadKeyOneList( )
{
//		keystruct	CSextractor::headkey1[] = {

	AddHeadKeyOne( "EPOCH   ", "",
	&thefield1.epoch, H_FLOAT, T_DOUBLE, "%7.2f" );
	AddHeadKeyOne( "OBJECT  ", "",
	thefield1.ident, H_STRING, T_STRING, "%18s" );
	AddHeadKeyOne( "ORIGIN  ", "", "SExtractor", H_STRING, T_STRING, "%18s" );
	AddHeadKeyOne( "CRVAL1", "WORLD X COORD. OF REFERENCE PIXEL", &ddummy, H_EXPO, T_DOUBLE, "%15G" );
	AddHeadKeyOne( "CRVAL2", "WORLD Y COORD. OF REFERENCE PIXEL", &ddummy, H_EXPO, T_DOUBLE, "%15G" );
	AddHeadKeyOne( "CRPIX1", "IMAGE X COORD. OF REFERENCE PIXEL",
	&idummy, H_INT, T_LONG, "%5d" );
	AddHeadKeyOne( "CRPIX2", "IMAGE Y COORD. OF REFERENCE PIXEL",
	&idummy, H_INT, T_LONG, "%5d" );
	AddHeadKeyOne( "CDELT1", "WORLD PIXEL STEP ALONG X",
	&ddummy, H_EXPO, T_DOUBLE, "%15G" );
	AddHeadKeyOne( "CDELT2", "WORLD PIXEL STEP ALONG Y",
	&ddummy, H_EXPO, T_DOUBLE, "%15G" );
	AddHeadKeyOne( "CROTA1", "CCW ANGLE FROM X-IMAGE TO X-WORLD",
	&ddummy, H_EXPO, T_DOUBLE, "%15G" );
	AddHeadKeyOne( "CROTA2", "CCW ANGLE FROM Y-IMAGE TO Y-WORLD",
	&ddummy, H_EXPO, T_DOUBLE, "%15G" );
	AddHeadKeyOne( "FITSFILE", "File name of the analysed image",
	thecat.image_name, H_STRING, T_STRING, "%-18s" );
	AddHeadKeyOne( "FITSEXT ", "FITS Extension number",
	&thecat.currext, H_INT, T_LONG, "%3d" );
	AddHeadKeyOne( "FITSNEXT", "Number of FITS image extensions in file",
	&thecat.next, H_INT, T_LONG, "3d" );
	AddHeadKeyOne( "SEXIMASX", "IMAGE WIDTH (PIXELS)",
	&thefield1.width, H_INT, T_LONG, "%10d" );
	AddHeadKeyOne( "SEXIMASY", "IMAGE HEIGHT (PIXELS)",
	&thefield1.height, H_INT, T_LONG, "%10d" );
	AddHeadKeyOne( "SEXSTRSY", "STRIP HEIGHT (LINES)",
	&thefield1.stripheight, H_INT, T_LONG, "%10d" );
	AddHeadKeyOne( "SEXIMABP", "FITS IMAGE BITPIX",
	&thefield1.bitpix, H_INT, T_LONG, "%3d" );
	AddHeadKeyOne( "SEXPIXS", "PIXEL SCALE (ARCSEC)",
	&thefield1.pixscale, H_EXPO, T_DOUBLE, "%12G" );
	AddHeadKeyOne( "SEXSFWHM", "SEEING FWHM (ARCSEC)",
	&prefs.seeing_fwhm, H_EXPO, T_DOUBLE, "%12G" );
	AddHeadKeyOne( "SEXNNWF ", "CLASSIFICATION NNW FILENAME",
	thecat.nnw_name, H_STRING, T_STRING, "%18s" );
	AddHeadKeyOne( "SEXGAIN ", "GAIN (IN E- PER ADU)",
	&prefs.gain, H_EXPO, T_DOUBLE, "%7.3F" );
	AddHeadKeyOne( "SEXBKGND", "MEDIAN BACKGROUND (ADU)",
	&thefield1.backmean, H_EXPO, T_FLOAT, "%12G" );
	AddHeadKeyOne( "SEXBKDEV", "MEDIAN RMS (ADU)",
	&thefield1.backsig, H_EXPO, T_FLOAT, "%12G" );
	AddHeadKeyOne( "SEXBKTHD", "EXTRACTION THRESHOLD (ADU)",
	&thefield2.thresh, H_EXPO, T_FLOAT, "%12G" );
	AddHeadKeyOne( "SEXCONFF", "CONFIGURATION FILENAME",
	thecat.prefs_name, H_STRING, T_STRING, "%18s" );
	AddHeadKeyOne( "SEXDETT ", "DETECTION TYPE",
	"CCD", H_STRING, T_STRING, "%s" );
	AddHeadKeyOne( "SEXTHLDT", "THRESHOLD TYPE",
	"SIGMA", H_STRING, T_STRING, "%s" );
	AddHeadKeyOne( "SEXTHLD ", "THRESHOLD",
	&prefs.dthresh[0], H_EXPO, T_DOUBLE, "%12G" );
	AddHeadKeyOne( "SEXMINAR", "EXTRACTION MINIMUM AREA (PIXELS)",
	&prefs.ext_minarea, H_INT, T_LONG, "%6d" );
	AddHeadKeyOne( "SEXCONV ", "CONVOLUTION FLAG",
	&prefs.filter_flag, H_BOOL, T_LONG, "%1s" );
	AddHeadKeyOne( "SEXCONVN", "CONVOLUTION NORM. FLAG",
	&prefs.filter_flag, H_BOOL, T_LONG, "%1s" );
	AddHeadKeyOne( "SEXCONVF", "CONVOLUTION FILENAME",
	thecat.filter_name, H_STRING, T_STRING, "%18s" );
	AddHeadKeyOne( "SEXDBLDN", "NUMBER OF SUB-THRESHOLDS",
	&prefs.deblend_nthresh, H_INT, T_LONG, "%3d" );
	AddHeadKeyOne( "SEXDBLDC", "CONTRAST PARAMETER",
	&prefs.deblend_mincont, H_FLOAT, T_DOUBLE, "%8f" );
	AddHeadKeyOne( "SEXCLN  ", "CLEANING FLAG",
	&prefs.clean_flag, H_BOOL, T_LONG, "%1s" );
	AddHeadKeyOne( "SEXCLNPA", "CLEANING PARAMETER",
	&prefs.clean_param, H_FLOAT, T_DOUBLE, "%8f" );
	AddHeadKeyOne( "SEXCLNST", "CLEANING OBJECT-STACK",
	&prefs.deblend_nthresh, H_INT, T_LONG, "%6d" );
	AddHeadKeyOne( "SEXAPERD", "APERTURE DIAMETER (PIXELS)",
	&prefs.apert[0], H_INT, T_LONG, "%7.1f" );
	AddHeadKeyOne( "SEXAPEK1", "KRON PARAMETER",
	&prefs.autoparam[0], H_FLOAT, T_DOUBLE, "%4.1f" );
	AddHeadKeyOne( "SEXAPEK2", "KRON ANALYSIS RADIUS",
	&prefs.autoparam[0], H_FLOAT, T_DOUBLE, "%4.1f" );
	AddHeadKeyOne( "SEXAPEK3", "KRON MINIMUM RADIUS",
	&prefs.autoparam[1], H_FLOAT, T_DOUBLE, "%4.1f" );
	AddHeadKeyOne( "SEXSATLV", "SATURATION LEVEL (ADU)",
	&prefs.satur_level, H_EXPO, T_DOUBLE, "%12G" );
	AddHeadKeyOne( "SEXMGZPT", "MAGNITUDE ZERO-POINT",
	&prefs.mag_zeropoint, H_FLOAT, T_DOUBLE, "%8.4f" );
	AddHeadKeyOne( "SEXMGGAM", "MAGNITUDE GAMMA",
	&prefs.mag_gamma, H_FLOAT, T_DOUBLE, "%4.2f" );
	AddHeadKeyOne( "SEXBKGSX", "BACKGROUND MESH WIDTH (PIXELS)",
	&thefield1.backw, H_INT, T_LONG, "%5d" );
	AddHeadKeyOne( "SEXBKGSY", "BACKGROUND MESH HEIGHT (PIXELS)",
	&thefield1.backh, H_INT, T_LONG, "%5d" );
	AddHeadKeyOne( "SEXBKGFX", "BACKGROUND FILTER WIDTH",
	&thefield1.nbackfx, H_INT, T_LONG, "%3d" );
	AddHeadKeyOne( "SEXBKGFY", "BACKGROUND FILTER HEIGHT",
	&thefield1.nbackfy, H_INT, T_LONG, "%3d" );
	AddHeadKeyOne( "SEXPBKGT", "PHOTOM BACKGROUND TYPE",
	"GLOBAL", H_STRING, T_STRING, "%s" );
	AddHeadKeyOne( "SEXPBKGS", "LOCAL AREA THICKNESS (PIXELS)",
	&prefs.pback_size, H_INT, T_LONG, "%3d" );
	AddHeadKeyOne( "SEXPIXSK", "PIXEL STACKSIZE (PIXELS)",
	&prefs.mem_pixstack, H_INT, T_LONG, "%8d" );
	AddHeadKeyOne( "SEXFBUFS", "FRAME-BUFFER SIZE (LINES)",
	&prefs.mem_bufsize, H_INT, T_LONG, "%5d" );
	AddHeadKeyOne( "SEXISAPR", "ISO-APER RATIO",
	 &ddummy, H_FLOAT, T_DOUBLE, "%4.2f" );
	AddHeadKeyOne( "SEXNDET ", "NB OF DETECTIONS",
	&thecat.ndetect, H_INT, T_LONG, "%9d" );
	AddHeadKeyOne( "SEXNFIN ", "NB OF FINAL EXTRACTED OBJECTS",
	&thecat.ntotal, H_INT, T_LONG, "%9d" );
	AddHeadKeyOne( "SEXNPARA", "NB OF PARAMETERS PER OBJECT",
	&thecat.nparam, H_INT, T_LONG, "%3d" );

	AddHeadKeyOne( "" );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		AddHeadKey
// Class:		CSextractor
// Purpose:		add emlement to key vector
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::AddHeadKey( const char* strName="", const char* strComment="",
							void* pPtr=NULL, h_type nHtype=H_INT,
							t_type nTtype=T_BYTE, const char* strPrintf="",
							const char* strUnit="", const char* strVoucd="", 
							const char* strVounit="", int nNaxis=0,
							int* pNaxisn=NULL, int nNobj=0, 
							int nNbytes=0, long nPos=0 )
{
	strcpy( headkey[m_nHeadKey].name, strName );
	strcpy( headkey[m_nHeadKey].comment, strComment );
	headkey[m_nHeadKey].ptr = pPtr;
	headkey[m_nHeadKey].htype = nHtype;
	headkey[m_nHeadKey].ttype = nTtype;
	strcpy( headkey[m_nHeadKey].printf, strPrintf );
	strcpy( headkey[m_nHeadKey].unit, strUnit );
	strcpy( headkey[m_nHeadKey].voucd, strVoucd );
	strcpy( headkey[m_nHeadKey].vounit, strVounit );
	headkey[m_nHeadKey].naxis = nNaxis;
	headkey[m_nHeadKey].naxisn = pNaxisn;
	headkey[m_nHeadKey].nobj = nNobj;
	headkey[m_nHeadKey].nbytes = nNbytes;
	headkey[m_nHeadKey].pos = nPos;

	m_nHeadKey++;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		InitHeadKeyList
// Class:		CSextractor
// Purpose:		initialize my head key list
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::InitHeadKeyList( )
{
//	keystruct	CSextractor::headkey[] = {

	AddHeadKey( "FITSFILE", "File name of the analysed image",
				thecat.image_name, H_STRING, T_STRING, "%-18s" );
	AddHeadKey( "FITSEXT ", "FITS Extension number",
				&thecat.currext, H_INT, T_LONG, "%3d" );
	AddHeadKey( "FITSNEXT", "Number of FITS image extensions in file",
				&thecat.next, H_INT, T_LONG, "3d" );
	AddHeadKey( "SEXVERS ", "Extraction software",
				thecat.soft_name, H_STRING, T_STRING, "%-18s" );
	AddHeadKey( "SEXDATE ", "Extraction date",
				thecat.ext_date, H_STRING, T_STRING, "%-18s" );
	AddHeadKey( "SEXTIME ", "Extraction time",
				thecat.ext_time, H_STRING, T_STRING, "%-18s" );
	AddHeadKey( "SEXELAPS", "Elapsed time during extraction (s)",
				&thecat.ext_elapsed, H_FLOAT, T_DOUBLE, "%7.1f" );
	AddHeadKey( "SEXBKGND", "Median background level (ADU)",
				&thefield1.backmean, H_EXPO, T_FLOAT, "%-13G" );
	AddHeadKey( "SEXBKDEV", "Median background RMS (ADU)",
				&thefield1.backsig, H_EXPO, T_FLOAT, "%-13G" );
	AddHeadKey( "SEXTHLD ", "Extraction threshold (ADU)",
				&thefield2.dthresh, H_EXPO, T_FLOAT, "%-15G" );
	AddHeadKey( "SEXATHLD", "Analysis threshold (ADU)",
				&thefield1.thresh, H_EXPO, T_FLOAT, "%-15G" );
	AddHeadKey( "SEXNDET ", "Number of raw detections", &thecat.ndetect, H_INT, T_LONG, "%9d" );
	AddHeadKey( "SEXNFIN ", "Final number of extracted sources", &thecat.ntotal, H_INT, T_LONG, "%9d" );
	AddHeadKey( "SEXNPARA", "Number of parameters per source", &thecat.nparam, H_INT, T_LONG, "%3d" );
	AddHeadKey( "SEXPXSCL", "Pixel scale used for measurements (arcsec)", &thefield1.pixscale, H_EXPO, T_DOUBLE, "%-15G" );
	AddHeadKey( "SEXSFWHM", "Source FWHM used for measurements (arcsec)",
	&prefs.seeing_fwhm, H_EXPO, T_DOUBLE, "%-13G" );
	AddHeadKey( "SEXNNWF ", "S/G classification NNW filename",
	thecat.nnw_name, H_STRING, T_STRING, "%-18s" );
	AddHeadKey( "SEXGAIN ", "Gain used (e-/ADU)",
	&prefs.gain, H_EXPO, T_DOUBLE, "%6.2f" );
	AddHeadKey( "SEXFLTR ", "Detection filtering activated (flag)",
	&prefs.filter_flag, H_BOOL, T_LONG, "%1s" );
	AddHeadKey( "SEXFILTN", "Filter filename",
	thecat.filter_name, H_STRING, T_STRING, "%-18s" );

	AddHeadKey( "SEXDETT ", "Detection type",
	&prefs.detect_type, H_STRING, T_STRING, "%-18s" );

	AddHeadKey( "SEXMINAR", "Minimum area used for detection (pixels)",
	&prefs.ext_minarea, H_INT, T_LONG, "%5d" );
	AddHeadKey( "SEXDBLDN", "Number of deblending thresholds",
	&prefs.deblend_nthresh, H_INT, T_LONG, "%3d" );
	AddHeadKey( "SEXDBLDC", "Minimum contrast used for deblending",
	&prefs.deblend_mincont, H_FLOAT, T_DOUBLE, "%8f" );
	AddHeadKey( "SEXCLN  ", "Cleaning activated (flag)",
	&prefs.clean_flag, H_BOOL, T_LONG, "%1s" );
	AddHeadKey( "SEXCLNPA", "Cleaning parameter",
	&prefs.clean_param, H_FLOAT, T_DOUBLE, "%5.2f" );
	AddHeadKey( "SEXCLNST", "Cleaning stack-size",
	&prefs.clean_stacksize, H_INT, T_LONG, "%6d" );
	AddHeadKey( "SEXAPED1", "Fixed photometric aperture #1 (pixels)",
	&prefs.apert[0], H_FLOAT, T_DOUBLE, "%7.1f" );
	AddHeadKey( "SEXAPED2", "Fixed photometric aperture #2 (pixels)",
	&prefs.apert[1], H_FLOAT, T_DOUBLE, "%7.1f" );
	AddHeadKey( "SEXAPED3", "Fixed photometric aperture #3 (pixels)",
	&prefs.apert[2], H_FLOAT, T_DOUBLE, "%7.1f" );
	AddHeadKey( "SEXAPED4", "Fixed photometric aperture #4 (pixels)",
	&prefs.apert[3], H_FLOAT, T_DOUBLE, "%7.1f" );
	AddHeadKey( "SEXAUTP1", "Parameter #1 used for automatic magnitudes",
	&prefs.autoparam[0], H_FLOAT, T_DOUBLE, "%4.1f" );
	AddHeadKey( "SEXAUTP2", "Parameter #2 used for automatic magnitudes",
	&prefs.autoparam[1], H_FLOAT, T_DOUBLE, "%4.1f" );
	AddHeadKey( "SEXPETP1", "Parameter #1 used for Petronsian magnitudes",
	&prefs.autoparam[0], H_FLOAT, T_DOUBLE, "%4.1f" );
	AddHeadKey( "SEXPETP2", "Parameter #2 used for Petrosian magnitudes",
	&prefs.autoparam[1], H_FLOAT, T_DOUBLE, "%4.1f" );
	AddHeadKey( "SEXSATLV", "Saturation level used for flagging (ADU)",
	&prefs.satur_level, H_EXPO, T_DOUBLE, "%-13G" );
	AddHeadKey( "SEXMGZPT", "Zero-point used for magnitudes",
	&prefs.mag_zeropoint, H_FLOAT, T_DOUBLE, "%8.4f" );
	AddHeadKey( "SEXMGGAM", "Gamma used for photographic photometry",
	&prefs.mag_gamma, H_FLOAT, T_DOUBLE, "%4.2f" );
	AddHeadKey( "SEXBKGSX", "Mesh width used for background mapping (pixels)",
	&thefield1.backw, H_INT, T_LONG, "%5d" );
	AddHeadKey( "SEXBKGSY", "Mesh height used for background mapping (pixels)",
	&thefield1.backh, H_INT, T_LONG, "%5d" );
	AddHeadKey( "SEXBKGFX", "Mask width used for background map filtering",
	&thefield1.nbackfx, H_INT, T_LONG, "%3d" );
	AddHeadKey( "SEXBKGFY", "Mask height used for background map filtering",
	&thefield1.nbackfy, H_INT, T_LONG, "%3d" );

	AddHeadKey( "SEXPBKGT", "Background type for photometry",
	&prefs.pback_type, H_STRING, T_STRING, "-18s" );

	AddHeadKey( "SEXPBKGS", "Thickness used for local background (pixels)",
	&prefs.pback_size, H_INT, T_LONG, "%3d" );
	AddHeadKey( "SEXPIXSK", "Pixel stack-size (pixels)",
	&prefs.mem_pixstack, H_INT, T_LONG, "%8d" );
	AddHeadKey( "SEXFBUFS", "Image-buffer height (scanlines)",
	&prefs.mem_bufsize, H_INT, T_LONG, "%5d" );
	AddHeadKey( "SEXMWSCL", "Measurement-weight re-scaling factor",
	&thewfield1.sigfac, H_EXPO, T_FLOAT, "%-13G" );
	AddHeadKey( "SEXDWSCL", "Detection-weight re-scaling factor",
	&thewfield2.sigfac, H_EXPO, T_FLOAT, "%-13G" );

	AddHeadKey( "" );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		AddKey
// Class:		CSextractor
// Purpose:		add emlement to key vector
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::AddObjKey( const char* strName="", const char* strComment="",
							void* pPtr=NULL, h_type nHtype=H_INT,
							t_type nTtype=T_BYTE, const char* strPrintf="",
							const char* strUnit="", const char* strVoucd="", 
							const char* strVounit="", int nNaxis=0,
							int* pNaxisn=NULL, int nNobj=0, 
							int nNbytes=0, long nPos=0 )
{
	strcpy( objkey[m_nObjKey].name, strName );
	strcpy( objkey[m_nObjKey].comment, strComment );
	objkey[m_nObjKey].ptr = pPtr;
	objkey[m_nObjKey].htype = nHtype;
	objkey[m_nObjKey].ttype = nTtype;
	strcpy( objkey[m_nObjKey].printf, strPrintf );
	strcpy( objkey[m_nObjKey].unit, strUnit );
	strcpy( objkey[m_nObjKey].voucd, strVoucd );
	strcpy( objkey[m_nObjKey].vounit, strVounit );
	objkey[m_nObjKey].naxis = nNaxis;
	objkey[m_nObjKey].naxisn = pNaxisn;
	objkey[m_nObjKey].nobj = nNobj;
	objkey[m_nObjKey].nbytes = nNbytes;
	objkey[m_nObjKey].pos = nPos;

	m_nObjKey++;
}

////////////////////////////////////////////////////////////////////
// Method:		InitObjKeyList
// Class:		CSextractor
// Purpose:		initialize my object key list
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::InitObjKeyList( )
{
//keystruct CSextractor::objkey[]= {

	AddObjKey( "NUMBER", "Running object number",
	&outobj.number, H_INT, T_LONG, "%10d", "",
	"meta.record", "" );
	AddObjKey( "EXT_NUMBER", "FITS extension number",
	&outobj2.ext_number, H_INT, T_SHORT, "%3d", "",
	"meta.id;meta.dataset", "" );
	AddObjKey( "FLUX_ISO", "Isophotal flux",
	&outobj2.flux_iso, H_FLOAT, T_FLOAT, "%12.7g", "count",
	"phot.flux", "ct" );
	AddObjKey( "FLUXERR_ISO", "RMS error for isophotal flux",
	&outobj2.fluxerr_iso, H_FLOAT, T_FLOAT, "%12.7g", "count",
	"stat.stdev;phot.flux", "ct" );
	AddObjKey( "MAG_ISO", "Isophotal magnitude",
	&outobj2.mag_iso, H_FLOAT, T_FLOAT, "%8.4f", "mag",
	"phot.mag", "mag" );
	AddObjKey( "MAGERR_ISO", "RMS error for isophotal magnitude",
	&outobj2.magerr_iso, H_FLOAT, T_FLOAT, "%8.4f", "mag",
	"stat.stdev;phot.mag", "mag" );

	AddObjKey( "FLUX_ISOCOR", "Corrected isophotal flux",
	&outobj2.flux_isocor, H_FLOAT, T_FLOAT, "%12.7g", "count",
	"phot.flux", "ct" );
	AddObjKey( "FLUXERR_ISOCOR", "RMS error for corrected isophotal flux",
	&outobj2.fluxerr_isocor, H_FLOAT, T_FLOAT, "%12.7g", "count",
	"stat.stdev;phot.flux", "ct" );
	AddObjKey( "MAG_ISOCOR", "Corrected isophotal magnitude",
	&outobj2.mag_isocor, H_FLOAT, T_FLOAT, "%8.4f", "mag",
	"phot.mag", "mag" );
	AddObjKey( "MAGERR_ISOCOR", "RMS error for corrected isophotal magnitude",
	&outobj2.magerr_isocor, H_FLOAT, T_FLOAT, "%8.4f", "mag",
	"stat.stdev;phot.mag", "mag" );

	AddObjKey( "FLUX_APER", "Flux vector within fixed circular aperture(s)",
	&outobj2.flux_aper, H_FLOAT, T_FLOAT, "%12.7g", "count",
	"phot.flux", "ct", 1, &prefs.flux_apersize );
	AddObjKey( "FLUXERR_APER", "RMS error vector for aperture flux(es)",
	&outobj2.fluxerr_aper, H_FLOAT, T_FLOAT, "%12.7g", "count",
	"stat.stdev;phot.flux", "ct", 1, &prefs.fluxerr_apersize );
	AddObjKey( "MAG_APER", "Fixed aperture magnitude vector",
	&outobj2.mag_aper, H_FLOAT, T_FLOAT, "%8.4f", "mag",
	"phot.mag", "mag", 1, &prefs.mag_apersize );
	AddObjKey( "MAGERR_APER", "RMS error vector for fixed aperture mag.",
	&outobj2.magerr_aper, H_FLOAT, T_FLOAT, "%8.4f", "mag",
	"stat.stdev;phot.mag", "mag", 1, &prefs.magerr_apersize );

	AddObjKey( "FLUX_AUTO", "Flux within a Kron-like elliptical aperture",
	&outobj2.flux_auto, H_FLOAT, T_FLOAT, "%12.7g", "count",
	"phot.flux;meta.main", "ct" );
	AddObjKey( "FLUXERR_AUTO", "RMS error for AUTO flux",
	&outobj2.fluxerr_auto, H_FLOAT, T_FLOAT, "%12.7g", "count",
	"stat.stdev;phot.flux;meta.main", "ct" );
	AddObjKey( "MAG_AUTO", "Kron-like elliptical aperture magnitude",
	&outobj2.mag_auto, H_FLOAT, T_FLOAT, "%8.4f", "mag",
	"phot.mag;meta.main", "mag" );
	AddObjKey( "MAGERR_AUTO", "RMS error for AUTO magnitude",
	&outobj2.magerr_auto, H_FLOAT, T_FLOAT, "%8.4f", "mag",
	"stat.stdev;phot.mag;meta.main", "mag" );

	AddObjKey( "FLUX_PETRO", "Flux within a Petrosian-like elliptical aperture",
	&outobj2.flux_petro, H_FLOAT, T_FLOAT, "%12.7g", "count",
	"phot.flux", "ct" );
	AddObjKey( "FLUXERR_PETRO", "RMS error for PETROsian flux",
	&outobj2.fluxerr_petro, H_FLOAT, T_FLOAT, "%12.7g", "count",
	"stat.stdev;phot.flux", "ct" );
	AddObjKey( "MAG_PETRO", "Petrosian-like elliptical aperture magnitude",
	&outobj2.mag_petro, H_FLOAT, T_FLOAT, "%8.4f", "mag",
	"phot.mag", "mag" );
	AddObjKey( "MAGERR_PETRO", "RMS error for PETROsian magnitude",
	&outobj2.magerr_petro, H_FLOAT, T_FLOAT, "%8.4f", "mag",
	"stat.stdev;phot.mag", "mag" );

	AddObjKey( "FLUX_BEST", "Best of FLUX_AUTO and FLUX_ISOCOR",
	&outobj2.flux_best, H_FLOAT, T_FLOAT, "%12.7g", "count",
	"phot.flux", "ct" );
	AddObjKey( "FLUXERR_BEST", "RMS error for BEST flux",
	&outobj2.fluxerr_best, H_FLOAT, T_FLOAT, "%12.7g", "count",
	"stat.stdev;phot.flux", "ct" );
	AddObjKey( "MAG_BEST", "Best of MAG_AUTO and MAG_ISOCOR",
	&outobj2.mag_best, H_FLOAT, T_FLOAT, "%8.4f", "mag",
	"phot.mag", "mag" );
	AddObjKey( "MAGERR_BEST", "RMS error for MAG_BEST",
	&outobj2.magerr_best, H_FLOAT, T_FLOAT, "%8.4f", "mag",
	"stat.stdev;phot.mag", "mag" );

	AddObjKey( "FLUX_PROFILE", "Flux weighted by the FILTERed profile",
	&outobj2.flux_prof, H_FLOAT, T_FLOAT, "%12.7g", "count"
	"phot.flux", "ct" );
	AddObjKey( "FLUXERR_PROFILE", "RMS error for PROFILE flux",
	&outobj2.fluxerr_prof, H_FLOAT, T_FLOAT, "%12.7g", "count",
	"stat.stdev;phot.flux", "ct" );
	AddObjKey( "MAG_PROFILE", "Magnitude weighted by the FILTERed profile",
	&outobj2.mag_prof, H_FLOAT, T_FLOAT, "%8.4f", "mag",
	"phot.mag", "mag" );
	AddObjKey( "MAGERR_PROFILE", "RMS error for MAG_PROFILE",
	&outobj2.magerr_prof, H_FLOAT, T_FLOAT, "%8.4f", "mag",
	"stat.stdev;phot.mag", "mag" );

	AddObjKey( "FLUX_WIN", "Gaussian-weighted flux",
	&outobj2.flux_win, H_FLOAT, T_FLOAT, "%12.7g", "count",
	"phot.flux", "ct" );
	AddObjKey( "FLUXERR_WIN", "RMS error for WIN flux",
	&outobj2.fluxerr_win, H_FLOAT, T_FLOAT, "%12.7g", "count",
	"stat.stdev;phot.flux", "ct" );
	AddObjKey( "MAG_WIN", "Gaussian-weighted magnitude",
	&outobj2.mag_win, H_FLOAT, T_FLOAT, "%8.4f", "mag",
	"phot.mag", "mag" );
	AddObjKey( "MAGERR_WIN", "RMS error for MAG_WIN",
	&outobj2.magerr_win, H_FLOAT, T_FLOAT, "%8.4f", "mag",
	"stat.stdev;phot.mag", "mag" );

	AddObjKey( "FLUX_SOMFIT", "Flux derived from SOM fit",
	&outobj2.flux_somfit, H_FLOAT, T_FLOAT, "%12.7g", "count",
	"phot.flux", "ct" );
	AddObjKey( "FLUXERR_SOMFIT", "RMS error for SOMFIT flux",
	&outobj2.fluxerr_somfit, H_FLOAT, T_FLOAT, "%12.7g", "count",
	"stat.stdev;phot.flux", "ct" );
	AddObjKey( "MAG_SOMFIT", "Magnitude derived from SOM fit",
	&outobj2.mag_somfit, H_FLOAT, T_FLOAT, "%8.4f", "mag",
	"phot.mag", "mag" );
	AddObjKey( "MAGERR_SOMFIT", "Magnitude error derived from SOM fit",
	&outobj2.magerr_somfit, H_FLOAT, T_FLOAT, "%8.4f", "mag",
	"stat.stdev;phot.mag", "mag" );
	AddObjKey( "ERROR_SOMFIT", "Reduced Chi-square error of the SOM fit",
	&outobj2.stderr_somfit, H_FLOAT, T_FLOAT, "%10.4g", "",
	"stat.fit.chi2", "" );
	AddObjKey( "VECTOR_SOMFIT", "Position vector of the winning SOM node",
	&outobj2.vector_somfit, H_FLOAT, T_FLOAT, "%5.2f", "",
	"src.morph.param", "", 1, &prefs.somfit_vectorsize );

	AddObjKey( "FLUX_GALFIT", "Flux derived from the galaxy fit",
	&outobj2.flux_galfit, H_FLOAT, T_FLOAT, "%12.7g", "count",
	"phot.flux;stat.fit.param", "ct" );
//
//	AddObjKey( "FLUXERR_GALFIT", "RMS error for GALFIT flux",
//	&outobj2.fluxerr_galfit, H_FLOAT, T_FLOAT, "%12.7g", "count" );
//
	AddObjKey( "MAG_GALFIT", "Magnitude derived from galaxy fit",
	&outobj2.mag_galfit, H_FLOAT, T_FLOAT, "%8.4f", "mag",
	"phot.mag;stat.fit.param", "mag" );

//
//	AddObjKey( "MAGERR_GALFIT", "Magnitude error derived from galaxy fit",
//	&outobj2.magerr_galfit, H_FLOAT, T_FLOAT, "%8.4f", "mag" );
//	AddObjKey( "ERROR_GALFIT", "Reduced Chi-square error of the galaxy fit",
//	&outobj2.stderr_galfit, H_FLOAT, T_FLOAT, "%10g", "" );

	AddObjKey( "GALDANG_IMAGE", "Galaxy disk position angle  from the galaxy fit",
	&outobj2.gdposang, H_FLOAT, T_FLOAT, "%5.1f", "deg",
	"pos.posAng;stat.fit.param", "deg" );
	AddObjKey( "GALDSCALE_IMAGE", "Galaxy disk-scale from the galaxy fit",
	&outobj2.gdscale, H_FLOAT, T_FLOAT, "%9.3f", "pixel",
	"src.morph.scLength;stat.fit.param", "pix" );
	AddObjKey( "GALDASPEC_IMAGE", "Galaxy disk aspect ratio from the galaxy fit",
	&outobj2.gdaspect, H_FLOAT, T_FLOAT, "%5.3f", "",
	"phys.size.axisRatio;stat.fit.param", "" );
	AddObjKey( "GALDE1_IMAGE", "Galaxy disk ellipticity #1 from the galaxy fit",
	&outobj2.gde1, H_FLOAT, T_FLOAT, "%6.4f", "",
	"src.ellipticity", "" );
	AddObjKey( "GALDE2_IMAGE", "Galaxy disk ellipticity #2 from the galaxy fit",
	&outobj2.gde2, H_FLOAT, T_FLOAT, "%6.4f", "",
	"src.ellipticity", "" );
	AddObjKey( "GALBRATIO_IMAGE", "Galaxy bulge ratio from the galaxy fit",
	&outobj2.gbratio, H_FLOAT, T_FLOAT, "%5.3f", "",
	"src.morph.param;stat.fit.param", "" );
	AddObjKey( "GALBANG_IMAGE", "Galaxy bulge position angle  from the galaxy fit",
	&outobj2.gbposang, H_FLOAT, T_FLOAT, "%5.1f", "deg",
	"pos.posAng;stat.fit.param", "deg" );
	AddObjKey( "GALBSCALE_IMAGE", "Galaxy bulge-scale from the galaxy fit",
	&outobj2.gbscale, H_FLOAT, T_FLOAT, "%9.3f", "pixel",
	"src.morph.scLength;stat.fit.param", "pix" );
	AddObjKey( "GALBASPEC_IMAGE", "Galaxy bulge aspect ratio from the galaxy fit",
	&outobj2.gbaspect, H_FLOAT, T_FLOAT, "%5.3f", "",
	"phys.size.axisRatio;stat.fit.param", "" );

	AddObjKey( "KRON_RADIUS", "Kron apertures in units of A or B",
	&outobj2.kronfactor, H_FLOAT, T_FLOAT, "%5.2f", "",
	"arith.factor;arith.ratio", "" );
	AddObjKey( "PETRO_RADIUS", "Petrosian apertures in units of A or B",
	&outobj2.petrofactor, H_FLOAT, T_FLOAT, "%5.2f", "",
	"arith.factor;arith.ratio", "" );
	AddObjKey( "BACKGROUND", "Background at centroid position",
	&outobj.bkg, H_FLOAT, T_FLOAT, "%12.7g", "count",
	"instr.skyLevel", "ct" );
	AddObjKey( "THRESHOLD", "Detection threshold above background",
	&outobj.dthresh, H_FLOAT, T_FLOAT, "%12.7g", "count",
	"instr.sensitivity;phot.flux.sb", "ct" );
	AddObjKey( "FLUX_MAX", "Peak flux above background",
	&outobj.peak, H_FLOAT, T_FLOAT, "%12.7g", "count",
	"phot.flux.sb;stat.max", "ct" );
	AddObjKey( "ISOAREA_IMAGE", "Isophotal area above Analysis threshold",
	&outobj.npix, H_INT, T_LONG, "%9d", "pixel**2",
	"phys.area", "pix2" );
	AddObjKey( "ISOAREAF_IMAGE", "Isophotal area (filtered) above Detection threshold",
	&outobj.fdnpix, H_INT, T_LONG, "%9d", "pixel**2",
	"phys.area", "pix2" );

	AddObjKey( "XMIN_IMAGE", "Minimum x-coordinate among detected pixels",
	&outobj.xmin, H_INT, T_LONG, "%10d", "pixel",
	"pos.cartesian.x;stat.min", "pix" );
	AddObjKey( "YMIN_IMAGE", "Minimum y-coordinate among detected pixels",
	&outobj.ymin, H_INT, T_LONG, "%10d", "pixel",
	"pos.cartesian.y;stat.min", "pix" );
	AddObjKey( "XMAX_IMAGE", "Maximum x-coordinate among detected pixels",
	&outobj.xmax, H_INT, T_LONG, "%10d", "pixel",
	"pos.cartesian.x;stat.max", "pix" );
	AddObjKey( "YMAX_IMAGE", "Maximum y-coordinate among detected pixels",
	&outobj.ymax, H_INT, T_LONG, "%10d", "pixel",
	"pos.cartesian.y;stat.max", "pix" );

	AddObjKey( "XPEAK_IMAGE", "x-coordinate of the brightest pixel",
	&outobj.peakx, H_INT, T_LONG, "%10d", "pixel",
	"pos.cartesian.x", "pix" );
	AddObjKey( "YPEAK_IMAGE", "y-coordinate of the brightest pixel",
	&outobj.peaky, H_INT, T_LONG, "%10d", "pixel",
	"pos.cartesian.y", "pix" );
	AddObjKey( "XPEAK_WORLD", "World-x coordinate of the brightest pixel",
	&outobj2.peakxw, H_FLOAT, T_DOUBLE, "%15.10e", "deg",
	"pos.eq.ra", "deg" );
	AddObjKey( "YPEAK_WORLD", "World-y coordinate of the brightest pixel",
	&outobj2.peakyw, H_FLOAT, T_DOUBLE, "%15.10e", "deg",
	"pos.eq.dec", "deg" );

	AddObjKey( "ALPHAPEAK_SKY", "Right ascension of brightest pix (native)",
	&outobj2.peakalphas, H_FLOAT, T_DOUBLE, "%11.7f", "deg",
	"pos.eq.ra", "deg" );
	AddObjKey( "DELTAPEAK_SKY", "Declination of brightest pix (native)",
	&outobj2.peakdeltas, H_FLOAT, T_DOUBLE, "%+11.7f", "deg",
	"pos.eq.dec", "deg" );

	AddObjKey( "ALPHAPEAK_J2000", "Right ascension of brightest pix (J2000)",
	&outobj2.peakalpha2000, H_FLOAT, T_DOUBLE, "%11.7f", "deg",
	"pos.eq.ra", "deg" );
	AddObjKey( "DELTAPEAK_J2000", "Declination of brightest pix (J2000)",
	&outobj2.peakdelta2000, H_FLOAT, T_DOUBLE, "%+11.7f", "deg",
	"pos.eq.dec", "deg" );

	AddObjKey( "ALPHAPEAK_B1950", "Right ascension of brightest pix (B1950)",
	&outobj2.peakalpha1950, H_FLOAT, T_DOUBLE, "%11.7f", "deg",
	"pos.eq.ra", "deg" );
	AddObjKey( "DELTAPEAK_B1950", "Declination of brightest pix (B1950)",
	&outobj2.peakdelta1950, H_FLOAT, T_DOUBLE, "%+11.7f", "deg",
	"pos.eq.dec", "deg" );

	AddObjKey( "X_IMAGE", "Object position along x",
	&outobj2.sposx, H_FLOAT, T_FLOAT, "%10.3f", "pixel",
	"pos.cartesian.x;pos.barycenter;instr.det;meta.main", "pix" );
	AddObjKey( "Y_IMAGE", "Object position along y",
	&outobj2.sposy, H_FLOAT, T_FLOAT, "%10.3f", "pixel",
	"pos.cartesian.y;pos.barycenter;instr.det;meta.main", "pix" );
	AddObjKey( "X_IMAGE_DBL", "Object position along x (double precision)",
	&outobj2.posx, H_FLOAT, T_DOUBLE, "%10.3f", "pixel",
	"pos.cartesian.x;pos.barycenter;instr.det", "pix" );
	AddObjKey( "Y_IMAGE_DBL", "Object position along y (double precision)",
	&outobj2.posy, H_FLOAT, T_DOUBLE, "%10.3f", "pixel",
	"pos.cartesian.x;pos.barycenter;instr.det", "pix" );
	AddObjKey( "X_WORLD", "Barycenter position along world x axis",
	&outobj2.mxw, H_FLOAT, T_DOUBLE, "%15.10e", "deg",
	"pos.eq.ra", "deg" );
	AddObjKey( "Y_WORLD", "Barycenter position along world y axis",
	&outobj2.myw, H_FLOAT, T_DOUBLE, "%15.10e", "deg",
	"pos.eq.dec", "deg" );
	AddObjKey( "X_MAMA", "Barycenter position along MAMA x axis",
	&outobj2.mamaposx, H_FLOAT, T_DOUBLE, "%8.1f", "m**(-6)",
	"pos.cartesian.x;instr.det;pos.barycenter", "um" );
	AddObjKey( "Y_MAMA", "Barycenter position along MAMA y axis",
	&outobj2.mamaposy, H_FLOAT, T_DOUBLE, "%8.1f", "m**(-6)",
	"pos.cartesian.y;instr.det;pos.barycenter", "um" );

	AddObjKey( "ALPHA_SKY", "Right ascension of barycenter (native)",
	&outobj2.alphas, H_FLOAT, T_DOUBLE, "%11.7f", "deg",
	"pos.eq.ra;pos.barycenter", "deg" );
	AddObjKey( "DELTA_SKY", "Declination of barycenter (native)",
	&outobj2.deltas, H_FLOAT, T_DOUBLE, "%+11.7f", "deg",
	"pos.eq.dec;pos.barycenter", "deg" );

	AddObjKey( "ALPHA_J2000", "Right ascension of barycenter (J2000)",
	&outobj2.alpha2000, H_FLOAT, T_DOUBLE, "%11.7f", "deg",
	"pos.eq.ra;pos.barycenter;meta.main", "deg" );
	AddObjKey( "DELTA_J2000", "Declination of barycenter (J2000)",
	&outobj2.delta2000, H_FLOAT, T_DOUBLE, "%+11.7f", "deg",
	"pos.eq.dec;pos.barycenter;meta.main", "deg" );

	AddObjKey( "ALPHA_B1950", "Right ascension of barycenter (B1950)",
	&outobj2.alpha1950, H_FLOAT, T_DOUBLE, "%11.7f", "deg",
	"pos.eq.ra;pos.barycenter", "deg" );
	AddObjKey( "DELTA_B1950", "Declination of barycenter (B1950)",
	&outobj2.delta1950, H_FLOAT, T_DOUBLE, "%+11.7f", "deg",
	"pos.eq.dec;pos.barycenter", "deg" );

	AddObjKey( "X2_IMAGE", "Variance along x",
	&outobj.mx2, H_EXPO, T_DOUBLE, "%15.10e", "pixel**2",
	"src.impactParam;instr.det", "pix2" );
	AddObjKey( "Y2_IMAGE", "Variance along y",
	&outobj.my2, H_EXPO, T_DOUBLE, "%15.10e", "pixel**2",
	"src.impactParam;instr.det", "pix2" );
	AddObjKey( "XY_IMAGE", "Covariance between x and y",
	&outobj.mxy, H_EXPO, T_DOUBLE, "%15.10e", "pixel**2",
	"src.impactParam;instr.det", "pix2" );
	AddObjKey( "X2_WORLD", "Variance along X-WORLD (alpha)",
	&outobj2.mx2w, H_EXPO, T_DOUBLE, "%15.10e", "deg**2",
	"src.impactParam", "deg2" );
	AddObjKey( "Y2_WORLD", "Variance along Y-WORLD (delta)",
	&outobj2.my2w, H_EXPO, T_DOUBLE, "%15.10e", "deg**2",
	"src.impactParam", "deg2" );
	AddObjKey( "XY_WORLD", "Covariance between X-WORLD and Y-WORLD",
	&outobj2.mxyw, H_EXPO, T_DOUBLE, "%15.10e", "deg**2",
	"src.impactParam", "deg2" );

	AddObjKey( "CXX_IMAGE", "Cxx object ellipse parameter",
	&outobj.cxx, H_EXPO, T_FLOAT, "%12.7e", "pixel**(-2)",
	"src.impactParam;instr.det", "pix-2" );
	AddObjKey( "CYY_IMAGE", "Cyy object ellipse parameter",
	&outobj.cyy, H_EXPO, T_FLOAT, "%12.7e", "pixel**(-2)",
	"src.impactParam;instr.det", "pix-2" );
	AddObjKey( "CXY_IMAGE", "Cxy object ellipse parameter",
	&outobj.cxy, H_EXPO, T_FLOAT, "%12.7e", "pixel**(-2)",
	"src.impactParam;instr.det", "pix-2" );
	AddObjKey( "CXX_WORLD", "Cxx object ellipse parameter (WORLD units)",
	&outobj2.cxxw, H_EXPO, T_FLOAT, "%12.7e", "deg**(-2)",
	"src.impactParam", "deg-2" );
	AddObjKey( "CYY_WORLD", "Cyy object ellipse parameter (WORLD units)",
	&outobj2.cyyw, H_EXPO, T_FLOAT, "%12.7e", "deg**(-2)",
	"src.impactParam", "deg-2" );
	AddObjKey( "CXY_WORLD", "Cxy object ellipse parameter (WORLD units)",
	&outobj2.cxyw, H_EXPO, T_FLOAT, "%12.7e", "deg**(-2)",
	"src.impactParam", "deg-2" );

	AddObjKey( "A_IMAGE", "Profile RMS along major axis",
	&outobj.a, H_FLOAT, T_FLOAT, "%9.3f", "pixel",
	"phys.size.smajAxis;instr.det;meta.main", "pix" );
	AddObjKey( "B_IMAGE", "Profile RMS along minor axis",
	&outobj.b, H_FLOAT, T_FLOAT, "%9.3f", "pixel",
	"phys.size.sminAxis;instr.det;meta.main", "pix" );
	AddObjKey( "THETA_IMAGE", "Position angle (CCW/x)",
	&outobj.theta, H_FLOAT, T_FLOAT, "%5.1f", "deg",
	"pos.posAng;instr.det;meta.main", "deg" );
	AddObjKey( "A_WORLD", "Profile RMS along major axis (world units)",
	&outobj2.aw, H_FLOAT, T_FLOAT, "%12.7g", "deg",
	"phys.size.smajAxis;meta.main", "deg" );
	AddObjKey( "B_WORLD", "Profile RMS along minor axis (world units)",
	&outobj2.bw, H_FLOAT, T_FLOAT, "%12.7g", "deg",
	"phys.size.sminAxis;meta.main", "deg" );
	AddObjKey( "THETA_WORLD", "Position angle (CCW/world-x)",
	&outobj2.thetaw, H_FLOAT, T_FLOAT, "%5.1f", "deg",
	"pos.posAng", "deg" );
	AddObjKey( "THETA_SKY", "Position angle (east of north) (native)",
	&outobj2.thetas, H_FLOAT, T_FLOAT, "%+6.2f", "deg",
	"pos.posAng", "deg" );
	AddObjKey( "THETA_J2000", "Position angle (east of north) (J2000)",
	&outobj2.theta2000, H_FLOAT, T_FLOAT, "%+6.2f", "deg",
	"pos.posAng;meta.main", "deg" );
	AddObjKey( "THETA_B1950", "Position angle (east of north) (B1950)",
	&outobj2.theta1950, H_FLOAT, T_FLOAT, "%+6.2f", "deg",
	"pos.posAng", "deg" );

	AddObjKey( "ERRX2_IMAGE", "Variance of position along x",
	&outobj.poserr_mx2, H_EXPO, T_DOUBLE, "%15.10e", "pixel**2",
	"stat.variance;pos.errorEllipse;instr.det", "pix2" );
	AddObjKey( "ERRY2_IMAGE", "Variance of position along y",
	&outobj.poserr_my2, H_EXPO, T_DOUBLE, "%15.10e", "pixel**2",
	"stat.variance;pos.errorEllipse;instr.det", "pix2" );
	AddObjKey( "ERRXY_IMAGE", "Covariance of position between x and y",
	&outobj.poserr_mxy, H_EXPO, T_DOUBLE, "%15.10e", "pixel**2",
	"stat.covariance;pos.errorEllipse;instr.det", "pix2" );
	AddObjKey( "ERRX2_WORLD", "Variance of position along X-WORLD (alpha)",
	&outobj2.poserr_mx2w, H_EXPO, T_DOUBLE, "%15.10e", "deg**2",
	"stat.variance;pos.errorEllipse", "deg2" );
	AddObjKey( "ERRY2_WORLD", "Variance of position along Y-WORLD (delta)",
	&outobj2.poserr_my2w, H_EXPO, T_DOUBLE, "%15.10e", "deg**2",
	"stat.variance;pos.errorEllipse", "deg2" );
	AddObjKey( "ERRXY_WORLD", "Covariance of position X-WORLD/Y-WORLD",
	&outobj2.poserr_mxyw, H_EXPO, T_DOUBLE, "%15.10e", "deg**2",
	"stat.covariance;pos.errorEllipse", "deg2" );

	AddObjKey( "ERRCXX_IMAGE", "Cxx error ellipse parameter",
	&outobj2.poserr_cxx, H_EXPO, T_FLOAT, "%12.7g", "pixel**(-2)",
	"src.impactParam;pos.errorEllipse;instr.det", "pix-2" );
	AddObjKey( "ERRCYY_IMAGE", "Cyy error ellipse parameter",
	&outobj2.poserr_cyy, H_EXPO, T_FLOAT, "%12.7g", "pixel**(-2)",
	"src.impactParam;pos.errorEllipse;instr.det", "pix-2" );
	AddObjKey( "ERRCXY_IMAGE", "Cxy error ellipse parameter",
	&outobj2.poserr_cxy, H_EXPO, T_FLOAT, "%12.7g", "pixel**(-2)",
	"src.impactParam;pos.errorEllipse;instr.det", "pix-2" );
	AddObjKey( "ERRCXX_WORLD", "Cxx error ellipse parameter (WORLD units)",
	&outobj2.poserr_cxxw, H_EXPO, T_FLOAT, "%12.7g", "deg**(-2)",
	"src.impactParam;pos.errorEllipse", "deg-2" );
	AddObjKey( "ERRCYY_WORLD", "Cyy error ellipse parameter (WORLD units)",
	&outobj2.poserr_cyyw, H_EXPO, T_FLOAT, "%12.7g", "deg**(-2)",
	"src.impactParam;pos.errorEllipse", "deg-2" );
	AddObjKey( "ERRCXY_WORLD", "Cxy error ellipse parameter (WORLD units)",
	&outobj2.poserr_cxyw, H_EXPO, T_FLOAT, "%12.7g", "deg**(-2)",
	"src.impactParam;pos.errorEllipse", "deg-2" );

	AddObjKey( "ERRA_IMAGE", "RMS position error along major axis",
	&outobj2.poserr_a, H_FLOAT, T_FLOAT, "%8.4f", "pixel"
	"stat.stdev;stat.max;pos.errorEllipse;instr.det;meta.main", "pix" );
	AddObjKey( "ERRB_IMAGE", "RMS position error along minor axis",
	&outobj2.poserr_b, H_FLOAT, T_FLOAT, "%8.4f", "pixel",
	"stat.stdev;stat.min;pos.errorEllipse;instr.det;meta.main", "pix" );
	AddObjKey( "ERRTHETA_IMAGE", "Error ellipse position angle (CCW/x)",
	&outobj2.poserr_theta, H_FLOAT, T_FLOAT, "%5.1f", "deg",
	"pos.posAng;pos.errorEllipse;instr.det;meta.main", "deg" );
	AddObjKey( "ERRA_WORLD", "World RMS position error along major axis",
	&outobj2.poserr_aw, H_FLOAT, T_FLOAT, "%12.7g", "pixel",
	"stat.stdev;stat.max;pos.errorEllipse;meta.main", "deg" );
	AddObjKey( "ERRB_WORLD", "World RMS position error along minor axis",
	&outobj2.poserr_bw, H_FLOAT, T_FLOAT, "%12.7g", "pixel",
	"stat.stdev;stat.min;pos.errorEllipse;meta.main", "deg" );
	AddObjKey( "ERRTHETA_WORLD", "Error ellipse pos. angle (CCW/world-x)",
	&outobj2.poserr_thetaw, H_FLOAT, T_FLOAT, "%5.1f", "deg",
	"pos.posAng;pos.errorEllipse", "deg" );
	AddObjKey( "ERRTHETA_SKY", "Native error ellipse pos. angle (east of north)",
	&outobj2.poserr_thetas, H_FLOAT, T_FLOAT, "%5.1f", "deg",
	"pos.posAng;pos.errorEllipse", "deg" );
	AddObjKey( "ERRTHETA_J2000", "J2000 error ellipse pos. angle (east of north)",
	&outobj2.poserr_theta2000, H_FLOAT, T_FLOAT, "%5.1f", "deg",
	"pos.posAng;pos.errorEllipse;meta.main", "deg" );
	AddObjKey( "ERRTHETA_B1950", "B1950 error ellipse pos. angle (east of north)",
	&outobj2.poserr_theta1950, H_FLOAT, T_FLOAT, "%5.1f", "deg",
	"pos.posAng;pos.errorEllipse", "deg" );

	AddObjKey( "XWIN_IMAGE", "Windowed position estimate along x",
	&outobj2.winpos_x, H_FLOAT, T_DOUBLE, "%10.3f", "pixel",
	"pos.cartesian.x;instr.det", "pix" );
	AddObjKey( "YWIN_IMAGE", "Windowed position estimate along y",
	&outobj2.winpos_y, H_FLOAT, T_DOUBLE, "%10.3f", "pixel",
	"pos.cartesian.y;instr.det", "pix" );

	AddObjKey( "XWIN_WORLD", "Windowed position along world x axis",
	&outobj2.winpos_xw, H_FLOAT, T_DOUBLE, "%15.10e", "deg",
	"pos.eq.ra", "deg" );
	AddObjKey( "YWIN_WORLD", "Windowed position along world y axis",
	&outobj2.winpos_yw, H_FLOAT, T_DOUBLE, "%15.10e", "deg",
	"pos.eq.dec", "deg" );

	AddObjKey( "ALPHAWIN_SKY", "Windowed right ascension  (native)",
	&outobj2.winpos_alphas, H_FLOAT, T_DOUBLE, "%11.7f", "deg",
	"pos.eq.ra", "deg" );
	AddObjKey( "DELTAWIN_SKY", "Windowed declination (native)",
	&outobj2.winpos_deltas, H_FLOAT, T_DOUBLE, "%+11.7f", "deg",
	"pos.eq.dec", "deg" );

	AddObjKey( "ALPHAWIN_J2000", "Windowed right ascension (J2000)",
	&outobj2.winpos_alpha2000, H_FLOAT, T_DOUBLE, "%11.7f", "deg",
	"pos.eq.ra", "deg" );
	AddObjKey( "DELTAWIN_J2000", "windowed declination (J2000)",
	&outobj2.winpos_delta2000, H_FLOAT, T_DOUBLE, "%+11.7f", "deg",
	"pos.eq.dec", "deg" );

	AddObjKey( "ALPHAWIN_B1950", "Windowed right ascension (B1950)",
	&outobj2.winpos_alpha1950, H_FLOAT, T_DOUBLE, "%11.7f", "deg",
	"pos.eq.ra", "deg" );
	AddObjKey( "DELTAWIN_B1950", "Windowed declination (B1950)",
	&outobj2.winpos_delta1950, H_FLOAT, T_DOUBLE, "%+11.7f", "deg",
	"pos.eq.deg", "deg" );
	AddObjKey( "X2WIN_IMAGE", "Windowed variance along x",
	&outobj2.win_mx2, H_EXPO, T_DOUBLE, "%15.10e", "pixel**2",
 	"src.impactParam;instr.det", "pix2" );
	AddObjKey( "Y2WIN_IMAGE", "Windowed variance along y",
	&outobj2.win_my2, H_EXPO, T_DOUBLE, "%15.10e", "pixel**2",
	"src.impactParam;instr.det", "pix2" );
	AddObjKey( "XYWIN_IMAGE", "Windowed covariance between x and y",
	&outobj2.win_mxy, H_EXPO, T_DOUBLE, "%15.10e", "pixel**2",
	"src.impactParam;instr.det", "pix2" );
	AddObjKey( "X2WIN_WORLD", "Windowed variance along X-WORLD (alpha)",
	&outobj2.win_mx2w, H_EXPO, T_DOUBLE, "%15.10e", "deg**2",
	"src.impactParam", "deg2" );
	AddObjKey( "Y2WIN_WORLD", "Windowed variance along Y-WORLD (delta)",
	&outobj2.win_my2w, H_EXPO, T_DOUBLE, "%15.10e", "deg**2",
	"src.impactParam", "deg2" );
	AddObjKey( "XYWIN_WORLD", "Windowed covariance between X-WORLD and Y-WORLD",
	&outobj2.win_mxyw, H_EXPO, T_DOUBLE, "%15.10e", "deg**2",
	"src.impactParam", "deg2" );

	AddObjKey( "CXXWIN_IMAGE", "Windowed Cxx object ellipse parameter",
	&outobj2.win_cxx, H_EXPO, T_FLOAT, "%12.7e", "pixel**(-2)",
	"src.impactParam;instr.det", "pix-2" );
	AddObjKey( "CYYWIN_IMAGE", "Windowed Cyy object ellipse parameter",
	&outobj2.win_cyy, H_EXPO, T_FLOAT, "%12.7e", "pixel**(-2)",
	"src.impactParam;instr.det", "pix-2" );
	AddObjKey( "CXYWIN_IMAGE", "Windowed Cxy object ellipse parameter",
	&outobj2.win_cxy, H_EXPO, T_FLOAT, "%12.7e", "pixel**(-2)",
	"src.impactParam;instr.det", "pix-2" );
	AddObjKey( "CXXWIN_WORLD", "Windowed Cxx object ellipse parameter (WORLD units)",
	&outobj2.win_cxxw, H_EXPO, T_FLOAT, "%12.7e", "deg**(-2)",
	"src.impactParam", "deg-2" );
	AddObjKey( "CYYWIN_WORLD", "Windowed Cyy object ellipse parameter (WORLD units)",
	&outobj2.win_cyyw, H_EXPO, T_FLOAT, "%12.7e", "deg**(-2)",
	"src.impactParam", "deg-2" );
	AddObjKey( "CXYWIN_WORLD", "Windowed Cxy object ellipse parameter (WORLD units)",
	&outobj2.win_cxyw, H_EXPO, T_FLOAT, "%12.7e", "deg**(-2)",
	"src.impactParam", "deg-2" );

	AddObjKey( "AWIN_IMAGE", "Windowed profile RMS along major axis",
	&outobj2.win_a, H_FLOAT, T_FLOAT, "%9.3f", "pixel",
	"phys.size.smajAxis;instr.det", "pix" );
	AddObjKey( "BWIN_IMAGE", "Windowed profile RMS along minor axis",
	&outobj2.win_b, H_FLOAT, T_FLOAT, "%9.3f", "pixel",
	"phys.size.sminAxis;instr.det", "pix" );
	AddObjKey( "THETAWIN_IMAGE", "Windowed position angle (CCW/x)",
	&outobj2.win_theta, H_FLOAT, T_FLOAT, "%5.1f", "deg",
	"pos.posAng;instr.det", "deg" );
	AddObjKey( "AWIN_WORLD", "Windowed profile RMS along major axis (world units)",
	&outobj2.win_aw, H_FLOAT, T_FLOAT, "%12.7g", "deg",
	"phys.size.smajAxis", "deg" );
	AddObjKey( "BWIN_WORLD", "Windowed profile RMS along minor axis (world units)",
	&outobj2.win_bw, H_FLOAT, T_FLOAT, "%12.7g", "deg",
	"phys.size.sminAxis", "deg" );
	AddObjKey( "THETAWIN_WORLD", "Windowed position angle (CCW/world-x)",
	&outobj2.win_thetaw, H_FLOAT, T_FLOAT, "%5.1f", "deg",
	"pos.posAng", "deg" );
	AddObjKey( "THETAWIN_SKY", "Windowed position angle (east of north) (native)",
	&outobj2.win_thetas, H_FLOAT, T_FLOAT, "%+6.2f", "deg",
	"pos.posAng", "deg" );
	AddObjKey( "THETAWIN_J2000", "Windowed position angle (east of north) (J2000)",
	&outobj2.win_theta2000, H_FLOAT, T_FLOAT, "%+6.2f", "deg",
	"pos.posAng", "deg" );
	AddObjKey( "THETAWIN_B1950", "Windowed position angle (east of north) (B1950)",
	&outobj2.win_theta1950, H_FLOAT, T_FLOAT, "%+6.2f", "deg",
	"pos.posAng", "deg" );

	AddObjKey( "ERRX2WIN_IMAGE", "Variance of windowed pos along x",
	&outobj2.winposerr_mx2, H_EXPO, T_DOUBLE, "%15.10e", "pixel**2",
	"stat.variance;pos.errorEllipse;instr.det", "pix2" );
	AddObjKey( "ERRY2WIN_IMAGE", "Variance of windowed pos along y",
	&outobj2.winposerr_my2, H_EXPO, T_DOUBLE, "%15.10e", "pixel**2",
	"stat.variance;pos.errorEllipse;instr.det", "pix2" );
	AddObjKey( "ERRXYWIN_IMAGE", "Covariance of windowed pos between x and y",
	&outobj2.winposerr_mxy, H_EXPO, T_DOUBLE, "%15.10e", "pixel**2",
	"stat.covariance;pos.errorEllipse;instr.det", "pix2" );
	AddObjKey( "ERRX2WIN_WORLD", "Variance of windowed pos along X-WORLD (alpha)",
	&outobj2.winposerr_mx2w, H_EXPO, T_DOUBLE, "%15.10e", "deg**2",
	"stat.variance;pos.errorEllipse", "deg2" );
	AddObjKey( "ERRY2WIN_WORLD", "Variance of windowed pos along Y-WORLD (delta)",
	&outobj2.winposerr_my2w, H_EXPO, T_DOUBLE, "%15.10e", "deg**2",
	"stat.variance;pos.errorEllipse", "deg2" );
	AddObjKey( "ERRXYWIN_WORLD", "Covariance of windowed pos X-WORLD/Y-WORLD",
	&outobj2.winposerr_mxyw, H_EXPO, T_DOUBLE, "%15.10e", "deg**2",
	"stat.covariance;pos.errorEllipse", "deg2" );

	AddObjKey( "ERRCXXWIN_IMAGE", "Cxx windowed error ellipse parameter",
	&outobj2.winposerr_cxx, H_EXPO, T_FLOAT, "%12.7g", "pixel**(-2)",
	"src.impactParam;pos.errorEllipse;instr.det", "pix-2" );
	AddObjKey( "ERRCYYWIN_IMAGE", "Cyy windowed error ellipse parameter",
	&outobj2.winposerr_cyy, H_EXPO, T_FLOAT, "%12.7g", "pixel**(-2)",
	"src.impactParam;pos.errorEllipse;instr.det", "pix-2" );
	AddObjKey( "ERRCXYWIN_IMAGE", "Cxy windowed error ellipse parameter",
	&outobj2.winposerr_cxy, H_EXPO, T_FLOAT, "%12.7g", "pixel**(-2)",
	"src.impactParam;pos.errorEllipse;instr.det", "pix-2" );
	AddObjKey( "ERRCXXWIN_WORLD", "Cxx windowed error ellipse parameter (WORLD units)",
	&outobj2.winposerr_cxxw, H_EXPO, T_FLOAT, "%12.7g", "deg**(-2)",
	"src.impactParam;pos.errorEllipse", "deg-2" );
	AddObjKey( "ERRCYYWIN_WORLD", "Cyy windowed error ellipse parameter (WORLD units)",
	&outobj2.winposerr_cyyw, H_EXPO, T_FLOAT, "%12.7g", "deg**(-2)",
	"src.impactParam;pos.errorEllipse", "deg-2" );
	AddObjKey( "ERRCXYWIN_WORLD", "Cxy windowed error ellipse parameter (WORLD units)",
	&outobj2.winposerr_cxyw, H_EXPO, T_FLOAT, "%12.7g", "deg**(-2)",
	"src.impactParam;pos.errorEllipse", "deg-2" );

	AddObjKey( "ERRAWIN_IMAGE", "RMS windowed pos error along major axis",
	&outobj2.winposerr_a, H_FLOAT, T_FLOAT, "%8.4f", "pixel",
	"stat.stdev;stat.max;pos.errorEllipse;instr.det", "pix" );
	AddObjKey( "ERRBWIN_IMAGE", "RMS windowed pos error along minor axis",
	&outobj2.winposerr_b, H_FLOAT, T_FLOAT, "%8.4f", "pixel",
	"stat.stdev;stat.min;pos.errorEllipse;instr.det", "pix" );
	AddObjKey( "ERRTHETAWIN_IMAGE", "Windowed error ellipse pos angle (CCW/x)",
	&outobj2.winposerr_theta, H_FLOAT, T_FLOAT, "%5.1f", "deg",
	"pos.posAng;pos.errorEllipse;instr.det", "deg" );
	AddObjKey( "ERRAWIN_WORLD", "World RMS windowed pos error along major axis",
	&outobj2.winposerr_aw, H_FLOAT, T_FLOAT, "%12.7g", "pixel",
	"stat.stdev;stat.max;pos.errorEllipse", "deg" );
	AddObjKey( "ERRBWIN_WORLD", "World RMS windowed pos error along minor axis",
	&outobj2.winposerr_bw, H_FLOAT, T_FLOAT, "%12.7g", "pixel",
	"stat.stdev;stat.min;pos.errorEllipse", "deg" );
	AddObjKey( "ERRTHETAWIN_WORLD", "Windowed error ellipse pos. angle (CCW/world-x)",
	&outobj2.winposerr_thetaw, H_FLOAT, T_FLOAT, "%5.1f", "deg",
	"pos.posAng;pos.errorEllipse", "deg" );
	AddObjKey( "ERRTHETAWIN_SKY", "Native windowed error ellipse pos. angle (east of north)",
	&outobj2.winposerr_thetas, H_FLOAT, T_FLOAT, "%5.1f", "deg",
	"pos.posAng;pos.errorEllipse", "deg" );
	AddObjKey( "ERRTHETAWIN_J2000", "J2000 windowed error ellipse pos. angle (east of north)",
	&outobj2.winposerr_theta2000, H_FLOAT, T_FLOAT, "%5.1f", "deg",
	"pos.posAng;pos.errorEllipse", "deg" );
	AddObjKey( "ERRTHETAWIN_B1950", "B1950 windowed error ellipse pos. angle (east of north)",
	&outobj2.winposerr_theta1950, H_FLOAT, T_FLOAT, "%5.1f", "deg",
	"pos.posAng;pos.errorEllipse", "deg" );

	AddObjKey( "NITER_WIN", "Number of iterations for WIN centering",
	&outobj2.winpos_niter, H_INT, T_SHORT, "%3d", "",
	"meta.number", "" );

	AddObjKey( "MU_THRESHOLD", "Detection threshold above background",
	&outobj2.threshmu, H_FLOAT, T_FLOAT, "%8.4f", "mag * arcsec**(-2)",
	"instr.sensitivity;phot.mag.sb", "mag.arcsec-2" );
	AddObjKey( "MU_MAX", "Peak surface brightness above background",
	&outobj2.maxmu, H_FLOAT, T_FLOAT, "%8.4f", "mag * arcsec**(-2)",
	"phot.mag.sb;stat.max", "mag.arcsec-2" );
	AddObjKey( "ISOAREA_WORLD", "Isophotal area above Analysis threshold",
	&outobj2.npixw, H_FLOAT, T_FLOAT, "%12.7g", "deg**2",
	"phys.angArea", "deg2" );
	AddObjKey( "ISOAREAF_WORLD", "Isophotal area (filtered) above Detection threshold",
	&outobj2.fdnpixw, H_FLOAT, T_FLOAT, "%12.7g", "deg**2",
	"phys.angArea", "deg2" );
	AddObjKey( "ISO0", "Isophotal area at level 0",
	&outobj.iso[0], H_INT, T_LONG, "%8d", "pixel**2",
	"phys.area;instr.det", "pix2" );
	AddObjKey( "ISO1", "Isophotal area at level 1",
	&outobj.iso[1], H_INT, T_LONG, "%8d", "pixel**2",
	"phys.area;instr.det", "pix2" );
	AddObjKey( "ISO2", "Isophotal area at level 2",
	&outobj.iso[2], H_INT, T_LONG, "%8d", "pixel**2",
	"phys.area;instr.det", "pix2" );
	AddObjKey( "ISO3", "Isophotal area at level 3",
	&outobj.iso[3], H_INT, T_LONG, "%8d", "pixel**2",
	"phys.area;instr.det", "pix2" );
	AddObjKey( "ISO4", "Isophotal area at level 4",
	&outobj.iso[4], H_INT, T_LONG, "%8d", "pixel**2",
	"phys.area;instr.det", "pix2" );
	AddObjKey( "ISO5", "Isophotal area at level 5",
	&outobj.iso[5], H_INT, T_LONG, "%8d", "pixel**2",
	"phys.area;instr.det", "pix2" );
	AddObjKey( "ISO6", "Isophotal area at level 6",
	&outobj.iso[6], H_INT, T_LONG, "%8d", "pixel**2",
	"phys.area;instr.det", "pix2" );
	AddObjKey( "ISO7", "Isophotal area at level 7",
	&outobj.iso[7], H_INT, T_LONG, "%8d", "pixel**2",
	"phys.area;instr.det", "pix2" );

	AddObjKey( "FLAGS", "Extraction flags",
				&outobj.flag, H_INT, T_SHORT, "%3d", "",
				"meta.code.qual", "" );
	AddObjKey( "FLAGS_WEIGHT", "Weighted extraction flags",
				&outobj.wflag, H_INT, T_SHORT, "%1d", "",
				"meta.code.qual", "" );
 	AddObjKey( "FLAGS_WIN", "Flags for WINdowed parameters",
				&outobj2.win_flag, H_INT, T_SHORT, "%3d", "",
				"meta.code.qual", "" );
 	AddObjKey( "IMAFLAGS_ISO", "FLAG-image flags OR'ed over the iso. profile",
				outobj.imaflag, H_INT, T_LONG, "%9u", "",
				"meta.code.qual", "", 1, &prefs.imaflag_size );
	AddObjKey( "NIMAFLAGS_ISO", "Number of flagged pixels entering IMAFLAGS_ISO",
				outobj.imanflag, H_INT, T_LONG, "%9d", "",
				"meta.number", "", 1, &prefs.imanflag_size );

	AddObjKey( "FWHM_IMAGE", "FWHM assuming a gaussian core",
	&outobj.fwhm, H_FLOAT, T_FLOAT, "%8.2f", "pixel",
	"phys.size.diameter;instr.det.psf", "pix" );
	AddObjKey( "FWHM_WORLD", "FWHM assuming a gaussian core",
	&outobj2.fwhmw, H_FLOAT, T_FLOAT, "%12.7g", "deg",
	"phys.angSize;instr.det.psf", "deg" );
	AddObjKey( "ELONGATION", "A_IMAGE/B_IMAGE",
	&outobj2.elong, H_FLOAT, T_FLOAT, "%8.3f", "",
	"src.ellipticity;arith.ratio;instr.det", "" );
	AddObjKey( "ELLIPTICITY", "1 - B_IMAGE/A_IMAGE",
	&outobj2.ellip, H_FLOAT, T_FLOAT, "%8.3f", "",
	"src.ellipticity;instr.det	", "" );
	AddObjKey( "POLAR_IMAGE", "(A_IMAGE^2 - B_IMAGE^2)/(A_IMAGE^2 + B_IMAGE^2)",
	&outobj2.polar, H_FLOAT, T_FLOAT, "%7.5f", "",
	"src.ellipticity;instr.det", "" );
	AddObjKey( "POLAR_WORLD", "(A_WORLD^2 - B_WORLD^2)/(A_WORLD^2 + B_WORLD^2)",
	&outobj2.polarw, H_FLOAT, T_FLOAT, "%7.5f", "",
	"src.ellipticity", "" );
	AddObjKey( "POLARWIN_IMAGE", "(AWIN^2 - BWIN^2)/(AWIN^2 + BWIN^2)",
	&outobj2.win_polar, H_FLOAT, T_FLOAT, "%7.5f", "",
	"src.ellipticity;instr.det", "" );
	AddObjKey( "POLARWIN_WORLD", "(AWIN^2 - BWIN^2)/(AWIN^2 + BWIN^2)",
	&outobj2.win_polarw, H_FLOAT, T_FLOAT, "%7.5f", "",
	"src.ellipticity", "" );
	AddObjKey( "CLASS_STAR", "S/G classifier output",
	&outobj2.sprob, H_FLOAT, T_FLOAT, "%5.2f", "",
	"src.class.starGalaxy", "" );
	AddObjKey( "VIGNET", "Pixel data around detection",
	&outobj2.vignet, H_FLOAT, T_FLOAT, "%12.7g", "count",
	"obs.image", "ct", 2, prefs.vignetsize );
	AddObjKey( "VIGNET_SHIFT", "Pixel data around detection, corrected for shift",
	&outobj2.vigshift, H_FLOAT, T_FLOAT, "%12.7g", "count",
	"obs.image", "ct", 2, prefs.vigshiftsize );
	AddObjKey( "VECTOR_ASSOC", "ASSOCiated parameter vector",
	&outobj2.assoc, H_FLOAT, T_FLOAT, "%12.7g", "",
	"src", "", 1, &prefs.assoc_size );
	AddObjKey( "NUMBER_ASSOC", "Number of ASSOCiated IDs",
	&outobj2.assoc_number, H_INT, T_LONG, "%10d", "",
	"meta.number;src", "" );

	AddObjKey( "THRESHOLDMAX", "Maximum threshold possible for detection",
	&outobj.dthresh, H_FLOAT, T_FLOAT, "%12.7g", "count",
	"phot.flux.sb;stat.max", "ct" );

	AddObjKey( "FLUX_GROWTH", "Cumulated growth-curve",
	&outobj2.flux_growth, H_FLOAT, T_FLOAT, "%12.7g", "count",
	"phot.count", "ct", 1, &prefs.flux_growthsize );
	AddObjKey( "FLUX_GROWTHSTEP", "Step for growth-curves",
	&outobj2.flux_growthstep, H_FLOAT, T_FLOAT, "%10.3f", "pixel",
	"pos.distance", "pix" );
	AddObjKey( "MAG_GROWTH", "Cumulated magnitude growth-curve",
	&outobj2.mag_growth, H_FLOAT, T_FLOAT, "%8.4f", "mag",
	"phot.mag", "mag", 1, &prefs.mag_growthsize );
	AddObjKey( "MAG_GROWTHSTEP", "Step for growth-curves",
	&outobj2.mag_growthstep, H_FLOAT, T_FLOAT, "%10.3f", "pixel",
	"pos.distance", "pix" );
	AddObjKey( "FLUX_RADIUS", "Fraction-of-light radii",
	&outobj2.flux_radius, H_FLOAT, T_FLOAT, "%10.3f", "pixel",
	"phys.size.radius;instr.det", "pix",  1, &prefs.flux_radiussize );

	AddObjKey( "XPSF_IMAGE", "X coordinate from PSF-fitting",
	&outobj2.x_psf, H_FLOAT, T_FLOAT, "%10.3f", "pixel",
	"pos.cartesian.x;stat.fit.param;instr.det", "pix", 1, &prefs.psf_xsize );
	AddObjKey( "YPSF_IMAGE", "Y coordinate from PSF-fitting",
	&outobj2.y_psf, H_FLOAT, T_FLOAT, "%10.3f", "pixel",
	"pos.cartesian.y;stat.fit.param;instr.det", "pix", 1, &prefs.psf_ysize );
	AddObjKey( "XPSF_WORLD", "PSF position along world x axis",
	&outobj2.xw_psf, H_FLOAT, T_DOUBLE, "%15.10e", "deg",
	"pos.eq.ra;stat.fit.param", "deg", 1, &prefs.psf_xwsize );
	AddObjKey( "YPSF_WORLD", "PSF position along world y axis",
	&outobj2.yw_psf, H_FLOAT, T_DOUBLE, "%15.10e", "deg",
	"pos.eq.dec;stat.fit.param", "deg", 1, &prefs.psf_ywsize );

	AddObjKey( "ALPHAPSF_SKY", "Right ascension of the fitted PSF (native)",
	&outobj2.alphas_psf, H_FLOAT, T_DOUBLE, "%11.7f", "deg",
	"pos.eq.ra;stat.fit.param", "deg", 1, &prefs.psf_alphassize );
	AddObjKey( "DELTAPSF_SKY", "Declination of the fitted PSF (native)",
	&outobj2.deltas_psf, H_FLOAT, T_DOUBLE, "%+11.7f", "deg",
	"pos.eq.dec;stat.fit.param", "deg", 1, &prefs.psf_deltassize );

	AddObjKey( "ALPHAPSF_J2000", "Right ascension of the fitted PSF (J2000)",
	&outobj2.alpha2000_psf, H_FLOAT, T_DOUBLE, "%11.7f", "deg",
	"pos.eq.ra;stat.fit.param", "deg", 1, &prefs.psf_alpha2000size );
	AddObjKey( "DELTAPSF_J2000", "Declination of the fitted PSF (J2000)",
	&outobj2.delta2000_psf, H_FLOAT, T_DOUBLE, "%+11.7f", "deg",
	"pos.eq.dec;stat.fit.param", "deg", 1, &prefs.psf_delta2000size );

	AddObjKey( "ALPHAPSF_B1950", "Right ascension of the fitted PSF (B1950)",
	&outobj2.alpha1950_psf, H_FLOAT, T_DOUBLE, "%11.7f", "deg",
	"pos.eq.ra;stat.fit.param", "deg", 1, &prefs.psf_alpha1950size );
	AddObjKey( "DELTAPSF_B1950", "Declination of the fitted PSF (B1950)",
	&outobj2.delta1950_psf, H_FLOAT, T_DOUBLE, "%+11.7f", "deg",
	"pos.eq.dec;stat.fit.param", "deg", 1, &prefs.psf_delta1950size );

	AddObjKey( "FLUX_PSF", "Flux from PSF-fitting",
	&outobj2.flux_psf, H_FLOAT, T_FLOAT, "%12.7g", "count",
	"phot.count;stat.fit.param", "ct", 1, &prefs.psf_fluxsize );
	AddObjKey( "FLUXERR_PSF", "RMS flux error for PSF-fitting",
	&outobj2.fluxerr_psf, H_FLOAT, T_FLOAT, "%12.7g", "count",
	"stat.stdev;phot.count", "ct", 1, &prefs.psf_fluxerrsize );
	AddObjKey( "MAG_PSF", "Magnitude from PSF-fitting",
	&outobj2.mag_psf, H_FLOAT, T_FLOAT, "%8.4f", "mag",
	"phot.mag", "mag", 1, &prefs.psf_magsize );
	AddObjKey( "MAGERR_PSF", "RMS magnitude error from PSF-fitting",
	&outobj2.magerr_psf, H_FLOAT, T_FLOAT, "%8.4f", "mag",
	"stat.stdev;phot.mag", "mag", 1, &prefs.psf_magsize );

	AddObjKey( "NITER_PSF", "Number of iterations for PSF-fitting",
	&outobj2.niter_psf, H_INT, T_SHORT, "%3d", "",
	"meta.number", "" );
	AddObjKey( "CHI2_PSF", "Reduced chi2 from PSF-fitting",
	&outobj2.chi2_psf, H_FLOAT, T_FLOAT, "%9.4g", "",
	"stat.fit.chi2", "" );

	AddObjKey( "ERRX2PSF_IMAGE", "Variance of PSF position along x",
	&outobj2.poserrmx2_psf, H_EXPO, T_DOUBLE, "%15.10e", "pixel**2",
	"stat.variance;pos.errorEllipse;instr.det", "pix2" );
	AddObjKey( "ERRY2PSF_IMAGE", "Variance of PSF position along y",
	&outobj2.poserrmy2_psf, H_EXPO, T_DOUBLE, "%15.10e", "pixel**2",
	"stat.variance;pos.errorEllipse;instr.det", "pix2" );
	AddObjKey( "ERRXYPSF_IMAGE", "Covariance of PSF position between x and y",
	&outobj2.poserrmxy_psf, H_EXPO, T_DOUBLE, "%15.10e", "pixel**2",
	"stat.covariance;pos.errorEllipse;instr.det", "pix2" );
	AddObjKey( "ERRX2PSF_WORLD", "Variance of PSF position along X-WORLD (alpha)",
	&outobj2.poserrmx2w_psf, H_EXPO, T_DOUBLE, "%15.10e", "deg**2",
	"stat.variance;pos.errorEllipse", "deg2" );
	AddObjKey( "ERRY2PSF_WORLD", "Variance of PSF position along Y-WORLD (delta)",
	&outobj2.poserrmy2w_psf, H_EXPO, T_DOUBLE, "%15.10e", "deg**2",
	"stat.variance;pos.errorEllipse", "deg2" );
	AddObjKey( "ERRXYPSF_WORLD", "Covariance of PSF position X-WORLD/Y-WORLD",
	&outobj2.poserrmxyw_psf, H_EXPO, T_DOUBLE, "%15.10e", "deg**2",
	"stat.covariance;pos.errorEllipse", "deg2" );

	AddObjKey( "ERRCXXPSF_IMAGE", "Cxx PSF error ellipse parameter",
	&outobj2.poserrcxx_psf, H_EXPO, T_FLOAT, "%12.7g", "pixel**(-2)",
	"src.impactParam;pos.errorEllipse;instr.det", "pix-2" );
	AddObjKey( "ERRCYYPSF_IMAGE", "Cyy PSF error ellipse parameter",
	&outobj2.poserrcyy_psf, H_EXPO, T_FLOAT, "%12.7g", "pixel**(-2)",
	"src.impactParam;pos.errorEllipse;instr.det", "pix-2" );
	AddObjKey( "ERRCXYPSF_IMAGE", "Cxy PSF error ellipse parameter",
	&outobj2.poserrcxy_psf, H_EXPO, T_FLOAT, "%12.7g", "pixel**(-2)",
	"src.impactParam;pos.errorEllipse;instr.det", "pix-2" );
	AddObjKey( "ERRCXXPSF_WORLD", "Cxx PSF error ellipse parameter (WORLD units)",
	&outobj2.poserrcxxw_psf, H_EXPO, T_FLOAT, "%12.7g", "deg**(-2)",
	"src.impactParam;pos.errorEllipse", "deg-2" );
	AddObjKey( "ERRCYYPSF_WORLD", "Cyy PSF error ellipse parameter (WORLD units)",
	&outobj2.poserrcyyw_psf, H_EXPO, T_FLOAT, "%12.7g", "deg**(-2)",
	"src.impactParam;pos.errorEllipse", "deg-2" );
	AddObjKey( "ERRCXYPSF_WORLD", "Cxy PSF error ellipse parameter (WORLD units)",
	&outobj2.poserrcxyw_psf, H_EXPO, T_FLOAT, "%12.7g", "deg**(-2)",
	"src.impactParam;pos.errorEllipse", "deg-2" );

	AddObjKey( "ERRAPSF_IMAGE", "PSF RMS position error along major axis",
	&outobj2.poserra_psf, H_FLOAT, T_FLOAT, "%8.4f", "pixel",
	"stat.stdev;stat.max;pos.errorEllipse;instr.det", "pix" );
	AddObjKey( "ERRBPSF_IMAGE", "PSF RMS position error along minor axis",
	&outobj2.poserrb_psf, H_FLOAT, T_FLOAT, "%8.4f", "pixel",
	"stat.stdev;stat.min;pos.errorEllipse;instr.det", "pix" );
	AddObjKey( "ERRTHTPSF_IMAGE", "PSF error ellipse position angle (CCW/x)",
	&outobj2.poserrtheta_psf, H_FLOAT, T_FLOAT, "%5.1f", "deg",
	"pos.posAng;pos.errorEllipse;instr.det", "deg" );
	AddObjKey( "ERRAPSF_WORLD", "World PSF RMS position error along major axis",
	&outobj2.poserraw_psf, H_FLOAT, T_FLOAT, "%12.7g", "pixel",
	"stat.stdev;stat.max;pos.errorEllipse", "deg" );
	AddObjKey( "ERRBPSF_WORLD", "World PSF RMS position error along minor axis",
	&outobj2.poserrbw_psf, H_FLOAT, T_FLOAT, "%12.7g", "pixel",
	"stat.stdev;stat.min;pos.errorEllipse", "deg" );
	AddObjKey( "ERRTHTPSF_WORLD", "PSF error ellipse pos. angle (CCW/world-x)",
	&outobj2.poserrthetaw_psf, H_FLOAT, T_FLOAT, "%5.1f", "deg",
	"pos.posAng;pos.errorEllipse", "deg" );
	AddObjKey( "ERRTHTPSF_SKY", "Native PSF error ellipse pos. angle (east of north)",
	&outobj2.poserrthetas_psf, H_FLOAT, T_FLOAT, "%5.1f", "deg",
	"pos.posAng;pos.errorEllipse", "deg" );
	AddObjKey( "ERRTHTPSF_J2000", "J2000 PSF error ellipse pos. angle (east of north)",
	&outobj2.poserrtheta2000_psf, H_FLOAT, T_FLOAT, "%5.1f", "deg",
	"pos.posAng;pos.errorEllipse", "deg" );
	AddObjKey( "ERRTHTPSF_B1950", "B1950 PSF error ellipse pos. angle (east of north)",
	&outobj2.poserrtheta1950_psf, H_FLOAT, T_FLOAT, "%5.1f", "deg",
	"pos.posAng;pos.errorEllipse", "deg" );

	AddObjKey( "X2PC_IMAGE", "PC variance along x",
	&outobj2.mx2_pc, H_EXPO, T_DOUBLE, "%15.10e", "pixel**2",
	"src.impactParam;instr.det", "pix2" );
	AddObjKey( "Y2PC_IMAGE", "PC variance along y",
	&outobj2.my2_pc, H_EXPO, T_DOUBLE, "%15.10e", "pixel**2",
	"src.impactParam;instr.det", "pix2" );
	AddObjKey( "XYPC_IMAGE", "PC covariance between x and y",
	&outobj2.mxy_pc, H_EXPO, T_DOUBLE, "%15.10e", "pixel**2",
	"src.impactParam;instr.det", "pix2" );

	AddObjKey( "APC_IMAGE", "PC profile RMS along major axis",
	&outobj2.a_pc, H_FLOAT, T_FLOAT, "%8.2f", "pixel",
	"phys.size.smajAxis;instr.det", "pix" );
	AddObjKey( "BPC_IMAGE", "PC profile RMS along minor axis",
	&outobj2.b_pc, H_FLOAT, T_FLOAT, "%8.2f", "pixel",
	"phys.size.sminAxis;instr.det", "pix" );
	AddObjKey( "THETAPC_IMAGE", "PC position angle (CCW/x)",
	&outobj2.theta_pc, H_FLOAT, T_FLOAT, "%5.1f", "deg",
	"pos.posAng;instr.det", "deg" );
	AddObjKey( "PC", "Principal components",
	&outobj2.vector_pc, H_FLOAT, T_FLOAT, "%15.10e", "",
	"src.morph.param", "", 1, &prefs.pc_vectorsize );

//	{"RETINOUT", T_FLOAT, &outobj.retinout, "%13g " );

	AddObjKey( "" );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		AddKey
// Class:		CSextractor
// Purpose:		add emlement to key vector
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::AddKey( const char* strName="", DefKeyType nType=P_FLOAT,
						 void* pPtr=NULL, int nImin=0, int nImax=0, 
						 double nDmin=0, double nDmax=0, 
						 const char vectKeyList[32][32]=NULL, 
						 int nNListMin=0, int nNListMax=0, 
						 int* pNListPtr=NULL, int nFlag=0 )
{
	int i=0;

	strcpy( key[m_nKey].name, strName );
	key[m_nKey].type = nType;
	key[m_nKey].ptr = pPtr;
	key[m_nKey].imin = nImin;
	key[m_nKey].imax = nImax;
	key[m_nKey].dmin = nDmin;
	key[m_nKey].dmax = nDmax;

	key[m_nKey].nlistmin = nNListMin;
	key[m_nKey].nlistmax = nNListMax;
	key[m_nKey].nlistptr = pNListPtr;
	key[m_nKey].flag = nFlag;

	if( vectKeyList )
	{
		for( i=0; i<32; i++ )
		{
			if( strcmp( vectKeyList[i], "" ) == 0 ) break;
			strcpy( key[m_nKey].keylist[i], vectKeyList[i] );
		}

	} else
	{
		strcpy( key[m_nKey].keylist[0], "" );
	}

	m_nKey++;

	return;
}


////////////////////////////////////////////////////////////////////
// Method:		Analyse
// Class:		CSextractor
// Purpose:		analyze my image ???
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::InitKeyList( )
{

	{	char vectTmp[32][32] = { "" };
		AddKey( "ANALYSIS_THRESH", P_FLOATLIST, prefs.thresh, 0,0, -BIG, BIG,
			vectTmp, 1, 2, &prefs.nthresh ); 
	}
	{	char vectTmp[32][32] = { "" };
		AddKey( "ASSOC_DATA", P_INTLIST, prefs.assoc_data, 0, 1000000,0.0,0.0,
				vectTmp, 1,MAXLIST, &prefs.nassoc_data );
	}
	{	char vectTmp[32][32] = { "" };
		AddKey( "ASSOC_NAME", P_STRING, prefs.assoc_name );
	}
	{	char vectTmp[32][32] = { "" };
		AddKey( "ASSOC_PARAMS", P_INTLIST, prefs.assoc_param, 1, 1000000,0.0,0.0,
				vectTmp, 2,3, &prefs.nassoc_param );
	}
	{	char vectTmp[32][32] = { "" };
		AddKey( "ASSOC_RADIUS", P_FLOAT, &prefs.assoc_radius, 0,0, 1e-10,1e+10 );
	}
	{	char vectTmp[32][32] = { "FIRST", "NEAREST", "MEAN", "MAG_MEAN", "SUM", "MAG_SUM","MIN", "MAX", "" };
		AddKey( "ASSOC_TYPE", P_KEY, &prefs.assoc_type, 0,0, 0.0,0.0, vectTmp );
	}
	{	char vectTmp[32][32] = { "ALL","MATCHED","-MATCHED","" };
		AddKey( "ASSOCSELEC_TYPE", P_KEY, &prefs.assocselec_type, 0,0, 0.0,0.0,
					vectTmp );
	}
	{	char vectTmp[32][32] = { "" };
		AddKey( "BACK_FILTERSIZE", P_INTLIST, prefs.backfsize, 1,11, 0.0,0.0,
				vectTmp, 1,2, &prefs.nbackfsize );
	}
	{	char vectTmp[32][32] = { "" };
		AddKey( "BACK_FILTTHRESH", P_FLOAT, &prefs.backfthresh, 0,0, 0.0, BIG );
	}
	{	char vectTmp[32][32] = { "" };
		AddKey( "BACKPHOTO_THICK", P_INT, &prefs.pback_size, 1, 256 );
	}
	{	char vectTmp[32][32] = { "GLOBAL","LOCAL","" };
		AddKey( "BACKPHOTO_TYPE", P_KEY, &prefs.pback_type, 0,0, 0.0,0.0,
					vectTmp );
	}
	{	char vectTmp[32][32] = { "" };
		AddKey( "BACK_SIZE", P_INTLIST, prefs.backsize, 1,2000000000, 0.0,0.0,
				vectTmp, 1,2, &prefs.nbacksize );
	}
	{	char vectTmp[32][32] = { "AUTO","MANUAL","" };
		AddKey( "BACK_TYPE", P_KEYLIST, prefs.back_type, 0,0, 0.0,0.0,
				vectTmp, 1, 2, &prefs.nback_type );
	}
	{	char vectTmp[32][32] = { "" };
		AddKey( "BACK_VALUE", P_FLOATLIST, prefs.back_val, 0,0, -BIG,BIG,
				vectTmp, 1, 2, &prefs.nback_val );
	}
	{	char vectTmp[32][32] = { "" };
		AddKey( "CATALOG_NAME", P_STRING, prefs.cat_name );
	}
	{	char vectTmp[32][32] = { "NONE", "ASCII","ASCII_HEAD", "ASCII_SKYCAT", "ASCII_VOTABLE","FITS_LDAC", "FITS_TPX", "FITS_1.0","" };
		AddKey( "CATALOG_TYPE", P_KEY, &prefs.cat_type, 0,0, 0.0,0.0, vectTmp );
	}
	{	char vectTmp[32][32] = { "" };
		AddKey( "CHECKIMAGE_NAME", P_STRINGLIST, prefs.check_name, 0,0,0.0,0.0,
				vectTmp, 0, MAXCHECK, &prefs.ncheck_name );
	}
	{	char vectTmp[32][32] = { "NONE", "IDENTICAL","BACKGROUND", "BACKGROUND_RMS", "MINIBACKGROUND","MINIBACK_RMS", "-BACKGROUND","FILTERED", "OBJECTS", "APERTURES", "SEGMENTATION", "ASSOC","-OBJECTS", "-PSF_PROTOS", "PSF_PROTOS","-PC_CONVPROTOS", "PC_CONVPROTOS", "PC_PROTOS", "" };
		AddKey( "CHECKIMAGE_TYPE", P_KEYLIST, prefs.check_type, 0,0, 0.0,0.0,
				vectTmp, 0, 17, &prefs.ncheck_type );
	}
	{	char vectTmp[32][32] = { "" };
		AddKey( "CLEAN", P_BOOL, &prefs.clean_flag );
	}
	{	char vectTmp[32][32] = { "" };
		AddKey( "CLEAN_PARAM", P_FLOAT, &prefs.clean_param, 0,0, 0.1,10.0 );
	}
	{	char vectTmp[32][32] = { "" };
		AddKey( "DEBLEND_MINCONT", P_FLOAT, &prefs.deblend_mincont, 0,0, 0.0,1.0 );
	}
	{	char vectTmp[32][32] = { "" };
		AddKey( "DEBLEND_NTHRESH", P_INT, &prefs.deblend_nthresh, 1,64 );
	}
	{	char vectTmp[32][32] = { "" };
		AddKey( "DETECT_MINAREA", P_INT, &prefs.ext_minarea, 1,1000000 );
	}
	{	char vectTmp[32][32] = { "" };
		AddKey( "DETECT_THRESH", P_FLOATLIST, prefs.dthresh, 0,0, -BIG, BIG,
				vectTmp, 1, 2, &prefs.ndthresh );
	}
	{	char vectTmp[32][32] = { "CCD","PHOTO","" };
		AddKey( "DETECT_TYPE", P_KEY, &prefs.detect_type, 0,0, 0.0,0.0, vectTmp );
	}
	{	char vectTmp[32][32] = { "" };
		AddKey( "FILTER", P_BOOL, &prefs.filter_flag );
	}

	AddKey( "FILTER_NAME", P_STRING, prefs.filter_name );
	AddKey( "FILTER_THRESH", P_FLOATLIST, prefs.filter_thresh, 0,0,-BIG,BIG,
				NULL, 0, 2, &prefs.nfilter_thresh );
	AddKey( "FITS_UNSIGNED", P_BOOL, &prefs.fitsunsigned_flag );
	AddKey( "FLAG_IMAGE", P_STRINGLIST, prefs.fimage_name, 0,0,0.0,0.0,
				NULL, 0, MAXFLAG, &prefs.nfimage_name );

	{	char vectTmp[32][32] = { "OR","AND","MIN", "MAX", "MOST","" };
		AddKey( "FLAG_TYPE",  P_KEYLIST, prefs.flag_type, 0,0, 0.0,0.0,
				vectTmp, 0, MAXFLAG, &idummy );
	}

	AddKey( "GAIN", P_FLOAT, &prefs.gain, 0,0, 0.0, 1e+30 );
	AddKey( "INTERP_MAXXLAG", P_INTLIST, prefs.interp_xtimeout, 1,1000000, 0.0,0.0,
					NULL, 1, 2, &prefs.ninterp_xtimeout );
	AddKey( "INTERP_MAXYLAG", P_INTLIST, prefs.interp_ytimeout, 1,1000000, 0.0,0.0,
				NULL, 1, 2, &prefs.ninterp_ytimeout );

	{	char vectTmp[32][32] = { "NONE","VAR_ONLY","ALL","" };
		AddKey( "INTERP_TYPE", P_KEYLIST, prefs.interp_type, 0,0, 0.0,0.0,
				vectTmp, 1, 2, &prefs.ninterp_type );
	}

	AddKey( "MAG_GAMMA", P_FLOAT, &prefs.mag_gamma, 0,0, 1e-10,1e+30 );
	AddKey( "MAG_ZEROPOINT", P_FLOAT, &prefs.mag_zeropoint, 0,0, -100.0, 100.0 );

	{	char vectTmp[32][32] = { "NONE","BLANK","CORRECT","" };
		AddKey( "MASK_TYPE", P_KEY, &prefs.mask_type, 0,0, 0.0,0.0, vectTmp );
	}

	AddKey( "MEMORY_BUFSIZE", P_INT, &prefs.mem_bufsize, 8, 65534 );
	AddKey( "MEMORY_OBJSTACK", P_INT, &prefs.clean_stacksize, 16, 65536 );
	AddKey( "MEMORY_PIXSTACK", P_INT, &prefs.mem_pixstack, 1000, 10000000 );
	AddKey( "NTHREADS", P_INT, &prefs.nthreads, 0, THREADS_PREFMAX );
	AddKey( "PARAMETERS_NAME", P_STRING, prefs.param_name );
	AddKey( "PHOT_APERTURES", P_FLOATLIST, prefs.apert, 0,0, 0.0,2*MAXPICSIZE,
				NULL, 1, MAXNAPER, &prefs.naper );
	AddKey( "PHOT_AUTOPARAMS", P_FLOATLIST, prefs.autoparam, 0,0, 0.0,10.0,
				NULL, 2,2, &prefs.nautoparam );
	AddKey( "PHOT_AUTOAPERS", P_FLOATLIST, prefs.autoaper, 0,0, 0.0,1e6,
				NULL, 2,2, &prefs.nautoaper );
	AddKey( "PHOT_FLUXFRAC", P_FLOATLIST, prefs.flux_frac, 0,0, 1e-6, 1.0,
				NULL, 1, MAXNAPER, &prefs.nflux_frac );
	AddKey( "PHOT_PETROPARAMS", P_FLOATLIST, prefs.petroparam, 0,0, 0.0,10.0,
				NULL, 2,2, &prefs.npetroparam );
	AddKey( "PIXEL_SCALE", P_FLOAT, &prefs.pixel_scale, 0,0, 0.0, 1e+10 );
	AddKey( "PSF_NAME", P_STRINGLIST, prefs.psf_name, 0,0, 0.0,0.0,
				NULL, 1, 2, &prefs.npsf_name );	//???
	AddKey( "PSF_NMAX", P_INT, &prefs.psf_npsfmax, 1, PSF_NPSFMAX );

	{	char vectTmp[32][32] = { "SPLIT","VECTOR","" };
		AddKey( "PSFDISPLAY_TYPE", P_KEY, &prefs.psfdisplay_type, 0,0, 0.0,0.0, vectTmp );
	}

	AddKey( "SATUR_LEVEL", P_FLOAT, &prefs.satur_level, 0,0, -1e+30, 1e+30 );
	AddKey( "SEEING_FWHM", P_FLOAT, &prefs.seeing_fwhm, 0,0, 1e-10, 1e+10 );
	AddKey( "SOM_NAME", P_STRING, prefs.som_name );
	AddKey( "STARNNW_NAME", P_STRING, prefs.nnw_name );

	{	char vectTmp[32][32] = { "RELATIVE","ABSOLUTE" };
		AddKey( "THRESH_TYPE", P_KEYLIST, prefs.thresh_type, 0,0, 0.0,0.0,
					vectTmp, 1, 2, &prefs.nthresh_type );
	}
	{	char vectTmp[32][32] = { "QUIET","NORMAL", "EXTRA_WARNINGS", "FULL","" };
		AddKey( "VERBOSE_TYPE", P_KEY, &prefs.verbose_type, 0,0, 0.0,0.0, vectTmp );
	}

	AddKey( "WEIGHT_GAIN", P_BOOL, &prefs.weightgain_flag );
	AddKey( "WEIGHT_IMAGE", P_STRINGLIST, prefs.wimage_name, 0,0,0.0,0.0,
				NULL, 0, MAXIMAGE, &prefs.nwimage_name );
	AddKey( "WEIGHT_THRESH", P_FLOATLIST, prefs.weight_thresh, 0,0, 0.0, BIG,
				NULL, 0, 2, &prefs.nweight_thresh );

	{	char vectTmp[32][32] = { "NONE","BACKGROUND", "MAP_RMS", "MAP_VAR","MAP_WEIGHT", "" };
		AddKey( "WEIGHT_TYPE", P_KEYLIST, prefs.weight_type, 0,0, 0.0,0.0,
				vectTmp, 0, MAXIMAGE, &prefs.nweight_type );
	}

	AddKey( "WRITE_XML", P_BOOL, &prefs.xml_flag );
	AddKey( "XML_NAME", P_STRING, prefs.xml_name );
	AddKey( "XSL_URL", P_STRING, prefs.xsl_name );
	AddKey( "" );
 

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		InitCatParams
// Class:		CSextractor
// Purpose:		init cat params - hardcode
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::InitCatParams( )
{
	m_nCatParams = 0;

	strcpy( m_vectCatParams[m_nCatParams++], "NUMBER" );
	strcpy( m_vectCatParams[m_nCatParams++], "FLUX_ISO" );
	strcpy( m_vectCatParams[m_nCatParams++], "FLUXERR_ISO" );
	strcpy( m_vectCatParams[m_nCatParams++], "MAG_ISO" );
	strcpy( m_vectCatParams[m_nCatParams++], "MAGERR_ISO" );
	strcpy( m_vectCatParams[m_nCatParams++], "FLUX_ISOCOR" );
	strcpy( m_vectCatParams[m_nCatParams++], "FLUXERR_ISOCOR" );
	strcpy( m_vectCatParams[m_nCatParams++], "MAG_ISOCOR" );
	strcpy( m_vectCatParams[m_nCatParams++], "MAGERR_ISOCOR" );
	strcpy( m_vectCatParams[m_nCatParams++], "FLUX_APER(1)" );
	strcpy( m_vectCatParams[m_nCatParams++], "FLUXERR_APER(1)" );
	strcpy( m_vectCatParams[m_nCatParams++], "MAG_APER(1)" );
	strcpy( m_vectCatParams[m_nCatParams++], "MAGERR_APER(1)" );
	strcpy( m_vectCatParams[m_nCatParams++], "FLUX_AUTO" );
	strcpy( m_vectCatParams[m_nCatParams++], "FLUXERR_AUTO" );
	strcpy( m_vectCatParams[m_nCatParams++], "MAG_AUTO" );
	strcpy( m_vectCatParams[m_nCatParams++], "MAGERR_AUTO" );
	strcpy( m_vectCatParams[m_nCatParams++], "FLUX_BEST" );
	strcpy( m_vectCatParams[m_nCatParams++], "FLUXERR_BEST" );
	strcpy( m_vectCatParams[m_nCatParams++], "MAG_BEST" );
	strcpy( m_vectCatParams[m_nCatParams++], "MAGERR_BEST" );

	//strcpy( m_vectCatParams[m_nCatParams++], "KRON_RADIUS" );
	//strcpy( m_vectCatParams[m_nCatParams++], "BACKGROUND" );
	//strcpy( m_vectCatParams[m_nCatParams++], "THRESHOLD" );
	//strcpy( m_vectCatParams[m_nCatParams++], "MU_THRESHOLD" );
	//strcpy( m_vectCatParams[m_nCatParams++], "FLUX_MAX" );
	//strcpy( m_vectCatParams[m_nCatParams++], "MU_MAX" );
	//strcpy( m_vectCatParams[m_nCatParams++], "ISOAREA_IMAGE" );
	//strcpy( m_vectCatParams[m_nCatParams++], "ISOAREA_WORLD" );

	strcpy( m_vectCatParams[m_nCatParams++], "XMIN_IMAGE" );
	strcpy( m_vectCatParams[m_nCatParams++], "YMIN_IMAGE" );
	strcpy( m_vectCatParams[m_nCatParams++], "XMAX_IMAGE" );
	strcpy( m_vectCatParams[m_nCatParams++], "YMAX_IMAGE" );
	strcpy( m_vectCatParams[m_nCatParams++], "X_IMAGE" );
	strcpy( m_vectCatParams[m_nCatParams++], "Y_IMAGE" );
	strcpy( m_vectCatParams[m_nCatParams++], "XPEAK_IMAGE"  );
	strcpy( m_vectCatParams[m_nCatParams++], "YPEAK_IMAGE"  );

	//strcpy( m_vectCatParams[m_nCatParams++], "X_WORLD" );
	//strcpy( m_vectCatParams[m_nCatParams++], "Y_WORLD" );
	//strcpy( m_vectCatParams[m_nCatParams++], "ALPHA_SKY" );
	//strcpy( m_vectCatParams[m_nCatParams++], "DELTA_SKY" );
	//strcpy( m_vectCatParams[m_nCatParams++], "ALPHA_J2000" );
	//strcpy( m_vectCatParams[m_nCatParams++], "DELTA_J2000" );
	//strcpy( m_vectCatParams[m_nCatParams++], "ALPHA_B1950" );
	//strcpy( m_vectCatParams[m_nCatParams++], "DELTA_B1950" );
	//strcpy( m_vectCatParams[m_nCatParams++], "X2_IMAGE" );
	//strcpy( m_vectCatParams[m_nCatParams++], "Y2_IMAGE" );
	//strcpy( m_vectCatParams[m_nCatParams++], "XY_IMAGE" );
	//strcpy( m_vectCatParams[m_nCatParams++], "X2_WORLD" );
	//strcpy( m_vectCatParams[m_nCatParams++], "Y2_WORLD" );
	//strcpy( m_vectCatParams[m_nCatParams++], "XY_WORLD" );
	//strcpy( m_vectCatParams[m_nCatParams++], "CXX_IMAGE" );
	//strcpy( m_vectCatParams[m_nCatParams++], "CYY_IMAGE" );
	//strcpy( m_vectCatParams[m_nCatParams++], "CXY_IMAGE" );
	//strcpy( m_vectCatParams[m_nCatParams++], "CXX_WORLD" );
	//strcpy( m_vectCatParams[m_nCatParams++], "CYY_WORLD" );
	//strcpy( m_vectCatParams[m_nCatParams++], "CXY_WORLD" );
	//strcpy( m_vectCatParams[m_nCatParams++], "A_IMAGE" );
	//strcpy( m_vectCatParams[m_nCatParams++], "B_IMAGE" );
	//strcpy( m_vectCatParams[m_nCatParams++], "A_WORLD" );
	//strcpy( m_vectCatParams[m_nCatParams++], "B_WORLD" );
	//strcpy( m_vectCatParams[m_nCatParams++], "THETA_IMAGE" );
	//strcpy( m_vectCatParams[m_nCatParams++], "THETA_WORLD" );
	//strcpy( m_vectCatParams[m_nCatParams++], "THETA_SKY" );
	//strcpy( m_vectCatParams[m_nCatParams++], "THETA_J2000" );
	//strcpy( m_vectCatParams[m_nCatParams++], "THETA_B1950" );
	strcpy( m_vectCatParams[m_nCatParams++], "ELONGATION" );
	strcpy( m_vectCatParams[m_nCatParams++], "ELLIPTICITY" );
	//strcpy( m_vectCatParams[m_nCatParams++], "ERRX2_IMAGE" );
	//strcpy( m_vectCatParams[m_nCatParams++], "ERRY2_IMAGE" );
	//strcpy( m_vectCatParams[m_nCatParams++], "ERRXY_IMAGE" );
	//strcpy( m_vectCatParams[m_nCatParams++], "ERRX2_WORLD" );
	//strcpy( m_vectCatParams[m_nCatParams++], "ERRY2_WORLD" );
	//strcpy( m_vectCatParams[m_nCatParams++], "ERRXY_WORLD" );
	//strcpy( m_vectCatParams[m_nCatParams++], "ERRCXX_IMAGE" );
	//strcpy( m_vectCatParams[m_nCatParams++], "ERRCYY_IMAGE" );
	//strcpy( m_vectCatParams[m_nCatParams++], "ERRCXY_IMAGE" );
	//strcpy( m_vectCatParams[m_nCatParams++], "ERRCXX_WORLD" );
	//strcpy( m_vectCatParams[m_nCatParams++], "ERRCYY_WORLD" );
	//strcpy( m_vectCatParams[m_nCatParams++], "ERRCXY_WORLD" );
	//strcpy( m_vectCatParams[m_nCatParams++], "ERRA_IMAGE" );
	//strcpy( m_vectCatParams[m_nCatParams++], "ERRB_IMAGE" );
	//strcpy( m_vectCatParams[m_nCatParams++], "ERRA_WORLD" );
	//strcpy( m_vectCatParams[m_nCatParams++], "ERRB_WORLD" );

	//strcpy( m_vectCatParams[m_nCatParams++], "ERRTHETA_IMAGE" );
	//strcpy( m_vectCatParams[m_nCatParams++], "ERRTHETA_WORLD" );
	//strcpy( m_vectCatParams[m_nCatParams++], "ERRTHETA_SKY" );
	//strcpy( m_vectCatParams[m_nCatParams++], "ERRTHETA_J2000" );
	//strcpy( m_vectCatParams[m_nCatParams++], "ERRTHETA_B1950" );

	strcpy( m_vectCatParams[m_nCatParams++], "FWHM_IMAGE" );
	//strcpy( m_vectCatParams[m_nCatParams++], "FWHM_WORLD" );

	//strcpy( m_vectCatParams[m_nCatParams++], "ISO0" );
	//strcpy( m_vectCatParams[m_nCatParams++], "ISO1" );
	//strcpy( m_vectCatParams[m_nCatParams++], "ISO2" );
	//strcpy( m_vectCatParams[m_nCatParams++], "ISO3" );
	//strcpy( m_vectCatParams[m_nCatParams++], "ISO4" );
	//strcpy( m_vectCatParams[m_nCatParams++], "ISO5" );
	//strcpy( m_vectCatParams[m_nCatParams++], "ISO6" );
	//strcpy( m_vectCatParams[m_nCatParams++], "ISO7" );

	strcpy( m_vectCatParams[m_nCatParams++], "FLAGS" );
	//strcpy( m_vectCatParams[m_nCatParams++], "IMAFLAGS_ISO(1)" );
	//strcpy( m_vectCatParams[m_nCatParams++], "NIMAFLAGS_ISO(1)" );

	strcpy( m_vectCatParams[m_nCatParams++], "CLASS_STAR" );
	strcpy( m_vectCatParams[m_nCatParams++], "VIGNET(5,5)" );


	return;
}
