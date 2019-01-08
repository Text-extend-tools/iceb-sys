/*$Id: saldonbs.c,v 1.28 2013/09/26 09:43:40 sasa Exp $*/
/*15.07.2015	19.06.2001	Белых А.И.	saldonbs.c
Обоpотный баланс по не балансовым счетам
*/
#include        "buhg.h"

class saldonbs_data
 {
  public:
  class SPISOK SHET;
  class SPISOK SHETRS;
  class SPISOK KONTR;

  masiv_din_double DEB; //Дебеты счетов до периода
  masiv_din_double KRE; //Кредиты счетов до периода
  masiv_din_double DEB1; //Дебеты счетов за период
  masiv_din_double KRE1; //Кредиты счетов за период
  masiv_din_double DEBRS; //Дебеты по счет/контрагент до периода
  masiv_din_double KRERS; //Кредиты по счет/контрагент до периода
  masiv_din_double DEBRS1; //Дебеты по счет/контрагент периода
  masiv_din_double KRERS1; //Кредиты по счет/контрагент периода
 };
 
int rozraxnbs(short,short,short,short,short,short,short,class saldonbs_data*);

void rasbnsrs(short i,const char *sh,const char *nash,int vids,int sb,short gn,class saldonbs_data *rekr,double *ideb,double *ikre,double *ideb1,double *ikre1,double *ideb2,double *ikre2,time_t vrem);

extern short	vplsh; /*0-двух уровневый план счетов 1-многоуровневый*/

extern short	startgodb; /*Стартовый год*/

void saldonbs(short dn,short mn,short gn,short dk,short mk,short gk)
{
int		kolshet=0; //Количество счетов в массиве
char		strsql[512];
SQL_str		row;
time_t		tmm=0;
int		i=0;
class iceb_tu_str nais("");
double		deb=0.,kre=0.,deb1=0.,kre1=0.,deb2=0.,kre2=0.;
double		ideb=0.,ikre=0.,ideb1=0.,ikre1=0.,ideb2=0.,ikre2=0.;
int		sb=1; //Расчитать по субсчетам
SQLCURSOR curr;
time(&tmm);
clear();
GDITE();

printw("%s\n",gettext("Расчёт баланса по не балансовим счетам"));
printw("%s %d.%d.%d%s %s %d.%d.%d%s\n",
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."));

refresh();

class saldonbs_data rekr;

if(rozraxnbs(dn,mn,gn,dk,mk,gk,sb,&rekr) != 0)
  return;

//bf=localtime(&tmm);


int Y,X;
kolshet=rekr.SHET.kolih();

getyx(stdscr,Y,X);
for(i=0; i <kolshet ; i++)
 {

  sprintf(strsql,"select saldo,nais,vids from Plansh where ns='%s'",rekr.SHET.ravno(i));
  
  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
    sprintf(strsql,gettext("Нет счета %s в плане счетов !"),rekr.SHET.ravno(i));
    iceb_t_soob(strsql);
    continue;
   }
  int vids=atoi(row[2]);
  int saldo=atoi(row[0]);
  nais.new_plus(row[1]);  
  move(Y,X);
  printw("%-5s %s %d %d\n",rekr.SHET.ravno(i),nais.ravno(),saldo,vids);
  strzag(LINES-1,0,kolshet,i);
  if(saldo == 3)
   {
    rasbnsrs(i,rekr.SHET.ravno(i),nais.ravno(),vids,sb,gn,&rekr,&ideb,&ikre,&ideb1,&ikre1,&ideb2,&ikre2,tmm);
   }
  else
   {
    
    deb=rekr.DEB.ravno(i);
    kre=rekr.KRE.ravno(i);
    deb1=rekr.DEB1.ravno(i);
    kre1=rekr.KRE1.ravno(i);
    deb2=deb+deb1;
    kre2=kre+kre1;     
    
    if(deb == 0. && kre == 0. && deb1 == 0. && kre1 == 0. &&\
    deb2 == 0. && kre2 == 0.)
      continue;

    if(deb > kre)
     {
      ideb+=deb-kre;
     }
    else
     {
      ikre+=kre-deb;
     }


    ikre1+=kre1;
    ideb1+=deb1;

    if(deb2 > kre2)
     {
      ideb2=deb2-kre2;
      sprintf(strsql,"insert into Saldo \
values ('%s',%d,'%s','%s',%.2f,%.2f,%d,%ld)",
      "0",0,rekr.SHET.ravno(i),"",deb2-kre2,0.,iceb_t_getuid(),tmm);   
     }
    else
     {
      ikre2=kre2-deb2;
      sprintf(strsql,"insert into Saldo \
values ('%s',%d,'%s','%s',%.2f,%.2f,%d,%ld)",
      "0",0,rekr.SHET.ravno(i),"",0.,kre2-deb2,iceb_t_getuid(),tmm);   
     }

     sql_zapis(strsql,0,0);
   }
 }
printw("\n%s:\n\
%s - %15s",gettext("Внебалансовые счета"),gettext("Сальдо начальное"),prnbr(ideb));

printw(" %15s\n",prnbr(ikre));
printw("%s - %15s",gettext("Оборот за период"),prnbr(ideb1));

printw(" %15s\n",prnbr(ikre1));
printw("%s - %15s",gettext("Сальдо конечное "),prnbr(ideb2));
printw(" %15s\n",prnbr(ikre2));

}

