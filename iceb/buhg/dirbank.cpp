/* $Id: dirbank.c,v 5.17 2013/05/17 14:55:57 sasa Exp $ */
/*13.07.2015	12.05.2001	Белых А.И.	dirbank.c
Создание меню выбора банка
Если вернули 0 выбрали банк
*/
#include        <errno.h>
#include        "buhg.h"


int dirbank(int *nomb,//Номер банка
class iceb_tu_str *imamah,//Имя машины в сети
class iceb_tu_str *katalog, //Каталог
class iceb_tu_str *imafil, //Имя файла для импорта
class iceb_tu_str *parol, //Пароль
class iceb_tu_str *comanda,
class iceb_tu_str *script)
{
class iceb_tu_str stroka("");
int	kolbank=0;
class iceb_tu_str bros("");
int	K=0;
int	nomer;
char strsql[512];
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;

sprintf(strsql,"select str from Alx where fil='banki.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s banki.alx",gettext("Не найдены настроки"));
  iceb_t_soob(strsql);
  return(1);
 }

//Определяем количество настроеных банков
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(polen(row_alx[0],&bros,2,'|') != 0)
   continue;  
  if(bros.ravno_atoi() == 0)
   continue;
  kolbank++;
 }

if(kolbank == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одного банка в файле !"));
  return(1);
 }


//Создаем класс меню
class iceb_t_mstr spis_menu(0);
char strmenu[1024];
//Создаем класс
class SPISOK NASTR;

cur_alx.poz_cursor(0);
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  if(polen(row_alx[0],nomb,2,'|') != 0)
    continue;  
  if(*nomb == 0)
   continue;

  if(polen(row_alx[0],&bros,1,'|') != 0)
    continue;  

  spis_menu.spis_str.plus(bros.ravno());

  if(pole2(row_alx[0],'|') == 3) //команда
   {
    polen(row_alx[0],comanda,3,'|');
    bros.new_plus(*nomb);
    bros.plus("|",comanda->ravno());
    NASTR.plus(bros.ravno());
   }
  else
   {
    if(polen(row_alx[0],imamah,3,'|') != 0)
      continue;  
    if(polen(row_alx[0],katalog,4,'|') != 0)
      continue;  
    if(polen(row_alx[0],imafil,5,'|') != 0)
      continue;  
    if(polen(row_alx[0],parol,6,'|') != 0)
      continue;  

    polen(row_alx[0],script,7,'|');

    sprintf(strmenu,"%d|%s|%s|%s|%s|%s",*nomb,imamah->ravno(),katalog->ravno(),imafil->ravno(),parol->ravno(),script->ravno());
    NASTR.plus(strmenu);
   }
 }

if(kolbank > 1)
 {
nazad:;

  helstr(LINES-1,0,"F10",gettext("выход"),NULL);
    
  int prc=100;
  memset(strsql,'\0',sizeof(strsql));
  nomer=spis_menu.menu(0,-1,-1,NULL,strsql,prc,&K,gettext("Виберите нужный банк"),0);
  switch(K)
   {
    case ENTER:
      break;
    case FK10:
    case ESC:
      return(1);
    default:
     goto nazad;
   }
 }
else
 nomer=0;

stroka.new_plus(NASTR.ravno(nomer));
polen(stroka.ravno(),nomb,1,'|');
polen(stroka.ravno(),comanda,2,'|');

if(pole2(stroka.ravno(),'|') > 3)
 {
  polen(stroka.ravno(),imamah,2,'|');
  polen(stroka.ravno(),katalog,3,'|');
  polen(stroka.ravno(),imafil,4,'|');
  polen(stroka.ravno(),parol,5,'|');
  comanda->new_plus("");
  polen(stroka.ravno(),script,6,'|');
 }
/*********
printw("\n%s\nnomer=%d nomb=%d imamah=%s katalog=%s imafil=%s\n",
stroka.ravno(),nomer,*nomb,imamah,katalog,imafil);
OSTANOV();
************/
return(0);
}
