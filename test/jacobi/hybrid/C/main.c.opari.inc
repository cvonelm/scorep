#include <opari2/pomp2_lib.h>

#include <stdint.h>
extern int64_t __attribute__((aligned (16))) F77_FUNC_(pomp_tpd,POMP_TPD);
#pragma omp threadprivate(F77_FUNC_(pomp_tpd,POMP_TPD))
#define POMP2_DLIST_00001 shared(pomp2_region_1)
static POMP2_Region_handle pomp2_region_1 = NULL;
#define pomp2_ctc_1 "242*regionType=parallelfor*sscl=main.c:195:195*escl=main.c:210:210**"

#ifdef __cplusplus
 extern "C"
#endif
void POMP2_Init_reg_mg6lu98tlexp_1()
{
    POMP2_Assign_handle( &pomp2_region_1, pomp2_ctc_1 );
}
