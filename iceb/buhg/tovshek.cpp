/*$Id: tovshek.c,v 5.15 2013/08/13 05:49:56 sasa Exp $*/
/*11.06.2013	18.04.2003	Белых А.И.	tovshek.c
Печать товарного чека
*/
#include	<errno.h>
#include	"buhg.h"


void	tovshek(SQLCURSOR *cur)
{
FILE            *ff;
char           	imaf[64];
SQL_str         row,row1;
char		strsql[1024];
class iceb_tu_str nmo("");  /*Наименование организации*/
class iceb_tu_str gor(""); /*Адрес*/
class iceb_tu_str pkod(""); /*Почтовый код*/
class iceb_tu_str nmb("");  /*Наименование банка*/
class iceb_tu_str mfo("");  /*МФО*/
class iceb_tu_str nsh("");  /*Номер счета*/
class iceb_tu_str inn("");  /*Индивидуальный налоговый номер*/
class iceb_tu_str grb("");  /*Город банка*/
class iceb_tu_str npnds(""); /*Номер плательщика НДС*/
class iceb_tu_str tel("");    /*Номер телефона*/
SQLCURSOR curr;
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);


sprintf(strsql,"select * from Kontragent where kodkon='00'");
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  iceb_t_soob(gettext("Не найден код организации 00 !"));
 }
else
 {
  nmo.new_plus(row[1]);
  gor.new_plus(row[3]);
  pkod.new_plus(row[5]);
  nmb.new_plus(row[2]);
  mfo.new_plus(row[6]);
  nsh.new_plus(row[7]);
  inn.new_plus(row[8]);
  grb.new_plus(row[4]);
  npnds.new_plus(row[9]);
  tel.new_plus(row[10]);
 } 



sprintf(imaf,"tsh%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
   {
    error_op_nfil(imaf,errno,"");
    return;
   }


fprintf(ff,"%s\n",nmo.ravno());

fprintf(ff,"%s %s\n",gettext("Адрес"),gor.ravno());
fprintf(ff,"%s %s\n",gettext("Код ОКПО"),pkod.ravno());

fprintf(ff,"%s %s %s %s\n",gettext("Р/С"),nsh.ravno(),
gettext("МФО"),mfo.ravno());

fprintf(ff,"%s %s %s %s\n",gettext("в"),nmb.ravno(),
gettext("г."),grb.ravno());

fprintf(ff,"%s %s\n",gettext("Налоговый номер"),inn.ravno());
fprintf(ff,"%s %s\n",gettext("Номер сви-ства плательщика НДС"),npnds.ravno());
fprintf(ff,"%s %s\n",gettext("Телефон"),tel.ravno());

fprintf(ff,"%20s%s N\n"," ",gettext("Товарный чек"));
fprintf(ff,"%20s%02d.%02d.%d%s\n"," ",dt,mt,gt,gettext("г."));

fprintf(ff,"\
-------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Н/п|       Наименование товара              |Ед.из.|Колич.|  Цена  |  Сумма   |\n"));
fprintf(ff,"\
-------------------------------------------------------------------------------\n");

cur->poz_cursor(0);
int nomer=0;
double kolih=0.;
double cena=0.;
double suma=0.;
double sumai=0.;
class iceb_tu_str naim("");

while(cur->read_cursor(&row) != 0)
 {
  /*Читаем наименование материалла*/
   sprintf(strsql,"select naimat from Material where kodm=%s",
   row[4]);
   if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
     naim.new_plus(row1[0]);
   else
     naim.new_plus("");     
  kolih=atof(row[6]);
  cena=atof(row[7]);
  cena=okrug(cena,0.01);
  suma=kolih*cena;
  suma=okrug(suma,0.01);
  sumai+=suma;  

  fprintf(ff,"%3d %-*.*s %-*s %6.6g %8.2f %10.2f\n",
  ++nomer,
  iceb_tu_kolbait(40,naim.ravno()),iceb_tu_kolbait(40,naim.ravno()),naim.ravno(),
  iceb_tu_kolbait(6,row[5]),row[5],
  kolih,cena,suma);
  
 }
fprintf(ff,"\
-------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f\n",iceb_tu_kolbait(67,gettext("Итого")),gettext("Итого"),sumai);

memset(strsql,'\0',sizeof(strsql));
preobr(sumai,strsql,0);
fprintf(ff,"%s:%s\n",gettext("Сумма прописью"),strsql);

fprintf(ff,"\n        %s_______________\n",gettext("Продавец"));

fclose(ff);
class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Товарный чек"));

iceb_t_ustpeh(imaf,0);

iceb_t_rabfil(&oth,"");

}
