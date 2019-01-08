/* $Id: dvmat.c,v 5.24 2013/05/17 14:56:01 sasa Exp $ */
/*04.09.2015	13.3.1999	Белых А.И.	dvmat.c
Движение по документам по материалу
*/
#include <math.h>
#include        <errno.h>
#include        "buhg.h"

void dvmat(short dn,short mn,short gn,short dk,short mk,short gk,int skl,int kodm)
{
FILE		*ff1;
char		imaf[64];
double		kolih,cena,itog=0.,itog2=0.;
short		d,m,g;
class iceb_tu_str nn("");
short		tipz;
class iceb_tu_str kop("");
class iceb_tu_str kor("");
int		skll,nk;
class iceb_tu_str naim("");
long		kolstr,kolstr1;
SQL_str         row,row1;
char		strsql[512];
double		kol2=0.;
SQLCURSOR cur;
SQLCURSOR cur1;
 
clear();
GDITE();

  
printw("%s %d.%d.%d%s => %d.%d.%d%s %s %d\n",
gettext("Дата"),
dn,mn,gn,gettext("г."),
dk,mk,gk,gettext("г."),
gettext("Код материалла"),
kodm);

if(skl != 0)
 printw(gettext("Склад %d\n"),skl);
else
 printw(gettext("По всем складам.\n"));
  
/*Читаем наименование материалла*/
sprintf(strsql,"select naimat from Material where kodm=%d",
kodm);
if(sql_readkey(&bd,strsql,&row1,&cur) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден код материалла"),kodm);
  iceb_t_soob(strsql);
  return;
 }

naim.new_plus(row1[0]);

printw(gettext("Наименование %s\n"),naim.ravno());
GDITE();
if(skl != 0)
 sprintf(strsql,"select datd,nomd,sklad,nomkar,kolih,cena,tipz from \
Dokummat1 where datd >= '%04d-%d-%d' and datd <= '%04d-%02d-%02d' and \
kodm=%d and sklad=%d order by datd asc",
 gn,mn,dn,gk,mk,dk,kodm,skl);
else
 sprintf(strsql,"select datd,nomd,sklad,nomkar,kolih,cena,tipz \
from Dokummat1 where datd >= '%04d-%d-%d' and datd <= '%04d-%02d-%02d' \
and kodm=%d order by datd asc",gn,mn,dn,gk,mk,dk,kodm);
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


sprintf(imaf,"dvi%d.lst",getpid());
if((ff1 = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

fprintf(ff1,gettext("%s\nДата %d.%d.%d Код материалла %d\n\
Наименование материалла %s\n"),
iceb_t_get_pnk("00",0),dk,mk,gk,kodm,naim.ravno());

if(skl != 0)
 fprintf(ff1,gettext("Склад %d\n"),skl);
else
 fprintf(ff1,gettext("По всем складам\n"));

fprintf(ff1,"\
------------------------------------------------------------------------------\n");

fprintf(ff1,gettext(" Дата     |Номер док.|Ном.к.|Количес.|Подтвер.| Цена   |Скл.|Опер.|Контрагент|\n"));

fprintf(ff1,"\
------------------------------------------------------------------------------\n");


itog=0.;
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  kolstr1++;
  strzag(LINES-1,0,kolstr,kolstr1);
    
  rsdat(&d,&m,&g,row[0],2);
  /*Пpопускаем стаpтовые остатки если это остатки не года начала поиска*/
  if(SRAV(row[1],"000",0) == 0 && g != gn)
   continue;
  nn.new_plus(row[1]);
  skll=atoi(row[2]);
  nk=atoi(row[3]);
  kolih=atof(row[4]);
  cena=atof(row[5]);
  tipz=atoi(row[6]);
  
  
  /*Читаем шапку накладной*/
  sprintf(strsql,"select kontr,kodop from Dokummat \
where datd='%d-%02d-%02d' and sklad=%d and nomd='%s'",
  g,m,d,skll,nn.ravno());

  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    beep();
    printw(gettext("Не найден документ %s %d.%d.%d %s\n"),
    row[1],d,m,g,row[2]);
    OSTANOV();
    continue;    
   }
  kor.new_plus(row1[0]);
  kop.new_plus(row1[1]);

  if(tipz == 2)
    kolih*=(-1);
  itog+=kolih;
  
  kol2=0.;
  if(nk != 0)
    kol2=readkolk(skll,nk,d,m,g,nn.ravno());  
  if(tipz == 2)
    kol2*=(-1);
    
  itog2+=kol2;
  
  printw("%02d.%02d.%d %-*s %-6d %8.8g %8.8g %8.8g %-4d %-*s %s\n",
  d,m,g,
  iceb_tu_kolbait(10,nn.ravno()),nn.ravno(),
  nk,kolih,kol2,cena,skll,
  iceb_tu_kolbait(5,kop.ravno()),kop.ravno(),
  kor.ravno());
    
  fprintf(ff1,"%02d.%02d.%d %-*s %-6d %8.8g %8.8g %8.8g %-4d %-*s %-*s\n",
  d,m,g,
  iceb_tu_kolbait(10,nn.ravno()),nn.ravno(),
  nk,kolih,kol2,cena,skll,
  iceb_tu_kolbait(5,kop.ravno()),kop.ravno(),
  iceb_tu_kolbait(10,kor.ravno()),kor.ravno());

 }
if(fabs(itog) < 0.00000001)
 itog=0.;
if(fabs(itog2) < 0.00000001)
 itog2=0.;
 
fprintf(ff1,"\
------------------------------------------------------------------------------\n");

fprintf(ff1,"%*s %8.8g %8.8g\n",iceb_tu_kolbait(28,gettext("Остаток :")),gettext("Остаток :"),itog,itog2);
printw("%*s %8.8g %8.8g\n",iceb_tu_kolbait(28,gettext("Остаток :")),gettext("Остаток :"),itog,itog2);

podpis(ff1);

fclose(ff1);

OSTANOV();

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Движение по материалу"));

iceb_t_ustpeh(oth.spis_imaf.ravno(0),3);

iceb_t_rabfil(&oth,"");

}
