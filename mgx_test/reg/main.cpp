#include <stdio.h>

int add(int a,int b,int c)
{
    int d = a +b +c;
    return d;
}

int main(int argc,char **argv)
{
    int a,b,c,sun;
    a = 5;
    b = 7;
    c = 10;

    sun = add(a,b,c);
    return 0;
}