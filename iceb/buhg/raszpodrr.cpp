/* $Id: raszpodrr.c,v 5.26 2013/09/26 09:43:39 sasa Exp $ */
/*16.04.2017	17.04.2000	Белых А.И.	raszpodrr.c
Расчёт свода начислений и удержаний по подразделениям на основе
указанных в начислениях кодах подразделений
*/
#include        <errno.h>
#include        "buhg.h"

extern short	*obud; /*Обязательные удержания*/

void raszpodrr(short mn,short gn,const char podr[],const char kateg[],
const char tabnom[],FILE *ff,FILE *ffsh)
{
short		kolnah;  /*Количество начислений*/
short		koluder;   /*Количество удержаний*/
double		inah;    /*Итого начислено*/
double		iuder;    /*Итого удержано с учётом сальдо*/
double		iuderb;    /*Итого удержано без учёта сальдо*/
double		kvid;    /*К выдаче с учётом сальдо*/
double		kvidb;   /*К выдаче без учёта сальдо*/
double		saldont;  /*Сальдо на начало периода по табельному номеру*/
double		inaht;    /*Итого начислено по табельному номеру*/
double		iudert;    /*Итого удержано по табельному номеру*/
double		iudertb;    /*Итого удержано по табельному номеру без учёта сальдо*/
double		kvidt;    /*К выдаче по табельному номеру c учетом сальдо*/
double		kvidtb;    /*К выдаче по табельному номеру без учёта сальдо*/
double		inahp;    /*Итого начисленное по подразделению*/
double		dolg,dolgb;
int		i,i1;
char		strsql[1024];
class iceb_tu_str strok("");
class iceb_tu_str shet("");
SQL_str         row,row1;
time_t		tmm;
struct  tm      *bf;
short		dkm;
int		kolstr,kolstr1,kolstr2;
long		tabn;
int		kpodr; /*Подразделение в котором начислена сумма*/
int             podro; /*Основное подразделение*/
short		prn;
double		sum;
short		knah;
double		koff;
class iceb_tu_str naim("");
double		bb;
double		sumaub;
double		saldb;
short		metkaou; /*0-обязательное 1-не обяз. удержание*/
double sum1;
double bb1=0.;
int		klst=0;
SQLCURSOR curr;
GDITE();
time(&tmm);
bf=localtime(&tmm);

printw(gettext("Расчёт свода начислений и удержаний по подразделениям."));
printw("\n%s %d.%d %s %d.%d\n",gettext("Период с"),mn,gn,
gettext("до"),mn,gn);
GDITE();

/*Определяем количество начислений и удержаний*/

sprintf(strsql,"select kod from Nash");
SQLCURSOR cur;
SQLCURSOR cur1;
if((kolnah=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
if(kolnah == 0)
 {
  iceb_t_soob(gettext("Не введены начисления !"));
  return;
 }
short nah[kolnah];

i=0;
while(cur.read_cursor(&row) != 0)
  nah[i++]=atoi(row[0]);

double snah[kolnah];
memset(snah,'\0',sizeof(snah));


sprintf(strsql,"select kod from Uder");
if((koluder=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
if(koluder == 0)
 {
  iceb_t_soob(gettext("Не введены удержания !"));
  return;
 }
short uder[koluder];
i=0;
while(cur.read_cursor(&row) != 0)
  uder[i++]=atoi(row[0]);

double suder[koluder];
double suderb[koluder];
memset(suder,'\0',sizeof(suder));
memset(suderb,'\0',sizeof(suderb));


/*Определяем начисления/счет удержания/счет */

sprintf(strsql,"select distinct prn,knah,shet from Zarp \
where datz >= '%d-%d-01' and datz <= '%d-%d-31' and suma <> 0.",
gn,mn,gn,mn);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
//Создаем классы
class masiv_din_double NASH_SUM;
class masiv_din_double UDER_SUM;
class masiv_din_double UDER_SUMB;
UDER_SUM.make_class(kolstr);
UDER_SUMB.make_class(kolstr);
NASH_SUM.make_class(kolstr);

class SPISOK NASH_SH; //Начисления/счет
class SPISOK UDER_SH; //Удержания/счет

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s,%s",row[1],row[2]);
  if(row[0][0] == '1')
     NASH_SH.plus(strsql);
  if(row[0][0] == '2')
     UDER_SH.plus(strsql);
 }
//UDER_SH.print_masiv_char(strsql); 

dpm(&dkm,&mn,&gn,5);

iceb_t_zagolov(gettext("Расчёт свода начислений и удержаний по подразделениям."),1,mn,gn,dkm,mn,gn,ff);

if(podr[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Коды подразделений вошедшие в расчёт"),podr);
  fprintf(ffsh,"%s:%s\n",gettext("Коды подразделений вошедшие в расчёт"),podr);
  printw("%s:%s\n",gettext("Коды подразделений вошедшие в расчёт"),podr);
  printcod(ff,"Podr","kod","naik",0,podr,&klst);
  printcod(ffsh,"Podr","kod","naik",0,podr,&klst);
 }
else
 {
  fprintf(ff,gettext("По всем подразделениям.\n"));
  fprintf(ffsh,gettext("По всем подразделениям.\n"));
  printw(gettext("По всем подразделениям.\n"));
 }
if(kateg[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Коды категорий вошедшие в расчёт"),kateg);
  fprintf(ffsh,"%s:%s\n",gettext("Коды категорий вошедшие в расчёт"),kateg);
  printw("%s:%s\n",gettext("Коды категорий вошедшие в расчёт"),kateg);
 }

if(tabnom[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Табельный номер"),tabnom);
  fprintf(ffsh,"%s:%s\n",gettext("Табельный номер"),tabnom);
  printw("%s:%s\n",gettext("Табельный номер"),tabnom);
 }
/*******************************************************/
/*                 Выполняем расчёт                    */
/*******************************************************/

sprintf(strsql,"select distinct podr,tabn from Zarp where \
datz >= '%d-%d-01' and datz <= '%d-%d-31' and suma <> 0.",
gn,mn,gn,mn);
//printw("%s\n",strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

if(kolstr == 0)
 {
  fprintf(ff,"%s\n",gettext("Не найдено ни одной записи !"));
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return;
 }

dolg=dolgb=inah=iuder=iuderb=kvid=kvidb=0.;
inahp=saldont=inaht=iudert=iudertb=kvidt=kvidtb=0.;
tabn=kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(proverka(podr,row[0],0,0) != 0)
    continue;
  if(proverka(tabnom,row[1],0,0) != 0)
    continue;
//  printw("%-4s %s\n",row[1],row[0]);

  kpodr=atoi(row[0]);
  tabn=atol(row[1]);

  /*Узнаем какая категория и основное подразделение*/
  sprintf(strsql,"select kateg,podr from Zarn where god=%d and mes=%d \
and tabn=%ld",gn,mn,tabn);

  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    printw("Не найдена запись настройки за %d.%d для %ld !\n",mn,gn,tabn);
    OSTANOV();
    continue;
   }

  podro=atoi(row1[1]);
  if(kpodr == 0)
   kpodr=podro;
   
  if(kateg[0] != '\0')
    if(pole1(kateg,row1[0],',',0,&i1) != 0)
     if(SRAV(kateg,row1[0],0) != 0)
      continue;


  /*Определяем сальдо на начало периода*/
  saldont=zarsald(1,mn,gn,tabn,&saldb);

  sprintf(strsql,"select prn,knah,suma,podr,godn,mesn,datz,shet \
from Zarp where datz >= '%d-%d-01' and datz <= '%d-%d-31' and \
tabn=%ld and suma <> 0.",gn,mn,gn,mn,tabn);
//  printw("%s\n",strsql);
  
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }
  if(kolstr2 == 0)
    continue;

  /*Определяем коэффициент начисления*/
  inahp=0.;
  while(cur1.read_cursor(&row1) != 0)
   {
//    printw("%s %s %s %s\n",row1[0],row1[1],row1[2],row1[3]);
    
    prn=atoi(row1[0]);
    sum=atof(row1[2]);
    
    if(prn == 1)
     {
      inaht+=sum;
      if(kpodr == atoi(row1[3]))
       inahp+=sum;

     }
   }  
  koff=0.;
  if(inahp != 0.)
    koff=inaht/inahp;
/*
  printw("%ld koff=%.2f/%.2f=%f\n",tabn,inaht,inahp,koff);
  refresh();
*/
  cur1.poz_cursor(0);
  while(cur1.read_cursor(&row1) != 0)
   {
    prn=atoi(row1[0]);
    knah=atoi(row1[1]);
    sum=atof(row1[2]);
    
    if(prn == 1)
     {

      if(kpodr != atoi(row1[3]))
       continue;
        
      for(i=0; i < kolnah ; i++)
       if(knah == nah[i])
        {
         snah[i]+=sum;
         break;
        }
      sprintf(strsql,"%s,%s",row1[1],row1[7]);
      if((i1=NASH_SH.find(strsql)) >= 0)
        NASH_SUM.plus(sum,i1);
     }

    if(prn == 2)
     {
      /*Усли удержание не относится к обязательным удержаниям то
       проверяем код подразделения
       */
      metkaou=0;      
      if(obud != NULL)
       {
        for(i1=1; i1<= obud[0]; i1++)
         if(obud[i1] == knah)
          {
           break;
          }
        if(i1 > obud[0]) /*Код не обязательное удержание*/
          {
           if(kpodr != atoi(row1[3]))
            continue;
           metkaou=1;
          }         
       }
       
      sprintf(strsql,"%s,%s",row1[1],row1[7]);
      int i22;
      i22=UDER_SH.find(strsql);

      iudert+=sum;
      for(i=0; i < koluder ; i++)
       if(knah == uder[i])
        {
         if(metkaou == 0)
          {
           if(koff != 0.)
            {
              suder[i]+=sum/koff;
              UDER_SUM.plus(sum/koff,i22);
            }
           else
            {
             printw("%s %ld\n",
             gettext("Табельный номер"),tabn);
             printw("%s %d !\n (%d/%.2f/%.2f)\n",
             gettext("Нет ни одного не обязательного удержания для подразделения"),
             kpodr,knah,inaht,inahp);
             OSTANOV();
            }
          }
         else
          {
           suder[i]+=sum;
           UDER_SUM.plus(sum,i22);
          }
         if(atoi(row1[4]) == gn && atoi(row1[5]) == mn)
          {
           iudertb+=sum;
           if(metkaou == 0 && koff != 0.)
            {
             suderb[i]+=sum/koff;
             UDER_SUMB.plus(sum/koff,i22);
            }
           else
            {
             suderb[i]+=sum;
             UDER_SUMB.plus(sum,i22);
            }
          }
         else
          {
           if(obud != NULL)
            for(i1=1; i1<= obud[0]; i1++)
             if(obud[i1] == knah)
              {
               iudertb+=sum;
               if(metkaou == 0 && koff != 0.)
                {
                 UDER_SUMB.plus(sum/koff,i22);
                 suderb[i]+=sum/koff;
                }
               else
                {
                 suderb[i]+=sum;
                 UDER_SUMB.plus(sum,i22);
                }
               break;

              }
          }

         break;        
        }
      }    
   }

  
  if(koff != 0.)
   {
    kvidt=(saldont+inaht+iudert)/koff;
    bb=prbper(mn,gn,bf->tm_mon+1,bf->tm_year+1900,tabn,&sumaub,0);
    kvidtb=(inaht+iudertb+bb)/koff;
   }

  if(kvidt > 0.)
    kvid+=kvidt;
  else
   dolg+=kvidt;

  if(kvidtb > 0.)
    kvidb+=kvidtb;
  else
   dolgb+=kvidtb;

  inahp=saldont=inaht=iudert=iudertb=kvidt=kvidtb=0.;


 }


/******************************************/
/*       Распечатываем                    */
/******************************************/
fprintf(ff,"\
----------------------------------------------\n");
fprintf(ff,gettext("Код|  Наименование начисления     |   Сумма  |\n"));
fprintf(ff,"\
----------------------------------------------\n");
inah=0.;
for(i=0 ; i < kolnah; i++)
 {
  if(snah[i] == 0.)
   continue;
  sprintf(strsql,"select naik from Nash where kod=%d",nah[i]);

  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    beep();
    printw("%s %d !\n",
    gettext("Не найден код начисления"),nah[i]);
    OSTANOV();
    naim.new_plus("");    
   }
  else
   naim.new_plus(row1[0]);
  fprintf(ff,"%3d %-*.*s %10.2f\n",
  nah[i],
  iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
  snah[i]);
  inah+=snah[i];
 }


fprintf(ff,"\
----------------------------------------------\n");
fprintf(ff,"%*s:%10.2f\n",iceb_tu_kolbait(34,gettext("Итого")),gettext("Итого"),inah);


fprintf(ff,"\
---------------------------------------------------------\n");
fprintf(ff,gettext("Код|  Наименование удержания      |   Сумма  |Сумма б.с.|\n"));
fprintf(ff,"\
---------------------------------------------------------\n");
iuderb=iuder=0.;

for(i=0 ; i < koluder; i++)
 {

  if(suder[i] == 0.)
   continue;

  sprintf(strsql,"select naik from Uder where kod=%d",uder[i]);

  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    beep();
    printw("%s %d !\n",
    gettext("Не найден код удержания"),uder[i]);
    OSTANOV();
    naim.new_plus("");    
   }
  else
   naim.new_plus(row1[0]);

  fprintf(ff,"%3d %-*.*s %10.2f %10.2f\n",
  uder[i],
  iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
  suder[i],suderb[i]);
  iuder+=suder[i];
  iuderb+=suderb[i];

 }

fprintf(ff,"\
---------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f %10.2f\n",iceb_tu_kolbait(34,gettext("Итого")),gettext("Итого"),iuder,iuderb);

fprintf(ff,"\n%*s:%10.2f\n",iceb_tu_kolbait(34,gettext("К выдаче с учётом сальдо")),gettext("К выдаче с учётом сальдо"),kvid);
fprintf(ff,"%*s:%10.2f\n",iceb_tu_kolbait(34,gettext("К выдаче без учёта сальдо")),gettext("К выдаче без учёта сальдо"),inah+iuderb);

printw("\
%s%10.2f\n\
%s%10.2f\n\
%s%10.2f\n\
%s%10.2f\n\
%s:%10.2f\n\
%s:%10.2f\n",
gettext("Итого начислено:"),inah,
gettext("Итого удержано :"),iuder,
gettext("Итого долгов   :"),dolg,
gettext("К выдаче       :"),kvid,
gettext("К выдаче без учёта сальдо"),inah+iuderb,
gettext("Долги без учёта сальдо"),dolgb);

iceb_t_zagolov(gettext("Расчёт свода начислений и удержаний по подразделениям"),1,mn,gn,dkm,mn,gn,ffsh);

/*Распечатка начилений/счет*/
fprintf(ffsh,"\n%s:\n",gettext("Начисления"));
fprintf(ffsh,"\
----------------------------------------------------\n");
fprintf(ffsh,gettext("\
Код|  Наименование начисления     |Счёт |   Сумма  |\n"));
fprintf(ffsh,"\
----------------------------------------------------\n");

//i=1;
bb=0.;
//while(NASH_SH.pol_masiv_char(i++,bros) == 0)
for(i=0; i < NASH_SH.kolih();i++)
 {
  if((sum=NASH_SUM.ravno(i)) == 0.)
   continue;
  
  polen(NASH_SH.ravno(i),&strok,1,',');
  polen(NASH_SH.ravno(i),&shet,2,',');
  sprintf(strsql,"select naik from Nash where kod=%s",strok.ravno());
     
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    printw("%s %s !\n",
    gettext("Не найден код начисления"),strok.ravno());
    OSTANOV();
    naim.new_plus("");
   }
  else
    naim.new_plus(row1[0]);
  fprintf(ffsh,"%3s %-*s %*s %10.2f\n",strok.ravno(),
  iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
  iceb_tu_kolbait(5,shet.ravno()),shet.ravno(),
  sum);
  bb+=sum;
 }

fprintf(ffsh,"\
----------------------------------------------------\n");

fprintf(ffsh,"%3s %*s: %10.2f\n%s:\n"," ",iceb_tu_kolbait(35,gettext("Итого")),gettext("Итого"),
bb,gettext("Удержания"));

fprintf(ffsh,"\
---------------------------------------------------------------\n");
fprintf(ffsh,gettext("\
Код|  Наименование удержания      |Счёт |   Сумма  |Сумма б.с.|\n"));
fprintf(ffsh,"\
---------------------------------------------------------------\n");

i=1;
bb1=bb=0.;

//while(UDER_SH.pol_masiv_char(i++,bros) == 0)
for(i=0; i < UDER_SH.kolih() ; i++)
 {
  sum=UDER_SUM.ravno(i);
  sum1=UDER_SUMB.ravno(i);
  if(sum == 0. && sum1 == 0.)
    continue;
       
  polen(UDER_SH.ravno(i),&strok,1,',');
  polen(UDER_SH.ravno(i),&shet,2,',');
  sprintf(strsql,"select naik from Uder where kod=%s",strok.ravno());
     
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    printw("%s %s !\n",
    gettext("Не найден код удержания"),strok.ravno());
    OSTANOV();
    naim.new_plus("");
   }
  else
    naim.new_plus(row1[0]);
  fprintf(ffsh,"%3s %-*s %*s %10.2f %10.2f\n",
  strok.ravno(),
  iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
  iceb_tu_kolbait(5,shet.ravno()),shet.ravno(),
  sum,sum1);
  bb+=sum;
  bb1+=sum1;
 }

fprintf(ffsh,"\
---------------------------------------------------------------\n");

fprintf(ffsh,"%3s %*s: %10.2f %10.2f\n"," ",iceb_tu_kolbait(35,gettext("Итого")),gettext("Итого"),bb,bb1);

fprintf(ffsh,"\n%*s:%10.2f\n",iceb_tu_kolbait(34,gettext("К выдаче с учётом сальдо")),gettext("К выдаче с учётом сальдо"),kvid);
fprintf(ffsh,"%*s:%10.2f\n",iceb_tu_kolbait(34,gettext("К выдаче без учёта сальдо")),gettext("К выдаче без учёта сальдо"),inah+iuderb);



}

