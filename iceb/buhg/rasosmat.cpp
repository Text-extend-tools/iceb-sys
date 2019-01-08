/* $Id: rasosmat.c,v 5.35 2013/09/26 09:43:39 sasa Exp $ */
/*15.07.2015	31.05.1999	Белых А.И.	rasosmat.c
Распечатка абсолютного остатка
*/
#include        <errno.h>
#include        "buhg.h"

extern char	*maskmat; /*Массив кодов отмеченных материалов*/

int rasosmat(class spis_oth *oth)
{
struct  tm      *bf;
time_t          tmm;
int		kom,kom1;
short		dp,mp,gp;
short		dn,mn,gn;
short		dk,mk,gk;
char		imaf1[64],imaf2[64];
int		sklad,kodm;
class iceb_tu_str naim("");
FILE		*ff1,*ff2;
double		ost=0.;
short		kgrm,kgrm1;
static class iceb_tu_str skladi("");
static class iceb_tu_str grupi("");
static class iceb_tu_str kodmz("");
static class iceb_tu_str dat1("");
double		itog,itogs;
short		kolskl,ii;
long		kolstr,kolstr1;
SQL_str         row,row1;
char		strsql[512];
class iceb_tu_str nm("");
int		kgr;
class iceb_tu_str naik("");
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);

SQLCURSOR curr;
if(dat1.ravno()[0] == '\0')
 {
  sprintf(strsql,"%d.%d.%d",dt,mt,gt);
  dat1.new_plus(strsql);
 }
VVOD MENU(3);
VVOD VV(0);
VV.VVOD_SPISOK_add_data(skladi.ravno(),128);
VV.VVOD_SPISOK_add_data(grupi.ravno(),128);
VV.VVOD_SPISOK_add_data(kodmz.ravno(),128);
VV.VVOD_SPISOK_add_data(dat1.ravno(),11);

VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка абсолютных остатков по материаллам"));

VV.VVOD_SPISOK_add_MD(gettext("Склад..(,,).........."));
VV.VVOD_SPISOK_add_MD(gettext("Группа.(,,).........."));
VV.VVOD_SPISOK_add_MD(gettext("Код материалла (,,*).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата остатка.(д.м.г)."));


naz:;

clear();


helstr(LINES-1,0,"F2/+",gettext("расчёт"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

skladi.new_plus(VV.data_ravno(0));
grupi.new_plus(VV.data_ravno(1));
kodmz.new_plus(VV.VVOD_SPISOK_return_data(2));
dat1.new_plus(VV.VVOD_SPISOK_return_data(3));
class iceb_tu_str kods("");
class iceb_tu_str naims("");
switch(kom)
 {
  case FK2:
  case PLU:
    break;
  case FK10:
  case ESC:
    return(1);
  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка складов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка материалов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 8 :
      case -1:
       goto naz;

      case 0 :
        if(dirsklad(1,&kods,&naims) == 0)
         VV.data_z_plus(0,kods.ravno()); 
        goto naz;

      case 1 :
        vibrek("Grup",&grupi,&naik);
        VV.VVOD_SPISOK_zapis_data(1,grupi.ravno());
        goto naz;

      case 2 :

        naik.new_plus("");
        if(dirmatr1(&kgr,&kodm,&naik,0,"",0,0,0) == 0)
         {
          VV.data_z_plus(2,kodm);
         }

        clear();
        goto naz;
     }
  case FK4:
    VV.VVOD_clear_data();
    goto naz;
 default:
    goto naz;
 }

if(VV.VVOD_SPISOK_return_data(0)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введён склад !"));
  goto naz;
 }

if(rsdat(&dn,&mn,&gn,dat1.ravno(),1) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата !"));
  goto naz;
 }

clear();
GDITE();

time(&tmm);
bf=localtime(&tmm);

sklad=(int)ATOFM(VV.VVOD_SPISOK_return_data(0));

dp=dk=dn;
mp=mk=mn;
gp=gk=gn;

dpm(&dp,&mp,&gp,2);

sprintf(strsql,"select kodm,kodgr,naimat from Material order by kodgr,naimat,kodm asc");
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одного материалла !"));
  return(1);
 }  
sprintf(imaf1,"ostm%d.lst",getpid());

if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  error_op_nfil(imaf1,errno,"");
  return(1);
 }

sprintf(imaf2,"ostm1_%d.lst",getpid());

if((ff2 = fopen(imaf2,"w")) == NULL)
 {
  error_op_nfil(imaf2,errno,"");
  return(1);
 }

fprintf(ff1,"\x1B\x33%c",28); /*Уменьшаем межстрочный интервал*/

fprintf(ff2,"\x1B\x33%c",28); /*Уменьшаем межстрочный интервал*/

kolskl=pole2(skladi.ravno(),',');
if(kolskl == 0)
 kolskl=1;

fprintf(ff1,"%s\n",iceb_t_get_pnk("00",0));


printw(gettext("Склад %d\n"),sklad);
refresh();

