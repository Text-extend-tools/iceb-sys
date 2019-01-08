/*$Id: vibshbn.c,v 5.24 2013/08/13 05:49:59 sasa Exp $*/
/*13.09.2010	17.11.2001	Белых А.И.	vibshbn.c
Выбор счета с которого будет осуществляться оплата
Если вернули 0-счет выбран
             1-нет
*/
#include        <ctype.h>
#include        "buhg.h"


int vibshbn(char *kontr,const char *tabl)
{
class iceb_tu_str bros("");
class iceb_tu_str shet(""); //Номер счета
class iceb_tu_str naimbn(""); //Наименование банка
SPISOK	SHETA;
SPISOK	KONTR; //Код контрагента
class iceb_tu_str imafil("");
SQL_str row_alx;
class SQLCURSOR cur_alx;
char strsql[1024];
int kolstr=0;

if(SRAV(tabl,"Pltp",0) == 0 || SRAV(tabl,"Tpltp",0) == 0)
  imafil.new_plus("avtprodok.alx");
if(SRAV(tabl,"Pltt",0) == 0 || SRAV(tabl,"Tpltt",0) == 0)
  imafil.new_plus("avtprodokt.alx");

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imafil.ravno());
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imafil.ravno());
  iceb_t_soob(strsql);
  return(1);
 }



while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#' || row_alx[0][0] == ' ' || row_alx[0][0] == '\t' ||
  row_alx[0][0] == '\n')
   continue;
  
  if(polen(row_alx[0],&shet,1,'|') == 0)
   {
    if(isdigit(shet.ravno()[0]) == 0) //Значит символ не число
      continue;
    if(polen(row_alx[0],&bros,3,'|') != 0)
      continue;
    KONTR.plus(bros.ravno());

    polen(row_alx[0],&naimbn,4,'|');

    sprintf(strsql,"%-*s|%s",iceb_tu_kolbait(15,shet.ravno()),shet.ravno(),naimbn.ravno());    
    SHETA.plus(strsql);


   }

 }

if(SHETA.kolih() != 0)
 {
  helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
  "F10",gettext("выход"),NULL);
  int kod_kl=0;
  int voz=dirmasiv(&SHETA, -1, -1,0,gettext("Выберите нужный счет"),1,&kod_kl);
  if(voz == -1 || kod_kl != ENTER)
    return(1);
  strcpy(kontr,KONTR.ravno(voz));
 }
else
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD("В файле нет ни одного счета");
  SOOB.VVOD_SPISOK_add_MD(imafil.ravno());
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return(1);
 }
return(0);
}
