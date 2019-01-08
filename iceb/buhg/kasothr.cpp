/* $Id: kasothr.c,v 5.63 2013/09/26 09:43:34 sasa Exp $ */
/*09.01.2018	07.10.2000	Белых А.И.	kasothr.c
Распечатка списка кассовых ордеров
*/
#include        <errno.h>
#include        "buhg.h"
#define		KOLSTRHAP 9 //Количество строк в шапке

void	sapkasord(short,short,short,int,const char *zagolov,FILE*);
void   hgrkd(FILE *ff);
void kasothr_ss(int metka,short dd,short md,short gd,int *nomlist,int *kolstr,double itpri,double itras,FILE *ffva,FILE *ffok,FILE *ff_prot);
void kasothr_endday(double itpri,double itras,double isumd,double saldon,FILE *ffva);
void kasothr_sapreestr(int *nomlist,int *kolstr,FILE *ff);
int kasothr_shet(int *kolstr,FILE *ff);
void kasothr_start_reestr(const char *dat_start,const char *dat_end,const char *vpr,const char *kassa,const char *kodop,const char *shet,int *nomlist,int *kolstr,FILE *ff);

extern int kol_strok_na_liste;

int kasothr(const char *dat_start,//Дата и время начала
const char *dat_end, //Дата и время конца
const char *kassa,const char *kodop,const char *shet,const char *vpr,const char *kod_kontr,
class spis_oth *oth)
{
short dn=0,mn=0,gn=0;
short hasn=0,minn=0,sekn=0;
short dk=0,mk=0,gk=0;
short hask=23,mink=59,sekk=59;

char imaf_prot[64];
char		imaf[645],imaf1[64],imafok[64],imaf2[64];
SQL_str		row,row1;
int		kolstr,kolstr1;
char		strsql[1024];
time_t		tmm;
char		vpr1[4];
short		dd,md,gd;
short		d,m,g;
short		tipz;
double		sumd=0.,sumpod=0.;
double		isumd=0.,isumpod=0.;
class iceb_tu_str fio("");
double		saldon=0.; //Сальдo на начало периода
double		saldoden; //Сальдо на начало дня
double		itpri,itras;
int		kollist=0;//Количество листов с начала года
int		kolstrok=0;
class iceb_tu_str nomdz("");
int		koldn=0;
double          suma_p=0.;
double          suma_r=0.;
class iceb_tu_str vrem_start("");
class iceb_tu_str vrem_end("");
double itogp=0.;
double itogr=0.;
class iceb_tu_str kod_edr("");
double kolih_prih_dok=0; //Количество приходных документов
double kolih_rash_dok=0; //Количество расходных документов

rsdatp(&dn,&mn,&gn,dat_start,&dk,&mk,&gk,dat_end);

if(polen(dat_start,&vrem_start,2,' ') == 0)
 rstime(&hasn,&minn,&sekn,vrem_start.ravno());

if(polen(dat_end,&vrem_end,2,' ') == 0)
 rstime(&hask,&mink,&sekk,vrem_end.ravno());

SQLCURSOR curr;
time(&tmm);
//bf=localtime(&tmm);

printw("%d.%d.%d%s => %d.%d.%d%s\n",
dn,mn,gn,gettext("г."),
dk,mk,gk,gettext("г."));
GDITE();

//Читаем код
sprintf(strsql,"select kod from Kontragent where kodkon='00'");
if(readkey(strsql,&row,&curr) == 1)
 kod_edr.new_plus(row[0]);

class iceb_fopen reestr;
sprintf(imaf,"kassa%d.lst",getpid());

if(reestr.start(imaf,"w") != 0)
 return(1);

class iceb_fopen prot;
sprintf(imaf_prot,"kassapr%d.lst",getpid());
if(prot.start(imaf_prot,"w") != 0)
 return(1);
 
class iceb_fopen vlkk;
sprintf(imaf1,"kasg%d.lst",getpid());

if(vlkk.start(imaf1,"w") != 0)
 return(1);

class iceb_fopen zvitk;
 
sprintf(imafok,"kasok%d.lst",getpid());
if(zvitk.start(imafok,"w") != 0)
 return(1);
 
class iceb_fopen gurr;

sprintf(imaf2,"kassf%d.lst",getpid());
if(gurr.start(imaf2,"w") != 0)
 return(1);
 
fprintf(gurr.ff,"\
                                     Додаток 4\n\
                                     до Положення про ведення касових операцій\n\
                                     у національній валюті в Україні\n\
                                     (пункт 33 розділу III)\n\
                                                         Типова форма N КО-3a\n");
fprintf(gurr.ff,"\n\
%s\n\
(найменування підприємства/установи/організації)\n\n\
Ідентифікаційний код в Єдиному державному реєстрі підприємств та організацій України %s\n",
iceb_t_get_pnk("00",0),kod_edr.ravno());  

fprintf(gurr.ff,"\n%30s %s\n","","ЖУРНАЛ");
fprintf(gurr.ff,"%7s %s\n","","реєстрації прибуткових і видаткових касових документів");

zagolov("",dat_start,dat_end,"",gurr.ff);
  

//Вычисляем сальдо на начало периода
saldon=0.;

sprintf(strsql,"select kassa,tp,datd,nomd,suma from Kasord1 where \
datd >= '%04d-01-01' and datd < '%04d-%d-%d' and datp <> '0000-00-00' order by datd,nomd asc",
gn,gn,mn,dn);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
 
d=m=g=dd=md=gd=0;
int nomlist_reestr=0;
int kolstr_reestr=0;


kollist=1; 
kolstrok=KOLSTRHAP;
kolstr1=0;
int koldok=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(kassa,row[0],0,0) != 0)
    continue;
  rsdat(&dd,&md,&gd,row[2],2);

  if(SRAV("000",row[3],0) == 0)
   {
    if(gd == gn)
     {
      saldon+=atof(row[4]);
     }
    continue;
   }

  if(row[1][0] == '0')
   {
    class iceb_tu_str repl;

    sprintf(strsql,"%s:%s",gettext("Дата документа"),iceb_tu_datzap(row[2]));
    repl.plus(strsql);

    sprintf(strsql,"%s tp %s Каsord1 %s !!!",gettext("Колонка"),gettext("в таблице"),gettext("равна нолю"));
    repl.ps_plus(strsql);

    iceb_t_soob(repl.ravno());
    continue;
   }

  if(sravmydat(d,m,g,dd,md,gd) != 0)
   {
    if(prot.ff != NULL)
     fprintf(prot.ff,"Сальдо на %d.%d.%d=%.2f\n",dd,md,gd,saldon);
    if(d != 0)
     {
      /*Вывод шапки так как поменялься день*/
      kasothr_ss(1,dd, md, gd,&kollist,&kolstrok,0.,0.,NULL,NULL,prot.ff);
      if(d != dd)
        kasothr_ss(0,dd, md, gd,&kollist,&kolstrok,0.,0.,NULL,NULL,prot.ff);/*строка сальдо*/
     
     }
    else
      kasothr_ss(0,dd, md, gd,&kollist,&kolstrok,0.,0.,NULL,NULL,prot.ff); /*остаток на начало дня*/
  
    kasothr_start_reestr(dat_start,dat_end,vpr,kassa,kodop,shet,&nomlist_reestr,&kolstr_reestr,NULL);
     
    d=dd; m=md; g=gd;
   }      

  if(vpr[0] != '\0')
   {
    if(vpr[0] == '+' && row[1][0] == '2')
      continue;
    if(vpr[0] == '-' && row[1][0] == '1')
      continue;
   }

  if(SRAV(nomdz.ravno(),row[3],0) != 0)
   {
    koldok++;
    kasothr_ss(0,dd, md, gd,&kollist,&kolstrok,0.,0.,NULL,NULL,prot.ff);

    if(kasothr_shet(&kolstr_reestr,NULL) == 1)
      kasothr_sapreestr(&nomlist_reestr,&kolstr_reestr,NULL);

    if(prot.ff != NULL)
     fprintf(prot.ff,"%s %s %s %s %s kolstrok=%d\n",row[0],row[1],iceb_tu_datzap(row[2]),row[3],row[4],kolstrok);

    nomdz.new_plus(row[3]);
   }    



  if(row[1][0] == '1')
   {
    saldon+=atof(row[4]);
   }
  else
   {
    saldon-=atof(row[4]);
   }


 }

