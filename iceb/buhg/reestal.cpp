/* $Id: reestal.c,v 5.35 2013/08/13 05:49:54 sasa Exp $ */
/*22.03.2018    06.01.1993      Белых А.И.      reestral.c
Подпрограмма получения реестра почтовых переводов работников
платящих алименты
*/
#include        <errno.h>
#include        "buhg.h"

void            restsp(int *kolstrok,FILE *ff);
void reestal_sap(int *kolstr,FILE *ff);
void            konre(double razom,double pdv,double vsego,double,FILE *ff);
void reestal_ss(int *kolstrok,FILE *ff);

extern short    mmm,ggg;
extern double   okrg;   /*Округление*/
extern short    kodpn;
extern double          nmpo; /*Вычисленная величина необлагаемого дохода*/
extern short    *kodmp;   /*Коды материальной помощи*/
extern int kol_strok_na_liste;

int reestal(class spis_oth *oth)
{
short           i;
FILE            *f1,*f2;
double          proc,prsb;
short           kodalim=0;   /*Код алиментов*/
short		kodps=0;     /*Код почтового сбора*/
int  tabn=0,tabn1=0;
short           pnom;
class iceb_tu_str adr("");
double          sumps;  /*Сумма почт. сбора с НДС*/
double          sumalim; /*Сумма почт. перевода*/
class iceb_tu_str br("");
class iceb_tu_str fam(""),famr("");
double          spr,ssb,osym;
char            imaf[64],imaf1[64];
double          sal; //Общая сумма алиментов
double          sps; //Сумма почтового сбора с НДС
short           mpr=0;   /*Метка пропуска строки*/
SQL_str         row;
char		strsql[1024];
static class iceb_tu_str dat1("");
short		mr,gr;
SQLCURSOR curr;
if(dat1.ravno()[0] == '\0')
 {
  sprintf(strsql,"%d.%d",mmm,ggg);
  dat1.new_plus(strsql);
 }
VVOD VVOD1(2);
VVOD1.VVOD_SPISOK_add_ZAG(gettext("Расчёт ведомости почтовых переводов алиментов"));
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите дату (м.г)."));

naz:; 
clear();

helstr(LINES-1,0,
"F1",gettext("помощь"),
"F5",gettext("настройка"),
" F10 ",gettext("выход"),NULL);

if((i=vvod1(&dat1,8,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
  return(1);

switch(i)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("zarp4_4.txt");
   goto naz;
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return(1);

  case FK5: /*настройка*/
    iceb_redfil("zaralim.alx",0);
    goto naz;

  case ENTER: /*РАСЧЁТ*/
    break;     
  default:
    goto naz;
 }
 


 
if(rsdat1(&mr,&gr,dat1.ravno()) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата !"));
  goto naz;
 }

clear();
GDITE();
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;

double nds1=iceb_t_pnds();


sprintf(strsql,"select str from Alx where fil='zaralim.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zaralim.alx");
  iceb_t_soob(strsql);
  return(1);
 }


sprintf(imaf,"reestr%d.lst",mr);
if((f1 = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

sprintf(imaf1,"vua%d.lst",mr);
if((f2 = fopen(imaf1,"w")) == NULL)
 {
  error_op_nfil(imaf1,errno,"");
  return(1);
 }

fprintf(f2,"%s\n\n%s\n",
gettext("Ведомость удержания алиментов."),iceb_t_get_pnk("00",0));

int kolstrok=0;
restsp(&kolstrok,f1);

prsb=0.;
tabn1=0;
sal=sps=osym=spr=ssb=proc=0.;
tabn=0;
pnom=0;

double razom=0.,pdv=0.,vsego=0.;
double nds=0.;
double sps_b_nds=0.; //Сумма почтового сбора без НДС
double isps_b_nds=0.; //Итоговая сумма почтового сбора без НДС

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  proc=prsb=sumps=sumalim=0.;
  if(SRAV(row_alx[0],"***",1) == 0)  //Метка нового документа
   {
    konre(razom,pdv,vsego,isps_b_nds,f1);
    pnom=0;
    osym=spr=ssb=proc=0.;
    razom=pdv=vsego=0.;
    isps_b_nds=0.;

    fprintf(f1,"\f");
    restsp(&kolstrok,f1);

    continue;
   }

  if(polen(row_alx[0],&br,1,'|') != 0)
    continue;

  if(SRAV("Код удержания алиментов",br.ravno(),0) == 0)
   {
    polen(row_alx[0],&kodalim,2,'|');
//    kodalim=(short)atoi(br);
    continue;
   }

  if(SRAV("Код удержания почтового сбора",br.ravno(),0) == 0)
   {
    polen(row_alx[0],&kodps,2,'|');
//    kodps=(short)atoi(br);
    continue;
   }

  tabn=br.ravno_atoi();
  if(tabn != 0)
   {
    //Проверяем табельный номер
    sprintf(strsql,"select tabn from Kartb where tabn=%d",tabn);
    if(readkey(strsql,&row,&curr) != 1)
     {
      sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),tabn);
      iceb_t_soob(strsql);
      continue;
     }     
   }
  if(tabn != 0 && tabn != tabn1)
   {
    tabn1=tabn;
   }

  if(tabn != 0)
    mpr=0;

  polen(row_alx[0],&proc,2,'|');
//  proc=ATOFM(br);

  polen(row_alx[0],&prsb,3,'|');
//  prsb=ATOFM(br);

  /*Получает на месте*/
  if(tabn != 0 && prsb == 0.)
    mpr=1;

  if(mpr == 1)
    continue;
  polen(row_alx[0],&fam,4,'|');

  polen(row_alx[0],&adr,5,'|');
  if(tabn != 0)
   {
    
    if(kodalim == 0)
     {
      iceb_t_soob(gettext("В настроечном файле не найден код алиментов !"));
      break;      
     }
    sprintf(strsql,"select fio from Kartb where tabn=%d",tabn);
    if(sql_readkey(&bd,strsql,&row,&curr) != 1)
     {
       beep();
       printw("%s %d !\n",gettext("Не найден табельный номер"),tabn);
       continue;
     }
    
    famr.new_plus(row[0]);
    
    printw("%5ld %-*.*s\n",tabn,iceb_tu_kolbait(20,fam.ravno()),iceb_tu_kolbait(20,fam.ravno()),fam.ravno());
    refresh();
    fprintf(f2,"\
----------------------------------------------------------------\n\
%5d %s -> %s  \n",tabn,famr.ravno(),fam.ravno());

    
    /*Читаем алименты*/
    sumalim=0.;
    sprintf(strsql,"select SUM(suma) from Zarp where datz >= '%d-%d-01' \
and datz <= '%d-%d-31' and tabn=%d and prn='2' \
and knah=%d",gr,mr,gr,mr,tabn,kodalim); 
    if(readkey(strsql,&row,&curr) >= 1)
      if(row[0] != NULL)
       sumalim=atof(row[0])*-1;      

    if(sumalim == 0.)
     {
      tabn=tabn1=0;
      continue;
     }      

    sumps=0.;
    sps_b_nds=0.;
    /*Читаем почтовый сбор*/
    sprintf(strsql,"select SUM(suma) from Zarp where datz >= '%d-%d-01' \
and datz <= '%d-%d-31' and tabn=%d and prn='2'\
and knah=%d",gr,mr,gr,mr,tabn,kodps); 
    if(readkey(strsql,&row,&curr) >= 1)
     {
      if(row[0] != NULL)
       {
        sumps=atof(row[0])*-1;      
        if(sumps != 0.)
         {
          sps_b_nds=sumps-sumps*nds1/(100.+nds1);
          sps_b_nds=okrug(sps_b_nds,0.01);
         }
       }
     }
     

    nds=sumps-sps_b_nds;
    nds=okrug(nds,0.01);

    pdv+=nds;
    razom+=sumalim;
    vsego+=sumalim+sumps;
    
    reestal_ss(&kolstrok,f1);

    fprintf(f1,"%3d %-*.*s %-*.*s %7.2f %8.2f %6.2f\n",
    ++pnom,
    iceb_tu_kolbait(30,adr.ravno()),iceb_tu_kolbait(30,adr.ravno()),adr.ravno(),
    iceb_tu_kolbait(25,fam.ravno()),iceb_tu_kolbait(25,fam.ravno()),fam.ravno(),
    sumalim,sps_b_nds,nds);

    fprintf(f2,"\n\
%*s:%20s (%.2f%%)\n",iceb_tu_kolbait(30,gettext("Алименты")),gettext("Алименты"),prnbr(sumalim),proc);
    fprintf(f2,"\
%*s:%20s\n",iceb_tu_kolbait(30,gettext("Почтовый сбор")),gettext("Почтовый сбор"),prnbr(sumps));

    osym+=sumalim;
    spr+=sumalim;
    sps+=sumps;
    ssb+=sumps;
    sal+=sumalim;
    isps_b_nds+=sps_b_nds;
   }
  else
   if(tabn1 != 0)
    {
     reestal_ss(&kolstrok,f1);
     fprintf(f1,"%3s %-*.*s %-*.*s\n",
     " ",
     iceb_tu_kolbait(30,adr.ravno()),iceb_tu_kolbait(30,adr.ravno()),adr.ravno(),
     iceb_tu_kolbait(25,fam.ravno()),iceb_tu_kolbait(25,fam.ravno()),fam.ravno());
    }
 }
printw("\n-------------------------------------------------------\n\
%s %20s\n",
gettext("Итого алиментов     -"),prnbr(sal));
printw("%s %20s\n\n",
gettext("Итого почтовый сбор -"),prnbr(sps));

fprintf(f2,"-----------------------------------------------------------------\n\
%s %20s\n",
gettext("Итого алиментов     -"),prnbr(sal));
fprintf(f2,"\
%s %20s\n",
gettext("Итого почтовый сбор -"),prnbr(sps));

konre(razom,pdv,vsego,isps_b_nds,f1);


podpis(f1);
podpis(f2);

fclose(f1);
fclose(f2);
OSTANOV();

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Список почтовых переводов."));
oth->spis_imaf.plus(imaf1);
oth->spis_naim.plus(gettext("Ведомость удержания алиментов."));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),1);
 
