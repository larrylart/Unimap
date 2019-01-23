
#ifndef _SEXTRACTOR_H
#define _SEXTRACTOR_H

#include <stdio.h>
#include <sys/timeb.h>


#if _FILE_OFFSET_BITS
#define OFF_T	off_t
#else
#define OFF_T	KINGLONG
#endif


// local headers
extern "C"
{
	#include "fits/fitscat.h"	

	#include "sextractor_define.h"
	#include "sextractor_globals.h"
	#include "sextractor_types.h"
	// wcs
	#include "../lib/wcstools-3.7.7/libwcs/wcs.h"
	#include "wcs/tnx.h"

}

#include "sextractor_bpro.h"
#include "sextractor_assoc.h"
#include "sextractor_astrom.h"
#include "sextractor_back.h"
#include "sextractor_check.h"
#include "sextractor_clean.h"
#include "sextractor_photom.h"
#include "sextractor_extract.h"
#include "sextractor_field.h"
#include "sextractor_filter.h"
#include "sextractor_flag.h"
#include "sextractor_growth.h"
#include "sextractor_image.h"
#include "sextractor_interpolate.h"
#include "sextractor_key.h"
#include "sextractor_neurro.h"
#include "sextractor_plist.h"
#include "sextractor_poly.h"
#include "sextractor_prefs.h"
#include "sextractor_preflist.h"
#include "sextractor_psf.h"
#include "sextractor_retina.h"
#include "sextractor_som.h"
#include "sextractor_weight.h"
#include "sextractor_winpos.h"
#include "sextractor_xml.h"

// external classes 
//class CLogger;
class CAstroImage;
class CImgView;
class CConfigDetect;
class CConfigMain;
class CUnimapWorker;

// global methods used for sorting in assoc
int CompAssoc( const void *i1, const void *i2 );

////////////////////////////////
// class: CSextractor
class CSextractor
{
// methods
public:
	CSextractor( );
	~CSextractor( );

	void Close( );
	void Init( );
	void InitData( );
	void InitGlob( );
	// new method for config
	void GetConfig( );

	/////////////////////////////
	// larry:: to init static
	void InitPrefsStruct( prefstruct& refPrefs );
	void InitCatStruct( sexcatstruct&	refCat );
	void InitObjStruct( objstruct&	refObj );
	void InitObjTwoStruct( obj2struct&	refObj );
	void AddHeadKeyOne( const char* strName, const char* strComment,
							void* pPtr, h_type nHtype,
							t_type nTtype, const char* strPrintf,
							const char* strUnit, const char* strVoucd, 
							const char* strVounit, int nNaxis,
							int* pNaxisn, int nNobj, 
							int nNbytes, long nPos );
	void InitHeadKeyOneList( );
	void AddHeadKey( const char* strName, const char* strComment,
							void* pPtr, h_type nHtype,
							t_type nTtype, const char* strPrintf,
							const char* strUnit, const char* strVoucd, 
							const char* strVounit, int nNaxis,
							int* pNaxisn, int nNobj, 
							int nNbytes, long nPos );
	void InitHeadKeyList( );
	void AddObjKey( const char* strName, const char* strComment,
							void* pPtr, h_type nHtype,
							t_type nTtype, const char* strPrintf,
							const char* strUnit, const char* strVoucd, 
							const char* strVounit, int nNaxis,
							int* pNaxisn, int nNobj, 
							int nNbytes, long nPos );
	void InitObjKeyList( );
	void AddKey( const char* strName, DefKeyType nType,
						 void* pPtr, int nImin, int nImax, 
						 double nDmin, double nDmax, 
						 const char vectKeyList[32][32], 
						 int nNListMin, int nNListMax, 
						 int* pNListPtr, int nFlag );
	void InitKeyList( );
	// initialize cat params
	void InitCatParams( );


	////////////////////////////////////////////////////////////////
	// Methods for MAKEIT section
	void Make( CAstroImage* pImg );
	void WriteError( char *msg1, char *msg2 );
	// this is from MISC
	float Hmedian( float *ra, int n );

