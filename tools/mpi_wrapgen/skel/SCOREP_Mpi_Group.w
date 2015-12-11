${guard:start}
/**
 * Measurement wrapper for ${name}
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Group.w
 * @note C interface
 * @note Introduced with MPI-${mpi:version}
 * @ingroup ${group|lowercase}
 * Triggers an enter event and an exit event. Furthermore, a communicator definition
 * event is called between enter and exit.
 */
${proto:c}
{
  SCOREP_IN_MEASUREMENT_INCREMENT();
  const int event_gen_active = SCOREP_MPI_IS_EVENT_GEN_ON_FOR(SCOREP_MPI_ENABLED_${group|uppercase});
  ${rtype} return_val;

  if (event_gen_active)
    {
      SCOREP_MPI_EVENT_GEN_OFF();
      SCOREP_EnterWrappedRegion(scorep_mpi_regions[SCOREP_MPI_REGION__${name|uppercase}],
                                ( intptr_t )P${name});
    }

  if (event_gen_active)
  {
    SCOREP_ENTER_WRAPPED_REGION();
  }
  return_val = ${call:pmpi};
  if (event_gen_active)
  {
    SCOREP_EXIT_WRAPPED_REGION();
  }
  if (*group != MPI_GROUP_NULL)
    scorep_mpi_group_create(*group);

  if (event_gen_active)
    {
      SCOREP_ExitRegion(scorep_mpi_regions[SCOREP_MPI_REGION__${name|uppercase}]);
      SCOREP_MPI_EVENT_GEN_ON();
    }
  SCOREP_IN_MEASUREMENT_DECREMENT();

  return return_val;
}
${guard:end}
