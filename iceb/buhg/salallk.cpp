/* $Id: salallk.c,v 5.55 2014/04/30 06:12:26 sasa Exp $ */
/*23.07.2018	08.08.1998	Белых А.И.	salallk.c
Получение сальдо по всем организациям по всем счетам
*/
#define	   KOLST  77 /*Количество строк на листе*/
#include        <errno.h>
#include        <math.h>
#include        "buhg.h"

void            saloro2(double[],char[],short,short,short,short,short,short,short);
int  saloro3(const char*,short,short,short,short,short,short,short,const char*,const char*,SPISOK*,int metka_r,class spis_oth*);
void            sstrsl(short*,short*,int metka_r,FILE*,FILE*);
void            sstrsl1(short*,short*,int metka_db,int metka_r,FILE*);
void		sal1(short,int metka_r,FILE*,FILE*);
void		sal2(short,int metka_dk,int metka_r,FILE*);
int xbudkz(int dk,const char *kodkontr,const char *shet,short dz,short mz,short gz,double sum_z,class iceb_tu_str *dataz);
int salallk_zvt(const char *kod_kontr,int *nom_zap,const char *imatab);

extern short	startgodb; /*Стартовый год*/

int salallk(class spis_oth *oth)
{
static class iceb_tu_str dat1(""),dat2("");
static class iceb_tu_str sheta("");
static class iceb_tu_str grupa("");
static class iceb_tu_str kontr("");
short           par;
short           dn,mn,gn;
short           dk,mk,gk;
class iceb_tu_str sh("");
long		kolstr;
SQL_str         row;
char		strsql[1024];
long		pozz;
int		kom1=0;
VVOD MENU(3);
VVOD VV(0);

if(dat1.ravno()[0] == '\0')
 {
  short    dt,mt,gt;
  poltekdat(&dt,&mt,&gt);
  sprintf(strsql,"01.01.%d",gt);
  dat1.new_plus(strsql);
  sprintf(strsql,"%02d.%02d.%d",dt,mt,gt);
  dat2.new_plus(strsql);
 } 

VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт сальдо по всем конт-там и по всем счетам с развернутым сальдо"));

VV.VVOD_SPISOK_add_MD(gettext("Начальная дата..."));
VV.VVOD_SPISOK_add_MD(gettext("Конечная дата...."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт.......(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Группа.....(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Контрагент.(,,).."));

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(sheta.ravno(),128);
VV.VVOD_SPISOK_add_data(grupa.ravno(),128);
VV.VVOD_SPISOK_add_data(kontr.ravno(),128);

naz:;
clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

nazz:;

par=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
sheta.new_plus(VV.data_ravno(2));
grupa.new_plus(VV.data_ravno(3));
kontr.new_plus(VV.data_ravno(4));


switch(par)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("buhg5_3.txt");
   clear();
   goto naz;
   
  case PLU:
  case FK2:
   break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите  нужное и нажмите Enter"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 3 :
      case -1:
       goto naz;
      case 0 :
        vibrek("Plansh",&sheta);
        VV.VVOD_SPISOK_zapis_data(2,sheta.ravno());
        goto naz;
      case 1 :
        vibrek("Gkont",&grupa);
        VV.VVOD_SPISOK_zapis_data(3,grupa.ravno());
        goto naz;
      case 2 :
        vibrek("Kontragent",&kontr);
        VV.VVOD_SPISOK_zapis_data(4,kontr.ravno());
        goto naz;
      }
     goto naz;
     
  case ESC:
  case FK10:
   return(1);

  case FK4:
    VV.VVOD_clear_data();
    goto naz;
   
  default:
   goto nazz;
 }

if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
  goto naz;

par=0;
VVOD DANET(1);
DANET.VVOD_SPISOK_add_MD(gettext("Встречные проводки учесть ?"));

if(danet(&DANET,1,stdscr) == 2)
  par=1;

DANET.VVOD_delete();
DANET.VVOD_SPISOK_add_MD(gettext("Расчёт с определением даты возникновения сальдо ?"));
int metka_r=0;
metka_r=danet(&DANET,2,stdscr);

/*Просматриваем план счетов, находим все счета с развернутым
сальдо и заряжаем в массив*/

clear();
GDITE();

move(0,0);
sprintf(strsql,"select ns from Plansh where saldo=3 and stat=0 \
order by ns asc");
//printw("strsql=%s\n",strsql);
//refresh();
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return(1);
 }
if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не нашли счетов с развернутым сальдо !"));
  return(1);
 }
class SPISOK sheta_srs;
pozz=0;
while(cur.read_cursor(&row) != 0)
 {
/*
  printw("%s %s %s\n",row[0],row[1],row[2]);
  refresh();
*/
  sh.new_plus(row[0]);
  if( pozz < kolstr-1)
   {
    cur.poz_cursor(pozz+1);
    cur.read_cursor(&row);

    /*Если нет субсчетов запоминаем счет*/
    if(SRAV(sh.ravno(),row[0],1) != 0)
     {
      cur.poz_cursor(pozz);
      cur.read_cursor(&row);
     }
   }
  pozz++;
  sheta_srs.plus(row[0]);
 }

printw(gettext("Расчёт сальдо по всем контрагентам и по всем счетам с развернутым сальдо"));

printw("\n%s %d.%d.%d %s %d.%d.%d\n",
gettext("Период с"),
dn,mn,gn,
gettext("до"),
dk,mk,gk);

printw(gettext("Количество счетов с развернутым сальдо - %d\n"),sheta_srs.kolih());

return(saloro3(sheta.ravno(),dn,mn,gn,dk,mk,gk,par,grupa.ravno(),kontr.ravno(),&sheta_srs,metka_r,oth));
}

