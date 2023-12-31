#ifndef _JACOBI_H
#define _JACOBI_H

struct JacobiData
{
    // input data
    int    iRows;
    int    iCols;
    int    iRowFirst;
    int    iRowLast;
    int    iIterMax;
    double fAlpha;
    double fRelax;
    double fTolerance;

    // calculated dx & dy
    double fDx;
    double fDy;

    // pointers to the allocated memory
    double* afU;
    double* afF;

    // start and end timestamps
    double fTimeStart;
    double fTimeStop;

    // calculated residual (output jacobi)
    double fResidual;
    // effective interation count (output jacobi)
    int    iIterCount;

    // calculated error (output error_check)
    double fError;

    // MPI-Variables
    int iMyRank;       // current process rank (number)
    int iNumProcs;     // how many processes

    // threadsnumbers in nested parallel regions
    int outer_threads;
    int inner_threads;
};

// jacobi calculation routine
void
Jacobi( JacobiData& data );

// final cleanup routines
void
Finish( JacobiData& data );

// routine to update the residual
// needed to have a function call
// in the parallel region for tests
void
update_residual( double& residual,
                 double  fLRes );

#endif
