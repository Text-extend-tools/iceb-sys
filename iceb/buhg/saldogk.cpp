/* $Id: saldogk.c,v 5.38 2013/09/26 09:43:40 sasa Exp $ */
/*01.05.2016    06.12.1993      Белых А.И.      saldogk.c
Программа переноса сальдо для "Главной книги"
*/
#include <math.h>
#include        "buhg.h"

void		saldonbs(short,short,short,short,short,short);
void ras2n(const char *sh,double *dns,double *kns,double *dpp,double *kpp,double *di,double *ki,const char *imatabl);
void ras1(int i,const char *ns,const char *nash,class GLKTMP *PROM,double *dns,double *kns,double *dpp,double *kpp,double *di,double *ki);


extern struct  passwd  *ktoz; /*Кто записал*/
extern short		startgodb;

void            saldogk()
{
double          dns,kns,dpp,kpp;
double          di,ki;
double          sd,sk,dpe,kpe;
short           dn,mn,gn;
short           dk,mk,gk;
time_t          tmmn;
int             i,sb;
class iceb_tu_str sh("");
SQL_str         row;
char		strsql[1024];
class iceb_tu_str nais("");
int		X,Y;
short		godp;
int		vids;
class iceb_tu_str sgod("");
short dt=0,mt=0,gt=0;
SQLCURSOR curr;
int kom=0;

if(iceb_t_prn() != 0) /*Проверка возможности изменять настройки*/
 return;

startgodb=0;
if(iceb_t_poldan("Стартовый год",&sgod,"nastrb.alx") == 0)
 {
  startgodb=sgod.ravno_atoi();
 }

if(startgodb == 0)
 {
  iceb_t_soob(gettext("Не введён стартовый год!"));
  return;
 }

VVOD VV(0);

VV.VVOD_SPISOK_add_ZAG(gettext("Главная книга"));
VV.VVOD_SPISOK_add_ZAG(gettext("Перенос сальдо"));
sprintf(strsql,"%s:%d",gettext("Стартовый год"),startgodb);
VV.VVOD_SPISOK_add_ZAG(strsql);

VV.VVOD_SPISOK_add_MD(gettext("С какого года.."));
VV.VVOD_SPISOK_add_MD(gettext("На какой год..."));
VV.VVOD_SPISOK_add_data(startgodb,5);
VV.VVOD_SPISOK_add_data(startgodb+1,5);

naz:;
clear();


helstr(LINES-1,0,"F2/+",gettext("расчёт"),
"F5",gettext("блокировка"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

switch(kom)
 {
  case FK2:
  case PLU:
    break;

  case FK5:
    iceb_tl_blok();
    goto naz;

  case FK10:
    return;
  default:
   goto naz;
 }

gn = (short)VV.data_ravno_atoi(0);
godp = (short)VV.data_ravno_atoi(1);
poltekdat(&dt,&mt,&gt);
if(gn == 0 || gn < gt-100 || gn > gt)
 { 
  iceb_t_soob(gettext("Не верно веден год с котрого нужно перенести !"));
  goto naz;
 } 

if(godp == 0 || godp < gt-100 || godp > gt)
 { 
  iceb_t_soob(gettext("Не верно веден год на который нужно перенести !"));
  goto naz;
 } 
dn=1;
mn=1;
dk=31;
mk=12;
gk=godp-1;

if(iceb_t_pvglkni(1,godp) != 0)
 goto naz;

clear();
VVOD DANET(1);
DANET.VVOD_SPISOK_add_ZAG("Перенос сальдо по счетам в \"Главной книге\"");
sprintf(strsql,"%s %d %s %s %d %s ? %s",
gettext("Перенести сальдо с"),
gn,
gettext("г."),
gettext("на"),
godp,
gettext("г."),
gettext("Вы уверены ?"));


DANET.VVOD_SPISOK_add_MD(strsql);

if(danet(&DANET,2,stdscr) == 2)
  return;

sb=1; /*Расчитать по субсчетам*/

time(&tmmn);
clear();

printw("%s %d -> %d\n",gettext("Перенос сальдо"),gn,godp);
GDITE();

GLKTMP PROM;
/*******************************************************/
printw("%s\n",gettext("Формируем массив счетов"));
refresh();
int msrv=0;
if(sb == 0)
 msrv=1;

/*************************/
/*Формируем массив счетов*/
/*************************/
SQLCURSOR cur;
SQLCURSOR cur1;
int kolstr=0;

sprintf(strsql,"select ns,vids,saldo from Plansh where stat=0 order by ns asc");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не введён план счетов !"));
  return;
 }

int pozz=0;
while(cur.read_cursor(&row) != 0)
 {
  pozz++;

  if(atoi(row[1]) == 1 && sb == 0)
    continue;
  sh.new_plus(row[0]);
  if(sb == 1 && atoi(row[1]) == 0 && pozz < kolstr-1)
   {
    cur.read_cursor(&row);
    /*Если нет субсчетов запоминаем счет*/
    if(SRAV(sh.ravno(),row[0],1) != 0)
     {
      cur.poz_cursor(pozz-1);
      cur.read_cursor(&row);
     }
    else
     pozz++;
   }
  PROM.masf.plus(row[0]);

  if(atoi(row[2]) == 3)
     PROM.msr.plus(row[0]);
 }
int ks=PROM.masf.kolih();
printw("%s: %d\n",gettext("Количество счетов"),ks);
/****************
printw("%s:",gettext("Счета"));
for(int ish=0; ish < PROM.masf.kolih() ; ish++)
  printw("%s ",PROM.masf.ravno(ish));
printw("\n");
***************/
refresh();



class iceb_t_tmptab tabtmp;
const char *imatmptab={"glkni"};
char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
shet char(32) not null DEFAULT '',\
kod_kontr char(32) not null DEFAULT '',\
debs double(16,2) not null DEFAULT 0.,\
kres double(16,2) not null DEFAULT 0.,\
debp double(16,2) not null DEFAULT 0.,\
krep double(16,2) not null DEFAULT 0.,\
unique(shet,kod_kontr)) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return;
 }  