/******************************/
/*Подпрограмма быстрого отчёта*/
/******************************/
int saloro3(const char *sheta,
short dn,short mn,
short gn,short dk,short mk,short gk,
short mks, //0-все проводки 1-без взаимно кореспондирующих
const char *grupa,
const char *kontr,
class SPISOK *sheta_srs,
int metka_r,
class spis_oth *oth)
{
int kls=sheta_srs->kolih(); //Количество счетов с развернутым сальдо
long		kolstr,kolstr1;
SQL_str         row,row1;
char		strsql[1024];
time_t          tmm;
short		mro,mrodeb,mrokre;
char		imaf[64],imaf1[64],imafdeb[64],imafkre[64];
long		koo=0;
long		i,i1,i2;
class iceb_tu_str sh("");
double		db,kr;
class iceb_tu_str kor("");
long		rzm;
double		mas1[6],mas2[6];
short		kli=0,kst=0;
short		klideb,kstdeb;
short		klikre,kstkre;
short		d,m,g;
class iceb_tu_str bros("");
short		godn;
double		itdeb,itkre;
SQLCURSOR cur;
SQLCURSOR cur1;

GDITE();

time(&tmm);

godn=startgodb;
if(startgodb == 0 || startgodb > gn)
 godn=gn;


class iceb_t_tmptab tabtmp;
const char *imatmptab={"saldoall"};
char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
kod_kontr char(32) not null DEFAULT '',\
nomz INT NOT NULL DEFAULT 0,\
index(nomz),\
unique(kod_kontr)) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return(1);
 }  


//class SPISOK spkontr;

/*Смотрим по каким контрагентам введено сальдо*/
sprintf(strsql,"select kodkon from Saldo where kkk=1 and gs=%d",godn);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
int nomer_zapisi=0;
while(cur.read_cursor(&row) != 0)
 {
  if(proverka(kontr,row[0],0,0) != 0)
    continue;
  if(grupa[0] != '\0')
   {
    sprintf(strsql,"select grup from Kontragent where kodkon='%s'",row[0]);
    if(readkey(strsql,&row1,&cur1) != 1)
     {
      sprintf(strsql,"%s %s!",gettext("Не найден код контрагента"),row[0]);
      iceb_t_soob(strsql);
      continue;
     }
    if(proverka(grupa,row1[0],0,0) != 0)
      continue;
   }
//  spkontr.plus(row[0]);
  salallk_zvt(row[0],&nomer_zapisi,imatmptab); /*Запись в таблицу*/
 }

/*Смотрим по каким контрагентам были сделаны проводки*/
sprintf(strsql,"select distinct kodkon from Prov where datp >= '%04d-1-1' and \
datp <= '%04d-%02d-%02d'and kodkon <> ''",godn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }


while(cur.read_cursor(&row) != 0)
 {
  if(proverka(kontr,row[0],0,0) != 0)
    continue;
  if(grupa[0] != '\0')
   {
    sprintf(strsql,"select grup from Kontragent where kodkon='%s'",row[0]);
    if(readkey(strsql,&row1,&cur1) != 1)
     {
      sprintf(strsql,"%s %s!",gettext("Не найден код контрагента"),row[0]);
      iceb_t_soob(strsql);
      continue;
     }
    if(proverka(grupa,row1[0],0,0) != 0)
      continue;
   }
//  if(spkontr.find_r(row[0]) < 0)
//    spkontr.plus(row[0]);
  salallk_zvt(row[0],&nomer_zapisi,imatmptab); /*Запись в таблицу*/
 }

//koo=spkontr.kolih();
sprintf(strsql,"select * from %s",imatmptab);
koo=readkey(strsql);


printw("%s: %d\n",
gettext("Количество контрагентов"),koo);

if(koo == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одного контрагента !"));
  return(1);
 }

rzm=koo*kls*4;
class masiv_din_double mdo;
mdo.make_class(rzm);

printw(gettext("Вычисляем стартовое сальдо по всем контрагентам\n"));
GDITE();
sprintf(strsql,"select ns,kodkon,deb,kre from Saldo where kkk='%d' and gs=%d",1,godn);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  if((i=sheta_srs->find(row[0])) < 0)
   continue;

  if(proverka(kontr,row[1],0,0) != 0)
    continue;

  if(grupa[0] != '\0')
   {
    sprintf(strsql,"select grup from Kontragent where kodkon='%s'",row[1]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)    
     if(proverka(grupa,row1[0],0,0) != 0)
       continue;
   }
   
      
/****************
  if((i1=spkontr.find_r(row[1])) >= 0)
   {
    rzm=(i1*kls*4)+(i*4);
    mdo.plus(atof(row[2]),rzm);
    mdo.plus(atof(row[3]),rzm+1);

   }
********************/
  sprintf(strsql,"select nomz from %s where kod_kontr='%s'",imatmptab,row[1]);

  if(readkey(strsql,&row1,&cur1) == 1)
   {
    i1=atoi(row1[0]);
    rzm=(i1*kls*4)+(i*4);
    mdo.plus(atof(row[2]),rzm);
    mdo.plus(atof(row[3]),rzm+1);
   }
  else
   {
    VVOD SOOB(1);

    sprintf(strsql,"%s %s %s",
    gettext("Не найден код контрагента"),
    row[1],
    gettext("в массиве кодов"));

    SOOB.VVOD_SPISOK_add_MD(strsql);

    soobshw(&SOOB,stdscr,-1,-1,0,1);

   }
 }