if(prot.ff != NULL)
 {
  fprintf(prot.ff,"Сальдо на начало периода=%.2f\n",saldon);
  fprintf(prot.ff,"\n\n===Количество строк=%d Номер листа =%d\n",kolstrok,kollist);
 }
saldoden=saldon;

printw("%s %02d.%02d.%d :%.2f\n",
gettext("Сальдо на"),dn,mn,gn,saldon);
refresh();


struct  tm      bf;
time_t time_start=0;
time_t time_end=0;

bf.tm_mday=(int)dn;
bf.tm_mon=(int)(mn-1);
bf.tm_year=(int)(gn-1900);
bf.tm_hour=(int)hasn;
bf.tm_min=(int)minn;
bf.tm_sec=sekn;
    
time_start=mktime(&bf);

bf.tm_mday=(int)dk;
bf.tm_mon=(int)(mk-1);
bf.tm_year=(int)(gk-1900);
bf.tm_hour=(int)hask;
bf.tm_min=(int)mink;
bf.tm_sec=sekk;

time_end=mktime(&bf);

sprintf(strsql,"select kassa,tp,datd,nomd,shetk,kodop,fio,osnov,dopol,nb,vrem \
from Kasord where datd >= '%04d-%d-%d' and datd <= '%04d-%d-%d' and podt=1 \
order by datd,tp,nomd asc",gn,mn,dn,gk,mk,dk);
 
