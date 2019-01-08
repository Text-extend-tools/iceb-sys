/*$Id: spis_koment.c,v 5.13 2013/05/17 14:56:32 sasa Exp $*/
/*14.07.2015	19.03.2006	Белых А.И.	spis_koment.c
Создание меню со списком комментариев
Если выбрали возвращаем 0, если нет -1
*/
#include        "buhl.h"


int spis_koment(class iceb_tu_str *koment,const char *imaf_s_koment)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
char stroka[2048];
sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_s_koment);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"Не найдены настройки %s",imaf_s_koment);
  iceb_t_soob(strsql);
  return(1);
 }

class SPISOK komentars;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  strncpy(stroka,row_alx[0],sizeof(stroka)-1);
  if(stroka[0] == '\n')
    continue;
  ud_simv(stroka,"\n");
  komentars.plus(stroka);


 }


helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
"F10",gettext("выход"),NULL);
int kod_kl=0;
int voz=dirmasiv(&komentars, -1,-1, 0,gettext("Выберите нужное и нажмите Enter"),1,&kod_kl);

if(voz < 0 || kod_kl != ENTER)
  return(-1);

koment->new_plus(komentars.ravno(voz));
return(0);
}
