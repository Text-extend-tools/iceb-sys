/*$Id:$*/
/*14.07.2015	14.07.2015	Белых А.И.	iceb_t_nabor.c
Получение меню для выбора нужного набора реквизитов
Если выбрали возвращаем 0, если нет -1
*/
#include        "buhl.h"
int iceb_t_nabor(class iceb_tu_str *nabor,const char *imaf_s_nabor)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
class SPISOK spnabor;
class iceb_tu_str naim("");

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_s_nabor);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"Не найдены настройки %s",imaf_s_nabor);
  iceb_t_soob(strsql);
  return(1);
 }

class SPISOK naborars;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(polen(row_alx[0],&naim,1,'|') != 0)
   continue;

  naborars.plus(naim.ravno());

  if(polen(row_alx[0],&naim,2,'|') != 0)
   continue;

  spnabor.plus(naim.ravno());  


 }


helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
"F10",gettext("выход"),NULL);
int kod_kl=0;
int voz=dirmasiv(&naborars, -1,-1, 0,gettext("Выберите нужное и нажмите Enter"),1,&kod_kl);

if(voz < 0 || kod_kl != ENTER)
  return(-1);

nabor->new_plus(spnabor.ravno(voz));
return(0);
}
