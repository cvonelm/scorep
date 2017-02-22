/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013, 2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

/**
 * @file src/tools/config/SCOREP_Config_LibraryDependecies.cpp
 *
 * Implements the representation and analysis of library dependencies
 */

#include <config.h>

#include <iostream>
#include <stdlib.h>
#include <cstring>

#include "SCOREP_Config_LibraryDependencies.hpp"
#include "scorep_config_utils.hpp"

using namespace std;

/* library_dependencies_inc.hpp are generated by
 * vendor/common/build-config/generate-library-dependency.sh
 */

#include <scorep_config_library_dependencies_backend_inc.hpp>
#if HAVE( MPI_SUPPORT )
#include <scorep_config_library_dependencies_mpi_backend_inc.hpp>
#endif /* MPI_SUPPORT */
#if HAVE( SHMEM_SUPPORT )
#include <scorep_config_library_dependencies_shmem_backend_inc.hpp>
#endif /* SHMEM_SUPPORT */


/* **************************************************************************************
                                                  class SCOREP_Config_LibraryDependencies
****************************************************************************************/

SCOREP_Config_LibraryDependencies::SCOREP_Config_LibraryDependencies( void )
{
    deque<string> libs;
    deque<string> ldflags;
    deque<string> rpaths;
    deque<string> dependency_las;

    add_library_dependencies_backend( libs, ldflags, rpaths, dependency_las, &m_la_objects );
    #if HAVE( MPI_SUPPORT )
    add_library_dependencies_mpi_backend( libs, ldflags, rpaths, dependency_las, &m_la_objects );
    #endif /* MPI_SUPPORT */
    #if HAVE( SHMEM_SUPPORT )
    add_library_dependencies_shmem_backend( libs, ldflags, rpaths, dependency_las, &m_la_objects );
    #endif /* SHMEM_SUPPORT */
}

SCOREP_Config_LibraryDependencies::~SCOREP_Config_LibraryDependencies()
{
}

deque<string>
SCOREP_Config_LibraryDependencies::getLibraries( const deque<string>& input_libs )
{
    /* Traversing backwards will add the -l flags from the scorep_* lib last.
       this makes the system more robust against broken dependencies in installed
       .la files of other libraries, in particular libbfd.
       During configure we tested whether linking works and this is more reliable
       than installed .la files. */
    deque<string>                   deps = get_dependencies( input_libs );
    deque<string>                   libs;
    deque<string>::reverse_iterator i;
    for ( i = deps.rbegin(); i != deps.rend(); i++ )
    {
        la_object obj = m_la_objects[ *i ];
        libs.push_front( "-l" + obj.m_lib_name.substr( 3 ) );
        libs.insert( libs.end(),
                     obj.m_libs.begin(),
                     obj.m_libs.end() );
    }

    return remove_double_entries( libs );
}

deque<string>
SCOREP_Config_LibraryDependencies::getLDFlags( const deque<string>& libs, bool install )
{
    deque<string>           deps = get_dependencies( libs );
    deque<string>           flags;
    deque<string>::iterator i;
    for ( i = deps.begin(); i != deps.end(); i++ )
    {
        la_object obj = m_la_objects[ *i ];
        if ( install )
        {
            flags.push_back( "-L" + obj.m_install_dir );
        }
        else
        {
            flags.push_back( "-L" + obj.m_build_dir + "/.libs" );
        }
        flags.insert( flags.end(),
                      obj.m_ldflags.begin(),
                      obj.m_ldflags.end() );
    }
    return remove_double_entries( flags );
}

deque<string>
SCOREP_Config_LibraryDependencies::getRpathFlags( const deque<string>& libs,
                                                  bool                 install )
{
    deque<string>           deps = get_dependencies( libs );
    deque<string>           flags;
    deque<string>::iterator i;
    deque<string>::iterator j;
    for ( i = deps.begin(); i != deps.end(); i++ )
    {
        la_object obj = m_la_objects[ *i ];
        if ( install )
        {
            flags.push_back( obj.m_install_dir );
        }
        else
        {
            flags.push_back( obj.m_build_dir + "/.libs" );
            // to support pre-installed components we need to add m_build_dir too.
            flags.push_back( obj.m_build_dir );
        }
        for ( j = obj.m_rpath.begin(); j != obj.m_rpath.end(); j++ )
        {
            string::size_type index = j->find( "-R" );
            if ( index == 0 )
            {
                j->replace( index, strlen( "-R" ), "" );
            }
            //flags.push_back( strip_head( *j, head, delimiter ) );
            flags.push_back( *j );
        }
    }
    return remove_double_entries( flags );
}

deque<string>
SCOREP_Config_LibraryDependencies::get_dependencies( const deque<string>& libs )
{
    deque<string> deps = libs;
    for ( int i = 0; i < deps.size(); i++ )
    {
        if ( m_la_objects.find( deps[ i ] ) == m_la_objects.end() )
        {
            cerr << "ERROR: Cannot resolve dependency '" << deps[ i ] << "'" << endl;
            exit( EXIT_FAILURE );
        }
        la_object obj = m_la_objects[ deps[ i ] ];

        deps.insert( deps.end(),
                     obj.m_dependency_las.begin(),
                     obj.m_dependency_las.end() );
    }
    return remove_double_entries( deps );
}

void
SCOREP_Config_LibraryDependencies::addDependency( const std::string& dependent_lib,
                                                  const std::string& dependency )
{
    if ( m_la_objects.find( dependent_lib ) == m_la_objects.end() )
    {
        cerr << "ERROR: Cannot add dependency to '" << dependent_lib << "'" << endl;
        exit( EXIT_FAILURE );
    }
    if ( m_la_objects.find( dependency ) == m_la_objects.end() )
    {
        cerr << "ERROR: Cannot add dependency '" << dependency << "'" << endl;
        exit( EXIT_FAILURE );
    }

    m_la_objects[ dependent_lib ].m_dependency_las.push_back( dependency );
}
