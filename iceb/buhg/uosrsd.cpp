/* $Id: uosrsd.c,v 5.24 2013/09/26 09:43:42 sasa Exp $ */
/*13.06.2013    22.08.1997      Белых А.И.      uosrsd.c
Распечатка перечня документов по группе 2 и 3 или инвентарному
номеру
*/
#include        <errno.h>
#include        "buhg.h"
#include "poiinpd.h"


int uosrsd(int in)
{
time_t 		tmm;
long		innom;
FILE		*ff;
char		imaf[64];
long		kolstr;
SQL_str         row,row1;
SQLCURSOR curr;
char		strsql[512];
short		d,m,g;
short		tp;
class iceb_tu_str kontr("");
class iceb_tu_str kodop("");
short		podt=0;
double		bs,iz,bsby,izby;
double		ibs=0.,iiz=0.;
double		ibsby=0.,iizby=0.;
class iceb_tu_str hna("");   /*Шифр нормы амортотчислений*/

hna.new_plus(in*(-1));

if(in < 0)
 sprintf(strsql,"select datd,innom,tipz,nomd,bs,iz,bsby,izby \
from Uosdok1");
else
 sprintf(strsql,"select datd,innom,tipz,nomd,bs,iz,bsby,izby \
from Uosdok1 where innom=%d",in);

class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(1);
 }



clear();
GDITE();
time(&tmm);

sprintf(imaf,"us%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
   {
    error_op_nfil(imaf,errno,"");
    return(1);
   }

iceb_t_zagolov(gettext("Распечатка записей документов."),0,0,0,0,0,0,ff);
if(in < 0)
 printw("%s %d\n",gettext("Группа"),in*(-1));
else
 printw("%s %d\n",gettext("Инвентарный номер"),in);

printw("\
--------------------------------------------------------------------------\n");

printw(gettext("\
Инвент.|О|  Дата    |Номер|Опе|Контр|Налоговый учёт   |Бухгалтерский уч.|\n"));

printw(gettext("\
номер  | | документа|докум|рац|агент|Бал.ст. | Износ  |Бал.ст. | Износ  |\n"));

printw("\
--------------------------------------------------------------------------\n");
if(in < 0)
  fprintf(ff,"%s %d\n",gettext("Группа"),in*(-1));
else
 fprintf(ff,"%s %d\n",gettext("Инвентарный номер"),in);

fprintf(ff,"\
---------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
Инвент.|О|  Дата    |Номер|Опе|Контр|Налоговый учёт   |Бухгалтерский уч.|\n"));

fprintf(ff,gettext("\
номер  | | документа|докум|рац|агент|Бал.ст. | Износ  |Бал.ст. | Износ  |\n"));

fprintf(ff,"\
---------------------------------------------------------------------------------\n");

refresh();
class poiinpdw_data rekin;
while(cur.read_cursor(&row) != 0)
 {
  rsdat(&d,&m,&g,row[0],2);
  innom=atol(row[1]);
  
  /*Читаем переменные данные*/

  poiinpd(innom,m,g,&rekin);

  if(in < 0)
   if(SRAV(hna.ravno(),rekin.hna.ravno(),0) != 0)
    continue;

  tp=atoi(row[2]);
  bs=atof(row[4]);
  iz=atof(row[5]);
  bsby=atof(row[6]);
  izby=atof(row[7]);
  if(tp == 2)
   {
    bs*=-1;
    bsby*=-1;
    izby*=-1;
   }  

  /*Читаем шапку документа*/
  sprintf(strsql,"select kontr,kodop,podt from Uosdok where datd='%d.%02d.%02d' and \
  nomd='%s'",g,m,d,row[3]);

  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    sprintf(strsql,"%s %s %d.%d.%d%s",
    gettext("Не найден документ !"),row[3],d,m,g,
    gettext("г."));
    iceb_t_soob(strsql);
    kontr.new_plus("");
    kodop.new_plus("");
   }
  else
   {
    kontr.new_plus(row1[0]);
    kodop.new_plus(row1[1]);
    podt=atoi(row1[2]);
   }  

  printw("%7ld ",innom);
  fprintf(ff,"%7ld ",innom);

  if(tp == 1)
   {
    printw("+");
    fprintf(ff,"+");
   }
  if(tp == 2)
   {
    printw("-");
    fprintf(ff,"-");
   }

  printw(" %02d.%02d.%d %-*s %-*s %-*s %8.2f %8.2f %8.2f %8.2f",
  d,m,g,
  iceb_tu_kolbait(5,row[3]),row[3],
  iceb_tu_kolbait(3,kodop.ravno()),kodop.ravno(),
  iceb_tu_kolbait(5,kontr.ravno()),kontr.ravno(),
  bs,iz,bsby,izby);
  refresh();
  
  fprintf(ff," %02d.%02d.%d %-*s %-*s %-*s %8.2f %8.2f %8.2f %8.2f",
  d,m,g,
  iceb_tu_kolbait(5,row[3]),row[3],
  iceb_tu_kolbait(3,kodop.ravno()),kodop.ravno(),
  iceb_tu_kolbait(5,kontr.ravno()),kontr.ravno(),
  bs,iz,bsby,izby);
  
  ibs+=bs;
  iiz+=iz;
  ibsby+=bsby;
  iizby+=izby;
  if(podt == 0)
   {
    printw(" %5.5s\n",gettext("Не подтвержден"));
    fprintf(ff," %s\n",gettext("Не подтвержден"));
   }
  if(podt == 1)
   {
    printw(" %5.5s\n",gettext("Подтвержден"));
    fprintf(ff," %s\n",gettext("Подтвержден"));
   }
 }

printw("%*s %8.2f %8.2f %8.2f %8.2f\n",iceb_tu_kolbait(36,gettext("Итого")),gettext("Итого"),ibs,iiz,ibsby,iizby);
fprintf(ff,"%*s %8.2f %8.2f %8.2f %8.2f\n",iceb_tu_kolbait(36,gettext("Итого")),gettext("Итого"),ibs,iiz,ibsby,iiz);

fclose(ff);

OSTANOV();

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Список документов"));
iceb_t_ustpeh(imaf,1);

iceb_t_ustpeh(imaf,0);

iceb_t_rabfil(&oth,"");
return(0);
}
