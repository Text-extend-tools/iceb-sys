/*$Id: muso.c,v 5.3 2013/09/26 09:43:36 sasa Exp $*/
/*13.03.2013	13.03.2013	Белых А.И.	muso.c
Автоматическое формирование документа на списание остатков по складу
для расходного документа
*/
#include "buhg.h"
extern short     mdd;  /*0-обычный документ 1-двойной*/

void muso(short dd,short md,short gd,const char *nomdok,int sklad,const char *kodkontr,const char *nomdokp)
{
char strsql[1024];
int kolstr=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;

int sklad2=0;


sprintf(strsql,"select * from Kart where sklad=%d",sklad);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной карточки !"));
  return;
 }

if(mdd == 1)
 {
  polen(kodkontr,&sklad2,2,'-');  
 }

class ostatokt ost;
int nomkart=0;

while(cur.read_cursor(&row) != 0)
 {
  ostkar(1,1,gd,dd,md,gd,sklad,atoi(row[1]),&ost);
  if(ost.ostg[3] <= 0.)
    continue; 

  nomkart=0;
  
  if(mdd == 1) /*Двойной документ*/
   {
    int nomkart=0;
    /*проверяем может карточка с подходящими реквизитами уже есть*/
    sprintf(strsql,"select nomk from Kart where kodm=%s and \
sklad=%d and shetu='%s' and cena=%s and ei='%s' and nds=%s \
and cenap=%s and krat=%s and mnds=%s and dat_god='%s'",
    row[2],sklad2,row[5],row[6],row[4],row[9],row[7],row[8],row[3],row[18]);
    if(readkey(strsql,&row1,&cur1) > 0)
     {
      nomkart=atoi(row1[0]);
     }
    else
     {
      short denv=0,mesv=0,godv=0; /*дата ввода в эксплуатацию*/
      short deng=0,mesg=0,godg=0; /*конечная дата годности товара*/
      /*создаём новую карточку*/
      nomkart=nomkr(sklad2);
      rsdat(&denv,&mesv,&godv,row[14],2);
      rsdat(&deng,&mesg,&godg,row[18],2);
                        
      if(zapkar(sklad2,nomkart,atoi(row[2]),atof(row[6]),row[4],row[5],atof(row[8]),atof(row[10]),atoi(row[11]),atof(row[9]),atoi(row[3]),atof(row[7]),denv,mesv,godv,row[15],row[16],row[17],deng,mesg,godg,0) != 0)
        continue;

     }

    /*запись в приходный документ*/
    zapvdok(dd,md,gd,sklad2,nomkart,atoi(row[2]),nomdokp,ost.ostg[3],atof(row[6]),row[4],atof(row[9]),atoi(row[3]),0,1,atoi(row[1]),"","",0,"");
    
   }
  /*запись в расходный документ*/
  zapvdok(dd,md,gd,sklad,atoi(row[1]),atoi(row[2]),nomdok,ost.ostg[3],atof(row[6]),row[4],atof(row[9]),atoi(row[3]),0,2,nomkart,"","",0,"");
 }

}