printw("%s\n",gettext("Просматриваем проводки"));
GDITE();
sprintf(strsql,"select datp,sh,shk,kodkon,deb,kre from Prov \
where val=0 and datp >= '%04d-%02d-%02d' and datp <= '%04d-%02d-%02d' \
order by datp,sh asc",godn,1,1,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

if(kolstr == 0)
 {
  beep();
  printw(gettext("Нет проводок за %d год.\n"),gn);
  OSTANOV();
  return(1);
 } 
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  kolstr1++;
  strzag(LINES-1,0,kolstr,kolstr1);
  if((i=sheta_srs->find(row[1])) < 0)
    continue;  

  if(proverka(kontr,row[3],0,0) != 0)
    continue;

  if(grupa[0] != '\0' )
   {
    sprintf(strsql,"select grup from Kontragent where kodkon='%s'",row[3]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)    
     if(proverka(grupa,row1[0],0,0) != 0)
       continue;
   }
/******************
  if((i1=spkontr.find_r(row[3])) < 0)
   {
    VVOD SOOB(1);

    sprintf(strsql,"%s %s %s",gettext("Не найден код контрагента"),row[3],gettext("в массиве кодов"));

    SOOB.VVOD_SPISOK_add_MD(strsql);

    sprintf(strsql,"%s %s %s %s %s %s",row[0],row[1],row[2],row[3],row[4],row[5]);
    SOOB.VVOD_SPISOK_add_MD(strsql);

    soobshw(&SOOB,stdscr,-1,-1,0,1);

    continue;
   }   
**********************/
  sprintf(strsql,"select nomz from %s where kod_kontr='%s'",imatmptab,row[3]);

  if(readkey(strsql,&row1,&cur1) == 0)
   {
    VVOD SOOB(1);

    sprintf(strsql,"%s %s %s",gettext("Не найден код контрагента"),row[3],gettext("в массиве кодов"));

    SOOB.VVOD_SPISOK_add_MD(strsql);

    sprintf(strsql,"%s %s %s %s %s %s",row[0],row[1],row[2],row[3],row[4],row[5]);
    SOOB.VVOD_SPISOK_add_MD(strsql);

    soobshw(&SOOB,stdscr,-1,-1,0,1);

    continue;
   }

  i1=atoi(row1[0]);

  if(mks == 1)
   {
    if(SRAV(row[1],row[2],1) == 0 || sheta_srs->find(row[2]) >= 0)
     continue;
   }
  rsdat(&d,&m,&g,row[0],2);
  
  rzm=(i1*kls*4)+(i*4);

   /*Период*/

  if(SRAV1(g,m,d,gn,mn,dn) <= 0)
   if(SRAV1(gk,mk,dk,g,m,d) <= 0)
    {
     mdo.plus(atof(row[4]),rzm+2);
     mdo.plus(atof(row[5]),rzm+3);
    }

   /*До периода*/
  if(SRAV1(g,m,d,gn,mn,dn) > 0)
    {
     mdo.plus(atof(row[4]),rzm);
     mdo.plus(atof(row[5]),rzm+1);
    }

 }

if(metka_r == 2)
  printw("%s\n",gettext("Распечатываем результат"));
else
  printw("%s\n",gettext("Определяем даты возникновения сальдо"));

GDITE();

sprintf(imaf,"sl%d.lst",getpid());
class iceb_fopen ff;
if(ff.start(imaf,"w") != 0)
 return(1);

sprintf(imaf1,"slk%d.lst",getpid());
class iceb_fopen ff1;
if(ff1.start(imaf1,"w") != 0)
 return(1);

sprintf(imafdeb,"sldeb%d.lst",getpid());
class iceb_fopen ffdeb;
if(ffdeb.start(imafdeb,"w") != 0)
 return(1);

char imafdeb_csv[64];
sprintf(imafdeb_csv,"sldeb%d.csv",getpid());
class iceb_fopen ffdcsv;
if(ffdcsv.start(imafdeb_csv,"w") != 0)
 return(1);

char imafkre_csv[64];
sprintf(imafkre_csv,"slkre%d.csv",getpid());
class iceb_fopen ffkcsv;
if(ffkcsv.start(imafkre_csv,"w") != 0)
 return(1);
 
sprintf(imafkre,"slkre%d.lst",getpid());
class iceb_fopen ffkre;
if(ffkre.start(imafkre,"w") != 0)
 return(1);
/********** 
if((ffkre = fopen(imafkre,"w")) == NULL)
 {
  error_op_nfil(imafkre,errno,"");
  return(1);
 }
****************/
fprintf(ff.ff,"\x1B\x33%c",30); /*Уменьшаем межстрочный интервал*/

fprintf(ff1.ff,"\x1B\x33%c",30); /*Уменьшаем межстрочный интервал*/

fprintf(ffdeb.ff,"\x1B\x33%c",30); /*Уменьшаем межстрочный интервал*/

fprintf(ffkre.ff,"\x1B\x33%c",30); /*Уменьшаем межстрочный интервал*/

