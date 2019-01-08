/*$Id: imp_kasord.c,v 5.32 2013/11/05 10:50:48 sasa Exp $*/
/*14.07.2015	28.03.2007	Белых А.И.	imp_kasord.c
Импорт кассовых ордеров из файла
*/
#include        <sys/stat.h>
#include        "buhg.h"


int imp_kasord_r(const char *imafim,int metka,class SPISOK *kodkontr_new);

void imp_kasord()
{
struct stat work;
static class iceb_tu_str imafim("");
char            strsql[512];

naz:;

if(iceb_t_menu_import(&imafim,gettext("Импорт кассовых ордеров"),"kasniko.alx","kas2_5.txt") != 0)
 return;
 

clear();
move(LINES-1,0);
GDITE();

if(stat(imafim.ravno(),&work) != 0)
 {
  sprintf(strsql,"%s !",gettext("Нет такого файла"));
  iceb_t_soob(strsql);
  goto naz;
 }
class SPISOK kodkontr_new;
int kolih_oh=0;
if((kolih_oh=imp_kasord_r(imafim.ravno(),0,&kodkontr_new)) == 0)
 {
  imp_kasord_r(imafim.ravno(),1,&kodkontr_new);
  unlink(imafim.ravno());
 }
else
 {
   VVOD SOOB(1);
   sprintf(strsql,"%s %d !",gettext("Количество ошибок"),kolih_oh);
   SOOB.VVOD_SPISOK_add_MD(strsql);
   SOOB.VVOD_SPISOK_add_MD(gettext("Загрузка невозможна !"));
   soobshw(&SOOB,stdscr,-1,-1,0,1);
 }
}
/************************/
/*Чтение из файла и выполнение команд*/
/***********************************/

void iceb_kasord_system()
{
char stroka[2048];
int voz=0;
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='kasniko.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"kasniko.alx");
  iceb_t_soob(strsql);
  return;
 }


while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  strncpy(stroka,row_alx[0],sizeof(stroka)-1);
  ud_simv(stroka,"\n");
  voz=iceb_t_system(stroka,0);
  if(voz != 0)
   OSTANOV();
 }

}
