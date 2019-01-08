/*$Id: glkninbs.c,v 5.45 2013/09/26 09:43:33 sasa Exp $*/
/*14.05.2013	19.06.2001	Белых А.И.	glkninbs.c
Обоpотный баланс по не балансовым счетам
*/
#include        <math.h>
#include        <errno.h>
#include        "buhg.h"

int     rozraxnbs(short dn,short mn,short gn,short dk,short mk,short gk,short sb,class SPISOK *SHET,\
int *kolshet,class SPISOK *SHETRS,int *kolshetrs,masiv_din_double *DEB,masiv_din_double *KRE,masiv_din_double *DEB1,\
masiv_din_double *KRE1,masiv_din_double *DEBRS,masiv_din_double *KRERS,masiv_din_double *DEBRS1,\
masiv_din_double *KRERS1,class SPISOK *KONTR,int *kolkontr);

void    rasbnsrs(short,const char*,const char*,int,int,short,int,\
masiv_din_double *,masiv_din_double *,masiv_din_double *,\
masiv_din_double *,class SPISOK*,class SPISOK*,int*,int*,\
double*,double*,double*,double*,double*,double*,FILE *);
void	shetstnbs(int *,int *,FILE *);

void	sapoborbal(int list,FILE *ff);

extern short	vplsh; /*0-двух уровневый план счетов 1-многоуровневый*/

extern short	startgodb; /*Стартовый год*/
extern int kol_strok_na_liste;

int glkninbs(class spis_oth *oth)
{
int		kolshet=0; //Количество счетов в массиве
int		kolshetrs=0; //Количество счетов с развернутым сальдо
int		kolkontr=0; //Количество контрагентов
char		strsql[512];
SQL_str		row;
static class iceb_tu_str sheta("");
static class iceb_tu_str dat1(""),dat2("");
short           dn,mn,gn;
short           dk,mk,gk;
time_t		tmm=0;
char		imaf[64];
FILE		*ff;
int		i=0;
class iceb_tu_str nais("");
double		deb=0.,kre=0.,deb1=0.,kre1=0.,deb2=0.,kre2=0.;
double		ideb=0.,ikre=0.,ideb1=0.,ikre1=0.,ideb2=0.,ikre2=0.;
int		kollist=0;
int		kolstrok=0;
VVOD VV(0);

VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт оборотного баланса по внебалансовим счетам"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт..........(,,)..."));

VV.VVOD_SPISOK_add_data(11);
VV.VVOD_SPISOK_add_data(11);
VV.VVOD_SPISOK_add_data(112);

VV.VVOD_SPISOK_zapis_data(0,dat1.ravno());
VV.VVOD_SPISOK_zapis_data(1,dat2.ravno());
VV.VVOD_SPISOK_zapis_data(2,sheta.ravno());

naz:;

clear(); /*Очистить экран и закрасить фоновым цветом*/

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F10",gettext("выход"),NULL);

int kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
sheta.new_plus(VV.data_ravno(2));

move(LINES-1,0);

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("buhg5_1_1.txt");
   goto naz;

  case ESC:
  case FK10:
   return(1);

  case FK2:
  case PLU:
     break;

  default:
     goto naz;
 }

if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
 goto naz;


clearstr(LINES-1,0);


VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите режим"));
MENU.VVOD_SPISOK_add_MD(gettext("Расчитать по субсчетам"));
MENU.VVOD_SPISOK_add_MD(gettext("Расчитать по счетам"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));
int sb=0;
while(menu3w(stdscr,&MENU,&sb,-1,-1,0) != 0);
 
if(sb == -1 || sb == 2)
 return(1);

if(sb == 0)
 sb=1;
else
 sb=0;

time(&tmm);
clear();
GDITE();

