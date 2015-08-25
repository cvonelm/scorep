/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2015,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INSTRUMENTER_SELECTOR_HPP
#define SCOREP_INSTRUMENTER_SELECTOR_HPP

/**
 * @file
 *
 * Defines the base class for selecting paradigm out of a group of mutual exclusive
 * paradigms.
 */

#include "scorep_instrumenter_paradigm.hpp"
#include <deque>

class SCOREP_Instrumenter;
class SCOREP_Instrumenter_Selector;

/* **************************************************************************************
 * Typedefs
 * *************************************************************************************/
/**
 * Type to list available paradigms.
 */
typedef std::deque<SCOREP_Instrumenter_Paradigm*> SCOREP_Instrumenter_ParadigmList;

/**
 * Type to list available selectors.
 */
typedef std::deque<SCOREP_Instrumenter_Selector*> SCOREP_Instrumenter_SelectorList;

/* **************************************************************************************
 * class SCOREP_Instrumenter_Selector
 * *************************************************************************************/
class SCOREP_Instrumenter_Selector
{
    /* ------------------------------------------------------------------------- types */
private:
    typedef enum
    {
        default_selection,
        auto_selection,
        user_selection
    } selection_priority_t;

    /* ----------------------------------------------------------------------- methods */
public:
    /**
     * Creates a SCOREP_Instrumenter_Selector instance.
     * @param name    The name of the selection group.
     */
    SCOREP_Instrumenter_Selector( std::string name );

    /**
     * Deletes a SCOREP_Instrumenter_Paradigm instance.
     */
    virtual
    ~SCOREP_Instrumenter_Selector();

    /**
     * Prints the default help output.
     */
    virtual void
    printHelp( void );

    /**
     * Returns the flag as used for the scorep-config tool.
     */
    virtual std::string
    getConfigToolFlag( SCOREP_Instrumenter_CmdLine& cmdLine );

    /**
     * Evaluates an scorep option for this paradigm.
     * Overwrite this function if you need to process different or more parameters.
     * @param arg  The option that is evaluated.
     * @returns true, if this argument was known to this adapter.
     */
    virtual bool
    checkOption( std::string arg );

    /**
     * This function is called during command line parsing during parsing of the
     * user command. The default implementation ignores all arguments.
     * Overwrite this function if the paradigm's behavior is influenced by options
     * of the compile/link command.
     * @param current The current argument under evaluation from the command line.
     * @param next    The next argument of the command line. Sometimes, the current
     *                argument indicates that the next argument contains some value
     *                that belongs together.
     * @returns This function should return true, if the next parameter and the
     *          current parameter belong together and the next parameter was already
     *          processed by this call. Thus, the iteration for the next parameter
     *          is skipped.
     */
    virtual bool
    checkCommand( const std::string& current,
                  const std::string& next );

    /**
     * This function is called during the command line parsing of the user
     * command. It decides whether options following the compiler name
     * belong to a wrapper, or to a build command.
     * @param current The current argument that is evaluated.
     * @param next    The next parameter.
     * @returns true if the current argument is a wrapper option.
     */
    virtual bool
    checkWrapperOption( const std::string& current,
                        const std::string& next );

    /**
     * Processes a key value pair from a config file. The default implementation
     * invokes the check for all paradigms of this group.
     * @param key   The key that is sepcified.
     * @param value The value of this key.
     */
    virtual void
    setConfigValue( const std::string& key,
                    const std::string& value );

    /**
     * Let all paradigms check the compiler name, which might affect the
     * automatic paradigm selection or paradigm configuration.
     * @param compiler  The name of the compiler as it appears in the user command.
     */
    virtual void
    checkCompilerName( const std::string& compiler );