iceb_t_zagolov(gettext("Сальдо по всем контрагентам"),dn,mn,gn,dk,mk,gk,ff.ff);
iceb_t_zagolov(gettext("Сальдо по всем контрагентам"),dn,mn,gn,dk,mk,gk,ff1.ff);
iceb_t_zagolov(gettext("Контрагенты дебиторы"),dn,mn,gn,dk,mk,gk,ffdeb.ff);
iceb_t_zagolov(gettext("Контрагенты кредиторы"),dn,mn,gn,dk,mk,gk,ffkre.ff);


iceb_t_zagolov(gettext("Контрагенты дебиторы"),dn,mn,gn,dk,mk,gk,ffdcsv.ff);
fprintf(ffdcsv.ff,"\n%s|%s|%s|%s|%s|%s|\n",gettext("Код контрагента"),gettext("Наименование контрагента"),
gettext("ЕГРПУ"),gettext("Счёт"),gettext("Сальдо дебетовое"),
gettext("Дата возникновения сальдо"));

iceb_t_zagolov(gettext("Контрагенты кредиторы"),dn,mn,gn,dk,mk,gk,ffkcsv.ff);
fprintf(ffkcsv.ff,"\n%s|%s|%s|%s|%s|%s|\n",gettext("Код контрагента"),gettext("Наименование контрагента"),
gettext("ЕГРПУ"),gettext("Счёт"),gettext("Сальдо кредитовое"),
gettext("Дата возникновения сальдо"));

kst=kstdeb=kstkre=5;
kli=klideb=klikre=1;
if(sheta[0] != '\0')
 {
  kst++; kstdeb++; kstkre++;
  fprintf(ff.ff,"%s:%s\n",gettext("Счёт"),sheta);
  fprintf(ff1.ff,"%s:%s\n",gettext("Счёт"),sheta);
  fprintf(ffdeb.ff,"%s:%s\n",gettext("Счёт"),sheta);
  fprintf(ffkre.ff,"%s:%s\n",gettext("Счёт"),sheta);
 }
if(grupa[0] != '\0')
 {
  kst++; kstdeb++; kstkre++;
  fprintf(ff.ff,"%s:%s\n",gettext("Группа"),grupa);
  fprintf(ff1.ff,"%s:%s\n",gettext("Группа"),grupa);
  fprintf(ffdeb.ff,"%s:%s\n",gettext("Группа"),grupa);
  fprintf(ffkre.ff,"%s:%s\n",gettext("Группа"),grupa);
 }
if(kontr[0] != '\0')
 {
  kst++; kstdeb++; kstkre++;
  fprintf(ff.ff,"%s:%s\n",gettext("Контрагент"),kontr);
  fprintf(ff1.ff,"%s:%s\n",gettext("Контрагент"),kontr);
  fprintf(ffdeb.ff,"%s:%s\n",gettext("Контрагент"),kontr);
  fprintf(ffkre.ff,"%s:%s\n",gettext("Контрагент"),kontr);
 }

sal1(kli,metka_r,ff.ff,ff1.ff);
sal2(klideb,2,metka_r,ffdeb.ff);
sal2(klikre,1,metka_r,ffkre.ff);

kst+=5;
kstdeb+=5;
kstkre+=5; //Увеличиваем на шапку

double deb=0.;
double kre=0.;
for(i=0; i< 6 ;i++)
  mas2[i]=0.;
