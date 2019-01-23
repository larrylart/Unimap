
// wx include
#include "wx/wxprec.h"

// opencv library
#include "cv.h"

// fftw library
#include "../lib/fftw/api/fftw3.h"

// local library
#include "../util/func.h"

// main header
#include "imagefft.h"

//larry : hack to bypass warning cast
#undef GetGValue
#define GetGValue(rgb) (LOBYTE((rgb) >> 8))

////////////////////////////////////////////////////////////////////
// Method:	constructor
// Class:	CImageFFT
// Purpose:	buildprepare my object
// Input:	wximage ref
// Output:	nothing
////////////////////////////////////////////////////////////////////
CImageFFT::CImageFFT( wxImage& rImage )
{
	int k=0;

	// reset data
	m_vectInputData = NULL;
	m_vectFFTData = NULL;
	m_vectInvFFTData = NULL;

	m_nWidth = rImage.GetWidth();   
	m_nHeight = rImage.GetHeight();   
    
	m_vectInputData = ( fftw_complex* ) fftw_malloc( sizeof( fftw_complex ) * m_nWidth * m_nHeight );
	m_vectFFTData = ( fftw_complex* ) fftw_malloc( sizeof( fftw_complex ) * m_nWidth * m_nHeight );
	m_fftPlan = fftw_plan_dft_2d( m_nHeight, m_nWidth, m_vectInputData, m_vectFFTData,  FFTW_FORWARD,  FFTW_ESTIMATE );

	unsigned char nRed, nGreen, nBlue;
	unsigned int i, j, idx;
	unsigned char* pImgSrc = rImage.GetData();
	// load img1's data to fftw input
	for( i = 0, k = 0; i < m_nHeight ; i++ )
	{
        for( j = 0; j < m_nWidth ; j++ )
		{
			idx = (i*m_nWidth + j)*3;
			// calculate z as color
			nRed = pImgSrc[idx]; 
			nGreen = pImgSrc[idx+1]; 
			nBlue = pImgSrc[idx+2]; 

            m_vectInputData[k][0] = ( double ) ( ( 0.299*nRed + 0.587*nGreen + 0.114*nBlue ) * 0.5 );
            m_vectInputData[k][1] = 0.0;
            k++;
        }
    }

}

////////////////////////////////////////////////////////////////////
// Method:	destructor
// Class:	CImageFFT
// Purpose:	clear/destroy my object data
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CImageFFT::~CImageFFT( )
{
	fftw_destroy_plan( m_fftPlan );

    if( m_vectInputData ) fftw_free( m_vectInputData );
    if( m_vectFFTData ) fftw_free( m_vectFFTData ); 
	if( m_vectInvFFTData ) fftw_free( m_vectInvFFTData );

}

////////////////////////////////////////////////////////////////////
// Method:	DoFFT
// Class:	CImageFFT
// Purpose:	calculate fft for an image - with fftw library
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CImageFFT::DoFFT( )
{
    // perform FFT 
    fftw_execute( m_fftPlan );
}