int ll1=ks*ks; /*Шахматка для заполнения*/
PROM.ssd.make_class(ks);
PROM.ssk.make_class(ks);
PROM.deb_end.make_class(ks);
PROM.kre_end.make_class(ks);
PROM.sl.make_class(ll1);
PROM.pe.make_class(ll1);

int ksrs=PROM.msr.kolih();
printw(gettext("Количество счетов с развернутым сальдо - %d\n"),ksrs);

/***********************************/
/*Узнаем стартовые сальдо по счетам*/
/***********************************/
printw(gettext("Вычисляем стартовые сальдо по счетам\n"));
refresh();
double deb,kre;
for(i=0; i < ks ; i++)
 {
  sh.new_plus(PROM.masf.ravno(i));
  sprintf(strsql,"select saldo,vids from Plansh where ns='%s'",sh.ravno());
  
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,gettext("Не найден счёт %s в плане счетов !"),sh.ravno());
    iceb_t_soob(strsql);
    continue;
   }

  int ts=0;
  if(atoi(row[0]) == 3)
    ts=1;
  vids=atoi(row[1]);
  
  if(vids == 0) /*Счет*/
   sprintf(strsql,"select kodkon,deb,kre from Saldo where kkk='%d' and gs=%d \
and ns like '%s%%'",ts,gn,sh.ravno());
  if(vids == 1) /*Субсчёт*/
   sprintf(strsql,"select kodkon,deb,kre from Saldo where kkk='%d' and gs=%d \
and ns='%s'",ts,gn,sh.ravno());

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
      msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

  if(kolstr <= 0)
     continue;

  while(cur.read_cursor(&row) != 0)
   {
    deb=atof(row[1]);
    kre=atof(row[2]);

    if(row[0][0] != '\0')
      glkni_zvt(0,sh.ravno(),row[0],deb,kre,imatmptab);

    PROM.ssd.plus(deb,i);
    PROM.ssk.plus(kre,i);
   }
/*  
  printw("%s %.2f %.2f\n",sh,ssd[i],ssk[i]);
  getch();
*/
  if(ts == 0)  /*Счета с не развернутым сальдо*/
   {
    if(PROM.ssd.ravno(i) >= PROM.ssk.ravno(i))
     {
      PROM.ssd.plus(PROM.ssk.ravno(i)*-1,i);
      PROM.ssk.new_plus(0.,i);
     }
    else
     {
      PROM.ssk.plus(PROM.ssd.ravno(i)*-1,i);
      PROM.ssd.new_plus(0.,i);
     }
   }
 }

