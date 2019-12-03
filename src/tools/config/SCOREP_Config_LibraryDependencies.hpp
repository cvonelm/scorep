/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2017,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013, 2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_CONFIG_LIBRARY_DEPENDENCIES_HPP
#define SCOREP_CONFIG_LIBRARY_DEPENDENCIES_HPP

/**
 * @file
 *
 * Declares classes for the representation and analysis of library dependencies
 */

#include <map>
#include <deque>
#include <string>
#include <generate-library-dependencies-la-object.hpp>


/**
 * A class to store and analyze the dependencies from libraries. To generate
 * the dependencies, the .la files are parsed and initialization code is
 * generated by build-config/common/generate-library-dependencies.sh
 */
class SCOREP_Config_LibraryDependencies
{
    // ------------------------------------- Public functions
public:
    /**
     * Constructs the library dependencies
     */
    SCOREP_Config_LibraryDependencies( void );

    /**
     * Destructor
     */
    virtual
    ~SCOREP_Config_LibraryDependencies();

    /**
     * Inserts a new @a la_object into the @a m_la_objects database.
     *
     * Used by the library wrapper adapter to add la_objects for the wrapper
     * libraries.
     *
     * See @a la_object constructor for the parameters.
     */
    void
    insert( const std::string&             lib_name,
            const std::string&             build_dir,
            const std::string&             install_dir,
            const std::deque<std::string>& libs,
            const std::deque<std::string>& ldflags,
            const std::deque<std::string>& rpaths,
            const std::deque<std::string>& dependency_las );

    /**
     * Returns a list of libraries containing the @a inputLibs and its dependencies.
     * @param inputLibs   A list of libraries, that should be linked.
     * @param honorLibs   Includes libraries listed in @p inputLibs
     * @param honorDeps   Includes dependencies from libraries listed in @p
     *                    inputLibs
     */
    std::deque<std::string>
    getLibraries( const std::deque<std::string>& inputLibs,
                  bool                           honorLibs = true,
                  bool                           honorDeps = true );

    /**
     * Returns a list of library path flags for the @a inputLibs and
     * its dependencies.
     * @param libs    A list of library names.
     * @param install If true the install paths are used. If false the
     *                build path are used.
     */
    std::deque<std::string>
    getLDFlags( const std::deque<std::string>& libs,
                bool                           install );

    /**
     * Returns a list of paths in which the executable should look at runtime
     * to find the libraries. It can be used to construct the rpath flags for
     * the @a inputLibs and its dependencies.
     * @param libs       A list of library names.
     * @param install    If true the install paths are used. If false the
     *                   build path are used.
     * @param honorLibs  Includes libraries listed in @p libs
     * @param honorDeps  Includes dependencies from libraries listed in @p
     *                   libs
     */
    std::deque<std::string>
    getRpathFlags( const std::deque<std::string>& libs,
                   bool                           install,
                   bool                           honorLibs = true,
                   bool                           honorDeps = true );

    /**
     * This function adds a dependency to a library. For both libraries the .la
     * must be available. It is used to add the selected version of modules,
     * for which Score-P provides more than one implementation, e.g. threading
     * system, mutex.
     * @param dependentLib  The library which depends on @a dependency.
     * @param dependency    The library which is added to the dependencies of
     *                      @a dependentLib.
     */
    void
    addDependency( const std::string& dependentLib,
                   const std::string& dependency );

    // ------------------------------------- Protected functions
protected:
    /**
     * Calculates the dependencies for a given set of libraries.
     * @param libs  A list of library names.
     */
    std::deque<std::string>
    get_dependencies( const std::deque<std::string>& libs,
                      bool                           honorLibs = true,
                      bool                           honorDeps = true );

    // ------------------------------------- Public members
private:
    std::map< std::string, la_object> m_la_objects;
};

#endif // SCOREP_CONFIG_LIBRARY_DEPENDENCIES_HPP
