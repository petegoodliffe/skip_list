//==============================================================================
// get_time.h
// Copyright (c) 2011 Pete Goodliffe. All rights reserved.
//==============================================================================

#pragma once

#if defined (WIN32) || defined (WIN64)
#define NOMINMAX
#include "windows.h"
#else
#include <mach/mach_time.h>
#endif

inline long get_time_ms()
{
#if defined (WIN32) || defined (WIN64)
    return GetTickCount();
#else
    //mach_timebase_info_data_t info;
    //kern_return_t err = mach_timebase_info( &info );
    static double conversion = 0.0;
    
    if( conversion == 0.0 )
    {
        mach_timebase_info_data_t info;
        kern_return_t err = mach_timebase_info( &info );
        
        //Convert the timebase into seconds
        if( err == 0  )
        {
            conversion = 1e-6 * (double) info.numer / (double) info.denom;
        }
    }
    return long(mach_absolute_time() * conversion);
#endif
}