/******************************************/
/*Просматриваем записи и заполняем массивы*/
/******************************************/

printw("%s\n",gettext("Просматриваем проводки, заполняем массивы"));
GDITE();
sprintf(strsql,"select datp,sh,shk,kodkon,deb,kre from Prov \
where val=0 and datp >= '%d-%02d-%02d' and \
datp <= '%d-%02d-%02d'",gn,1,1,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено проводок !"));
 }
else
 {
  double cel=kolstr;
  double drob=0.;

  drob=modf(cel,&drob);
  if(drob > 0.000001)
   {
    VVOD SOOB(1);
    sprintf(strsql,"%s !!!",gettext("Внимание"));
    SOOB.VVOD_SPISOK_add_MD(strsql);
    SOOB.VVOD_SPISOK_add_MD(gettext("Не парное количество проводок"));
    SOOB.VVOD_SPISOK_add_MD(gettext("Проверте базу данных на логическую целосность"));
    soobshw(&SOOB,stdscr,-1,-1,0,1);
   }
 }
printw("%s:%d\n",gettext("Количество проводок"),kolstr);
refresh();

int mpe=0;
short den,mes,god;
int kolstr1=0;
class iceb_tu_str shk("");
double sm=0.;
int i1=0;
if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  rsdat(&den,&mes,&god,row[0],2);
  deb=atof(row[4]);   
  kre=atof(row[5]);
  
  if(sravmydat(den,mes,god,dn,mn,gn)  < 0) /*до периода*/
   {
     mpe=0;
   }

  if(sravmydat(den,mes,god,dn,mn,gn)  >= 0) /*период*/
  if(sravmydat(den,mes,god,dk,mk,gk)  <= 0)
   {
    mpe=1;
   }

  if(row[3][0] != '\0')
      glkni_zvt(mpe,row[1],row[3],deb,kre,imatmptab);

  if(fabs(deb) > 0.009)
   {
    sh.new_plus(row[1]);
    shk.new_plus(row[2]);
    sm=deb;
   }
  else
   {
    sh.new_plus(row[2]);
    shk.new_plus(row[1]);
    sm=kre;
   }
  if(sh.ravno()[0] == '\0')
   {
    VVOD SOOB(1);
    sprintf(strsql,"%s %s %s %s %s %s",
    row[0],row[1],row[2],row[3],row[4],row[5]);
    SOOB.VVOD_SPISOK_add_MD(strsql);
    SOOB.VVOD_SPISOK_add_MD(gettext("Не введён счёт !"));
    soobshw(&SOOB,stdscr,-1,-1,0,1);

    continue;
   }
  if(shk.ravno()[0] == '\0')
   {
    VVOD SOOB(1);
    sprintf(strsql,"%s %s %s %s %s %s",
    row[0],row[1],row[2],row[3],row[4],row[5]);
    SOOB.VVOD_SPISOK_add_MD(strsql);
    SOOB.VVOD_SPISOK_add_MD(gettext("Не введён коресподирующий счёт !"));
    soobshw(&SOOB,stdscr,-1,-1,0,1);

    continue;
   }

  if((i=PROM.masf.find(sh.ravno(),msrv)) == -1)
   {
    VVOD SOOB(1);
    sprintf(strsql,"%s %s %s %s %s %s",
    row[0],row[1],row[2],row[3],row[4],row[5]);
    SOOB.VVOD_SPISOK_add_MD(strsql);
    sprintf(strsql,gettext("Не найден счёт %s в массиве счетов !"),sh.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);

    continue;
   }

  if((i1=PROM.masf.find(shk.ravno(),msrv)) == -1)
   {
    VVOD SOOB(1);
    sprintf(strsql,"%s %s %s %s %s %s",
    row[0],row[1],row[2],row[3],row[4],row[5]);
    SOOB.VVOD_SPISOK_add_MD(strsql);
    sprintf(strsql,gettext("Не найден счёт корреспондент %s в масиве счетов !"),shk.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);
    SOOB.VVOD_SPISOK_add_MD("rozrax");
    soobshw(&SOOB,stdscr,-1,-1,0,1);

    continue;
   }

  if(mpe == 0)
    PROM.sl.plus(sm,i*ks+i1);
  else
    PROM.pe.plus(sm,i*ks+i1);
  
