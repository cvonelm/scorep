/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2014, 2016, 2020,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2015, 2017, 2022,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2017,
 * Technische Universitaet Darmstadt, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INSTRUMENTER_THREAD_HPP
#define SCOREP_INSTRUMENTER_THREAD_HPP

/**
 * @file
 *
 * Defines the threading paradigms.
 */

#include "scorep_instrumenter_selector.hpp"
#include <set>

/* **************************************************************************************
 * class SCOREP_Instrumenter_SingleThreaded
 * *************************************************************************************/

class SCOREP_Instrumenter_SingleThreaded : public SCOREP_Instrumenter_Paradigm
{
public:
    SCOREP_Instrumenter_SingleThreaded( SCOREP_Instrumenter_Selector* selector );
};

/* *****************************************************************************
 * class SCOREP_Instrumenter_Omp
 * ****************************************************************************/
class SCOREP_Instrumenter_Omp : public SCOREP_Instrumenter_Paradigm
{
public:
    SCOREP_Instrumenter_Omp( SCOREP_Instrumenter_Selector* selector );

    bool
    checkCommand( const std::string& current,
                  const std::string& next ) override;

    void
    checkDependencies( void ) override;

protected:
    std::set<std::string> m_openmp_flags; // provided by scorep_config_tool_backend.h

private:
    bool
    checkForOpenmpOption( const std::string& current );
};

/* *****************************************************************************
 * class SCOREP_Instrumenter_Pthread
 * ****************************************************************************/
class SCOREP_Instrumenter_Pthread : public SCOREP_Instrumenter_Paradigm
{
public:
    SCOREP_Instrumenter_Pthread( SCOREP_Instrumenter_Selector* selector );

    bool
    checkCommand( const std::string& current,
                  const std::string& next ) override;

private:
    std::string m_pthread_cflag;
    std::string m_pthread_lib;

    bool
    is_pthread_library( const std::string& libraryName );
};

/* **************************************************************************************
 * class SCOREP_Instrumenter_Thread
 * *************************************************************************************/
class SCOREP_Instrumenter_Thread : public SCOREP_Instrumenter_Selector
{
public:
    SCOREP_Instrumenter_Thread();
};

#endif // SCOREP_INSTRUMENTER_THREAD_HPP
