/*$Id: avtprukr.c,v 5.35 2014/08/31 06:18:17 sasa Exp $*/
/*13.07.2015	16.11.2002	Белых А.И.	avtprukr.c
Подпрограмма автоматического выполнения проводок для "Учета командировочных расходов"
*/

#include        <math.h>
#include        "buhg.h"


void avtprukr(short dd,short md,short gd,
const char *nomdok)
{
FILE *ff_prot=NULL;
class iceb_tu_str vstr("");
char		strsql[1024];
int		kolstr=0;
SQL_str		row;
class iceb_tu_str kodop("");
class iceb_tu_str kodops("");
class iceb_tu_str shetu("");
class iceb_tu_str shetkr("");
class iceb_tu_str shetnds("");
class iceb_tu_str kodu("");
class iceb_tu_str kodkr("");
class iceb_tu_str kodnds("");;
class iceb_tu_str koment("");
double		deb=0.;
double		kre=0.;
short		dp=0,mp=0,gp=0;
class iceb_tu_str kodkont("");
class iceb_tu_str kodr("");
struct OPSHET	shetv;
class iceb_tu_str nomao(""); //Номер авансового отчёта
class iceb_tu_str kodkontr("");
SQLCURSOR curr;
int kekv=0;
class SPISOK sp_prov;
class masiv_din_double sum_prov_dk;
SQL_str row_alx;
class SQLCURSOR cur_alx;
int metka_vkontr=2; /*метка режима выполнения проводки для контрагента если его нет в списке счёта 0-запрос 2-автоматическая вставка*/

sprintf(strsql,"select shet,kolih,cena,snds,kontr,sn,ss,kodr from Ukrdok1 where \
datd='%04d-%d-%d' and nomd='%s'",gd,md,dd,nomdok);

SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
if(kolstr == 0)
 return;

//Узнаем дату авансового отчёта

sprintf(strsql,"select kont,vkom,nomao,datao from Ukrdok where god=%d and nomd='%s'",gd,nomdok);
if(sql_readkey(&bd,strsql,&row,&curr) == 1)
 {
  kodkont.new_plus(row[0]);
  kodop.new_plus(row[1]);
  nomao.new_plus(row[2]);
  rsdat(&dp,&mp,&gp,row[3],2);
 }
else
 return;
 
if(dp == 0)
 {
  VVOD VVOD1(2);
  VVOD1.VVOD_SPISOK_add_MD(gettext("Введите дату авансового отчёта"));
  vstr.new_plus("");
  int kom=0;  
  if((kom=vvod1(&vstr,11,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
    return;
  if(kom == ESC)
    return;

  if(rsdat(&dp,&mp,&gp,vstr.ravno(),1) != 0)
   {
    iceb_t_soob(gettext("Не верно введена дата авансового отчёта !"));
    return;
   }

  sprintf(strsql,"update Ukrdok set datao='%04d-%d-%d' where god=%d and nomd='%s'",
  gp,mp,dp,gd,nomdok);
  
  sql_zapis(strsql,1,0);
  
 }

if(iceb_t_pvglkni(mp,gp) != 0)
 return;

sprintf(strsql,"select str from Alx where fil='avtprukr.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдены настройки avtprukr.alx\n");
  return;
 }

if(nomao.ravno()[0] != '\0')
 {
//  sprintf(koment,"N%s",nomao);
  koment.new_plus("N");
  koment.plus(nomao.ravno());
 } 
while(cur.read_cursor(&row) != 0)
 {
  cur_alx.poz_cursor(0);
  while(cur_alx.read_cursor(&row_alx) != 0)
   {
    if(row_alx[0][0] == '#')
     continue;

    if(polen(row_alx[0],&kodops,1,'|') != 0)
     continue;

    if(polen(row_alx[0],&kodr,6,'|') == 0)
     {
      if(kodr.ravno_atoi() != 0)
       if(atoi(row[7]) != kodr.ravno_atoi())
        continue;
     }

    if(SRAV(kodop.ravno(),kodops.ravno(),0) != 0)
     continue;

    if(polen(row_alx[0],&shetu,2,'|') != 0)
     continue;

    if(SRAV(row[0],shetu.ravno(),0) != 0)
     continue;

    if(polen(row_alx[0],&shetkr,3,'|') != 0)
     continue;
    
    
    if(row[5][0] != '\0')
     shetnds.new_plus(row[5]);
    else
     if(polen(row_alx[0],&shetnds,4,'|') != 0)
       continue;

    polen(row_alx[0],&kekv,5,'|');

    kodu.new_plus(kodkont.ravno());
    if(provsh(&kodu,shetu.ravno(),&shetv,metka_vkontr,0) != 0)
       continue;

    kodkr.new_plus(kodkont.ravno());
    if(provsh(&kodkr,shetkr.ravno(),&shetv,metka_vkontr,0) != 0)
       continue;

    kodnds.new_plus(kodkont.ravno());
    if(provsh(&kodnds,shetnds.ravno(),&shetv,metka_vkontr,0) != 0)
       continue;

    kodkontr.new_plus(row[4]);
    if(kodkontr.getdlinna() > 1)
     if(provsh(&kodkontr,shetkr.ravno(),&shetv,metka_vkontr,0) != 0)
       continue;
    
    //Проводка на сумму без НДС
    kre=0.;
    deb=atof(row[2])+atof(row[6]);
    if(kodkontr.getdlinna() <= 1)            
      zapmpr(gp,mp,dp,shetu.ravno(),shetkr.ravno(),kodu.ravno(),kodkr.ravno(),deb,kre,koment.ravno(),2,kekv,&sp_prov,&sum_prov_dk,ff_prot);
    else
     {
      zapmpr(gp,mp,dp,shetkr.ravno(),shetkr.ravno(),kodkr.ravno(),kodkontr.ravno(),deb,kre,koment.ravno(),2,kekv,&sp_prov,&sum_prov_dk,ff_prot);
      zapmpr(gp,mp,dp,shetu.ravno(),shetkr.ravno(),kodkontr.ravno(),kodkontr.ravno(),deb,kre,koment.ravno(),2,kekv,&sp_prov,&sum_prov_dk,ff_prot);
     }

    //Проводка на сумму НДС
    kre=0.;
    deb=atof(row[3]);
            
    zapmpr(gp,mp,dp,shetnds.ravno(),shetkr.ravno(),kodnds.ravno(),kodkr.ravno(),deb,kre,koment.ravno(),2,kekv,&sp_prov,&sum_prov_dk,ff_prot);
   }

  
 }


/*Запись проводок из памяти в базу*/

zapmpr1(nomdok,kodop.ravno(),0,time(NULL),ICEB_MP_UKR,dd,md,gd,0,&sp_prov,&sum_prov_dk,ff_prot);


}