//printw("%s-%s\n",__FUNCTION__,strsql);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(-1);
 }
if(kolstr <= 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  fprintf(reestr.ff,"%s\n",gettext("Не найдено ни одной записи !"));
  return(1);
 }

hgrkd(gurr.ff);
kolstr_reestr=0;

kasothr_start_reestr(dat_start,dat_end,vpr,kassa,kodop,shet,&nomlist_reestr,&kolstr_reestr,reestr.ff);

d=m=g=dd=md=gd=0;
class SQLCURSOR cur1;

itpri=itras=isumd=isumpod=0.;
kolstr1=0;
if(koldok > 0)
  kollist+=1;

int metka_sap=0;

kolstrok=KOLSTRHAP;
int kollist_nah=kollist;
int nom_zap=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  
  if(row[1][0] == '0')
   {
    class iceb_tu_str repl;

    sprintf(strsql,"%s:%s",gettext("Дата документа"),iceb_tu_datzap(row[2]));
    repl.plus(strsql);

    sprintf(strsql,"%s tp %s Каsord1 %s !!!",gettext("Колонка"),gettext("в таблице"),gettext("равна нолю"));
    repl.ps_plus(strsql);

    iceb_t_soob(repl.ravno());
    continue;
   }

  if(proverka(kassa,row[0],0,0) != 0)
    continue;
  if(proverka(kodop,row[5],0,0) != 0)
    continue;

  rsdat(&dd,&md,&gd,row[2],2);

  tipz=atoi(row[1]);

  if(vrem_start.getdlinna() > 1 || vrem_end.getdlinna() > 1)
   {
    if(atoi(row[10]) < time_start || atoi(row[10]) > time_end)
     {
      sumkasor(row[0],tipz,row[3],gd,&sumd,&sumpod);
      if(tipz == 2)
       sumd*=-1;    
      saldoden+=sumd;

      if(atoi(row[10]) < time_start)
        saldon+=sumd;
      continue;     
     }
   }

  if(metka_sap == 0) /*Нужно взять дату первого документа для первой шапки*/
   {
    sapkasord(dd,md,gd,kollist,gettext("Вкладной лист кассовой книги"),vlkk.ff);
    sapkasord(dd,md,gd,kollist,gettext("Отчёт кассира"),zvitk.ff);
    metka_sap++;
   }  

  if(kod_kontr[0] != '\0')
   {
    sprintf(strsql,"select kontr from Kasord1 where kassa=%s and god=%d and tp=%s and nomd='%s'",
    row[0],gd,row[1],row[3]);

    if(cur1.make_cursor(&bd,strsql) < 0)
      msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    int metka_kontr=0;
    while(cur1.read_cursor(&row1) != 0)
     {
      if(proverka(kod_kontr,row1[0],0,0) == 0)
       {
        metka_kontr=1;
        break;
        
       } 
     }
    if(metka_kontr == 0)
     continue;
   }
  
  
  if(sravmydat(d,m,g,dd,md,gd) != 0)
   {
    if(d != 0)
     {
      /*Выводим шапку так как поменялся день*/
      kasothr_ss(1,dd, md, gd,&kollist,&kolstrok,itpri,itras,vlkk.ff,zvitk.ff,prot.ff);
      if(d != dd)
       {
        kasothr_ss(0,dd, md, gd,&kollist,&kolstrok,itpri,itras,vlkk.ff,zvitk.ff,prot.ff);
        fprintf(vlkk.ff,"%*s:%10.2f\n",
        iceb_tu_kolbait(50,gettext("Сальдо")),gettext("Сальдо"),isumd+saldon);
        fprintf(zvitk.ff,"%*s:%10.2f\n",
        iceb_tu_kolbait(50,gettext("Сальдо")),gettext("Сальдо"),isumd+saldon);
       }      
     }

    if(d == 0)    
     {
      if(SRAV("000",row[3],0) == 0)
       {
        sumkasor(row[0],tipz,row[3],gd,&sumd,&sumpod);
        if(tipz == 2)
          sumd*=-1;

        saldoden+=sumd;
        saldon+=sumd;
       }

      kasothr_ss(0,dd, md, gd,&kollist,&kolstrok,itpri,itras,vlkk.ff,zvitk.ff,prot.ff);
      fprintf(vlkk.ff,"%*s:%10.2f\n",iceb_tu_kolbait(50,gettext("Остаток на начало дня")),gettext("Остаток на начало дня"),saldoden);
      fprintf(zvitk.ff,"%*s:%10.2f\n",iceb_tu_kolbait(50,gettext("Остаток на начало дня")),gettext("Остаток на начало дня"),saldoden);
     }

    d=dd; m=md; g=gd;
    if(SRAV("000",row[3],0) == 0)
      continue;
   }      

  
  
  if(vpr[0] != '\0')
   {
    if(vpr[0] == '+' && tipz == 2)
      continue;
    if(vpr[0] == '-' && tipz == 1)
      continue;
   }

  if(proverka(shet,row[4],0,0) != 0)
    continue;
 
 
  memset(vpr1,'\0',sizeof(vpr1));
  if(tipz == 1)
   vpr1[0]='+';
  if(tipz == 2)
   vpr1[0]='-';
  rsdat(&dd,&md,&gd,row[2],2);
  
  fio.new_plus(row[6]);
  if(fio.ravno()[0] == '\0')
   {
    sprintf(strsql,"select kontr from Kasord1 where kassa=%s and \
god=%d and tp=%d and nomd='%s'",row[0],gd,tipz,row[3]);
    if(sql_readkey(&bd,strsql,&row1,&curr) >= 1)
     {
       /*Читаем наименование контрагента*/
      sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row1[0]);
      if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
        fio.new_plus(row1[0]);
     }
   }

  sumkasor(row[0],tipz,row[3],gd,&sumd,&sumpod);

  if(tipz == 2)
   {
    suma_r+=sumd;

    fprintf(gurr.ff,"%5d %42s %02d.%02d.%d %-*s %10.2f %-*.*s\n",
    ++nom_zap," ",dd,md,gd,
    iceb_tu_kolbait(8,row[3]),row[3],
    sumd,
    iceb_tu_kolbait(10,row[7]),iceb_tu_kolbait(10,row[7]),row[7]);

    for(int ii=10; ii < iceb_tu_strlen(row[7]); ii+=10)
     fprintf(gurr.ff,"%79s %-*.*s\n",
     "",
     iceb_tu_kolbait(10,iceb_tu_adrsimv(ii,row[7])),
     iceb_tu_kolbait(10,iceb_tu_adrsimv(ii,row[7])),
     iceb_tu_adrsimv(ii,row[7]));

    sumd*=-1;
    itras+=sumd;
    kolih_rash_dok++;
   } 
  else
   {
    suma_p+=sumd;

    fprintf(gurr.ff,"%5d %02d.%02d.%d %-*s %10.2f %-*.*s\n",
    ++nom_zap,
    dd,md,gd,
    iceb_tu_kolbait(8,row[3]),row[3],
    sumd,
    iceb_tu_kolbait(10,row[7]),iceb_tu_kolbait(10,row[7]),row[7]);

    for(int ii=10; ii < iceb_tu_strlen(row[7]); ii+=10)
     fprintf(gurr.ff,"%36s %-*.*s\n",
     "",
     iceb_tu_kolbait(10,iceb_tu_adrsimv(ii,row[7])),
     iceb_tu_kolbait(10,iceb_tu_adrsimv(ii,row[7])),
     iceb_tu_adrsimv(ii,row[7]));

    itpri+=sumd;
    kolih_prih_dok++;
   }
  saldoden+=sumd;    
  isumd+=sumd;
  isumpod+=sumpod;  

  if(tipz == 1)
   itogp+=sumd;

  if(tipz == 2)
   itogr+=sumd;
   
  if(kasothr_shet(&kolstr_reestr,reestr.ff) == 1)
    kasothr_sapreestr(&nomlist_reestr,&kolstr_reestr,reestr.ff);

  fprintf(reestr.ff,"%-5s %-3s %-*s %02d.%02d.%d %10.2f %-*s %-*.*s %s %s\n",
  row[0],
  vpr1,
  iceb_tu_kolbait(10,row[3]),row[3],
  dd,md,gd,sumd,
  iceb_tu_kolbait(13,row[9]),row[9],
  iceb_tu_kolbait(30,fio.ravno()),iceb_tu_kolbait(30,fio.ravno()),fio.ravno(),
  row[7],
  row[8]);


  kasothr_ss(0,dd, md, gd,&kollist,&kolstrok,itpri,itras,vlkk.ff,zvitk.ff,prot.ff);
   
  printw("%s %s %-*s %-*s %10.2f %s\n",
  vpr1,
  iceb_tu_datzap(row[2]),
  iceb_tu_kolbait(7,row[3]),
  row[3],
  iceb_tu_kolbait(6,row[5]),
  row[5],
  sumd,
  iceb_tu_vremzap(row[10]));
  
  refresh();
      
  if(prot.ff != NULL)
   fprintf(prot.ff,"%s %s %s %s %s %s Дата записи-%s kolstrok=%d\n",row[0],row[1],row[2],row[3],row[4],row[5],iceb_tu_vremzap(row[10]),kolstrok);

  if(tipz == 1)
   {
    fprintf(vlkk.ff,"%-*s %-*.*s %-10.10s %10.2f\n",
    iceb_tu_kolbait(8,row[3]),row[3],
    iceb_tu_kolbait(30,fio.ravno()),iceb_tu_kolbait(30,fio.ravno()),fio.ravno(),
    row[4],sumd);

    fprintf(zvitk.ff,"%-*s %-*.*s %-*.*s %10.2f\n",
    iceb_tu_kolbait(8,row[3]),row[3],
    iceb_tu_kolbait(30,fio.ravno()),iceb_tu_kolbait(30,fio.ravno()),fio.ravno(),
    iceb_tu_kolbait(10,row[4]),iceb_tu_kolbait(10,row[4]),row[4],
    sumd);
   }
  if(tipz == 2)
   {
    fprintf(vlkk.ff,"%-*s %-*.*s %-*.*s %10s %10.2f\n",
    iceb_tu_kolbait(8,row[3]),row[3],
    iceb_tu_kolbait(30,fio.ravno()),iceb_tu_kolbait(30,fio.ravno()),fio.ravno(),
    iceb_tu_kolbait(10,row[4]),iceb_tu_kolbait(10,row[4]),row[4],
    " ",sumd*-1);

    fprintf(zvitk.ff,"%-*s %-*.*s %-*.*s %10s %10.2f\n",
    iceb_tu_kolbait(8,row[3]),row[3],
    iceb_tu_kolbait(30,fio.ravno()),iceb_tu_kolbait(30,fio.ravno()),fio.ravno(),
    iceb_tu_kolbait(10,row[4]),iceb_tu_kolbait(10,row[4]),row[4],
    " ",sumd*-1);
   }
 } 

