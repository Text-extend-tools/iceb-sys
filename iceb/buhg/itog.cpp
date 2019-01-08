/*$Id: itog.c,v 5.19 2013-01-15 10:17:56 sasa Exp $*/
/*11.01.2018	18.02.2003	Белых А.И.	itod.c
Вывод итога в ведомости на выдачу зарплаты
*/

#include        "buhg.h"


void itog(FILE *ff,double itogo)
{
class iceb_tu_str stroka("");
prnb(itogo,&stroka);
fprintf(ff,"\
------------------------------------------------------------------------------\n\
%*s:%15s\n",iceb_tu_kolbait(43,gettext("Итого")),gettext("Итого"),stroka.ravno());

class iceb_t_fioruk_rk fio_prov;
iceb_t_fioruk(4,&fio_prov);

fprintf(ff,"\n\
            Відомість склав___________________%s\n\
                      (підпис, призвище, ініціали)\n\
\n\
            Відомість перевірив_______________%s\n\
                      (підпис, призвище, ініціали)\n",
iceb_t_getfioop(),
fio_prov.famil_inic.ravno());




/***************************
int kolstr=0;
SQL_str row_alx;
class SQLCURSOR cur_alx;
char strsql[512];
sprintf(strsql,"select str from Alx where fil='zarspv1.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zarspv1.alx");
  iceb_t_soob(strsql);
  return;
 }


int nomstr=0;

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  nomstr++;
  stroka.new_plus(row_alx[0]);
  switch(nomstr)
   {
    case 2:
     iceb_tu_vstav(&stroka,iceb_t_getfioop(),47,70,1);
     break;
    case 5:
     iceb_tu_vstav(&stroka,fio_prov.famil_inic.ravno(),47,70,1);
     break;
   }
  fprintf(ff,"%s",stroka.ravno());
  
 }
**************************/ 



podpis(ff);
}
