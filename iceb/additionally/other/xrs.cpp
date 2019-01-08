/*$Id:$*/
/*31.03.2003	31.03.2003	Белых А.И.	xrs.c
Расчет себестоимости изготовления продукции
*/
#include 	"../iceb.h"
#define         DVERSIQ "03.12.2001"
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <unistd.h>
#include        <errno.h>
#include        <math.h>
#include        <sys/types.h>
#include        <sys/stat.h>
#include        <memory.h>
#include        <pwd.h>
#include        <time.h>
#include        <locale.h>
#include        "../buhg/buhg.h"

void		rassb(void);

short           ddd,mmm,ggg; /*Месяц год*/
char		*organ;
char		*putnansi=NULL;
char		*imabaz=NULL;
SQL_baza	bd;
extern char	*nameprinter;
short		vplsh; /*0-двух уровневый план счетов 1-многоуровневый*/
short		startgodb=0;

int             main(int argc,char **argv,char **envp)
{
struct  passwd  *ktor; /*Кто работает*/
char            bros[100];
SQL_str   	row;
char		kto[20];
char		*host=NULL;
char		*parol=NULL;
int		regim;
int		i=0;
time_t		tmm=0;
struct  tm      *bf;
int		kom=0;
SQLCURSOR curr; 
ktor=getpwuid(getuid());

(void)setlocale(LC_ALL,"");
(void)setlocale(LC_NUMERIC,"C"); //Переменная среды определяющая чем 
                                 //разделяется целая и дробная часть числа


init_screen();  /*Запуск экранного режима*/

umask(0117); /*Установка маски для записи и чтения группы*/

strcpy(kto,ktor->pw_name);

/*Читаем ключи запуска программы*/
readkey_par(argc,argv,&parol,&imabaz,&host,&regim);

/*Определяем путь на файли настройки*/
if((putnansi=getenv("PUTNANSI")) == NULL)
 {
  i=strlen(CONFIG_PATH)+1;
  if((putnansi=(char *)malloc(i*sizeof(char))) == NULL)
       error2(20,0,1,"Не могу выделить пямять для пути !");
  strcpy(putnansi,CONFIG_PATH);
 }  
/*
printw("putnansi=%s\n",putnansi);
OSTANOV();
*/
if(sql_openbaz(&bd,imabaz,host,kto,parol) != 0)
 {
  msql_error(&bd,"","");
  endwin();
  return(0);
 }

/*Читаем наименование организации*/
sprintf(bros,"select naikon from Kontragent where kodkon='00'");
if(sql_readkey(&bd,bros,&row,&curr) != 1)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено код организации 00 !"));  
  soobshw(&SOOB,stdscr,-1,-1,0,1);

  i=1+strlen(gettext("Не найдено наименование организации по коду 00 !"));
  if((organ=(char *)malloc(i*sizeof(char))) == NULL)
       error2(20,0,1,"Не могу выделить пямять для organ !");
  strcpy(organ,gettext("Не найдено наименование организации по коду 00 !"));
 }
else
 {
  i=strlen(row[0])+1;
  if((organ=(char *)malloc(i*sizeof(char))) == NULL)
       error2(20,0,1,"Не могу выделить пямять для organ !");
  strcpy(organ,row[0]);
 }

VVOD MENU(4);
VVOD VV(0);

/*Ищем настройку*/

if(nastsys(putnansi,imabaz) != 0)
  goto kon;

time(&tmm);
bf=localtime(&tmm);

ddd=bf->tm_mday;
if(bf->tm_mday > 14)
   mmm=bf->tm_mon+1;
else
   mmm=bf->tm_mon+1-1;
ggg=bf->tm_year+1900;
if(mmm == 0)
 {
  mmm=12;
  ggg-=1;
 }

char imaf[100];
memset(imaf,'\0',sizeof(imaf));
sprintf(imaf,"%s/%s/%s",putnansi,imabaz,"nastrb.alx");
poldan("Многопорядковый план счетов",bros,imaf);
if(SRAV(bros,"Вкл",1) == 0)
 vplsh=1;

bros[0]='\0';
poldan("Стартовый год",bros,imaf);
startgodb=atoi(bros);


MENU.VVOD_SPISOK_add_ZAG("Расчет себестоимости.");
MENU.VVOD_SPISOK_add_ZAG(organ);

