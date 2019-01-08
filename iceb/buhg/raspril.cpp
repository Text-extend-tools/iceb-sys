/*$Id: raspril.c,v 5.14 2013/08/13 05:49:53 sasa Exp $*/
/*15.07.2015	02.02.2007	Белых А.И.	raspril.c
Распечатка приложения к накладной 
*/
#include <errno.h>
#include "buhg.h"


void raspril(short dg,short mg,short gg, //дата документа
int skl,  //Склад
const char *nomdok) //Номер документа
{
class iceb_tu_str bros("");
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
SQL_str row1;
class SQLCURSOR cur1;
struct tm *bf;
time_t vrem=time(NULL);
bf=localtime(&vrem);



/*Читаем шапку документа*/
sprintf(strsql,"select * from Dokummat \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",
gg,mg,dg,skl,nomdok);

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  VVOD SOOB(1);

  sprintf(strsql,"%s-%s",__FUNCTION__,gettext("Не найден документ !")),
  SOOB.VVOD_SPISOK_add_MD(strsql);

  sprintf(strsql,gettext("Дата=%d.%d.%d Документ=%s Склад=%d"),
  dg,mg,gg,nomdok,skl);
  SOOB.VVOD_SPISOK_add_MD(strsql);

  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return;
 }

class iceb_tu_str kontr("");
class iceb_tu_str kodop("");

kontr.new_plus(row[3]);
int tipz=atoi(row[0]);
kodop.new_plus(row[6]);
class iceb_tu_str kodkontr00("");


kodkontr00.new_plus("00");
if(tipz == 2)
 {
  if(SRAV("00",kodkontr00.ravno(),0) == 0)
    kodkontr00.new_plus(iceb_t_getk00(0,kodop.ravno()));
 }



int kolstr=0;

sprintf(strsql,"select * from Dokummat1 where datd='%04d-%02d-%02d' and \
sklad=%d and nomd='%s' order by kodm asc",
gg,mg,dg,skl,nomdok);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

FILE *ffdok;
char imafdok[64];
sprintf(imafdok,"pril%d.lst",getpid());
if((ffdok = fopen(imafdok,"w")) == NULL)
 {
  error_op_nfil(imafdok,errno,"");
  return;
 }
int kolstrok=0;

rashap(tipz,&kolstrok,kodkontr00.ravno(),kontr.ravno(),10,0,ffdok); //печать реквизитов контагентов

class iceb_tu_str naim("");
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);

fprintf(ffdok,"\n\
                  %s %02d.%02d.%d%s %s %02d:%02d:%02d\n\
                  %s N%s %s %02d.%02d.%d%s\n\n",
gettext("ПРИЛОЖЕНИЕ от"),
dt,mt,gt,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min,bf->tm_sec,
gettext("К документу"),
nomdok,
gettext("от"),
dg,mg,gg,
gettext("г."));

//fprintf(ffdok,"\x1B\x4D"); /*12-знаков*/
fprintf(ffdok,"\
----------------------------------------------------------------------------------\n");

fprintf(ffdok,gettext("\
 N | Код |     Наименование             |Ед.изм.| Выписано | Осталось | Взяли    |\n"));

fprintf(ffdok,"\
----------------------------------------------------------------------------------\n");
double kolih_vip=0.; //Выписанное количество
double kolih_pod=0.; //подтверждённое количество
double kolih_ost=0.; //количество которе осталось забрать
int nom_str=0;
double itog1=0.,itog2=0.;
while(cur.read_cursor(&row) != 0)
 {
  /*Читаем наименование материалла*/
  sprintf(strsql,"select naimat,kodgr from Material where kodm=%s",row[4]);

  if(readkey(strsql,&row1,&cur1) != 1)
   {
    VVOD SOOB(1);

    sprintf(strsql,"%s %s !",gettext("Не найден код материалла"),row[4]);
    SOOB.VVOD_SPISOK_add_MD(strsql);

    soobshw(&SOOB,stdscr,-1,-1,0,1);
    naim.new_plus("");
   }
  else
   {
    naim.new_plus(row1[0]);
    if(row[16][0] != '\0')
     {
      naim.plus(" ",row[16]);
     }
   }
  kolih_vip=atof(row[5]);
  kolih_pod=readkolk(skl,atoi(row[3]),dg,mg,gg,nomdok);  
  kolih_ost=kolih_vip-kolih_pod;
  
  fprintf(ffdok,"%3d %5s %-*.*s %-*.*s %10.10g %10.10g\n",
  ++nom_str,
  row[4],
  iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
  iceb_tu_kolbait(7,row[7]),iceb_tu_kolbait(7,row[7]),row[7],
  kolih_vip,kolih_ost);

  if(iceb_tu_strlen(naim.ravno()) > 30)
   fprintf(ffdok,"%3s %5s %s\n","","",iceb_tu_adrsimv(30,naim.ravno()));

  itog1+=kolih_vip;
  itog2+=kolih_ost;  
 }

fprintf(ffdok,"\
----------------------------------------------------------------------------------\n");
fprintf(ffdok,"%*s:%10.10g %10.10g\n",iceb_tu_kolbait(48,gettext("Итого")),gettext("Итого"),itog1,itog2);

fprintf(ffdok,"\n\%s%s\n",gettext("\
Руководитель__________________        Главный  бухгалтер_______________\n\n"),
gettext("\
Отпустил______________________        Принял__________________________"));

podpis(ffdok);
fclose(ffdok);

class spis_oth oth;
oth.spis_imaf.plus(imafdok);
oth.spis_naim.plus(gettext("Приложение к документу"));
iceb_t_ustpeh(imafdok,1);

iceb_t_rabfil(&oth,"");


}
