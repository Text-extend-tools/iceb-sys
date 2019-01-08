/* $Id: poifl.c,v 5.25 2014/02/28 05:13:47 sasa Exp $ */
/*30.11.2016    06.07.1995      Белых А.И.      poifl.c
Поиск в файле заданного табельного номера
*/
#include        <errno.h>
#include        "buhg.h"

int poifil(int tbn,const char *imaf,const char *zagl,FILE *ff);
void poifl_zarrud(int tn,FILE *ff);
void poifl_zarlgot(int tn,FILE *ff);
void poifl_pidp(int metka_s,int tn,FILE *ff);
void poifl_do(int tabnom,FILE *ff);

void poifl(int tbn)
{
char		imafo[64];
FILE		*ff;

GDITE();
sprintf(imafo,"znast%d.lst",getpid());
if((ff = fopen(imafo,"w")) == NULL)
 {
  error_op_nfil(imafo,errno,"");
  return;
 }
startfil(ff);
fprintf(ff,"%s %d %s\n\
----------------------------------------------------\n",
gettext("Просмотр настройки по табельному номеру"),tbn,iceb_t_get_fiotbn(tbn));

poifl_do(tbn,ff);

poifil(tbn,"zaralim.alx",gettext("Имеет алименты"),ff);

poifl_zarrud(tbn,ff);

poifl_pidp(2,tbn,ff);

poifl_pidp(3,tbn,ff);

poifil(tbn,"zarproc.alx",gettext("Записан у файл автоматизированного расчёта начислений"),ff);

poifl_pidp(1,tbn,ff);

poifl_zarlgot(tbn,ff);

poifil(tbn,"zarsppo.alx",gettext("Записан у файл персональных процентов отчисления в пенсионный фонд"),ff);

fclose(ff);

prosf(imafo);
unlink(imafo);
}

/****************************************/
/*Поиск в файле заданного табельного номера*/
/****************************************/
int poifil(int tbn,const char *imaf,const char *zagl,FILE *ff)
{
short           voz=0;
char		strsql[512];
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;


sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(0);
 }

if(kolstr == 0)
 {
  return(0);
 }


while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  if( tbn == atoi(row_alx[0]))
   {
    voz++;
    if(voz == 1)
     fprintf(ff,"%s\n",zagl);
    fprintf(ff,"%s",row_alx[0]);     
   }
 }
return(voz);
}
/******************************/
/*поиск в справочнике регулярных удержаний*/
/*****************************************/
void poifl_zarrud(int tn,FILE *ff)
{
char strsql[512];
SQL_str row,row1;
class SQLCURSOR cur,cur1;
sprintf(strsql,"select * from Zarrud where tn=%d",tn);
if(readkey(strsql,&row,&cur) != 1)
 return;

fprintf(ff,"\n%s\n",gettext("Регулярное удержание"));

fprintf(ff,"\
----------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Таб.номер|     Фамилия        |Код уд.| Значение |Метка| Дата  |Коментарий\n"));
/**********
123456789 12345678901234567890 1234567 1234567890 12345
***********/
fprintf(ff,"\
----------------------------------------------------------------------------------------\n");

class iceb_tu_str metka("");
class iceb_tu_str fio("");

sprintf(strsql,"select fio from Kartb where tabn=%s",row[0]);
if(readkey(strsql,&row1,&cur1) == 1)
 fio.new_plus(row1[0]);

if(atoi(row[3]) == 1)
 metka.new_plus(gettext("Процент"));

fprintf(ff,"%9s %*.*s %7s %10s %*.*s %s %s\n",
row[0],
iceb_tu_kolbait(20,fio.ravno()),
iceb_tu_kolbait(20,fio.ravno()),
fio.ravno(),
row[1],
row[2],
iceb_tu_kolbait(5,metka.ravno()),  
iceb_tu_kolbait(5,metka.ravno()),  
metka.ravno(),
iceb_tu_datzap_mg(row[4]),
row[5]);  





}
/**********************************/
/*поиск в справочнике льгот*/
/********************************/
void poifl_zarlgot(int tn,FILE *ff)
{
char strsql[512];
SQL_str row,row1;
class SQLCURSOR cur,cur1;
int kolstr=0; 
sprintf(strsql,"select * from Zarlgot where tn=%d order by dndl asc",tn);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
if(kolstr == 0)
 return;
fprintf(ff,"\n%s\n",gettext("Льготы для подоходного налога"));
fprintf(ff,"\
----------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Таб.номер|     Фамилия        |Дата н.дей|Процент|Кол. льгот|Код л|Коментарий\n"));
/**********
123456789 12345678901234567890 1234567890 1234567 1234567890 12345
***********/
fprintf(ff,"\
----------------------------------------------------------------------------------------\n");
class iceb_tu_str fio("");