fprintf(ff1,"\n%s %s\n\
%s %d.%d.%d%s  %s %d.%d.%d%s %s: %d:%d\n",
gettext("Абсолютный остаток по складу"),
skladi.ravno(),
gettext("На"),
dn,mn,gn,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(ff2,"\n%s %s\n\
%s %d.%d.%d%s  %s %d.%d.%d%s %s: %d:%d\n",
gettext("Абсолютный остаток по складу"),
skladi.ravno(),
gettext("На"),
dn,mn,gn,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

if(grupi.ravno()[0] != '\0')
 {
  fprintf(ff1,"%s: %s\n",gettext("Группа"),grupi.ravno());
  fprintf(ff2,"%s: %s\n",gettext("Группа"),grupi.ravno());
 }

if(kodmz.ravno()[0] != '\0')
 {
  if(kodmz.ravno()[0] != '*')
   {
    fprintf(ff1,"%s: %s\n",gettext("Код материалла"),kodmz.ravno());
    fprintf(ff2,"%s: %s\n",gettext("Код материалла"),kodmz.ravno());
   }
  else
   {
    fprintf(ff1,"%s: %s\n",gettext("Код материалла"),maskmat);
    fprintf(ff2,"%s: %s\n",gettext("Код материалла"),maskmat);
   }
 }



itog=itogs=0.;
kgrm=kgrm1=0;
kolstr1=0;

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(grupi.ravno(),row[1],0,0) != 0)
    continue;
  if(kodmz.ravno()[0] != '\0')
   {
    if(kodmz.ravno()[0] != '*')
     {
      if(proverka(kodmz.ravno(),row[0],0,0) != 0)
        continue;
      }
    else
     if(maskmat != NULL)
      if(proverka(maskmat,row[0],0,0) != 0)
        continue;
   }

    kgrm=atoi(row[1]);
    if(kgrm1 != kgrm)
     {
      if(kgrm1 != 0)
       {
        printw("%s %10.10g\n",gettext("Итого по группе"),itog);
        GDITE();
        fprintf(ff1,"\
------------------------------------------------------------------------------------------------------------------------------------\n\
%*s %8.8g\n",iceb_tu_kolbait(56,gettext("Итого по группе")),gettext("Итого по группе"),itog);
        fprintf(ff2,"\
-----------------------------------------------------------------\n\
%*s %8.8g\n",iceb_tu_kolbait(56,gettext("Итого по группе")),gettext("Итого по группе"),itog);
        itog=0.;
       }
      sprintf(strsql,"select naik from Grup where kod=%d",
      kgrm);
      if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
       {
        beep();
        printw("%s %d !\n",gettext("Не найдена группа"),kgrm);
        OSTANOV();
        nm.new_plus("");
       }    
      else
        nm.new_plus(row1[0]);
      printw("Група %d %s\n",kgrm,nm.ravno());
      GDITE();
      fprintf(ff1,"\n%s %d %s\n\
------------------------------------------------------------------------------------------------------------------------------------\n",
      gettext("Группа"),kgrm,nm.ravno());
      fprintf(ff2,"\n%s %d %s\n\
-----------------------------------------------------------------\n",
      gettext("Группа"),kgrm,nm.ravno());
      kgrm1=kgrm;
     } 

    naim.new_plus(row[2]);
    kodm=atoi(row[0]);
    
    ost=0.;
    for(ii=1; ii <= kolskl ; ii++)
     {
      if(kolskl > 1)
       {
        polen(skladi.ravno(),&sklad,ii,',');
 //       sklad=(int)ATOFM(bros);
       }
      else
        sklad=(int)ATOFM(skladi.ravno());

      ost+=ostdok1(dp,mp,gp,sklad,kodm);
     }

    if(ost != 0)
     {
      fprintf(ff1,"%-5d %-*s %8.8g|          |          |          |          |          |          |\n",
      kodm,
      iceb_tu_kolbait(50,naim.ravno()),naim.ravno(),
      ost);
      fprintf(ff1,"\
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n");
      fprintf(ff2,"%-5d %-*s %8.8g\n",
      kodm,
      iceb_tu_kolbait(50,naim.ravno()),naim.ravno(),
      ost);
      
      itog+=ost;
      itogs+=ost;
     }
 }
//rewind(ff);
printw("%s:%10.10g\n",gettext("Итого по группе"),itog);
printw("%s:%10.10g\n",gettext("Общий итог"),itogs);
fprintf(ff1,"\
------------------------------------------------------------------------------------------------------------------------------------\n\
%*s %8.8g\n",iceb_tu_kolbait(56,gettext("Итого по группе")),gettext("Итого по группе"),itog);
fprintf(ff2,"\
-----------------------------------------------------------------\n\
%*s %8.8g\n",iceb_tu_kolbait(56,gettext("Итого по группе")),gettext("Итого по группе"),itog);

fprintf(ff1,"\
------------------------------------------------------------------------------------------------------------------------------------\n\
%*s %8.8g\n",iceb_tu_kolbait(56,gettext("Общий итог")),gettext("Общий итог"),itogs);
fprintf(ff2,"\
-----------------------------------------------------------------\n\
%*s %8.8g\n",iceb_tu_kolbait(56,gettext("Общий итог")),gettext("Общий итог"),itogs);

podpis(ff1);
podpis(ff2);


fclose(ff1);
fclose(ff2);
printw_vr(tmm);


OSTANOV();

oth->spis_imaf.plus(imaf2);
oth->spis_naim.plus(gettext("Распечатка абсолютных остатков по материаллам"));
oth->spis_imaf.plus(imaf1);
oth->spis_naim.plus(gettext("Пустографка"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),0);
 
return(0); 
}
