#include "test.h"

int main(int argc, char *const *argv){
#if 1
    for (int i = 0 ; i<argc; ++i)	{
	    printf("argv[%d]: %s, addr: %lx\n", i, argv[i], (unsigned long)argv[i]);
	}

    for (int i = 0 ; environ[i]; ++i){
        printf("environ[%d]: %s, addr: %lx\n", i, environ[i], (unsigned long)environ[i]);
    }
    while(1) {
        sleep(100);
    }
#else
    printf("var a = %d\n",a);
#endif
    return 0;
}

