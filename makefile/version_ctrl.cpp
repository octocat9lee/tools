#include <stdlib.h>
#include <stdio.h>

_VERSION_CODE_
_BUILD_TIME_
_VERSION_INFO_
_SVN_INFO_

const char INTERP[] __attribute__((section(".interp"))) = "/lib64/ld-linux-x86-64.so.2";

#ifdef __cplusplus
extern "C"
{
#endif

void lib_print_version()
{
    printf("%s\n", version_code);
    printf("%s\n", build_time);
    printf("%s\n", svn_info);
    printf("%s\n", version_info);
    exit(0);
}

#ifdef __cplusplus
}
#endif

