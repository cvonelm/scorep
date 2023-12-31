#include <cmath>
#include <iostream>
#include <cstddef>
#include <cstdlib>
#include <cstdio>
#ifdef _OPENMP
#include <omp.h>
#else
#include <sys/time.h>
#endif
#include "jacobi.h"

#include <scorep/SCOREP_User.h>

#define U( j, i ) data.afU[ ( ( j ) - data.iRowFirst ) * data.iCols + ( i ) ]
#define F( j, i ) data.afF[ ( ( j ) - data.iRowFirst ) * data.iCols + ( i ) ]

using namespace std;

// setting values, init mpi, omp etc
void
Init( JacobiData& data, int& argc, char** argv )
{
    int   ITERATIONS = 1000;
    char* env        = getenv( "ITERATIONS" );
    if ( env )
    {
        int iterations = atoi( env );
        if ( iterations > 0 )
        {
            ITERATIONS = iterations;
        }
        else
        {
            printf( "Ignoring invalid ITERATIONS=%s!\n", env );
        }
    }

#if _OPENMP >= 201107
    omp_set_nested( true );
    data.outer_threads = omp_get_max_threads();
#pragma omp parallel
    {
#pragma omp master
        {
            data.inner_threads = omp_get_max_threads();
        }
    }
    printf( "Jacobi %d OpenMP-%u thread(s) used on the outer level and %d thread(s) on the inner one\n", data.outer_threads,  _OPENMP, data.inner_threads );
#elif defined( _OPENMP )
    omp_set_nested( true );
    data.outer_threads = sqrt( omp_get_max_threads() );
    data.inner_threads = omp_get_max_threads() / data.outer_threads;
    printf( "Jacobi %d OpenMP-%u thread(s) used on the outer level and %d thread(s) on the inner one\n", data.outer_threads,  _OPENMP, data.inner_threads );
#else
    printf( "Jacobi (serial)\n" );
#endif

// default medium
    data.iCols      = 2000;
    data.iRows      = 2000;
    data.fAlpha     = 0.8;
    data.fRelax     = 1.0;
    data.fTolerance = 1e-10;
    data.iIterMax   = ITERATIONS;
    cout << "\n-> matrix size: " << data.iCols << "x" << data.iRows
         << "\n-> alpha: " << data.fAlpha
         << "\n-> relax: " << data.fRelax
         << "\n-> tolerance: " << data.fTolerance
         << "\n-> iterations: " << data.iIterMax << endl << endl;


    // MPI values, set to defaults to avoid data inconsistency
    data.iMyRank   = 0;
    data.iNumProcs = 1;
    data.iRowFirst = 0;
    data.iRowLast  = data.iRows - 1;

    // memory allocation for serial & omp
    data.afU = new double[ data.iRows * data.iCols ];
    data.afF = new double[ data.iRows * data.iCols ];

    // calculate dx and dy
    data.fDx = 2.0 / ( data.iCols - 1 );
    data.fDy = 2.0 / ( data.iRows - 1 );

    data.iIterCount = 0;

    return;
}

// final cleanup routines
void
Finish( JacobiData& data )
{
    delete[] data.afU;
    delete[] data.afF;

    return;
}

//routine to update the residuals, this function call
//is needed for tests and it must be in this source
//file to avoid inlining
void
update_residual( double& residual, double fLRes )
{
    residual += fLRes * fLRes;
    return;
}

// print result summary
void
PrintResults( const JacobiData& data )
{
    if ( data.iMyRank == 0 )
    {
        cout << " Number of iterations : " << data.iIterCount << endl;
        cout << " Residual             : " << data.fResidual << endl;
        cout << " Solution Error       : " << data.fError << endl;
        cout << " Elapsed Time         : " << data.fTimeStop - data.fTimeStart << endl;
        cout << " MFlops               : " << 0.000013 * data.iIterCount
            * ( data.iCols - 2 ) * ( data.iRows - 2 )
            / ( data.fTimeStop - data.fTimeStart ) << endl;
    }
    return;
}

// Initializes matrix
// Assumes exact solution is u(x,y) = (1-x^2)*(1-y^2)
void
InitializeMatrix( JacobiData& data )
{
    /* Initialize initial condition and RHS */
#pragma omp parallel
#pragma omp for
    for ( int j = data.iRowFirst; j <= data.iRowLast; j++ )
    {
        for ( int i = 0; i < data.iCols; i++ )
        {
            // TODO: check if this values have to be ints or doubles
            int xx = ( int )( -1.0 + data.fDx * i );
            int yy = ( int )( -1.0 + data.fDy * j );

            int xx2 = xx * xx;
            int yy2 = yy * yy;

            U( j, i ) = 0.0;
            F( j, i ) = -data.fAlpha * ( 1.0 - xx2 ) * ( 1.0 - yy2 )
                        + 2.0 * ( -2.0 + xx2 + yy2 );
        }
    }
}

// Checks error between numerical and exact solution
void
CheckError( JacobiData& data )
{
    double error = 0.0;

    for ( int j = data.iRowFirst; j <= data.iRowLast; j++ )
    {
        if ( ( data.iMyRank != 0 && j == data.iRowFirst ) ||
             ( data.iMyRank != data.iNumProcs - 1 && j == data.iRowLast ) )
        {
            continue;
        }

        for ( int i = 0; i < data.iCols; i++ )
        {
            double xx   = -1.0 + data.fDx * i;
            double yy   = -1.0 + data.fDy * j;
            double temp = U( j, i ) - ( 1.0 - xx * xx ) * ( 1.0 - yy * yy );
            error += temp * temp;
        }
    }

    data.fError = sqrt( error ) / ( data.iCols * data.iRows );
}

double
get_wtime()
{
#ifdef _OPENMP
    return omp_get_wtime();
#else
    struct timeval tp;
    gettimeofday( &tp, 0 );
    return tp.tv_sec + ( tp.tv_usec * 1.0e-6 );
#endif
}


int
main( int argc, char** argv )
{
    SCOREP_USER_REGION( "main_user_instrumented", SCOREP_USER_REGION_TYPE_FUNCTION );
    int retVal = 0;    // return value

    JacobiData myData;


    // sets default values or reads from stdin
    // inits MPI and OpenMP if needed
    // distribute MPI data, calculate MPI bounds
    Init( myData, argc, argv );

    if ( myData.afU && myData.afF )
    {
        // matrix init
        InitializeMatrix( myData );

        // starting timer
        myData.fTimeStart = get_wtime();

        // running calculations
        Jacobi( myData );

        // stopping timer
        myData.fTimeStop = get_wtime();

        // error checking
        CheckError( myData );

        // print result summary
        PrintResults( myData );
    }
    else
    {
        cout << " Memory allocation failed ...\n";
        retVal = -1;
    }

    // cleanup
    Finish( myData );
    return retVal;
}
