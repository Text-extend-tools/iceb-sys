/* $Id: salork.c,v 5.54 2013/09/26 09:43:55 sasa Exp $ */
/*27.07.2016    22.01.1996      Белых А.И.      salork.c
Получение сальдо по оpганизации и счету
*/
#include        <errno.h>
#include        "buhl.h"

void  salork_akt_sv(class akt_sverki *akt_svr,short dn,short mn,short gn,short dk,short mk,short gk,const char *kor,const char *nai,const char *imaf,const char *imaf_bi);


void salork(const char *sh, //Счет
const char *kor,  //Код контрагента
double *db,  //Дебет
double *kr,  //Дебет
short rp,    //*0-без распечатки 1-с распечаткой*/
char *imaf, //Имя файла
char *imafact, //Имя файла с актом сверки
const char *nai,  //Наименование контрагента
short dn,short mn,short gn, //Дата начала
short dk,short mk,short gk, //Дата конца
const char *shv,
short mks, //*0-все проводки 1-без взаимно кореспондирующих*/
char *imaf_act, /*с месячными итогами*/
char *imaf_act_bi) /*без месячных итогов*/
{
time_t          tmm;
struct  tm      *bf;
short           d,m,g;
class iceb_fopen ff1;
class iceb_fopen ffact;
double          brr;
class iceb_tu_str shh("");
double          ddn,kkn;
double		dd,kk;
short           msv; /*0-без сверки 1-со сверкой*/
short           mvp; /*Метка второго прохода*/
short           mvs; /*Метка выдачи сальдо*/
char		strsql[512];
SQL_str         row;
long		kolstr;
short		godn;
short		shett=0;
short		mes=0,god=0;
double		deb=0.,kre=0.;
double		debm=0.,krem=0.;
class iceb_tu_str koment("");
short	startgodb=0; /*Стартовый год*/


if(iceb_t_poldan("Стартовый год",strsql,"nastrb.alx") == 0)
 startgodb=atoi(strsql);

godn=startgodb;
if(startgodb == 0 || startgodb > gn)
 godn=gn;

*db=0.;
*kr=0.;
mvp=msv=0;
if(shv[0] != '\0')
  msv=1;

shh.new_plus(sh);
if(rp == 1)
 {
  sprintf(imaf,"slo%d.lst",getpid());
  if(ff1.start(imaf,"w") != 0)
   return;
/*****************
  if((ff1 = fopen(imaf,"w")) == NULL)
   {
    error_op_nfil(imaf,errno,"");
    return;
   }
*****************/
  sprintf(imafact,"act%d.lst",getpid());
  if(ffact.start(imafact,"w") != 0)
   return;
/***************
  if((ffact = fopen(imafact,"w")) == NULL)
   {
    error_op_nfil(imafact,errno,"");
    return;
   }
***************/
 }

naz:;

ddn=kkn=0.;
dd=kk=0.;
sprintf(strsql,"select deb,kre from Saldo where kkk='1' and gs=%d and \
ns='%s' and kodkon='%s'",godn,shh.ravno(),kor);
SQLCURSOR cur;
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  ddn=atof(row[0]);
  kkn=atof(row[1]);
 }

if(rp == 1)
 {
  time(&tmm);
  bf=localtime(&tmm);
//  fprintf(ff1.ff,"\x0F");
  fprintf(ff1.ff,"%s\n\
%s %s %s %s %s\n\
     %s %d.%d.%d%s %s %d.%d.%d%s\n\
%s %d.%d.%d  %s - %d:%d\n",
iceb_t_get_pnk("00",0),
gettext("Распечатка проводок по счёту"),
shh.ravno(),
gettext("для контрагента"),
kor,nai,
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("Время"),
bf->tm_hour,bf->tm_min);

  fprintf(ff1.ff,"\
--------------------------------------------------------------------------------------------------\n");

fprintf(ff1.ff,gettext("  Дата    |Сче.к.|Записал|Номер доку.|    Дебет   |   Кредит   |      К о м е н т а р и й       |\n"));
/*******
                                                                                               12345678901234567890123456789012
********/
  fprintf(ff1.ff,"\
--------------------------------------------------------------------------------------------------\n");


  //Заголовок акта сверки
  zagacts(dn,mn,gn,dk,mk,gk,bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,kor,nai,ffact.ff);
  zagsh(sh,ffact.ff);
  


 }