class iceb_tu_str edrpu("");
class iceb_tu_str data_vs(""); /*дата возникновения возникновения сальдо*/
for(i1=0;i1<koo;i1++)
 {
  strzag(LINES-1,0,koo,i1);

  for(i2=0; i2< 6 ;i2++)
    mas1[i2]=0.;
  mro=mrodeb=mrokre=0;
  itdeb=itkre=db=kr=0;

//  kor.new_plus(spkontr.ravno(i1));
  sprintf(strsql,"select kod_kontr from %s where nomz=%ld",imatmptab,i1);

  if(readkey(strsql,&row1,&cur1) != 1)
    continue;
  kor.new_plus(row1[0]);
  
  for(i=0; i < kls; i++)
   {
    sh.new_plus(sheta_srs->ravno(i),sizeof(sh)-1);
    if(proverka(sheta,sh.ravno(),1,0) != 0)
     continue;
     
    rzm=(i1*kls*4)+(i*4);

    if(fabs(mdo.ravno(rzm)) < 0.009 && 
       fabs(mdo.ravno(rzm+1)) < 0.009 &&
       fabs(mdo.ravno(rzm+2)) < 0.009 &&
       fabs(mdo.ravno(rzm+3)) < 0.009)
        continue;
    if(fabs(mdo.ravno(rzm)-mdo.ravno(rzm+1)) < 0.009 && 
       fabs(mdo.ravno(rzm+2)) < 0.009 &&
       fabs(mdo.ravno(rzm+3)) < 0.009)
        continue;

     mas1[2]+=mdo.ravno(rzm+2);
     mas1[3]+=mdo.ravno(rzm+3);

    if(mro == 0)
     {
      sprintf(strsql,"select naikon,kod from Kontragent where kodkon='%s'",kor.ravno());
      if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
       {
         beep();
         printw("%s %s !\n",
         gettext("Не найден код контрагента"),kor.ravno());
         OSTANOV();
         continue;
       }
//      else
//       printw("%*s %s\n",iceb_tu_kolbait(6,kor.ravno()),kor.ravno(),row[0]);
      bros.new_plus(row[0]);
      edrpu.new_plus(row[1]);
      if(mdo.ravno(rzm) > mdo.ravno(rzm+1))
       {
        fprintf(ff.ff,"%*s %-*.*s %-*s %11.2f %11s %11.2f %11.2f",
        iceb_tu_kolbait(7,kor.ravno()),kor.ravno(),
        iceb_tu_kolbait(25,bros.ravno()),iceb_tu_kolbait(25,bros.ravno()),bros.ravno(),
        iceb_tu_kolbait(6,sh.ravno()),sh.ravno(),
        mdo.ravno(rzm)-mdo.ravno(rzm+1)," ",mdo.ravno(rzm+2),mdo.ravno(rzm+3));
        mas1[0]+=mdo.ravno(rzm)-mdo.ravno(rzm+1);
       }
      else
       {
        fprintf(ff.ff,"%*s %-*.*s %-*s %11s %11.2f %11.2f %11.2f",
        iceb_tu_kolbait(7,kor.ravno()),kor.ravno(),
        iceb_tu_kolbait(25,bros.ravno()),iceb_tu_kolbait(25,bros.ravno()),bros.ravno(),
        iceb_tu_kolbait(6,sh.ravno()),sh.ravno(),
        " ",mdo.ravno(rzm+1)-mdo.ravno(rzm),mdo.ravno(rzm+2),mdo.ravno(rzm+3));
        mas1[1]+=mdo.ravno(rzm+1)-mdo.ravno(rzm);
       } 
/*
      printw("%4s %-30.30s %-5s %10.10f %10.10f\n",
      kor.ravno(),bros.ravno(),sh,mdo[rzm]+mdo[rzm+2],mdo[rzm+1]+mdo[rzm+3]);
*/	
     }
    else
     {
      if(mdo.ravno(rzm) > mdo.ravno(rzm+1))
       {
        fprintf(ff.ff,"%7s %-25.25s %-*s %11.2f %11s %11.2f %11.2f",
        " "," ",
        iceb_tu_kolbait(6,sh.ravno()),sh.ravno(),
        mdo.ravno(rzm)-mdo.ravno(rzm+1)," ",mdo.ravno(rzm+2),mdo.ravno(rzm+3));
        mas1[0]+=mdo.ravno(rzm)-mdo.ravno(rzm+1);
       }
      else
       {
        fprintf(ff.ff,"%7s %-25.25s %-*s %11s %11.2f %11.2f %11.2f",
        " "," ",
        iceb_tu_kolbait(6,sh.ravno()),sh.ravno()," ",mdo.ravno(rzm+1)-mdo.ravno(rzm),mdo.ravno(rzm+2),mdo.ravno(rzm+3));
        mas1[1]+=mdo.ravno(rzm+1)-mdo.ravno(rzm);
/*
      printw("%4s %-30.30s %-5s %10.10f %10.10f\n",
      " "," ",sh,mdo[rzm]+mdo[rzm+2],mdo[rzm+1]+mdo[rzm+3]);
*/
       }
     }    

    if(mdo.ravno(rzm)+mdo.ravno(rzm+2) > mdo.ravno(rzm+1)+mdo.ravno(rzm+3))
     {
      deb=(mdo.ravno(rzm)+mdo.ravno(rzm+2))-(mdo.ravno(rzm+1)+mdo.ravno(rzm+3));
      if(metka_r == 1)
        xbudkz(2,kor.ravno(),sh.ravno(),dk,mk,gk,deb,&data_vs);
      if(mro == 0)
       {
        fprintf(ff1.ff,"%*s %-*.*s %-*s %11.2f %11s %s\n",
        iceb_tu_kolbait(7,kor.ravno()),kor.ravno(),
        iceb_tu_kolbait(25,bros.ravno()),iceb_tu_kolbait(25,bros.ravno()),bros.ravno(),
        iceb_tu_kolbait(6,sh.ravno()),sh.ravno(),
        deb,
        "",
        data_vs.ravno());
       }
      else
       {
        fprintf(ff1.ff,"%7s %-25.25s %-*s %11.2f %11s %s\n",
        " "," ",iceb_tu_kolbait(6,sh.ravno()),sh.ravno(),
        deb,
        "",
        data_vs.ravno());

       }


      if(fabs(deb) > 0.009)
       {
        itdeb+=deb;
        if(mrodeb == 0)
         {

          fprintf(ffdeb.ff,"%*s %-*.*s %-*s %11.2f %s\n",
          iceb_tu_kolbait(7,kor.ravno()),kor.ravno(),
          iceb_tu_kolbait(25,bros.ravno()),iceb_tu_kolbait(25,bros.ravno()),bros.ravno(),
          iceb_tu_kolbait(6,sh.ravno()),sh.ravno(),
          deb,
          data_vs.ravno());


          fprintf(ffdcsv.ff,"%s|%s|%s|%s|%s|%s|\n",
          kor.ravno(),
          bros.ravno(),
          edrpu.ravno(),
          sh.ravno(),
          iceb_tu_double_to_char_zp(deb,2),
          data_vs.ravno());

         }
        else
         {
          fprintf(ffdeb.ff,"%7s %-25.25s %-*s %11.2f %s\n",
          " "," ",iceb_tu_kolbait(6,sh.ravno()),sh.ravno(),
          deb,
          data_vs.ravno());


          fprintf(ffdcsv.ff,"%s|%s||%s|%s|%s|\n",
          " "," ",sh.ravno(),
          iceb_tu_double_to_char_zp(deb,2),
          data_vs.ravno());
         }
        mrodeb++;
        sstrsl1(&kstdeb,&klideb,2,metka_r,ffdeb.ff);
       }        
     
     
      fprintf(ff.ff," %11.2f %11s %s\n",deb,"",data_vs.ravno());
      mas1[4]+=deb;
     }
    else
     {
      kre=(mdo.ravno(rzm+1)+mdo.ravno(rzm+3))-(mdo.ravno(rzm)+mdo.ravno(rzm+2));
      if(metka_r == 1)
        xbudkz(1,kor.ravno(),sh.ravno(),dk,mk,gk,kre,&data_vs);
      if(mro == 0)
       {
        fprintf(ff1.ff,"%*s %-*.*s %-*s %11s %11.2f %s\n",
        iceb_tu_kolbait(7,kor.ravno()),kor.ravno(),
        iceb_tu_kolbait(25,bros.ravno()),iceb_tu_kolbait(25,bros.ravno()),bros.ravno(),
        iceb_tu_kolbait(6,sh.ravno()),sh.ravno(),
        " ",
        kre,
        data_vs.ravno());
       }
      else
       {
        fprintf(ff1.ff,"%7s %-25.25s %-*s %11s %11.2f %s\n",
        " "," ",iceb_tu_kolbait(6,sh.ravno()),sh.ravno()," ",
        kre,
        data_vs.ravno());

       }


      if(fabs(kre) > 0.009)
       {
        itkre+=kre;
        if(mrokre == 0)
         {
          fprintf(ffkre.ff,"%*s %-*.*s %-*s %11.2f %s\n",
          iceb_tu_kolbait(7,kor.ravno()),kor.ravno(),
          iceb_tu_kolbait(25,bros.ravno()),iceb_tu_kolbait(25,bros.ravno()),bros.ravno(),
          iceb_tu_kolbait(6,sh.ravno()),sh.ravno(),
          kre,
          data_vs.ravno());

          fprintf(ffkcsv.ff,"%s|%s|%s|%s|%s|%s|\n",
          kor.ravno(),
          bros.ravno(),
          edrpu.ravno(),
          sh.ravno(),
          iceb_tu_double_to_char_zp(kre,2),
          data_vs.ravno());
         }
        else
         {
          fprintf(ffkre.ff,"%7s %-25.25s %-*s %11.2f %s\n",
          " "," ",iceb_tu_kolbait(6,sh.ravno()),sh.ravno(),
          kre,
          data_vs.ravno());

          fprintf(ffkcsv.ff,"%s|%s||%s|%s|%s|\n",
          " "," ",sh.ravno(),
          iceb_tu_double_to_char_zp(kre,2),
          data_vs.ravno());

         }
        mrokre++;

        sstrsl1(&kstkre,&klikre,1,metka_r,ffkre.ff);
       }      
      fprintf(ff.ff," %11s %11.2f %s\n"," ",kre,data_vs.ravno());
      mas1[5]+=kre;
     }
    sstrsl(&kst,&kli,metka_r,ff.ff,ff1.ff);
    mro++;
   }

  if(mro > 1)
   {
//    sprintf(bros,"------%s %s :",gettext("Итого по"),kor.ravno());
    bros.new_plus("------");
    bros.plus(gettext("Итого по"),kor.ravno());
    bros.plus(" :");
    
    if(mas1[0] > mas1[1])
     {
      fprintf(ff.ff,"%*s %11.2f %11s %11.2f %11.2f",
      iceb_tu_kolbait(40,bros.ravno()),bros.ravno(),mas1[0]-mas1[1]," ",mas1[2],mas1[3]);
     }
    else
     {
      fprintf(ff.ff,"%*s %11s %11.2f %11.2f %11.2f",
      iceb_tu_kolbait(40,bros.ravno()),bros.ravno()," ",mas1[1]-mas1[0],mas1[2],mas1[3]);
     }

    if(mas1[0]+mas1[2] > mas1[1]+mas1[3])
     {
      fprintf(ff1.ff,"%*s %11.2f\n",
      iceb_tu_kolbait(40,bros.ravno()),bros.ravno(),
      (mas1[0]+mas1[2])-(mas1[1]+mas1[3]));
      fprintf(ff.ff," %11.2f\n",(mas1[0]+mas1[2])-(mas1[1]+mas1[3]));

     }
    else
     {
      fprintf(ff1.ff,"%*s %11s %11.2f\n",
      iceb_tu_kolbait(40,bros.ravno()),bros.ravno()," ",
      (mas1[1]+mas1[3])-(mas1[0]+mas1[2]));
      fprintf(ff.ff," %11s %11.2f\n"," ",(mas1[1]+mas1[3])-(mas1[0]+mas1[2]));
     }
    sstrsl(&kst,&kli,metka_r,ff.ff,ff1.ff);

   }

  if(mrodeb > 1)
   {
//    sprintf(bros,"------%s %s :",gettext("Итого по"),kor.ravno());
    bros.new_plus("------");
    bros.plus(gettext("Итого по"),kor.ravno());
    bros.plus(" :");

    fprintf(ffdeb.ff,"%*s %11.2f\n",
    iceb_tu_kolbait(40,bros.ravno()),bros.ravno(),itdeb);
    sstrsl1(&kstdeb,&klideb,2,metka_r,ffdeb.ff);
   

    fprintf(ffdcsv.ff,"|%s|||%s\n",
    bros.ravno(),iceb_tu_double_to_char_zp(itdeb,2));

   }
  if(mrokre > 1)
   {
//    sprintf(bros,"------%s %s :",gettext("Итого по"),kor.ravno());
    bros.new_plus("------");
    bros.plus(gettext("Итого по"),kor.ravno());
    bros.plus(" :");

    fprintf(ffkre.ff,"%*s %11.2f\n",
    iceb_tu_kolbait(40,bros.ravno()),bros.ravno(),itkre);
    sstrsl1(&kstkre,&klikre,1,metka_r,ffkre.ff);

    fprintf(ffkcsv.ff,"|%s|||%s|\n",
    bros.ravno(),iceb_tu_double_to_char_zp(itkre,2));

   }
  mas2[0]+=mas1[0];
  mas2[1]+=mas1[1];
  mas2[2]+=mas1[2];
  mas2[3]+=mas1[3];
  mas2[4]+=mas1[4];
  mas2[5]+=mas1[5];

 }