fprintf(reestr.ff,"\
--------------------------------------------------------------------------------------------------------------------\n");
fprintf(reestr.ff,"%*s:%10.2f\n",iceb_tu_kolbait(31,gettext("Итого приход")),gettext("Итого приход"),itogp);
fprintf(reestr.ff,"%*s:%10.2f\n",iceb_tu_kolbait(31,gettext("Итого расход")),gettext("Итого расход"),itogr);

kasothr_endday(itpri,itras,isumd,saldon,vlkk.ff);

kasothr_endday(itpri,itras,isumd,saldon,zvitk.ff);

koldn=period(dn,mn,gn,dk,mk,gk,0);

if(koldn == 1)
 {
  fprintf(vlkk.ff,"%s:%d\n",gettext("Количество листов"),kollist-kollist_nah+1);
  fprintf(zvitk.ff,"%s:%d\n",gettext("Количество листов"),kollist-kollist_nah+1);
 }
else
 {
  fprintf(vlkk.ff,"%s %02d.%02d.%d%s %s %02d.%02d.%d%s\n",
  gettext("С"),
  dn,mn,gn,
  gettext("г."),
  gettext("по"),
  dk,mk,gk,
  gettext("г."));

  fprintf(vlkk.ff,"%s %d %s.\n",
  gettext("распечатано"),
  kollist,
  gettext("листов кассовой книги"));

  fprintf(zvitk.ff,"%s %02d.%02d.%d%s %s %02d.%02d.%d%s\n",
  gettext("С"),
  dn,mn,gn,
  gettext("г."),
  gettext("по"),
  dk,mk,gk,
  gettext("г."));

  fprintf(zvitk.ff,"%s %d %s.\n",
  gettext("распечатано"),
  kollist,
  gettext("листов кассовой книги"));
 }  