MENU.VVOD_SPISOK_add_MD("Расчет себестоимости.");
MENU.VVOD_SPISOK_add_MD("Работа со списком контрагентов.");
MENU.VVOD_SPISOK_add_MD(gettext("Конец работы."));

for(;;)
 {

  clear();
  znak(0,COLS-26);
  infosys(12,COLS-27,host,VERSION,DVERSIQ,ddd,mmm,ggg,imabaz,nameprinter);

  if(menu3(&MENU,&kom,1,0,0) != 0)
    continue;

  switch (kom)
   {
    case 2 :
    case -1 :
      goto kon;
      break;

    case 0:
     rassb();
     break;

    case 1:
     clear(); /*Очистить экран */
     
     class iceb_tu_str kod("");
     class iceb_tu_str naim("");
     dirkontr(&kod,&naim,0);

     break;

   }
 }

kon:;

sql_closebaz(&bd);
VVOD SOOBN(1);
SOOBN.VVOD_SPISOK_add_MD(gettext("База закрыта. Программа закончила работу."));
soobsh(&SOOBN,-1,-1,1);
refresh();
endwin();
 
}
/*************************************/
/*Расчет себестоимости               */
/*************************************/

void	rassb()
{
static  char    dat1[11],dat2[11];
static  char    grupa[80];
int		kom=0;
char		strsql[300];
short		dn,mn,gn;
short		dk,mk,gk;
struct  tm      *bf;
time_t  tmmn=0;

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG("Расчет себестоимоти.");

VV.VVOD_SPISOK_add_MD("Дата начала.(д.м.г)..");
VV.VVOD_SPISOK_add_MD("Дата конца..(д.м.г)..");
VV.VVOD_SPISOK_add_MD("Группа контрагентов..");

VV.VVOD_SPISOK_add_data(dat1,sizeof(dat1));
VV.VVOD_SPISOK_add_data(dat2,sizeof(dat2)); 
VV.VVOD_SPISOK_add_data(grupa,sizeof(grupa)); 

naz:;

clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

move(LINES-1,0);

strcpy(dat1,VV.VVOD_SPISOK_return_data(0));
strcpy(dat2,VV.VVOD_SPISOK_return_data(1));
strcpy(grupa,VV.VVOD_SPISOK_return_data(2));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   sprintf(strsql,"%s/doc/%s",putnansi,"buhg5_1.txt");
   prosf(strsql);
   goto naz;

  case ESC:
  case FK10:
   return;

  case FK2:
  case PLU:
     break;

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }


if(rsdatp(&dn,&mn,&gn,dat1,&dk,&mk,&gk,dat2) != 0)
 goto naz;


time(&tmmn);
bf=localtime(&tmmn);

clear();

printw("Расчет себестоимоти.\n");
printw("За рериод %d.%d.%d => %d.%d.%d\n\n",
dn,mn,gn,dk,mk,gk);

int     kolkontr=0;
int	kolstr=0;
SQL_str row,row1;
char	kontr[20];
char	kodm[20];
int	nomer=0;
double	suma=0.;


//Создаем список контрагентов на которые списывались материальные средства

sprintf(strsql,"select kodkon,adres from Kontragent where kod='#'");

