/* $Id: dvuslf2.c,v 5.42 2013/09/26 09:43:32 sasa Exp $ */
/*14.07.2015    21.06.2000      Белых А.И.      dvuslf2.c
Движение учлуг в развернутом виде
для сверки получения или оказания услуг за определенный период времени
*/
#include        <errno.h>
#include        "buhg.h"

void            gsapp1(short,short,short,short,short,short,short *,short *,FILE*);
void            gsapp1_shetl(short dn,short mn,short gn,short dk,short mk,short gk,short *sl,short *kst,FILE *ff);
void		itgdvus(short,const char*,const char*,double*,double*,short*,FILE*);

extern char	*masuslug; /*Массив кодов отмеченных услуг*/
extern int kol_strok_na_liste;

int dvuslf2(class spis_oth *oth)
{
FILE            *ff;
class iceb_tu_str bros("");
time_t          tmm;
static class iceb_tu_str dat1(""),dat2("");
static class iceb_tu_str podrz("");
static class iceb_tu_str grz("");
static class iceb_tu_str kodmz("");
static class iceb_tu_str nabor("");
static class iceb_tu_str korz("");
static class iceb_tu_str kodop("");
static class iceb_tu_str prr("");
static class iceb_tu_str shetz("");
int           kom,kom1;
short           dn,mn,gn;
short           dk,mk,gk;
short           dp,mp,gp;
short           d,m,g;
char            imaf[64];
short           sli=0,kst=0;
int             podr,podrl;
class iceb_tu_str org("");
short           mvstr; /*Метка выдачи строки*/
double		its=0.,itk=0.,br;
class iceb_tu_str kop("");
int		kgr,kodm;
long		kolstr,kolstr1;
SQL_str         row,row1;
char		strsql[1024];
short		tipz;
class iceb_tu_str kor("");
class iceb_tu_str ei("");
class iceb_tu_str naim("");
double		kolih,cena;
short		metka; /*0-материал 1-услуга*/
class iceb_tu_str shet(""),shet1("");
class iceb_tu_str naimsh("");
double		ishetk,ishets;
class iceb_tu_str naik("");
SQLCURSOR curr;

VVOD MENU(3);
VVOD VV(0);

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(podrz.ravno(),128);
VV.VVOD_SPISOK_add_data(korz.ravno(),128);
VV.VVOD_SPISOK_add_data(grz.ravno(),128);
VV.VVOD_SPISOK_add_data(kodop.ravno(),128);
VV.VVOD_SPISOK_add_data(kodmz.ravno(),128);
VV.VVOD_SPISOK_add_data(prr.ravno(),2);
VV.VVOD_SPISOK_add_data(nabor.ravno(),128);
VV.VVOD_SPISOK_add_data(shetz.ravno(),128);

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.........."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..........."));
VV.VVOD_SPISOK_add_MD(gettext("Подразделение.(,,)..."));
VV.VVOD_SPISOK_add_MD(gettext("Контрагент...(,,)...."));
VV.VVOD_SPISOK_add_MD(gettext("Группа.......(,,)...."));
VV.VVOD_SPISOK_add_MD(gettext("Код операции (,,)...."));
VV.VVOD_SPISOK_add_MD(gettext("Код услуги....(,,/*)."));
VV.VVOD_SPISOK_add_MD(gettext("Приход/расход.(+/-).."));
VV.VVOD_SPISOK_add_MD(gettext("Набор операций......."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт учета....(,,)..."));

VV.VVOD_SPISOK_add_ZAG(gettext("Движение услуг"));
VV.VVOD_SPISOK_add_ZAG(gettext("Введите нужные реквизиты"));

naz:;

clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("поиск"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
podrz.new_plus(VV.data_ravno(2));
korz.new_plus(VV.data_ravno(3));
grz.new_plus(VV.data_ravno(4));
kodop.new_plus(VV.data_ravno(5));
kodmz.new_plus(VV.data_ravno(6));
prr.new_plus(VV.data_ravno(7));
nabor.new_plus(VV.data_ravno(8));
shetz.new_plus(VV.data_ravno(9));

class iceb_tu_str kod("");

switch (kom)
 {

  case FK10:
  case ESC:
   return(1);

  case FK1:
    GDITE();

    iceb_t_pdoc("usl4_1.txt");

    clear();
    goto naz;

  case FK2:
  case PLU:
    break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций приходов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операцій расходов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка услуг"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр наборов операций"));
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
        if(dirpodrusl(1,&kod,&naik) == 0)
         podrz.z_plus(kod.ravno());

        VV.VVOD_SPISOK_zapis_data(2,podrz.ravno());
        goto naz;

      case 1 :
        vibrek("Kontragent",&korz);
        VV.VVOD_SPISOK_zapis_data(3,korz.ravno());
        goto naz;

      case 2 :
        vibrek("Uslgrup",&grz);
        VV.VVOD_SPISOK_zapis_data(4,grz.ravno());
        goto naz;

      case 3 :
        if(diruslop1(1,&kodop,&naik) == 0)
          VV.data_z_plus(5,kodop.ravno());
        goto naz;

      case 4 :
        if(diruslop2(1,&kodop,&naik) == 0)
          VV.data_z_plus(5,kodop.ravno());
        goto naz;
        
      case 5 :

        naik.new_plus("");
        if(dirusl(&kgr,&kodm,&naik,1,"") == 0)
         {
          kodmz.z_plus(kodm);
          VV.VVOD_SPISOK_zapis_data(6,kodmz.ravno());
         }
        
        goto naz;

      case 6 :
        vibrek("Plansh",&shetz);
        VV.VVOD_SPISOK_zapis_data(9,shetz.ravno());
        goto naz;
               
      case 7 :
        iceb_redfil("nabopru.alx",0);
        goto naz;
     }
    goto naz;

  case FK4:
    VV.VVOD_clear_data();
    goto naz;
     
  default:
    goto naz;
 }

