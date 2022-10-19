#include "test.h"

int main(int argc, char *const *argv){
    for (int i = 0 ; i<argc; ++i)	{
	    printf("argv[%d]: %s, addr: %lx\n", i, argv[i], (unsigned long)argv[i]);
	}

    for (int i = 0 ; environ[i]; ++i){
        printf("environ[%d]: %s, addr: %lx\n", i, environ[i], (unsigned long)environ[i]);
    }

    for (;;){
        printf("test env addr");
        sleep(1);
    }
    return 0;
}