SQLCURSOR cur;
SQLCURSOR curr; 
if((kolkontr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

if(kolkontr == 0)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return;
 }

SPISOK SPK(0); //Список контрагентов
SPISOK MAT(0); //Список кодов материалов
while(cur.read_cursor(&row) != 0)
 {
  SPK.plus(row[0]);
  MAT.plus(row[1]);
  if(atoi(row[1]) == 0)
   {
    VVOD SOOB(1);
    sprintf(strsql,"Не введен код материала для контрагента %s !",row[0]);
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
   }
 }




double sumapr[kolkontr]; //Суммы по материалам пришедшим из производства
memset(&sumapr,'\0',sizeof(sumapr));

SPISOK SPKMT(0);  //Список код контрагента|код материала
masiv_din_double SUMASP(0); //Сумма списания по контрагентам

SPISOK KODM(0); //Список кодов материалов
masiv_din_double SUMAMAT(0); //Сумма списания по материалу

//Смотрим все расходы в диапазоне дат

sprintf(strsql,"select sklad,nomk,nomd,datd,tipz,kolih,cena from Zkart where datdp >= '%04d-%d-%d' and \
datdp <= '%04d-%d-%d'",
gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

if(kolstr == 0)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return;
 }
int  tipz=0;
while(cur.read_cursor(&row) != 0)
 {

  tipz=atoi(row[4]);  

  if(tipz == 1) //Определяем приход из производства
   {
    //Читаем код материала
    
    memset(kodm,'\0',sizeof(kodm));
    sprintf(strsql,"select kodm from Kart where sklad=%s and nomk=%s",
    row[0],row[1]);
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      VVOD SOOB(1);
      
      SOOB.VVOD_SPISOK_add_MD("Не нашли карточку !");
      sprintf(strsql,"Склад %s Номер карточки %s",row[0],row[1]);
      SOOB.VVOD_SPISOK_add_MD(strsql);
      soobshw(&SOOB,stdscr,-1,-1,0,1);
      continue;
    
     }
    strncpy(kodm,row1[0],sizeof(kodm)-1);
    if((nomer=MAT.find(kodm)) < 0)
      continue;     
    suma=atof(row[5])*atof(row[6]);
    suma=okrug(suma,0.01);
    sumapr[nomer]+=suma;
   }  
  
  
  if(tipz == 2) //Определяем что списано
   {
    //Читаем шапку документа
    memset(kontr,'\0',sizeof(kontr));
    sprintf(strsql,"select kontr from Dokummat where datd='%s' and sklad=%s and nomd='%s'",
    row[3],row[0],row[2]);
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {

      VVOD SOOB(1);
      SOOB.VVOD_SPISOK_add_MD(gettext("Не найден документ !"));  
      
      sprintf(strsql,gettext("Дата=%s Документ=%s Склад=%d"),
      row[3],row[2],atoi(row[0]));
      
      SOOB.VVOD_SPISOK_add_MD(strsql);
      soobshw(&SOOB,stdscr,-1,-1,0,1);
      continue;
     }

    strncpy(kontr,row1[0],sizeof(kontr)-1);
    if(SPK.find(kontr) < 0)
      continue;
          
    //Читаем группу контрагента
    sprintf(strsql,"select grup from Kontragent where kodkon='%s'",kontr);
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      VVOD SOOB(1);
      sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),kontr);
      SOOB.VVOD_SPISOK_add_MD(strsql);
      soobshw(&SOOB,stdscr,-1,-1,0,1);
      continue;
     }
    strcpy(strsql,row1[0]);
    if(proverka(grupa,strsql,0,0) != 0)
      continue;
      
    //Читаем код материала
    
    memset(kodm,'\0',sizeof(kodm));
    sprintf(strsql,"select kodm from Kart where sklad=%s and nomk=%s",
    row[0],row[1]);
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      VVOD SOOB(1);
      
      SOOB.VVOD_SPISOK_add_MD("Не нашли карточку !");
      sprintf(strsql,"Склад %s Номер карточки %s",row[0],row[1]);
      SOOB.VVOD_SPISOK_add_MD(strsql);
      soobshw(&SOOB,stdscr,-1,-1,0,1);
      continue;
    
     }
    strncpy(kodm,row1[0],sizeof(kodm)-1);

    suma=atof(row[5])*atof(row[6]);
    suma=okrug(suma,0.01);
    
    sprintf(strsql,"%s|%s",kontr,kodm);
    if((nomer=SPKMT.find(strsql)) < 0)
      SPKMT.plus(strsql);
      
    SUMASP.plus(suma,nomer);

    if((nomer=KODM.find(kodm)) < 0)
      KODM.plus(kodm);
      
    SUMAMAT.plus(suma,nomer);
   }    
 }

/***********Распечатка по контрагентам**************/

char  imaf[30];
FILE  *ff;
int   kolpar=0;
int   nomkont=0;
char  kontr1[20];
char  naim[80];

