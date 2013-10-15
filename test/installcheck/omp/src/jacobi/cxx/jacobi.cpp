/*
 ******************************************************************
 * Subroutine HelmholtzJ
 * Solves poisson equation on rectangular grid assuming :
 * (1) Uniform discretization in each direction, and
 * (2) Dirichlect boundary conditions
 *
 * Jacobi method is used in this routine
 *
 * Input : n,m   Number of grid points in the X/Y directions
 *         dx,dy Grid spacing in the X/Y directions
 *         alpha Helmholtz eqn. coefficient
 *         omega Relaxation factor
 *         f(n,m) Right hand side function
 *         u(n,m) Dependent variable/Solution
 *         tolerance Tolerance for iterative solver
 *         maxit  Maximum number of iterations
 *
 * Output : u(n,m) - Solution
 *****************************************************************
 */
#include <cmath>
#include <cstdlib>
#include <iostream>
#include "jacobi.h"

#include <scorep/SCOREP_User.h>

using namespace std;

#define U( j, i ) afU[ ( ( j ) - data.iRowFirst ) * data.iCols + ( i ) ]
#define F( j, i ) afF[ ( ( j ) - data.iRowFirst ) * data.iCols + ( i ) ]
#define UOLD( j, i ) uold[ ( ( j ) - data.iRowFirst ) * data.iCols + ( i ) ]

SCOREP_USER_GLOBAL_REGION_EXTERNAL( scorep_region2 );
SCOREP_USER_METRIC_EXTERNAL( scorep_globalMetric );

void
Jacobi( JacobiData& data )
{
    SCOREP_USER_FUNC_BEGIN();
    /*use local pointers for performance reasons*/
    double* afU, * afF;
    afU = data.afU;
    afF = data.afF;

    double* uold = new double[ data.iCols * data.iRows ];

    SCOREP_USER_REGION_DEFINE( scorep_iteration );
    SCOREP_USER_METRIC_LOCAL( scorep_localMetric );
    SCOREP_USER_METRIC_INIT( scorep_localMetric, "LOCAL_METRIC_PARALLEL", "s", SCOREP_USER_METRIC_TYPE_DOUBLE,
                             SCOREP_USER_METRIC_CONTEXT_GLOBAL );
    SCOREP_USER_METRIC_INT64( scorep_globalMetric, 1 );

    if ( uold )
    {
        double ax       = 1.0 / ( data.fDx * data.fDx );    /* X-direction coef */
        double ay       = 1.0 / ( data.fDy * data.fDy );    /* Y_direction coef */
        double b        = -2.0 * ( ax + ay ) - data.fAlpha; /* Central coeff */
        double residual = 10.0 * data.fTolerance;

        SCOREP_USER_REGION_BEGIN( scorep_region2, "PHASE",
                                  SCOREP_USER_REGION_TYPE_PHASE );

        while ( data.iIterCount < data.iIterMax&& residual > data.fTolerance )
        {
            SCOREP_USER_REGION_BEGIN( scorep_iteration, "ITERATION",
                                      SCOREP_USER_REGION_TYPE_DYNAMIC );

            residual = 0.0;

            /* copy new solution into old */
#pragma omp parallel
            {
                SCOREP_USER_REGION_DEFINE( scorep_region1 );
                SCOREP_USER_REGION_BEGIN( scorep_region1, "REGION1",
                                          SCOREP_USER_REGION_TYPE_COMMON );
                SCOREP_USER_PARAMETER_INT64( "INT_PARAM", -1 );
                SCOREP_USER_PARAMETER_UINT64( "UINT_PARAM", 3 );
                SCOREP_USER_PARAMETER_STRING( "STRING_PARAM", "test" );
                SCOREP_USER_METRIC_DOUBLE( scorep_localMetric, 3.0 );

#pragma omp for
                for ( int j = 1; j < data.iRows - 1; j++ )
                {
                    for ( int i = 1; i < data.iCols - 1; i++ )
                    {
                        UOLD( j, i ) = U( j, i );
                    }
                }

                SCOREP_USER_REGION_END( scorep_region1 );

                double fLRes;

                /* compute stencil, residual and update */
#pragma omp for reduction(+:residual)
                for ( int j = data.iRowFirst + 1; j <= data.iRowLast - 1; j++ )
                {
                    for ( int i = 1; i < data.iCols - 1; i++ )
                    {
                        fLRes = ( ax * ( UOLD( j, i - 1 ) + UOLD( j, i + 1 ) )
                                  + ay * ( UOLD( j - 1, i ) + UOLD( j + 1, i ) )
                                  +  b * UOLD( j, i ) - F( j, i ) ) / b;

                        /* update solution */
                        U( j, i ) = UOLD( j, i ) - data.fRelax * fLRes;

                        /* accumulate residual error */
                        residual += fLRes * fLRes;
                    }
                }
            } /* end omp parallel */

            /* error check */
            data.iIterCount++;
            residual = sqrt( residual ) / ( data.iCols * data.iRows );

            SCOREP_USER_REGION_END( scorep_iteration );
        } /* while */

        SCOREP_USER_REGION_END( scorep_region2 );

        data.fResidual = residual;
        delete[] uold;
    }
    else
    {
        cerr << "Error: cant allocate memory\n";
        Finish( data );
        exit( 1 );
    }
    SCOREP_USER_FUNC_END();
}