return(0);
}

/*******/
/*Шапка*/
/*******/
void restsp(int *kolstrok,FILE *ff)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
class iceb_tu_str stroka("");
*kolstrok=0;

sprintf(strsql,"select str from Alx where fil='zar_pp_start.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zar_pp_start.alx");
  iceb_t_soob(strsql);
  return;
 }

SQL_str row;
SQLCURSOR cur;
class iceb_tu_str kod("");
class iceb_tu_str naim_org("");
class iceb_tu_str adres_org("");

sprintf(strsql,"select naikon,adres,kod from Kontragent where kodkon='00'");
if(readkey(strsql,&row,&cur) == 1)
 {
  naim_org.new_plus(row[0]);
  adres_org.new_plus(row[1]);
  kod.new_plus(row[2]);
 }

int nomer_str=0;
class iceb_tu_str stroka1("");
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  stroka.new_plus(row_alx[0]);
  nomer_str++;
  switch(nomer_str)
   {
    case 1:
      iceb_tu_vstav(&stroka,naim_org.ravno(),0,66,1);
      break;

    case 4:
      iceb_tu_vstav(&stroka,kod.ravno(),21,42,2);
      break;

    case 10:
//      sprintf(stroka1,"%s %s",naim_org.ravno(),adres_org.ravno());
      stroka1.new_plus(naim_org.ravno());
      stroka1.plus(" ",adres_org.ravno());
      iceb_tu_vstav(&stroka,stroka1.ravno(),11,119,1);
      break;
   }
  *kolstrok+=1;   
  fprintf(ff,"%s",stroka.ravno());
 }