/*
printw("%d.%d.%d %d.%d.%d\n",dn,mn,gn,dk,mk,gk);
refresh();
*/
mvs=0;
g=godn;
m=1;
d=1;
class akt_sverki akt_sv;
akt_sv.shet.new_plus(sh);

int nomer=0;

while(sravmydat(d,m,g,dk,mk,gk) <= 0)
  {
/*
   printw("%d.%d.%d\n",d,m,g);
   refresh();
  */
   if(sravmydat(d,m,g,dn,mn,gn) >= 0 && mvs == 0)
    {
     akt_sv.dn=d;
     akt_sv.mn=m;
     akt_sv.gn=g;
     if(ddn > kkn)    
      {
       akt_sv.start_saldo_deb=ddn-kkn;
       ddn=ddn-kkn;
       kkn=0.;
       if(rp == 1)
        {
         fprintf(ff1.ff,"%*s:%12s\n",
         iceb_tu_kolbait(37,gettext("Сальдо начальное")),gettext("Сальдо начальное"),
         prnbr(ddn));
         
         printw("%*s:%12s\n",
         iceb_tu_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),prnbr(ddn));

         fprintf(ffact.ff,"%02d.%02d.%4d %-20.20s %10.2f %10s| |\n",
         d,m,g,gettext("Сальдо начальное"),ddn," ");

         fprintf(ffact.ff,"\
------------------------------------------------------ ---------------------------------------------------\n");

        }
      }
     else
      {
       akt_sv.start_saldo_kre=kkn-ddn;
       kkn=kkn-ddn;
       ddn=0.;
       if(rp == 1)
        {
         fprintf(ff1.ff,"\
%*s:%12s %12s\n",
         iceb_tu_kolbait(37,gettext("Сальдо начальное")),gettext("Сальдо начальное")," ",prnbr(kkn));
         printw("\
%*s:%12s %12s\n",
         iceb_tu_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное")," ",prnbr(kkn));

         fprintf(ffact.ff,"%02d.%02d.%4d %-*.*s %10s %10.2f| |\n",
         d,m,g,
         iceb_tu_kolbait(20,gettext("Сальдо начальное")),iceb_tu_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),
         " ",kkn);

         fprintf(ffact.ff,"\
------------------------------------------------------ ---------------------------------------------------\n");
        }
      }     

     mvs=1;
    }

   sprintf(strsql,"select * from Prov \
where val=0 and datp='%d-%02d-%02d' and sh='%s' and kodkon='%s'",
   g,m,d,shh.ravno(),kor);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     msql_error(&bd,"",strsql);
     dpm(&d,&m,&g,1);
     continue;
    }
   //printw("rozden-kolstr=%d\nsrtsql=%s\n",kolstr,strsql);
   //getch();
   if(kolstr == 0)
    {
     dpm(&d,&m,&g,1);
     continue;
    }
   shett=0;
   while(cur.read_cursor(&row) != 0)
    {

     if(mks == 1)
       if(SRAV(row[2],sh,1) == 0 && SRAV(row[3],shv,1) == 0)
         continue;
     deb=atof(row[9]);
     kre=atof(row[10]);

     /*printw("%d.%d.%d %.2f %.2f\n",bh2n.god,bh2n.mes,bh2n.den,ddn,kkn);*/
     if(sravmydat(d,m,g,dn,mn,gn) >= 0)
     if(sravmydat(dk,mk,gk,d,m,g) >= 0)
      {
       if(rp == 1)
        {
         if(shett == 0)
          {
     //      if(mes != m)
           if(sravmydat(1,m,g,1,mes,god) != 0)
            {
             if(mes != 0)
              {
               salorksl(dd,kk,ddn,kkn,debm,krem,ff1.ff,ffact.ff);
               debm=krem=0.;
              }
             mes=m;
             god=g;
            }
           shett=1;
          }

         debm+=deb;
         krem+=kre;

         fprintf(ff1.ff,"%02d.%02d.%4d %-*s %-3s%4s %-*s %12s",
         d,m,g,
         iceb_tu_kolbait(6,row[3]),row[3],
         row[5],row[11],
         iceb_tu_kolbait(11,row[6]),row[6],
         prnbr(deb));

         fprintf(ff1.ff," %12s %-*.*s\n",prnbr(kre),iceb_tu_kolbait(32,row[13]),iceb_tu_kolbait(32,row[13]),row[13]);

         koment.new_plus(row[6]);
         koment.plus(" ",row[13]);
         
         fprintf(ffact.ff,"%02d.%02d.%4d %-*.*s %10.2f %10.2f| |\n",
         d,m,g,
         iceb_tu_kolbait(20,koment.ravno()),iceb_tu_kolbait(20,koment.ravno()),koment.ravno(),
         deb,kre);

         //заряжаем массивы
         sprintf(strsql,"%d.%d.%d|%s",d,m,g,row[6]);
         if(row[6][0] == '\0')
           nomer=-1;
         else
           nomer=akt_sv.data_nomd.find(strsql);

         if(nomer < 0)
          {
           akt_sv.data_nomd.plus(strsql);
           akt_sv.koment.plus(koment.ravno());
          }
         akt_sv.suma_deb.plus(deb,nomer);
         akt_sv.suma_kre.plus(kre,nomer);
         
         fprintf(ffact.ff,"\
------------------------------------------------------ ---------------------------------------------------\n");

        }
       dd+=deb;
       kk+=kre;

      }

     if(sravmydat(d,m,g,dn,mn,gn) < 0)
      {
       ddn+=deb;
       kkn+=kre;
      }

    }
   dpm(&d,&m,&g,1);
  }

