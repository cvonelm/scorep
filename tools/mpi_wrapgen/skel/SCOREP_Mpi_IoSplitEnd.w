${guard:start}
/**
 * Declaration of PMPI-symbol for ${name}
 */
${proto:c(P)};

/**
 * Measurement wrapper for ${name}
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_IoSplitEnd.w
 * @note C interface
 * @note Introduced with MPI-${mpi:version}
 * @ingroup ${group|lowercase}
 * Triggers an enter and exit event.
 * It wraps the ${name} call with enter and exit events.
 */
${proto:c}
{
  SCOREP_IN_MEASUREMENT_INCREMENT();
  const int event_gen_active = SCOREP_MPI_IS_EVENT_GEN_ON;
  const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR(SCOREP_MPI_ENABLED_${group|uppercase});
  SCOREP_IoHandleHandle io_handle = SCOREP_INVALID_IO_HANDLE;
  MPI_Status mystatus;
  ${rtype} return_val;

  if ( event_gen_active )
  {
    SCOREP_MPI_EVENT_GEN_OFF();
    if ( event_gen_active_for_group )
    {
      SCOREP_EnterWrappedRegion(scorep_mpi_regions[SCOREP_MPI_REGION__${name|uppercase}]);
      io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_MPI, &fh );
      if ( status == MPI_STATUS_IGNORE )
      {
          status = &mystatus;
      }
    }
    else if ( SCOREP_IsUnwindingEnabled() )
    {
      SCOREP_EnterWrapper(scorep_mpi_regions[SCOREP_MPI_REGION__${name|uppercase}]);
    }
  }

  SCOREP_ENTER_WRAPPED_REGION();
  return_val = ${call:pmpi};
  SCOREP_EXIT_WRAPPED_REGION();

  if ( event_gen_active )
  {
    if ( event_gen_active_for_group )
    {
      if( return_val == MPI_SUCCESS && io_handle != SCOREP_INVALID_IO_HANDLE )
      {
          int n_elements;
          SCOREP_MpiRequestId matching_id;
          MPI_Datatype datatype;

          scorep_mpi_io_split_end( io_handle, &matching_id, &datatype );

          const int type_size = mpi_io_get_type_size( datatype );
          PMPI_Get_count( status, datatype, &n_elements );
#if HAVE( MPI_2_0_SYMBOL_PMPI_TYPE_DUP )
          PMPI_Type_free( &datatype );
#endif

          SCOREP_IoOperationComplete( io_handle,
                                      SCOREP_IO_OPERATION_MODE_${attribute(operation_type)},
                                      ( uint64_t ) n_elements * type_size,
                                      matching_id );
      }

      SCOREP_IoMgmt_PopHandle( io_handle );

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
