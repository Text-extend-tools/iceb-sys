/* $Id: kategsh.c,v 5.33 2013/09/26 09:43:35 sasa Exp $ */
/*14.07.2015    23.11.1992      Белых А.И.      kategsh.c
Подпрограмма получения всех начислений/счет и 
удержаний/счет по категориям
*/
#include        <errno.h>
#include        "buhg.h"

void            sappksh(short,short,char[],short,short,short,FILE*,double*,short*,class SPISOK*,int,int);
void schsh(short mn,short gn,short mk,short gk,class SPISOK *na,short kon,class SPISOK *ud,short kou,short *ka,short kka,short *koll,FILE *kaw,double *nao,double *udo,const char podr[],const char kateg[],const char nahis[],const char uder[],short dia,const char shet[],FILE *ffhoz,FILE *ffbu);
void		rasmaskatsh(FILE*,double*,class SPISOK*,short,short,short,double*,short*,short*,int);
void		kategshh(short,short,short,short,short,struct tm*,const char*,const char*,const char*,const char*,const char*,int,FILE*);
int             bih(int,int,class SPISOK*,int);

extern char	*shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/

void kategsh(short mn,short gn,short mk,short gk,const char podr[],
const char kateg[],const char nahis[],const char uder[],const char shet[],FILE *kaw,FILE *ffhoz,FILE *ffbu)
{
short           kon;  /*Количество статей начисления/счет*/
short           kou;  /*Количество статей удержания/счет*/
short           kka;  /*Количество категорий*/
int             ik;
struct  tm      *bf;
time_t          tmm;
SQL_str         row;
char		strsql[512];
short		dkm;

GDITE();
printw("%s %d.%d %s %d.%d\n",gettext("Период с"),mn,gn,
gettext("до"),mk,gk);
refresh();

/*Создаем массив категорий*/
printw(gettext("Создаем массив категорий.\n"));
refresh();

sprintf(strsql,"select kod from Kateg");
class SQLCURSOR cur;
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
memset(ka,'\0',sizeof(ka));

ik=0;
while(cur.read_cursor(&row) != 0)
  ka[ik++]=atoi(row[0]);

/*
printw("%d %d %d\n",kon,kou,kka);
printw("Коды категорий - ");
for(i=0;i<kka;i++)
  printw(" %d",ka[i]);
printw("Конец\n");
OSTANOV();
  */
printw(gettext("Создаем массив начислений/счет.\n"));
refresh();

/*Создаем массив начислений/счетов*/
sprintf(strsql,"select distinct knah,shet from Zarp where \
datz >= '%04d-%d-1' and datz <= '%04d-%d-31' and prn='1' and suma <> 0. \
order by knah asc",gn,mn,gk,mk);

if((kon=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
class SPISOK nash; //Список начисление/счёт

if(kon == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи начислений !"));
  goto vp;
 }

while(cur.read_cursor(&row) != 0)
 {
/*
  printw("n-%s %s\n",row[0],row[1]);
  refresh();
*/
  sprintf(strsql,"%s|%s",row[0],row[1]);
  nash.plus(strsql);
 }
 

vp:;
printw(gettext("Создаем массив удержаний/счет.\n"));
refresh();

/*Создаем массив удержаний/счетов*/
sprintf(strsql,"select distinct knah,shet from Zarp where \
datz >= '%04d-%d-1' and datz <= '%04d-%d-31' and prn='2' and suma <> 0. \
order by knah asc",gn,mn,gk,mk);

if((kou=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

class SPISOK udsh; //Список удержание/счёт

if(kou == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи удержания !"));
 }

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s|%s",row[0],row[1]);
  udsh.plus(strsql);  
 }



double  nao[kka*kon];
memset(nao,'\0',sizeof(nao));

double udo[kka*kou];
memset(udo,'\0',sizeof(udo));

short koll[kka];
memset(koll,'\0',sizeof(koll));


time(&tmm);
bf=localtime(&tmm);

dpm(&dkm,&mk,&gk,5);

kategshh(mn,gn,dkm,mk,gk,bf,podr,nahis,uder,kateg,shet,0,kaw);

if(ffhoz != NULL)
 {
  kategshh(mn,gn,dkm,mk,gk,bf,podr,nahis,uder,kateg,shet,1,ffhoz);
 }
if(ffbu != NULL)
 {
  kategshh(mn,gn,dkm,mk,gk,bf,podr,nahis,uder,kateg,shet,2,ffbu);
 }

schsh(mn,gn,mk,gk,&nash,kon,&udsh,kou,ka,kka,koll,kaw,nao,udo,podr,kateg,nahis,uder,0,shet,ffhoz,ffbu);


podpis(kaw);

if(ffbu != NULL)
 {
  podpis(ffbu);
 }
if(ffhoz != NULL)
 {
  podpis(ffhoz);
 }
}


/********/
/*Поиск */
/********/
void            schsh(short mn,short gn, //Дата начала
short mk,short gk, //Дата конца
class SPISOK *na, //Массив кодов начислений
short kon, //Количество статей начисления
class SPISOK *ud, //Массив кодов удержаний
short kou, //Количество статей удержания
short *ka, //Массив категорий
short kka, //Количество категорий
short *koll,
FILE *kaw,double *nao,double *udo,const char podr[],const char kateg[],const char nahis[],
const char uder[],short dia,const char shet[],
FILE *ffhoz,FILE *ffbu)
{
int             i,i1;
int             skn=0;
int             mtn;  /*Количество человек*/
double          sn,su,snb,sub,snnb,sunb;
double          sum,sumb,sumnb;
double          dolgi,dolgib,dolginb;
short           m,g;
double          itsal,itsalb,itsalnb;  /*Итоговое сальдо*/
long		kolstr,kolstr1,kolstr2;
SQL_str         row,row1;
char		strsql[512];
long		tbn;
int		kkat;
short		prn;
double		sym;
double		saldb;

/*
printw("schsh: %d.%d %d.%d-------\n",mn,gn,mk,gk);
OSTANOV();
*/

for(i=0;i<kka*kou;i++)
 udo[i]=0.;
for(i=0;i<kka*kon;i++)
 nao[i]=0.;
for(i=0;i<kka;i++)
   koll[i]=0;

//ta=0;
mtn=0;
itsal=dolgi=sum=0.;
itsalb=dolgib=sumb=0.;
itsalnb=dolginb=sumnb=0.;
snb=sub=0.;
snnb=sunb=0.;

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
  sprintf(strsql,"select tabn,podr,kateg from Zarn where god=%d and mes=%d",g,m);
  printw("%02d.%d\n",m,g);
//  printw("%s\n",strsql);
  SQLCURSOR cur;
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    return;
   }
  if(kolstr == 0)
   {
    continue;
   }

  kolstr1=0;
  while(cur.read_cursor(&row) != 0)
   {
/*
    printw("Zarn-%s %s %s\n",row[0],row[1],row[2]);
    refresh();
*/
    strzag(LINES-1,0,kolstr,++kolstr1);

    if(proverka(podr,row[1],0,0) != 0)
      continue;
    if(proverka(kateg,row[2],0,0) != 0)
      continue;
    
    tbn=atol(row[0]);
//    kpd=atoi(row[1]);
    kkat=atoi(row[2]);

    for(i=0 ; i < kka ; i++)
     if(ka[i] == kkat)
       {
	skn=i;
	break;
       }

    /*Если нет категории в списке читаем дальше*/
    if(i == kka)
     {
//    printw("kkat=%d Нет в списке!\n",kkat);
//    OSTANOV();

      continue;
     }

    /*Читаем сальдо*/
    sum=sumb=sumnb=0.;
    if(m == mn && g == gn)
      sum=zarsald(1,m,g,tbn,&saldb);
    sumb=saldb;
    sumnb=sum-saldb;

    itsal+=sum;
    itsalb+=sumb;
    itsalnb+=sumnb;

    sprintf(strsql,"select prn,knah,suma,shet from Zarp where datz >= '%d-%02d-01' and \
datz <= '%d-%02d-31' and tabn=%ld and suma <> 0.",
    g,m,g,m,tbn);
//  printw("%s\n",strsql);
    class SQLCURSOR cur1;
    if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
     {
      msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
      continue;
     }
    if(kolstr2 == 0)
      continue;

      
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

//      knah=atoi(row1[1]);
      sym=atof(row1[2]);
      sum+=sym;
      if(shetb != NULL && shetbu != NULL)
       {
        if(row1[0][0] == '1')
         {
          if(pole1(shetb,row1[3],',',0,&i1) == 0 || SRAV(shetb,row1[3],0) == 0)
           {
            sumb+=sym;
            snb+=sym;
           }
          else
           {
            sumnb+=sym;
            snnb+=sym;
           }

         }
        if(row1[0][0] == '2')
         {
          if(pole1(shetbu,row1[3],',',0,&i1) == 0 || SRAV(shetbu,row1[3],0) == 0)
           {
            sumb+=sym;
            sub+=sym;
           }
          else
           {
            sumnb+=sym;
            sunb+=sym;
           }
         }
       }

      if(prn == 1)
       {
        sprintf(strsql,"%s|%s",row1[1],row1[3]);

//        if(pole1(na,strsql,'#',0,&i) != 0)
        if((i=na->find(strsql)) < 0)
         {
          beep();
          printw("Не найдено %s в массиве начислений/счетов!\n",strsql);
          OSTANOV();  
          continue;
         }       
	nao[i*kka+skn]+=sym;
       }
      if(prn == 2)
       {
        sprintf(strsql,"%s|%s",row1[1],row1[3]);

//        if(pole1(ud,strsql,'#',0,&i) != 0)
        if((i=ud->find(strsql)) < 0)
         {
          beep();
          printw("Не найдено %s в массиве удержаний/счетов !\n",strsql);
          OSTANOV();  
          continue;
         }       
	udo[i*kka+skn]+=sym;
       }

     }

     mtn++;
     for(i=0; i<kka ; i++)
       if(ka[i] == kkat)
	 koll[i]++;

    if(sum < 0.)
      dolgi+=sum;
    if(sumb < 0.)
      dolgib+=sumb;
    if(sumnb < 0.)
      dolginb+=sumnb;
    sum = 0.;

   }
  
 }

if(sum < 0.)
   dolgi+=sum;

sn=su=0.;

if(kon > 0)
 {
  printw("%s\n-------------------------\n",
  gettext("Распечатываем начисления."));
  refresh();
  /*Распечатываем начисления*/
  rasmaskatsh(kaw,nao,na,kka,kon,1,&sn,ka,koll,0);
 }

if(kou > 0)
 {
  printw("\n%s\n-------------------------\n",
  gettext("Распечатываем удержания."));
  refresh();
  /*Распечатываем удержания*/
  rasmaskatsh(kaw,udo,ud,kka,kou,2,&su,ka,koll,0);
 }
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
%s - %d\n",
gettext("Итого удержано :"),su,
gettext("Итого долгов   :"),dolgi,
gettext("К выдаче       :"),sn+su-dolgi+itsal,
gettext("Сальдо         :"),itsal+sn+su,
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
gettext("Сальдо         :"),itsal+sn+su);

