

#include <cmath>
#include <string>
#include <vector>
#include <iostream>



//yeah, we're working with fixed sizes again...
const int N = 128; //the width of the image
const int M = 128; //the height of the image

double fRe[N][M][3], fIm[N][M][3], fAmp[N][M][3]; //the signal's real part, imaginary part, and amplitude
double FRe[N][M][3], FIm[N][M][3], FAmp[N][M][3]; //the FT's real part, imaginary part and amplitude


// allocate
 fRe = (double***) calloc( h, sizeof( double** ) );
 fIm = (double***) calloc( h, sizeof( double** ) );
 fAmp = (double***) calloc( h, sizeof( double** ) );
 for( x=0; x<w; x++ )
 {
 	fRe[x] = (double**) calloc( h, sizeof( double* ) );
 	fIm[x] = (double**) calloc( h, sizeof( double* ) );
 	fAmp[x] = (double**) calloc( h, sizeof( double* ) );
 	for( y=0; y<h; y++ )
 	{
 		fRe[x][y] = (double*) malloc( 3*sizeof( double ) );
 		fIm[x][y] = (double*) malloc( 3*sizeof( double ) );
 		fAmp[x][y] = (double*) malloc( 3*sizeof( double ) );
	}
 }

  //set signal to the image
  for(int x = 0; x < N; x++)
  {
	  for(int y = 0; y < M; y++)
	  {
		fRe[x][y][0] = img[N * y + x].r;
		fRe[x][y][1] = img[N * y + x].g;
		fRe[x][y][2] = img[N * y + x].b;
	  }
	}

  int ytrans=8; //translate everything a bit down to put the text on top

  //set new FT buffers
  for(int x = 0; x < w; x++)
  {
	  for(int y = 0; y < h; y++)
	  {
		  for(int c = 0; c < 3; c++)
		  {
			FRe2[x][y][c] = FRe[x][y][c];
			FIm2[x][y][c] = FIm[x][y][c];
		  }
		}
	}

  bool changed = 1, endloop = 0;
  while(!endloop)
  {
    if(done()) end();
    readKeys();
    if(keyPressed(SDLK_a) || changed) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {fRe2[x][y][c] = fRe[x][y][c];} changed = 1;} //no effect
    if(keyPressed(SDLK_b)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {fRe2[x][y][c] = 255 - fRe[x][y][c];} changed = 1;} //negative
    if(keyPressed(SDLK_c)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {fRe2[x][y][c] = fRe[x][y][c]/2;} changed = 1;} //half amplitude
    if(keyPressed(SDLK_d)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {if((x % 4 == 0) || (y%4 == 0)) {fRe2[x][y][c] = 0;} else{fRe2[x][y][c] = fRe[x][y][c];}} changed = 1;} //grid
    if(keyPressed(SDLK_e)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {if((x % 8 == 0) || (y%8 == 0)) {fRe2[x][y][c] = 0;} else{fRe2[x][y][c] = fRe[x][y][c];}} changed = 1;} //grid
    if(keyPressed(SDLK_f)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {if((x % 16 == 0) || (y%16 == 0)) {fRe2[x][y][c] = 0;} else{fRe2[x][y][c] = fRe[x][y][c];}} changed = 1;} //grid
    if(keyPressed(SDLK_g)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {if((x % 32==0) || (y%32==0)) {fRe2[x][y][c] = 0;} else{fRe2[x][y][c] = fRe[x][y][c];}} changed = 1;} //grid
    if(keyPressed(SDLK_h)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {fRe2[x][y][c] = 128 + 128 * sin(3.1415*x / 8.0);} changed = 1;} //grid
    if(keyPressed(SDLK_i)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {fRe2[x][y][c] = 128 + 128 * sin(3.1415*y / 8.0);} changed = 1;} //grid
    if(keyPressed(SDLK_j)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {fRe2[x][y][c] = 128 + 128 * sin(3.1415*(x + y) / 8.0);} changed = 1;} //grid
    if(keyPressed(SDLK_k)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) {fRe2[x][y][0] = 128 + 128 * sin(3.1415*x / 8.0); fRe2[x][y][1] = 128 + 128 * sin(3.1415*y / 8.0); fRe2[x][y][2] = 128 + 128 * sin(3.1415*(x + y) / 8.0);} changed = 1;} //grid
    if(keyPressed(SDLK_l)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {fRe2[x][y][c] = fRe[(x+N/8) % N][(y+M/8) % M][c];} changed = 1;} //translate image
    if(keyPressed(SDLK_m)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {fRe2[x][y][c] = fRe[(x+N/4) % N][(y+M/4) % M][c];} changed = 1;} //translate image
    if(keyPressed(SDLK_n)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {fRe2[x][y][c] = fRe[(x+N / 2) % N][(y+M / 2) % M][c];} changed = 1;} //translate image
    if(keyPressed(SDLK_o)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {fRe2[x][y][c] = fRe[(x + y) % N][y][c];} changed = 1;} //skew image
    if(keyPressed(SDLK_p)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {if((x < N / 2 + 4&& x > N / 2 - 4) && (y < M / 2 + 4 && y > M / 2 - 4)) {fRe2[x][y][c] = 255;} else{fRe2[x][y][c] = 0;}} changed = 1;} //square
    if(keyPressed(SDLK_q)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {if((x < N / 2 + 8&& x > N / 2 - 8) && (y < M / 2 + 8 && y > M / 2 - 8)) {fRe2[x][y][c] = 255;} else{fRe2[x][y][c] = 0;}} changed = 1;} //square
    if(keyPressed(SDLK_r)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {if((x < N / 2 + 16 && x > N / 2 - 16) && (y < M / 2 + 16 && y > M / 2 - 16)) {fRe2[x][y][c] = 255;} else{fRe2[x][y][c] = 0;}} changed = 1;} //square
    if(keyPressed(SDLK_s)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {if((x < N / 2 + 32 && x > N / 2 - 32) && (y < M / 2 + 32 && y > M / 2 - 32)) {fRe2[x][y][c] = 255;} else{fRe2[x][y][c] = 0;}} changed = 1;} //square
    if(keyPressed(SDLK_t)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {if(x + y <(N+M)/2) {fRe2[x][y][c] = 255;} else{fRe2[x][y][c] = 0;}} changed = 1;} //triangle
    if(keyPressed(SDLK_u)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {if((x - N / 2) * (x - N / 2) + (y - M / 2) * (y - M / 2) < 64 || (N - (x - N / 2)) * (N - (x - N / 2)) + (M - (y - M / 2)) * (M - (y - M / 2)) < 64 || (x - N / 2) * (x - N / 2) + (M - (y - M / 2)) * (M - (y - M / 2)) < 64 || (N - (x - N / 2)) *(N - (x - N / 2)) + (y - M / 2) *(y - M / 2) < 64) {fRe2[x][y][c] = 255;} else{fRe2[x][y][c] = 0;}} changed = 1;} //circle
    if(keyPressed(SDLK_v)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {if((x - N / 2) * (x - N / 2) + (y - M / 2) * (y - M / 2) < 512 || (N - (x - N / 2)) * (N - (x - N / 2)) + (M - (y - M / 2)) *(M - (y - M / 2)) < 512 || (x - N / 2) * (x - N / 2) + (M - (y - M / 2)) * (M - (y - M / 2)) < 512 || (N - (x - N / 2)) *(N - (x - N / 2)) + (y - M / 2) * (y - M / 2) < 512) {fRe2[x][y][c] = 255;} else{fRe2[x][y][c] = 0;}} changed = 1;} //circle
    if(keyPressed(SDLK_w)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {if((x - N / 2) * (x - N / 2) + (y - M / 2) * (y - M / 2) < 64 || (N - (x - N / 2)) * (N - (x - N / 2)) + (M - (y - M / 2)) *(M - (y - M / 2)) < 64 || (x - N / 2) * (x - N / 2) + (M - (y - M / 2)) * (M - (y - M / 2)) < 64 || (N - (x - N / 2)) *(N - (x - N / 2)) + (y - M / 2) * (y - M / 2) < 64) {fRe2[x][y][c] = fRe[x][y][c];} else{fRe2[x][y][c] = 0;}} changed = 1;} //circle of image
    if(keyPressed(SDLK_x)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {if((x - N / 2) * (x - N / 2) + (y - M / 2) * (y - M / 2) < 512 || (N - (x - N / 2)) * (N - (x - N / 2)) + (M - (y - M / 2)) *(M - (y - M / 2)) < 512 || (x - N / 2) * (x - N / 2) + (M - (y - M / 2)) * (M - (y - M / 2)) < 512 || (N - (x - N / 2)) *(N - (x - N / 2)) + (y - M / 2) * (y - M / 2) < 512) {fRe2[x][y][c] = fRe[x][y][c];} else{fRe2[x][y][c] = 0;}} changed = 1;} //circle of image
    if(keyPressed(SDLK_UP)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {fIm2[x][y][c] = fRe2[x][y][c];} changed = 1;} //imaginary part = real part
    if(keyPressed(SDLK_DOWN)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {fIm2[x][y][c] = 0;} changed = 1;} //imaginary part=0
    if(keyPressed(SDLK_SPACE)) endloop= 1;
    if(changed)
    {
      //Draw the image and it's FT
      calculateAmp(N, M, fAmp2[0][0], fRe2[0][0], fIm2[0][0]);
      draw(0, ytrans, N, M, fRe2[0][0], 0, 0);  draw(N, 0+ytrans, N, M, fIm2[0][0], 0, 0);  draw(2 * N, 0+ytrans, N, M, fAmp2[0][0], 0, 0); //draw real, imag and amplitude
      FFT2D(N, M, 0, fRe2[0][0], fIm2[0][0], FRe[0][0], FIm[0][0]);
      calculateAmp(N, M, FAmp[0][0], FRe[0][0], FIm[0][0]);
      draw(0,M + ytrans, N, M, FRe[0][0], 1, 1);  draw(N, M + ytrans, N, M, FIm[0][0], 1, 1);  draw(2 * N, M + ytrans, N, M, FAmp[0][0], 1, 0); //draw real, imag and amplitude
      print("Press a-z for effects and space to accept", 0, 0, RGB_White, 1, ColorRGB(128, 0, 0));
      redraw();
    }
    changed=0;
  }
  changed = 1;
  while(!done())
  {
    readKeys();
    if(keyPressed(SDLK_a) || changed) { for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {FRe2[x][y][c] = FRe[x][y][c]; FIm2[x][y][c] = FIm[x][y][c];} changed = 1;} //no filter
    if(keyPressed(SDLK_b)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {FRe2[x][y][c] = FRe[x][y][c]; FIm2[x][y][c] = 0;} changed = 1;} //imaginary part 0
    if(keyPressed(SDLK_c)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {FRe2[x][y][c] = 0; FIm2[x][y][c] = FIm[x][y][c];} changed = 1;} //real part 0
    if(keyPressed(SDLK_d)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {FRe2[x][y][c] = FIm[x][y][c]; FIm2[x][y][c] = FRe[x][y][c];} changed = 1;} //swap real and imag part
    if(keyPressed(SDLK_e)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {if(x * x + y * y < 16 || (N - x) * (N - x) + (M - y) * (M - y) < 16 || x * x + (M - y) * (M - y) < 16 || (N - x) * (N - x) + y * y < 16) {FRe2[x][y][c] = FRe[x][y][c]; FIm2[x][y][c] = FIm[x][y][c];} else{FRe2[x][y][c] = 0; FIm2[x][y][c] = 0;}} changed = 1;} //LP filter
    if(keyPressed(SDLK_f)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {if(x * x + y * y <64 || (N - x) * (N - x) + (M - y) * (M - y) < 64 || x * x + (M - y) * (M - y) < 64 || (N - x) * (N - x) + y * y <64) {FRe2[x][y][c] = FRe[x][y][c]; FIm2[x][y][c] = FIm[x][y][c];} else{FRe2[x][y][c] = 0; FIm2[x][y][c] = 0;}} changed = 1;} //LP filter
    if(keyPressed(SDLK_g)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {if(x * x + y * y < 256 || (N - x) * (N - x) + (M - y) * (M - y) < 256 || x * x + (M - y) * (M - y) < 256 || (N - x) * (N - x) + y * y < 256) {FRe2[x][y][c] = FRe[x][y][c]; FIm2[x][y][c] = FIm[x][y][c];} else{FRe2[x][y][c] = 0; FIm2[x][y][c] = 0;}} changed = 1;} //LP filter
    if(keyPressed(SDLK_h)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {if(x * x + y * y < 1024 || (N - x) * (N - x) + (M - y) * (M - y) < 1024 || x * x + (M - y) * (M - y) < 1024 || (N - x) * (N - x) + y * y < 1024) {FRe2[x][y][c] = FRe[x][y][c]; FIm2[x][y][c] = FIm[x][y][c];} else{FRe2[x][y][c] = 0; FIm2[x][y][c] = 0;}} changed = 1;} //LP filter
    if(keyPressed(SDLK_i)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {if(x * x + y * y > 16 && (N - x) * (N - x) + (M - y) * (M - y) > 16 && x * x + (M - y) * (M - y) > 16 && (N - x) * (N - x) + y * y > 16) {FRe2[x][y][c] = FRe[x][y][c]; FIm2[x][y][c] = FIm[x][y][c];} else{FRe2[x][y][c] = 0; FIm2[x][y][c] = 0;}} changed = 1;} //HP filter
    if(keyPressed(SDLK_j)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c=0;c<3;c++ ) {if(x * x + y * y > 64&& (N - x) * (N - x) + (M - y) * (M - y) > 64&& x * x + (M - y) * (M - y) > 64&& (N - x) * (N - x) + y * y > 64) {FRe2[x][y][c] = FRe[x][y][c]; FIm2[x][y][c] = FIm[x][y][c];} else{FRe2[x][y][c] = 0; FIm2[x][y][c] = 0;}} changed = 1;} //HP filter
    if(keyPressed(SDLK_k)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {if(x * x + y * y > 256 && (N - x) * (N - x) + (M - y) * (M - y) > 256 && x * x + (M - y) * (M - y) > 256 && (N - x) * (N - x) + y * y > 256) {FRe2[x][y][c] = FRe[x][y][c]; FIm2[x][y][c] = FIm[x][y][c];} else{FRe2[x][y][c] = 0; FIm2[x][y][c] = 0;}} changed = 1;} //HP filter
    if(keyPressed(SDLK_l)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {if(x * x + y * y >512 && (N - x) * (N - x) + (M - y) * (M - y) > 512 && x * x + (M - y) * (M - y) > 512 && (N - x) * (N - x) + y * y >512) {FRe2[x][y][c] = FRe[x][y][c]; FIm2[x][y][c] = FIm[x][y][c];} else{FRe2[x][y][c] = 0; FIm2[x][y][c] = 0;}} changed = 1;} //HP filter
    if(keyPressed(SDLK_m)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {FRe2[x][y][c] = 0; FIm2[x][y][c] = 0;} changed = 1;} //make all zero
    if(keyPressed(SDLK_n)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {FRe2[x][y][c] = FRe[x][y][c]/2; FIm2[x][y][c] = FIm[x][y][c];} changed = 1;} //divide real through 2
    if(keyPressed(SDLK_o)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {FRe2[x][y][c] = FRe[x][y][c]; FIm2[x][y][c] = FIm[x][y][c]/2;} changed = 1;} //divide imag through 2
    if(keyPressed(SDLK_p)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {if((x < 4 || x > N - 4) && (y < 4 || y > N -4)) {FRe2[x][y][c] = FRe[x][y][c]; FIm2[x][y][c] = FIm[x][y][c];} else{FRe2[x][y][c] = 0; FIm2[x][y][c] = 0;}} changed = 1;} //square LP filter
    if(keyPressed(SDLK_q)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {if((x < 8||x > N -8) && (y < 8 || y > N -8)) {FRe2[x][y][c] = FRe[x][y][c]; FIm2[x][y][c] = FIm[x][y][c];} else{FRe2[x][y][c] = 0; FIm2[x][y][c] = 0;}} changed = 1;} //square LP filter
    if(keyPressed(SDLK_r)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {if((x < 16 || x > N - 16) && (y < 16||y > N -16)) {FRe2[x][y][c] = FRe[x][y][c]; FIm2[x][y][c] = FIm[x][y][c];} else{FRe2[x][y][c] = 0; FIm2[x][y][c] = 0;}} changed = 1;} //square LP filter
    if(keyPressed(SDLK_s)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {if((x * x + y * y < 256 || (N - x) * (N - x) + (M - y) * (M - y) < 256 || x * x + (M - y) * (M - y) < 256 || (N - x) * (N - x) + y * y < 256) && (x * x + y * y > 128 && (N - x) * (N - x) + (M - y) * (M - y) > 128&& x * x + (M - y) * (M - y) > 128 && (N - x) * (N - x) + y * y > 128)) {FRe2[x][y][c] = FRe[x][y][c]; FIm2[x][y][c] = FIm[x][y][c];} else{FRe2[x][y][c] = 0; FIm2[x][y][c] = 0;}} changed = 1;} //BP filter
    if(keyPressed(SDLK_t)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {if((x * x + y * y < 512 || (N - x) * (N - x) + (M - y) * (M - y) < 512||x * x + (M - y) * (M - y) < 512 || (N - x) * (N - x) + y * y < 512) && (x * x + y * y > 256 && (N - x) * (N - x) + (M - y) * (M - y) > 256 && x * x + (M - y) * (M - y) > 256 && (N - x) * (N - x) + y * y > 256)) {FRe2[x][y][c] = FRe[x][y][c]; FIm2[x][y][c] = FIm[x][y][c];} else{FRe2[x][y][c] = 0; FIm2[x][y][c] = 0;}} changed = 1;} //BP filter
    if(keyPressed(SDLK_u)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {FRe2[x][y][c] = cos(3.1415*x / 8.0) * FRe[x][y][c]; FIm2[x][y][c] = cos(3.1415*x / 8.0) * FIm[x][y][c];} changed = 1;} //some cos function
    if(keyPressed(SDLK_v)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {FRe2[x][y][c] = cos(y / 8.0) * FRe[x][y][c]; FIm2[x][y][c] = cos(y / 8.0) * FIm[x][y][c];} changed = 1;} //some cos function
    if(keyPressed(SDLK_w)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {FRe2[x][y][c] = cos(x / 8.0) * FRe[x][y][c]; FIm2[x][y][c] = cos(y / 8.0) * FIm[x][y][c];} changed = 1;} //some cos function
    if(keyPressed(SDLK_x)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {FRe2[x][y][c] = cos(x/4.0) * FRe[x][y][c]; FIm2[x][y][c] = cos(y/4.0) * FIm[x][y][c];} changed = 1;} //some cos function
    if(keyPressed(SDLK_y)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {if(x == 0||y == 0) {FRe2[x][y][c] = 0; FIm2[x][y][c] = 0;} else{FRe2[x][y][c] = FRe[x][y][c]; FIm2[x][y][c] = FIm[x][y][c];}} changed = 1;} //take out horizontal and vertical line through the center
    if(keyPressed(SDLK_z)) {for(int x = 0; x < N; x++) for(int y = 0; y < M; y++) for(int c = 0; c < 3; c++) {FRe2[x][y][c] = FRe[x][y][c]/2; FIm2[x][y][c] = FIm[x][y][c]/2;} changed = 1;} //divide all through 2
    if(keyPressed(SDLK_UP)) {FRe2[0][0][0] = FRe[0][0][0]; FRe2[0][0][1] = FRe[0][0][1]; FRe2[0][0][2] = FRe[0][0][2]; FIm2[0][0][0] = FIm[0][0][0]; FIm2[0][0][1] = FIm[0][0][1]; FIm2[0][0][2] = FIm[0][0][2]; changed = 1;} //Bring DC back
    if(keyPressed(SDLK_DOWN)) {FRe2[0][0][0] = 0; FRe2[0][0][1] = 0; FRe2[0][0][2] = 0; FIm2[0][0][0] = 0; FIm2[0][0][1] = 0; FIm2[0][0][2] = 0; changed = 1;} //remove DC
    if(keyPressed(SDLK_LEFT)) {FRe2[0][0][0] = FIm[0][0][0]; FRe2[0][0][1] = FIm[0][0][1]; FRe2[0][0][2] = FIm[0][0][2]; FIm2[0][0][0] = FRe[0][0][0]; FIm2[0][0][1] = FRe[0][0][1]; FIm2[0][0][2] = FRe[0][0][2]; changed = 1;} //Bring DC back
    if(keyPressed(SDLK_RIGHT)) {FRe2[0][0][0] = N * M; FRe2[0][0][1] = N * M; FRe2[0][0][2] = N * M; FIm2[0][0][0] = N * M; FIm2[0][0][1] = N * M; FIm2[0][0][2] = N * M; changed = 1;} //remove DC
    if(changed)
    {
      //after pressing a key: calculate the inverse!
      FFT2D(N, M, 1, FRe2[0][0], FIm2[0][0], fRe[0][0], fIm[0][0]);
      calculateAmp(N, M, fAmp[0][0], fRe[0][0], fIm[0][0]);
      draw(0, 3 * M + ytrans, N, M, fRe[0][0], 0, 0);  draw(N, 3 * M + ytrans, N, M, fIm[0][0], 0, 0);  draw(2 * N, 3 * M + ytrans, N, M, fAmp[0][0], 0, 0);
      calculateAmp(N, M, FAmp2[0][0], FRe2[0][0], FIm2[0][0]);
      draw(0, 2 * M + ytrans, N, M, FRe2[0][0], 1, 1);  draw(N, 2 * M + ytrans, N, M, FIm2[0][0], 1, 1);  draw(2 * N, 2 * M + ytrans, N, M, FAmp2[0][0], 1, 0);
      print("Press a-z for filters and arrows to change DC", 0, 0, RGB_White, 1, ColorRGB(128, 0, 0));
      redraw();
    }
    changed=0;
  }
  return 0;
}

