#include <stdio.h>

void foo(void)
{
    printf("foo\n");
}

void zoo(void)
{
    foo();
}

void bar(void)
{
    zoo();
}

int main(int argc, char **argv)
{
    bar();
    return 0;
}
