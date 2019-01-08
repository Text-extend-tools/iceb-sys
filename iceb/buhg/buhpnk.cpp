/*$Id: buhpnk.c,v 5.1 2013-04-07 10:22:41 sasa Exp $*/
/*30.03.2016	04.03.2013	Белых А.И.	buhpnk.c
Получение наименования контрагента для проводки 
Если код контрагента в проводке не введён и проводка сделана из какойнибудь подсистемы то пытаемся найти
Для зарплаты ищем по номеру документа (в номере документа есть табельный номер)
Для остальных подсистем по дате номеру документа и номеру подразделения ищем в шапке документа
номер контрагента

*/
#include "buhg.h"

int buhpnk(class iceb_tu_str *naimkontr, /*сюда записываем полученное наименование контрагента*/
char *kodkontr, /*код контрагента из проводки если он там есть*/
int metka_pod, /*метка подсистемы смотри iceb_t_mpods.c */
int tipz, /*1-приходный 2-расходный*/
char *datdok, /*дата документа в SQL формате*/
char *nomdok,
int podr)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

naimkontr->new_plus("");

if(kodkontr[0] != '\0')
 {
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kodkontr);
  if(readkey(strsql,&row,&cur) != 1)
   {
    /*в проводке может быть табельный номер и его нет в списке контрагентов*/
    if(SRAV(ICEB_ZAR_PKTN,kodkontr,1) == 0)
     {
      char tabnom[128];
      sprintf(tabnom,"%s",iceb_tu_adrsimv(iceb_tu_strlen(ICEB_ZAR_PKTN),kodkontr));
      sprintf(strsql,"select fio from Kartb where tabn=%s",tabnom);
      if(readkey(strsql,&row,&cur) == 1)
       {
        naimkontr->new_plus(row[0]);
        return(0);
       }
  
     }
    beep();
    printw("\n%s-%s %s\n",__FUNCTION__,gettext("Не найден код контрагента"),kodkontr);
    OSTANOV();
    return(1);
   }
  else
   naimkontr->new_plus(row[0]);

  return(0);
 }
else
 {
  class iceb_tu_str kod_kontr("");   
  short d=0,m=0,g=0;
  switch(metka_pod)
   {
    case 0: /*проводка без признака подсистемы*/
      return(0);

    case 1: /*материальный учёт*/
     sprintf(strsql,"select kontr from Dokummat where datd='%s' and sklad=%d and nomd='%s' and tip=%d",datdok,podr,nomdok,tipz);
     if(readkey(strsql,&row,&cur) == 1)
      kod_kontr.new_plus(row[0]);
     break;

    case 2: /*заработная плата*/
     polen(nomdok,&kod_kontr,2,'-');     
     sprintf(strsql,"select fio from Kartb where tabn=%d",kod_kontr.ravno_atoi());
     if(readkey(strsql,&row,&cur) == 1)
      {
       naimkontr->new_plus(row[0]);
       return(0);
      }

     return(1 );       

    case 3: /*учёт основных средств*/
     sprintf(strsql,"select kontr from Uosdok where datd='%s' and nomd='%s' and tipz=%d",datdok,nomdok,tipz);
     if(readkey(strsql,&row,&cur) == 1)
      kod_kontr.new_plus(row[0]);
     break;

    case 4: /*Платёжные поручения*/
     sprintf(strsql,"select polu from Pltp where datd='%s' and nomd='%s'",datdok,nomdok);
     if(readkey(strsql,&row,&cur) != 1)
      return(1);
     polen(row[0],&kod_kontr,1,'#');
     
     break;

    case 5: /*Платёжные требования*/
     sprintf(strsql,"select plat from Pltt where datd='%s' and nomd='%s'",datdok,nomdok);
     if(readkey(strsql,&row,&cur) != 1)
      return(1);
     polen(row[0],&kod_kontr,1,'#');
     
     break;
 
    case 6: /*учёт услуг*/
     sprintf(strsql,"select kontr from Usldokum where datd='%s' and podr=%d and nomd='%s' and tp=%d",datdok,podr,nomdok,tipz);
     if(readkey(strsql,&row,&cur) == 1)
      kod_kontr.new_plus(row[0]);
     break;

    case 7: /*учёт кассовых ордеров*/
     rsdat(&d,&m,&g,datdok,2);
     sprintf(strsql,"select kontr from Kasord1 where kassa=%d and god=%d and tp=%d and nomd='%s'",podr,g,tipz,nomdok);
     if(readkey(strsql,&row,&cur) == 1)
      kod_kontr.new_plus(row[0]);
     break;

    case 8: /*учёт командировочных расходов*/
     rsdat(&d,&m,&g,datdok,2);
     sprintf(strsql,"select kont from Ukrdok where god=%d and nomd='%s'",g,nomdok);
     if(readkey(strsql,&row,&cur) == 1)
      kod_kontr.new_plus(row[0]);
     break;
   };

  if(kod_kontr.getdlinna() <= 1)
   return(0);
      
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kod_kontr.ravno());
  if(readkey(strsql,&row,&cur) == 1)
   {
    naimkontr->new_plus(kod_kontr.ravno());
    naimkontr->plus(" ",row[0]);
   }
  else
   return(1);
 }

return(0);
}