if(VV.VVOD_SPISOK_return_data(8)[0] != '\0')
 {
  class iceb_tu_str nabor("");
  if(iceb_t_poldan(VV.VVOD_SPISOK_return_data(8),&nabor,"nabopru.alx") != 0)
   {
    sprintf(strsql,gettext("Не найден набор операций %s !"),VV.VVOD_SPISOK_return_data(8));
    iceb_t_soob(strsql);
    goto naz;
   }
  else
   VV.data_plus(5,nabor.ravno());
 }

if(kodmz.ravno()[0] == '*' && masuslug == NULL)
 { 
  iceb_t_soob(gettext("Не сделан список материалов !"));
  goto naz;
 } 

if(rsdatp(&dn,&mn,&gn,VV.VVOD_SPISOK_return_data(0),&dk,&mk,&gk,VV.VVOD_SPISOK_return_data(1)) != 0)
  goto naz;

tipz=0;
if(VV.VVOD_SPISOK_return_data(7)[0] == '+')
  tipz=1;
if(VV.VVOD_SPISOK_return_data(7)[0] == '-')
  tipz=2;

time(&tmm);
//bf=localtime(&tmm);

clear();
GDITE();

sprintf(strsql,"select * from Usldokum2 where datp >= '%04d-%02d-%02d' and \
datp <= '%04d-%02d-%02d' order by shetu,podr,datp asc",
gn,mn,dn,gk,mk,dk);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(1);
 }


