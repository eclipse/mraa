#include "stdio.h"

#include "maa.h"

int
main(int argc, char **argv)
{
    fprintf(stdout, "hello maa\n Version: %d\n", get_version());
    return 0;
}
