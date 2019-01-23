/****************************************
 * Extended Assert Utility
 * By Bill Perone (billperone@yahoo.com)
 * Original: 8-17-2002
 * Revised:  8-21-2002
 *           8-22-2002
 *           1-08-2004
 *
 * Usage:  Assert(expression, descrition)
 *
 * Notes:  For best functionality make sure that this file is included before all other headers
 *
 * Special Thanks: Steve Rabin, John Robbins
 *
 * This code is provided "as is" and you can use it freely as long as 
 * credit is given to Bill Perone in the application it is used in
 ****************************************/


#include "assert.h"


#ifdef WIN32 // running on windows platform

#include <windows.h>
#include <stdio.h>


#define BUFSIZE 80

// obtains the OS platform and version from windows and puts it in output string
BOOL OSVersion(char *output)
{
	OSVERSIONINFOEX osvi;
    BOOL bOsVersionInfoEx;
	int  n= 0;

    // Try calling GetVersionEx using the OSVERSIONINFOEX structure.
    // If that fails, try using the OSVERSIONINFO structure.
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    if (!(bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO *) &osvi)))
    {
       osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
       if (!GetVersionEx((OSVERSIONINFO *) &osvi)) 
           return FALSE;
    }

   switch (osvi.dwPlatformId)
   {
      // Test for the Windows NT product family.
      case VER_PLATFORM_WIN32_NT:

         // Test for the specific product family.
         if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
            n+= sprintf(output+n, "Microsoft Windows Server&nbsp;2003 family, ");

         if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
            n+= sprintf(output+n, "Microsoft Windows XP ");

         if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
            n+= sprintf(output+n, "Microsoft Windows 2000 ");

         if ( osvi.dwMajorVersion <= 4 )
            n+= sprintf(output+n, "Microsoft Windows NT ");

         // Test for specific product on Windows NT 4.0 SP6 and later.
         if (bOsVersionInfoEx)
         {
            // Test for the workstation type.
            /*if ( osvi.wProductType == VER_NT_WORKSTATION )
            {
               if( osvi.dwMajorVersion == 4 )
                  n+= sprintf(output+n,  "Workstation 4.0 " );
               else if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
                  n+= sprintf(output+n,  "Home Edition " );
               else
                  n+= sprintf(output+n,  "Professional " );
            }
            
            // Test for the server type.
            else 
			if ( osvi.wProductType == VER_NT_SERVER )
            {
               if( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
               {
                  if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                     n+= sprintf(output+n,  "Datacenter Edition " );
                  else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                     n+= sprintf(output+n,  "Enterprise Edition " );
                  else if ( osvi.wSuiteMask == VER_SUITE_BLADE )
                     n+= sprintf(output+n,  "Web Edition " );
                  else
                     n+= sprintf(output+n,  "Standard Edition " );
               } else 
			   if( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
               {
                  if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                     n+= sprintf(output+n,  "Datacenter Server " );
                  else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                     n+= sprintf(output+n,  "Advanced Server " );
                  else
                     n+= sprintf(output+n,  "Server " );
               } else  // Windows NT 4.0 
               {
                  if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                     n+= sprintf(output+n, "Server 4.0, Enterprise Edition " );
                  else
                     n+= sprintf(output+n,  "Server 4.0 " );
               }
            }*/
         }
         else  // Test for specific product on Windows NT 4.0 SP5 and earlier
         {
            HKEY hKey;
            char szProductType[BUFSIZE];
            DWORD dwBufLen=BUFSIZE;
            LONG lRet;

            lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                                "SYSTEM\\CurrentControlSet\\Control\\ProductOptions",
                                0, KEY_QUERY_VALUE, &hKey );
            if (lRet != ERROR_SUCCESS)
               return FALSE;

            lRet = RegQueryValueEx(hKey, "ProductType", NULL, NULL,
                                  (LPBYTE) szProductType, &dwBufLen);
            if ((lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE))
               return FALSE;

            RegCloseKey( hKey );

            if ( lstrcmpi( "WINNT", szProductType) == 0 )
               n+= sprintf(output+n, "Workstation " );
            if ( lstrcmpi( "LANMANNT", szProductType) == 0 )
               n+= sprintf(output+n, "Server " );
            if ( lstrcmpi( "SERVERNT", szProductType) == 0 )
               n+= sprintf(output+n, "Advanced Server " );

            n+= sprintf(output+n, "%d.%d ", osvi.dwMajorVersion, osvi.dwMinorVersion );
         }

         // Display service pack (if any) and build number.
         if( osvi.dwMajorVersion == 4 && 
             lstrcmpi( osvi.szCSDVersion, "Service Pack 6" ) == 0 )
         {
            HKEY hKey;
            LONG lRet;

            // Test for SP6 versus SP6a.
            lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                                "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Hotfix\\Q246009",
                                0, KEY_QUERY_VALUE, &hKey );
            if( lRet == ERROR_SUCCESS )
               n+= sprintf(output+n, "Service Pack 6a (Build %d)\n", osvi.dwBuildNumber & 0xFFFF );         
            else // Windows NT 4.0 prior to SP6a
            {
               n+= sprintf(output+n, "%s (Build %d)\n", osvi.szCSDVersion, osvi.dwBuildNumber & 0xFFFF);
            }

            RegCloseKey( hKey );
         }
         else // Windows NT 3.51 and earlier or Windows 2000 and later
         {
            n+= sprintf(output+n, "%s (Build %d)\n", osvi.szCSDVersion, osvi.dwBuildNumber & 0xFFFF);
         }

         break;

      // Test for the Windows 95 product family.
      case VER_PLATFORM_WIN32_WINDOWS:

         if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
         {
             n+= sprintf(output+n, "Microsoft Windows 95 ");
             if ( osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B' )
                n+= sprintf(output+n, "OSR2 " );
         } 

         if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
         {
             n+= sprintf(output+n, "Microsoft Windows 98 ");
             if ( osvi.szCSDVersion[1] == 'A' )
                n+= sprintf(output+n, "SE " );
         } 

         if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
         {
             n+= sprintf(output+n, "Microsoft Windows Millennium Edition\n");
         } 
         break;

      case VER_PLATFORM_WIN32s:

         n+= sprintf(output+n, "Microsoft Win32s\n");
         break;
   }
   return TRUE;
}


