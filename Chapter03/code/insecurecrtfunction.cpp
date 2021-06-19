// ConsoleApplication13.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <time.h>

int main()
{
    time_t tNow = time(NULL);
    time_t tEnd = tNow + 1800;
    //注意下面两行的区别
    struct tm* ptm = localtime(&tNow);
    //struct tm* ptmEnd = localtime(&tEnd);

    char szTmp[50] = { 0 };
    strftime(szTmp, 50, "%H:%M:%S", ptm);

    struct tm* ptmEnd = localtime(&tEnd);
    char szEnd[50] = { 0 };
    strftime(szEnd, 50, "%H:%M:%S", ptmEnd);
    printf("%s \n", szTmp);
    printf("%s \n", szEnd);
    
    return 0;
}

