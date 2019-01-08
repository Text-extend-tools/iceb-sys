/*$Id: ukrku.c,v 5.21 2013/08/13 05:49:56 sasa Exp $*/
/*16.07.2015	15.09.2003	Белых А.И.	ukrku.c
Распечатка командировочного удостоверения
*/
#include <errno.h>
#include "buhg.h"


void   ukrku(short gd,const char *nomd)
{
FILE    *ffku;
char    imaf[64];
char    strsql[512];
SQLCURSOR cur;
SQLCURSOR curr;
class iceb_tu_str kont("");
class iceb_tu_str pnaz("");
class iceb_tu_str organ("");
class iceb_tu_str celk("");
short   dn,mn,gn;
short   dk,mk,gk;
class iceb_tu_str nompr("");
short   dp,mp,gp;
SQL_str row;
class iceb_tu_str stroka("");
int     nomer=0;
class iceb_tu_str fio("");
class iceb_tu_str dolgn("");
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;

GDITE();

sprintf(strsql,"select kont,pnaz,organ,celk,datn,datk,nompr,datp \
from Ukrdok where god=%d and nomd='%s'",gd,nomd);
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  sprintf(strsql,"%s !",gettext("Не найдена шапка документа !"));
  iceb_t_soob(strsql);
  return;
 }
kont.new_plus(row[0]);

pnaz.new_plus(row[1]);

organ.new_plus(row[2]);

celk.new_plus(row[3]);

rsdat(&dn,&mn,&gn,row[4],2);

rsdat(&dk,&mk,&gk,row[5],2);

nompr.new_plus(row[6]);

rsdat(&dp,&mp,&gp,row[7],2);

sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kont.ravno());
if(sql_readkey(&bd,strsql,&row,&curr) == 1)
  fio.new_plus(row[0]);
  
short kolsimv=iceb_tu_strlen(ICEB_ZAR_PKTN);

class iceb_tu_str tabnom("");
class iceb_tu_str nomp("");

tabnom.new_plus(iceb_tu_adrsimv(kolsimv,kont.ravno()));

//Читаем должность
sprintf(strsql,"select fio,dolg,nomp from Kartb where tabn=%d",tabnom.ravno_atoi());
if(sql_readkey(&bd,strsql,&row,&curr) == 1)
 {
  fio.new_plus(row[0]);
  dolgn.new_plus(row[1]);
  nomp.new_plus(row[2]);
 }
int koldn=period(dn,mn,gn,dk,mk,gk,0);

sprintf(strsql,"select str from Alx where fil='ukrku.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"ukrku.alx");
  iceb_t_soob(strsql);
  return;
 }


sprintf(imaf,"ku%d.lst",getpid());

if((ffku = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  nomer++;
  stroka.new_plus(row_alx[0]);
  switch(nomer)
   {
    case 9:
      iceb_tu_vstav(&stroka,nomd,53,65,1);
      break;

    case 11:
      iceb_tu_vstav(&stroka,fio.ravno(),30,67,1);
      break;

    case 12:
      iceb_tu_vstav(&stroka,dolgn.ravno(),23,67,1);
      break;

    case 13:
      iceb_tu_vstav(&stroka,pnaz.ravno(),40,67,1);
      break;

    case 14:
      iceb_tu_vstav(&stroka,organ.ravno(),23,67,1);
      break;

    case 15:
      iceb_tu_vstav(&stroka,celk.ravno(),23,67,1);
      break;

    case 17:
      sprintf(strsql,"%d",koldn);
      iceb_tu_vstav(&stroka,strsql,19,20,1);
      memset(strsql,' ',sizeof(strsql));
      if(dn != 0)
       sprintf(strsql,"%02d.%02d.%04d",dn,mn,gn);

      iceb_tu_vstav(&stroka,strsql,29,38,1);
/************
      По просьбе клиента дату конца не печататем
      memset(strsql,' ',sizeof(strsql));
      if(dk != 0)
        sprintf(strsql,"%02d.%02d.%04d",dk,mk,gk);
      iceb_tu_vstav(&stroka,strsql,45,54,1);
***************/
      break;

    case 18:
      iceb_tu_vstav(&stroka,nompr.ravno(),17,25,1);

      sprintf(strsql,"%02d.%02d.%04d",dp,mp,gp);
      iceb_tu_vstav(&stroka,strsql,31,40,1);

      break;

    case 19:
      iceb_tu_vstav(&stroka,nomp.ravno(),33,62,1);
      break;
     
   }

  fprintf(ffku,"%s",stroka.ravno());

 }

podpis(ffku);

fclose(ffku);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка командировочного удостоверения"));

iceb_t_ustpeh(imaf,1);

iceb_t_rabfil(&oth,"");


}