void DFT2D( int n, int m, bool inverse, double *gRe, double *gIm, double *GRe, double *GIm )
{
  std::vector<double> Gr2(m * n * 3);
  std::vector<double> Gi2(m * n * 3); //temporary buffers

	//calculate the fourier transform of the columns
	for(int x = 0; x < n; x++)
	{
		for(int c = 0; c < 3; c++)
		{
			print(" % done",8, 0, RGB_White, 1);
			print(50 * x / n, 0, 0, RGB_White, 1);
			if(done()) end();
			redraw();
			//This is the 1D DFT:
			for(int w = 0; w < m; w++)
			{
				Gr2[m * 3 * x + 3 * w + c] = Gi2[m * 3 * x + 3 * w + c] = 0;
				for(int y = 0; y < m; y++)
				{
					double a= 2 * pi * w * y / double(m);
					if(!inverse)a = -a;
					double ca = cos(a);
					double sa = sin(a);
					Gr2[m * 3 * x + 3 * w + c] += gRe[m * 3 * x + 3 * y + c] * ca - gIm[m * 3 * x + 3 * y + c] * sa;
					Gi2[m * 3 * x + 3 * w + c] += gRe[m * 3 * x + 3 * y + c] * sa + gIm[m * 3 * x + 3 * y + c] * ca;
				}

			}
		}
	}

  //calculate the fourier transform of the rows
  for(int y = 0; y < m; y++)
  for(int c = 0; c < 3; c++)
  {
    print(" % done",8, 0, RGB_White, 1);
    print(50 + 50 * y / m, 0, 0, RGB_White, 1);
    if(done()) end();
    redraw();
    //This is the 1D DFT:
    for(int w = 0; w < n; w++)
    {
      GRe[m * 3 * w + 3 * y + c] = GIm[m * 3 * w + 3 * y + c] = 0;
      for(int x = 0; x < n; x++)
      {
        double a = 2 * pi * w * x / double(n);
        if(!inverse)a = -a;
        double ca = cos(a);
        double sa = sin(a);
        GRe[m * 3 * w + 3 * y + c] += Gr2[m * 3 * x + 3 * y + c] * ca - Gi2[m * 3 * x + 3 * y + c] * sa;
        GIm[m * 3 * w + 3 * y + c] += Gr2[m * 3 * x + 3 * y + c] * sa + Gi2[m * 3 * x + 3 * y + c] * ca;
      }
        if(inverse)
        {
          GRe[m * 3 * w + 3 * y + c] /= n;
          GIm[m * 3 * w + 3 * y + c] /= n;
        }
        else{
          GRe[m * 3 * w + 3 * y + c] /= m;
          GIm[m * 3 * w + 3 * y + c] /= m;
        }
    }
  }
}

