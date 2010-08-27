/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @file       silc_timer_intel_mmtimer.c
 * @author     Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 * @author     Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 * This is the implementation of the SILC_Timing.h interface that uses the
 * intel_mmtimer timer.
 */

#include <config.h>
#include "SILC_Timing.h"

#if HAVE( MMTIMER_H )
#include <mmtimer.h>
#elif HAVE( LINUX_MMTIMER_H )
#include <linux/mmtimer.h>
#elif HAVE( SN_MMTIMER_H )
#include <sn/mmtimer.h>
#else
#error "You need to include mmtimer.h"
#endif
#include <sys/mman.h>

#include <sys/ioctl.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "silc_utility/SILC_Utils.h"

#define MMTIMER_FULLNAME "/dev/mmtimer"

/**
   Indicates whether the timer is initialized.
 */
static bool isInitialized = false;

/**
   Stores the clock resolution.
 */
static unsigned long ticks_per_sec = 0;

/**
   Pointer to the clock counter storage.
 */
static volatile unsigned long* mmdev_timer_addr = NULL;

void
SILC_Timer_Initialize()
{
    if ( isInitialized )
    {
        return;
    }

    int           fd;
    unsigned long femtosecs_per_tick = 0;
    int           offset;

    if ( ( fd = open( MMTIMER_FULLNAME, O_RDONLY ) ) == -1 )
    {
        SILC_ERROR_POSIX( "Failed to open " MMTIMER_FULLNAME );
    }

    if ( ( offset = ioctl( fd, MMTIMER_GETOFFSET, 0 ) ) == -ENOSYS )
    {
        SILC_ERROR_POSIX( "Cannot get mmtimer offset" );
    }

    if ( ( mmdev_timer_addr = mmap( 0, sysconf( _SC_PAGESIZE ) /*getpagesize()*/, PROT_READ, MAP_SHARED, fd, 0 ) )
         == NULL )
    {
        SILC_ERROR_POSIX( "Cannot mmap mmtimer" );
    }
    mmdev_timer_addr += offset;

    ioctl( fd, MMTIMER_GETFREQ, &ticks_per_sec );

    close( fd );
    isInitialized = true;
}

uint64_t
SILC_GetClockTicks()
{
    return *mmdev_timer_addr;
}


uint64_t
SILC_GetClockResolution()
{
    return ticks_per_sec;
}
