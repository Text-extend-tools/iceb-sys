/*$Id: zarstfn53.c,v 5.18 2013/08/13 05:50:03 sasa Exp $*/
/*12.01.2018	17.03.2004	Белых А.И.	zarstfn53.c
Шапка типовой формы N53 - платежная ведомость
*/
#include <errno.h>
#include "buhg.h"


int  zarstfn53_2(const char *naipodr,const char *mesqc,short gr,const char *shet,const char *nomerved,int *kolstr,FILE *ff);


int  zarstfn53(const char *naipodr,short mr,short gr,
const char *shet,
const char *nomerved,
int *kolstr,
FILE *ff)
{
class iceb_tu_str mesqc("");

mesc(mr,0,&mesqc);
/************
if(sravmydat(30,mr,gr,16,6,2009) < 0)
  return(zarstfn53_0(naipodr,mesqc.ravno(),gr,shet,kolstr,ff));

if(sravmydat(30,mr,gr,16,6,2009) > 0 &&  sravmydat(31,mr,gr,5,1,2018) < 0)
  return(zarstfn53_1(naipodr,mesqc.ravno(),gr,shet,nomerved,kolstr,ff));
else
*************/
  return(zarstfn53_2(naipodr,mesqc.ravno(),gr,shet,nomerved,kolstr,ff));

return(1);
}
/*************************************/
/**************************************/
int  zarstfn53_2(const char *naipodr,const char *mesqc,short gr,
const char *shet,
const char *nomerved,
int *kolstr,
FILE *ff)
{
short dt=0,mt=0,gt=0;
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr1=0;
char strsql[512];
class iceb_tu_str stroka("");
class iceb_tu_str kod("");
SQLCURSOR cur;
SQL_str   row;

poltekdat(&dt,&mt,&gt);

//читаем код 
sprintf(strsql,"select kod from Kontragent where kodkon='00'");
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
  kod.new_plus(row[0]);



sprintf(strsql,"select str from Alx where fil='zarspv3.alx' order by ns asc");
if((kolstr1=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr1 == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zarspv3.alx");
  iceb_t_soob(strsql);
  return(1);
 }

int nomstr=0;
char bros[512];
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  nomstr++;
  stroka.new_plus(row_alx[0]);
  switch(nomstr)
   {
    case 5:
     iceb_tu_vstav(&stroka,iceb_t_get_pnk("00",0),0,76,1);
     break;

    case 8:
     iceb_tu_vstav(&stroka,kod.ravno(),65,76,1);
     break;

    case 9:
     iceb_tu_vstav(&stroka,naipodr,0,76,1);
     break;

    case 13:
     sprintf(bros,"%d %s",gt,"року");
     
     iceb_tu_vstav(&stroka,bros,21,34,1);
     break;

    case 17:
     iceb_tu_vstav(&stroka,shet,42,49,1);
     break;

    case 20:
     sprintf(bros,"%d %s",gt,"року");
     
     iceb_tu_vstav(&stroka,bros,14,22,1);
     break;

    case 22:
     iceb_tu_vstav(&stroka,nomerved,43,52,1);
     break;

    case 23:
     sprintf(bros,"%s %d %s",mesqc,gr,"року");
     
     iceb_tu_vstav(&stroka,bros,28,50,1);
     break;
        
   }
  *kolstr+=1;     
  fprintf(ff,"%s",stroka.ravno());
  
 }
 
return(0);

}