sprintf(imaf,"dv%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

iceb_t_zagolov(gettext("Движение услуг"),dn,mn,gn,dk,mk,gk,ff);
kst=5;

if(tipz == 0)
 {
  fprintf(ff,"%s\n",gettext("Перечень приходов/расходов"));
  kst++;
 }
if(tipz == 1)
 {
  fprintf(ff,"%s\n",gettext("Перечень приходов"));
  kst++;
 }
if(tipz == 2)
 {
  fprintf(ff,"%s\n",gettext("Перечень расходов"));
  kst++;
 }

if(podrz.ravno()[0] != 0)
 {
  fprintf(ff,"%s %s\n",gettext("Подразделение"),podrz.ravno());
  kst++;
 }
if(korz.ravno()[0] != '\0')
 {
  fprintf(ff,gettext("Код контрагента - %s\n"),korz.ravno());
  kst++;
 }
if(kodop.ravno()[0] != '\0')
 {
  fprintf(ff,"%s - %s\n",gettext("Код операции"),kodop.ravno());
  kst++;
 }
if(grz.ravno()[0] != 0)
 {
  fprintf(ff,gettext("Группа - %s\n"),grz.ravno());
  kst++;
 }
if(kodmz.ravno()[0] != 0)
 {
  fprintf(ff,"%s %s\n",gettext("Код услуги"),kodmz.ravno());
  kst++;
 }
if(nabor.ravno()[0] != '\0')
 {
  fprintf(ff,gettext("Набор операций - %s\n"),nabor.ravno());
  kst++;
 }
if(shetz.ravno()[0] != '\0')
 {
  fprintf(ff,gettext("Счета учета - %s\n"),shetz.ravno());
  kst++;
 }
 
podr=0;
mvstr=podrl=0;

printw("%s %d.%d.%d %s %d.%d.%d\n",gettext("Период с"),dn,mn,gn,gettext("до"),
dk,mk,gk);

kolstr1=0;
ishetk=ishets=0.;

gsapp1(dn,mn,gn,dk,mk,gk,&sli,&kst,ff);

while(cur.read_cursor(&row) != 0)
 {
  kolstr1++;
  strzag(LINES-1,0,kolstr,kolstr1);  
  podr=atoi(row[10]);
  shet.new_plus(row[9]);  

  if(proverka(shetz.ravno(),shet.ravno(),1,0) != 0)
     continue;

  if(proverka(podrz.ravno(),row[10],0,0) != 0)
    continue;

  if(tipz != 0 )
   if(tipz != atoi(row[0]))
       continue;


  
  /*Читаем накладную*/
  sprintf(strsql,"select kontr,kodop from Usldokum \
where datd='%s' and podr=%d and nomd='%s' and tp=%s",
  row[1],podr,row[3],row[0]);

  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    VVOD SOOB(1);

    SOOB.VVOD_SPISOK_add_MD(gettext("Не найден документ !"));

    sprintf(strsql,"%s=%s %s=%s %s=%d",
    gettext("Дата"),row[4],
    gettext("Документ"),row[2],
    gettext("Подраделение"),podr);   
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);

    fprintf(ff,gettext("Не найден документ !"));
    fprintf(ff,"%s=%s %s=%s %s=%d\n",
    gettext("Дата"),row[4],
    gettext("Документ"),row[2],
    gettext("Подраделение"),podr);   
    continue;
   }

  kor.new_plus(row1[0]);
  kop.new_plus(row1[1]);

  if(proverka(korz.ravno(),kor.ravno(),0,0) != 0)
    continue;
  if(proverka(kodop.ravno(),kop.ravno(),0,0) != 0)
    continue;

  metka=atoi(row[4]);
  kodm=atoi(row[5]);
  cena=atof(row[7]);
  ei.new_plus(row[8]);


  if(kodmz.ravno()[0] != '\0')
   {
    bros.new_plus(kodm); 
    if(kodmz.ravno()[0] == '*' && masuslug != NULL)
     {
      if(proverka(masuslug,bros.ravno(),0,0) != 0)
        continue;
     }
    else
      if(proverka(kodmz.ravno(),bros.ravno(),0,0) != 0)
        continue;
   }

  if(metka == 0)
   {
    /*Узнаем наименование материалла*/
    sprintf(strsql,"select naimat,kodgr from Material where kodm=%d",
    kodm);
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      beep();
      fprintf(ff,"%s %d !\n",gettext("Не найден код материалла"),kodm);
      printw("%s %d !\n",gettext("Не найден код материалла"),kodm);
      OSTANOV();
      continue;
     }
   }

  if(metka == 1)
   {
    /*Читаем наименование услуги*/
    sprintf(strsql,"select naius,kodgr from Uslugi where kodus=%d",
    kodm);
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      printw("%s %d !\n",gettext("Не найден код услуги"),kodm);
      fprintf(ff,"%s %d !\n",gettext("Не найден код услуги"),kodm);
      OSTANOV();
      continue;
     }
   }

  naim.new_plus(row1[0]);

  if(proverka(grz.ravno(),row1[1],0,0) != 0)
     continue;

  if(podrl != podr)
   {
    if(podrl != 0)
     {
      gsapp1_shetl(dn,mn,gn,dk,mk,gk,&sli,&kst,ff);
      fprintf(ff,"%s %d",gettext("Подразделение"),podr);
      fprintf(ff,"----------------------------------\n");
     } 
    podrl=podr;
   } 


  if(SRAV(shet1.ravno(),shet.ravno(),0) != 0)
   {
    if(shet1.ravno()[0] != '\0')
      itgdvus(0,shet1.ravno(),naimsh.ravno(),&ishetk,&ishets,&kst,ff);

    naimsh.new_plus("");
    sprintf(strsql,"select nais from Plansh where ns='%s'",shet.ravno());
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      sprintf(strsql,gettext("Не найден счёт %s в плане счетов !"),shet.ravno());
      iceb_t_soob(strsql);
     }
    else
     naimsh.new_plus(row1[0]);   

    gsapp1_shetl(dn,mn,gn,dk,mk,gk,&sli,&kst,ff);

    fprintf(ff,"%s %s %s\n",gettext("Счёт"),shet.ravno(),naimsh.ravno());
    shet1.new_plus(shet.ravno());
   }

  if(metka == 1) //Смотрим дополнение к наименованию
   {
    sprintf(strsql,"select dnaim from Usldokum1 where datd='%s' and \
podr=%s and nomd='%s' and metka=1 and kodzap=%s and tp=%s",
    row[1],row[10],row[3],row[5],row[0]);
    if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
     {
      naim.plus(" ",row1[0]);
     }    
   }     
  

  /*printw("%d %s\n",mt5.nkr,naim);*/
  refresh();
  mvstr=0;
  
  rsdat(&dp,&mp,&gp,row[2],2);
  rsdat(&d,&m,&g,row[1],2);
  kolih=atof(row[6]);
  

  /*Узнаем наименование контрагента*/
  if(kor.ravno()[0] == '0' && polen(kor.ravno(),&bros,1,'-') == 0)
     kor.new_plus(bros.ravno());   

  org.new_plus("");
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
  kor.ravno());
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
     fprintf(ff,"%s %s !\n",gettext("Не найден код контрагента"),kor.ravno());
     printw("%s %s !\n",gettext("Не найден код контрагента"),kor.ravno());
     OSTANOV();
   }
  else
    org.new_plus(row1[0]);

  gsapp1_shetl(dn,mn,gn,dk,mk,gk,&sli,&kst,ff);

  if(atoi(row[0]) == 1)
     kop.new_plus("+");
  if(atoi(row[0]) == 2)
     kop.new_plus("-");

  printw("%s\n",naim.ravno());
  if(mvstr == 0)
   {
     
    fprintf(ff,"\
%-5d %-*.*s %-*s %-*s %14.10g %12.12g %15.2f %-*s %02d.%02d.%d %02d.%02d.%d %-7s %3s %s\n",
    kodm,
    iceb_tu_kolbait(35,naim.ravno()),iceb_tu_kolbait(35,naim.ravno()),naim.ravno(),
    iceb_tu_kolbait(5,ei.ravno()),ei.ravno(),
    iceb_tu_kolbait(7,row[9]),
    row[9],
    cena,
    kolih,kolih*cena,
    iceb_tu_kolbait(15,row[3]),
    row[3],
    d,m,g,dp,mp,gp,kop.ravno(),kor.ravno(),org.ravno());
    if(iceb_tu_strlen(naim.ravno()) > 35)
     {
      gsapp1_shetl(dn,mn,gn,dk,mk,gk,&sli,&kst,ff);
      fprintf(ff,"%5s %s\n"," ",iceb_tu_adrsimv(35,naim.ravno()));
     }
   }
  else
   {
/*
    printw("%s %s %s %s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4],
    row[5],row[6],row[7]);
    printw("cena=%f\n",cena);
*/
    fprintf(ff,"\
%5s %-35s %-15s %-*s %-*s %14.10g %12.12g %15.2f %-*s %02d.%02d.%d %02d.%02d.%d %-7s %3s %s\n",
    " "," "," ",
    iceb_tu_kolbait(5,ei.ravno()),
    ei.ravno(),
    iceb_tu_kolbait(7,row[9]),
    row[9],
    cena,
    kolih,kolih*cena,
    iceb_tu_kolbait(15,row[3]),
    row[3],
    d,m,g,dp,mp,gp,kop.ravno(),kor.ravno(),org.ravno());
   }  
  br=cena*kolih;
  br=okrug(br,0.01);
  its+=br;
  itk+=kolih;
  ishetk+=kolih;
  ishets+=br;  
  mvstr++;

 }


