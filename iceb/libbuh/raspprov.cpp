/*$Id: raspprov.c,v 5.22 2013-04-07 10:22:55 sasa Exp $*/
/*28.03.2013	04.07.2001	Белых А.И.	raspprov.c
Распечатка проводок из подсистем "Материальный учет",
"Расчёт зарплаты" "Учет основных средств", "Учет кассовых ордеров",
"Учет услуг"
*/
#include        <errno.h>
#include        "buhl.h"


void	raspprov(SQLCURSOR *cur,
short dd,short md,short gd, //Дата документа
const char *nomd, //номер документа
int metkapros, //Метка просмотра 1-дебеты 2-кредиты 3-все
int pods) //1-материальный учет
          //2-Расчёт зарплаты
          //3-Учет основных средств
          //4-Учет услуг
          //5-Учет кассовых ордеров
          //6-платежные документы
          //7-командировочные расходы
{
double	deb,kre;
double	ideb=0.,ikre=0.;
char	imaf[64];
FILE	*ff;
short   d,m,g;
time_t	tmm;
struct  tm      *bf;
SQL_str		row;


VVOD SOOBN(1);
SOOBN.VVOD_SPISOK_add_MD(gettext("Выгружаем проводки в файл"));
soobsh(&SOOBN,-1,-1,1);

move(LINES-1,0);

GDITE();

sprintf(imaf,"provp%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
  {
   error_op_nfil(imaf,errno,"");
   return;
  }

time(&tmm);
bf=localtime(&tmm);
fprintf(ff,"%s\n\n\
%s.\n\
%s %d.%d.%d  %s: %d:%d\n",
iceb_t_get_pnk("00",0),
gettext("Распечатка проводок"),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("Время"),bf->tm_hour,bf->tm_min);

if(pods == 1)
 fprintf(ff,"%s\n",gettext("Материальный учет"));
if(pods == 2)
 fprintf(ff,"%s.\n",gettext("Расчёт зарплати"));
if(pods == 3)
 fprintf(ff,"%s.\n",gettext("Учет основных средств"));
if(pods == 4)
 fprintf(ff,"%s.\n",gettext("Учет услуг"));
if(pods == 5)
 fprintf(ff,"%s.\n",gettext("Учет кассовых ордеров"));
if(pods == 6)
 fprintf(ff,"%s.\n",gettext("Платежные документы"));
if(pods == 7)
 fprintf(ff,"%s.\n",gettext("Учёт командировочных расходов"));

if(pods == 2)
 {
  fprintf(ff,"%d.%d%s %s\n",
  md,gd,
  gettext("г."),
  nomd);
 }
else
 { 
  fprintf(ff,"%s:%s %s %d.%d.%d%s\n",
  gettext("Документ"),
  nomd,
  gettext("от"),
  dd,md,gd,
  gettext("г."));
 }
fprintf(ff,"\
--------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
  Дата    |Счет |СчетК|  Дебет   |  Кредит  |Конт.|Кто |  Коментарий\n"));
fprintf(ff,"\
--------------------------------------------------------------------------\n");
cur->poz_cursor(0);
while(cur->read_cursor(&row) != 0)
 {
  deb=atof(row[4]);
  kre=atof(row[5]);

  if(metkapros == 1 && kre != 0.)
    continue;

  if(metkapros == 2 && deb != 0.)
    continue;
  rsdat(&d,&m,&g,row[0],2);
  
  fprintf(ff,"%02d.%02d.%d %-*s %-*s %10.2f %10.2f %-*s %-4s %s\n",
  d,m,g,
  iceb_tu_kolbait(5,row[1]),row[1],
  iceb_tu_kolbait(5,row[2]),row[2],
  deb,kre,
  iceb_tu_kolbait(5,row[3]),row[3],
  row[8],row[7]);
  
  ideb+=deb;
  ikre+=kre;  
 }
fprintf(ff,"\
--------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f %10.2f\n",
iceb_tu_kolbait(22,gettext("Итого")),gettext("Итого"),ideb,ikre);


podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка проводок"));
iceb_t_ustpeh(imaf,0);
iceb_t_rabfil(&oth,"");
}
