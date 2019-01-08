/*$Id: nahpodr.c,v 5.37 2013/09/26 09:43:36 sasa Exp $*/
/*28.03.2018	10.04.2003	Белых А.И.	nahpodr.c
Расчёт распределения начислений/удержаний по подразделениям
*/
#include        <errno.h>
#include        "buhg.h"
#define		KOLZNAK 9 //Количество знаков в колонке вместе с дробной частью

void		rasmaspodr(FILE*,double*,short*,short,short,short,double*,short*,short*);
int  nahpodr1(short,short,short,short,short,short,const char*,const char*,const char*,const char*,const char*,class spis_oth*);
void            sappkpodr(short,short,class iceb_tu_str*,short,short,short,FILE*,double*,short*,int);

extern short	mmm,ggg;

int nahpodr(class spis_oth *oth)
{
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
static class iceb_tu_str podr("");
static class iceb_tu_str kategr("");
static class iceb_tu_str nahis("");
static class iceb_tu_str tabnom("");
static class iceb_tu_str shet("");
char		bros[512];
int		kom1=0;
short		dn,mn,gn,dk,mk,gk;
class iceb_tu_str fio("");
VVOD VV(0);
VVOD MENU(3);

if(dat1.ravno()[0] == '\0')
 {
  sprintf(bros,"%d.%d.%d",1,mmm,ggg);
  dat1.new_plus(bros);  
 }
VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт свода начислений и удержаний по подразделения (форма 1)"));

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(podr.ravno(),128);
VV.VVOD_SPISOK_add_data(kategr.ravno(),128);
VV.VVOD_SPISOK_add_data(nahis.ravno(),128);
VV.VVOD_SPISOK_add_data(tabnom.ravno(),128);
VV.VVOD_SPISOK_add_data(shet.ravno(),128);

VV.VVOD_SPISOK_add_MD(gettext("Дата начала...(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца....(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения.(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Код категории.....(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Код начисления...(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Табельный номер..(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт.............(,,).."));

naz:;

clear(); 

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

int kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
podr.new_plus(VV.data_ravno(2));
kategr.new_plus(VV.data_ravno(3));
nahis.new_plus(VV.data_ravno(4));
tabnom.new_plus(VV.data_ravno(5));
shet.new_plus(VV.data_ravno(6));

class iceb_tu_str naim("");

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   prosf(bros);
   goto naz;

  case FK10:
  case ESC:
   return(1);

  case FK5:
  case FK2:
  case PLU:
   break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка категорий"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка начислений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка табельных номеров"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 4 :
      case -1:
       goto naz;

      case 0 :
        podr.new_plus("");
        naim.new_plus("");
        if(dirzarpodr(2,&podr,&naim) == 0)
          VV.data_z_plus(2,podr.ravno());
        goto naz;

      case 1 :
        vibrek("Kateg",&kategr,&naim);
        VV.VVOD_SPISOK_zapis_data(3,kategr.ravno());
        goto naz;

      case 2 :
        if(dirnach(1,&nahis,&naim) == 0)
         VV.VVOD_SPISOK_zapis_data(4,nahis.ravno());
        goto naz;

      case 3 :
      
        fio.new_plus("");
        int tmm=0;
        if(dirtab(&tmm,&fio,0,1) == 0)
         {
          tabnom.z_plus(tmm);
          VV.VVOD_SPISOK_zapis_data(5,tabnom.ravno());
         }
        goto naz;

     }

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
    goto naz;
 }

if(rsdatp(&dn,&mn,&gn,VV.VVOD_SPISOK_return_data(0),&dk,&mk,&gk,VV.VVOD_SPISOK_return_data(1)) != 0)
  goto naz;


return(nahpodr1(dn,mn,gn,dk,mk,gk,podr.ravno(),kategr.ravno(),nahis.ravno(),tabnom.ravno(),shet.ravno(),oth));

}

/******************************************/
/* Расчёт                                 */
/******************************************/