	void ReadAstroImageParam( CAstroImage* pImg, picstruct	*field );
	

	////////////////////////////////////////////////////////////////
	// Methods for PHOTOM section
	void ComputeAperFlux(picstruct *field, picstruct *wfield,
											objstruct *obj, int i);
	void ComputePetroFlux( picstruct *field, picstruct *dfield, 
							picstruct *wfield, picstruct *dwfield, 
													objstruct *obj);
	void ComputeAutoFlux(picstruct *field, picstruct *dfield, 
								  picstruct *wfield, picstruct *dwfield, 
									objstruct *obj);
	void ComputeIsocorFlux( picstruct *field, objstruct *obj );
	void ComputeMags( picstruct *field, objstruct *obj );

	////////////////////////////////////////////////////////////////
	// Methods for  ANALYS section
	void Analyse(picstruct *field, picstruct *dfield, int objnb,
											objliststruct *objlist);
	void ExamineIso(picstruct *field, picstruct *dfield, 
								objstruct *obj, pliststruct *pixel);
	void AnalyseEndObject( picstruct *field, picstruct *dfield, 
							picstruct *wfield, picstruct *dwfield, 
							int n, objliststruct *objlist );

	////////////////////////////////////////////////////////////////
	// Methods for PLIST section
	int CreateBlank( objliststruct *objlist, int no );
	int CreateSubMap( objliststruct *objlist, int no );
	void InitPlist( void );

	////////////////////////////////////////////////////////////////
	// Methods for EXTRACTOR section
	void LutzAlloc( int width, int height );
	void LutzFree( );
	int Lutz( objliststruct *objlistroot, int nroot, objstruct *objparent, 
											objliststruct *objlist );
	void LutzSort( infostruct *info, objliststruct *objlist );

	////////////////////////////////////////////////////////////////
	// Methods for SCAN section
	void ScanImage( picstruct *field, picstruct *dfield, 
					picstruct **pffield, int nffield, 
					picstruct *wfield, picstruct *dwfield );
	void Update( infostruct *infoptr1, infostruct *infoptr2, pliststruct *pixel );
	void Sortit( picstruct *field, picstruct *dfield, picstruct *wfield,
							picstruct *dwfield, infostruct *info, objliststruct *objlist,
							PIXTYPE *cdwscan, PIXTYPE *wscan );
	void PreAnalyse( int no, objliststruct *objlist, int analyse_type );

	////////////////////////////////////////////////////////////////
	// Methods for FLAG section
	void GetFlags( objstruct *obj, pliststruct *pixel );
	void MergeFlags( objstruct *objmaster, objstruct *objslave );

	////////////////////////////////////////////////////////////////
	// Methods for MANOBJLIST section
	int Belong( int corenb, objliststruct *coreobjlist,
							int shellnb, objliststruct *shellobjlist );
	int AddObj( int objnb, objliststruct *objl1, objliststruct *objl2 );

	////////////////////////////////////////////////////////////////
	// Methods for REFINE section
	int Parcelout( objliststruct *objlistin, objliststruct *objlistout );
	void AllocParcelout( void );
	void FreeParcelout( void );
	int Gatherup( objliststruct *objlistin, objliststruct *objlistout );

	////////////////////////////////////////////////////////////////
	// Methods for ASSOC section
	void SortAssoc( picstruct *field, assocstruct *assoc );
	assocstruct* LoadAssoc( char *filename );
	void InitAssoc( picstruct *field );
	void EndAssoc( picstruct *field );
	int DoAssoc( picstruct *field, float x, float y );

