/*$Id: zarsprvmn.c,v 5.32 2014/04/30 06:12:27 sasa Exp $*/
/*23.04.2014	08.04.2002	Белых А.И.	zarsprvmn.c
Получение справки о доходах по датам в счёт которых были 
сделаны начисления
*/
#include        <errno.h>
#include        "buhg.h"

void             rekvizkon(SQL_str,FILE*);


void	zarsprvmn(long tabn,
short mn,short gn,
short mk,short gk)
{
class iceb_tu_str dolg("");
class iceb_tu_str fam("");
char		strsql[1024];
SQL_str		row;
SQLCURSOR curr;

/****************Расчёт*********************/


int		kolstr=0,kolstr1=0;
char		imaf[64];
FILE		*ff;
short           sovm=0; /*0-нет 1- Совместитель*/
class iceb_tu_str inn("");
short		mes,god;
short           mesz=0,godz=0;

sprintf(strsql,"select fio,sovm,inn,dolg,harac,podr from Kartb where tabn=%ld",tabn);
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  sprintf(strsql,"%s %ld !",gettext("Не найден табельный номер"),tabn);
  iceb_t_soob(strsql);
  return;
 }

fam.new_plus(row[0]);
dolg.new_plus(row[3]);
inn.new_plus(row[2]);


clear();
printw("%s\n",gettext("Расчёт справок о зарплате."));
printw("%s\n",fam.ravno());
printw("%s %d.%d%s %s %d.%d%s\n",
gettext("Период с"),
mn,gn,gettext("г."),
gettext("до"),
mk,gk,gettext("г."));
GDITE();


if(gn == gk)
 sprintf(strsql,"select suma,godn,mesn from Zarp where tabn=%ld and \
prn='1' and suma <> 0. and (godn = %d and (mesn >= %d and mesn <= %d)) order by godn,mesn asc",
 tabn,gn,mn,mk);
else
 sprintf(strsql,"select suma,godn,mesn from Zarp where tabn=%ld and \
prn='1' and suma <> 0. and ((godn < %d and godn > %d) or(godn = %d and mesn >= %d) or (godn = %d and mesn <= %d)) order by godn,mesn asc",
tabn,gn,gk,gn,mn,gk,mk);

//printw("\n%s\n",strsql);
//OSTANOV();

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не введены начисления !"));
  return;
 }

sprintf(imaf,"sprdm%ld_%d.lst",tabn,getpid());
if((ff = fopen(imaf,"w")) == NULL)
   {
    error_op_nfil(imaf,errno,"");
    return;
   }

/*Читаем реквизиты организации*/
sprintf(strsql,"select * from Kontragent where kodkon='00'");
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  beep();
  printw("%s\n",gettext("Не найден контрагент с кодом 00 !"));
  OSTANOV();
 }
else
 {
  rekvizkon(row,ff);
 }

  
  fprintf(ff,"\n\
	      %s\n\
%s %d.%d%s %s %d.%d%s\n\
%s %s %s ",
gettext("С п р а в к а."),
gettext("о заработной плате за период с"),
mn,gn,
gettext("г."),
gettext("до"),
mk,gk,
gettext("г."),
gettext("Работник"),
fam.ravno(),
gettext("работает"));

if(sovm == 0)
   fprintf(ff," %s\n",gettext("постоянно."));
else
   fprintf(ff," %s\n",gettext("по совместительству."));
fprintf(ff,"%s %s\n",gettext("Идентификационный код"),inn.ravno());
fprintf(ff,"%s: %s\n",gettext("Должность"),dolg.ravno());


short		ots=10;
fprintf(ff,"\
%*s-------------------------\n\
%*s%s\n\
%*s-------------------------\n",
ots," ",
ots," ","|  Дата |   Начислено   |",
ots," ");


godz=mesz=0;
double suma=0.;
double sumames=0.;
double sumaitog=0.;

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  suma=atof(row[0]);
  god=atoi(row[1]);
  mes=atoi(row[2]);
  
  if(mesz != mes)
   {
    if(mesz != 0)
     {
      printw("%02d.%d %15.2f\n",mesz,godz,sumames);
      fprintf(ff,"%*s|%02d.%d %15.2f|\n",ots," ",mesz,godz,sumames);
     }
    sumames=0.;
    mesz=mes;
    godz=god;
   }  
  sumames+=suma;
  sumaitog+=suma;
  
 }
printw("%02d.%d %15.2f\n",mesz,godz,sumames);
printw("%*s %15.2f\n",iceb_tu_kolbait(7,gettext("Итого")),gettext("Итого"),sumaitog);

fprintf(ff,"%*s|%02d.%d %15.2f|\n",ots," ",mesz,godz,sumames);
memset(strsql,'\0',sizeof(strsql));
preobr(sumaitog,strsql,0);

fprintf(ff,"%*s-------------------------\n\
%*s %*s %15.2f\n\n\
%s\n%s\n",
ots," ",
ots," ",
iceb_tu_kolbait(7,gettext("Итого")),
gettext("Итого"),
sumaitog,
gettext("Общая сумма доходов:"),strsql);

class iceb_t_fioruk_rk rek;
iceb_t_fioruk(1,&rek);

fprintf(ff,"\n\n%*s:_______________________%s\n",iceb_tu_kolbait(20,gettext("Руководитель")),gettext("Руководитель"),rek.famil_inic.ravno());

iceb_t_fioruk(2,&rek);
fprintf(ff,"\n%*s:_______________________%s\n",iceb_tu_kolbait(20,gettext("Главный бухгалтер")),gettext("Главный бухгалтер"),rek.famil_inic.ravno());

podpis(ff);
fclose(ff);
OSTANOV();
class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Справки про зарплату"));
iceb_t_ustpeh(imaf,3);

iceb_t_rabfil(&oth,"");

}
