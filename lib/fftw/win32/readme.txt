This project was prepared in Visual Studio 2008 SP1 Express Edition for FFTW 3.2. It
was created to build proper static and dynamic libraries for developers that use the
Visual Studio IDE and compiler.

There are 3 build configurations:
	1) Debug (Static)
	2) Release (Static)
	3) Release-DLL (Dynamic)
	
- All builds rely on a properly configured config.h file in the root FFTW folder.
  The one provided should be sufficient for standard VS 2008 installs.
  
- All builds will output the necessary files in the ./win32/output folder. It is
  created and populated as a Post-Build event.
	./win32/output/bin		- Stores bench.exe and libfftw.dll
	./win32/output/inc		- Stores the header file fftw3.h
	./win32/output/lib		- Stores the libfftw.lib

- All builds use the SSE2 extension. If you do not support that extension,
  remove HAVE_SSE2 from the project settings: C/C++ / Preprocessor / Preprocessor Definitions.
  You will also need to undefine it (comment out) from the config.h file.


Questions / Comments?
Contact: Nathaniel Meyer
Email:	 nate@nutty.ca