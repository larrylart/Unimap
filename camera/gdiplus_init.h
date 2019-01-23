
#ifndef _GDI_PLUS_INIT_H
#define _GDI_PLUS_INIT_H

// system headers
#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>
#include <objbase.h>
#include <atlbase.h>
#include <sti.h>
#include <gdiplus.h>

// class:	CGdiplusInit
////////////////////////////////////////////////////////////////////
class CGdiplusInit : public Gdiplus::GdiplusStartupOutput
{
public:
    CGdiplusInit(
        Gdiplus::DebugEventProc debugEventCallback       = 0,
        BOOL                    suppressBackgroundThread = FALSE,
        BOOL                    suppressExternalCodecs   = FALSE
    )
    {
        Gdiplus::GdiplusStartupInput StartupInput(
            debugEventCallback,
            suppressBackgroundThread,
            suppressExternalCodecs
        );

        StartupStatus = Gdiplus::GdiplusStartup(
            &Token, 
            &StartupInput, 
            this
        );
    }

    ~CGdiplusInit()
    {
        if (StartupStatus == Gdiplus::Ok)
        {
            Gdiplus::GdiplusShutdown(Token);
        }
    }

private:
    Gdiplus::Status StartupStatus;
    ULONG_PTR       Token;
};

#endif
