/* $Id: nahud.c,v 5.29 2013/09/26 09:43:36 sasa Exp $ */
/*30.03.2016    07.12.1992      Белых А.И.      nahud.c
Подпрограмма поиска всех начислений и удержаний на конкретного
человека по конкретному номеру
*/
#include        <errno.h>
#include        "buhg.h"

extern char	*shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/
extern short	*obud; /*Обязательные удержания*/
extern double   okrg; /*Округление*/
extern class iceb_tu_str shrpz; /*Счет расчётов по зарплате*/

void nahud(short g,short m, //Дата
long tn, //Табельный номер
double *nah,
double *nahprov, //Сумма начисления на которую надо сделать проводки
double *uder, //Общая сумма удержаний
double *uderprov, //Cумма удержаний для котрой нужно сделать проводки
double *nahb,
double *uderb,
double *uderbprov,
short ras,  //0-не распечатывать 1-распечатывать на печать 2-на экран*/
double sl,  //Сальдо
short mp,  //0-На системный принтер 1-на приэкранную печать
const char *fio)
{
char            bros[512];
struct  tm      *bf=NULL;
time_t          tmm;
FILE            *ff=NULL;
double          dolg=0.,bbr=0.;
short           uhsl;
double          ius;
char		imaf[64];
class iceb_tu_str nai("");
long		kolstr;
SQL_str         row,row1;
char		strsql[512];
short		dz,mz,gz;
short		mz1,gz1;
short		prn,knah;
double		sym;
double		sumabu; /*Сумма бюджетных удержаний*/
int		i1;
short		metkaprov=0;
SQLCURSOR curr;
*nah=*nahprov=*uder=*uderprov=*nahb=*uderb=*uderbprov=0.;
uhsl=0;

if(ras != 0)
 {
  time(&tmm);
  bf=localtime(&tmm);
  bros[0]='\0';
  uhsl=0;
  if(iceb_t_poldan("Учет сальдо в распечатке",bros,"zarnast.alx") == 0)
     if(SRAV(bros,"Вык",1) == 0)
       uhsl=1;
 }
sprintf(strsql,"select * from Zarp where datz >='%d-%02d-01' and \
datz <= '%d-%02d-31' and tabn=%ld order by prn,knah asc",
g,m,g,m,tn);

SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  if(sql_nerror(&bd) != ER_TABLEACCESS_DENIED_ERROR) //Запрет команды select  
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
  return;

ius=0.;
while(cur.read_cursor(&row) != 0)
 {
  rsdat(&dz,&mz,&gz,row[0],2);
  prn=atoi(row[2]);
  knah=atoi(row[3]);
  sym=atof(row[4]);
  gz1=atoi(row[6]);
  mz1=atoi(row[7]);
  if(prn == 1)
   {
    *nah+=sym;
    if(shetb != NULL)
     if(pole1(shetb,row[5],',',0,&i1) == 0 || SRAV(shetb,row[5],0) == 0)
       *nahb+=sym;

    //Проверяем нужно ли делать проводки для этого удержания
    metkaprov=0;
    sprintf(strsql,"select prov from Nash where kod=%d",knah);
    if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
      if(row1[0][0] == '1')
       {
        metkaprov=1;
       }

   if(metkaprov == 0)
    *nahprov+=sym;
   
   }
  if(prn == 2)
   {
    if(uhsl == 1 && ras != 0)
     {
      if(mz1 == m && gz1 == g)
       {
	ius+=sym;
//        printw("sym=%.2f m=%d mz1=%d g=%d gz1=%d\n",
//        sym,m,mz1,g,gz1);
       }
      else
       {
//        printw("*sym=%.2f\n",sym);
        if(obud != NULL)
         for(i1=1; i1<= obud[0]; i1++)
          if(obud[i1] == knah)
           {
    	    ius+=sym;
            break;
           }
       }

     }
    //Проверяем нужно ли делать проводки для этого удержания
    metkaprov=0;
    sprintf(strsql,"select prov from Uder where kod=%d",knah);
    if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
      if(row1[0][0] == '1')
       {
        metkaprov=1;
       }

   *uder+=sym;
   if(metkaprov == 0)
    *uderprov+=sym;

   if(shetbu != NULL)
    if(pole1(shetbu,row[5],',',0,&i1) == 0 || SRAV(shetbu,row[5],0) == 0)
     {
      *uderb+=sym;
      if(metkaprov == 0)
        *uderbprov+=sym;
     }
   }
  if(ras == 2)
   {
    if(prn == 1)
      sprintf(strsql,"select naik from Nash where kod=%s",row[3]);
    if(prn == 2)
      sprintf(strsql,"select naik from Uder where kod=%s",row[3]);
       
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      beep();
      if(prn == 1)
       printw("%s %s !\n",gettext("Не найден код начисления"),row[3]);
      if(prn == 2)
       printw("%s %s\n",gettext("Не найден код удержания"),row[3]);
      OSTANOV();
      nai.new_plus("");
     }
    else
      nai.new_plus(row1[0]);
    printw("%2d %02d %-*.*s %*s.%s %15s %s\n",
    knah,dz,
    nai.ravno(),
    iceb_tu_kolbait(30,row[7]),iceb_tu_kolbait(30,row[7]),row[7],
    iceb_tu_kolbait(2,row[6]),row[6],
    prnbr(sym),
    row[12]);
   }
 }

