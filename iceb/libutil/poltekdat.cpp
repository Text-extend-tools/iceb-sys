/*$Id: poltekdat.c,v 5.5 2013/05/17 14:56:42 sasa Exp $*/
/*02.02.2015	16.07.2004	Белых А.И.	poltekdat.c
Получение текущего числа
*/
#include <time.h>
#include <stdio.h>
void poltekdat(short *dt,short *mt,short *gt)
{
time_t vrem;
time(&vrem);
struct tm *bf=localtime(&vrem);

*dt=bf->tm_mday;
*mt=bf->tm_mon+1;
*gt=bf->tm_year+1900;

}
/****************************/
const char *poltekdat()
{
static char str[64];
time_t vrem;
time(&vrem);
struct tm *bf=localtime(&vrem);
sprintf(str,"%02d.%02d.%04d",bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900);
return(str);

}