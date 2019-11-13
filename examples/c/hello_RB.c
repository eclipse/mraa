#include<stdio.h>
#include"mraa.h"

int main()
{
fprintf(stdout,"Hello Rugged Board User!!!!\n You are using limraa version:%s\n On Board:%s\n",mraa_get_version(),mraa_get_platform_name());
mraa_deinit();
return MRAA_SUCCESS;
}