	////////////////////////////////////////////////////////////////
	// Methods for ASTROM section
	void InitAstrom( picstruct *field );
	void ComputeAstrom( picstruct *field, objstruct *obj );
	double* ComputeWcs( picstruct *field, double mx, double my );
	void AstromShapeparam( picstruct *field, objstruct *obj );
	void AstromWinShapeparam( picstruct *field, objstruct *obj );
	void AstromErrParam( picstruct *field, objstruct *obj );
	void AstromWinErrParam( picstruct *field, objstruct *obj );
	void CopyAstrom( picstruct *infield, picstruct *outfield );
	void EndAstrom( picstruct *field );
	void Precess( double yearin, double alphain, double deltain,
				double yearout, double *alphaout, double *deltaout );
	void J2B( double yearobs, double alphain, double deltain,
								double *alphaout, double *deltaout);

	////////////////////////////////////////////////////////////////
	// Methods for BACK (ground) section
	void MakeBack( picstruct *field, picstruct *wfield );
	void BackStat( backstruct *backmesh, backstruct *wbackmesh,
					PIXTYPE *buf, PIXTYPE *wbuf, size_t bufsize,
					int n, int w, int bw, PIXTYPE wthresh );
	void BackHisto( backstruct *backmesh, backstruct *wbackmesh,
					PIXTYPE *buf, PIXTYPE *wbuf, size_t bufsize,
					int n, int w, int bw, PIXTYPE wthresh );
	float BackGuess( backstruct *bkg, float *mean, float *sigma );
	void FilterBack( picstruct *field );
	float LocalBack( picstruct *field, objstruct *obj );
	PIXTYPE Back( picstruct *field, int x, int y );
	float* MakeBackSpline( picstruct *field, float *map );
	void SubBackLine( picstruct *field, int y, PIXTYPE *line );
	void BackRmsLine( picstruct *field, int y, PIXTYPE *line );
	void CopyBack( picstruct *infield, picstruct *outfield );
	void EndBack( picstruct *field );

	////////////////////////////////////////////////////////////////
	// Methods for the FIELD section
	picstruct* NewField( CAstroImage* pImg, int flags );
	picstruct* InheritField(picstruct *infield, int flags );
	void EndField( picstruct *field );

	////////////////////////////////////////////////////////////////
	// Methods for the BPRO section
	void PlayBpann( bpannstruct *bpann, NFLOAT *invec, NFLOAT *outvec );
	bpannstruct* LoadTabBpann( tabstruct *tab, char *filename );
	void FreeBpann( bpannstruct *bpann );

	////////////////////////////////////////////////////////////////
	// Methods for the CATOUT section
	void ReadCatParams( char *filename );
	void ReadCatParamsHardcode( );
	void UpdateParamFlags( );
	void InitCat( void );
	void WriteVoFields( FILE *file );
	void ReInitCat( picstruct *field );
	void WriteCat( int n, objliststruct *objlist );
	void EndCat( char *error );
	void ReEndCat( );

	////////////////////////////////////////////////////////////////
	// Methods for the CHECK section
	void AddCheck( checkstruct *check, float *psf,
					int w,int h, int ix,int iy, float amplitude );
	void BlankCheck( checkstruct *check, PIXTYPE *mask, int w,
						int h, int xmin,int ymin, PIXTYPE val );
	checkstruct* InitCheck( char *filename, checkenum check_type, int next );
	void ReInitCheck( picstruct *field, checkstruct *check );
	void WriteCheck( checkstruct *check, PIXTYPE *data, int w );
	void ReEndCheck( picstruct *field, checkstruct *check );
	void EndCheck( checkstruct *check );

	////////////////////////////////////////////////////////////////
	// Methods for the CLEAN section
	void InitClean( void );
	void EndClean( void );
	int Clean( picstruct *field, picstruct *dfield, int objnb,
									objliststruct *objlistin );
	void AddCleanObj( objstruct *objin );
	void MergeObject( objstruct *objslave,objstruct *objmaster );
	void SubCleanObj( int objnb );

