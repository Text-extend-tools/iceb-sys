/*$Id: shet_fak_sap.c,v 5.12 2013/05/17 14:56:11 sasa Exp $*/
/*02.10.2014	21.12.2005	Белых А.И.	shet_fak_sap.c
Распечатка шапки счёта-фактуры
*/
#include	"buhg.h"


int shet_fak_sap(short dd,short md,short gd,
const char *nomdok,
const char *ras_shet,
const char *mfo,
const char *kod,
const char *naim_kontr,
const char *dover,
const char *herez,
FILE *ff)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
class iceb_tu_str stroka("");
char bros[512];
int kol_str_ots=0;
//читаем количество сторок отступа для логотипа
if(iceb_t_poldan("Количество строк отступа для счёта с логотипом",bros,"uslnast.alx") == 0)
  kol_str_ots=atoi(bros);

for(int i=0; i < kol_str_ots; i++)
 fprintf(ff,"\n");

sprintf(strsql,"select str from Alx where fil='usl_shet_fak.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"usl_shet_fak.alx");
  iceb_t_soob(strsql);
  return(1);
 }



int nomer_str=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  nomer_str++;
  stroka.new_plus(row_alx[0]);
  switch(nomer_str)
   {
    case 2:
     iceb_tu_vstav(&stroka,ras_shet,15,67,1);
     break;

    case 3:
     iceb_tu_vstav(&stroka,mfo,15,67,1);
     break;

    case 4:
     iceb_tu_vstav(&stroka,kod,15,67,1);
     break;

    case 6:
     iceb_tu_vstav(&stroka,nomdok,21,39,1);
     sprintf(bros,"%02d.%02d.%d",dd,md,gd);     
     iceb_tu_vstav(&stroka,bros,58,69,1);
     break;

    case 8:
     iceb_tu_vstav(&stroka,naim_kontr,12,67,1);
     break;

    case 11:
     iceb_tu_vstav(&stroka,dover,16,27,1);
     iceb_tu_vstav(&stroka,herez,34,67,1);
     break;
     
   }
  fprintf(ff,"%s",stroka.ravno());
 }

return(0);
}
