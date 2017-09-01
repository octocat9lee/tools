#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Function prototypes with attributes */
void traceBegin( void )
    __attribute__ ((no_instrument_function, constructor));

void traceEnd( void )
    __attribute__ ((no_instrument_function, destructor));

void __cyg_profile_func_enter( void *, void * )
    __attribute__ ((no_instrument_function));

void __cyg_profile_func_exit( void *, void * )
    __attribute__ ((no_instrument_function));

static FILE *fp_trace;

void __attribute__((constructor)) traceBegin(void)
{
    fp_trace = fopen("code_trace.out", "w");
}

void __attribute__((destructor)) traceEnd(void)
{
    if(fp_trace != NULL)
    {
        fclose(fp_trace);
    }
}

void __cyg_profile_func_enter(void *func, void *caller)
{
    if(fp_trace != NULL)
    {
        fprintf(fp_trace, "entry %p %p\n", func, caller);
    }
}

void __cyg_profile_func_exit(void *func, void *caller)
{
    if(fp_trace != NULL)
    {
        fprintf(fp_trace, "exit %p %p\n", func, caller);
    }
}

#ifdef __cplusplus
}
#endif

