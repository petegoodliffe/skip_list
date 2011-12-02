//==============================================================================
// get_time.h
// Copyright (c) 2011 Pete Goodliffe. All rights reserved.
//==============================================================================

#pragma once

#if defined (WIN32) || defined (WIN64)
#define NOMINMAX
#include "windows.h"
#elif defined(__APPLE__)
#include <mach/mach_time.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

inline long get_time_ms()
{
#if defined (WIN32) || defined (WIN64)
    return GetTickCount();
#elif defined(__APPLE__)
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
#else
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000 + t.tv_usec/1000; 
#endif
}
