
#if HAVE( ${name|uppercase}_COMPLIANT )
${guard:start}
/**
 * Declaration of PMPI-symbol for ${name}
 */
${proto:c(P)};

/**
 * Measurement wrapper for ${name}
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_StdCompl.w
 * @note C interface
 * @note Introduced with MPI-${mpi:version}
 * @ingroup ${group|lowercase}
 * Triggers an enter and exit event.
 * It wraps the ${name} call with enter and exit events.
 */
${proto:c}
{
  SCOREP_IN_MEASUREMENT_INCREMENT();
  const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
  const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR(SCOREP_MPI_ENABLED_${group|uppercase});
  ${rtype} return_val;
  ${decl}

  if (event_gen_active)
  {
    ${init}

    SCOREP_MPI_EVENT_GEN_OFF();
    if (event_gen_active_for_group)
    {
      SCOREP_EnterWrappedRegion(scorep_mpi_regions[SCOREP_MPI_REGION__${name|uppercase}]);

      ${xblock}
    }
    else if ( SCOREP_IsUnwindingEnabled() )
    {
      SCOREP_EnterWrapper(scorep_mpi_regions[SCOREP_MPI_REGION__${name|uppercase}]);
    }
  }

  SCOREP_ENTER_WRAPPED_REGION();
  return_val = ${call:pmpi};
  SCOREP_EXIT_WRAPPED_REGION();

  if (event_gen_active)
  {
    if (event_gen_active_for_group)
    {
      SCOREP_ExitRegion(scorep_mpi_regions[SCOREP_MPI_REGION__${name|uppercase}]);
    }
    else if ( SCOREP_IsUnwindingEnabled() )
    {
      SCOREP_ExitWrapper(scorep_mpi_regions[SCOREP_MPI_REGION__${name|uppercase}]);
    }
    SCOREP_MPI_EVENT_GEN_ON();
  }

  SCOREP_IN_MEASUREMENT_DECREMENT();
  return return_val;
}
${guard:end}
#endif
