/* $Id: zarstom.c,v 5.58 2014/08/31 06:18:19 sasa Exp $ */
/*28.03.2018	25.03.2000	Белых А.И.	zarstom.c
Определение доли бюджетных денег в выдаче на руки и в
платежах (удержаниях).
Отчет разработан для бюджетных организаций
*/
#include        <errno.h>
#include        "buhg.h"

void    rasmasbn(int,char*,double*,int,int,char*,FILE*,double*);

extern short    *knvr;/*Коды начислений не входящие в расчёт подоходного налога*/

extern char	*shetb; /*Бюджетные счета*/
extern char	*shetbu; /*Бюджетные счета удержаний*/
extern short    *kodmp;   /*Коды материальной помощи*/

void zarstom(short mn,short gn, //Дата начала
short mk,short gk, //Дата конца
const char *podr, //Подразделение
const char *tabnom, //Табельный номер
const char *grupap,  //Группа подразделения
FILE *ff, //Файл общий
FILE *ffhoz, //Файл хозрасчёт
FILE *ffbud) //Файл бюджет
{
double	ogrzp[2]={0.,0.}; /*Разграниечение зарплаты для расчёта пенсионных*/
int kodsocstr=0;  /*Код отчисления на соц-страх*/
int kodpen=0;  /*Код пенсионных отчислений*/
class iceb_tu_str bros("");
short		kou; /*Количество удержаний*/
short		*uder; /*Массив удержаний*/
double		*sumuder; /*Массив сумм удержаний*/
char		*uder_shet; /*Маccив удержание/счет*/
double		*uder_shet_sum=NULL; /*Массив сумм удержание/счет*/
int		koushet=0; /*Количество удержаний/счет*/
char		*nah_shet; /*Маccив начисление/счет*/
double		*nah_shet_sum; /*Массив сумм начисление/счет*/
int		nahshet; /*Количество начисление/счет*/
int		i,i1;
SQL_str         row,row1,row2;
char		strsql[1024];
long		kolstr,kolstr1,kolstr2;
long		tabn;
double		sum;
short		knah;
double		sumakv; /*сумма к выдаче по табельному номеру*/
double		sumana; /*сумма начисленная по табельному номеру*/
double		sumabs; /*сумма начисленная по бюджетным счетам по табельному номеру*/
double		sumaos; /*сумма начисленная по остальным счетам*/
double		sumakvb; /*Сумма к выдаче приходящаяся на бюджетные счета*/
double		sumakvo; /*Сумма к выдаче приходящаяся на остальные счета*/
double		sumapen; /*Сумма с которой вычисляются пенсионные отчисления*/
double		sumapenbd=0.; /*Сумма с которой вычисляются пенсионные отчисления c бюджетных счетов*/
double		sumasoc; /*Сумма с которой вычисляются отчисления в соц-страх*/
double		sumasocbd=0.; /*Сумма с которой вычисляются отчисления в соц-страх*/

double		osumakv; /*Общая сумма к выдаче*/
double		osumana; /*Общая сумма начисленная*/
double		osumabs; /*Общая сумма начисленная по бюджетным счетам*/
double		osumaos; /*Общая сумма начисленная по остальным счетам*/
double		osumakvb; /*Общая cума к выдаче приходящаяся на бюджетные счета*/
double		osumakvo; /*Общая cума к выдаче приходящаяся на остальные счета*/
double		sumauderb; /*Сумма удержаний по бюджетным счетам*/

short		prn,dkm;
class iceb_tu_str naiud("");
double		itogo,itogo1,itogo2;
double		sumub,sumuo;
double		pens;     /*Сумма пенсионных отчислений*/
double		penso[2]; /*Разделение пенсионных до 150 и после*/
double		pensbu[2]; /*Разделение пенсионных бюджетных до 150 и после*/
double		pensnb[2]; /*Разделение пенсионных не бюджетных до 150 и после*/
double		socstrax=0.;        //Сумма отчисления в соц-страх
double		socstro[2]; //Соцстрах общий 
double		socstrb[2]; //Соцстрах бюджет
double		socstrn[2]; //Соцстах не бюджет
double		bb;
double		sald,saldb;
double		isald,isaldb;
double		idolgi,idolgib;
long		dlinao;
short		d,m,g;
int		klst=0;

if(shetb == NULL)
 {
  iceb_t_soob("Не введены Бюджетные счета !");
  return;
 }

printw("%d.%d => %d.%d\n",mn,gn,mk,gk);
GDITE();
SQLCURSOR cur1;
class iceb_tu_str glavbuh("");

iceb_t_poldan("Табельный номер бухгалтера",&bros,"zarnast.alx");
iceb_t_poldan("Код фонда социального страхования",&kodsocstr,"zarnast.alx");
sprintf(strsql,"select pm,ms from Zarmzpm where data <= '%04d-%02d-%d' order by data desc limit 1",gn,mn,1);
if(readkey(strsql,&row,&cur1) > 0)
 {

  if(gn <= 2006)
   ogrzp[0]=150;
  else
    ogrzp[0]=atof(row[0]);

  ogrzp[1]=atof(row[1]);

 }

if(bros.ravno()[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%d",bros.ravno_atoi());
  if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
   {
    sprintf(strsql,"%s! %d",gettext("Не найдена каточка главного бухгалтера"),bros.ravno_atoi());
    iceb_t_soob(strsql);
   }
  else
   {
    glavbuh.new_plus(row[0]);
   }
 }

uder_shet=NULL;
nah_shet=NULL;

/*Создаем массив удержаний*/

sprintf(strsql,"select kod from Uder");
SQLCURSOR cur;
if((kou=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kou == 0)
 {
  iceb_t_soob(gettext("Не введены удержания !"));
  return;
 }

if((uder=(short *)calloc(kou,sizeof(short))) == NULL)
 {
  printw("Не могу выделить память для uder !\n");
  OSTANOV();
  endwin();
  exit(16);
 }

i=0;
while(cur.read_cursor(&row) != 0)
  uder[i++]=atoi(row[0]);

if((sumuder=(double *)calloc(kou,sizeof(double))) == NULL)
 {
  beep();
  printw("Не могу выделить память для sumuder !\n");
  refresh();
  endwin();
  exit(16);
 }
/*Создаем массив начисление/счет*/
sprintf(strsql,"select distinct knah,shet from Zarp where \
datz >= '%04d-%d-01' and datz <= '%04d-%d-31' and prn='1' and \
suma <> 0. order by knah,shet asc",gn,mn,gk,mk);

if((nahshet=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }


dlinao=0;
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s|%s",row[0],row[1]);
  sozdmas(strsql,&nah_shet,&dlinao,'#');
 }
if(nahshet == 1)
    strcat(nah_shet,"#");


if((nah_shet_sum=(double *)calloc(nahshet,sizeof(double))) == NULL)
 {
  printw(" Не могу выделить память для nah_shet_sum !!!");
  OSTANOV();
  endwin();
  exit(16);
 }
  


/*Создаем массив удержание/счет*/
if(shetbu != NULL)
 {
  sprintf(strsql,"select distinct knah,shet from Zarp where \
datz >= '%04d-%d-01' and datz <= '%04d-%d-31' and prn='2' and \
suma <> 0. order by knah,shet asc",gn,mn,gk,mk);

  if((koushet=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    return;
   }


  dlinao=0;
  while(cur.read_cursor(&row) != 0)
   {
    sprintf(strsql,"%s|%s",row[0],row[1]);
    sozdmas(strsql,&uder_shet,&dlinao,'#');
   }
  if(koushet == 1)
    strcat(uder_shet,"#");

  if((uder_shet_sum=(double *)calloc(koushet,sizeof(double))) == NULL)
   {
    printw(" Не могу выделить память для uder_shet_sum !!!");
    OSTANOV();
    endwin();
    exit(16);
   }
    

 }

if(gn != gk)
   sprintf(strsql,"select distinct tabn from Zarn where \
(god > %d and god < %d) or \
(god = %d and mes >= %d) or \
(god = %d and mes <= %d)",
gn,gk,gn,mn,gk,mk);
else
   sprintf(strsql,"select distinct tabn from Zarn where god=%d and \
mes >= %d and mes <= %d",gn,mn,mk);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));

  free(uder);
  free(sumuder);
  if(uder_shet != NULL)
   {
    free(uder_shet);
    free(uder_shet_sum);
    free(nah_shet);
    free(nah_shet_sum);
   }
  return;
 }

for(i=0;i<2;i++)
 penso[i]=pensnb[i]=pensbu[i]=socstro[i]=socstrb[i]=socstrn[i]=0.;


iceb_t_poldan("Код пенсионного отчисления",&kodpen,"zarnast.alx");

sumapen=sumapenbd=pens=sumaos=sumakv=sumana=sumabs=sumakvb=sumakvo=0.;
idolgi=idolgib=osumakv=osumana=osumabs=osumaos=osumakvb=osumakvo=0.;
sumauderb=0.;
sald=saldb=0.;
isald=isaldb=0.;
SQLCURSOR curr;
kolstr1=tabn=0;
short dz,mz,gz;

while(cur.read_cursor(&row2) != 0)
 {
  printw("%s\n",row2[0]);
  strzag(LINES-1,0,kolstr,kolstr1++);
  if(proverka(tabnom,row2[0],0,0) != 0)
    continue;
    
  tabn=atol(row2[0]);

  sumasoc=sumasocbd=sumapen=sumapenbd=pens=socstrax=sumaos=sumakv=sumana=sumabs=sumakvb=sumakvo=0.;
  sumauderb=0.;
  saldb=sumakv=sald=0.;

//  Если заказать за год то не понятно что делать если перешел из одного подразделения в другое
  if(podr[0] != '\0' || grupap[0] != '\0')
   {
    class iceb_tu_str podrzap("");
    class iceb_tu_str grupzap("");
    dz=1; mz=mn; gz=gn;
    for(;;)
     {    
      sprintf(strsql,"select podr from Zarn where mes=%d and god=%d and \
tabn=%ld",mz,gz,tabn);
      if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
       {
        dpm(&dz,&mz,&gz,3);
       }
      else 
       break;
     }
    podrzap.new_plus(row1[0]);

    if(grupap[0] != '\0')
     {
      sprintf(strsql,"select grup from Podr where kod=%s",podrzap.ravno());
      if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
       {
        printw("Не найдено подразделение %s в таблице Podr!\n",podrzap.ravno());
        OSTANOV();
        continue;
       }
     }


    grupzap.new_plus(row1[0]);

    if(proverka(podr,podrzap.ravno(),0,0) == 0)
      if(proverka(grupap,grupzap.ravno(),1,0) == 0)
        sumakv=sald=zarsald(1,mn,gn,tabn,&saldb);
   }
  else
    sumakv=sald=zarsald(1,mn,gn,tabn,&saldb);


  isald+=sald-saldb;
  isaldb+=saldb;

  sprintf(strsql,"select tabn,prn,knah,suma,shet,datz,mesn,godn,podr \
from Zarp where datz >= '%04d-%d-01' and datz <= '%04d-%d-31' and \
tabn=%ld and suma <> 0. order by prn,datz asc",gn,mn,gk,mk,tabn);
  class SQLCURSOR cur1;
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }
  if(kolstr2 == 0)
   printw("Нет начислений/удержаний ! %f/%f\n",sald,saldb);


  if(kolstr2 != 0)
  while(cur1.read_cursor(&row) != 0)
   {
    /*Смотрим код подразделения*/
    if(proverka(podr,row[8],0,0) != 0)
        continue;

    if(grupap[0] != '\0')
     {
      sprintf(strsql,"select grup from Podr where kod=%s",row[8]);
      if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
       {
        printw("Не найдено подразделение %s в таблице Podr!\n",row[8]);
        OSTANOV();
        continue;
       }

      if(proverka(grupap,row1[0],1,0) != 0)
       continue;
     }

    prn=atoi(row[1]);
    knah=atoi(row[2]);

    sum=atof(row[3]);
    sum=okrug(sum,0.01);

    if(prn == 1)
     {
      sumana+=sum;

      if(knvr != NULL)
       {
        for(i=1; i <= knvr[0]; i++)
         if(knah == knvr[i])
           goto vpr;
       }

      rsdat(&d,&m,&g,row[5],2);

vpr:;
       
      
      if(pole1(shetb,row[4],',',0,&i1) != 0 && SRAV(shetb,row[4],0) != 0)
       {
        sumaos+=sum;       
       }    
      else
       {
        sumabs+=sum;
       }
      sprintf(strsql,"%s|%s",row[2],row[4]);
      if(sum != 0.)
       {
        if(pole1(nah_shet,strsql,'#',0,&i1) != 0)
         {
          beep();
          printw("Не найдено %s в массиве начислений/счетов !\n",strsql);
          OSTANOV();  
         }
        else
         nah_shet_sum[i1]+=sum;
       }
     }

    if(prn == 2)
     {
      for(i=0; i < kou ; i++)
       if(knah == uder[i])
         break;
      if(i == kou)
       {
        beep();
        printw("Не найден код удержания %d в массиве удержаний !\n",knah);
        continue;
       }

      sumuder[i]+=sum;

      if(knah == kodpen)
       {
        pens+=sum;
        if(shetbu != NULL)
         {
          if(pole1(shetbu,row[4],',',0,&i1) == 0 || SRAV(shetbu,row[4],0) == 0)
           {
            if(sumapenbd <= ogrzp[0]) 
             {
              pensbu[0]+=sum;
              penso[0]+=sum;
             }
            else
             {
              pensbu[1]+=sum;
              penso[1]+=sum;
             }

           }
          else
           {
            if(sumapen-sumapenbd <= ogrzp[0]) 
             {
              penso[0]+=sum;
              pensnb[0]+=sum;
             }
            else
             {
              pensnb[1]+=sum;
              penso[1]+=sum;
             }
           }

         }
       }

      if(knah == kodsocstr)
       {
        socstrax+=sum;
        if(shetbu != NULL)
         {
          if(pole1(shetbu,row[4],',',0,&i1) == 0 || SRAV(shetbu,row[4],0) == 0)
           {
            if(sumasocbd <= ogrzp[0]) 
             {
              socstrb[0]+=sum;
              socstro[0]+=sum;
             }
            else
             {
              socstrb[1]+=sum;
              socstro[1]+=sum;
             }

           }
          else
           {
            if(sumasoc-sumasocbd <= ogrzp[0]) 
             {
              socstro[0]+=sum;
              socstrn[0]+=sum;
             }
            else
             {
              socstrn[1]+=sum;
              socstro[1]+=sum;
             }
           }

         }
       }

      if(shetbu != NULL)
       {
        if(pole1(shetbu,row[4],',',0,&i1) == 0 || SRAV(shetbu,row[4],0) == 0)
         sumauderb+=sum;
        sprintf(strsql,"%s|%s",row[2],row[4]);
        if(sum != 0.)
         {
          if(pole1(uder_shet,strsql,'#',0,&i1) != 0)
           {
            beep();
            printw("Не найдено %s в массиве удержаний/счетов !\n",strsql);
            OSTANOV();  
           }
          else
           {
            uder_shet_sum[i1]+=sum;
           }
        }
       }    
     }

    sumakv+=sum;  
   }
 if(sumakv > 0.009 && shetbu == NULL)
  {
   if(sumana != 0.)
     sumakvo=sumaos*sumakv/sumana;
   
   sumakvo=okrug(sumakvo,0.01);

   sumakvb=sumakv-sumakvo;
   osumakv+=sumakv;
   osumakvb+=sumakvb;
   osumakvo+=sumakvo;

  }

 if(shetbu != NULL)
  {
   sumakvb=saldb+sumabs+sumauderb;

   sumakvo=sumakv-sumakvb;

   if(sumakvb > 0.)
    {
     osumakv+=sumakvb;
     osumakvb+=sumakvb;
    }
   else
    {
     idolgib+=sumakvb;
     if(sumakvb < -0.009)
      printw("Долг бюджетный: %ld %.2f\n",tabn,sumakvb);
    }

   if(sumakvo > 0.)
    {
     osumakv+=sumakvo;
     osumakvo+=sumakvo;
    }
   else
    {
     idolgi+=sumakvo;
     if(sumakvo < -0.009)
       printw("Долг не бюджетный: %ld %.2f\n",tabn,sumakvo);
    }         
  }
 if(shetbu == NULL)
  {
   if(pens != 0.)
    {
     if(sumapen <= ogrzp[0]) 
      {
       penso[0]+=pens;
       bb=sumaos*pens/sumapen;
       pensnb[0]+=bb;
       pensbu[0]+=pens-bb;
      }
     else
      {
       penso[1]+=pens;
       bb=sumaos*pens/sumapen;
       pensnb[1]+=bb;
       pensbu[1]+=pens-bb;
      }
    }
   if(socstrax != 0.)
    {
     if(sumasoc <= ogrzp[0]) 
      {
       socstro[0]+=socstrax;
       bb=sumaos*socstrax/sumasoc;
       socstrn[0]+=bb;
       socstrb[0]+=socstrax-bb;
      }
     else
      {
       socstro[1]+=pens;
       bb=sumaos*socstrax/sumasoc;
       socstrn[1]+=bb;
       socstrb[1]+=socstrax-bb;
      }
    }
  }
 osumana+=sumana;
 osumabs+=sumabs;
 osumaos+=sumaos;
 }

