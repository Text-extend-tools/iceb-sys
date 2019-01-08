/* $Id: iceb_t_nomnak.c,v 5.14 2013/10/04 07:25:02 sasa Exp $ */
/*03.10.2013	02.02.1999	Белых А.И.	iceb_t_nomnak.c
Определение следующего номера накладной
*/
#include	"buhl.h"

extern short   mfnn; /*Метка формирования номера накладной
                      -1 - Автоматическая нумерация выключена
                       0 - Приходы и расходы - нумерация общая
                       1 - Приходы отдельно расходы отдельно
                      */

void iceb_t_nomnak(const char *data,
const char *skll, //Склад
class iceb_tu_str *ndk,  //Номер документа
int tz,
int metka, //0-мат. учет 1-услуги 2-кассовые ордера 3-командировочные расходы
int metka_lid_nol)  //0-без лидирующих нолей 1-с лидирующими нолями
{
short d=0,m=0,g=0;
rsdat(&d,&m,&g,data,1);

char nomer_dok[512];
memset(nomer_dok,'\0',sizeof(nomer_dok));

iceb_t_nomnak(g,skll,nomer_dok,tz,metka,metka_lid_nol);
ndk->new_plus(nomer_dok);
}
/*********************/
void iceb_t_nomnak(short g,
const char *skll, //Склад
class iceb_tu_str *ndk,  //Номер документа
int tz,
int metka, //0-мат. учет 1-услуги 2-кассовые ордера 3-командировочные расходы
int metka_lid_nol)  //0-без лидирующих нолей 1-с лидирующими нолями
{

char nomer_dok[512];
memset(nomer_dok,'\0',sizeof(nomer_dok));

iceb_t_nomnak(g,skll,nomer_dok,tz,metka,metka_lid_nol);
ndk->new_plus(nomer_dok);
}
/**********************************/
void iceb_t_nomnak(short g,
int skll, //Склад
class iceb_tu_str *ndk,  //Номер документа
int tz,
int metka, //0-мат. учет 1-услуги 2-кассовые ордера 3-командировочные расходы
int metka_lid_nol)  //0-без лидирующих нолей 1-с лидирующими нолями
{
class iceb_tu_str sklad(skll);
char nomer_dok[512];
memset(nomer_dok,'\0',sizeof(nomer_dok));

iceb_t_nomnak(g,sklad.ravno(),nomer_dok,tz,metka,metka_lid_nol);
ndk->new_plus(nomer_dok);
}
/**********************************************************/
void iceb_t_nomnak(short g,
const char *skll, //Склад
char *ndk,  //Номер документа
int tz,
int metka, //0-мат. учет 1-услуги 2-кассовые ордера 3-командировочные расходы
int metka_lid_nol)  //0-без лидирующих нолей 1-с лидирующими нолями
{
class iceb_tu_str bros("");
long            mpn;  /*Максимальный порядковый номер*/
long            pn;   /*Порядковый номер*/
class iceb_tu_str skkk("");
char		strsql[512];
int		kolstr=0;
SQL_str		row;

ndk[0]='\0';
if(mfnn == -1)
  return;

GDITE();

if(mfnn == 1)
 skkk.new_plus(skll);  
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
  msql_error(&bd,__FUNCTION__,strsql);
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
    polen(row[0],&pn,2,'/');
//    pn=(long)ATOFM(bros);
   }

  if(mfnn == 1)
   {
    if(tz == 1)
     if(polen(row[0],&bros,1,'+') == 0)
      if(SRAV(skkk.ravno(),bros.ravno(),0) == 0) 
       { 
        polen(row[0],&pn,2,'+');
//        pn=(long)ATOFM(bros);
       }

    if(tz == 2)
     if(polen(row[0],&bros,1,'-') == 0)
      if(SRAV(skkk.ravno(),bros.ravno(),0) == 0) 
       { 
        polen(row[0],&pn,2,'-');
//        pn=(long)ATOFM(bros);
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
