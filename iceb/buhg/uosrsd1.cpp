/* $Id: uosrsd1.c,v 5.25 2013/11/05 10:50:49 sasa Exp $ */
/*13.06.2013	25.11.1999	Белых А.И.	uosrsd1.c
Распечатка списка документов относящихзся к конкретному инвентарному 
номеру.
*/
#include        <errno.h>
#include        "buhg.h"


void            uosrsd1(long in)
{
FILE		*ff;
char		imaf[64];
long		kolstr;
SQL_str         row,row1;
SQLCURSOR curr;
char		strsql[512];
short		d1,m1,g1;
short		tp;
class iceb_tu_str kodop("");
time_t 		tmm;
class iceb_tu_str naim("");

clear(); /*Очистить экран и закрасить фоновым цветом*/
printw("%s\n%s %ld\n",gettext("Просмотр документов"),gettext("Инвентарный номер"),in);
refresh();

//Читаем наименование инвентарного номера
sprintf(strsql,"select naim from Uosin where innom=%ld",in);
if(sql_readkey(&bd,strsql,&row,&curr) > 0)
  naim.new_plus(row[0]); 

sprintf(strsql,"select * from Uosdok1 where innom=%ld \
order by datd asc, tipz desc",in);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
sprintf(imaf,"uosd%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

time(&tmm);
iceb_t_zagolov(gettext("Список документов по инвентарному номеру"),0,0,0,0,0,0,ff);

fprintf(ff,"%s: %ld %s\n",gettext("Инвентарный номер"),
in,naim.ravno());

fprintf(ff,"\
-----------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
 |  Дата    |  Номер   |Опер|Подра|Матер.|  Налоговый учёт     |  Бухгалтерский учёт |Коф.инд.|\n\
 |          | документа|ация|здел.|ответ.|Бал.стоим.| Износ    |Бал.стоим.| Износ    |        |\n"));
/*           1234567890 1234 12345 123456*/
fprintf(ff,"\
-----------------------------------------------------------------------------------------------\n");
double i_bs_nu=0.;
double i_iz_nu=0.;
double i_bs_bu=0.;
double i_iz_bu=0.;
double bs_nu=0.;
double iz_nu=0.;
double bs_bu=0.;
double iz_bu=0.;

while(cur.read_cursor(&row) != 0)
 {
  rsdat(&d1,&m1,&g1,row[0],2);
  tp=atoi(row[1]);

  sprintf(strsql,"select kodop from Uosdok where datd='%s' and \
nomd='%s'",row[0],row[4]);
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    beep();
    printw("%s %s %s !\n",gettext("Не найден документ"),row[4],row[1]);
    OSTANOV();
    kodop.new_plus("");
   }
  else
   kodop.new_plus(row1[0]);
  
  bs_nu=atof(row[8]);
  iz_nu=atof(row[9]);
  bs_bu=atof(row[14]);
  iz_bu=atof(row[15]);
     
  if(tp == 1)
   {
    fprintf(ff,"+");
    i_bs_nu+=bs_nu;
    i_bs_bu+=bs_bu;
    i_iz_nu+=iz_nu;
    i_iz_bu+=iz_bu;  
   }
  if(tp == 2)
   {
    fprintf(ff,"-");
    i_bs_nu-=bs_nu;
    i_bs_bu-=bs_bu;
    i_iz_nu-=iz_nu;
    i_iz_bu-=iz_bu;  
   }
  fprintf(ff," %02d.%02d.%d %-*s %-*s %-*s %-*s %10.2f %10.2f %10.2f %10.2f %8.2f",
  d1,m1,g1,
  iceb_tu_kolbait(10,row[4]),row[4],
  iceb_tu_kolbait(4,kodop.ravno()),kodop.ravno(),
  iceb_tu_kolbait(5,row[5]),row[5],
  iceb_tu_kolbait(6,row[6]),row[6],
  atof(row[8]),atof(row[9]),
  atof(row[14]),atof(row[15]),atof(row[10]));

  if(atoi(row[2]) == 0)
   {
    fprintf(ff," %*.*s",
    iceb_tu_kolbait(10,gettext("Не подтвержден")),
    iceb_tu_kolbait(10,gettext("Не подтвержден")),
    gettext("Не подтвержден"));
   }

  if(atoi(row[2]) == 1)
   {
    fprintf(ff," %*.*s",
    iceb_tu_kolbait(10,gettext("Подтвержден")),
    iceb_tu_kolbait(10,gettext("Подтвержден")),
    gettext("Подтвержден"));
   }
  fprintf(ff," %s %s\n",iceb_tu_vremzap(row[13]),iceb_t_kszap(row[12]));
    
 }    

fprintf(ff,"\
-----------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s %10.2f %10.2f %10.2f %10.2f\n",
iceb_tu_kolbait(41,gettext("Итого")),gettext("Итого"),i_bs_nu,i_iz_nu,i_bs_bu,i_iz_bu);


podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Список документов по инвентарному номеру"));

iceb_t_ustpeh(imaf,0);

iceb_t_rabfil(&oth,"");

}





