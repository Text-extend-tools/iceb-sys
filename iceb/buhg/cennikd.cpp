/*$Id: cennikd.c,v 5.17 2013/09/26 09:43:29 sasa Exp $*/
/*13.07.2015	07.05.2003	Белых А.И.	cennikd.c
Распечатка ценников для документа
*/
#include        <errno.h>
#include	"buhg.h"

extern double	okrg1;  /*Округление суммы*/

void   cennikd(short dd,short md,short gd, //дата документа
int skl,  //Склад
const char *nomdok) //Номер документа
{
char 	strsql[512];
SQL_str	row,row1;
int 	kolstr=0;

GDITE();

sprintf(strsql,"select distinct kodm from Dokummat1 where datd='%04d-%02d-%02d' and \
sklad=%d and nomd='%s'",
gd,md,dd,skl,nomdok);
SQLCURSOR cur;
SQLCURSOR cur1;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
  return;

FILE *ff;
char imaf[64];

sprintf(imaf,"cen%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

class iceb_tu_str naim("");
double cena=0.;
class iceb_tu_str kodtv("");
class iceb_tu_str strihkod("");
time_t          tmm;
struct  tm      *bf;
time(&tmm);
bf=localtime(&tmm);


while(cur.read_cursor(&row) != 0)
 {
  cena=0.;
  naim.new_plus("");
  strihkod.new_plus("");
  kodtv.new_plus("");

  //Читаем цену продажи и наименование товара
  sprintf(strsql,"select naimat,strihkod,cenapr from Material where kodm=%s",row[0]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
   {
    naim.new_plus(row1[0]);
    strihkod.new_plus(row1[1]);

    cena=atof(row1[2]);
    cena=okrug(cena,okrg1);
   }  
  kodtv.new_plus(row[0]);
  cennik(bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,kodtv.ravno(),naim.ravno(),cena,strihkod.ravno(),iceb_t_get_pnk("00",0),ff);
 }

fclose(ff);

cenniks(imaf); //Преобразование в колонки

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Ценники на товары"));
iceb_t_ustpeh(imaf,0);

iceb_t_rabfil(&oth,"");
}