////////////////////////////////////////////////////////////////////
// Method:	GetImageFFT
// Class:	CImageFFT
// Purpose:	get image after FFT calc
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
IplImage* CImageFFT::GetImageFFT( )
{
//	if( pInputImage == NULL ) return NULL ; 
	
    int i=0, j=0, k=0, l=0;
	int index1=0, index2=0, temp=0;
 
	// create output image
	IplImage* pImageOutput = cvCreateImage( cvSize( m_nWidth, m_nHeight ), IPL_DEPTH_8U, 3 );
	RgbImage iplOutputImgT( pImageOutput );
    
    // perform FFT 
    DoFFT( );

	unsigned char* buf1 = new unsigned char [m_nWidth*m_nHeight];
	Pump_FFTtoPowerSpectraNoShift( buf1, m_vectFFTData, m_nWidth, m_nHeight );

//	double* reOut = (double*) malloc( sizeof(double)* width * height );
//	double* imOut = (double*) malloc( sizeof(double)* width * height );

	// re center
/*	for( i = 0; i <width; i++ ) 
	{
		for( j = 0; j <height; j++ ) 
		{ 
			// Refocuses on the image 
			x = i; 
			y = j; 
			if( i < width/2 && j < height/2 ) { x = i + width/2; y = j + height/2; } 
			if( i >= width/2 && j < height/2 ) { x = i-width/2; y = j + height/2; }
			if( i < width/2 && j >= height/2 ) { x = i + width / 2; y = j-height/2; }
			if( i >= width/2 && j >= height/2 ) { x = i-width/2; y = j-height/2; } 
			reOut[y * width + x] = fft[j * width + i][0]; 
			imOut[y * width + x] = fft[j * width + i][1]; 
		} 
	}
*/

	/////////////////////// new estim ????
	int w = m_nWidth;
	int h = m_nHeight;

	unsigned char nSat=0, nHue=0, nLight=0;

	//i=0;
	j=0;
	//while( i < w ) 
	while( j < h )
	{
		//j = 0;
		i = 0;
		//while( j < h )
		while( i < w )
		{
			index1 = i + w*j; //(i + w*j)*3;
			if (i < w/2) {k=i + (w/2);}
			if (j < h/2) {l=j + (h/2);}
			if (i >=w/2) {k=i - (w/2);}
			if (j >=h/2) {l=j - (h/2);}
			index2 = (k+w*l)*3;

//			k=i; l=j;

//			temp =log(((fourred[index1/3].re * fourred[index1/3].re + fourred[index1/3].im * fourred[index1/3].im)))*5;
//			if (temp < 0) {temp=0;}
//			if (temp > 255) {temp = 255;}
//			im2->rgb_data[index2] = temp;

			//temp = log( (sqr( fft[index1/3][0] ) + sqr( fft[index1/3][1])) )*5;

			//temp = (unsigned char) log( (sqr( fft[index1][0] ) + sqr( fft[index1][1])) )*5;

			//temp = (int) round( atan2( fft[index1][1], fft[index1][0] ) * 40 );

			nHue   = round( ComplexPhi( m_vectFFTData[w*j + i][0], m_vectFFTData[w*j + i][1] ) );
			nLight = buf1[w*j + i];
			nSat   = 128;

			//temp = log( (sqr( reOut[index1] ) + sqr( imOut[index1] )) )*5;
//			if (temp < 0) {temp=0;}
//			if (temp > 255) {temp = 255;}
			//im2->rgb_data[index2+1] = temp;
			DWORD myRGB = (DWORD) HLStoRGB(nHue, nLight , nSat);
			iplOutputImgT[l][k].g = (unsigned char) GetRValue( myRGB );
			iplOutputImgT[l][k].r = (unsigned char) GetGValue( myRGB );
			iplOutputImgT[l][k].b = (unsigned char) GetBValue( myRGB );

//			temp = log(((fourblue[index1/3].re * fourblue[index1/3].re + fourblue[index1/3].im * fourblue[index1/3].im)))*5;
//			if (temp < 0) {temp=0;}
//			if (temp > 255) {temp = 255;}
//			im2->rgb_data[index2+2] = temp;

			//j++;
			i++;
		}
		j++;
		//i++;
       }


 /*   // copy FFT result to img2's data
    for( i = 0, k = 0 ; i < height ; i++ )
	{
		for( j = 0 ; j < width ; j++ ) 
		{
			// shift ?
			//int i2 = (i + height / 2) % height;
			//int j2 = (j + width / 2) % width;

			//img2_data[i * step + j] = ( uchar ) fft[k++][0]; [0] = re; [1] = im
			double vals = sqrt( sqr(reOut[k])+ sqr(imOut[k]) );
			
			//iplOutputImgT[i][j].r = ( uchar ) 10.0*log10( sqr(reOut[k])+ sqr(imOut[k]) );
			//double vals = (double) sqrt( sqr(fft[k][0])+ sqr(fft[k][1]) );
			//iplOutputImgT[i][j].g = (unsigned char) 20.0*log10( vals );

			//iplOutputImgT[i][j].r = 255*log10( 9*( sqr(reOut[k])+ sqr(imOut[k]) )/(255*255) + 1.0);
			//iplOutputImgT[i][j].r = ( uchar ) atan2( fft[k][1], fft[k][0] );
			iplOutputImgT[i][j].r = 0; 
			iplOutputImgT[i][j].b = 0;
			k++;
		}
	}
*/	

//	cvSaveImage( "./fft.bmp", pImageOutput ); 

//	free( reOut );
//	free( imOut );

	return( pImageOutput );
}

////////////////////////////////////////////////////////////////////
// FFT POWER SPECTRUM
////////////////////////////////////////////////////////////////////
void CImageFFT::Pump_FFTtoPowerSpectraNoShift(unsigned char* out, fftw_complex *complex_in, int xsize, int ysize)
{
	double dMaxAmp = log10( (double) (xsize*ysize*255+1) );

	int i;

	for ( i = 0; i<xsize * ysize; i++)
		out[i] = (unsigned char) round( 255*(log10( ComplexAbs(complex_in[i][0],complex_in[i][1])+1)/dMaxAmp) );	
}

// HLStoRGB
DWORD CImageFFT::HLStoRGB(WORD hue,WORD lum, WORD sat)
{
      WORD R,G,B;                /* RGB component values */ 
      WORD  Magic1,Magic2;       /* calculated magic numbers (really!) */ 

      if (sat == 0) {            /* achromatic case */ 
         R=G=B=(lum*RGBMAX)/HLSMAX;
         if (hue != UNDEFINED) {
            /* ERROR */ 
          }
       }
      else  {                    /* chromatic case */ 
         /* set up magic numbers */ 
         if (lum <= (HLSMAX/2))
            Magic2 = (lum*(HLSMAX + sat) + (HLSMAX/2))/HLSMAX;
         else
            Magic2 = lum + sat - ((lum*sat) + (HLSMAX/2))/HLSMAX;
         Magic1 = 2*lum-Magic2;

         /* get RGB, change units from HLSMAX to RGBMAX */ 
         R = (HueToRGB(Magic1,Magic2,hue+(HLSMAX/3))*RGBMAX +
   (HLSMAX/2))/HLSMAX;
         G = (HueToRGB(Magic1,Magic2,hue)*RGBMAX + (HLSMAX/2)) / HLSMAX;
         B = (HueToRGB(Magic1,Magic2,hue-(HLSMAX/3))*RGBMAX +
   (HLSMAX/2))/HLSMAX;
      }
      return(RGB(R,G,B));
}

// HueToRGB
DWORD CImageFFT::HueToRGB(WORD n1,WORD n2,WORD hue)
{
      /* range check: note values passed add/subtract thirds of range */ 
      if (hue < 0)
         hue += HLSMAX;
 
      if (hue > HLSMAX)
         hue -= HLSMAX;

      /* return r,g, or b value from this tridrant */ 
      if (hue < (HLSMAX/6))
          return ( n1 + (((n2-n1)*hue+(HLSMAX/12))/(HLSMAX/6)) );
      if (hue < (HLSMAX/2))
         return ( n2 );
      if (hue < ((HLSMAX*2)/3))
         return ( n1 +    (((n2-n1)*(((HLSMAX*2)/3)-hue)+(HLSMAX/12))/(HLSMAX/6))
   );
      else
         return ( n1 );
}
