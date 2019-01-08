/* $Id: rasosdk.c,v 5.32 2013/09/26 09:43:39 sasa Exp $ */
/*15.07.2015	22.02.1999	Белих А.И.	rasosdk.c
Получение распечатки движения по конкретному материалу
*/
#include <math.h>
#include        <errno.h>
#include	"buhg.h"


void		rasosdk()
{
struct  tm      *bf;
time_t          tmm;
int		gr,kd;
short		dos,mos,gos;
short		d,m,g;
int		par;
class iceb_tu_str naiskl("");
class iceb_tu_str naimat("");
char		imaf[64];
FILE		*ff;
double		ost;
long		kolstr,kolp;
SQL_str         row;
char		strsql[512];
static class iceb_tu_str dat1("");
static class iceb_tu_str skladi("");
static class iceb_tu_str kodmat("");
char		pv[8];
double		bb;
int		skl=0;

if(dat1.ravno()[0] == '\0')
 {
  short dt,mt,gt;
  poltekdat(&dt,&mt,&gt);
  sprintf(strsql,"%d.%d.%d",dt,mt,gt);
  dat1.new_plus(strsql);
 }
VVOD VV(0);
VV.VVOD_SPISOK_add_data(kodmat.ravno(),10);
VV.VVOD_SPISOK_add_data(skladi.ravno(),128);
VV.VVOD_SPISOK_add_data(dat1.ravno(),11);

VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка карточек с остатками"));

VV.VVOD_SPISOK_add_MD(gettext("Код материаллa..."));
VV.VVOD_SPISOK_add_MD(gettext("Склад..(,,)......"));
VV.VVOD_SPISOK_add_MD(gettext("Дата остатка....."));


naz:;

clear();

helstr(LINES-1,0,"F2/+",gettext("расчёт"),
"F3",gettext("склады"),
"F4",gettext("материалы"),
"F10",gettext("выход"),NULL);

par=VV.vvod(0,1,1,-1,-1);

kodmat.new_plus(VV.data_ravno(0));
skladi.new_plus(VV.data_ravno(1));
dat1.new_plus(VV.VVOD_SPISOK_return_data(2));
class iceb_tu_str kods("");
class iceb_tu_str naims("");
switch (par)
 {
  case ESC:
  case FK10:
    return;

  case PLU:
  case FK2:
   if(dat1.ravno()[0] == '\0')
    {
     iceb_t_soob(gettext("Не введена дата остатка !"));
     goto naz;
    }
   if(rsdat(&dos,&mos,&gos,dat1.ravno(),0) != 0)
    {
     iceb_t_soob(gettext("Не верно введена дата !"));
     goto naz;
    }
   break;

  case FK3: /*Склады*/
   if(dirsklad(1,&kods,&naims) == 0) 
    VV.data_z_plus(1,kods.ravno());
   goto naz;

  case FK4 : /*Материалы*/
   clear();
   gr=kd=0;  
   if(dirmatr(&gr,&kd,&naims,1,0,"",0,0) == 0)
    {
     VV.data_plus(0,kd);
     goto naz;
    }
   goto naz;

  default:
    goto naz;

 }

if(VV.VVOD_SPISOK_return_data(0)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введён код материалла !"));
  goto naz;
 }
SQLCURSOR cur;

sprintf(strsql,"select naimat from Material where kodm=%s",VV.VVOD_SPISOK_return_data(0));
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код материалла"),VV.VVOD_SPISOK_return_data(0));
  iceb_t_soob(strsql);
  goto naz;
 }

naimat.new_plus(row[0]);

skl=0;
skl=(int)ATOFM(VV.VVOD_SPISOK_return_data(1));

if(skl != 0)
 {
  sprintf(strsql,"select naik from Sklad where kod=%d",
  skl);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,gettext("Нет склада %d в списке складов !"),skl);
    iceb_t_soob(strsql);
    goto naz;
   }

  naiskl.new_plus(row[0]);
 }


GDITE();
sprintf(strsql,"select * from Dokummat1 where datd >= '%d-01-01' and \
kodm=%s and datd <= '%d-%02d-%02d'",gos,VV.VVOD_SPISOK_return_data(0),gos,mos,dos);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
 }