if(metka_r == 1)
 {
  fprintf(ff.ff,"\
----------------------------------------------------------------------------------------------------------------------------\n");
  fprintf(ff1.ff,"\
----------------------------------------------------------------------------\n");
  fprintf(ffdeb.ff,"\
----------------------------------------------------------------\n");
  fprintf(ffkre.ff,"\
----------------------------------------------------------------\n");
 }
else
 {
  fprintf(ff.ff,"\
-----------------------------------------------------------------------------------------------------------------\n");
  fprintf(ff1.ff,"\
-----------------------------------------------------------------\n");
  fprintf(ffdeb.ff,"\
-----------------------------------------------------\n");
  fprintf(ffkre.ff,"\
-----------------------------------------------------\n");
 } 
printw("\n%20s",prnbr(mas2[0]));
printw(" %20s\n",prnbr(mas2[1]));
printw("%20s",prnbr(mas2[2]));
printw(" %20s\n",prnbr(mas2[3]));
printw("%20s",prnbr(mas2[4]));
printw(" %20s\n",prnbr(mas2[5]));

fprintf(ff.ff,"%*s %11.2f %11.2f %11.2f %11.2f %11.2f %11.2f\n",
iceb_tu_kolbait(40,gettext("Итого :")),gettext("Итого :"),mas2[0],mas2[1],mas2[2],mas2[3],mas2[4],mas2[5]);