/******************************/
/* Формирование массивов      */
/******************************/

int     rozraxnbs(short dn,short mn,short gn,short dk,short mk,
short gk,short sb, //0-по счетам 1-по субсчетам
class saldonbs_data *rekr)
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
printw(gettext("Формируем массив счетов\n"));
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
  return(1);

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

  rekr->SHET.plus(row[0]);
  if(atoi(row[2]) == 3)
   {
    rekr->SHETRS.plus(row[0]);
   }

 }
printw("%s - %d\n",gettext("Количество счетов"),rekr->SHET.kolih());
printw(gettext("Количество счетов с развернутым сальдо - %d\n"),rekr->SHETRS.kolih());

rekr->DEB.make_class(rekr->SHET.kolih());
rekr->KRE.make_class(rekr->SHET.kolih());

rekr->DEB1.make_class(rekr->SHET.kolih());
rekr->KRE1.make_class(rekr->SHET.kolih());

//Определяем количество контрагентов
for(i=0; i < rekr->SHETRS.kolih() ;i ++)
 {
  sprintf(strsql,"select distinct kodkon from Skontr where ns='%s'",rekr->SHETRS.ravno(i));
  if(cur.make_cursor(&bd,strsql) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }
  while(cur.read_cursor(&row) != 0)
    if(rekr->KONTR.find_r(row[0]) < 0)
       rekr->KONTR.plus(row[0]);

 }
printw("%s:%d\n",gettext("Количество контрагентов"),rekr->KONTR.kolih());
int razmer=rekr->SHETRS.kolih()*rekr->KONTR.kolih();
if(razmer > 0)
 {
  rekr->DEBRS.make_class(razmer);
  rekr->KRERS.make_class(razmer);
  rekr->DEBRS1.make_class(razmer);
  rekr->KRERS1.make_class(razmer);
 }

/***********************************/
/*Узнаем стартовые сальдо по счетам*/
/***********************************/
printw(gettext("Вычисляем стартовые сальдо по счетам.\n"));
refresh();

