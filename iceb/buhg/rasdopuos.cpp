/*$Id: rasdopuos.c,v 5.7 2013/09/26 09:43:38 sasa Exp $*/
/*15.07.2015	11.08.2011	Белых А.И.	rasdopuos.c
Распечатка дополнительной информации по инвентарному номеру
*/
#include <errno.h>
#include "buhg.h"
#include "poiinpd.h"
#include "bsizt.h"

extern short	startgoduos;

void rasdopuos(short d,short m,short g,int innom)
{
int kolstr=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;
char strsql[512];
FILE *ff;
char imaf[64];

sprintf(strsql,"select * from Uosin where innom=%d",innom);
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %d!",gettext("Не найдена карточка инвентарного номера"),innom);
  iceb_t_soob(strsql);
  return;
 }
sprintf(imaf,"dopuos%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

zagolov(gettext("Карточка инвентарного номера"),0,0,0,d,m,g,iceb_t_get_pnk("00",0),ff);
fprintf(ff,"%s:%d\n",gettext("Инвентарный номер"),innom);
fprintf(ff,"---------------------------------------------------------\n");

fprintf(ff,"%s%s\n",gettext("Наименование................."),row[2]);//0
fprintf(ff,"%s%s\n",gettext("Завод изготовитель..........."),row[3]);//1
fprintf(ff,"%s%s\n",gettext("Паспорт/чертеж N............."),row[4]);//2
fprintf(ff,"%s%s\n",gettext("Модель/тип/марка............."),row[5]);//3
fprintf(ff,"%s%s\n",gettext("Год выпуска.................."),row[1]);//4
fprintf(ff,"%s%s\n",gettext("Заводской номер.............."),row[6]);//5
fprintf(ff,"%s%s\n",gettext("Дата ввода в эксплуатацию...."),iceb_tu_datzap(row[7]));//6

class poiinpdw_data rekin;
//poiinpd(innom,m,g,shetu,hzt,hau,hna,&popkf,&soso,nomz,&mz,&gz,hnaby,&popkfby);
poiinpd(innom,m,g,&rekin);

class iceb_tu_str naim("");
sprintf(strsql,"select nais from Plansh where ns='%s'",rekin.shetu.ravno());
if(readkey(strsql,&row,&cur) == 1)
 naim.new_plus(row[0]);
fprintf(ff,"%s%s %s\n",gettext("Счёт учета..................."),rekin.shetu.ravno(),naim.ravno());//7

naim.new_plus("");
sprintf(strsql,"select naik from Uoshz where kod='%s'",rekin.hzt.ravno());
if(readkey(strsql,&row,&cur) == 1)
 naim.new_plus(row[0]);


fprintf(ff,"%s%s %s\n",gettext("Шифр затрат амортотчислений.."),rekin.hzt.ravno(),naim.ravno());//8

naim.new_plus("");
sprintf(strsql,"select naik from Uoshau where kod='%s'",rekin.hau.ravno());
if(readkey(strsql,&row,&cur) == 1)
 naim.new_plus(row[0]);

fprintf(ff,"%s%s %s\n",gettext("Шифр аналитического учета...."),rekin.hau.ravno(),naim.ravno());//9

naim.new_plus("");
sprintf(strsql,"select naik from Uosgrup where kod='%s'",rekin.hna.ravno());
if(readkey(strsql,&row,&cur) == 1)
 naim.new_plus(row[0]);

fprintf(ff,"%s%s %s\n",gettext("Шифр н-мы амор-ний (нал.уч).."),rekin.hna.ravno(),naim.ravno());//10
fprintf(ff,"%s%.10g/%.10g\n",gettext("Поправочный коэффициент(н/б)."),rekin.popkf,rekin.popkfby);//11

class iceb_tu_str so_txt("");
if(rekin.soso == 0)
 so_txt.new_plus(gettext("Бух.учёт + Налоговый учёт +"));
if(rekin.soso == 1)
 so_txt.new_plus(gettext("Бух.учёт - Налоговый учёт -"));
if(rekin.soso == 2)
 so_txt.new_plus(gettext("Бух.учёт + Налоговый учёт -"));
if(rekin.soso == 3)
 so_txt.new_plus(gettext("Бух.учёт - Налоговый учёт +"));




fprintf(ff,"%s%s\n",gettext("Состояние объекта............"),so_txt.ravno());//12
fprintf(ff,"%s%s\n",gettext("Номерной знак................"),rekin.nomz.ravno());//13

naim.new_plus("");
sprintf(strsql,"select naik from Uosgrup1 where kod='%s'",rekin.hnaby.ravno());
if(readkey(strsql,&row,&cur) == 1)
 naim.new_plus(row[0]);
fprintf(ff,"%s%s %s\n",gettext("Шифр н-мы амор-ний (бух.уч).."),rekin.hnaby.ravno(),naim.ravno());//14


int podd=0; //Цех/участок
int kodotl=0;

poiin(innom,d,m,g,&podd,&kodotl);
class bsizw_data bal_st;
bsiz(innom,podd,d,m,g,&bal_st,NULL);

fprintf(ff,"%30s %*s  %s\n","",iceb_tu_kolbait(22,gettext("Налоговый учёт")),gettext("Налоговый учёт"),gettext("Бухгалтерский учёт"));
char bros[512];
sprintf(bros,"%s 1.%d",gettext("Балансовая стоимость на"),startgoduos);
fprintf(ff,"%*s:%10.2f %10.2f %10.2f %10.2f\n",iceb_tu_kolbait(31,bros),bros,bal_st.sbs,bal_st.siz,bal_st.sbsby,bal_st.sizby);
  
fprintf(ff,"%*s:%10.2f %10.2f %10.2f %10.2f\n",iceb_tu_kolbait(31,gettext("Изменение за период")),gettext("Изменение за период"),bal_st.bs,bal_st.iz,bal_st.bsby,bal_st.izby);
fprintf(ff,"%*s:%10s %10.2f %10s %10.2f\n",iceb_tu_kolbait(31,gettext("Амортизация")),gettext("Амортизация")," ",bal_st.iz1," ",bal_st.iz1by);

sprintf(bros,"%s %02d.%d",gettext("Балансовая стоимость на"),m,g);

fprintf(ff,"%*s:%10.2f %10.2f %10.2f %10.2f\n",iceb_tu_kolbait(31,bros),bros,bal_st.sbs+bal_st.bs,bal_st.siz+bal_st.iz+bal_st.iz1,bal_st.sbsby+bal_st.bsby,bal_st.sizby+bal_st.izby+bal_st.iz1by);

fprintf(ff,"%*s %10.2f %10s %10.2f",iceb_tu_kolbait(42,gettext("Остаток")),gettext("Остаток"),(bal_st.sbs+bal_st.bs)-(bal_st.siz+bal_st.iz+bal_st.iz1)," ",(bal_st.sbsby+bal_st.bsby)-(bal_st.sizby+bal_st.izby+bal_st.iz1by));














sprintf(strsql,"select zapis from Uosin1 where innom=%d and md=0 order by nomz asc",innom);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

if(kolstr > 0)
 {
  fprintf(ff,"\n%s\n",gettext("Дополнительная информация"));
  fprintf(ff,"---------------------------------------------------------\n");
  while(cur.read_cursor(&row) != 0)
    fprintf(ff,"%s\n",row[0]);
 }




/*Распечатка драг металлов если они есть*/
sprintf(strsql,"select * from Uosindm where innom=%d",innom);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

if(kolstr > 0)
 {
  fprintf(ff,"\n%s\n",gettext("Список драгоценных металлов"));
  fprintf(ff,"---------------------------------------------------------\n");
  while(cur.read_cursor(&row) != 0)
   {
    naim.new_plus("");
    sprintf(strsql,"select naik from Uosdm where kod=%s",row[1]);
    if(readkey(strsql,&row1,&cur1) == 1)
     naim.new_plus(row1[0]);

    fprintf(ff,"%2s %-*.*s %*s %10.10g\n",
    row[1],
    iceb_tu_kolbait(30,naim.ravno()),
    iceb_tu_kolbait(30,naim.ravno()),
    naim.ravno(),
    iceb_tu_kolbait(6,row[2]),
    row[2],
    atof(row[3]));
   }
 }




podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Карточка инвентарного номера"));

iceb_t_ustpeh(imaf,3);

iceb_t_rabfil(&oth,"");


}
