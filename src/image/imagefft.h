////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _IMAGE_FFT_H
#define _IMAGE_FFT_H

class CImageFFT
{
// methods
public:
	CImageFFT( wxImage& rImage );
	~CImageFFT( );

	/////////////////////
	// image transforms
	void DoFFT( );
	IplImage* GetImageFFT( );

// data
public:
	unsigned int m_nWidth;
	unsigned int m_nHeight;

	fftw_complex* m_vectInputData;
	fftw_complex* m_vectFFTData;
	fftw_complex* m_vectInvFFTData;
	// process fft plan
	fftw_plan m_fftPlan;

private:
	DWORD HLStoRGB(WORD hue,WORD lum, WORD sat);
	DWORD HueToRGB(WORD n1,WORD n2,WORD hue);

	// color stuff
	static inline double ComplexAbs( double re, double im ){ return sqrt(re*re+im*im); };
	// complex phi
	static inline double ComplexPhi( double re, double im )
	{
		double phi;

		if (re!=0)
			phi = atan(im/re);
		else 
			phi = PI/2;

		if (re<0)
		{
			phi = PI+phi;
		}

		if (phi<0)
		{
			phi = 2*PI+phi;
		}

		return 255*phi/(2.0*PI); 
	}
	// calc power spectrum
	void Pump_FFTtoPowerSpectraNoShift(unsigned char* out, fftw_complex *complex_in, int xsize, int ysize);

};

#endif