/*
printw("Конец\n");
refresh();
*/

if(rp == 1)
 {
  brr=dd+ddn;

  printw("%*s:%12s",
  iceb_tu_kolbait(20,gettext("Оборот за период")),gettext("Оборот за период"),prnbr(dd));

  printw(" %12s\n",prnbr(kk));

  if(dd+ ddn >= kk+kkn)
   {
    brr=(dd+ddn)-(kk+kkn);
    printw("%*s:%12s\n",iceb_tu_kolbait(20,gettext("Сальдо конечное")),gettext("Сальдо конечное"),prnbr(brr));
   }
  else
   {
    brr=(kk+kkn)-(dd+ddn);
    printw("%*s:%12s %12s\n",
    iceb_tu_kolbait(20,gettext("Сальдо конечное")),gettext("Сальдо конечное")," ",prnbr(brr));
   }
  salorksl(dd,kk,ddn,kkn,debm,krem,ff1.ff,ffact.ff);

  if(shv[0] == '\0')
   {
    podpis(ff1.ff);
//    fprintf(ff1.ff,"\x12");
//    fclose(ff1.ff);
    ff1.end();
    konact(ffact.ff);
    ffact.end(); 
//    fclose(ffact);
   }

  sprintf(imaf_act,"asv%d.lst",getpid());
  sprintf(imaf_act_bi,"asvbi%d.lst",getpid());

  salork_akt_sv(&akt_sv,dn,mn,gn,dk,mk,gk,kor,nai,imaf_act,imaf_act_bi);
 }

*db+=dd+ddn;
*kr+=kk+kkn;
if(shv[0] != '\0' && mvp == 0)
 {
  shh.new_plus(shv);
  mvp=1;
  goto naz;
 }