	////////////////////////////////////////////////////////////////
	// Methods for the FILTER section
	void Convolve( picstruct *field, PIXTYPE *mscan );
	int GetConv( char *filename );
	int GetConvConf( int nFilterId );
	void GetFilter( int nFilterId, char *filename );
	int GetNeurFilter( char *filename );
	void EndFilter( );
	void Filter( picstruct *field, PIXTYPE *mscan );
	void NeurFilter( picstruct *field, PIXTYPE *mscan );
	void ConvolveImage( picstruct *field, double *vig1,
						double *vig2, int width, int height );

	////////////////////////////////////////////////////////////////
	// Methods for the GRAPH section
	//void SexMove( double x, double y );	
	//void SexDraw( PIXTYPE *bmp, int w, int h, double sexx2, 
	//						double sexy2, PIXTYPE val );
	//void SexCircle( PIXTYPE *bmp, int w, int h, double x, 
	//					double y, double r, PIXTYPE val );
	//void SexEllips( PIXTYPE *bmp, int w, int h, double x, 
	//					double y, double a, double b, 
	//					double theta, PIXTYPE val, int dotflag );

	////////////////////////////////////////////////////////////////
	// Methods for the GROWTH section
	void InitGrowth( );
	void EndGrowth( );
	void MakeAverGrowth( picstruct *field, picstruct *wfield, objstruct *obj );

	////////////////////////////////////////////////////////////////
	// Methods for the IMAGE section
	int CopyImage( picstruct *field, PIXTYPE *dest, 
						int w,int h, int ix,int iy );
	void AddImage( picstruct *field, float *psf,
					int w,int h, int ix,int iy, float amplitude );
	int CopyImageCenter( picstruct *field, PIXTYPE *dest, 
									int w,int h, float x,float y );
	void AddImageCenter( picstruct *field, float *psf, 
									int w,int h, float x,float y, 
												float amplitude );
	void BlankImage( picstruct *field, PIXTYPE *mask, int w,
							int h, int xmin,int ymin, PIXTYPE val );
	void PasteImage( picstruct *field, PIXTYPE *mask, int w,
								int h, int xmin,int ymin );
	int VignetResample( double *pix1, int w1, int h1,
								double *pix2, int w2, int h2,
								double dx, double dy, double step2 );

	////////////////////////////////////////////////////////////////
	// Methods for the INTERPOLATE section
	void InitInterpolate( picstruct *field, int xtimeout, int ytimeout );
	void Interpolate( picstruct *field, picstruct *wfield,
								PIXTYPE *data, PIXTYPE *wdata );
	void EndInterpolate( picstruct *field );

	////////////////////////////////////////////////////////////////
	// Methods for the NEURRO section
	void NeurInit( );
	void NeurClose( );
	void NeurResp( double *input, double *output );
	double FSigmoid( double x );
//	void GetNnw( );

	////////////////////////////////////////////////////////////////
	// Methods for the PC section
	void PcEnd( pcstruct *pc );
	pcstruct* PcLoad( catstruct *cat );
	void PcFit( psfstruct *psf, double *data, double *weight,
						int width, int height,int ix, int iy,
						double dx, double dy, int npc, float backrms );

	////////////////////////////////////////////////////////////////
	// Methods for the PSF section
	void PsfInit( psfstruct *psf );
	void PsfEnd( psfstruct *psf, psfitstruct *psfit );
	psfstruct* PsfLoad( char *filename );
	void PsfReadContext( psfstruct *psf, picstruct *field );
	void PsfFit( psfstruct *psf, picstruct *field, 
						picstruct *wfield, objstruct *obj );
	void DoublePsfFit( psfstruct *ppsf, picstruct *pfield, 
						picstruct *pwfield, objstruct *obj, 
						psfstruct *psf, picstruct *field, picstruct *wfield );
	void PsfBuild( psfstruct *psf );
	double* ComputeGradient( double *weight,int width, int height,
                         double *masks,double *maskx,double *masky
                        ,double *m );
	double* ComputeGradientPhot( double *pweight,int width, 
							int height, double *pmasks,double *pm );
	void ComputePos( int *pnpsf, int *pconvflag, int *pnpsfflag,
						double radmin2, double radmax2, double r2,
						double *sol, double *flux, double *deltax, 
						double *deltay, double *pdx, double *pdy );
	void ComputePosPhot( int *pnpsf,double *sol,double *flux );
	void ComputePosErr( int j,double *var,double *sol,
								obj2struct *obj2arg, double *x2,
								double *y2, double *xy );
	void SvdFit( double *a, double *b, int m, int n, 
						double *sol, double *vmat, double *wmat );
	void SvdVar( double *v, double *w, int n, double *cov );