/*Проверяем есть ли неправильные проводки*/
  if(ksrs != 0)
   if(PROM.msr.find(row[2],msrv) >= 0)
   {

    if(row[3][0] == '\0')
     {

      VVOD SOOB(1);
      sprintf(strsql,"%d.%d.%d %-5s %-5s %-5s %10s %10s",
      den,mes,god,row[1],row[2],row[3],row[4],row[5]);
      SOOB.VVOD_SPISOK_add_MD(strsql);
      SOOB.VVOD_SPISOK_add_MD(gettext("В проводке нет кода контрагента !"));
      SOOB.VVOD_SPISOK_add_MD("rozrax");
      soobshw(&SOOB,stdscr,-1,-1,0,1);

     } 
   }

  /*Проверяем есть ли счёт в списке счетов с развернутым сальдо*/
  if(ksrs != 0)
   if(PROM.msr.find(row[1],msrv) >= 0)
   {
   
    if(row[3][0] == '\0')
     {

      VVOD SOOB(1);
      sprintf(strsql,"%d.%d.%d %-5s %-5s %-5s %10s %10s",
      den,mes,god,row[1],row[2],row[3],row[4],row[5]);
      SOOB.VVOD_SPISOK_add_MD(strsql);
      SOOB.VVOD_SPISOK_add_MD(gettext("В проводке нет кода контрагента !"));
      SOOB.VVOD_SPISOK_add_MD("rozrax");
      soobshw(&SOOB,stdscr,-1,-1,0,1);

     } 


   }



 }

/*************************************************************/

printw("%s\n",gettext("Расчёт закончен"));
refresh();

/*Чистим нулевой год куда будут записаны сальдо*/
sprintf(strsql,"delete from Saldo where gs=0");

sql_zapis(strsql,0,0);



/************************/
/*Запись стартового сальдо*/
/************************/
/*Так как все проводки встречаются дважды то все делим на 2*/
for(i=0;i<ks*ks;i++)
  {
   if(PROM.sl.ravno(i) != 0.)
    PROM.sl.new_plus(PROM.sl.ravno(i)/2.,i);
   if(PROM.pe.ravno(i) != 0.)
    PROM.pe.new_plus(PROM.pe.ravno(i)/2.,i);
  }

printw("%s\n",gettext("Записываем стартовые сальдо"));
refresh();


di=ki=sd=sk=dpe=kpe=0.;
dns=kns=dpp=kpp=0.;
getyx(stdscr,Y,X);
short metkasaldo=0;
for(i=0; i <ks ; i++)
 {
  strzag(LINES-1,0,ks,i);
  
  sh.new_plus(PROM.masf.ravno(i));
  sprintf(strsql,"select saldo,nais,vids from Plansh where ns='%s'",sh.ravno());
  
  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
    sprintf(strsql,gettext("Нет счета %s в плане счетов !"),sh.ravno());
    iceb_t_soob(strsql);
    continue;
   }
  metkasaldo=atoi(row[0]);
  nais.new_plus(row[1]);  
  vids=atoi(row[2]);
  move(Y,X);
  printw("%-5s %s\n",sh.ravno(),nais.ravno());
  refresh();
  
  if(metkasaldo == 3)
   {
    ras2n(sh.ravno(),&dns,&kns,&dpp,&kpp,&di,&ki,imatmptab);
   }
  else
   ras1(i,sh.ravno(),nais.ravno(),&PROM,&dns,&kns,&dpp,&kpp,&di,&ki);

 }

printw("\n\
%*s:%15s",iceb_tu_kolbait(20,gettext("Сальдо начальное")),
gettext("Сальдо начальное"),prnbr(dns));
refresh();

printw(" %15s\n",prnbr(kns));
printw("%*s:%15s",iceb_tu_kolbait(20,gettext("Оборот за период")),gettext("Оборот за период"),prnbr(dpp));

printw(" %15s\n",prnbr(kpp));
printw("%*s:%15s",iceb_tu_kolbait(20,gettext("Сальдо конечное ")),gettext("Сальдо конечное "),prnbr(di));
printw(" %15s\n",prnbr(ki));

printw_vr(tmmn);

