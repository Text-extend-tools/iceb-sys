/*$Id: menufil.c,v 5.9 2013/05/17 14:56:31 sasa Exp $*/
/*24.05.2016	28.01.2000	Белых А.И.	menufil.c
Чтение файла в массив и запуск меню выбора строки из массива
Возвращает номер строки выбранной в меню 
 -1 если ничего невыбрали
 -2 нет такого файла
*/
#include	"buhl.h"


int menufil(const char *imafil,
int y,int x, //Координаты вывода меню
int kk, //Номер строки если -10 выход
class SPISOK *masiv,
char *soob) //Строка сообщений
{
int		voz;

if(menufil1(imafil,masiv) == -2)
  return(-2);

/*Если в списке только одна строка то выбирать не из чего и её возвращаем*/
if(masiv->kolih() == 1)
 return(0);

int kod_kl=0;
helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
"F10",gettext("выход"),NULL);

voz=dirmasiv(masiv, y, x, kk,soob,1,&kod_kl);

return(voz);

}

/*********************************************/

int menufil1(const char *imafil,SPISOK *masiv)
{

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];


sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imafil);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"Не найдены настройки %s",imafil);
  iceb_t_soob(strsql);
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  
  if(polen(row_alx[0],strsql,sizeof(strsql),1,'|') != 0)
    continue;
  
  masiv->plus_bps(row_alx[0]);
 }


return(0);

}