if(rp == 1 && msv == 1)
 {
  fprintf(ff1.ff,"\
---------------------------------------------------------------------------------------\n\
%*s:%12s",
  iceb_tu_kolbait(37,gettext("Общий итог")),gettext("Общий итог"),prnbr(*db));
  fprintf(ff1.ff,"\
 %12s\n",prnbr(*kr));

  if(*db >= *kr)
   {
    brr=*db-*kr;
    fprintf(ff1.ff,"\
%37s %12s\n"," ",prnbr(brr));
   }
  else
   {
    brr=*kr-*db;
    fprintf(ff1.ff,"\
%37s %12s %12s\n"," "," ",prnbr(brr));
   }
  podpis(ff1.ff);
  ff1.end();
  konact(ffact.ff);
  ffact.end();

   
 }
}
/**************/
/*Вывод сальдо*/
/**************/
void salorksl(double dd, double kk,double ddn,double kkn,
double debm,double krem,
FILE *ff1,FILE *ffact)
{

double brr=dd+ddn;
double krr=kk+kkn;
fprintf(ff1,"\
---------------------------------------------------------------------------------------\n");

if(debm != 0. || krem != 0.)
 {
  fprintf(ff1,"\
%*s:%12s",iceb_tu_kolbait(37,gettext("Оборот за месяц")),gettext("Оборот за месяц"),prnbr(debm));

  fprintf(ff1," %12s\n",prnbr(krem));
  if(ffact != NULL)
   fprintf(ffact,"%*s %10.2f %10.2f| |\n",
   iceb_tu_kolbait(31,gettext("Оборот за месяц")),gettext("Оборот за месяц"),debm,krem);
 }

fprintf(ff1,"%*s:%12s",
iceb_tu_kolbait(37,gettext("Оборот за период")),
gettext("Оборот за период"),
prnbr(dd));

fprintf(ff1," %12s\n",prnbr(kk));

if(ffact != NULL)
 fprintf(ffact,"%*s %10.2f %10.2f| |\n",
 iceb_tu_kolbait(31,gettext("Оборот за период")),gettext("Оборот за период"),dd,kk);

fprintf(ff1,"%*s:%12s",
iceb_tu_kolbait(37,gettext("Сальдо раз-нутое")),
gettext("Сальдо раз-нутое"),
prnbr(brr));
fprintf(ff1," %12s\n",prnbr(brr));

if(ffact != NULL)
 fprintf(ffact,"%*s %10.2f %10.2f| |\n",
 iceb_tu_kolbait(31,gettext("Сальдо раз-нутое")),gettext("Сальдо раз-нутое"),brr,krr);

if(dd+ ddn >= kk+kkn)
 {
  brr=(dd+ddn)-(kk+kkn);
  fprintf(ff1,"%*s:%12s\n",
  iceb_tu_kolbait(37,gettext("Сальдо свернутое")),gettext("Сальдо свернутое"),prnbr(brr));

  if(ffact != NULL)
   fprintf(ffact,"%*s %10.2f %10s| |\n",
   iceb_tu_kolbait(31,gettext("Сальдо свернутое")),gettext("Сальдо свернутое"),brr," ");
 }
else
 {
  brr=(kk+kkn)-(dd+ddn);
  fprintf(ff1,"%*s:%12s %12s\n",
  iceb_tu_kolbait(37,gettext("Сальдо свернутое")),gettext("Сальдо свернутое")," ",prnbr(brr));


  if(ffact != NULL)
   fprintf(ffact,"%*s %10s %10.2f| |\n",
   iceb_tu_kolbait(31,gettext("Сальдо свернутое")),gettext("Сальдо свернутое")," ",brr);

 }
}
/*****************************/
/*Заголовок акта сверки      */
/*****************************/

void   zagacts(short dn,short mn,short gn,
short dk,short mk,short gk,
short d,short m,short g,
const char *kodkontr,const char *naimkont,FILE *ff)
{


fprintf(ff,gettext("\
                                       АКТ СВЕРКИ ВЗАИМНЫХ РАСЧЕТОВ.\n\n\
   Мы ниже подписавшиеся, главный бухгалтер %s\n\
________________________________________________________________________________________________________\n\
                    (наименование учереждения, фамилия и инициалы)\n\
и главный бухгалтер %s\n\
_________________________________________________________________________________________________________\n\
                   (наименование учереждения, фамилия и инициалы)\n\
составили настоящий акт о том, что проведена проверка взаимных расчётов.\n"),
iceb_t_get_pnk("00",0),naimkont);

fprintf(ff,"%s %02d.%02d.%d%s %s %02d.%02d.%d%s\n",
gettext("За период с"),
dn,mn,gn,
gettext("г."),
gettext("по"),
dk,mk,gk,
gettext("г."));

fprintf(ff,"%s:%02d.%02d.%d %s\n\n",
gettext("По состоянию на"),
d,m,g,gettext("г."));

fprintf(ff,"%s:\n",gettext("При проверке установлено"));

fprintf(ff,"\
%s %-51s %s %s %s\n",
gettext("По"),
iceb_t_get_pnk("00",0),
gettext("По"),
kodkontr,naimkont);



}
/*********************************/
/*Концовка акта сверки           */
/*********************************/