fprintf(ff1.ff,"%*s %11.2f %11.2f\n",
iceb_tu_kolbait(40,gettext("Итого :")),gettext("Итого :"),mas2[4],mas2[5]);

fprintf(ffdeb.ff,"%*s %11.2f\n",
iceb_tu_kolbait(40,gettext("Итого :")),gettext("Итого :"),mas2[4]);


fprintf(ffdcsv.ff,"|%s|||%s\n",
gettext("Итого :"),iceb_tu_double_to_char_zp(mas2[4],2));

fprintf(ffkre.ff,"%*s %11.2f\n",
iceb_tu_kolbait(40,gettext("Итого :")),gettext("Итого :"),mas2[5]);


fprintf(ffkcsv.ff,"|%s|||%s|\n",
gettext("Итого :"),iceb_tu_double_to_char_zp(mas2[5],2));

if(mas2[0] > mas2[1])
    fprintf(ff.ff,"%*s %11.2f %11s %11.2f %11.2f",
    iceb_tu_kolbait(40,gettext("Итого (свернуто):")),gettext("Итого (свернуто):"),mas2[0]-mas2[1]," ",mas2[2],mas2[3]);
  else
    fprintf(ff.ff,"%*s %11s %11.2f %11.2f %11.2f",
    iceb_tu_kolbait(40,gettext("Итого (свернуто):")),gettext("Итого (свернуто):")," ",mas2[1]-mas2[0],mas2[2],mas2[3]);

if(mas2[0]+mas2[2] > mas2[1]+mas2[3])
 {
  fprintf(ff1.ff,"%*s %11.2f\n",
  iceb_tu_kolbait(40,gettext("Итого (свернуто):")),gettext("Итого (свернуто):"),
  (mas2[0]+mas2[2])-(mas2[1]+mas2[3]));
  fprintf(ff.ff," %11.2f\n",(mas2[0]+mas2[2])-(mas2[1]+mas2[3]));

 }
else
 {
  fprintf(ff1.ff,"%*s %11s %11.2f\n",
  iceb_tu_kolbait(40,gettext("Итого (свернуто):")),gettext("Итого (свернуто):")," ",
  (mas2[1]+mas2[3])-(mas2[0]+mas2[2]));
  fprintf(ff.ff," %11s %11.2f\n"," ",(mas2[1]+mas2[3])-(mas2[0]+mas2[2]));
 }

podpis(ff.ff);

podpis(ff1.ff);

podpis(ffdeb.ff);

podpis(ffkre.ff);

podpis(ffdcsv.ff);
ffdcsv.end();
podpis(ffkcsv.ff);
ffkcsv.end();

ff.end();
ff1.end();
ffdeb.end();
ffkre.end();

printw_vr(tmm);
OSTANOV();

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Расчёт оборотов по всем контрагентам"));

oth->spis_imaf.plus(imaf1);
oth->spis_naim.plus(gettext("Расчёт сальдо по всем контрагентам"));