if(shetbu != NULL && shetb != NULL)
 {


  printw("%s.\n------------------------------------------------\n",
  gettext("Бюджет"));
  printw("\n\
%s %15.2f\n\
%s %15.2f\n",
  gettext("Сальдо         :"),itsalb,
  gettext("Итого начислено:"),snb);
  printw("\
%s %15.2f\n\
%s %15.2f\n\
%s %15.2f\n\
%s %15.2f\n\n",
  gettext("Итого удержано :"),sub,
  gettext("Итого долгов   :"),dolgib,
  gettext("К выдаче       :"),snb+sub-dolgib+itsalb,
  gettext("Сальдо         :"),itsalb+snb+sub);

  refresh();

  if(kon > 0)
   {
    /*Распечатываем начисления*/
    rasmaskatsh(ffbu,nao,na,kka,kon,1,&sn,ka,koll,2);
   }

  if(kou > 0)
   {
    /*Распечатываем удержания*/
    rasmaskatsh(ffbu,udo,ud,kka,kou,2,&su,ka,koll,2);
   }

  fprintf(ffbu,"%s.\n------------------------------------------------\n",
  gettext("Бюджет"));

  fprintf(ffbu,"\
%s %15.2f\n\
%s %15.2f\n\
%s %15.2f\n\
%s %15.2f\n\
%s %15.2f\n\
%s %15.2f\n",
  gettext("Сальдо         :"),itsalb,
  gettext("Итого начислено:"),snb,
  gettext("Итого удержано :"),sub,
  gettext("Итого долгов   :"),dolgib,
  gettext("К выдаче       :"),snb+sub-dolgib+itsalb,
  gettext("Сальдо         :"),itsalb+snb+sub);


  printw("%s.\n------------------------------------------------\n",
  gettext("Хозрасчёт"));
  printw("\n\
%s %15.2f\n\
%s %15.2f\n",
  gettext("Сальдо         :"),itsalnb,
  gettext("Итого начислено:"),snnb);
  printw("\
%s %15.2f\n\
%s %15.2f\n\
%s %15.2f\n\
%s %15.2f\n\n",
  gettext("Итого удержано :"),sunb,
  gettext("Итого долгов   :"),dolginb,
  gettext("К выдаче       :"),snnb+sunb-dolginb+itsalnb,
  gettext("Сальдо         :"),itsalnb+snnb+sunb);

  refresh();

  if(kon > 0)
   {
    /*Распечатываем начисления*/
    rasmaskatsh(ffhoz,nao,na,kka,kon,1,&sn,ka,koll,1);
   }

  if(kou > 0)
   {
    /*Распечатываем удержания*/
    rasmaskatsh(ffhoz,udo,ud,kka,kou,2,&su,ka,koll,1);
   }

  fprintf(ffhoz,"%s.\n------------------------------------------------\n",
  gettext("Хозрасчёт"));

  fprintf(ffhoz,"\
%s %15.2f\n\
%s %15.2f\n\
%s %15.2f\n\
%s %15.2f\n\
%s %15.2f\n\
%s %15.2f\n",
  gettext("Сальдо         :"),itsalnb,
  gettext("Итого начислено:"),snnb,
  gettext("Итого удержано :"),sunb,
  gettext("Итого долгов   :"),dolginb,
  gettext("К выдаче       :"),snnb+sunb-dolginb+itsalnb,
  gettext("Сальдо         :"),itsalnb+snnb+sunb);
  
 }
}

