/* $Id: zarraspr.c,v 5.18 2013/08/13 05:50:02 sasa Exp $ */
/*04.11.2009	24.09.1998	Белых А.И.	zarraspr.c
Распечатка выполненых проводок на экран
*/
#include        <errno.h>
#include        "buhg.h"


void zarraspr(int tbn,short mm,short gg,const char *fio)
{
struct  tm      *bf;
class iceb_tu_str nn("");
double		suma;
long		kolstr;
SQL_str         row;
char		strsql[512];
short		dp,mp,gp;
double		deb,kre;
FILE		*ff;
char		imaf[64];
time_t		vrem;

clear();
//sprintf(nn,"%d-%d",mm,tbn);
nn.new_plus(mm);
nn.plus("-");
nn.plus(tbn);

printw("%s %d %s %s %d.%d%s\n",
gettext("Проводки для"),tbn,fio,
gettext("за"),mm,gg,
gettext("г."));
refresh();

sprintf(strsql,"select datp,sh,shk,kodkon,deb,kre,komen,ktoi,vrem from Prov \
where kto='%s' and nomd='%s' and datd>='%d-%02d-01' and \
datd <= '%d-%02d-31' order by datp,sh asc",
ICEB_MP_ZARP,nn.ravno(),gg,mm,gg,mm);
SQLCURSOR cur;
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
sprintf(imaf,"provzp%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
   {
    error_op_nfil(imaf,errno,"");
    return;
   }

fprintf(ff,"%s %d %s %s %d.%d%s\n",
gettext("Проводки для"),tbn,fio,
gettext("за"),mm,gg,
gettext("г."));

suma=0.;
while(cur.read_cursor(&row) != 0)
 {
  deb=atof(row[4]);
  kre=atof(row[5]);
  
  if(kre != 0.)
    continue;
  rsdat(&dp,&mp,&gp,row[0],2);
  printw("%02d.%02d.%d %-*s %-*s %-*s %8.2f %s\n",
  dp,mp,gp,
  iceb_tu_kolbait(5,row[1]),row[1],
  iceb_tu_kolbait(5,row[2]),row[2],
  iceb_tu_kolbait(5,row[3]),row[3],
  deb,row[6]);

  vrem=atol(row[8]);
  bf=localtime(&vrem);

  fprintf(ff,"%02d.%02d.%d %-*s %-*s %-*s %8.2f %s %s %d.%d.%d %d:%d:%d\n",
  dp,mp,gp,
  iceb_tu_kolbait(5,row[1]),row[1],
  iceb_tu_kolbait(5,row[2]),row[2],
  iceb_tu_kolbait(5,row[3]),row[3],
  deb,row[6],iceb_t_kszap(row[7]),
  bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
  bf->tm_hour,bf->tm_min,bf->tm_sec);

  suma+=deb;

 }
printw("%27s: %8.2f\n",gettext("Итого"),suma);
fprintf(ff,"%27s: %8.2f\n",gettext("Итого"),suma);
fclose(ff);

printw("%s %s.\n",
gettext("Распечатка выгружена в файл"),imaf);

helstr(LINES-1,0,"Enter",gettext("работа с файлом"),
"FK2",gettext("пров."),
"F10",gettext("выход"),NULL);

ppp:;

dp=getch();
    class spis_oth oth;
switch(dp)
 {
  case FK10:
  case ESC:
    unlink(imaf);
    break;

  case ENTER:
    sprintf(strsql,"%s %d %s %d.%d%s\n",
    gettext("Проводки для"),tbn,
    gettext("за"),mm,gg,
    gettext("г."));

    oth.spis_imaf.plus(imaf);
    oth.spis_naim.plus(strsql);
    iceb_t_ustpeh(imaf,3);
    iceb_t_rabfil(&oth,"");
    break;  

  case FK2:
    dirprzar(tbn,mm,gg,fio);
    break;
    
  default:
    goto ppp;
 }
}
