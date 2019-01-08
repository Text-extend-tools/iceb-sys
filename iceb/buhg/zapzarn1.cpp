/* $Id: zapzarn1.c,v 5.14 2013-04-07 10:22:45 sasa Exp $ */
/*09.11.2008	18.09.1998	Белых А.И.	zapzarn1.c
Запись структуры zar3 в базу
*/
#include        "buhg.h"


void		zapzarn1(long tnn, //Табельный номер
short tipn, //1-начисление(ден дов) 2- удержание(прин. в расч.)
short kd, //Код начисления
short dn,short ms,short gd, //Дата до которой действует начисление
const char *sh) //Счет
{
time_t		vrem;
char		strsql[512];

time(&vrem);
sprintf(strsql,"replace into Zarn1 \
values (%ld,%d,%d,'%04d-%02d-%02d','%s')",
tnn,tipn,kd,gd,ms,dn,sh);
/*
  printw("\nstrsql=%s\n",strsql);
  refresh();
  OSTANOV();
*/

if(sql_zap(&bd,strsql) != 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
 }

}
