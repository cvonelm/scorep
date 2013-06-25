/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @file scorep_instrumenter_pdt.cpp
 * @status alpha
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * Implements the class for PDT instrumentation.
 */

#include <config.h>
#include "scorep_instrumenter_pdt.hpp"
#include "scorep_instrumenter_cmd_line.hpp"
#include "scorep_instrumenter_install_data.hpp"
#include "scorep_instrumenter_utils.hpp"
#include "scorep_instrumenter.hpp"
#include <scorep_config_tool_backend.h>
#include <scorep_config_tool_mpi.h>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <sstream>

#include <UTILS_Error.h>
#include <UTILS_IO.h>

/* **************************************************************************************
 * class SCOREP_Instrumenter_PdtAdapter
 * *************************************************************************************/
SCOREP_Instrumenter_PdtAdapter::SCOREP_Instrumenter_PdtAdapter( void )
    : SCOREP_Instrumenter_Adapter( SCOREP_INSTRUMENTER_ADAPTER_PDT, "pdt" )
{
    m_conflicts.push_back( SCOREP_INSTRUMENTER_ADAPTER_PREPROCESS );
    m_requires.push_back( SCOREP_INSTRUMENTER_ADAPTER_USER );
    m_default_off.push_back( SCOREP_INSTRUMENTER_ADAPTER_COBI );
    m_default_off.push_back( SCOREP_INSTRUMENTER_ADAPTER_COMPILER );

    m_pdt_bin_path    = PDT;
    m_pdt_config_file = PDT_CONFIG;
    m_use_params      = true;

#ifndef HAVE_PDT
    unsupported();
#endif
}

std::string
SCOREP_Instrumenter_PdtAdapter::getConfigToolFlag( void )
{
    return "";
}

std::string
SCOREP_Instrumenter_PdtAdapter::precompile( SCOREP_Instrumenter &         instrumenter,
                                            SCOREP_Instrumenter_CmdLine & cmdLine,
                                            const std::string &           source_file )
{
    std::string extension = get_extension( source_file );
    if ( is_fortran_file( source_file ) )
    {
        std::transform( extension.begin(), extension.end(), extension.begin(), ::toupper );
    }
    std::string modified_file = remove_path( remove_extension( source_file ) +
                                             "_pdt" + extension );
    std::string       pdb_file = remove_path( remove_extension( source_file ) + ".pdb" );
    std::stringstream command;
    std::string       cflags = "`" + instrumenter.getConfigBaseCall() + " --inc` "
                               + instrumenter.getCompilerFlags();

    // Create database file
    if ( is_c_file( source_file ) )
    {
        command << m_pdt_bin_path << "/cparse " << source_file;
    }
    else if ( is_fortran_file( source_file ) )
    {
        command << m_pdt_bin_path << "/gfparse " << source_file;
    }
    else
    {
        command << m_pdt_bin_path << "/cxxparse " << source_file;
    }
    command << " " << cmdLine.getDefineFlags()
            << " " << cmdLine.getIncludeFlags()
            << " " << cflags;
#ifdef _OPENMP
    if ( cmdLine.isOpenmpApplication() )
    {
        command << " -D_OPENMP=";
        command << _OPENMP;
    }
#endif

    if ( cmdLine.isMpiApplication() )
    {
        command << " -I" SCOREP_MPI_INCLUDE;
    }

    instrumenter.executeCommand( command.str() );

    // instrument source
    command.str( "" );
    command << m_pdt_bin_path << "/tau_instrumentor "
            << " " << pdb_file
            << " " << source_file
            << " -o " << modified_file
            << " -spec " << m_pdt_config_file
            << " " << m_params;

    if ( cmdLine.isMpiApplication() )
    {
        command << " -I" SCOREP_MPI_INCLUDE;
    }

    instrumenter.executeCommand( command.str() );

    instrumenter.addTempFile( modified_file );
    instrumenter.addTempFile( pdb_file );

    return modified_file;
}

void
SCOREP_Instrumenter_PdtAdapter::setBuildCheck( void )
{
    m_pdt_config_file = simplify_path( BUILD_PDT_CONFIG );
}

void
SCOREP_Instrumenter_PdtAdapter::setConfigValue( const std::string& key,
                                                const std::string& value )
{
    if ( key == "PDT_CONFIG" && value != "" )
    {
        m_pdt_config_file = value;
    }
    else if ( key == "PDT" && value != "" )
    {
        set_pdt_path( value );
    }
}

void
SCOREP_Instrumenter_PdtAdapter::set_pdt_path( const std::string& pdt )
{
    if ( pdt == "yes" )
    {
        char* path = UTILS_GetExecutablePath( "tau_instrumentor" );
        if ( path != NULL )
        {
            m_pdt_bin_path = path;
            free( path );
        }
        else
        {
            std::cout << "ERROR: Unable to find PDT binaries.\n";
            exit( EXIT_FAILURE );
        }
    }
    else if ( pdt == "no" )
    {
        return;
    }
    else
    {
        m_pdt_bin_path = pdt;
    }
}