//Итого по счёту
itgdvus(0,shet1.ravno(),naimsh.ravno(),&ishetk,&ishets,&kst,ff);

//Общий итог
ishetk=itk; //Чтобы не обнулились результаты
ishets=its;
itgdvus(1,shet1.ravno(),naimsh.ravno(),&ishetk,&ishets,&kst,ff);

printw("%s:\n\
%-*s %12.12g\n\
%-*s %12.2f%s\n",
gettext("Итого"),
iceb_tu_kolbait(10,gettext("количество")),gettext("количество"),
itk,
iceb_tu_kolbait(10,gettext("сумма")),gettext("сумма"),
its,
gettext("грн.")); 


fclose(ff);

OSTANOV();
oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Движение услуг"));

iceb_t_ustpeh(oth->spis_imaf.ravno(0),1);

return(0);
}
/*******/
/*Шапка*/
/*******/
void            gsapp1_shetl(short dn,short mn,short gn,short dk,short mk,
short gk,
short *sl, //Счетчик листов
short *kst,  //Счетчик строк
FILE *ff)
{
*kst+=1;

if( *kst <= kol_strok_na_liste)
 return;

*kst=0;
fprintf(ff,"\f");

gsapp1(dn,mn,gn,dk,mk,gk,sl,kst,ff);
*kst+=1;
}
/**************/
/*Выдача итога*/
/**************/