reestal_sap(kolstrok,ff);

}

/*******/
/*Конец*/
/*******/
void            konre(double razom,double pdv,double vsego,double p_zbir,
FILE *ff)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%60s:%7.2f %8.2f %6.2f\n",
gettext("Итого"),razom,p_zbir,pdv);

class iceb_tu_str stroka("");
class iceb_tu_str stroka1("");

sprintf(strsql,"select str from Alx where fil='zar_pp_end.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zar_pp_end.alx");
  iceb_t_soob(strsql);
  return;
 }

SQL_str row;
SQLCURSOR cur;

class iceb_tu_str fio_ruk("");

iceb_t_poldan("Табельный номер руководителя",&stroka1,"zarnast.alx");

if(stroka1.ravno()[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",stroka1.ravno());
  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
    fio_ruk.new_plus(row[0]);
 } 

class iceb_tu_str fio_buh("");

iceb_t_poldan("Табельный номер бухгалтера",&stroka1,"zarnast.alx");

if(stroka1.ravno()[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",stroka1.ravno());
  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
    fio_buh.new_plus(row[0]);
 } 


int nomer_str=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  stroka.new_plus(row_alx[0]);
  nomer_str++;
  switch(nomer_str)
   {
    case 1:
      preobr(razom,&stroka1,0);
      iceb_tu_vstav(&stroka,stroka1.ravno(),25,77,1);
      break;

    case 2:
      preobr(pdv,&stroka1,0);
      iceb_tu_vstav(&stroka,stroka1.ravno(),25,77,1);
      break;

    case 3:
      preobr(vsego,&stroka1,0);
      iceb_tu_vstav(&stroka,stroka1.ravno(),25,85,1);
      break;

    case 7:
      iceb_tu_vstav(&stroka,fio_ruk.ravno(),20,85,1);
      break;

    case 10:
      iceb_tu_vstav(&stroka,fio_buh.ravno(),20,85,1);
      break;
   }
  fprintf(ff,"%s",stroka.ravno());
 }


}
/*************************************/
/*Шапка*/
/**************************************/
void reestal_sap(int *kolstr,FILE *ff)
{

*kolstr+=5;

fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------\n\
 N |          Куди                |   Кому (найменування    |  Сумма  |Плата за|      |Відмітка|        |        |\n\
   |  (повна адреса одержувача)   |   підприємства або      |переказу|пере-ння| ПДВ  |   про  |   N    |Примітка|\n\
   |                              |   прізвище одержувача)  | (грн)  |без ПДВ |(грн) |  інші  |переказу|        |\n\
   |                              |                         |        | (грн)  |      |послуги |        |        |\n\
-----------------------------------------------------------------------------------------------------------------\n");
}
/*************************************/
/*счетчик строк*/
void reestal_ss(int *kolstrok,FILE *ff)
{
*kolstrok+=1;
if(*kolstrok <= kol_strok_na_liste)
 return;
fprintf(ff,"\f");
*kolstrok=0; 

reestal_sap(kolstrok,ff);
*kolstrok+=1;
}
