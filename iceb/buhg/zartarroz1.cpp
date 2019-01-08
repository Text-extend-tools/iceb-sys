/*$Id: zartarroz1.c,v 5.10 2013/05/17 14:56:18 sasa Exp $*/
/*03.12.2008	28.11.2008	Белых А.И.	zartarroz1.c
Работа с тарифной сеткой
*/

#include <errno.h>
#include "buhg.h"

class zartarroz1_data
 {
  public:
   class masiv_din_int roz;
   class SPISOK tarset;
   
   void clear()
    {
     roz.free_class();
     tarset.free_class();
    }
 
 };

class zartarroz1_data zar_tark;

int zartarroz1()
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

zar_tark.clear();

sprintf(strsql,"select str from Alx where fil='zartarroz1.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  return(1);
 }



char stroka[1024];
memset(stroka,'\0',sizeof(stroka));
class iceb_tu_str roz;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(polen(row_alx[0],&roz,1,'|') != 0)
   continue;

  if(roz.ravno_atoi() == 0)
   continue;

  zar_tark.roz.plus(roz.ravno_atoi());  
  zar_tark.tarset.plus(row_alx[0]);
 }
return(0);


}
/******************************/
/*Найти нужный тариф*/
/******************************/
double zartarroz1(int roz,int kof,int tabnom,short mr,short gr)
{
char strsql[512];
double suma=0.;
int nomer_roz=0;
if(zar_tark.roz.kolih() == 0)
 {
  if(mr > 0)
   {
    sprintf(strsql,"%s-количество номеров тарифной сетки равно нолю!\n",__FUNCTION__);
    zar_pr_ins(strsql);
   }   
  return(0.);
 }  
if((nomer_roz=zar_tark.roz.find(roz)) < 0)
 {
  if(mr > 0)
   {
    sprintf(strsql,"%s-Не найден номер тарифа %d списке !\n",__FUNCTION__,roz);
    zar_pr_ins(strsql);
   }
  return(0.);
 }

if(mr > 0)
 {
  sprintf(strsql,"Строка тарифной сетки N%d :%s",nomer_roz+2,zar_tark.tarset.ravno(nomer_roz));
  zar_pr_ins(strsql);
 }

if(polen(zar_tark.tarset.ravno(nomer_roz),&suma,kof+1,'|') == 0)
 {
  if(mr > 0)
   {
    sprintf(strsql,"Номер колонки:%d значение:%f\n",kof,suma);
    zar_pr_ins(strsql);
   }
 }
else
 {
  if(mr > 0)
   {
    sprintf(strsql,"Не нашли поле %d в строке тарифной сетки!\n",kof+1);
    zar_pr_ins(strsql);
   }
 }
return(suma);
}