char VCAssert(char expression, const char *exptext, const char *description, const char *fileline, int linenum, char *ignorealways)
{	
	if (!expression)
	{
		char  errmsg[1024];
		char  OSPlatform[80];
		DWORD UserNameSize= 64;
		DWORD MinProcessWorkSize,
			  MaxProcessWorkSize;
		char  UserName[64];		
		int   retval;		


		//GetUserObjectInformation
		GetUserName(UserName, &UserNameSize);
		// obtain OS version information
		OSVersion(OSPlatform);		
		// get the memory available to the process
		GetProcessWorkingSetSize(GetCurrentProcess(), &MinProcessWorkSize, &MaxProcessWorkSize);

		// format the message
		_snprintf(errmsg, 1024, 
			      "%s%s\n%s%i\n\n%s is running %s\nResolution %ix%i\nProcess Work Range %i - %i bytes", 
		          exptext, description, fileline, linenum, UserName, OSPlatform, 
				  GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
				  MinProcessWorkSize, MaxProcessWorkSize);
				
		if (GetSystemMetrics(SM_SLOWMACHINE))
			strcat(errmsg, "\nSlow machine suspected");
		
		retval= MessageBox(NULL, errmsg, "Debug Assertion Failed", 
		                   MB_ABORTRETRYIGNORE|MB_ICONEXCLAMATION|MB_SYSTEMMODAL|MB_SETFOREGROUND|MB_TOPMOST);

		if (retval==IDIGNORE) 
		{
			*ignorealways= 1;
			return 0;
		} 
		if (retval==IDRETRY) return 0;

#ifdef ASSERT_COPY_TO_CLIPBOARD
		// copy it to the clipboard if they hit Abort
		if (OpenClipboard(NULL))
		{
		   HGLOBAL hMem;		   
		   char   *pMem;
		   
		   hMem= GlobalAlloc(GHND|GMEM_DDESHARE, strlen(errmsg)+1);

		   if (hMem) 
		   {
			  pMem= (char *)GlobalLock(hMem);
			  strcpy(pMem, errmsg);
			  GlobalUnlock(hMem);
			  EmptyClipboard();
			  SetClipboardData(CF_TEXT, hMem);
		   }
		   CloseClipboard();
		}
#endif

		return 1;
	} 

	return 0;
}


#endif