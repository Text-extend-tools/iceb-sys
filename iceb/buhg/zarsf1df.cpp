/*$Id: zarsf1df.c,v 5.4 2013/05/17 14:56:18 sasa Exp $*/
/*02.10.2014	24.03.2011	Белых А.И.	zarsf1df.c
Печать концовки формы 1ДФ
*/

#include "buhg.h"

void zarsf1df(int kolih_str,int kolih_fiz_os,int kolih_list,int TYP,FILE *ff1)
{
int nomstr=0;

class iceb_tu_str stroka("");
const char *imaf_alx={"zarsf1df_1e.alx"};

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_t_soob(strsql);
  return;
 }
class iceb_t_fioruk_rk rukov;
class iceb_t_fioruk_rk glavbuh;

iceb_t_fioruk(1,&rukov);
iceb_t_fioruk(2,&glavbuh);

short dt=0,mt=0,gt=0;
poltekdat(&dt,&mt,&gt);
char data_pod[64];
sprintf(data_pod,"%02d.%02d.%04d",dt,mt,gt);
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  nomstr++;
  stroka.new_plus(row_alx[0]);
  switch(nomstr)
   {
    case 2:
     iceb_tu_vstav(&stroka,kolih_str,19,27,1);
     iceb_tu_vstav(&stroka,kolih_fiz_os,53,62,1);
     iceb_tu_vstav(&stroka,kolih_list,80,89,1);
     iceb_tu_vstav(&stroka,data_pod,101,122,1);
     break;

    case 5: /*Руководитель*/
     if(TYP == 0)
      {
       iceb_tu_vstav(&stroka,rukov.inn.ravno(),21,321,1);
       iceb_tu_vstav(&stroka,rukov.famil_inic.ravno(),59,86,1);
       iceb_tu_vstav(&stroka,rukov.telef.ravno(),89,99,1);
      }
     break;    

    case 10: /*главный бухгалтер*/
     iceb_tu_vstav(&stroka,glavbuh.inn.ravno(),21,321,1);
     iceb_tu_vstav(&stroka,glavbuh.famil_inic.ravno(),59,86,1);
     iceb_tu_vstav(&stroka,glavbuh.telef.ravno(),89,99,1);
     break;    

    case 15: /*самозанятая особа*/
     if(TYP == 1)
      {
       iceb_tu_vstav(&stroka,rukov.inn.ravno(),21,321,1);
       iceb_tu_vstav(&stroka,rukov.famil_inic.ravno(),59,86,1);
       iceb_tu_vstav(&stroka,rukov.telef.ravno(),89,99,1);
      }
     break;    

   }
  fprintf(ff1,"%s",stroka.ravno());  
 
 }
 


}
