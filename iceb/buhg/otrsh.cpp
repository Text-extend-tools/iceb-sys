/* $Id: otrsh.c,v 5.24 2013/09/26 09:43:36 sasa Exp $ */
/*15.07.2015    13.07.1996      Белых А.И.      otrsh.c
Подпрограмма получения всех отработанных часов и дней
по категориям
*/
#include        <errno.h>
#include        "buhg.h"

void            sappk1(short,class iceb_tu_str*,short,short,short,double*,FILE*,short*);
void            sch1(short,short,short,short,short,short*,short*,double*,double*,double*,short*,FILE*,short,short,const char[],const char[],const char[],const char[]);
void		rasmasov(FILE*,double*,short*,short,short,double*,short*,short*);


void otrsh(short mn,short gn,short mk,short gk,const char podr[],
const char kateg[],const char tabelq[],const char tabnom[],FILE *kaw)
{
short           kon;  /*Количество видов табелей*/
short           kka;  /*Количество категорий*/
int             in,ik;
struct  tm      *bf;
time_t          tmm;
SQL_str         row;
char		strsql[1024];
int		klst=0;

printw("%s %d.%d %s %d.%d\n",gettext("Период с"),mn,gn,
gettext("до"),mk,gk);
refresh();
/*Определяем количество табелей*/

kka=kon=0;
in=ik=0;
sprintf(strsql,"select kod from Kateg");
class SQLCURSOR cur;
if((kka=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kka == 0)
 {
  sprintf(strsql,"%s-%s!",__FUNCTION__,gettext("Не введены категории"));
  iceb_t_soob(strsql);
  return;
 }
short ka[kka];
memset(ka,'\0',sizeof(ka));


while(cur.read_cursor(&row) != 0)
  ka[ik++]=atoi(row[0]);

sprintf(strsql,"select kod from Tabel");

if((kon=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kon == 0)
 {
  sprintf(strsql,"%s-%s!",__FUNCTION__,gettext("Не введены виды табеля"));
  iceb_t_soob(strsql);
  return;
 }

short na[kon];
memset(na,'\0',sizeof(na));


while(cur.read_cursor(&row) != 0)
  na[in++]=atoi(row[0]);

double nao[kka*kon];
memset(nao,'\0',sizeof(nao));

double udo[kka*kon];
memset(udo,'\0',sizeof(udo));

double kal_dnei[kka*kon];
memset(kal_dnei,'\0',sizeof(kal_dnei));

short koll[kka];
memset(koll,'\0',sizeof(koll));


/*
printw("%d %d\n",kon,kka);
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

fprintf(kaw,"%s\n\n\
   	                   %s\n\
			   %s 1.%d.%d%s %s 31.%d.%d%s\n\
  		          %s %d.%d.%d%s  %s: %d:%d\n\n",
iceb_t_get_pnk("00",0),
gettext("Расчёт отработанного времени."),
gettext("Период с"),
mn,gn,
gettext("г."),
gettext("до"),
mk,gk,
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
if(kateg[0] != '\0')
 fprintf(kaw,"%s:%s\n",gettext("Коды категорий вошедшие в расчёт"),kateg);
if(tabelq[0] != '\0')
 fprintf(kaw,"%s:%s\n",gettext("Коды табеля вошедшие в расчёт"),tabelq);
if(tabnom[0] != '\0')
 fprintf(kaw,"%s:%s\n",gettext("Табельные номера вошедшие в расчёт"),tabnom);


sch1(mn,gn,mk,gk,0,na,ka,nao,udo,kal_dnei,koll,kaw,kon,kka,podr,kateg,tabelq,tabnom);


}


/********/
/*Поиск */
/********/
void            sch1(short mn,short gn,short mk,short gk,short dia,
short *na, //Массив кодов табелей
short *ka, //Массив категорий
double *nao, //Массив содержимого табелей часов
double *udo, //Массив содержимого табелей дней
double *kal_dnei, //Массив содержащий календарные дни
short *koll, //Массив количества человек в каждой категории
FILE *kaw,
short kon, //Количество видов табелей
short kka, //Количество категорий
const char podr[],const char kateg[],const char tabelq[],const char tabnom[])
{
int             i;
int             skn=0;
int             kkk=0;
unsigned short  mtn;  /*Количество человек*/
double          sn,su;
short           m,g;
long		kolstr;
SQL_str         row,row1;
char		strsql[512];
long		tbn;
int		kkat;
short		vt;
float		dnei=0.,has=0.,kal_dn=0.;
/*
printw("sch1-00000000000\n");
refresh();
*/
for(i=0;i<kka*kon;i++)
 nao[i]=udo[i]=0.;
for(i=0;i<kka;i++)
   koll[i]=0;

mtn=0;


g=gn;

for(m=mn; ; m++)
 {
  if(m==13)
   {
    m=1;
    g++;
   }
  if(g>gk)
    break;
  if(g == gk && m > mk)
    break;

  sprintf(strsql,"select tabn,podr,kateg from Zarn where god=%d and mes=%d",
  g,m);
  SQLCURSOR cur;
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }

  if(kolstr == 0)
   {
    continue;
   }


  while(cur.read_cursor(&row) != 0)
   {
    if(proverka(tabnom,row[0],0,0) != 0)
      continue;

    if(proverka(podr,row[1],0,0) != 0)
      continue;

    if(proverka(kateg,row[2],0,0) != 0)
      continue;


    tbn=atol(row[0]);
    kkat=atoi(row[2]);
        

    sprintf(strsql,"select kodt,dnei,has,kdnei from Ztab where god=%d and \
mes=%d and tabn=%ld",
    g,m,tbn);
    SQLCURSOR cur1;
    if((kolstr=cur1.make_cursor(&bd,strsql)) < 0)
     {
      msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
      continue;
     }

    if(kolstr == 0)
     {
      continue;
     }

    while(cur1.read_cursor(&row1) != 0)
     {
      if(proverka(tabelq,row1[0],0,0) != 0)
        continue;

      vt=atoi(row1[0]);
      dnei=atof(row1[1]);
      has=atof(row1[2]);
      kal_dn=atof(row1[3]);
      
      for(i=0 ; i < kka ; i++)
        if(ka[i] == kkat)
         {
   	  skn=i;
  	  break;
         }
      /*Если нет категории в списке читаем дальше*/
      if(i == kka)
       continue;


      for(i=0 ; i < kon ; i++)
       if(na[i] == vt)
	{
	 kkk=i;
	 break;
	}

      nao[kkk*kka+skn]+=has;
      udo[kkk*kka+skn]+=dnei;
      kal_dnei[kkk*kka+skn]+=kal_dn;
     }

     mtn++;
     for(i=0; i<kka ; i++)
       if(ka[i] == kkat)
	 koll[i]++;

   }
 }

/*Распечатываем*/
printw("%s\n-------------------------\n",
gettext("Распечатываем отработанные часы."));
fprintf(kaw,"%s\n",gettext("Отработанные часы."));
sn=su=0.;
rasmasov(kaw,nao,na,kka,kon,&sn,ka,koll);

printw("\n%s\n-------------------------\n",

gettext("Распечатываем отработанные дни."));
fprintf(kaw,"%s\n",gettext("Отработанные дни."));

rasmasov(kaw,udo,na,kka,kon,&su,ka,koll);

fprintf(kaw,"%s\n",gettext("Календаные дни."));
double ikdn=0.;
rasmasov(kaw,kal_dnei,na,kka,kon,&ikdn,ka,koll);

printw("\n\
%s:%.2f\n\
%s:%.2f\n\
%s:%.2f\n\
%s:%d\n",
gettext("Итого часов"),sn,
gettext("Итого дней"),su,
gettext("Итого календарных дней"),ikdn,
gettext("Количество работников"),mtn);

fprintf(kaw,"\n\
%s:%.2f\n\
%s:%.2f\n\
%s:%.2f\n\
%s:%d\n",
gettext("Итого часов"),sn,
gettext("Итого дней"),su,
gettext("Итого календарных дней"),ikdn,
gettext("Количество работников"),mtn);

}

/***************/
/*Шапка таблицы*/
/***************/
void sappk1(short kka, //Количество категорий
class iceb_tu_str *st,
short pn, //Позиция начала
short pk, //Позиция конца
short konu,  //Количество начислений или удержаний
double *nao,
FILE *kaw,short *ka)
{
int             i,i1,i2;
class iceb_tu_str nai("");
double          br2;
SQL_str         row;
char		strsql[512];
SQLCURSOR curr;
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

fprintf(kaw,"%*.*s|%-*s",
iceb_tu_kolbait(3,gettext("Код")),iceb_tu_kolbait(3,gettext("Код")),gettext("Код"),
iceb_tu_kolbait(30,gettext("Т а б е л я")),gettext("Т а б е л я"));

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
      nai.new_plus("");
       beep();
       printw("Не найден код категорії %d\n",ka[i]);
       OSTANOV();
     }
    else
      nai.new_plus(row[0]);

  fprintf(kaw,"|%-*.*s",iceb_tu_kolbait(12,nai.ravno()),iceb_tu_kolbait(12,nai.ravno()),nai.ravno());
 }
fprintf(kaw,"|%-*.*s|\n",iceb_tu_kolbait(12,gettext("Итого")),iceb_tu_kolbait(12,gettext("Итого")),gettext("Итого"));

fprintf(kaw,"   |%-30s"," ");
i2=0;
for(i=pn; i< pk+i2 && i < kka; i++)
 {
  /*Если колонка нулевая не печатать*/
  br2=0.;
  for(i1=i; i1< kka*konu ; i1+=kka)
   {
    br2+=nao[i1];
   }
  if(br2 != 0.)
    fprintf(kaw,"|%-12d",ka[i]);
  else
   i2++;
 }
fprintf(kaw,"|%-12.12s|\n"," ");

fprintf(kaw,"%s\n",st->ravno());

}