void itgdvus(short met, //0-по счету 1 общий итог
const char *kod,const char *naik,double *ikol,double *isum,
short *kst,FILE *ff)
{
char		bros[512];

if(met == 0)
 sprintf(bros,"%s %s %s",gettext("Итого по счёту"),kod,naik);
if(met == 1)
 sprintf(bros,"%s",gettext("Общий итог"));
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\
%*.*s:%12.12g %15.2f\n",
iceb_tu_kolbait(70,bros),iceb_tu_kolbait(70,bros),bros,*ikol,*isum);

*kst=*kst+2;

*ikol=0.;
*isum=0.;
}
/***********************************/
/*Выдача шапки*/
/**********************************/
void            gsapp1(short dn,short mn,short gn,short dk,short mk,
short gk,
short *sl, //Счетчик листов
short *kst,  //Счетчик строк
FILE *ff)
{
*sl+=1;

fprintf(ff,"\
%s %02d.%02d.%4d%s %s %02d.%02d.%4d%s %*s%d\n",
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."),
iceb_tu_kolbait(100,gettext("Лист N")),gettext("Лист N"),
*sl);

fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext(" Код |        Наименование               |Един.| Счёт  |              |            |               |               | Д а т а  | Д а т а  |       |\n"));
fprintf(ff,gettext("     |          услуги                   |изме-| учета |   Ц е н а    | Количество |     Сумма     |Номер документа| выписки  |подтверж- |Код оп.|         К о н т р а г е н т\n"));
fprintf(ff,gettext("     |                                   |рения|       |              |            |               |               |документа | дения    |       |\n"));

fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

*kst+=6;

}