printw("%s %d.%d.%d%s %s %d.%d.%d%s\n",
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."));

refresh();
class SPISOK SHET; //Общий список счетов
class SPISOK SHETRS; //Список счетов с развернутым сальдо
class SPISOK KONTR; //Общий список контрагентов

masiv_din_double DEB; //Дебеты счетов до периода
masiv_din_double KRE; //Кредиты счетов до периода
masiv_din_double DEB1; //Дебеты счетов за период
masiv_din_double KRE1; //Кредиты счетов за период
masiv_din_double DEBRS; //Дебеты по счет/контрагент до периода
masiv_din_double KRERS; //Кредиты по счет/контрагент до периода
masiv_din_double DEBRS1; //Дебеты по счет/контрагент периода
masiv_din_double KRERS1; //Кредиты по счет/контрагент периода

if(rozraxnbs(dn,mn,gn,dk,mk,gk,sb,\
&SHET,&kolshet,&SHETRS,&kolshetrs,&DEB,&KRE,&DEB1,&KRE1,\
&DEBRS,&KRERS,&DEBRS1,&KRERS1,&KONTR,&kolkontr) != 0)
 return(1);


sprintf(imaf,"obnds%d-%d_%d.lst",mn,mk,getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

sprintf(strsql,"%s (%s)",gettext("Б а л а н с"),
gettext("Внебалансовый"));

iceb_t_zagolov(strsql,dn,mn,gn,dk,mk,gk,ff);

kollist=1;
sapoborbal(kollist,ff);
kolstrok=11;

int Y,X;
SQLCURSOR cur;
getyx(stdscr,Y,X);
for(i=0; i <kolshet ; i++)
 {

  if(proverka(sheta.ravno(),SHET.ravno(i),1,0) != 0)
    continue;

  sprintf(strsql,"select saldo,nais,vids from Plansh where ns='%s'",SHET.ravno(i));
  
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,gettext("Нет счета %s в плане счетов !"),SHET.ravno(i));
    iceb_t_soob(strsql);
    continue;
   }
  int vids=atoi(row[2]);
  int saldo=atoi(row[0]);
  nais.new_plus(row[1]);  
  move(Y,X);
  printw("%-5s %s\n",SHET.ravno(i),nais.ravno());
  strzag(LINES-1,0,kolshet,i);
  if(saldo == 3)
   {
    rasbnsrs(i,SHET.ravno(i),nais.ravno(),vids,sb,gn,kolshetrs,&DEBRS,&KRERS,&DEBRS1,\
&KRERS1,&KONTR,&SHETRS,&kolstrok,&kollist,&ideb,&ikre,&ideb1,&ikre1,
&ideb2,&ikre2,ff);
   }
  else
   {
    
    deb=DEB.ravno(i);
    kre=KRE.ravno(i);
    deb1=DEB1.ravno(i);
    kre1=KRE1.ravno(i);
    deb2=deb+deb1;
    kre2=kre+kre1;     
    
    if(deb == 0. && kre == 0. && deb1 == 0. && kre1 == 0. &&\
    deb2 == 0. && kre2 == 0.)
      continue;
    shetstnbs(&kolstrok,&kollist,ff);

    if(deb > kre)
     {
      ideb+=deb-kre;
      fprintf(ff,"%-13.13s %15.2f %15s %15.2f %15.2f",
      SHET.ravno(i),deb-kre," ",deb1,kre1);
     }
    else
     {
      ikre+=kre-deb;
      fprintf(ff,"%-13.13s %15s %15.2f %15.2f %15.2f",
      SHET.ravno(i)," ",kre-deb,deb1,kre1);
     }

    ikre1+=kre1;
    ideb1+=deb1;

    if(deb2 > kre2)
     {
      ideb2+=deb2-kre2;
      fprintf(ff," %15.2f\n",deb2-kre2);
     }
    else
     {
      ikre2+=kre2-deb2;
      fprintf(ff," %15s %15.2f\n"," ",kre2-deb2);
     }
   }
 }

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"\
%-*s %15.2f %15.2f %15.2f %15.2f %15.2f %15.2f\n",
iceb_tu_kolbait(13,gettext("Итого")),gettext("Итого"),
ideb,ikre,ideb1,ikre1,ideb2,ikre2);

printw("\n\
%*s - %15s",iceb_tu_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),prnbr(ideb));

printw(" %15s\n",prnbr(ikre));
printw("%*s - %15s",iceb_tu_kolbait(20,gettext("Оборот за период")),gettext("Оборот за период"),prnbr(ideb1));

printw(" %15s\n",prnbr(ikre1));
printw("%*s - %15s",iceb_tu_kolbait(20,gettext("Сальдо конечное")),gettext("Сальдо конечное"),prnbr(ideb2));
printw(" %15s\n",prnbr(ikre2));
fprintf(ff,"\n%s_________________________\n",gettext("Главный бухгалтер"));