sprintf(strsql,"select ns from Plansh where stat=1");
if(readkey(strsql) > 0)
 {
  clearstr(LINES-1,0); /*вытираем строку ждите*/
  
  DANET.VVOD_delete();
  DANET.VVOD_SPISOK_add_MD("Перенести сальдо по небалансовым счетам ?");

  if(danet(&DANET,2,stdscr) == 1)
   {
    time(&tmmn);

    saldonbs(dn,mn,gn,dk,mk,gk);

    printw("\nБалансовые счета:\n\
%*s:%15s",iceb_tu_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),prnbr(dns));
    refresh();

    printw(" %15s\n",prnbr(kns));
    printw("%*s:%15s",iceb_tu_kolbait(20,gettext("Оборот за период")),gettext("Оборот за период"),prnbr(dpp));

    printw(" %15s\n",prnbr(kpp));
    printw("%*s:%15s",iceb_tu_kolbait(20,gettext("Сальдо конечное ")),gettext("Сальдо конечное "),prnbr(di));
    printw(" %15s\n",prnbr(ki));
    printw_vr(tmmn);
   }
 }

printw("%s\n",gettext("Чистим перед записью"));
refresh();

/*Чистим год куда будут записаны сальдо*/

sprintf(strsql,"delete from Saldo where gs=%d",godp);
sql_zapis(strsql,0,0);

printw("%s\n",gettext("Устанавливаем год"));
refresh();

/*Устанавливаем год куда будут записаны сальдо*/
sprintf(strsql,"update Saldo set gs=%d where gs=0",godp);
sql_zapis(strsql,0,0);


if(godp <= gt)
 {
  sprintf(strsql,"update Alx set str='Стартовый год|%d\n' where fil='nastrb.alx' and str like 'Стартовый год|%d%%'",
  godp,gn);

  if(sql_zapis(strsql,0,0) == 0)
    startgodb=godp;
 }

OSTANOV();
}

/***************************/
/*Распечатка обычных счетов*/
/***************************/

void ras1(int i,
const char *ns, //Счет
const char *nash,  //Наименование
class GLKTMP *PROM,
double *dns,
double *kns,
double *dpp,
double *kpp,
double *di,
double *ki)
{
short           i1;
short		mos; /*Метка оборотов по счетам*/
double		deb,kre;
char		strsql[512];
time_t		vrem;

deb=kre=0.;
mos=0;
double sd=0.,sk=0.;
double dpe=0.,kpe=0.;
int ks=PROM->masf.kolih();

for(i1=0;i1<ks;i1++)
 {
  sd+=PROM->sl.ravno(i*ks+i1);
  sk+=PROM->sl.ravno(i+ks*i1);

  dpe+=PROM->pe.ravno(i*ks+i1);
  kpe+=PROM->pe.ravno(i+ks*i1);
  if(fabs(PROM->pe.ravno(i*ks+i1)) > 0.009 || fabs(PROM->pe.ravno(i+ks*i1)) > 0.009)
    mos=1;
 }

if(mos == 0)
 {
  if(fabs(sd) < 0.01 && fabs(sk) < 0.01 && fabs(dpe) < 0.01 && fabs(kpe) < 0.01
  && fabs(PROM->ssd.ravno(i)) < 0.01 && fabs(PROM->ssk.ravno(i)) < 0.01)
    return;

  if(fabs(sd+PROM->ssd.ravno(i)-sk-PROM->ssk.ravno(i)) < 0.01 && fabs(dpe) < 0.01 && fabs(kpe) < 0.01)
   return;
 }

*dpp+=dpe;
*kpp+=kpe;
if(PROM->ssd.ravno(i)+sd + dpe >= PROM->ssk.ravno(i)+sk+kpe)
 {
  if(PROM->ssd.ravno(i)+sd >= PROM->ssk.ravno(i)+sk)
   {
    *dns+=(PROM->ssd.ravno(i)+sd)-(PROM->ssk.ravno(i)+sk);
   }
  else
   {
    *kns+=(PROM->ssk.ravno(i)+sk)-(PROM->ssd.ravno(i)+sd);
   }
  *di=*di+PROM->ssd.ravno(i)+sd+dpe - (PROM->ssk.ravno(i)+sk+kpe);
  deb=PROM->ssd.ravno(i)+sd+dpe - (PROM->ssk.ravno(i)+sk+kpe);
 }
else
 {
  if(PROM->ssd.ravno(i)+sd >= PROM->ssk.ravno(i)+sk)
   {
    *dns+=(PROM->ssd.ravno(i)+sd)-(PROM->ssk.ravno(i)+sk);
   }
  else
   {
    *kns+=(PROM->ssk.ravno(i)+sk)-(PROM->ssd.ravno(i)+sd);
   }
  *ki=*ki+PROM->ssk.ravno(i)+sk+kpe-(PROM->ssd.ravno(i)+sd+dpe);
  kre=PROM->ssk.ravno(i)+sk+kpe-(PROM->ssd.ravno(i)+sd+dpe);
 }

if(fabs(deb) > 0.009 || fabs(kre) > 0.009) 
 {

  time(&vrem);

  sprintf(strsql,"insert into Saldo values ('%s',%d,'%s','%s',%.2f,%.2f,%d,%ld)","0",0,ns,"",deb,kre,iceb_t_getuid(),vrem);   

  sql_zapis(strsql,0,0);
 }
}

