/* $Id: rasnu.c,v 5.23 2013/09/26 09:43:38 sasa Exp $ */
/*30.03.2016    25.05.1993      Белых А.И.              rasnu.c
Подпрограмма выбора распечатки начислений и удержаний
*/
#include        <errno.h>
#include        "buhg.h"

extern char     *printer;
extern char     *printer1;
extern class iceb_tu_str shrpz; /*Счет расчётов по зарплате*/

void rasnu(short g,short m,int tn,
double sl, //Сальдо
short mp,  //0-системная печать 1- приэкранная
const char *fio)
{
struct  tm      *bf;
short		na;
char            str[1024];
char            imf[64];
char            imf1[64];
double          nah=0.,uder=0.;
double          dolg,bbr;
FILE		*f,*f1;
long		kolstr;
SQL_str         row,row1;
char		strsql[512];
class iceb_tu_str naim("");

bbr=0.;

sprintf(imf,"rnu%d.dat",tn);

if((f = fopen(imf,"w")) == NULL)
 {
  error_op_nfil(imf,errno,"");
  return;
 }
fprintf(f,"%s\n\%s\n\
----------------------------------------------------------\n",
gettext("Отмеченные знаком \"+\" строки начислений и удержаний"),
gettext("войдут в распечатку."));

sprintf(strsql,"select * from Zarp where datz >='%04d-%02d-01' and \
datz <= '%04d-%02d-31' and tabn=%d order by prn,knah asc",
g,m,g,m,tn);
SQLCURSOR cur;
SQLCURSOR curr;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  printw("%s\n",sql_error(&bd));
  OSTANOV();
 }

if(kolstr == 0)
 {
  return;
 }

fprintf(f,"+%-*s | %2d.%4d | %10.2f\n",iceb_tu_kolbait(30,gettext("Сальдо")),gettext("Сальдо"),m,g,sl);

while(cur.read_cursor(&row) != 0)
 {
  na=atoi(row[2]);
  if(na == 1)
    sprintf(strsql,"select naik from Nash where kod=%s",row[3]);
  if(na == 2)
    sprintf(strsql,"select naik from Uder where kod=%s",row[3]);
     
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    beep();
    if(na == 1)
     printw("%s %s !\n",gettext("Не найден код начисления"),row[3]);
    if(na == 2)
     printw("%s %s !\n",gettext("Не найден код удержания"),row[3]);
    OSTANOV();
    naim.new_plus("");
   }
  else
    naim.new_plus(row1[0]);


  fprintf(f,"+%-*s | %2s.%4s | %10.2f\n",
  iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),row[7],row[6],atof(row[4]));
 }
fclose(f);

sprintf(str,"%s",imf);
vizred(str);

if((f1 = fopen(imf,"r")) == NULL)
 {
  error_op_nfil(imf,errno,"");
  return;
 }

nah=uder+0.;
while(fgets(str,sizeof(str),f1) != NULL)
 {
  if(str[0] == '+')
   {
    polen(str,&bbr,3,'|');
//    bbr=ATOFM(sh);

    if(bbr > 0.)
      nah+=bbr;
    else
      uder+=bbr;

   }
 }

dolg=bbr=nah+uder;
if(dolg < 0.)
    bbr=0.;

fclose(f1);
unlink(imf);

time_t tmm;
time(&tmm);
bf=localtime(&tmm);

sprintf(imf1,"kon%d.lst",tn);

char osnovan[128];
sprintf(osnovan,"%s %d.%d%s",gettext("Заработная плата."),m,g,gettext("г."));

//Узнаем номер документа
class iceb_tu_str vidan("");
sprintf(strsql,"select nomp,vidan from Kartb where tabn=%d",
tn);
if(sql_readkey(&bd,strsql,&row,&curr) == 1)
 {
//  sprintf(vidan,"%s %s %s",gettext("Паспорт"),row[0],row[1]);
  vidan.new_plus(gettext("Паспорт"));
  vidan.plus(" ",row[0]);
  vidan.plus(" ",row[1]);
 }
sprintf(strsql,"%s%d",ICEB_ZAR_PKTN,tn);
kasord2(imf1,"",bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,shrpz.ravno(),strsql,bbr,fio,osnovan,"",vidan.ravno(),"",f1);

class spis_oth oth;
oth.spis_imaf.plus(imf1);
oth.spis_naim.plus(gettext("Кассовый ордер"));
iceb_t_ustpeh(imf1,1);
iceb_t_rabfil(&oth,"");


}
