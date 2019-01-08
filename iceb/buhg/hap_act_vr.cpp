/*$Id: hap_act_vr.c,v 5.11 2013/05/17 14:56:02 sasa Exp $*/
/*14.07.2015	19.04.2006	Белых А.И.	hap_act_vr.c
Распечатка шапки акта выполненых работ
*/
#include        <errno.h>
#include	"buhg.h"


int hap_act_vr(short dd,short md,short gd,
const char *nomdok,
const char *naim_kontr,
const char *dover,
const char *ispolnitel,
FILE *ff)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[1024];

class iceb_tu_str bros("");
int kol_str_ots=0;
//читаем количество сторок отступа для логотипа
if(iceb_t_poldan("Количество строк отступа для счёта с логотипом",&bros,"uslnast.alx") == 0)
  kol_str_ots=bros.ravno_atoi();

for(int i=0; i < kol_str_ots; i++)
 fprintf(ff,"\n");

sprintf(strsql,"select str from Alx where fil='usl_act_vr.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"usl_act_vr.alx");
  iceb_t_soob(strsql);
  return(1);
 }

class iceb_tu_str stroka("");
int nomer_str=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  stroka.new_plus(row_alx[0]);
//  printw("%s",stroka);
  nomer_str++;
  switch(nomer_str)
   {
    case 2:
     iceb_tu_vstav(&stroka,nomdok,39,75,1);
     break;

    case 5:
     iceb_tu_vstav(&stroka,naim_kontr,10,42,1);
     iceb_tu_vstav(&stroka,dover,58,75,1);
     break;

    case 6:
     iceb_tu_vstav(&stroka,&naim_kontr[33],1,42,1);
     break;

    case 7:
     iceb_tu_vstav(&stroka,ispolnitel,12,42,1);
     break;

    case 8:
     iceb_tu_vstav(&stroka,&ispolnitel[31],1,42,1);
     break;

    case 9:
     sprintf(strsql,"%02d.%02d.%d",dd,md,gd);
     iceb_tu_vstav(&stroka,strsql,17,36,1);
     break;
     
   }
  fprintf(ff,"%s",stroka.ravno());
 }
return(0);
}