class iceb_tu_str propis_prih("");
class iceb_tu_str propis_rash("");

preobr(kolih_prih_dok,&propis_prih,2);
preobr(kolih_rash_dok,&propis_rash,2);

fprintf(vlkk.ff,"\n%*s_________________\n",
iceb_tu_kolbait(10,gettext("Кассир")),
gettext("Кассир"));
fprintf(vlkk.ff,"%13s(підпис)\n\n","");

fprintf(vlkk.ff,"%s\n","Залишок у касовій книзі перевірив і документи в кількості");

fprintf(vlkk.ff,"%s %s\n",propis_prih.ravno(),"прибуткових та");
fprintf(vlkk.ff,"(словами)\n");
fprintf(vlkk.ff,"%s %s\n",propis_rash.ravno(),"видаткових одержав");
fprintf(vlkk.ff,"(словами)\n");

fprintf(vlkk.ff,"\n%*s_______________\n",
iceb_tu_kolbait(10,gettext("Бухгалтер")),
gettext("Бухгалтер"));
fprintf(vlkk.ff,"%13s(підпис)\n","");

fprintf(zvitk.ff,"\n%*s_________________\n",
iceb_tu_kolbait(10,gettext("Кассир")),
gettext("Кассир"));
fprintf(zvitk.ff,"%13s(підпис)\n\n","");