while(cur.read_cursor(&row) != 0)
 {
  fio.new_plus("");
  sprintf(strsql,"select fio from Kartb where tabn=%s",row[0]);
  if(readkey(strsql,&row1,&cur1) == 1)
   fio.new_plus(row1[0]);

  fprintf(ff,"%9s %*.*s %10s %7s %10s %5s %s\n",
  row[0],
  iceb_tu_kolbait(20,fio.ravno()),
  iceb_tu_kolbait(20,fio.ravno()),
  fio.ravno(),
  iceb_tu_datzap_mg(row[1]),
  row[2],
  row[3],
  row[4],
  row[5]);  

 }
fprintf(ff,"\
----------------------------------------------------------------------------------------\n");


}
/**********************************/
/*поиск в справочнике пенсионеров инвалидов работающих по договорам подряда*/
/********************************/
void poifl_pidp(int metka_s,int tn,FILE *ff)
{
char strsql[512];
SQL_str row,row1;
class SQLCURSOR cur,cur1;
int kolstr=0; 
sprintf(strsql,"select * from Zargr where kg=%d and tn=%d order by dnd asc",metka_s,tn);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
if(kolstr == 0)
 return;

if(metka_s == 1)
  fprintf(ff,"\n%s\n",gettext("Cправочник пенсионеров"));
if(metka_s == 2)
  fprintf(ff,"\n%s\n",gettext("Cправочник инвалидов"));
if(metka_s == 3)
  fprintf(ff,"\n%s\n",gettext("Cправочник работающих по договорам подряда"));

fprintf(ff,"\
----------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Таб.номер|          Фамилия             |Дата начал|Дата конца|Коментарий\n"));
/**********
123456789 12345678901234567890 1234567890 1234567890 12345
***********/
fprintf(ff,"\
----------------------------------------------------------------------------------------\n");

class iceb_tu_str fio("");
class iceb_tu_str dnd("");
class iceb_tu_str dkd("");

while(cur.read_cursor(&row) != 0)
 {
  fio.new_plus("");
  sprintf(strsql,"select fio from Kartb where tabn=%s",row[1]);
  if(readkey(strsql,&row1,&cur1) == 1)
   fio.new_plus(row1[0]);

  dnd.new_plus(iceb_tu_datzap_mg(row[2]));
  dkd.new_plus(iceb_tu_datzap_mg(row[3]));
  

  fprintf(ff,"%9s %*.*s %10s %10s %s\n",
  row[1],
  iceb_tu_kolbait(30,fio.ravno()),
  iceb_tu_kolbait(30,fio.ravno()),
  fio.ravno(),
  dnd.ravno(),
  dkd.ravno(),
  row[4]);

 }
fprintf(ff,"\
----------------------------------------------------------------------------------------\n");

}
/****************************/
/*поиск в таблице должностных окладов*/
/**************************************/
void poifl_do(int tabnom,FILE *ff)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
int kolstr=0; 
sprintf(strsql,"select * from Zarsdo where tn=%d order by dt asc",tabnom);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
if(kolstr == 0)
 return;

fprintf(ff,"%s\n",gettext("Справочник должностных окладов"));

fprintf(ff,"\
---------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
 Т.н. |Дата   |К.н.|  Сумма   |Способ расчёта|Кол.ч.|Коэф.| Счёт |Коментарий\n"));

/*
123456 00.0000 1234 1234567890 12345678901234 123456 12345 123456
*/
fprintf(ff,"\
---------------------------------------------------------------------------------\n");

int metka_sp=0;
class iceb_tu_str naim_sp("");
while(cur.read_cursor(&row) != 0)
 {
  metka_sp=atoi(row[4]);

  switch(metka_sp)
   {
    case 0:
     naim_sp.new_plus(gettext("мес.дни"));
     break;     
    case 1:
     naim_sp.new_plus(gettext("мес.часы"));
     break;     
    case 2:
     naim_sp.new_plus(gettext("мес."));
     break;     
    case 3:
     naim_sp.new_plus(gettext("часы"));
     break;     
    case 4:
     naim_sp.new_plus(gettext("дни"));
     break;     
    case 5:
     naim_sp.new_plus(gettext("тариф дни"));
     break;     
    case 6:
     naim_sp.new_plus(gettext("тариф часы"));
     break;     
   };
  


  fprintf(ff,"%6s %s %4s %10s %-*.*s %6s %5s %6s %-*s %s %s\n",
  row[0],
  iceb_tu_datzap_mg(row[1]),
  row[2],
  row[3],
  iceb_tu_kolbait(14,naim_sp.ravno()),
  iceb_tu_kolbait(14,naim_sp.ravno()),
  naim_sp.ravno(),
  row[5],
  row[6],
  row[7],
  iceb_tu_kolbait(20,row[8]),
  row[8],
  iceb_tu_vremzap(row[10]),
  iceb_t_kszap(row[9]));
 }

fprintf(ff,"\
---------------------------------------------------------------------------------\n");
fprintf(ff,"\n");

}
