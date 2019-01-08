/* $Id: rasnas.c,v 5.24 2013/12/31 11:48:11 sasa Exp $ */
/*15.07.2015    09.07.1996      Белых А.И.      rasnas.c
Распечатка истории настройки по заданному инвентарному номеру
и подразделению
*/
#include        <errno.h>
#include        "buhg.h"


void            rasnas(long in)
{
time_t 		tmm;
long		kolstr;
SQL_str         row;
char		strsql[512];
FILE		*ff;
char		imaf[64];
class iceb_tu_str naim("");
SQLCURSOR cur;

//Читаем наименование инвентарного номера
sprintf(strsql,"select naim from Uosin where innom=%ld",in);
if(sql_readkey(&bd,strsql,&row,&cur) > 0)
  naim.new_plus(row[0]); 

sprintf(strsql,"select * from Uosinp where innom=%ld \
order by god,mes asc",in);
/*
printw("\n%s\n",strsql);
refresh();
*/
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return;
 }

sprintf(imaf,"uosnas%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

time(&tmm);
iceb_t_zagolov(gettext("Список изменений по инвентарному номеру."),0,0,0,0,0,0,ff);

fprintf(ff,"%s: %ld %s\n",
gettext("Инвентарный номер"),in,naim.ravno());

fprintf(ff,"\
--------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
  Дата | Счёт  |Шифр  |Шифр анал.|  Группа |Поправочный коэффици.|Состояние| Номерной |Кто зап.\n\
       | учёта |затрат|  учёта   |Нал.|Бух.|Налоговый |Бухгалтер.|объекта  |   знак   |\n"));
/*
1234567 1234567 123456 1234567890 1234 1234 1234567890 1234567890 123456789 1234567890
*/

fprintf(ff,"\
--------------------------------------------------------------------------------------------------\n");
char sostob[64];

while(cur.read_cursor(&row) != 0)
 {
  if(row[8][0] == '0')
   strcpy(sostob,gettext("Бух+Нал+"));
  if(row[8][0] == '1')
   strcpy(sostob,gettext("Бух-Нал-"));
  if(row[8][0] == '2')
   strcpy(sostob,gettext("Бух+Нал-"));
  if(row[8][0] == '3')
   strcpy(sostob,gettext("Бух-Нал+"));
   
  fprintf(ff,"%2s.%s %-*s %-*s %-*s %-*s %-*s %10.10g %10.10g %-*s %-*s %s %s\n",
  row[1],
  row[2],
  iceb_tu_kolbait(7,row[3]),row[3],
  iceb_tu_kolbait(6,row[4]),row[4],
  iceb_tu_kolbait(10,row[5]),row[5],
  iceb_tu_kolbait(4,row[6]),row[6],
  iceb_tu_kolbait(4,row[12]),row[12],
  atof(row[7]),
  atof(row[13]),
  iceb_tu_kolbait(9,sostob),sostob,
  iceb_tu_kolbait(10,row[9]),row[9],
  iceb_t_kszap(row[10]),
  iceb_tu_vremzap(row[11]));

 }
podpis(ff);
fclose(ff);
class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Список изменений по инвентарному номеру"));

iceb_t_ustpeh(imaf,0);

iceb_t_rabfil(&oth,"");

}