*nah=okrug(*nah,okrg);
*nahprov=okrug(*nahprov,okrg);
*uder=okrug(*uder,okrg);
*uderprov=okrug(*uderprov,okrg);

*nahb=okrug(*nahb,okrg);
*uderb=okrug(*uderb,okrg);
*uderbprov=okrug(*uderbprov,okrg);

if(ras != 0)
 {

  if(uhsl == 0)
     dolg=bbr=*nah+*uder+sl;

  if(uhsl == 1)
   {
    bbr=prbper(m,g,bf->tm_mon+1,bf->tm_year+1900,tn,&sumabu,0);
    dolg=bbr=*nah+ius+bbr;
   }

  if(dolg < 0.)
    bbr=0.;
  if(ras == 1)
   {
    time(&tmm);
    bf=localtime(&tmm);
    sprintf(imaf,"ko%ld.lst",tn);
    char osnovan[512];
    sprintf(osnovan,"%s %d.%d%s",gettext("Заработная плата."),m,g,gettext("г."));
    //Узнаем номер документа
    class iceb_tu_str nomd("");
    char vidan[512];
    memset(vidan,'\0',sizeof(vidan));
    sprintf(strsql,"select nomp,vidan,datvd from Kartb where tabn=%ld",tn);
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
     {
      short d=0,m,g;
      rsdat(&d,&m,&g,row[2],2);
      nomd.new_plus(row[0]);
      if(d == 0)
       sprintf(vidan,"%s %s %s",gettext("Паспорт"),nomd.ravno(),row[1]);
      else
       sprintf(vidan,"%s %s %02d.%02d.%d%s %s",gettext("Паспорт"),nomd.ravno(),d,m,g,gettext("г."),row[1]);
      
     }
    sprintf(bros,"%s%ld",ICEB_ZAR_PKTN,tn);    
    kasord2(imaf,"",bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,shrpz.ravno(),bros,bbr,fio,osnovan,"",vidan,"",ff);

    class spis_oth oth;
    oth.spis_imaf.plus(imaf);
    oth.spis_naim.plus(gettext("Кассовый ордер"));
    iceb_t_rabfil(&oth,"");
   }
  if(ras == 2)
   {
    bbr=(*nah)+(*uder);
    printw("\
------------------------------------------------------\n\
%39s %15s\n",gettext("Итого"),prnbr(bbr));
   }
 }

}
