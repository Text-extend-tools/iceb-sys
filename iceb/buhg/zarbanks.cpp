/*$Id: zarbanks.c,v 5.6 2013/05/17 14:56:17 sasa Exp $*/
/*10.12.2010	09.12.2009	Белых А.И.	zarmbanks.c
Получение списка банков
*/
#include <errno.h>
#include "buhg.h"


int zarbanks(class iceb_tu_str *kod00,int *kod_ban) /*Код банка в списке банков*/
{
int kod_kl=0;
class SPISOK naimb;
class SPISOK kodkontr;
class masiv_din_int kodbi;
class iceb_tu_str nmb("");
class iceb_tu_str kodb("");
class iceb_tu_str kodk("00");
class masiv_din_int kod_banka; /*коды банков в списке банков в таблице Zarsb*/
int kod_banka_int=0; /*код банка в списке банков в таблице Zarsb*/
class SQLCURSOR cur_alx;
SQL_str row_alx;
int kolstr=0;
char strsql[512];
*kod_ban=0;

sprintf(strsql,"select str from Alx where fil='zarbanks.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(-1);
 }

if(kolstr == 0)
 {
  iceb_t_soob("Не найдены настройки zarbanks.alx");
  return(-1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  if(polen(row_alx[0],&kodb,1,'|') != 0)
   continue;

  if(polen(row_alx[0],&nmb,2,'|') != 0)
   continue;

  polen(row_alx[0],&kodk,3,'|');
  if(kodk.getdlinna() <= 1)
   kodk.new_plus("00");

  polen(row_alx[0],&kod_banka_int,4,'|');
   
  naimb.plus(nmb.ravno());
  kodbi.plus(kodb.ravno_atoi());  
  kodkontr.plus(kodk.ravno());  
  kod_banka.plus(kod_banka_int);
 }

if(naimb.kolih() == 1)
 {
  *kod_ban=kod_banka.ravno(0);
  kod00->new_plus(kodkontr.ravno(0));
  return(kodbi.ravno(0));
 } 
helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
"F10",gettext("выход"),NULL);

int voz=dirmasiv(&naimb, -1,-1, 0,"",1,&kod_kl);

if(voz < 0 || kod_kl != ENTER)
    return(-1);
*kod_ban=kod_banka.ravno(voz);
kod00->new_plus(kodkontr.ravno(voz));
return(kodbi.ravno(voz));
}