for(i=0; i < rekr->SHET.kolih() ; i++)
 {
  sprintf(strsql,"select saldo,vids from Plansh where ns='%s'",rekr->SHET.ravno(i));
  
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    VVOD SOOB(1);
    sprintf(strsql,gettext("Не найден счёт %s в плане счетов !"),rekr->SHET.ravno(i));
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    continue;
   }

  tipsaldo=0;
  if(atoi(row[0]) == 3)
    tipsaldo=1;
  vids=atoi(row[1]);
  
  if(vids == 0) /*Счёт*/
   sprintf(strsql,"select ns,deb,kre,kodkon from Saldo where kkk='%d' \
and gs=%d and ns like '%s%%'",tipsaldo,godn,rekr->SHET.ravno(i));

  if(vids == 1) /*Субсчёт*/
   sprintf(strsql,"select ns,deb,kre,kodkon from Saldo where kkk='%d' \
and gs=%d and ns='%s'",tipsaldo,godn,rekr->SHET.ravno(i));

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

  rekr->DEB.plus(deb,i);
  rekr->KRE.plus(kre,i);

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
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено проводок !"));
  return(2);
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
  if((i=rekr->SHET.find_r(shet.ravno())) < 0)
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

  if((i1=rekr->SHETRS.find_r(shet.ravno())) >= 0)
    i2=rekr->KONTR.find_r(kontr.ravno());
  if(sravmydat(den,mes,god,dn,mn,gn) < 0)
   {
    rekr->DEB.plus(deb,i);
    rekr->KRE.plus(kre,i);
    if(i1 >= 0 && i2 >= 0)
     {
      rekr->DEBRS.plus(deb,i1*rekr->SHETRS.kolih()+i2);
      rekr->KRERS.plus(kre,i1*rekr->SHETRS.kolih()+i2);
     }
   }
  else
   {

    rekr->DEB1.plus(deb,i);
    rekr->KRE1.plus(kre,i);
    if(i1 >= 0 && i2 >= 0)
     {
      rekr->DEBRS1.plus(deb,i1*rekr->SHETRS.kolih()+i2);
      rekr->KRERS1.plus(kre,i1*rekr->SHETRS.kolih()+i2);
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
class saldonbs_data *rekr,
double *ideb,double *ikre,
double *ideb1,double *ikre1,double *ideb2,double *ikre2,
time_t vrem)
{
short           i1,i2;
long		kolstr;
SQL_str         row,row1;
char		strsql[512];
double		db=0.,kr=0.,db1=0.,kr1=0.;
double		deb=0.,kre=0.,deb1=0.,kre1=0.,deb2=0.,kre2=0.;
short		godn;


godn=startgodb;
if(startgodb == 0 || startgodb > gn)
 godn=gn;

/*printw("%d %s %s rs\n",i,sh,nash);*/
if(vplsh == 1)
 sprintf(strsql,"select kodkon,ns from Skontr where ns='%s'",sh);
if(vplsh == 0 && sb == 0)
 sprintf(strsql,"select kodkon,ns from Skontr where ns like '%s%%'",sh);
if(vplsh == 0 && sb == 1)
 sprintf(strsql,"select kodkon,ns from Skontr where ns='%s'",sh);

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

int kolshetrs=rekr->SHETRS.kolih();
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
   i2=rekr->KONTR.find_r(row[0]);
   i1=rekr->SHETRS.find_r(row[1]);
//   printw("i1=%d i2=%d kolshetrs=%d\n",i1,i2,kolshetrs);
   
   if(i1 >= 0 && i2 >= 0)
    {

     db+=rekr->DEBRS.ravno(i1*kolshetrs+i2);
     kr+=rekr->KRERS.ravno(i1*kolshetrs+i2);
     db1+=rekr->DEBRS1.ravno(i1*kolshetrs+i2);
     kr1+=rekr->KRERS1.ravno(i1*kolshetrs+i2);
    }
/*********
   printw("db=%.2f kr=%.2f db1=%.2f kr1=%.2f\n",
   db,kr,db1,kr1);
   OSTANOV();
***************/
   if(db > kr)
    {
     deb+=db-kr;
    }
   else
    {
     kre+=kr-db;
    }

   deb1+=db1;
   kre1+=kr1;


   if(db+db1 > kr+kr1)
    {
     if((db+db1)-(kr+kr1) != 0.)
      {
       deb2+=(db+db1)-(kr+kr1);
       sprintf(strsql,"insert into Saldo \
values ('%s',%d,'%s','%s',%.2f,%.2f,%d,%ld)",
       "1",0,sh,row[0],(db+db1)-(kr+kr1),0.,iceb_t_getuid(),vrem);   
       if(sql_zap(&bd,strsql) != 0)
        msql_error(&bd,gettext("Ошибка записи !"),strsql);
      }
    }
   else
    {
     if((kr+kr1)-(db+db1) != 0.)
      {
       kre2+=(kr+kr1)-(db+db1);
       sprintf(strsql,"insert into Saldo \
values ('%s',%d,'%s','%s',%.2f,%.2f,%d,%ld)",
       "1",0,sh,row[0],0.,(kr+kr1)-(db+db1),iceb_t_getuid(),vrem);   
       if(sql_zap(&bd,strsql) != 0)
        msql_error(&bd,gettext("Ошибка записи !"),strsql);
      }
    }
 }

*ideb+=deb;
*ikre+=kre;
*ideb1+=deb1;
*ikre1+=kre1;
*ideb2+=deb2;
*ikre2+=kre2;

}