podpis(ff);
fprintf(ff,"\x1B\x50"); /*10-знаков*/
fprintf(ff,"\x12");  /*Нормальный режим печати*/
fprintf(ff,"\x1b\x6C%c",1); /*Установка левого поля в ноль*/

fclose(ff);

printw_vr(tmm);

OSTANOV();
oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Расчёт оборотного баланса по внебалансовим счетам"));
iceb_t_ustpeh(imaf,3);

return(0);

}

/******************************/
/* Формирование массивов      */
/******************************/

int     rozraxnbs(short dn,short mn,short gn,short dk,short mk,
short gk,short sb, //0-по счетам 1-по субсчетам
class SPISOK *SHET, //Список счетов
int *kolshet, //Количество счетов в плане счетов
class SPISOK *SHETRS, //Список счетов с развернутым сальдо
int *kolshetrs, //Количество счетов с развернутым сальдо
masiv_din_double *DEB, //Дебеты до периода
masiv_din_double *KRE, //Кредиты до периода
masiv_din_double *DEB1, //Дебеты период
masiv_din_double *KRE1, //Кредиты период
masiv_din_double *DEBRS, //Дебеты до периода счетов с развернутым сальдо
masiv_din_double *KRERS, //Кредиты до периода счетов с развернутым сальдо
masiv_din_double *DEBRS1, //Дебеты периода счетов с развернутым сальдо
masiv_din_double *KRERS1, //Кредиты периода счетов с развернутым сальдо
class SPISOK *KONTR, //Список контрагентов
int *kolkontr)
{
short		godn;
char		strsql[512];
int		kolstr;
SQL_str		row;
double		deb,kre;
int		i=0,i1=0,i2=0;
class iceb_tu_str shet("");
short		tipsaldo; //Тип сальдо
short		vids;     //Вид счета
short		den,mes,god;
class iceb_tu_str kontr("");

godn=startgodb;
if(startgodb == 0 || startgodb > gn)
 godn=gn;

printw("%s:%d\n",gettext("Стаpтовый год"),godn);
printw(gettext("Формируем массив счетов.\n"));
refresh();

/*Формируем массив счетов*/

sprintf(strsql,"select ns,vids,saldo from Plansh where stat=1 \
order by ns asc");

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
if(kolstr == 0)
 {
  iceb_t_soob(gettext("В плане счетов нет ни одного внебалансового счета !"));
  return(1);
 }

int pozz=0;
while(cur.read_cursor(&row) != 0)
 {
  pozz++;

  if(atoi(row[1]) == 1 && sb == 0)
    continue;
  shet.new_plus(row[0]);
  if(sb == 1 && atoi(row[1]) == 0 && pozz < kolstr-1)
   {
    cur.read_cursor(&row);
    /*Если нет субсчетов запоминаем счет*/
    if(SRAV(shet.ravno(),row[0],1) != 0)
     {
      cur.poz_cursor(pozz-1);
      cur.read_cursor(&row);
     }
    else
     pozz++;
   }

  SHET->plus(row[0]);
  *kolshet+=1;
  if(atoi(row[2]) == 3)
   {
    SHETRS->plus(row[0]);
    *kolshetrs+=1;
    
   }

 }
printw("%s: %d\n",gettext("Количество счетов"),
*kolshet);
refresh();
//SHET->print_masiv_char();

printw(gettext("Количество счетов с развернутым сальдо - %d\n"),
*kolshetrs);
refresh();

//SHETRS->print_masiv_char();

DEB->make_class(*kolshet);
KRE->make_class(*kolshet);

DEB1->make_class(*kolshet);
KRE1->make_class(*kolshet);

//Определяем количество контрагентов
for(i=0; i < *kolshetrs ;i ++)
 {
  sprintf(strsql,"select distinct kodkon from Skontr where ns='%s'",SHETRS->ravno(i));
  if((*kolkontr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }  
  while(cur.read_cursor(&row) != 0)
    if(KONTR->find_r(row[0]) < 0)
       KONTR->plus(row[0]);

 }
printw("%s:%d\n",
gettext("Количество контрагентов"),
KONTR->kolih());
int razmer=(*kolshetrs)*KONTR->kolih();
if(razmer > 0)
 {
  DEBRS->make_class(razmer);
  KRERS->make_class(razmer);
  DEBRS1->make_class(razmer);
  KRERS1->make_class(razmer);
 }

/***********************************/
/*Узнаем стартовые сальдо по счетам*/
/***********************************/
printw(gettext("Вычисляем стартовые сальдо по счетам.\n"));
refresh();

for(i=0; i < *kolshet ; i++)
 {
  sprintf(strsql,"select saldo,vids from Plansh where ns='%s'",SHET->ravno(i));
  
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,gettext("Не найден счёт %s в плане счетов !"),SHET->ravno(i));
    iceb_t_soob(strsql);
    continue;
   }

  tipsaldo=0;
  if(atoi(row[0]) == 3)
    tipsaldo=1;
  vids=atoi(row[1]);
  
  if(vids == 0) /*Счёт*/
   sprintf(strsql,"select ns,deb,kre,kodkon from Saldo where kkk='%d' \
and gs=%d and ns like '%s%%'",tipsaldo,godn,SHET->ravno(i));

  if(vids == 1) /*Субсчёт*/
   sprintf(strsql,"select ns,deb,kre,kodkon from Saldo where kkk='%d' \
and gs=%d and ns='%s'",tipsaldo,godn,SHET->ravno(i));

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
      msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

   if(kolstr == 0)
     continue;
  deb=kre=0.;
  while(cur.read_cursor(&row) != 0)
   {
    deb+=atof(row[1]);
    kre+=atof(row[2]);
   }

  if(tipsaldo == 0)  /*Счета с не развернутым сальдо*/
   {
    if(deb >= kre)
     {
      deb-=kre;
      kre=0.;
     }
    else
     {
      kre-=deb;
      deb=0.;
     }
   }

  DEB->plus(deb,i);
  KRE->plus(kre,i);

 }