void FFT2D(int n, int m, bool inverse, double *gRe, double *gIm, double *GRe, double *GIm)
{
  int l2n = 0, p = 1; //l2n will become log_2(n)
  while(p < n) {p *= 2; l2n++;}
  int l2m = 0; p = 1; //l2m will become log_2(m)
  while(p < m) {p *= 2; l2m++;}

  m= 1<<l2m; n= 1<<l2n; //Make sure m and n will be powers of 2, otherwise you'll get in an infinite loop

  //Erase all history of this array
  for(int x = 0; x <m; x++) //for each column
  for(int y = 0; y < m; y++) //for each row
  for(int c = 0; c < 3; c++) //for each color component
  {
    GRe[3 * m * x + 3 * y + c] = gRe[3 * m * x + 3 * y + c];
    GIm[3 * m * x + 3 * y + c] = gIm[3 * m * x + 3 * y + c];
  }

  //Bit reversal of each row
  int j;
  for(int y = 0; y < m; y++) //for each row
  for(int c = 0; c < 3; c++) //for each color component
  {
    j = 0;
    for(int i = 0; i < n - 1; i++)
    {
      GRe[3 * m * i + 3 * y + c] = gRe[3 * m * j + 3 * y + c];
      GIm[3 * m * i + 3 * y + c] = gIm[3 * m * j + 3 * y + c];
      int k = n / 2;
      while (k <= j) {j -= k; k/= 2;}
      j += k;
    }
  }
  //Bit reversal of each column
  double tx = 0, ty = 0;
  for(int x = 0; x < n; x++) //for each column
  for(int c = 0; c < 3; c++) //for each color component
  {
    j = 0;
    for(int i = 0; i < m - 1; i++)
    {
      if(i < j)
      {
        tx = GRe[3 * m * x + 3 * i + c];
        ty = GIm[3 * m * x + 3 * i + c];
        GRe[3 * m * x + 3 * i + c] = GRe[3 * m * x + 3 * j + c];
        GIm[3 * m * x + 3 * i + c] = GIm[3 * m * x + 3 * j + c];
        GRe[3 * m * x + 3 * j + c] = tx;
        GIm[3 * m * x + 3 * j + c] = ty;
      }
      int k = m / 2;
      while (k <= j) {j -= k; k/= 2;}
      j += k;
    }
  }

  //Calculate the FFT of the columns
  for(int x = 0; x < n; x++) //for each column
  for(int c = 0; c < 3; c++) //for each color component
  {
    //This is the 1D FFT:
    double ca = -1.0;
    double sa = 0.0;
    int l1 = 1, l2 = 1;
    for(int l=0;l<l2n;l++)
    {
      l1 = l2;
      l2 *= 2;
      double u1 = 1.0;
      double u2 = 0.0;
      for(int j = 0; j < l1; j++)
      {
        for(int i = j; i < n; i += l2)
        {
          int i1 = i + l1;
          double t1 = u1 * GRe[3 * m * x + 3 * i1 + c] - u2 * GIm[3 * m * x + 3 * i1 + c];
          double t2 = u1 * GIm[3 * m * x + 3 * i1 + c] + u2 * GRe[3 * m * x + 3 * i1 + c];
          GRe[3 * m * x + 3 * i1 + c] = GRe[3 * m * x + 3 * i + c] - t1;
          GIm[3 * m * x + 3 * i1 + c] = GIm[3 * m * x + 3 * i + c] - t2;
          GRe[3 * m * x + 3 * i + c] += t1;
          GIm[3 * m * x + 3 * i + c] += t2;
        }
        double z =  u1 * ca - u2 * sa;
        u2 = u1 * sa + u2 * ca;
        u1 = z;
      }
      sa = sqrt((1.0 - ca) / 2.0);
      if(!inverse) sa = -sa;
      ca = sqrt((1.0 + ca) / 2.0);
    }
  }
  //Calculate the FFT of the rows
  for(int y = 0; y < m; y++) //for each row
  for(int c = 0; c < 3; c++) //for each color component
  {
    //This is the 1D FFT:
    double ca = -1.0;
    double sa = 0.0;
    int l1= 1, l2 = 1;
    for(int l = 0; l < l2m; l++)
    {
      l1 = l2;
      l2 *= 2;
      double u1 = 1.0;
      double u2 = 0.0;
      for(int j = 0; j < l1; j++)
      {
        for(int i = j; i < n; i += l2)
        {
          int i1 = i + l1;
          double t1 = u1 * GRe[3 * m * i1 + 3 * y + c] - u2 * GIm[3 * m * i1 + 3 * y + c];
          double t2 = u1 * GIm[3 * m * i1 + 3 * y + c] + u2 * GRe[3 * m * i1 + 3 * y + c];
          GRe[3 * m * i1 + 3 * y + c] = GRe[3 * m * i + 3 * y + c] - t1;
          GIm[3 * m * i1 + 3 * y + c] = GIm[3 * m * i + 3 * y + c] - t2;
          GRe[3 * m * i + 3 * y + c] += t1;
          GIm[3 * m * i + 3 * y + c] += t2;
        }
        double z =  u1 * ca - u2 * sa;
        u2 = u1 * sa + u2 * ca;
        u1 = z;
      }
      sa = sqrt((1.0 - ca) / 2.0);
      if(!inverse) sa = -sa;
      ca = sqrt((1.0 + ca) / 2.0);
    }
  }

  int d;
  if(inverse) d = n; else d = m;
  for(int x = 0; x < n; x++) for(int y = 0; y < m; y++) for(int c = 0; c < 3; c++) //for every value of the buffers
  {
    GRe[3 * m * x + 3 * y + c] /= d;
    GIm[3 * m * x + 3 * y + c] /= d;
  }
}

