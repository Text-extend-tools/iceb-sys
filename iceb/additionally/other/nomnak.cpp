/* $Id: nomnak.c,v 5.10 2011-01-13 13:43:22 sasa1 Exp $ */
/*05.12.2008	02.02.1999	Белых А.И.	nomnak.c
Определение следующего номера накладной
*/
#include	"buhg.h"

extern short   mfnn; /*Метка формирования номера накладной
                      -1 - Автоматическая нумерация выключена
                       0 - Приходы и расходы - нумерация общая
                       1 - Приходы отдельно расходы отдельно
                      */

void nomnak(short g,
const char *skll, //Склад
char *ndk,  //Номер документа
int tz,
int metka, //0-мат. учет 1-услуги 2-кассовые ордера 3-командировочные расходы
int metka_lid_nol)  //0-без лидирующих нолей 1-с лидирующими нолями
{
char		bros[512];
long            mpn;  /*Максимальный порядковый номер*/
long            pn;   /*Порядковый номер*/
char		skkk[32];
char		strsql[512];
int		kolstr=0;
SQL_str		row;

ndk[0]='\0';
if(mfnn == -1)
  return;

GDITE();

memset(skkk,'\0',sizeof(skkk));
if(mfnn == 1)
 sprintf(skkk,"%s",skll);  
pn=mpn=0;

if(metka == 0)
 sprintf(strsql,"select nomd from Dokummat where tip=%d and \
datd >= '%04d-01-01' and datd <= '%04d-12-31' and sklad=%s",tz,g,g,skll);

if(metka == 1)
 sprintf(strsql,"select nomd from Usldokum where tp=%d and \
datd >= '%04d-01-01' and datd <= '%04d-12-31' and podr=%s",tz,g,g,skll);

if(metka == 2)
 sprintf(strsql,"select nomd from Kasord where tp=%d and \
god=%d and kassa=%s",tz,g,skll);

if(metka == 3)
 sprintf(strsql,"select nomd from Ukrdok where god=%d",g);

//printw("\n%s\n",strsql);
//OSTANOV();
//refresh();
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
//  printw("%d %s\n",kolstr,row[0]);  
//  refresh();
  if(metka == 3)
   {
    pn=atoi(row[0]);
   }

  if(mfnn == 0)
   {
    POLE(row[0],bros,2,'/');
    pn=(long)ATOFM(bros);
   }

  if(mfnn == 1)
   {
    if(tz == 1)
     if(POLE(row[0],bros,1,'+') == 0)
      if(SRAV(skkk,bros,0) == 0) 
       { 
        POLE(row[0],bros,2,'+');
        pn=(long)ATOFM(bros);
       }

    if(tz == 2)
     if(POLE(row[0],bros,1,'-') == 0)
      if(SRAV(skkk,bros,0) == 0) 
       { 
        POLE(row[0],bros,2,'-');
        pn=(long)ATOFM(bros);
       } 
   }

  if(metka == 3)
   {
    pn=atoi(row[0]);
   }

  if(pn > mpn)
   mpn=pn;

 }
//printw("kolstr=%d mpn=%d\n",kolstr,mpn);
//refresh();

if(mfnn == 0) //Приходы и расходы - общая нумерация
 {
  if(metka_lid_nol == 0)
   {
    if(metka != 3)
     sprintf(ndk,"%s/%ld",skll,mpn+1);
    else
     sprintf(ndk,"%ld",mpn+1);
   }
  else
   {
    if(metka != 3)
     sprintf(ndk,"%s/%05ld",skll,mpn+1);
    else
     sprintf(ndk,"%05ld",mpn+1);
   }
  
 }
 
if(mfnn == 1)
 {
  if(metka_lid_nol == 0)
   {
    if(tz == 1)
      sprintf(ndk,"%s+%ld",skll,mpn+1);
    if(tz == 2)
      sprintf(ndk,"%s-%ld",skll,mpn+1);
   }
  else
   {
    if(tz == 1)
      sprintf(ndk,"%s+%05ld",skll,mpn+1);
    if(tz == 2)
      sprintf(ndk,"%s-%05ld",skll,mpn+1);
   }
 }
clearstr(LINES-1,0);
}