/********************/
/*Распечатка массива*/
/*********************/
void rasmasov(FILE *kaw,
double *nao, //Массив параметров
short *na,  //Массив с перечнем параметров
short kka,  //Количество категорий
short kon, //Количество параметров
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

*sn=0.;
for(ii=0 ; ii < kka; ii += kkol)
 {
  /*printw("ii-%d kka-%d korr-%d\n",ii,kka,korr);*/
  /*Проверяем если все равно нулю то печатаем следующюю группу столбиков*/
  i2=0; br2=0.;
  for(i=ii; i<ii+kkol+i2 && i < kka;i++)
   {
    br1=0.;
    for(skn=i;skn< kka*(kon-1)+ii+kkol && skn<kka*kon; skn+=kka)
      br1+=nao[skn];
    if(br1 == 0.)
     i2++;
    br2+=br1;
   }
  if(br2 == 0.)
    continue;
  sappk1(kka,&st,ii,(short)(ii+kkol),kon,nao,kaw,ka);
  br1=0.;
  for(i=0 ; i<kon;i++)
   {
    /*Проверяем если по горизонтали сумма нулевая то строчку не печатаем*/
/*  printw("\ni-%d kka-%d\n",i,kka);*/
    br1=0.;
     for(skn=0;skn< kka ; skn++)
      {
/*     printw("skn-%d nao[%d]=%.2f\n",skn,skn+i*kka,nao[skn+i*kka]);*/
       br1+=nao[skn+i*kka];
      }
/*  printw("\n\n");
    OSTANOV();   */
    if(br1 == 0.)
      continue;

    sprintf(strsql,"select naik from Tabel where kod=%d",na[i]);
    if(sql_readkey(&bd,strsql,&row,&curr) != 1)
     {
      nai.new_plus("");
       beep();
       printw("%s %d !\n",gettext("Не найден код табеля"),na[i]);
       OSTANOV();
     }
    else
     nai.new_plus(row[0]);

    fprintf(kaw,"%3d %-*.*s",
    na[i],
    iceb_tu_kolbait(30,nai.ravno()),iceb_tu_kolbait(30,nai.ravno()),nai.ravno());
/*
    if(kka-ii < kkol)
       kkk = kka-ii;
    else
       kkk = kkol;
  */
    /*Предыдущая информация*/
    br1=0.;
    if(ii > 0 )
     {
      for(skn=i*kka; skn<i*kka+ii && skn < kka*kon ;skn++)
       {
	/*printw(" skn=%d",skn);*/
	 br1+=nao[skn];
       }
      /*printw("br1=%.2f\n",br1);*/
     }
    i2=0;
    for(skn=i*kka+ii; skn<i*kka+ii+kkol+i2 && skn < i*kka+kka ;skn++)
     {

      /*Если колонка нулевая не печатать*/
      br2=0.;
      for(i1=skn-(i*kka); i1< kka*kon ; i1+=kka)
	 br2+=nao[i1];

      if(br2 != 0.)
       {
	/*printw("skn-%d (%d) br2=%.2f %d  \n",skn,i*kka+ii+kkk+i2,br2,kka*kon);*/
	fprintf(kaw," %12.2f",nao[skn]);
       }
      else
       {
	i2++;
       }
      br1+=nao[skn];
     }
    fprintf(kaw," %12.2f\n",br1);
    /*printw("%3d %-30s %.2f\n",zr2.tn,nai.ravno(),br1);*/
   }
  fprintf(kaw,"%s\n",st.ravno());
  fprintf(kaw,"%-*s",iceb_tu_kolbait(34,gettext("Итого")),gettext("Итого"));
  i2=0;
  /*printw("kkol=%d ii=%d kka=%d kon=%d\n",kkol,ii,kka,kon);*/
  for(i=ii; i<ii+kkol+i2 && i < kka ;i++)
   {
    /*printw("i=%d i2=%d  %d\n",i,i2,kka*(kon-1)+ii+kkol);*/

    br1=0.;
    for(skn=i;skn < kka*(kon-1)+ii+kkol+i2 && skn< kka*kon ; skn+=kka)
     {
  /*    printw("skn=%d\n",skn);*/
      br1+=nao[skn];
     }
    if(br1 != 0.)
       fprintf(kaw," %12.2f",br1);
    else
      i2++;
    *sn=*sn+br1;
   }
  fprintf(kaw," %12.2f\n",*sn);

  fprintf(kaw,"%s\n",st.ravno());

  /*Распечатываем количество человек*/
  fprintf(kaw,"%-*s",iceb_tu_kolbait(34,gettext("Количество работников")),gettext("Количество работников"));
  /*Предыдущая информация по людям*/
  khel=0;

  for(i=0; i<ii && i<kka;i++)
    khel+=koll[i];

  i2=0;
  for(i=ii; i<ii+kkol+i2 && i<kka;i++)
   {
      /*Если колонка нулевая не печатать*/
      br2=0.;
      for(i1=i; i1< kka*kon ; i1+=kka)
	 br2+=nao[i1];

      if(br2 != 0.)
       {
	fprintf(kaw," %12d",koll[i]);
	khel+=koll[i];
       }
      else
	i2++;
   }
  fprintf(kaw," %12d\n\n",khel);
  if(i == kka)
   break;
  ii+=i2;
 }
}
