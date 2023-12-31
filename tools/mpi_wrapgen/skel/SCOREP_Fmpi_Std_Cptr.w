${guard:start}
/**
 * Measurement TYPE(C_PTR) overload wrapper for ${name}
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std_Cptr.w
 * @note Fortran interface
 * @note Introduced with MPI-${mpi:version}
 * @ingroup ${group|lowercase}
 */
${proto:fortran(_cptr)} {
  SCOREP_IN_MEASUREMENT_INCREMENT();
  ${decl:fortran}
  ${init:fortran}

  ${xblock:fortran}

  *ierr = ${call:fortran}

  ${cleanup:fortran}
  SCOREP_IN_MEASUREMENT_DECREMENT();
}
${guard:end}