void	konact(FILE *ff)
{
fprintf(ff,"\n");
fprintf(ff,gettext("\
После отражения в учете указанных дополнительных проводок сальдо на ________________________\n\
выражается в сумме__________________________________________________________________________\n\
____________________________________________________________________________________________\n\
                  (сумма прописью и цифрами)\n\
в пользу____________________________________________________________________________________\n\
                   (Указать наименование учереждения)\n\n"));


fprintf(ff,"\
%*s________________________    %*s________________________\n\n",
iceb_tu_kolbait(25,gettext("Руководитель")),
gettext("Руководитель"),
iceb_tu_kolbait(25,gettext("Руководитель")),
gettext("Руководитель"));


fprintf(ff,"\
%*s________________________    %*s________________________\n",
iceb_tu_kolbait(25,gettext("Главный бухгалтер")),
gettext("Главный бухгалтер"),
iceb_tu_kolbait(25,gettext("Главный бухгалтер")),
gettext("Главный бухгалтер"));

                                     
fprintf(ff,"\n");

SQL_str row;
SQLCURSOR cur;
class iceb_tu_str telef("");
if(readkey("select telef from Kontragent where kodkon='00'",&row,&cur) == 1)
 telef.new_plus(row[0]);

fprintf(ff,gettext("\
Вышлите нам, пожалуйста, один экземпляр акта с Вашими подписью и печатью.\n\
В случае несоответствия сумм, просим выслать розшифровку или провести сверку\n\
по телефону"));

fprintf(ff," %s\n",telef.ravno()); 

fprintf(ff,gettext("\
Просим в течении 5 дней вернуть подписанный акт.\n\
При невозвращении акта, сумма будет зачтена, как согласованная."));

podpis(ff);

}
/*********************************/
/*Шака                           */
/*********************************/

void	zagsh(const char *shet,FILE *ff)
{
char	strsql[512];
SQL_str row;
SQLCURSOR cur;
class iceb_tu_str naim("");

sprintf(strsql,"select nais from Plansh where ns='%s'",shet);
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 naim.new_plus(row[0]);
 
fprintf(ff,"\n%s:%s %s\n",gettext("Счет"),shet,naim.ravno());
fprintf(ff,"\
------------------------------------------------------ ---------------------------------------------------\n");
fprintf(ff,gettext("\
  Дата    |   Коментарий       | Дебет    | Кредит   | |  Дата    |   Коментарий       |  Дебет | Кредит |\n"));

fprintf(ff,"\
------------------------------------------------------ ---------------------------------------------------\n");

}
/**********************************/
/*Распечатка акта сверки с общей суммой по документу*/
/*****************************************************/
void  salork_akt_sv(class akt_sverki *akt_svr,short dn,short mn,short gn,
short dk,short mk,short gk,
const char *kor,const char *nai,
const char *imaf,
const char *imaf_bi)
{
class iceb_fopen ff;
class iceb_fopen ff_bi;

if(ff.start(imaf,"w") != 0)
 return;
if(ff_bi.start(imaf_bi,"w") != 0)
 return;

short dt,mt,gt;
poltekdat(&dt,&mt,&gt);

zagacts(dn,mn,gn,dk,mk,gk,dt,mt,gt,kor,nai,ff.ff);
zagacts(dn,mn,gn,dk,mk,gk,dt,mt,gt,kor,nai,ff_bi.ff);

akt_sv(akt_svr,ff.ff,ff_bi.ff);

konact(ff.ff);

podpis(ff.ff);
ff.end();

konact(ff_bi.ff);

podpis(ff_bi.ff);
ff_bi.end();

}
