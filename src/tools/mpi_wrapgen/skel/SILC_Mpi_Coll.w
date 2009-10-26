${guard:start}
/**
 * Measurement wrapper for ${name}
 * @note Auto-generated by wrapgen from template: coll.w
 * @note C interface
 * @note Introduced with MPI-${mpi:version}
 * @ingroup ${group|lowercase}
 * It wraps the mpi call with an enter and exit event. Additionally, a collective
 * event is generated in between the enter and exit event after the PMPI call.
 */
${proto:c} 
{
  ${rtype} return_val;

  if (IS_EVENT_GEN_ON_FOR(${group|uppercase}))
    {
      ${decl}

      EVENT_GEN_OFF();
      SILC_EnterRegion(silc_mpi_regid[SILC__${name|uppercase}]);

      return_val = ${call:pmpi};

      ${xblock}
      SILC_MpiCollective(silc_mpi_regid[SILC__${name|uppercase}],
		       root_loc, SILC_COMM_ID(comm), 
                       ${mpi:sendcount}, 
                       ${mpi:recvcount});
      SILC_ExitRegion(silc_mpi_regid[SILC__${name|uppercase}]);
      EVENT_GEN_ON();
    }
  else
    {
      return_val = ${call:pmpi};
    }

  return return_val;
}
${guard:end}