/******************************************/
/*Просматриваем записи и заполняем массивы*/
/******************************************/

printw("%s\n",gettext("Просматриваем проводки, заполняем массивы."));
refresh();
sprintf(strsql,"select datp,sh,shk,kodkon,deb,kre from Prov \
where val=-1 and datp >= '%d-%02d-%02d' and \
datp <= '%d-%02d-%02d'",godn,1,1,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено проводок !"));
  return(0);
 }

int kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4],row[5]);
  strzag(LINES-2,0,kolstr,++kolstr1);

  rsdat(&den,&mes,&god,row[0],2);
  shet.new_plus(row[1]);
  kontr.new_plus(row[3]);
  deb=atof(row[4]);   
  kre=atof(row[5]);
  if((i=SHET->find_r(shet.ravno())) < 0)
   {
    VVOD SOOB(1);
    sprintf(strsql,"%s %s %s %s %s %s",
    row[0],row[1],row[2],row[3],row[4],row[5]);
    SOOB.VVOD_SPISOK_add_MD(strsql);
    sprintf(strsql,gettext("Не найден счёт %s в массиве счетов !"),shet.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);

    continue;
   }

  if((i1=SHETRS->find_r(shet.ravno())) >= 0)
    i2=KONTR->find_r(kontr.ravno());
  if(sravmydat(den,mes,god,dn,mn,gn) < 0)
   {
    DEB->plus(deb,i);
    KRE->plus(kre,i);
    if(i1 >= 0 && i2 >= 0)
     {
      DEBRS->plus(deb,i1*(*kolshetrs)+i2);
      KRERS->plus(kre,i1*(*kolshetrs)+i2);
     }
   }
  else
   {

    DEB1->plus(deb,i);
    KRE1->plus(kre,i);
    if(i1 >= 0 && i2 >= 0)
     {
      DEBRS1->plus(deb,i1*(*kolshetrs)+i2);
      KRERS1->plus(kre,i1*(*kolshetrs)+i2);
     }
   }
 }
return(0);
}