	////////////////////////////////////////////////////////////////
	// Methods for the POLY section
	polystruct* PolyInit( int *group, int ndim, int *degree, int ngroup );
	void PolyEnd( polystruct *poly );
	double PolyFunc( polystruct *poly, double *pos );
	void PolyFit( polystruct *poly, double *x, double *y, 
							double *w, int ndata, double *extbasis );
	void CholSolve( double *a, double *b, int n );

	////////////////////////////////////////////////////////////////
	// Methods for the READIMAGE section
	void* LoadImgStrip( picstruct *field, picstruct *wfield );
	void* LoadStrip( picstruct *field, picstruct *wfield );
	void CopyData( picstruct *field, int offset, int size );
	void ReadAstroImageData( picstruct *field, PIXTYPE *ptr, int size );
	void ReadData( picstruct *field, PIXTYPE *ptr, int size );
	void ReadIData( picstruct *field, FLAGTYPE *ptr, int size );
	void ReadImageHead( picstruct *field );
	char* ReadFitsHead( FILE *file, char *filename, int *nblock );

	////////////////////////////////////////////////////////////////
	// Methods for the RETINA section
	float ReadRetina( picstruct *field, retistruct *retina, float x, float y );
	retistruct* GetRetina( char *filename );
	void EndRetina( retistruct *retina );

	////////////////////////////////////////////////////////////////
	// Methods for the SOM section
	void SomPhot( somstruct *som, float back, float backnoise, 
							float gain, float dx0, float dy0, 
							float *vector, float clip );
	int SomMkWeight( somstruct *som, float back, float backnoise, float gain );
	float SomErr( somstruct *som, float dist, int flags );
	float SomLinMin( somstruct *som );
	void SomConjgrad( somstruct *som, float ftol );
	void SomEnd( somstruct *som );
	somstruct* SomLoad( char *filename );

	////////////////////////////////////////////////////////////////
	// Methods for the PREFS section
	void DumpPrefs( int state );
	void ReadPrefs( char *filename, char **argkey, 
							char **argval, int narg );
	void ClosePrefs( );
	int FindKeys( char *str, char keyw[][32], int mode );
	int FindKeysOrig( char *str, char** keyw, int mode );
	int CiStrCmp( char *cs, char *ct, int mode );
	void UsePrefs( );

	////////////////////////////////////////////////////////////////
	// Methods for the WEIGHT section
	picstruct* NewWeight( char *filename, picstruct *reffield,
										weightenum wtype, int nok );
	void WeightToVar( picstruct *wfield, PIXTYPE *data, int npix );

	////////////////////////////////////////////////////////////////
	// Methods for the WINPOS section
	void ComputeWinpos( picstruct *field, picstruct *wfield, 
											objstruct *obj );
	////////////////////////////////////////////////////////////////
	// Methods for the XML section
	int InitXml( int next );
	int EndXml( void );
	int UpdateXml( sexcatstruct *sexcat, picstruct *dfield, 
							picstruct *field, picstruct *dwfield, 
											picstruct *wfield );
	int WriteXml( char *filename );
	int WriteXmlHeader( FILE *file );
	int WriteXmlMeta( FILE *file, char *error );
	void WriteXmlError( char *filename, char *error );

// data
public:
//	CLogger*		m_pLogger;
	CUnimapWorker*	m_pUnimapWorker;
	CAstroImage*	m_pAstroImage;
//	CImgView*		m_pImgView;
	// config
	CConfigDetect*	m_pDetectConfig;
	CConfigMain*	m_pMainConfig;
 
