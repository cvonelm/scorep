/** @file SILC_Fortran_Wrapper.h
    @author Daniel Lorenz

    This file provides macros to generate decoration for function names for C-functions
    called from Fortran. Furthermore, conversion types and constantsa are defined.
 */

/** @def SILC_FORTRAN_XSUFFIX
    Appends '_' to the name.
    @param name The name that gets decorated.
 */
#define SILC_FORTRAN_XSUFFIX( name ) name ## _

/** @def SILC_FORTRAN_XSUFFIX2
    Appends '__' to the name.
    @param name The name that gets decorated.
 */
#define SILC_FORTRAN_XSUFFIX2( name ) name ## __

/** @def SILC_FORTRAN_UPCASE
    Appends '_U' to the name. With generator tools definies can be generated to exchange
    the name_U by all upcase letters.
    @param name The name that gets decorated.
 */
#define SILC_FORTRAN_UPCASE( name ) name ## _U

/** @def SILC_FORTRAN_LOWCASE
    Appends '_L' to the name. With generator tools definies can be generated to exchange
    the name_L by all lowcase letters.
    @param name The name that gets decorated.
 */
#define SILC_FORTRAN_LOWCASE( name ) name ## _L

/** @def SILC_FORTRAN_SUFFIX
    Appends '_' to the name. For this SILC_FORTRAN_XSUFFIX is called to ensure the
    with nested calls, the inner name is substituted first.
    @param name The name that gets decorated.
 */
#define SILC_FORTRAN_SUFFIX( name ) SILC_FORTRAN_XSUFFIX( name )

/** @def SILC_FORTRAN_SUFFIX2
    Appends '__' to the name.  For this SILC_FORTRAN_XSUFFIX2 is called to ensure the
    with nested calls, the inner name is substituted first.
    @param name The name that gets decorated.
 */
#define SILC_FORTRAN_SUFFIX2( name ) SILC_FORTRAN_XSUFFIX2( name )

/** @def SILC_FORTRAN_SUB1(name)
    Defines the first possibility to decorate C-functions called from Fortran. It puts all
    letters in upcase.
    @param name The name that gets decorated.
 */
#define SILC_FORTRAN_SUB1( name )  SILC_FORTRAN_UPCASE( name )

/** @def SILC_FORTRAN_SUB2(name)
    Defines the first possibility to decorate C-functions called from Fortran. It puts all
    letters in lowcase.
    @param name The name that gets decorated.
 */
#define SILC_FORTRAN_SUB2( name )  SILC_FORTRAN_LOWCASE( name )

/** @def SILC_FORTRAN_SUB3(name)
    Defines the first possibility to decorate C-functions called from Fortran. It puts all
    letters in lowcase and appends '_'.
    @param name The name that gets decorated.
 */
#define SILC_FORTRAN_SUB3( name )  SILC_FORTRAN_SUFFIX( SILC_FORTRAN_LOWCASE( name ) )

/** @def SILC_FORTRAN_SUB4(name)
    Defines the first possibility to decorate C-functions called from Fortran. It puts all
    letters in lowcase and appends '__'.
    @param name The name that gets decorated.
 */
#define SILC_FORTRAN_SUB4( name )  SILC_FORTRAN_SUFFIX2( SILC_FORTRAN_LOWCASE( name ) )

/** Defines the type obtained from Fortran calls for region handles.
 */
typedef int SILC_Fortran_RegionHandle;

/** Defines the type obtained from Fortran calls for metric handles.
 */
typedef int SILC_Fortran_MetricHandle;

/** Defines the type obtained from Fortran calls for metric group handles.
 */
typedef int SILC_Fortran_MetricGroup;

/** @def SILC_FORTRAN_INVALID_REGION
    Value of an invalid region handle from fortran calls.
 */
#define SILC_FORTRAN_INVALID_REGION -1

/** @def SILC_FORTRAN_INVALID_METRIC
    Value of an invalid metric handle from fortran calls.
 */
#define SILC_FORTRAN_INVALID_METRIC -1

/** @def SILC_FORTRAN_INVALID_GROUP
    Value of an invalid metric group handle from fortran calls.
 */
#define SILC_FORTRAN_INVALID_GROUP  -1

/** @def SILC_FORTRAN_INVALID_GROUP
    Value of the metric group handle for the default group from fortran calls.
 */
#define SILC_FORTRAN_DEFAULT_GROUP  -2
