/*$Id: rasdoksp.c,v 5.16 2013/08/13 05:49:52 sasa Exp $*/
/*15.07.2015	27.02.2007	Белых А.И.	rasdoksp.c
Распечатка спецификации (по просьбе картографической фабрики)
Если вернули 0-документ создан
             1-нет
*/
#include <errno.h>
#include	"buhg.h"

extern double	okrcn;  /*Округление цены*/
extern double   okrg1; /*Округление*/

int rasdoksp(short dg,short mg,short gg, //дата документа
int skl,  //Склад
const char *nomdok, //Номер документа
class iceb_tu_str *imaf_ras)
{
char strsql[1024];
char imaf[64];
FILE *ff;
SQL_str row;
SQL_str row1;
class SQLCURSOR cur;
class SQLCURSOR cur1;


/*Читаем шапку документа*/
sprintf(strsql,"select * from Dokummat \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",
gg,mg,dg,skl,nomdok);

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  VVOD SOOB(1);

  sprintf(strsql,"%s-%s",__FUNCTION__,gettext("Не найден документ !")),
  SOOB.VVOD_SPISOK_add_MD(strsql);

  sprintf(strsql,"%s=%d.%d.%d %s=%s %s=%d",
  gettext("Дата"),
  dg,mg,gg,
  gettext("Номер документа"),
  nomdok,
  gettext("Код склада"),
  skl);

  SOOB.VVOD_SPISOK_add_MD(strsql);

  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return(1);
 }
float pnds=atof(row[13]);

char kontr[64];
strncpy(kontr,row[3],sizeof(kontr)-1);

int tipz=atoi(row[0]);

char kodop[64];
strcpy(kodop,row[6]);

sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
gg,nomdok,skl,11);
short lnds=0;
if(readkey(strsql,&row,&cur) == 1)
 lnds=atoi(row[0]);

sprintf(strsql,"select * from Dokummat1 where datd='%04d-%02d-%02d' and \
sklad=%d and nomd='%s' order by kodm asc",
gg,mg,dg,skl,nomdok);
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

class iceb_tu_str dogov;
iceb_t_getuslp(kontr,&dogov);

class iceb_tu_str naim_kontr("");
//Читаем наименование организации
sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kontr);
if(readkey(strsql,&row1,&cur1) == 1)
 {
  naim_kontr.new_plus(row1[0]);
 }

sprintf(imaf,"rsp%d.lst",getpid());
imaf_ras->new_plus(imaf);

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

fprintf(ff,"\
%50s Додаток N\n\
%45s до договору N%s\n\n","","",dogov.ravno());

fprintf(ff,"\
%20s СПЕЦИФІКАЦІЯ N\n\n\
%20s від _______________\n",
"",
"");

fprintf(ff,"1.Постачальник передає у власність Покупця наступний товар:\n");

fprintf(ff,"\
----------------------------------------------------------------------------\n");

fprintf(ff,"\
 N |   Найменування товару        |Од.вим.| Кількість|  Ціна    |  Сумма    |\n");

fprintf(ff,"\
----------------------------------------------------------------------------\n");
int nomer_pp=0;
class iceb_tu_str naim("");
double kolih;
double cena;
double suma;
double itogo=0.;
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"select naimat from Material where kodm=%s",row[4]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
   {
    naim.new_plus(row1[0]);
    if(row[16][0] != '\0')
     {
      naim.plus(" ",row[16]);
     }
   }
  else
   naim.new_plus("");
  kolih=atof(row[5]);
  cena=atof(row[6]);
  cena=okrug(cena,okrcn);
  suma=kolih*cena;  
  suma=okrug(suma,okrg1);
  itogo+=suma;
  
  fprintf(ff,"%3d %-*.*s %-*s %10.10g %10s",
  ++nomer_pp,
  iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
  iceb_tu_kolbait(7,row[7]),row[7],
  kolih,prcn(cena));
  fprintf(ff," %10s\n",prcn(suma));
  if(iceb_tu_strlen(naim.ravno()) > 30)
   fprintf(ff,"%3s %s\n","",iceb_tu_adrsimv(30,naim.ravno()));
 }

fprintf(ff,"\
----------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%11.2f\n",iceb_tu_kolbait(63,gettext("Итого")),gettext("Итого"),itogo);

int dlinna=63;
double itogo1=dok_end_mu(tipz,dg,mg,gg,nomdok,skl,dlinna,itogo,pnds,lnds,0,0,ff);

class iceb_tu_str suma_prop("");
preobr(itogo1,&suma_prop,0);

fprintf(ff,"%s:%s\n\n",gettext("Сумма прописью"),suma_prop.ravno());

class iceb_tu_str uslopl("");
class iceb_tu_str srokp("");

/*Читаем условия оплаты товара*/
sprintf(strsql,"select zapis from Kontragent1 where kodkon='%s' and \
zapis like 'Условия оплаты товара%%'",kontr);
if(readkey(strsql,&row,&cur) == 1)
 {
  polen(row[0],&uslopl,2,'|');
 }

/*Читаем срок поставки товара*/
sprintf(strsql,"select zapis from Kontragent1 where kodkon='%s' and \
zapis like 'Срок поставки товара%%'",kontr);
if(readkey(strsql,&row,&cur) == 1)
 {
  polen(row[0],&srokp,2,'|');
 }


fprintf(ff,"\
2. Договірні ціни не вищі 20%% рівня звичайних цін.\n\
3. Умови оплаты товару:%-6s\n\
4. Умови постачання товару: СРТ м.Вінниця, згідно ІНКОТЕРМС 2000.\n\
5. Термін постачання товару: протягом %s днів з дати підписання сторонами данної\n\
   специфікації.\n\
6. Ця спецификація набирає сили з дати її підписання сторонами і є невід'ємною\n\
   частиною вищевказаного договору.\n\
7. Ця специфікація, підписана по факсімільному зв'язку, має юридичну чинність і\n\
   повинна бути підтверджена оригіналом протягом 30-ти днів.\n\
8. Ця специфікація складена в 2-х (двох) примірниках по одному для кожної із сторін.\n\
\n\
                        ПІДПИСИ СТОРІН:\n",uslopl.ravno(),srokp.ravno());


class iceb_tu_str kodkontr00("00");
class iceb_tu_str naim00("");

if(tipz == 2)
 {
  if(iceb_t_poldan(kodop,strsql,"matnast.alx") == 0)
   {
    kodkontr00.new_plus(strsql);
   }
 }


//Читаем наименование организации
sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kodkontr00.ravno());
if(readkey(strsql,&row,&cur) == 1)
  naim00.new_plus(row[0]);
 

if(tipz == 1)
 fprintf(ff,"\
       Постачальник:                            Покупець:\n\
%-*.*s %s\n",iceb_tu_kolbait(40,naim_kontr.ravno()),iceb_tu_kolbait(40,naim_kontr.ravno()),naim_kontr.ravno(),naim00.ravno());

if(tipz == 2)
 fprintf(ff,"\
       Покупец:                            Постачальник:\n\
%-*.*s %s\n",iceb_tu_kolbait(40,naim_kontr.ravno()),iceb_tu_kolbait(40,naim_kontr.ravno()),naim_kontr.ravno(),naim00.ravno());

fprintf(ff,"\n\n\n");

podpis(ff);

fclose(ff);

iceb_t_ustpeh(imaf,1);

return(0);
}