sprintf(imaf,"rs%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }
startfil(ff);
zagolov("Расчет себестоимоти.",bf,dn,mn,gn,dk,mk,gk,organ,ff);

kolpar=SPKMT.kolih();
//printw("kolpar=%d\n",kolpar);
suma=0.;
for(nomkont=0 ; nomkont < kolkontr; nomkont++)
 {
  strcpy(kontr,SPK.ravno(nomkont));
//  printw("kontr=%s\n",kontr);
//  refresh();

  if(nomkont != 0)
   {
    fprintf(ff,"\
---------------------------------------------------------\n");
    fprintf(ff,"%45s:%10.2f\n",gettext("Итого"),suma);
   }
  suma=0.;
  memset(naim,'\0',sizeof(naim));

  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kontr);
  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
    VVOD SOOB(1);
    sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),kontr);
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    continue;
   }
  strncpy(naim,row[0],sizeof(naim)-1);


  fprintf(ff,"\n%-4s %s\n",kontr,naim);
  fprintf(ff,"%45s:%10.2f\n","Объем производства",sumapr[nomkont]);
  fprintf(ff,"\
---------------------------------------------------------\n");
  fprintf(ff,"\
Код |    Наименование материала              |   Сумма  |\n");
  fprintf(ff,"\
---------------------------------------------------------\n");
  
  for(nomer=0; nomer < kolpar; nomer++)
   {
    POLE(SPKMT.ravno(nomer),kontr1,1,'|');
//    printw("kontr1=%s\n",kontr1);

    if(SRAV(kontr,kontr1,0) != 0)
      continue;    
    POLE(SPKMT.ravno(nomer),kodm,2,'|');

    //Читаем наименование материала
    memset(naim,'\0',sizeof(naim));
    sprintf(strsql,"select naimat from Material where kodm=%s",kodm);
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
      strncpy(naim,row[0],sizeof(naim)-1);    
    fprintf(ff,"%-4s %-40.40s %10.2f\n",
    kodm,naim,SUMASP.ravno(nomer));
    suma+=SUMASP.ravno(nomer);

    //Читаем наименование материала
    memset(naim,'\0',sizeof(naim));
    sprintf(strsql,"select naimat from Material where kodm=%s",kodm);
    if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
      strncpy(naim,row1[0],sizeof(naim)-1);    

    printw("%-4s %-40.40s %10.2f\n",kodm,naim,suma);
    refresh();
   }   


 }
fprintf(ff,"\
---------------------------------------------------------\n");
fprintf(ff,"%45s:%10.2f\n",gettext("Итого"),suma);

fclose(ff);



/*********распечатка итога по материалам**************/

char imafit[30];

sprintf(imafit,"rsi%d.lst",getpid());

if((ff = fopen(imafit,"w")) == NULL)
 {
  error_op_nfil(imafit,errno,"");
  return;
 }
startfil(ff);
zagolov("Расчет себестоимоти (общий итог).",bf,dn,mn,gn,dk,mk,gk,organ,ff);


fprintf(ff,"\
---------------------------------------------------------\n");
fprintf(ff,"\
Код |    Наименование материала              |   Сумма  |\n");
fprintf(ff,"\
---------------------------------------------------------\n");

kolpar=KODM.kolih();
suma=0.;
for(nomer=0; nomer < kolpar; nomer++)
 {
  strncpy(kodm,KODM.ravno(nomer),sizeof(kodm)-1);
  
  //Читаем наименование материала
  memset(naim,'\0',sizeof(naim));
  sprintf(strsql,"select naimat from Material where kodm=%s",kodm);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    strncpy(naim,row[0],sizeof(naim)-1);    
  fprintf(ff,"%-4s %-40.40s %10.2f\n",
  kodm,naim,SUMAMAT.ravno(nomer));
  suma+=SUMAMAT.ravno(nomer);

 }
fprintf(ff,"\
---------------------------------------------------------\n");
fprintf(ff,"%45s:%10.2f\n",gettext("Итого"),suma);
fclose(ff);

time_t tmmk;
double dii,kii;

time(&tmmk);
dii=(tmmk-tmmn)/60.;
dii=modf(dii,&kii);

printw("\n%s %.f%s %.f%s\n",
gettext("Время расчёта"),
kii,
gettext("мин."),
dii*60.,
gettext("сек."));

OSTANOV();
sprintf(strsql,"%s %s",imaf,imafit);
//rabfil("Расчет себестоимости.",strsql);
class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus("Распечатка списка показаний счётчика");
oth.spis_imaf.plus(imafit);
oth.spis_naim.plus("Итоги");
iceb_t_rabfil(&oth,"");

}