/******************************************/
/*Работаем со счетами с развернутым сальдо*/
/******************************************/
void ras2n(const char *sh,
double *dns,
double *kns,
double *dpp,
double *kpp,
double *di,
double *ki,
const char *imatabl)
/****************
class SPISOK *ns_kontr,
class masiv_din_double *ns_kontr_debs,
class masiv_din_double *ns_kontr_kres,
class masiv_din_double *ns_kontr_debp,
class masiv_din_double *ns_kontr_krep)
**********************/
{
double          d1=0.,k1=0.;
double		debs=0.,kres=0.;
double		debp=0.,krep=0.;
double          sd=0.,sk=0.,dpe=0.,kpe=0.;
char strsql[1024];
SQL_str row;
SQLCURSOR cur;
double deb,kre;

/******************
for(int nomer_kontr=0; nomer_kontr < ns_kontr->kolih(); nomer_kontr++)
 {
  debs=ns_kontr_debs->ravno(nomer_kontr);
  kres=ns_kontr_kres->ravno(nomer_kontr);
  debp=ns_kontr_debp->ravno(nomer_kontr);
  krep=ns_kontr_krep->ravno(nomer_kontr);

  if(debs > kres)
   sd+=debs-kres;
  else
   sk+=kres-debs;
  deb=kre=0.;  
  if(debs+debp > kres+krep)
   {
    d1+=debs+debp-kres-krep;  
    deb=debs+debp-kres-krep;  
   }
  else
   {
    k1+=kres+krep-debs-debp;
    kre=kres+krep-debs-debp;
   }

  if(fabs(deb) > 0.009 || fabs(kre) > 0.009) 
    {
     sprintf(strsql,"insert into Saldo \
values ('%s',%d,'%s','%s',%.2f,%.2f,%d,%ld)",
     "1",0,sh,ns_kontr->ravno(nomer_kontr),deb,kre,iceb_t_getuid(),time(NULL));   

     sql_zapis(strsql,0,0);
    }

  dpe+=debp;
  kpe+=krep;
 }
*****************/

sprintf(strsql,"select debs,kres,debp,krep,kod_kontr from %s where shet='%s'",imatabl,sh);

if(cur.make_cursor(&bd,strsql) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {

  debs=atof(row[0]);
  kres=atof(row[1]);
  debp=atof(row[2]);
  krep=atof(row[3]);

  if(debs > kres)
   sd+=debs-kres;
  else
   sk+=kres-debs;

  deb=kre=0.;  
  if(debs+debp > kres+krep)
   {
    d1+=debs+debp-kres-krep;  
    deb=debs+debp-kres-krep;  
   }
  else
   {
    k1+=kres+krep-debs-debp;
    kre=kres+krep-debs-debp;
   }
  if(fabs(deb) > 0.009 || fabs(kre) > 0.009) 
    {
     sprintf(strsql,"insert into Saldo \
values ('%s',%d,'%s','%s',%.2f,%.2f,%d,%ld)",
     "1",0,sh,row[4],deb,kre,iceb_t_getuid(),time(NULL));   

     sql_zapis(strsql,0,0);
    }

  dpe+=debp;
  kpe+=krep;
 }


*dns+=sd;
*kns+=sk;

*dpp+=dpe;
*kpp+=kpe;

*di+=d1; 
*ki+=k1;


}