/***************/
/*Шапка таблицы*/
/***************/
void            sappksh(short kk, //1-начисления  2-удержания*/
short kka,  //Количество категорий
class iceb_tu_str *st,
short pn,  //Позиция начала
short pk,  //Позиция конца
short konu,  //Количество начислений или удержаний
FILE *kaw,double *nao,short *ka,
class SPISOK *nan,
int metka,int metkash)
{
int             i,i1,i2;
class iceb_tu_str nai("");
double          br2;
SQL_str         row;
SQLCURSOR curr;
char		strsql[512];
int		nomstr=0;
/*Формируем строку подчеркивания*/
/*Полка над наименованием*/
st->new_plus("-----------------------------------------");
i2=0;
for(i=pn; i< pk+i2 && i < kka; i++)
 {
  /*Если колонка нулевая не печатать*/
  br2=0.;
  nomstr=0;
  for(i1=i; i1< kka*konu ; i1+=kka)
   if(bih(metka,metkash,nan,nomstr++) == 0)
    br2+=nao[i1];

  if(br2 != 0. )
    st->plus("-------------");
  else
   i2++;
 }
/*Полка над ИТОГО*/
st->plus("-------------");

fprintf(kaw,"\n%s\n",st->ravno());

if(kk == 1)
 fprintf(kaw,"Код|%-*s|%-*.*s",
 iceb_tu_kolbait(30,gettext("Начисления")),gettext("Начисления"),
 iceb_tu_kolbait(5,gettext("Счёт")),iceb_tu_kolbait(5,gettext("Счёт")),gettext("Счёт"));

if(kk == 2)
 fprintf(kaw,"Код|%-*s|%-*.*s",
 iceb_tu_kolbait(30,gettext("Удержания")),gettext("Удержания"),
 iceb_tu_kolbait(5,gettext("Счёт")),iceb_tu_kolbait(5,gettext("Счёт")),gettext("Счёт"));

i2=0;
for(i=pn; i< pk+i2 && i < kka; i++)
 {
  /*Если колонка нулевая не печатать*/
  br2=0.;
  nomstr=0;
  for(i1=i; i1< kka*konu ; i1+=kka)
   if(bih(metka,metkash,nan,nomstr++) == 0)
    br2+=nao[i1];

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

fprintf(kaw,"   |%-30s|%5.5s"," "," ");
i2=0;
for(i=pn; i< pk+i2 && i < kka; i++)
 {
  /*Если колонка нулевая не печатать*/
  br2=0.;
  nomstr=0;
  for(i1=i; i1< kka*konu ; i1+=kka)
   if(bih(metka,metkash,nan,nomstr++) == 0)
    br2+=nao[i1];

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

void		rasmaskatsh(FILE *kaw,
double *naon, //Массив параметров
class SPISOK *nan,    //Массив с перечнем параметров
short kkan,   //Количество категорий
short konn,   //Количество параметров
short metka,  //1-начисления 2-удержания
double *sn,   //Сумма по массиву
short *ka,short *koll,
int metkash) //0-все 1-хозрасчёт 2-бюджет
{
int		ii,i1,i2,i,skn;
short           kkol=15; /*Количество колонок в распечатке*/
double		br1,br2;
int		khel;
class iceb_tu_str st("");
SQL_str         row;
class iceb_tu_str nai("");
char		strsql[512];
class iceb_tu_str kod("");
class iceb_tu_str shet("");
int		nomstr=0;
SQLCURSOR curr;

*sn=0.;
for(ii=0 ; ii < kkan; ii += kkol)
 {
  /*printw("ii-%d kkan-%d korr-%d\n",ii,kkan,korr);*/
  /*Проверяем если все равно нулю то печатаем следующюю группу столбиков*/
  i2=0; br2=0;
  for(i=ii; i<ii+kkol+i2 && i < kkan;i++)
   {
    br1=0.;
    nomstr=0;   
    for(skn=i;skn< kkan*(konn-1)+ii+kkol && skn<kkan*konn; skn+=kkan)
      if( bih(metka,metkash,nan,nomstr++) == 0)
       br1+=naon[skn];

    if(br1 == 0.)
     i2++;
    br2+=br1;
   }
  if(br2 == 0.)
    continue;
  sappksh(metka,kkan,&st,ii,(short)(ii+kkol),konn,kaw,naon,ka,nan,metka,metkash);
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

    polen(nan->ravno(i),&kod,1,'|');
    polen(nan->ravno(i),&shet,2,'|');
    
    if(metka == 1) //Начисления
     {
      if(metkash == 2) //Бюджет
       if(proverka(shetb,shet.ravno(),0,1) != 0)
         continue;

      if(metkash == 1) //Хозрасчёт
       if(proverka(shetb,shet.ravno(),0,1) == 0)
         continue;
      sprintf(strsql,"select naik from Nash where kod=%s",kod.ravno());
     }            
    if(metka == 2)
     {
      if(metkash == 2) //Бюджет
       if(proverka(shetbu,shet.ravno(),0,1) != 0)
         continue;

      if(metkash == 1) //Хозрасчёт
       if(proverka(shetbu,shet.ravno(),0,1) == 0)
         continue;
      sprintf(strsql,"select naik from Uder where kod=%s",kod.ravno());
     }
    nai.new_plus("");
    if(sql_readkey(&bd,strsql,&row,&curr) != 1)
     {
      beep();
      if(metka == 1) 
       printw("%s %s (%s)!\n",gettext("Не найден код начисления"),kod.ravno(),nan);
      if(metka == 2) 
        printw("%s %s !\n",gettext("Не найден код удержания"),kod.ravno());
      OSTANOV();
     }
    else
     nai.new_plus(row[0]);

    fprintf(kaw,"%3s %-*.*s %-*.*s",
    kod.ravno(),
    iceb_tu_kolbait(30,nai.ravno()),iceb_tu_kolbait(30,nai.ravno()),nai.ravno(),
    iceb_tu_kolbait(5,shet.ravno()),iceb_tu_kolbait(5,shet.ravno()),shet.ravno());

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
      nomstr=0;
      for(i1=skn-(i*kkan); i1< kkan*konn ; i1+=kkan)
       if(bih(metka,metkash,nan,nomstr++) == 0)
	 br2+=naon[i1];

      if(br2 != 0.)
       {
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

  fprintf(kaw,"%-*s",iceb_tu_kolbait(39,gettext(" И т о г о")),gettext(" И т о г о"));

  i2=0;
  for(i=ii; i<ii+kkol+i2 && i < kkan ;i++)
   {
    br1=0.;
    nomstr=0;
    for(skn=i;skn < kkan*(konn-1)+ii+kkol+i2 && skn< kkan*konn ; skn+=kkan)
      if(bih(metka,metkash,nan,nomstr++) == 0)
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
  fprintf(kaw,"%-*s",iceb_tu_kolbait(39,gettext(" Количество работающих ")),gettext(" Количество работающих "));
  /*Предыдущая информация по людям*/
  khel=0;

  for(i=0; i<ii && i<kkan;i++)
    khel+=koll[i];

  i2=0;
  for(i=ii; i<ii+kkol+i2 && i<kkan;i++)
   {
      /*Если колонка нулевая не печатать*/
      br2=0.;
      nomstr=0;
      for(i1=i; i1< kkan*konn ; i1+=kkan)
       if(bih(metka,metkash,nan,nomstr++) == 0)
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
  ii+=i2;
 }

}
/**********************************/
/*Шапка отчёта                    */
/**********************************/

void		kategshh(short mn,short gn,
short dkm,short mk,short gk,
struct tm *bf,
const char *podr,
const char *nahis,
const char *uder,
const char *kateg,
const char *shet,
int metkaot, //0-все 1-хозрасчёт 2-бюджет
FILE *kaw)
{
int		klst=0;

fprintf(kaw,"%s\n\n\
		      %s\n\
			   %s 1.%d.%d%s %s %d.%d.%d%s\n\
 		         %s %d.%d.%d%s  %s: %d:%d\n\n",
iceb_t_get_pnk("00",0),
gettext("Расчёт начислений и удержаний по категориям."),
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

if(metkaot == 1)
 fprintf(kaw,"%s.\n",gettext("Хозрасчёт"));
if(metkaot == 2)
 fprintf(kaw,"%s.\n",gettext("Бюджет"));
 
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
if(shet[0] != '\0')
 fprintf(kaw,"%s:%s\n",gettext("Счёт"),shet);
}

/***************************************/
/*Рзаборка с бюджетом и хозрасчётом    */
/***************************************/
//Если вернули 0 - подходить 1-нет
int    bih(int metka,int metkash,
class SPISOK *nan,int i)    //Массив с перечнем параметров
{
class iceb_tu_str shet("");

polen(nan->ravno(i),&shet,2,'|');

if(metka == 1) //Начисления
 {
  if(metkash == 2) //Бюджет
   if(proverka(shetb,shet.ravno(),0,1) != 0)
     return(1);

  if(metkash == 1) //Хозрасчёт
   if(proverka(shetb,shet.ravno(),0,1) == 0)
     return(1);
 }            
if(metka == 2)
 {
  if(metkash == 2) //Бюджет
   if(proverka(shetbu,shet.ravno(),0,1) != 0)
     return(1);

  if(metkash == 1) //Хозрасчёт
   if(proverka(shetbu,shet.ravno(),0,1) == 0)
     return(1);
 }

return(0);

}