//Draws an image
void draw(int xPos, int yPos, int n, int m, double *g, bool shift, bool neg128) //g is the image to be drawn
{
  for(int x = 0; x < n; x++)
  for(int y = 0; y < m; y++)
  {
    int x2 = x, y2 = y;
    if(shift) {x2 = (x + n / 2) % n; y2 = (y + m / 2) % m;} //Shift corners to center
    ColorRGB color;
    //calculate color values out of the floating point buffer
    color.r = int(g[3 * m * x2 + 3 * y2 + 0]);
    color.g = int(g[3 * m * x2 + 3 * y2 + 1]);
    color.b = int(g[3 * m * x2 + 3 * y2 + 2]);

    if(neg128) color = color + RGB_Gray;
    //negative colors give confusing effects so set them to 0
    if(color.r < 0) color.r = 0;
    if(color.g < 0) color.g = 0;
    if(color.b < 0) color.b = 0;
    //set color components higher than 255 to 255
    if(color.r > 255) color.r = 255;
    if(color.g > 255) color.g = 255;
    if(color.b > 255) color.b = 255;
    //plot the pixel
    pset(x + xPos, y + yPos, color);
  }
}

//Calculates the amplitude of *gRe and *gIm and puts the result in *gAmp
void calculateAmp(int n, int m, double *gAmp, double *gRe, double *gIm)
{
  for(int x = 0; x < n; x++)
  for(int y = 0; y < m; y++)
  for(int c = 0; c < 3; c++)
  {
    gAmp[m * 3 * x + 3 * y + c] = sqrt(gRe[m * 3 * x + 3 * y + c] * gRe[m * 3 * x + 3 * y + c] + gIm[m * 3 * x + 3 * y + c] * gIm[m * 3 * x + 3 * y + c]);
  }
}
