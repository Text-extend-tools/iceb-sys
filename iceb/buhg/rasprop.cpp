/* $Id: rasprop.c,v 5.21 2013/08/13 05:49:53 sasa Exp $ */
/*01.01.2017	13.06.2000	Белых А.И.	rasprop.c
Распечатка пропуска для вывоза полученного товара-материалла
*/
#include        <errno.h>
#include	"buhg.h"

extern double   okrg1; /*Округление*/
extern double	okrcn;  /*Округление цены*/
extern short    srtnk;

void rasprop(short dd,short md,short gd,int skl,const char *nomdok)
{
SQL_str         row,rowtmp;
int		kolstr;
char		strsql[1024];
FILE            *f1;
class iceb_tu_str kontr("");
int		tipz;
char		datop[32];
short		metz;
short		d,m,g;
class iceb_tu_str bros("");
class iceb_tu_str naiskl("");
char		imaf[64];
double		kratn;
class iceb_tu_str ein("");
double		kolih,kolihi;
int		nomerz;
double		mest,mesti;
class iceb_tu_str naim("");
SQLCURSOR cur,curtmp;

/*Читаем шапку документа*/
sprintf(strsql,"select * from Dokummat \
where datd='%d-%02d-%02d' and sklad=%d and nomd='%s'",
gd,md,dd,skl,nomdok);

if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s-%s=%d %s=%s %s=%d ",
  __FUNCTION__,
  gettext("Год"),gd,
  gettext("Документ"),
  nomdok,
  gettext("Склад"),
  skl);  

  iceb_t_soob(strsql);

  return;
 }

kontr.new_plus(row[3]);
tipz=atoi(row[0]);


sprintf(strsql,"select nomerz,sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d",
gd,nomdok,skl);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

memset(datop,'\0',sizeof(datop));


if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
  metz=atoi(row[0]);
  if(metz == 9)
   {
    rsdat(&d,&m,&g,row[1],0);
    if(d != 0 && SRAV1(g,m,d,gd,md,dd) > 0)
     sprintf(datop,"%02d.%02d.%d",d,m,g);
   }
 }

sprintf(strsql,"%d",getpid());
memset(imaf,'\0',sizeof(imaf));
sprintf(imaf,"prp%.6s.lst",strsql);

if((f1 = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }
int kolstrok=0;

rashap(tipz,&kolstrok,"00",kontr.ravno(),0,0,f1);
fprintf(f1,"\x1B\x50"); /*10-знаков*/

/*Читаем наименование склада*/
sprintf(strsql,"select naik from Sklad where kod=%d",
skl);

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  move(20,0);
  beep();
  printw(gettext("Не найден склад %d в списке складов !"),skl);
  printw("\n");
  OSTANOV();
 }
else
  naiskl.new_plus(row[0]);

fprintf(f1,"\n%s\n\n",naiskl.ravno());

fprintf(f1,"         %s N%s\n",
gettext("ПРОПУСК к расходной накладной"),nomdok);

memset(strsql,'\0',sizeof(strsql));
mesc(md,1,strsql);
fprintf(f1,"\
                      %d %s %d%s\n",
dd,strsql,gd,gettext("г."));

/*Читаем доверенность*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
gd,nomdok,skl,1);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  fprintf(f1,"%s N",gettext("Доверенность"));
  polen(row[0],&bros,1,'#');
  fprintf(f1," %s",bros.ravno());
  polen(row[0],&bros,2,'#');
  fprintf(f1," %s %s",gettext("от"),bros.ravno());
 }
fprintf(f1,"\n");


/*Читаем через кого*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
gd,nomdok,skl,2);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
  fprintf(f1,"Через кого: %s\n",row[0]);

fprintf(f1,"\x0F"); /*Включение ужатого режима печати*/

fprintf(f1,"\
--------------------------------------------------------------------------------\n");
fprintf(f1,gettext(" N |       Наименование продукции                 |Ед. |Количество| Кол. |Крат-|\n"));
fprintf(f1,gettext("   |              (товару)                        |изм.|          | м-ст |ность|\n"));
fprintf(f1,"\
--------------------------------------------------------------------------------\n");

/*Готовим отсортированный файл*/

//sortnak(dd,md,gd,skl,nomdok);
class iceb_t_tmptab tabtmp;
const char *imatmptab={__FUNCTION__};

if(sortdokmuw(dd,md,gd,skl,nomdok,imatmptab,&tabtmp,NULL) != 0)
 return;


sprintf(strsql,"select * from %s",imatmptab);

if(srtnk == 1)
  sprintf(strsql,"select * from %s order by kgrm asc,naim asc",imatmptab);

int kolstrtmp=0;
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора!"),strsql);
  return;
 }

kolihi=mesti=0.;
nomerz=0;
while(curtmp.read_cursor(&rowtmp) != 0)
 {

  naim.new_plus(rowtmp[1]);
  kratn=atoi(rowtmp[5]);
  ein.new_plus(rowtmp[6]);
  kolih=atof(rowtmp[9]);

  kolihi+=kolih;
  mest=0.;
  if(kratn != 0.)
   {
    mest=kolih/kratn;
    mest=okrug(mest,0.1);
   }
  mesti+=mest;
  fprintf(f1,"%3d %-*.*s %-*s %10.10g %6.6g %4.4g\n",
  ++nomerz,
  iceb_tu_kolbait(46,naim.ravno()),iceb_tu_kolbait(46,naim.ravno()),naim.ravno(),
  iceb_tu_kolbait(4,ein.ravno()),ein.ravno(),
  kolih,mest,kratn);

 }
fprintf(f1,"\
--------------------------------------------------------------------------------\n");
fprintf(f1,"%*s:%10.10g %6.6g\n",
iceb_tu_kolbait(55,gettext("Итого")),gettext("Итого"),kolihi,mesti);


fprintf(f1,"\n\%s%s\n",gettext("\
Директор______________________        Главный  бухгалтер_______________\n\n"),
gettext("\
Отпустил______________________        Принял__________________________"));

fclose(f1);
class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка пропуска"));
iceb_t_ustpeh(imaf,1);
iceb_t_rabfil(&oth,"");

}
