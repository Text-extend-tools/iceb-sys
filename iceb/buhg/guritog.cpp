/* $Id: guritog.c,v 5.12 2013/09/26 09:43:33 sasa Exp $ */
/*14.07.2015    23.10.1996      Белых А.И.      guritog.c
Распечатка итогов по счетам после вывода журнала ордера
в вертикальной форме для счёта с развернутым сальдо
*/
#include        "buhg.h"


void            guritog(char sh[],char nsh[],short dn,short mn,short gn,
short dk,short mk,short gk,FILE *ff,class SPISOK *mso, //Счета  общий
int koo,
class masiv_din_double *mdos,
class masiv_din_double *mkos)
{
int kso=mso->kolih();   /*Количество счетов общий*/
struct  tm      *bf;
time_t		tmm;
char		bros[512];
int             i,i1;
double          db,kr,dbi,kri;
char            shet[64];
SQL_str   	row;
SQLCURSOR       cur;

time(&tmm);
bf=localtime(&tmm);

fprintf(ff,"\f\%s\n\n\
%s %s %s\n\
%s\n\
%s %d.%d.%d%s %s %d.%d.%d%s\n\
%s %d.%d.%d%s %s - %d:%d\n",
iceb_t_get_pnk("00",0),
gettext("Счёт"),
sh,nsh,
gettext("Всего по счетам корреспондентам."),
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(ff,"\
--------------------------------------------------------------------------\n");
fprintf(ff,gettext("  Счёт    |   Наименование счета         |   Дебет       |   Кредит      |\n"));
fprintf(ff,"\
--------------------------------------------------------------------------\n");


dbi=kri=0.;

for(i=0 ; i < kso; i++)
 {
  db=kr=0.;
  for(i1=i; i1 < koo*kso ; i1+=kso)
   {
    db+=mdos->ravno(i1);
    kr+=mkos->ravno(i1);
   }
  dbi+=db;
  kri+=kr;
  strncpy(shet,mso->ravno(i),sizeof(shet)-1);
  sprintf(bros,"select nais from Plansh where ns='%s'",
  shet);

  if(sql_readkey(&bd,bros,&row,&cur) != 1)
   {
    beep();
    printw(gettext("Не найден счёт %s в плане счетов !\n"),shet);
    OSTANOV();
    continue;
   }
  strncpy(bros,row[0],sizeof(bros)-1);
  if(db != 0. || kr != 0.)
   {
    fprintf(ff,"%-*s %-*.*s %15s",
    iceb_tu_kolbait(10,shet),shet,
    iceb_tu_kolbait(30,bros),iceb_tu_kolbait(30,bros),bros,prnbr(db));
    fprintf(ff," %15s\n",prnbr(kr));
   }
 }
fprintf(ff,"\
--------------------------------------------------------------------------\n");
fprintf(ff,"%*s %15s",iceb_tu_kolbait(41,gettext("Итого")),gettext("Итого"),prnbr(dbi));

fprintf(ff," %15s\n",prnbr(kri));
}
