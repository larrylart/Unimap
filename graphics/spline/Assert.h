/****************************************
 * Extended Assert Utility
 * By Bill Perone (billperone@yahoo.com)
 * Original: 17-08-2002
 * Revised:  21-08-2002
 *           22-08-2002
 *           05-12-2003
 *
 * Usage:  Assert(expression, descrition)
 *
 * Notes:  For best functionality make sure that this file is included before all other headers
 *
 * Special Thanks: Steve Rabin, John Robbins
 *
 * © 2003, This code is provided "as is" and you can use it freely as long as 
 * credit is given to Bill Perone in the application it is used in
 ****************************************/


#pragma once


#if defined(WIN32) || defined(WIN64) // running on windows platform

	#if defined(_DEBUG) && !defined(WIN64) // if in debug mode use my assert message box 

// comment this line out if you don't want the assert text to automatically be
// copied to to the clipboard if the user presses Abort on the message box
//#define ASSERT_COPY_TO_CLIPBOARD
		#pragma message("Asserts enabled")

		#ifdef __cplusplus 
		extern "C" 
		#endif 
		char VCAssert(char expression, const char *exptext, const char *description, const char *fileline, int line, char *ignorealways);

		#define Assert(exp, description) \
		   {  static char ignoreAlways = 0; \
			  if (!ignoreAlways) \
				 if (VCAssert(exp, "Expression " #exp " failed\n", description, \
							  "Source file " __FILE__ " compiled on " __TIMESTAMP__ "\nLine ", __LINE__, &ignoreAlways))  \
				 { _asm { int 3 } } \
		   }

	#else // not in debug mode, don't compile asserts, {} done so that "else Assert(exp, desc);" works
		#pragma message("Asserts disabled")
		#define Assert(exp, description) {}
	#endif

#else // not running on windows
    #define Assert(exp, description)  assert((exp) && (description));

#endif

// useful utility for when you are dealing with STL file IO stuff
// this will print out filename: description in an assert
// filename is a std::string
#ifdef __cplusplus
#define AssertFileName(exp, filename, description) Assert(exp, (filename + ": " + description).c_str())
#endif


// override the normal assert (this only works if Assert.h is included before everything else)
#define assert(exp)  Assert(exp, "Unhandled exception")



