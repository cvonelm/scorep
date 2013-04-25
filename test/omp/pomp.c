#ifdef __cplusplus
extern "C"
{
#endif
#include <stddef.h>
#include <stdio.h>

extern void
POMP2_Init_reg_4k6lsdctlab37_1();

void
POMP2_Init_regions()
{
    printf( "POMP2_Init_regions\n" );
    POMP2_Init_reg_4k6lsdctlab37_1();
}

size_t
POMP2_Get_num_regions()
{
    return 1;
}

const char*
POMP2_Get_opari2_version()
{
    return "1.0.7";
}

#ifdef __cplusplus
}
#endif