if(kolstr == 0)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Нет документов на этот материал !"));
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  goto naz;
 }

time(&tmm);
bf=localtime(&tmm);

sprintf(imaf,"mat%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

clear();
refresh();

fprintf(ff,"%s\n\n%s %s %s\n\
%s %d.%d.%d%s  %s - %d:%d\n\
%s %d.%d.%d%s %s.\n",
iceb_t_get_pnk("00",0),
gettext("Движение материалла"),
VV.VVOD_SPISOK_return_data(0),naimat.ravno(),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min,
gettext("До"),
dos,mos,gos,
gettext("г."),
gettext("включительно"));

printw("%s\n%s %s %s\n\
%s %d.%d.%d%s  %s - %d:%d\n\
%s %d.%d.%d%s %s.\n",
iceb_t_get_pnk("00",0),
gettext("Движение материалла"),
VV.VVOD_SPISOK_return_data(0),naimat.ravno(),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min,
gettext("До"),
dos,mos,gos,
gettext("г."),
gettext("включительно"));
refresh();

if(naiskl.ravno()[0] != '\0')
  fprintf(ff,gettext("Склад %s\n"),naiskl.ravno());

fprintf(ff,"\
--------------------------------------------------------------------------------\n");
fprintf(ff,gettext("Скл.|  Дата    |Номер документа|Ед.изм.|П/в|Карт. |Количест.| Цена  |Цена с НДС|\n"));
fprintf(ff,"\
--------------------------------------------------------------------------------\n");
printw("\
---------------------------------------------------------------------\n");
printw(gettext("Скл.|  Дата    |Номер документа|Ед.изм.|П/в|Карт. |Количест.| Цена  |\n"));
printw("\
---------------------------------------------------------------------\n");
/*printw("kolstr=%d\n",kolstr);*/
ost=0.;
kolp=0;
float pnds=0.;
SQL_str row1;
SQLCURSOR cur1;
while(cur.read_cursor(&row) != 0)
 {
  if(proverka(skladi.ravno(),row[1],0,0) != 0)
    continue;

  //читаем процент НДС в шапку документа
  sprintf(strsql,"select pn from Dokummat where datd='%s' and sklad=%s and nomd='%s'",
  row[0],row[1],row[2]);
  if(readkey(strsql,&row1,&cur1) == 1)
   pnds=atof(row1[0]);
   
  memset(pv,'\0',sizeof(pv));
  if(row[13][0] == '1')
   {
    pv[0]='+';
    ost+=atof(row[5]);
   }
  if(row[13][0] == '2')
   {
    pv[0]='-';
    ost-=atof(row[5]);
   }  

  bb=atof(row[6]);
  bb=bb+bb*pnds/100.;
  bb=okrug(bb,0.01);
  rsdat(&d,&m,&g,row[0],2); 

  fprintf(ff,"%-4s %02d.%02d.%d %-*s %-*s %3s %-6s %9.9g %7.7g %8.8g\n",
  row[1],d,m,g,
  iceb_tu_kolbait(15,row[2]),row[2],
  iceb_tu_kolbait(7,row[7]),row[7],
  pv,row[3],atof(row[5]),atof(row[6]),bb);

  printw("%-4s %02d.%02d.%d %-*s %-*s %3s %-6s %9.9g %7.7g\n",
  row[1],d,m,g,
  iceb_tu_kolbait(15,row[2]),row[2],
  iceb_tu_kolbait(7,row[7]),row[7],
  pv,row[3],atof(row[5]),atof(row[6]));

  strzag(LINES-1,0,kolstr,kolp);  
  kolp++;
 }

if(fabs(ost) < 0.00000001)
 ost=0.;
 
fprintf(ff,"%*s %9.9g\n",iceb_tu_kolbait(50,gettext("Остаток:")),gettext("Остаток:"),ost);
printw("%*s %9.9g\n",iceb_tu_kolbait(50,gettext("Остаток:")),gettext("Остаток:"),ost);

fclose(ff);
printw("%s %s\n",gettext("Распечатка выгружена"),imaf);
OSTANOV();

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка движения по материалу"));

iceb_t_ustpeh(imaf,0);

iceb_t_rabfil(&oth,"");

}
