/*$Id: zarsdf1df.c,v 5.27 2013/09/26 09:43:45 sasa Exp $*/
/*27.02.2009	01.04.2004	Белых А.И.	zarsdf1df.c
Чтение списка кодов признаков доходов с кодами начислений которые к ним относятся.
*/
#include "buhg.h"
#include "sprkvrt1.h"

extern short *knvr;/*Коды начислений не входящие в расчёт подоходного налога*/

void zarsdf1df(class sprkvrt1_nast *nast)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

class iceb_tu_str bros("");
class iceb_tu_str kodd("");

sprintf(strsql,"select str from Alx where fil='zarsdf1df.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zarsdf1df.alx");
  iceb_t_soob(strsql);
  return;
 }


while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(polen(row_alx[0],&kodd,1,'|') != 0)
   continue;
  
  if(SRAV(kodd.ravno(),"Коды не входящие в форму 1ДФ",0) == 0)
   {
    sozmas(&nast->kodnvf8dr,row_alx[0]);
    continue;
   }
  if(SRAV(kodd.ravno(),"Коды удержаний не входящие в форму 1ДФ",0) == 0)
   {
    sozmas(&nast->kodud,row_alx[0]);
    continue;
   }

  if(SRAV("Счета для поиска доходов по коду",kodd.ravno(),1) == 0)
   {
  
    polen(row_alx[0],&bros,2,'|');
    if(bros.ravno()[0] == '\0')
     continue;

    iceb_t_fplus(1,bros.ravno(),&nast->sheta,&cur_alx);
     
    polen(row_alx[0],&bros,7,' ');
    nast->kodi_pd_prov.plus(bros.ravno_atoi(),-1);

    continue;
   }

  if(SRAV("Коды командировочных расходов для кода дохода",kodd.ravno(),1) == 0)
   {

    polen(row_alx[0],&bros,7,' ');
    if(bros.ravno_atoi() == 0)
     continue;

    nast->kodkr.plus(bros.ravno_atoi(),-1);
  
    polen(row_alx[0],&bros,2,'|');

    iceb_t_fplus(1,bros.ravno(),&nast->kodd_for_kodkr,&cur_alx);
     

    continue;
   }

  if(SRAV("Физическое лицо",kodd.ravno(),0) == 0)
   {
    polen(row_alx[0],&bros,2,'|');
    if(SRAV(bros.ravno(),"Вкл",1) == 0)
     {
      nast->TYP=1;
     }
    continue;
   }

  if(kodd.ravno_atoi() == 0)
   continue;

  if(kodd.ravno_atoi() == 133)
   {
    polen(row_alx[0],&nast->gosstrah,2,'|');
    continue;
   }   

  nast->kodi_pd.plus(atoi(kodd.ravno()),-1);
  polen(row_alx[0],&kodd,2,'|');
  nast->kodi_nah.plus(kodd.ravno());    
  /*проверяем по первому коду берётся подоходный или нет остальные коды должны по идее всегда соотвецтвовать первому*/
  if(provkod(knvr,kodd.ravno_atoi()) >= 0)
   nast->metka_podoh.plus(1);
  else
   nast->metka_podoh.plus(0);
 }

}