int nahpodr1(short dn,short mn,short gn,short dk,short mk,short gk,const char *podr,
const char *kateg,const char *nahislen,const char *tabnom,const char *shet,class spis_oth *oth)
{
char		strsql[512];
short		kolnah=0;
short		kolud=0;
short		kolpodr=0;
SQL_str		row;
time_t		tmm;
int		i=0;
SQLCURSOR 	curr;
clear();
printw("%s\n",gettext("Расчёт свода начислений и удержаний по подразделения (форма 1)"));
printw("%s %d.%d %s %d.%d\n",gettext("Период с"),mn,gn,
gettext("до"),mk,gk);
GDITE();

time(&tmm);

/*Определяем количество начислений*/

sprintf(strsql,"select kod from Nash");
class SQLCURSOR cur;
if((kolnah=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolnah == 0)
 {
  iceb_t_soob(gettext("Не введены начисления !"));
  return(1);
 }

short nahisl[kolnah];
i=0;
while(cur.read_cursor(&row) != 0)
  nahisl[i++]=atoi(row[0]);

//Определяем количество удержаний

sprintf(strsql,"select kod from Uder");
if((kolud=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolud == 0)
 {
  iceb_t_soob(gettext("Не введены удержания !"));
  return(1);
 }

short uder[kolud];
i=0;
while(cur.read_cursor(&row) != 0)
  uder[i++]=atoi(row[0]);

/*Определяем количество подразделений*/

sprintf(strsql,"select kod from Podr");
if((kolpodr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolpodr == 0)
 {
  iceb_t_soob(gettext("Не введены подразделения !"));
  return(1);
 }

short kolludpodr[kolpodr]; //Количество людей в подразделении
memset(&kolludpodr,'\0',sizeof(kolludpodr));

short podraz[kolpodr];
i=0;
while(cur.read_cursor(&row) != 0)
  podraz[i++]=atoi(row[0]);

double masivsum[kolpodr*kolnah];
memset(&masivsum,'\0',sizeof(masivsum));

double masivsumud[kolpodr*kolud];
memset(&masivsumud,'\0',sizeof(masivsumud));

int kolstr=0;
int kolstr1=0;
int kolstr2=0;
int nompodr=0;
int nomnah=0;
int kpodr=0;
int knah=0;
SQL_str row1;
double suma=0.;
short prn=0;
int tabn;
int nomer;
double sal=0.;
double saldb=0.;
double  suman=0.;
double  sumau=0.;
double	saldo=0.;
double	dolgi=0.;

class masiv_din_int TABNOM; //Массив табельных номеров
class masiv_din_int KODPODR; //Массив подразделений в которых находятся табельные номера
class masiv_din_double SALDO; //Массив сальдо по табельным номерам

short d=1;
short m=mn;
short g=gn;

while(sravmydat(d,m,g,1,mk,gk) <= 0)
 {
  
  printw("%s %d.%d%s\n",gettext("Дата"),m,g,
  gettext("г."));
  
  sprintf(strsql,"select tabn,podr,kateg from Zarn where god=%d and mes=%d", g,m);
//  printw("%s\n",strsql);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    return(1);
   }

  kolstr1=0;
  while(cur.read_cursor(&row) != 0)
   {
//    printw("%s %s %s\n",row[0],row[1],row[2]);
    
    strzag(LINES-1,0,kolstr,++kolstr1);
    if(proverka(tabnom,row[0],0,0) != 0)
      continue;
    if(proverka(podr,row[1],0,0) != 0)
      continue;
    if(proverka(kateg,row[2],0,0) != 0)
      continue;

    tabn=atoi(row[0]);

    if((nomer=TABNOM.find(tabn)) < 0)
      TABNOM.plus(tabn,nomer);

    kpodr=atoi(row[1]);

    if(nomer < 0)
     {
      KODPODR.plus(kpodr,nomer);
      sal=zarsald(dn,mn,gn,tabn,&saldb);
      SALDO.plus(sal,nomer);
      saldo+=sal;
     }

 
    //Определям номер по порядку подразделения
    
    for(nompodr=0 ; nompodr < kolpodr ; nompodr++)
     if(podraz[nompodr] == kpodr)
	break;
    
    /*Если нет подразделения в списке читаем дальше*/
    if(nompodr == kolpodr)
     {
      if(kpodr == 0)
       printw("%s %s %s !\n",gettext("Табельный номер"),
       row[0],"В карточку не введён код подразделения");
      else
       {
        printw("%s %s\n",gettext("Табельный номер"),row[0]);
        printw("%s %d !\n",gettext("Не найден код подразделения"),kpodr);
       }
      OSTANOV();

      continue;
     }


    short dnah=1;
    short dkon=31;    
    if(m == mn)
     dnah=dn;
    if(m == mk)
     dkon=dk;
    sprintf(strsql,"select prn,knah,suma,shet from Zarp where \
datz >= '%d-%d-%d' and datz <= '%d-%d-%d' and tabn=%s and suma <> 0.",
    g,m,dnah,g,m,dkon,row[0]);

    SQLCURSOR cur1;
    if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
     {
      msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
      continue;
     }
    if(nomer < 0) //Если только что добавили то номер меньше ноля
      nomer=TABNOM.find(tabn);

    if(kolstr2 > 0)
       kolludpodr[nompodr]++; /*Если есть начисления-удержания то увеличиваем счётчик*/

    while(cur1.read_cursor(&row1) != 0)
     {
//      printw("%s %s %s %s\n",row1[0],row1[1],row1[2],row1[3]);

      if(proverka(nahislen,row1[1],0,0) != 0)
         continue;
      if(proverka(shet,row1[3],0,0) != 0)
         continue;

      prn=atoi(row1[0]);
      knah=atoi(row1[1]);
      suma=atof(row1[2]);

      SALDO.plus(suma,nomer);

      if(prn == 1)      
       {
        for(nomnah=0 ; nomnah < kolnah ; nomnah++)
         if(nahisl[nomnah] == knah)
          break;
        masivsum[kolpodr*nomnah+nompodr]+=suma;
       }

      if(prn == 2)      
       {
        for(nomnah=0 ; nomnah < kolud ; nomnah++)
         if(uder[nomnah] == knah)
          break;

        masivsumud[kolpodr*nomnah+nompodr]+=suma;
       }
     }
   }
  dpm(&d,&m,&g,3); //Увеличиваем на месяц
 }
GDITE();


/*********************************************/
char  imaf[64];
FILE  *ff;

d=1;
dpm(&d,&mk,&gk,5);
printw("%s\n",gettext("Распечатываем начисления."));
refresh();
//Распечатываем свод начислений по подразделениям
sprintf(imaf,"npd%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }


iceb_t_zagolov(gettext("Свод начислений по подразделениям."),dn,mn,gn,dk,mk,gk,ff);

suman=0.;

rasmaspodr(ff,masivsum,nahisl,kolpodr,kolnah,1,&suman,podraz,kolludpodr);

podpis(ff);
fprintf(ff,"\x12");  /*Нормальный режим печати*/
fprintf(ff,"\x1B\x50"); /*10-знаков*/
fclose(ff);

/****************************************/

printw("%s\n",gettext("Распечатываем удержания"));
refresh();
//Распечатываем свод удержаний по подразделениям
char  imafud[64];
sprintf(imafud,"upd%d.lst",getpid());

if((ff = fopen(imafud,"w")) == NULL)
 {
  error_op_nfil(imafud,errno,"");
  return(1);
 }

iceb_t_zagolov(gettext("Свод удержаний по подразделениям"),dn,mn,gn,dk,mk,gk,ff);

sumau=0.;
rasmaspodr(ff,masivsumud,uder,kolpodr,kolud,2,&sumau,podraz,kolludpodr);

podpis(ff);
fclose(ff);

/***********************************/

printw("%s\n",gettext("Распечатываем начиления/удержания"));
refresh();
//Распечатываем свод удержаний по подразделениям
char  imafnahud[64];
sprintf(imafnahud,"nupd%d.lst",getpid());

if((ff = fopen(imafnahud,"w")) == NULL)
 {
  error_op_nfil(imafnahud,errno,"");
  return(1);
 }


iceb_t_zagolov(gettext("Свод начислений и удержаний по подразделениям"),dn,mn,gn,dk,mk,gk,ff);

suman=sumau=0.;

rasmaspodr(ff,masivsum,nahisl,kolpodr,kolnah,1,&suman,podraz,kolludpodr);
rasmaspodr(ff,masivsumud,uder,kolpodr,kolud,2,&sumau,podraz,kolludpodr);
//Распечатываем сумму к выдаче по подразделениям
double sumakvd=0.;
double sumakvdi=0.;
nomer=TABNOM.kolih();
class iceb_tu_str naim("");

for(i=0; i < kolpodr; i++)
 {
  sumakvd=0.;
  dolgi=0.;
  for(int nomertab=0; nomertab < nomer; nomertab++)
   {
    if(podraz[i] != KODPODR.ravno(nomertab))
     continue;
    sal=SALDO.ravno(nomertab);
    if(sal < 0.)
     dolgi+=sal;
    else
     sumakvd+=sal;
   }
  if(sumakvd == 0.)
    continue;

  sprintf(strsql,"select naik from Podr where kod=%d",podraz[i]);

  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
   naim.new_plus(row[0]);
  else
   naim.new_plus("");
  fprintf(ff,"%2d %-*.*s %9.2f\n",
  podraz[i],
  iceb_tu_kolbait(20,naim.ravno()),iceb_tu_kolbait(20,naim.ravno()),naim.ravno(),
  sumakvd);
  sumakvdi+=sumakvd;
 }

fprintf(ff,"%*s:%9.2f\n\n",iceb_tu_kolbait(33,gettext("Итого")),gettext("Итого"),sumakvdi);

dolgi=0.;
//Определяем сумму долгов
nomer=TABNOM.kolih();
for(i=0; i < nomer; i++)
 {
  sal=SALDO.ravno(i);
  if(sal < 0.)
   dolgi+=sal;
 }


printw("\n\
%s %15.2f\n\
%s %15.2f\n\
%s %15.2f\n\
%s %15.2f\n\
%s %15.2f\n\
%s %15.2f\n",
gettext("Сальдо         :"),saldo,
gettext("Итого начислено:"),suman,
gettext("Итого удержано :"),sumau,
gettext("Итого долгов   :"),dolgi,
gettext("К выдаче       :"),suman+sumau-dolgi+saldo,
gettext("Сальдо         :"),suman+sumau+saldo);

fprintf(ff,"\
%s %15.2f\n\
%s %15.2f\n\
%s %15.2f\n\
%s %15.2f\n\
%s %15.2f\n\
%s %15.2f\n",
gettext("Сальдо         :"),saldo,
gettext("Итого начислено:"),suman,
gettext("Итого удержано :"),sumau,
gettext("Итого долгов   :"),dolgi,
gettext("К выдаче       :"),suman+sumau-dolgi+saldo,
gettext("Сальдо         :"),suman+sumau+saldo);


podpis(ff);
fclose(ff);

iceb_t_ustpeh(imafnahud,0);
iceb_t_ustpeh(imafud,0);
iceb_t_ustpeh(imaf,0);

printw_vr(tmm);

OSTANOV();

oth->spis_imaf.plus(imafnahud);
oth->spis_naim.plus(gettext("Свод начислений и учержаний по подразделениям"));

oth->spis_imaf.plus(imafud);
oth->spis_naim.plus(gettext("Свод удержаний по подразделениям"));
oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Свод начислений по подразделениям"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
  iceb_t_ustpeh(oth->spis_imaf.ravno(nom),0);
return(0);
}

/******************************/
/*Программа распечатки массива*/
/******************************/

void		rasmaspodr(FILE *kaw,
double *naon, //Массив параметров*/
short *nan,  //Массив с перечнем параметров
short kkan,  //Количество категорий
short konn,  //Количество параметров
short metka, //1-начисления 2-удержания
double *sn, //Сумма по массиву
short *ka, //Массив со списком подразделений
short *koll) //Массив количества людей по подразделениям
{
int		ii,i1,i2,i,skn;
short           kkol; /*Количество колонок в распечатке*/
double		br1,br2;
int		khel;
class iceb_tu_str st("");
SQL_str         row;
class iceb_tu_str nai("");
char		strsql[512];
SQLCURSOR curr;
/*
При ужатом режиме 234 знака при 10 знаках на дюйм
                  275 знаков при 12 знаках на дюйм
(275-35)/13=18.46
*/


//kkol=17;
kkol=(275-35)/(KOLZNAK+1)-1;
*sn=0;
for(ii=0 ; ii < kkan; ii += kkol)
 {
//printw("ii-%d kkan-%d korr-%d\n",ii,kkan,korr);
  /*Проверяем если все равно нулю то печатаем следующюю группу столбиков*/
  i2=0; br2=0;
  for(i=ii; i<ii+kkol+i2 && i < kkan;i++)
   {
    br1=0.;
    for(skn=i;skn< kkan*(konn-1)+ii+kkol && skn<kkan*konn; skn+=kkan)
      br1+=naon[skn];
    if(br1 == 0.)
     i2++;
    br2+=br1;
   }
  if(br2 == 0.)
    continue;

  sappkpodr(metka,kkan,&st,ii,(short)(ii+kkol),konn,kaw,naon,ka,KOLZNAK);

  br1=0.;
  for(i=0 ; i<konn;i++)
   {
    /*Проверяем если по горизонтали сумма нулевая то строчку не печатаем*/
/*  printw("\ni-%d kkan-%d\n",i,kkan);*/
    br1=0.;
     for(skn=0;skn< kkan ; skn++)
      {
/*     printw("skn-%d naon[%d]=%.2f\n",skn,skn+i*kkan,naon[skn+i*kkan]);*/
       br1+=naon[skn+i*kkan];
      }
/*  printw("\n\n");
    OSTANOV();   */
    if(br1 == 0.)
      continue;
    nai.new_plus("");
    if(metka == 1)
      sprintf(strsql,"select naik from Nash where kod=%d",nan[i]);
    if(metka == 2)
      sprintf(strsql,"select naik from Uder where kod=%d",nan[i]);
    if(sql_readkey(&bd,strsql,&row,&curr) != 1)
     {
      beep();
      if(metka == 1) 
       printw("%s %d !\n",gettext("Не найден код начисления"),nan[i]);
      if(metka == 2) 
        printw("%s %d !\n",gettext("Не найден код удержания"),nan[i]);
     OSTANOV();
     }
    else
      nai.new_plus(row[0]);

    fprintf(kaw,"%3d %-*.*s",nan[i],iceb_tu_kolbait(30,nai.ravno()),iceb_tu_kolbait(30,nai.ravno()),nai.ravno());

    /*Предыдущая информация*/
    br1=0.;
    if(ii > 0 )
     {
      for(skn=i*kkan; skn<i*kkan+ii && skn < kkan*konn ;skn++)
       {
	/*printw(" skn=%d",skn);*/
	 br1+=naon[skn];
       }
      /*printw("br1=%.2f\n",br1);
      refresh();*/
     }
    i2=0;
    for(skn=i*kkan+ii; skn<i*kkan+ii+kkol+i2 && skn < i*kkan+kkan ;skn++)
     {

      /*Если колонка нулевая не печатать*/
      br2=0.;
      for(i1=skn-(i*kkan); i1< kkan*konn ; i1+=kkan)
	 br2+=naon[i1];

      if(br2 != 0.)
       {
	/*printw("skn-%d (%d) br2=%.2f %d  \n",skn,i*kkan+ii+kkk+i2,br2,kkan*konn);*/
	fprintf(kaw," %*.2f",KOLZNAK,naon[skn]);
       }
      else
       {
	i2++;
       }
      br1+=naon[skn];
     }
    fprintf(kaw," %*.2f\n",KOLZNAK,br1);
   }
  fprintf(kaw,"%s\n",st.ravno());

  fprintf(kaw,"%-*s",iceb_tu_kolbait(34,gettext(" И т о г о")),gettext(" И т о г о"));

  i2=0;
  for(i=ii; i<ii+kkol+i2 && i < kkan ;i++)
   {
    br1=0.;
    for(skn=i;skn < kkan*(konn-1)+ii+kkol+i2 && skn< kkan*konn ; skn+=kkan)
      br1+=naon[skn];

    if(br1 != 0.)
       fprintf(kaw," %*.2f",KOLZNAK,br1);
    else
      i2++;
    *sn=*sn+br1;
   }
  fprintf(kaw," %*.2f\n",KOLZNAK,*sn);

  fprintf(kaw,"%s\n",st.ravno());

  /*Распечатываем количество человек*/
  fprintf(kaw,"%-*s",iceb_tu_kolbait(34,gettext(" Количество работающих")),gettext(" Количество работающих"));
  /*Предыдущая информация по людям*/
  khel=0;

  for(i=0; i<ii && i<kkan;i++)
    khel+=koll[i];

  i2=0;
  for(i=ii; i<ii+kkol+i2 && i<kkan;i++)
   {
      /*Если колонка нулевая не печатать*/
      br2=0.;
      for(i1=i; i1< kkan*konn ; i1+=kkan)
	 br2+=naon[i1];

      if(br2 != 0.)
       {
	fprintf(kaw," %*d",KOLZNAK,koll[i]);
	khel+=koll[i];
       }
      else
	i2++;
   }
  fprintf(kaw," %*d\n\n",KOLZNAK,khel);
  if(i == kkan)
   break;
  /*ii+=korr*kkol+i3;*/
  ii+=i2;
 }

}

/***************/
/*Шапка таблицы*/
/***************/
void sappkpodr(short kk, //1-начисления  2-удержания
short kka,  //Количество категорий
class iceb_tu_str *st,
short pn,  //Позиция начала
short pk,  //Позиция конца
short konu, //Количество начислений или удержаний
FILE *kaw,double *nao,short *ka,
int kolznak)
{
int             i,i1,i2;
class iceb_tu_str nai("");
double          br2;
SQL_str         row;
char		strsql[512];
char		polkah[kolznak+2];
SQLCURSOR curr;
memset(polkah,'\0',sizeof(polkah));

memset(polkah,'-',kolznak+1);


/*Формируем строку подчеркивания*/
/*Полка над наименованием*/
st->new_plus("-----------------------------------");
i2=0;
for(i=pn; i< pk+i2 && i < kka; i++)
 {
  /*Если колонка нулевая не печатать*/
  br2=0.;
  for(i1=i; i1< kka*konu ; i1+=kka)
   {
     br2+=nao[i1];
   }
  if(br2 != 0. )
    st->plus(polkah);
  else
   i2++;
 }
/*Полка над ИТОГО*/
st->plus(polkah);

fprintf(kaw,"\n%s\n",st->ravno());

if(kk == 1)
 fprintf(kaw,"Код|%-*s",iceb_tu_kolbait(30,gettext("Начисления")),gettext("Начисления"));
if(kk == 2)
   fprintf(kaw,"Код|%-*s",iceb_tu_kolbait(30,gettext("Удержания")),gettext("Удержания"));
i2=0;
for(i=pn; i< pk+i2 && i < kka; i++)
 {
  /*Если колонка нулевая не печатать*/
  br2=0.;
  for(i1=i; i1< kka*konu ; i1+=kka)
   {
    br2+=nao[i1];
   }
  if(br2 == 0.)
   {
    i2++;
    continue;
   }
  nai.new_plus("");
  sprintf(strsql,"select naik from Podr where kod=%d",ka[i]);
  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
     beep();
     printw("Не найден код подразделения %d\n",ka[i]);
     OSTANOV();
   }
  else
   nai.new_plus(row[0]);

  fprintf(kaw,"| %-*.*s",iceb_tu_kolbait(kolznak-1,nai.ravno()),iceb_tu_kolbait(kolznak-1,nai.ravno()),nai.ravno());
 }

fprintf(kaw,"| %-*.*s|\n",iceb_tu_kolbait(kolznak-1,gettext(" Итого")),
iceb_tu_kolbait(kolznak-1,gettext(" Итого")),gettext(" Итого"));

fprintf(kaw,"   |%-30s"," ");
i2=0;
for(i=pn; i< pk+i2 && i < kka; i++)
 {
  /*Если колонка нулевая не печатать*/
  br2=0.;
  for(i1=i; i1< kka*konu ; i1+=kka)
    br2+=nao[i1];

  if(br2 != 0.)
    fprintf(kaw,"| %-*d",kolznak-1,ka[i]);
  else
   i2++;
 }
fprintf(kaw,"| %-*s|\n",kolznak-1," ");

fprintf(kaw,"%s\n",st->ravno());

}
