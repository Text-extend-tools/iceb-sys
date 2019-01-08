/*$Id: iceb_tu_cpbstr.c,v 5.4 2013/08/13 05:50:32 sasa Exp $*/
/*22.07.2013	04.06.2009	Белых А.И.	iceb_tu_cpbstr.c
Копировать одну строку в другую исключив из неё заданную цепочку символов 
*/
#include "util.h"

void iceb_tu_cpbstr(char *strin,int dlstrin,const char *strout,const char *udstr)
{
memset(strin,'\0',dlstrin);
int dlud=strlen(udstr);
int shstrout=0;
int dlstrout=strlen(strout);
for(int ii=0; ii < dlstrin && shstrout < dlstrout; ii++)
 {
  if(SRAV(&strout[shstrout],udstr,0) == 0)
   {
    shstrout+=dlud;
   }
  strin[ii]=strout[shstrout++];
  
 } 


}
/*******************/

void iceb_tu_cpbstr(class iceb_tu_str *strin,const char *strout,const char *udstr)
{
int dlstrin=strlen(strout)+1;
char strinb[dlstrin];

iceb_tu_cpbstr(strinb,dlstrin,strout,udstr);
strin->new_plus(strinb);
}
/*************************/
void iceb_tu_cpbstr(class SPISOK *strin,const char *strout,const char *udstr)
{
int dlstrin=strlen(strout)+1;
char strinb[dlstrin];

iceb_tu_cpbstr(strinb,dlstrin,strout,udstr);
strin->plus(strinb);
}

