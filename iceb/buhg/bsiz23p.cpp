/* $Id: bsiz23p.c,v 5.21 2013-04-18 09:26:02 sasa Exp $ */
/*13.07.2015    03.10.2004      Белых А.И.      bsiz23p.c
Получение балансовой стоимсости и износа за период для 
групп у которых амортизация расчитывается в целом по группе
*/
#include        "buhg.h"
#include "poiinpd.h"

extern short	startgoduos; /*Стартовый год*/

void bsiz23p(const char *kodgr,
short dn,short mn,short gn,
short dk,short mk,short gk,
class bsiz_data *data)
{
long		kolstr;
SQL_str         row;
SQLCURSOR curr;
char		strsql[1024];
char            bros[1024];
char            bros1[1024];
short           d,m,g;
short		tp;
long		innom;
double		pbs,piz;
class poiinpdw_data rekin;

data->nu.clear_data();

//class iceb_tu_str filnast;
//imafn("uosnast.alx",&filnast);

sprintf(strsql,"Стартовая балансовая стоимость для группы %s",kodgr);
if(iceb_t_poldan(strsql,bros,"uosnast.alx") != 0)
 {
  VVOD SOOB(1);
  sprintf(bros1,"%s %s",gettext("Не найдена"),strsql);
  SOOB.VVOD_SPISOK_add_MD(bros1);
  SOOB.VVOD_SPISOK_add_MD("uosnast.alx");
  soobshw(&SOOB,stdscr,-1,-1,0,1);
 }

data->nu.bs_nah_per+=ATOFM(bros);
data->nu.bs_kon_per+=ATOFM(bros);

sprintf(strsql,"Стартовый износ для группы %s",kodgr);
if(iceb_t_poldan(strsql,bros,"uosnast.alx") != 0)
 {
  sprintf(bros1,"Не найден %s",strsql);
  iceb_t_soob(bros1);
 }

data->nu.izmen_iznosa_nah_per+=ATOFM(bros);
data->nu.izmen_iznosa_kon_per+=ATOFM(bros);

/*
printw("bsiz23--%s %d.%d.%d\n",kodgr,dp,mp,gp);
refresh();
*/

sprintf(strsql,"select datd,tipz,innom,podr,bs,iz,nomd,kodop from Uosdok1 where \
datd >= '%d-%d-%d' and datd <= '%d-%d-%d' and podt=1 \
order by datd asc, tipz desc",startgoduos,1,1,gk,mk,dk);

class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s-%s",__FUNCTION__,gettext("Не найдено ни одной записи !"));
  iceb_t_soob(strsql);
  return;
 }




while(cur.read_cursor(&row) != 0)
 {
/*****
  printw("%s %s %s %s %s %s %s\n",
  row[0],row[1],row[2],row[3],row[4],row[5],row[6]);
  refresh();
**********/
  if(SRAV(gettext("ОСТ"),row[7],0) == 0)
    continue;
      
  rsdat(&d,&m,&g,row[0],2);
  tp=atoi(row[1]);
  innom=atol(row[2]);
  pbs=atof(row[4]);
  piz=atof(row[5]);
  /*Читаем переменные данные на конец периода так как мог быть приход в периоде*/
//  poiinpd(innom,mk,gk,shetu,hzt,hau,hna,&popkf,&soso,nomz,&mz,&gz,hnaby,&popkfby);
  poiinpd(innom,mk,gk,&rekin);

  if(SRAV(kodgr,rekin.hna.ravno(),0) != 0)
   continue;
  
  if(tp == 2)
   {
    pbs*=(-1.);
    piz*=(-1.);
   }
  
  if(sravmydat(d,m,g,dn,mn,gn) < 0) //до начала периода
   {
    data->nu.bs_nah_per+=pbs;
    data->nu.izmen_iznosa_nah_per+=piz;
   }
  else
   {
    if(tp == 2)
     {
      data->nu.bs_rash_za_per+=pbs;
      data->nu.iznos_rash_za_per+=piz;
     }
    else
     {
      if(startgoduos == g && SRAV(row[7],gettext("ОСТ"),0) == 0)
       {
        //это стартовый приход
       }
      else
       {
        data->nu.bs_prih_za_per+=pbs;
        data->nu.iznos_prih_za_per+=piz;
       }
     }
   }
  data->nu.bs_kon_per+=pbs;
  data->nu.izmen_iznosa_kon_per+=piz;

 }


/*
printw("bs23=%.2f iz-%.2f\n",*bs,*iz);
refresh();
OSTANOV();
*/

/*Узнаем амортизацию*/
/*******************************
if(startgoduos != 0 && startgoduos != gk)
  sprintf(strsql,"select suma,mes,god,innom from Uosamor where \
((god = %d and mes >= %d) or \
(god > %d and god < %d) or \
(god = %d and mes <= %d)) and \
innom=%d and suma <> 0. order by god,mes asc",
startgoduos,1,startgoduos,gk,gk,mk,atoi(kodgr)*(-1));
else
  sprintf(strsql,"select suma,mes,god,innom from Uosamor where \
god = %d and mes >= %d and  mes <= %d and \
innom=%d and suma <> 0. order by god,mes asc",gk,1,mk,atoi(kodgr)*(-1));
*********************/
sprintf(strsql,"select suma,da from Uosamor where \
da >= '%04d-01-01' and da <= '%04d-%02d-31' and \
innom=%d and suma <> 0. order by da asc",startgoduos,gk,mk,atoi(kodgr)*(-1));

//printw("\n%s\n",strsql);
//refresh();


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  rsdat(&d,&m,&g,row[1],2);

//  m=atoi(row[1]);
//  g=atoi(row[2]);
  piz=atof(row[0]);  
  if(sravmydat(1,m,g,1,mn,gn) < 0)
    data->nu.amort_nah_per+=piz;

  data->nu.amort_kon_per+=piz;

 }

data->nu.bs_raznica=data->nu.bs_kon_per-data->nu.bs_nah_per;
data->nu.izmen_iznosa_raznica=data->nu.izmen_iznosa_kon_per-data->nu.izmen_iznosa_nah_per;
data->nu.amort_raznica=data->nu.amort_kon_per-data->nu.amort_nah_per;

/******************
printw("Код группы налогового учёта %s\n\
%10.2f %10.2f %10.2f\n\
%10.2f %10.2f %10.2f\n\
%10.2f %10.2f %10.2f\n",
kodgr,
data->nu.bs_nah_per,data->nu.izmen_iznosa_nah_per,data->nu.amort_nah_per,
data->nu.bs_kon_per,data->nu.izmen_iznosa_kon_per,data->nu.amort_kon_per,
data->nu.bs_raznica,data->nu.izmen_iznosa_raznica,data->nu.amort_raznica);
OSTANOV();
****************/

}