fprintf(zvitk.ff,"%s\n","Залишок у касовій книзі перевірив і документи в кількості");

fprintf(zvitk.ff,"%s %s\n",propis_prih.ravno(),"прибуткових та");
fprintf(zvitk.ff,"(словами)\n");
fprintf(zvitk.ff,"%s %s\n",propis_rash.ravno(),"видаткових одержав");
fprintf(zvitk.ff,"(словами)\n");


fprintf(zvitk.ff,"\n%*s_______________\n",
iceb_tu_kolbait(10,gettext("Бухгалтер")),
gettext("Бухгалтер"));
fprintf(zvitk.ff,"%13s(підпис)\n\n","");

fprintf(gurr.ff,"\
-----------------------------------------------------------------------------------\n");
fprintf(gurr.ff,"%*s:%10.2f %31s %10.2f\n",iceb_tu_kolbait(25,gettext("Итого")),gettext("Итого"),suma_p," ",suma_r);

podpis(reestr.ff);

podpis(vlkk.ff);
podpis(zvitk.ff);

podpis(gurr.ff);


reestr.end();
vlkk.end();

zvitk.end();
gurr.end();

prot.end();
printw_vr(tmm);


OSTANOV();

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Реестр кассовых ордеров"));

oth->spis_imaf.plus(imaf1);
oth->spis_naim.plus(gettext("Вкладной лист кассовой книги"));

