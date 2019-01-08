/* $Id: kateg.c,v 5.42 2013/09/26 09:43:34 sasa Exp $ */
/*14.07.2015    23.11.1992      Белых А.И.      kateg.c
Подпрограмма получения всех начислений и удержаний по категориям
*/
#include        <errno.h>
#include        "buhg.h"

void            sappk(short,short,char[],short,short,short,FILE*,double*,short*);
void            sch(short,short,short,short,short*,short,short*,short,short*,short,short*,short*,FILE*,double*,double*,const char[],const char[],const char[],const char[],const short,const char[],const char[]);
void		rasmaskat(FILE*,double*,short*,short,short,short,double*,short*,short*);


void kateg(short mn,short gn,short mk,short gk,const char podr[],
const char kateg[],const char nahis[],const char uder[],const char tabnom[],const char shet[],FILE *kaw)
{
short           kon;  /*Количество статей начисления*/
short           kou;  /*Количество статей удержания*/
short           kka;  /*Количество категорий*/
int             in,iu,ik;
struct  tm      *bf;
time_t          tmm;
SQL_str         row;
char		strsql[512];
short		dkm;
int		klst=0;

printw("%s %d.%d %s %d.%d\n",gettext("Период с"),mn,gn,
gettext("до"),mk,gk);
GDITE();
/*Определяем количество начислений и удержаний*/

kka=kon=kou=0;
in=iu=ik=0;

sprintf(strsql,"select kod from Nash");
class SQLCURSOR cur;
if((kon=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
if(kon == 0)
 {
  printw("%s\n",gettext("Не введены начисления !"));
  OSTANOV();
  return;
 }
short na[kon];
while(cur.read_cursor(&row) != 0)
  na[in++]=atoi(row[0]);

sprintf(strsql,"select kod from Uder");

if((kou=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kou == 0)
 {
  printw("%s\n",gettext("Не введены удержания !"));
  OSTANOV();
  return;
 }

short ud[kou];

while(cur.read_cursor(&row) != 0)
  ud[iu++]=atoi(row[0]);

sprintf(strsql,"select kod from Kateg");

if((kka=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kka == 0)
 {
  printw("Не введены категории !\n");
  OSTANOV();
  return;
 }
short ka[kka];
while(cur.read_cursor(&row) != 0)
  ka[ik++]=atoi(row[0]);

double nao[kka*kon];
memset(&nao,'\0',sizeof(nao));

double udo[kka*kou];
memset(&udo,'\0',sizeof(udo));

short kollnah[kka];
short kollud[kka];
memset(&kollnah,'\0',sizeof(kollnah));
memset(&kollud,'\0',sizeof(kollud));

/*
printw("%d %d %d\n",kon,kou,kka);
printw("Коды категорий - ");
for(i=0;i<kka;i++)
  printw(" %d",ka[i]);
printw("Конец\n");
OSTANOV();
  */


/*
for(i=0; i< kka; i++)
 fprintf(kaw,"%d ",ka[i]);
fprintf(kaw,"\n");
  */

time(&tmm);
bf=localtime(&tmm);

dpm(&dkm,&mk,&gk,5);
//printw("kka=%d\n",kka);
//OSTANOV();
  
fprintf(kaw,"%s\n\n\
%s\n\
%s 1.%d.%d%s %s %d.%d.%d%s\n\
%s %d.%d.%d%s  %s: %d:%d\n\n",
iceb_t_get_pnk("00",0),
gettext("Расчёт начислений и удержаний по категориям"),
gettext("Период с"),
mn,gn,
gettext("г."),
gettext("до"),
dkm,mk,gk,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

if(podr[0] != '\0')
 {
  fprintf(kaw,"%s:%s\n",gettext("Коды подразделений вошедшие в расчёт"),podr);
  printcod(kaw,"Podr","kod","naik",0,podr,&klst);
 }
else
 fprintf(kaw,gettext("По всем подразделениям.\n"));
if(nahis[0] != '\0')
 fprintf(kaw,"%s:%s\n",gettext("Коды начислений вошедшие в расчёт"),nahis);
if(uder[0] != '\0')
 fprintf(kaw,"%s:%s\n",gettext("Коды удержаний вошедшие в расчёт"),uder);
if(kateg[0] != '\0')
 fprintf(kaw,"%s:%s\n",gettext("Коды категорий вошедшие в расчёт"),kateg);
if(tabnom[0] != '\0')
 fprintf(kaw,"%s:%s\n",gettext("Табельный номер"),tabnom);
if(shet[0] != '\0')
 fprintf(kaw,"%s:%s\n",gettext("Счёт"),shet);

sch(mn,gn,mk,gk,na,kon,ud,kou,ka,kka,kollnah,kollud,kaw,nao,udo,podr,kateg,nahis,\
uder,0,tabnom,shet);


podpis(kaw);


}


/********/
/*Поиск */
/********/
void            sch(short mn,short gn,short mk,short gk,
short *na, //Массив кодов начислений
short kon, //Количество статей начисления
short *ud,  //Массив кодов удержаний
short kou,  //Количество статей удержания
short *ka,  //Коды категорий
short kka, //Количество категорий
short *kollnah, //Массив количества человек в каждой категории
short *kollud,
FILE *kaw,
double *nao, //Массив содержимого начислений по категориям
double *udo, //Массив содержимого удержаний по категориям
const char podr[],const char kateg[],const char nahis[],
const char uder[],const short dia,const char tabnom[],
const char shet[]) //Счёт
{
int             i;
int             skn=0;
int             kkk=0;
int             mtn;  /*Количество человек*/
double          sn,su;
double          sum;
double          dolgi;
short           m,g;
double          itsal=0;  /*Итоговое сальдо*/
long		kolstr,kolstr1,kolstr2;
SQL_str         row,row1;
char		strsql[512];
long		tbn;
int		kkat,knah;
short		prn;
double		sym;
double		saldb;
short		metkas=0;
/*
printw("sch: %d.%d %d.%d-------\n",mn,gn,mk,gk);
OSTANOV();
*/

for(i=0;i<kka*kou;i++)
 udo[i]=0.;
for(i=0;i<kka*kon;i++)
 nao[i]=0.;

//ta=0;
mtn=0;
itsal=dolgi=sum=0.;

g=gn;

for(m=mn; ; m++)
 {
  if(m == 13)
   {
    m=1;
    g++;
   }
  if(g > gk)
    break;
  if(g == gk && m > mk)
    break;

  printw("%s %d.%d%s\n",gettext("Дата"),m,g,
  gettext("г."));
  
  sprintf(strsql,"select tabn,podr,kateg from Zarn where god=%d and mes=%d",
  g,m);
//  printw("%s\n",strsql);
  SQLCURSOR cur;
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    return;
   }
  if(kolstr == 0)
    continue;
  kolstr1=0;
  while(cur.read_cursor(&row) != 0)
   {
    strzag(LINES-1,0,kolstr,++kolstr1);
/*
    printw("Zarn-%s %s %s\n",row[0],row[1],row[2]);
    refresh();
*/
    if(proverka(tabnom,row[0],0,0) != 0)
      continue;
    if(proverka(podr,row[1],0,0) != 0)
      continue;
    if(proverka(kateg,row[2],0,0) != 0)
      continue;
    if(proverka(shet,row[3],0,0) != 0)
      continue;

    tbn=atol(row[0]);
    kkat=atoi(row[2]);

    for(i=0 ; i < kka ; i++)
     if(ka[i] == kkat)
       {
	skn=i+1;
	break;
       }

    /*Если нет категории в списке читаем дальше*/
    if(i == kka)
     {
      if(kkat == 0)
       printw("%s %ld %s !\n",gettext("Табельный номер"),
       tbn,gettext("В карточку не введён код категории"));
      else
       printw("%s %d !\n",gettext("Не найден код категории"),kkat);

      OSTANOV();

      continue;
     }

    /*Читаем сальдо*/
    sum=0.;
    if(m == mn && g == gn)
     {
      sum=zarsald(1,m,g,tbn,&saldb);
      itsal+=sum;
     }
    sprintf(strsql,"select prn,knah,suma,shet from Zarp where \
datz >= '%d-%02d-01' and datz <= '%d-%02d-31' and tabn=%ld and \
suma <> 0.",g,m,g,m,tbn);

//  printw("%s\n",strsql);
    SQLCURSOR cur1;
    if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
     {
      msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
      continue;
     }
    metkas=0;
    if(kolstr2 == 0)
     {
      if(sum < 0.)
       {
        dolgi+=sum;
        metkas=1;
       }
      continue;
     }

    short metkanah=0;
    short metkaud=0;      
    while(cur1.read_cursor(&row1) != 0)
     {
/*
      printw("Zarp-%s %s %s\n",row1[0],row1[1],row1[2]);
      refresh();
*/
      prn=atoi(row1[0]);

      if(prn == 1)
        if(proverka(nahis,row1[1],0,0) != 0)
          continue;

      if(prn == 2)
        if(proverka(uder,row1[1],0,0) != 0)
          continue;

      if(proverka(shet,row1[3],0,0) != 0)
         continue;

      knah=atoi(row1[1]);
      sym=atof(row1[2]);

      sum+=sym;

      if(prn == 1)
       {
        if(metkanah == 0)
         {
          for(i=0; i<kka ; i++)
           if(ka[i] == kkat)
            {
             kollnah[i]++;
             break;
            }
          metkanah++;
         }
        for(i=0 ; i < kon ; i++)
	  if(na[i] == knah)
	   {
	    kkk=i+1;
	    break;
	   }
      /*
	if(dia == 0)
	 {
	  printw("kkk-%d kka-%d skn-%d\n",kkk,kka,skn);
	  refresh();
	 }
	*/
	nao[(kkk-1)*kka+skn-1]+=sym;
       }

      if(prn == 2)
       {
        if(metkaud == 0)
         {
          for(i=0; i<kka ; i++)
           if(ka[i] == kkat)
            {
             kollud[i]++;
             break;
            }
          metkaud++;
         }

        for(i=0 ; i < kou ; i++)
	  if(ud[i] == knah)
	   {
	    kkk=i+1;
	    break;
	   }
	udo[(kkk-1)*kka+skn-1]+=sym;
       }

     }

    mtn++;
    if(sum < 0.)
      dolgi+=sum;
    sum = 0.;

   }
  
 }

if(sum < 0. && metkas == 0)
   dolgi+=sum;


/*Распечатываем начисления*/
printw("%s\n-------------------------\n",
gettext("Распечатываем начисления."));
refresh();
sn=su=0.;
rasmaskat(kaw,nao,na,kka,kon,1,&sn,ka,kollnah);

printw("\n%s\n-------------------------\n",
gettext("Распечатываем удержания."));
refresh();
/*Распечатываем удержания*/
rasmaskat(kaw,udo,ud,kka,kou,2,&su,ka,kollud);

printw("\n\
%s %15.2f\n\
%s %15.2f\n",
gettext("Сальдо         :"),itsal,
gettext("Итого начислено:"),sn);

printw("\
%s %15.2f\n\
%s %15.2f\n\
%s %15.2f\n\
%s %15.2f\n\n\
%s - %ld\n",
gettext("Итого удержано :"),su,
gettext("Итого долгов   :"),dolgi,
gettext("К выдаче       :"),sn+su-dolgi+itsal,
gettext("Сальдо         :"),sn+su+itsal,
gettext("Количество работников"),mtn);
refresh();

fprintf(kaw,"\
%s %15.2f\n\
%s %15.2f\n\
%s %15.2f\n\
%s %15.2f\n\
%s %15.2f\n\
%s %15.2f\n",
gettext("Сальдо         :"),itsal,
gettext("Итого начислено:"),sn,
gettext("Итого удержано :"),su,
gettext("Итого долгов   :"),dolgi,
gettext("К выдаче       :"),sn+su-dolgi+itsal,
gettext("Сальдо         :"),sn+su+itsal);

}

/***************/
/*Шапка таблицы*/
/***************/
void            sappk(short kk, //1-начисления  2-удержания
short kka,  //Количество категорий
class iceb_tu_str *st,
short pn,  //Позиция начала
short pk,  //Позиция конца
short konu, //Количество начислений или удержаний
FILE *kaw,double *nao,short *ka)
{
int             i,i1,i2;
class iceb_tu_str nai("");
double          br2;
SQL_str         row;
SQLCURSOR curr;
char		strsql[512];

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
    st->plus("-------------");
  else
   i2++;
 }
/*Полка над ИТОГО*/
st->plus("-------------");

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
  sprintf(strsql,"select naik from Kateg where kod=%d",ka[i]);
  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
     beep();
     printw("Не найден код категорії %d\n",ka[i]);
     OSTANOV();
   }
  nai.new_plus(row[0]);

  fprintf(kaw,"| %-*.*s",iceb_tu_kolbait(11,nai.ravno()),iceb_tu_kolbait(11,nai.ravno()),nai.ravno());
 }

fprintf(kaw,"| %-*.*s|\n",iceb_tu_kolbait(11,gettext(" Итого")),iceb_tu_kolbait(11,gettext(" Итого")),gettext(" Итого"));

fprintf(kaw,"   |%-30s"," ");
i2=0;
for(i=pn; i< pk+i2 && i < kka; i++)
 {
  /*Если колонка нулевая не печатать*/
  br2=0.;
  for(i1=i; i1< kka*konu ; i1+=kka)
   {
    br2+=nao[i1];
/*
    if(kk == 1)
     br2+=nao[i1];
    if(kk == 2)
     br2+=udo[i1];
*/
   }
  if(br2 != 0.)
    fprintf(kaw,"| %-11d",ka[i]);
  else
   i2++;
 }
fprintf(kaw,"| %-11.11s|\n"," ");

fprintf(kaw,"%s\n",st->ravno());

}

/******************************/
/*Программа распечатки массива*/
/******************************/

void		rasmaskat(FILE *kaw,
double *naon, //Массив параметров*/
short *nan,  //Массив с перечнем параметров
short kkan,  //Количество категорий
short konn,  //Количество параметров
short metka, //1-начисления 2-удержания
double *sn, //Сумма по массиву
short *ka,short *koll)
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
kkol=17;

//korr=0;
*sn=0;
for(ii=0 ; ii < kkan; ii += kkol)
 {
  /*printw("ii-%d kkan-%d korr-%d\n",ii,kkan,korr);*/
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
  sappk(metka,kkan,&st,ii,(short)(ii+kkol),konn,kaw,naon,ka);
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


    if(metka == 1)
      sprintf(strsql,"select naik from Nash where kod=%d",nan[i]);
    if(metka == 2)
      sprintf(strsql,"select naik from Uder where kod=%d",nan[i]);

    nai.new_plus("");
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
      nai.new_plus(row[0]);

    fprintf(kaw,"%3d %-*.*s",
    nan[i],
    iceb_tu_kolbait(30,nai.ravno()),iceb_tu_kolbait(30,nai.ravno()),nai.ravno());

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
	fprintf(kaw," %12.2f",naon[skn]);
       }
      else
       {
	i2++;
       }
      br1+=naon[skn];
     }
    fprintf(kaw," %12.2f\n",br1);
    /*printw("%3d %-30s %.2f\n",zr2.tn,nai.ravno(),br1);*/
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
       fprintf(kaw," %12.2f",br1);
    else
      i2++;
    *sn=*sn+br1;
   }
  fprintf(kaw," %12.2f\n",*sn);

  fprintf(kaw,"%s\n",st.ravno());

  /*Распечатываем количество человек*/
  fprintf(kaw,"%-*s",iceb_tu_kolbait(34,gettext(" Количество работающих ")),gettext(" Количество работающих "));
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
	fprintf(kaw," %12d",koll[i]);
	khel+=koll[i];
       }
      else
	i2++;
   }
  fprintf(kaw," %12d\n\n",khel);
  if(i == kkan)
   break;
  /*ii+=korr*kkol+i3;*/
  ii+=i2;
 }

}