    /**
     * Selections a pardigm from the group. Is is only possible to overwrite a
     * former selection if the current selection has a higher priority.
     * If the current selection has the same piority like the fromer, it
     * results in a conflict.
     * A selection of a user option has a higher priority than an automatic selection.
     * @param selection         Pointer to the selectes paradigm.
     * @param is_user_selection True if the current selection was made by a user.
     */
    virtual void
    select( SCOREP_Instrumenter_Paradigm* selection,
            bool                          is_user_selection );

    /**
     * Calls all paradigms to check for object based configurations.
     * @param instrumenter The instrumenter instance.
     */
    virtual void
    checkObjects( SCOREP_Instrumenter& instrumenter );

    /**
     * Returns a pointer to the currently selected paradigm.
     */
    virtual SCOREP_Instrumenter_Paradigm*
    getSelection( void );

    /* ----------------------------------------------------------------------- members */
protected:
    /**
     * Stores the name of the paradigm.
     */
    std::string m_name;

    /**
     * Specifies the priority of the current selection.
     */
    selection_priority_t m_current_priority;

    /**
     * The currently enabled paradigm.
     */
    SCOREP_Instrumenter_Paradigm* m_current_selection;

    /**
     * A List of known paradigms.
     */
    SCOREP_Instrumenter_ParadigmList m_paradigm_list;

    /* ---------------------------------------------------------------- static methods */
public:
    /**
     * Print the help information for all paradigms.
     */
    static void
    printAll( void );

    /**
     * Let all paradigms evaluate an argument form the command.
     * @param current The current argument that is evaluated.
     * @param next    The next parameter.
     * @returns true if the next parameter was already processed.
     */
    static bool
    checkAllCommand( const std::string& current,
                     const std::string& next );

    /**
     * Let the selected paradigm decide whether options
     * following the compiler name belong to a wrapper, or
     * to a build command.
     * @param current The current argument that is evaluated.
     * @param next    The next parameter.
     * @returns true if the current argument is a wrapper option.
     */
    static bool
    checkAllWrapperOption( const std::string& current,
                           const std::string& next );

    /**
     * Let all paradigms evaluate a scorep option.
     * @param arg  The scorep option that is evaluated.
     * @returns   true if one adapter knew this argument.
     */
    static bool
    checkAllOption( std::string arg );

    /**
     * Processes a key value pair from a config file for all selectors.
     * @param key   The key that is sepcified.
     * @param value The value of this key.
     */
    static void
    setAllConfigValue( const std::string& key,
                       const std::string& value );

    /**
     * Let all paradigm groups check the compiler name, which might affect the
     * automatic paradigm selection or paradigm configuration.
     * @param compiler  The name of the compiler as it appears in the user command.
     */
    static void
    checkAllCompilerName( const std::string& compiler );

    /**
     * Check the dependencies of the selected paradigm
     */
    static void
    checkDependencies( void );

    /**
     * Check the default relationships of the selected paradigm.
     */
    static void
    checkDefaults( void );

    /**
     * Does the selected paradigm make use of the instrumentation filter?
     */
    static bool
    supportInstrumentFilters( void );

    /**
     * Prints a list of selected paradigms.
     */
    static void
    printSelectedParadigms( void );

    /**
     * Returns the config tool flags for all selection groups. It has a leading space.
     */
    static std::string
    getAllConfigToolFlags( SCOREP_Instrumenter_CmdLine& cmdLine );

    /**
     * Returns whether a specific paradigm is selected.
     * @param name  The paradigm name as specified on the command line.
     */
    static bool
    isParadigmSelected( std::string name );

    /**
     * Checks whether all selected paradigms are supported.
     */
    static void
    checkAllSupported( void );

    /**
     * Let all paradigms check the object files.
     * @param instrumenter The instrumenter instance.
     */
    static void
    checkAllObjects( SCOREP_Instrumenter& instrumenter );

    /* ---------------------------------------------------------------- static members */
private:
    /**
     * A List of all selectors.
     */
    static SCOREP_Instrumenter_SelectorList m_selector_list;
};

#endif