oth->spis_imaf.plus(imafok);
oth->spis_naim.plus(gettext("Отчёт кассира"));

oth->spis_imaf.plus(imaf2);
sprintf(strsql,"%s %s",gettext("ЖУРНАЛ"),
gettext("регистрации приходных и расходных кассовых документов"));

oth->spis_naim.plus(strsql);

oth->spis_imaf.plus(imaf_prot);
oth->spis_naim.plus("Протокол хода расчёта");

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),1);
return(0);
}

/*******/
/*Шапка*/
/*******/

void sapkasord(short dd,short md,short gd,int nomlist,const char *zagol,FILE *ffva)
{

class iceb_tu_str mesqc("");
mesc(md,1,&mesqc);

fprintf(ffva,"%s\n",zagol);

fprintf(ffva,"%s \"%02d\" %s %d%s                     %s %d\n",
"Каса за",
dd,mesqc.ravno(),gd,
"р.",
"Сторінка",nomlist);

fprintf(ffva,"\
-------------------------------------------------------------------------\n");
fprintf(ffva,"\
 Номер  |  Від кого отримано чи кому   |  Номер   |  Надход- | Видаток  |\n\
докумен-|         видано               |кореспон- |  ження   |          |\n\
  та    |                              |дуючого   |          |          |\n\
        |                              |рахунку,  |          |          |\n\
        |                              |субрахунку|          |          |\n");
fprintf(ffva,"\
-------------------------------------------------------------------------\n\
   1    |               2              |    3     |    4     |     5    |\n\
-------------------------------------------------------------------------\n");

/*
12345678 123456789012345678901234567890 1234567890 1234567890 1234567890
*/

}
/***************************/
/*Шапка журнала регистрации кассовых документов*/
/***********************************************/