oth->spis_imaf.plus(imafdeb);
oth->spis_naim.plus(gettext("Список контрагентов с дебетовым сальдо"));

oth->spis_imaf.plus(imafkre);
oth->spis_naim.plus(gettext("Список контрагентов с кредитовым сальдо"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),1);

oth->spis_imaf.plus(imafdeb_csv);
oth->spis_naim.plus(gettext("Контрагенты с дебетовым сальдо для импорта в электонные таблицы"));

oth->spis_imaf.plus(imafkre_csv);
oth->spis_naim.plus(gettext("Контрагенты с кредитовым сальдо для импорта в электонные таблицы"));

return(0);
}

/*************************/
/*Счетчик строк и листов*/
/*************************/
void sstrsl(short *kst, //Количество строк
short *kli, //Количество листов
int metka_r, /*1-с датой 2-без*/
FILE *ff,FILE *ff1)
{

*kst+=1;
if(*kst >= KOLST)
 {
  fprintf(ff,"\f");
  fprintf(ff1,"\f");
  *kli=*kli+1;
  sal1(*kli,metka_r,ff,ff1);
  *kst=5;
 }  

}

/**********/
/*Шапка 1*/
/*********/
void sal1(short kli,int metka_r,FILE *ff,FILE *ff1)
{

fprintf(ff,"%90s%s%d\n","",gettext("Лист N"),kli);
fprintf(ff1,"%50s%s%d\n","",gettext("Лист N"),kli);

if(metka_r == 1) /*с датой возникновения сальдо*/
 {
  fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------\n");

  fprintf(ff,gettext("\
 Код   | Наименование организации| Счёт |  Сальдо начальное     |  Оборот за период     |    Сальдо конечное    |   Дата   |\n\
       |                         |      |   Дебет   |  Кредит   |   Дебет   |  Кредит   |   Дебет   |  Кредит   |  сальдо  |\n"));

  fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------\n");

  fprintf(ff1,"\
----------------------------------------------------------------------------\n");

  fprintf(ff1,gettext("\
  Код  | Наименование организации| Счёт |    Сальдо конечное    |   Дата   |\n\
       |                         |      |   Дебет   |  Кредит   |  сальдо  |\n"));

  fprintf(ff1,"\
----------------------------------------------------------------------------\n");
 }
else
 {
  fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------\n");

  fprintf(ff,gettext("\
 Код   | Наименование организации| Счёт |  Сальдо начальное     |  Оборот за период     |    Сальдо конечное    |\n\
       |                         |      |   Дебет   |  Кредит   |   Дебет   |  Кредит   |   Дебет   |  Кредит   |\n"));

  fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------\n");

  fprintf(ff1,"\
-----------------------------------------------------------------\n");

  fprintf(ff1,gettext("\
  Код  | Наименование организации| Счёт |    Сальдо конечное    |\n\
       |                         |      |   Дебет   |  Кредит   |\n"));

  fprintf(ff1,"\
-----------------------------------------------------------------\n");
 }
}
/*************************/
/*Счетчик строк и листов*/
/*************************/
void sstrsl1(short *kst, //Количество строк
short *kli, //Количество листов
int metka_dk,
int metka_r,
FILE *ff)
{

*kst+=1;
if(*kst >= KOLST)
 {
  fprintf(ff,"\f");
  *kli+=1;
  sal2(*kli,metka_dk,metka_r,ff);
  *kst=5;
 }  

}

/**********/
/*Шапка 2*/
/*********/
void sal2(short kli,int metka_dk, /*1-кредитовое сальдо 2-дебетовое*/
int metka_r,FILE *ff1)
{

fprintf(ff1,"%35s%s%d\n","",gettext("Лист N"),kli);
if(metka_r == 1)
 {
  fprintf(ff1,"\
----------------------------------------------------------------\n");

  if(metka_dk == 2)
   fprintf(ff1,gettext("\
  Код  | Наименование организации| Счёт |  Сальдо   |   Дата   |\n\
       |                         |      | дебетовое |  сальдо  |\n"));
  else
   fprintf(ff1,gettext("\
  Код  | Наименование организации| Счёт |  Сальдо   |   Дата   |\n\
       |                         |      | кредитовое|  сальдо  |\n"));
  
  fprintf(ff1,"\
----------------------------------------------------------------\n");
 }
else
 {
  fprintf(ff1,"\
-----------------------------------------------------\n");

  if(metka_dk == 2)
   fprintf(ff1,gettext("\
  Код  | Наименование организации| Счёт |  Сальдо   |\n\
       |                         |      | дебетовое |\n"));
  else
   fprintf(ff1,gettext("\
  Код  | Наименование организации| Счёт |  Сальдо   |\n\
       |                         |      | кредитовое|\n"));

  fprintf(ff1,"\
-----------------------------------------------------\n");
 }

}
/*********************/
/*запись контрагента во временную таблицу*/
/*******************************************/
int salallk_zvt(const char *kod_kontr,int *nom_zap,const char *imatab)
{
char strsql[512];

sprintf(strsql,"select kod_kontr from %s where kod_kontr='%s'",imatab,kod_kontr);
if(readkey(strsql) == 1)
 return(0);

sprintf(strsql,"insert into %s values('%s',%d)",imatab,kod_kontr,*nom_zap);

if(sql_zapis(strsql,1,0) != 0)
 return(1);

*nom_zap+=1;

return(0);
}