GDITE();

dpm(&dkm,&mk,&gk,5);

zagolov(gettext("Расчёт долей бюджетных счетов"),1,mn,gn,dkm,mk,gk,iceb_t_get_pnk("00",0),ff);
zagolov(gettext("Расчёт долей бюджетных счетов"),1,mn,gn,dkm,mk,gk,iceb_t_get_pnk("00",0),ffhoz);
zagolov(gettext("Расчёт долей бюджетных счетов"),1,mn,gn,dkm,mk,gk,iceb_t_get_pnk("00",0),ffbud);

if(tabnom[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Табельный номер"),tabnom);
  fprintf(ffhoz,"%s:%s\n",gettext("Табельный номер"),tabnom);
  fprintf(ffbud,"%s:%s\n",gettext("Табельный номер"),tabnom);
 }
if(grupap[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Группа подразделения"),grupap);
  fprintf(ffhoz,"%s:%s\n",gettext("Группа подразделения"),grupap);
  fprintf(ffbud,"%s:%s\n",gettext("Группа подразделения"),grupap);
 } 

if(podr[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Коды подразделений вошедшие в расчёт"),podr);
  printcod(ff,"Podr","kod","naik",0,podr,&klst);

  fprintf(ffhoz,"%s:%s\n",gettext("Коды подразделений вошедшие в расчёт"),podr);
  printcod(ffhoz,"Podr","kod","naik",0,podr,&klst);

  fprintf(ffbud,"%s:%s\n",gettext("Коды подразделений вошедшие в расчёт"),podr);
  printcod(ffbud,"Podr","kod","naik",0,podr,&klst);
 }
else
 {
  fprintf(ff,gettext("По всем подразделениям.\n"));
  fprintf(ffhoz,gettext("По всем подразделениям.\n"));
  fprintf(ffbud,gettext("По всем подразделениям.\n"));
 }

printw("\
%s%10.2f\n\
%s%10.2f\n",
gettext("Сальдо на начало месяца (хозрасчёт).................."),
isald,
gettext("Сальдо на начало месяца (бюджет)....................."),
isaldb);

printw("\
%s%10.2f\n\
%s%10.2f\n\
%s%10.2f\n\
%s%10.2f\n\
%s%10.2f\n\
%s%10.2f\n",
gettext("Общая сумма начисленная.............................."),
osumana,
gettext("Общая сумма к выдаче................................."),
osumakv,
gettext("Общая сумма начисленная по бюджетным счетам.........."),
osumabs,
gettext("Общая сумма начисленная по остальным счетам.........."),
osumaos,
gettext("Общая cума к выдаче приходящаяся на бюджетные счета.."),
osumakvb,
gettext("Общая cума к выдаче приходящаяся на остальные счета.."),
osumakvo);

if(shetbu != NULL)
 {
  printw("\
%s%10.2f\n\
%s%10.2f\n",
  gettext("Долги (хозрасчёт)..................................."),
  idolgi,
  gettext("Долги (бюджет)......................................"),
  idolgib);
 }

fprintf(ff,"%s:%s\n",
gettext("Бюджетные счета"),shetb);

fprintf(ffhoz,"%s:%s\n",
gettext("Бюджетные счета"),shetb);

fprintf(ffbud,"%s:%s\n",
gettext("Бюджетные счета"),shetb);

if(shetbu != NULL)
 {

  fprintf(ff,"\n\
%s%10.2f\n\
%s%10.2f\n",
  gettext("Сальдо на начало месяца (хозрасчёт).................."),
  isald,
  gettext("Сальдо на начало месяца (бюджет)....................."),
  isaldb);

  fprintf(ffhoz,"\n\
%s%10.2f\n",
  gettext("Сальдо на начало месяца (хозрасчёт).................."),
  isald);

  fprintf(ffbud,"\n\
%s%10.2f\n",
  gettext("Сальдо на начало месяца (бюджет)....................."),
  isaldb);

 }

fprintf(ff,"\
%s%10.2f\n\
%s%10.2f\n\
%s%10.2f\n\
%s%10.2f\n\
%s%10.2f\n\
%s%10.2f\n",
gettext("Общая сумма начисленная.............................."),
osumana,
gettext("Общая сумма к выдаче................................."),
osumakv,
gettext("Общая сумма начисленная по бюджетным счетам.........."),
osumabs,
gettext("Общая сумма начисленная по остальным счетам.........."),
osumaos,
gettext("Общая cума к выдаче приходящаяся на бюджетные счета.."),
osumakvb,
gettext("Общая cума к выдаче приходящаяся на остальные счета.."),
osumakvo);

fprintf(ffbud,"\
%s%10.2f\n\
%s%10.2f\n",
gettext("Общая сумма начисленная по бюджетным счетам.........."),
osumabs,
gettext("Общая cума к выдаче приходящаяся на бюджетные счета.."),
osumakvb);

fprintf(ffhoz,"\
%s%10.2f\n\
%s%10.2f\n",
gettext("Общая сумма начисленная по остальным счетам.........."),
osumaos,
gettext("Общая cума к выдаче приходящаяся на остальные счета.."),
osumakvo);

if(shetbu != NULL)
 {
  fprintf(ff,"\
%s%10.2f\n\
%s%10.2f\n",
  gettext("Долги (хозрасчёт)..................................."),
  idolgi,
  gettext("Долги (бюджет)......................................"),
  idolgib);

  fprintf(ffhoz,"\
%s%10.2f\n",
  gettext("Долги (хозрасчёт)..................................."),
  idolgi);

  fprintf(ffbud,"\
%s%10.2f\n",
  gettext("Долги (бюджет)......................................"),
  idolgib);
 }
double itogon;
double itogou;

rasmasbn(nahshet,nah_shet,nah_shet_sum,0,1,shetb,ff,&itogon);
rasmasbn(koushet,uder_shet,uder_shet_sum,0,2,shetbu,ff,&itogou);
fprintf(ff,"\n%.2f %.2f = %.2f\n",itogon,itogou,itogon+itogou);

rasmasbn(nahshet,nah_shet,nah_shet_sum,0,1,shetb,ffbud,&itogon);
rasmasbn(koushet,uder_shet,uder_shet_sum,0,2,shetbu,ffbud,&itogou);
fprintf(ffbud,"\n%.2f %.2f = %.2f\n",itogon,itogou,itogon+itogou);

if(shetbu != NULL)
 {
  rasmasbn(nahshet,nah_shet,nah_shet_sum,1,1,shetb,ff,&itogon);
  rasmasbn(koushet,uder_shet,uder_shet_sum,1,2,shetbu,ff,&itogou);
  fprintf(ff,"\n%.2f %.2f = %.2f\n",itogon,itogou,itogon+itogou);

  rasmasbn(nahshet,nah_shet,nah_shet_sum,1,1,shetb,ffhoz,&itogon);
  rasmasbn(koushet,uder_shet,uder_shet_sum,1,2,shetbu,ffhoz,&itogou);
  fprintf(ffhoz,"\n%.2f %.2f = %.2f\n",itogon,itogou,itogon+itogou);
 }

if(shetbu == NULL)
 {
  fprintf(ff,"\n\n\
--------------------------------------------------------------------\n");
  fprintf(ff,gettext("Код|    Наименование удержания    | Сумма    | Бюджет   | Хозрасчёт|\n"));
  fprintf(ff,"\
--------------------------------------------------------------------\n");
  itogo1=itogo2=itogo=0;

  for(i=0; i < kou ;i++)
   {
    if(sumuder[i] == 0.)
     continue;
     
    sprintf(strsql,"select naik from Uder where kod=%d",uder[i]);
    if(sql_readkey(&bd,strsql,&row,&curr) != 1)
     {
      beep();
      printw("%s %d !\n",gettext("Не найден код удержания"),uder[i]);
      OSTANOV();
      naiud.new_plus("");
     }
    else
     naiud.new_plus(row[0]);
    sumuo=osumaos*sumuder[i]/osumana;
    sumuo=okrug(sumuo,0.01);
    sumub=sumuder[i]-sumuo;  
    fprintf(ff,"%-3d %-*.*s %10.2f %10.2f %10.2f\n",
    uder[i],
    iceb_tu_kolbait(30,naiud.ravno()),iceb_tu_kolbait(30,naiud.ravno()),naiud.ravno(),
    sumuder[i],sumub,sumuo);
    
    itogo+=sumuder[i];
    itogo1+=sumub;
    itogo2+=sumuo;
   }
  fprintf(ff,"\
--------------------------------------------------------------------\n\
%*s:%10.2f %10.2f %10.2f\n",iceb_tu_kolbait(34,gettext("Итого")),gettext("Итого"),itogo,itogo1,itogo2);
 }

fprintf(ff,"\x1B\x50"); /*10-знаков*/

free(uder);
free(sumuder);

if(uder_shet != NULL)
 {
  free(uder_shet);
  free(uder_shet_sum);
  free(nah_shet);
  free(nah_shet_sum);
 }

fprintf(ff,"\n%s____________________%s\n",gettext("Главный бухгалтер"),glavbuh.ravno());

fprintf(ffhoz,"\n%s____________________%s\n",gettext("Главный бухгалтер"),glavbuh.ravno());

fprintf(ffbud,"\n%s____________________%s\n",gettext("Главный бухгалтер"),glavbuh.ravno());


podpis(ff);
podpis(ffhoz);
podpis(ffbud);

}
/********************/
/*Распечатка массива*/
/*********************/
void    rasmasbn(int nahshet,char *nah_shet,
double *nah_shet_sum,
int metka, //0-бюджет 1-хозрасчёт
int prn, //1-приход 2-расход
char *shetbh, //Список бюджетных счетов
FILE *ff,
double *itogo)
{
class iceb_tu_str bros("");
class iceb_tu_str kodnah("");
class iceb_tu_str kodnahz("");
class iceb_tu_str shet("");
int		i=0;
class iceb_tu_str naiud("");
char		strsql[512];
SQL_str		row;
int		kolnu=0;
double		itogonu=0.;
SQLCURSOR curr;
*itogo=0.;

if(metka == 0)
 {
  fprintf(ff,"\n%s:%.*s\n",
  gettext("Бюджет"),
  iceb_tu_kolbait(55,shetbh),
  shetbh);
  for(int nom=55; nom < iceb_tu_strlen(shetbh); nom+=60)
   fprintf(ff,"%.*s\n",iceb_tu_kolbait(60,iceb_tu_adrsimv(nom,shetbh)),
   iceb_tu_adrsimv(nom,shetbh));
 }
if(metka == 1)
 fprintf(ff,"\n%s\n",gettext("Хозрасчёт"));

fprintf(ff,"\
------------------------------------------------------\n");
if(prn == 1)
  fprintf(ff,gettext("Код|    Наименование начисления   | Счёт  | Сумма    |\n"));

if(prn == 2)
  fprintf(ff,gettext("Код|    Наименование удержания    | Счёт  | Сумма    |\n"));

fprintf(ff,"\
------------------------------------------------------\n");
//printw("nahshet=%d nah_shet=%s\n",nahshet,nah_shet);
for(i=0; i < nahshet ;i++)
 {
//  printw("nah_shet_sum[%d]=%f\n",i,nah_shet_sum[i]);
  if(nah_shet_sum[i] == 0.)
   continue;
  polen(nah_shet,&bros,i+1,'#');     
  polen(bros.ravno(),&kodnah,1,'|');
  if(kodnah.ravno()[0] == '\0')
    continue;
      
  polen(bros.ravno(),&shet,2,'|');
//  printw("shetbh=%s shet=%s\n",shetbh,shet);

  if(metka == 0)
   if(proverka(shetbh,shet.ravno(),0,1) != 0)
    continue;

  if(metka == 1)
   if(proverka(shetbh,shet.ravno(),0,1) == 0)
    continue;

  if(SRAV(kodnah.ravno(),kodnahz.ravno(),0) != 0)
   {
    if(kodnahz.ravno()[0] != '\0' && kolnu > 1)
     {
      fprintf(ff,"%*s %10.2f\n",iceb_tu_kolbait(53,gettext("Итого")),gettext("Итого"),itogonu);
     }    
    kolnu=0;
    itogonu=0.;
    kodnahz.new_plus(kodnah.ravno());
   }

  kolnu++;

  if(prn == 1)
    sprintf(strsql,"select naik from Nash where kod=%s",kodnah.ravno());
  if(prn == 2)
    sprintf(strsql,"select naik from Uder where kod=%s",kodnah.ravno());
  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
    if(prn == 1)
      printw("%s %s !\n",gettext("Не найден код начисления"),kodnah.ravno());
    if(prn == 2)
      printw("%s %s !\n",gettext("Не найден код удержания"),kodnah.ravno());
    OSTANOV();
    naiud.new_plus("");
   }
  else
   naiud.new_plus(row[0]);

  fprintf(ff,"%-3s %-*.*s %-*s %10.2f\n",
  kodnah.ravno(),
  iceb_tu_kolbait(30,naiud.ravno()),iceb_tu_kolbait(30,naiud.ravno()),naiud.ravno(),
  iceb_tu_kolbait(7,shet.ravno()),shet.ravno(),
  nah_shet_sum[i]);
  
  *itogo+=nah_shet_sum[i];
  itogonu+=nah_shet_sum[i];
 }
if(kolnu > 1)
   fprintf(ff,"%*s %10.2f\n",iceb_tu_kolbait(53,gettext("Итого")),gettext("Итого"),itogonu);

fprintf(ff,"\
------------------------------------------------------\n\
%*s:%10.2f\n",iceb_tu_kolbait(42,gettext("Итого")),gettext("Итого"),*itogo);
}
