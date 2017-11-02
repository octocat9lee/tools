#include "static_func.h"
#include "shared_func.h"
#include <stdio.h>

int main()
{
    staticPrint();
    sharedPrint();
    printf("Hello World!\r\n");
    return 0;
}