/****************************************/
/*Счет с развернутым сальдо              */
/*****************************************/
void rasbnsrs(short i, //Номер счета в массиве счетов
const char *sh,
const char *nash,  //Наименование счета
int vids,  //0-счет 1-субсчет
int sb, //0-по счетам 1-по субсчетам
short gn, //Год начала
int kolshetrs, //Количество счетов с развернутым сальдо
masiv_din_double *DEBRS, //Дебеты по счет/контрагент до периода
masiv_din_double *KRERS, //Кредиты по счет/контрагент до периода
masiv_din_double *DEBRS1, //Дебеты по счет/контрагент периода
masiv_din_double *KRERS1, //Кредиты по счет/контрагент периода
class SPISOK *KONTR, //Общий список контрагентов
class SPISOK *SHETRS, //Список счетов с развернутым сальдо
int *kolstrok,int *kollist,double *ideb,double *ikre,
double *ideb1,double *ikre1,double *ideb2,double *ikre2,
FILE *ff)
{
short           i1,i2;
long		kolstr;
SQL_str         row,row1;
char		strsql[512];
double		db=0.,kr=0.,db1=0.,kr1=0.;
double		deb=0.,kre=0.,deb1=0.,kre1=0.,deb2=0.,kre2=0.;
short		godn;

/*
printw("Обрабатываем счёт с развернутым сальдо.\n");
printw("Счёт %s\n",sh);
refresh();
*/

godn=startgodb;
if(startgodb == 0 || startgodb > gn)
 godn=gn;

/*printw("%d %s %s rs\n",i,sh,nash);*/

if(sb == 0)
 sprintf(strsql,"select kodkon,ns from Skontr where ns like '%s%%'",sh);
if(sb == 1)
 sprintf(strsql,"select kodkon,ns from Skontr where ns='%s'",sh);

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);


if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {

  /*Определяем стартовое сальдо по организации*/

   sprintf(strsql,"select deb,kre,ns from Saldo where kkk='1' and gs=%d and \
ns like '%s' and kodkon='%s'",godn,row[1],row[0]);

   db=kr=db1=kr1=0.;
   
   SQLCURSOR cur1;
   if((kolstr=cur1.make_cursor(&bd,strsql)) < 0)
      msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
   while(cur1.read_cursor(&row1) != 0)
    {
     db+=atof(row1[0]);
     kr+=atof(row1[1]);
    }
   i2=KONTR->find_r(row[0]);
   i1=SHETRS->find_r(row[1]);
   
   if(i1 >= 0 && i2 >= 0)
    {

     db+=DEBRS->ravno(i1*kolshetrs+i2);
     kr+=KRERS->ravno(i1*kolshetrs+i2);
     db1+=DEBRS1->ravno(i1*kolshetrs+i2);
     kr1+=KRERS1->ravno(i1*kolshetrs+i2);
    }
   if(db > kr)
     deb+=db-kr;
   else
     kre+=kr-db;

   deb1+=db1;
   kre1+=kr1;

   if(db+db1 > kr+kr1)
     deb2+=(db+db1)-(kr+kr1);
   else
     kre2+=(kr+kr1)-(db+db1);
 }


if(fabs(deb) < 0.01 && fabs(kre) < 0.01 && fabs(deb1) < 0.01
&& fabs(kre1) < 0.01 && fabs(deb2) < 0.01 && fabs(kre2) < 0.01)
  return;

shetstnbs(kolstrok,kollist,ff);

fprintf(ff,"%-*s %15.2f %15.2f %15.2f %15.2f %15.2f %15.2f\n",
iceb_tu_kolbait(13,sh),sh, deb, kre, deb1, kre1 , deb2 ,kre2);

*ideb+=deb;
*ikre+=kre;
*ideb1+=deb1;
*ikre1+=kre1;
*ideb2+=deb2;
*ikre2+=kre2;

}
/*******************************/
/*Шарка оборотного баланса     */
/*******************************/

void	sapoborbal(int list,FILE *ff)
{

fprintf(ff,"%*s%d\n",iceb_tu_kolbait(100,gettext("Лист N")),gettext("Лист N"),list);

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("             |          C а л ь д о          |         О б о р о т           |         С а л ь д о           |\n"));
fprintf(ff,gettext("  Счёт    -------------------------------------------------------------------------------------------------\n"));
fprintf(ff,gettext("             |    Дебет      |    Кредит     |    Дебет      |     Кредит    |    Дебет      |    Кредит     |\n"));

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------\n");
}

/***************/
/*Счетчик строк*/
/***************/
void	shetstnbs(int *kolstr,int *kollist,FILE *ff)
{
*kolstr+=1;
if(*kolstr > kol_strok_na_liste)
 {
  fprintf(ff,"\f");
  *kollist+=1;
  *kolstr=6;
  sapoborbal(*kollist,ff);
 }
}