	// flags
	int m_bIsDataInit;

	// larry's params
	int m_nImageChannels;
	int m_bUseAreasToMask;

	////////////////////////////////////////////////////////////////
	// Data for PARAM
	objstruct	outobj;
	obj2struct	outobj2;

	////////////////////////////////////////////////////////////////
	// Data for GLOBAL
	sexcatstruct		thecat;
	picstruct		thefield1,thefield2, thewfield1,thewfield2;
	objstruct			flagobj;
	obj2struct			flagobj2;
//	obj2struct			outobj2;
	float				ctg[37], stg[37];
	char				gstr[MAXCHAR];
	// time vars
	time_t	thetimet, thetimet2;

	unsigned long   m_nGlobImgPointer;

	////////////////////////////////////////////////////////////////
	// Data for PLIST
	int	plistexist_value, plistexist_dvalue, plistexist_cdvalue,
		plistexist_flag, plistexist_wflag, plistexist_dthresh, plistexist_var,
		plistoff_value, plistoff_dvalue, plistoff_cdvalue,
		plistoff_flag[MAXFLAG], plistoff_wflag, plistoff_dthresh, plistoff_var,
		plistsize;

	////////////////////////////////////////////////////////////////
	// Data for EXTRACTOR
	PIXTYPE		*dumscan;
	infostruct	*info, *store;
	char		*marker;
	status		*psstack;
	int		*start, *end, *discan, xmin,ymin,xmax,ymax;

	////////////////////////////////////////////////////////////////
	// Data for CATOUT
	catstruct	*fitscat;
	tabstruct	*objtab;
	FILE		*ascfile;
	char		*buf;
	int			catopen_flag;
	// hardcode replacement for default.param file - todo :: dialog selector
	int			m_nCatParams;
	char		m_vectCatParams[200][MAXCHAR];

	////////////////////////////////////////////////////////////////
	// Data for CLEAN
	LONG		*cleanvictim;
	objliststruct	*cleanobjlist; // laconic, isn't it? 		

	////////////////////////////////////////////////////////////////
	// Data for FILTER
	filterstruct	*thefilter;

	////////////////////////////////////////////////////////////////
	// Data for GRAPH
	double	sexx1, sexy1;

	////////////////////////////////////////////////////////////////
	// Data for GROWTH
	double	*growth;
	int	ngrowth;

	////////////////////////////////////////////////////////////////
	// Data for IMAGE
	float	interpm[INTERPW*INTERPH];

	////////////////////////////////////////////////////////////////
	// Data for NEURRO
	brainstruct	*brain;

	////////////////////////////////////////////////////////////////
	// Data for PSF
//	keystruct	objkey[];
//	objstruct	outobj;
	psfstruct	*psf,*ppsf,*thepsf;
	psfitstruct	*thepsfit,*ppsfit,*psfit;
	PIXTYPE		*checkmask;

	////////////////////////////////////////////////////////////////
	// Data for RETINA
	retistruct	*theretina;

	////////////////////////////////////////////////////////////////
	// Data for REFINE
	objliststruct	*objlistRefine;
	short		*son, *ok;

	////////////////////////////////////////////////////////////////
	// Data for SOM
	somstruct	*thesom;

	////////////////////////////////////////////////////////////////
	// Data for PREFS
	prefstruct		prefs;

	////////////////////////////////////////////////////////////////
	// Data for XML
	xmlstruct*	xmlstack;
	int			nxml;
	int			nxmlmax;

private:
	keystruct	headkey1[100];
	int m_nHeadKeyOne;

	keystruct	headkey[100];
	int m_nHeadKey;

	keystruct	objkey[1000];
	int m_nObjKey;

	pkeystruct	key[100];
	int m_nKey;

	int	idummy;
	double	ddummy;
	
	//char		keylist[sizeof(key)/sizeof(pkeystruct)][32];
	char**		keylist;

	static char* default_prefs[];
};

#endif