void   hgrkd(FILE *ff)
{
fprintf(ff,"\
------------------------------------------------------------------------------------------\n\
  N  |Прибутковий докум. |          |          |Видатковий документ|          |          |\n\
 з/п ---------------------  Сума    |Примітки  |--------------------  Сума    |Примітки  |\n\
     |  дата    | номер  |          |          |  дата    | номер  |          |          |\n\
------------------------------------------------------------------------------------------\n\
  1  |    2     |   3    |    4     |     5    |     6    |   7    |    8     |    9     |\n\
------------------------------------------------------------------------------------------\n");


}
/*************************/
/*Счётчик строк*/
/*******************/
void kasothr_ss(int metka, //0-счётчик применять 1-не применять
short dd,short md,short gd,int *nomlist,int *kolstr,double itpri,double itras,FILE *ffva,FILE *ffok,FILE *ff_prot)
{
if(metka == 0)
 {
  *kolstr+=1;
  if(*kolstr <= kol_strok_na_liste-1)
   return;
  *kolstr=1;
 }
else
 *kolstr=0;
 
*nomlist+=1;
*kolstr+=KOLSTRHAP;
if(ffva != NULL)
 {
  fprintf(ffva,"%*s:%10.2f %10.2f\n",
  iceb_tu_kolbait(50,gettext("Перенос")),gettext("Перенос"),itpri,itras*-1);
  fprintf(ffva,"\f");
  sapkasord(dd,md,gd,*nomlist,gettext("Вкладной лист кассовой книги"),ffva);
 }
if(ffok != NULL)
 {
  fprintf(ffok,"%*s:%10.2f %10.2f\n",
  iceb_tu_kolbait(50,gettext("Перенос")),gettext("Перенос"),itpri,itras*-1);
  fprintf(ffok,"\f");
  sapkasord(dd,md,gd,*nomlist,gettext("Отчет кассира"),ffok);
 }
if(ff_prot != NULL)
 fprintf(ff_prot,"Вывели шапку kolstr=%d nomlist=%d\n",*kolstr,*nomlist); 
}
/**********************************************/
/*концовка за день*/
/************************************************/
void kasothr_endday(double itpri,double itras,double isumd,double saldon,FILE *ffva)
{

fprintf(ffva,"\
-------------------------------------------------------------------------\n");

fprintf(ffva,"%*s|%10.2f|%10.2f|\n",iceb_tu_kolbait(50,"Разом за день"),"Разом за день",itpri,itras*-1);
fprintf(ffva,"%50s|---------------------|\n"," ");
fprintf(ffva,"%*s|%10.2f|\n",iceb_tu_kolbait(50,"Залишок на кінець дня,"),"Залишок на кінець дня,",isumd+saldon);
fprintf(ffva,"%50s|----------|\n"," ");
fprintf(ffva,"%*s|%10s|\n",iceb_tu_kolbait(50,"у тому числі на зарплату"),"у тому числі на зарплату"," ");
fprintf(ffva,"%50s|----------|\n"," ");
}

/*************************************************************/
/*Шапка реестра */
/*************************************************************/

void kasothr_sapreestr(int *nomlist,int *kolstr,FILE *ff)
{

*nomlist+=1;
*kolstr+=4;

if(ff != NULL)
 {
  fprintf(ff,"%95s %s N%d\n","",gettext("Страница"),*nomlist);

  fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------\n");
  fprintf(ff,gettext("\
Касса|+/-|Номер док.|Дата док. |Сумм док.|Номер бланка |            Фамилия           |Операция \n"));
  fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------\n");
 }
}
/******************************/
/*счётчик строк*/
/********************************/
int kasothr_shet(int *kolstr,FILE *ff)
{
*kolstr+=1;
if(*kolstr <= kol_strok_na_liste)
 return(0);

if(ff != NULL)
  fprintf(ff,"\f");

*kolstr=1;
return(1);
}
/********************************************/
/*начальная страница рeестра*/
/**************************************/
void kasothr_start_reestr(const char *dat_start,
const char *dat_end,
const char *vpr,const char *kassa,const char *kodop,const char *shet,int *nomlist,int *kolstr,FILE *ff)
{
class iceb_tu_str nadpis("");

if(vpr[0] == '\0')
  nadpis.new_plus(gettext("Реестр кассовых ордеров"));
if(vpr[0] == '+')
  nadpis.new_plus(gettext("Реестр приходних кассовых ордеров"));
if(vpr[0] == '-')
  nadpis.new_plus(gettext("Реестр расходных кассовых ордеров"));
  
*kolstr=5;
iceb_t_zagolov(nadpis.ravno(),dat_start,dat_end,ff);


if(kassa[0] != '\0')
 {
  *kolstr+=1;
  if(ff != NULL)
    fprintf(ff,"%s:%s\n",gettext("Касса"),kassa);
 }
if(kodop[0] != '\0')
 {
  *kolstr+=1;
  if(ff != NULL)
    fprintf(ff,"%s:%s\n",gettext("Код операции"),kodop);
 }
if(shet[0] != '\0')
 {
  *kolstr+=1;
  if(ff != NULL)
    fprintf(ff,"%s:%s\n",gettext("Счёт"),shet);
 }
kasothr_sapreestr(nomlist,kolstr,ff);
}
