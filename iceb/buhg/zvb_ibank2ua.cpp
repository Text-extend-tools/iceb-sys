/*$Id: zvb_ibank2ua.c,v 5.14 2013/09/26 09:43:46 sasa Exp $*/
/*30.06.2013	18.09.2008	Белых А.И.	zvb_ibank2ua.c
Выгрузна в файл для экспорта в систему iBank2 UA
*/
#include <math.h>
#include    "buhg.h"


void zvb_ibank2ua(short prn,const char *zapros,struct dirnupoi *poisk,const char *kod00)
{
char strsql[512];
char imaf[64];
char imafr[64];
FILE  *ff;
FILE  *ffr;
int   kolstr;


SQLCURSOR cur;
SQLCURSOR cur1;
SQLCURSOR curr;
SQL_str row,row1;

if((kolstr=cur.make_cursor(&bd,zapros)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),zapros);
  return;
 }
class iceb_tu_str vidnahis(""); 

if(zvb_ibank2ua_start(imafr,imaf,&vidnahis,kod00,&ff,&ffr) != 0)
 return;

class iceb_tu_str kartshet("");
class iceb_tu_str inn("");
int kolstr1=0;
class iceb_tu_str fio("");
double suma=0.;
double sumai=0.;
int nomer_zap=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(poidirnu(row,poisk,&fio) != 0)
     continue;
  suma=atof(row[4]);

  if(fabs(suma) < 0.01)
   continue;

  if(prn == 2)
    suma*=(-1);   

    
  sprintf(strsql,"select fio,bankshet,inn from Kartb where tabn=%s",row[1]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
   {
    kartshet.new_plus(row1[1]);
    inn.new_plus(row1[2]);
   }
  else
   {
    kartshet.new_plus("");
    inn.new_plus("");
   }
  if(kartshet.ravno()[0] == '\0')
   {
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("Не введён счёт !"));
    sprintf(strsql,"%s:%s %s",gettext("Табельный номер"),row[1],fio.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    continue;
   }


  sumai+=suma;

  zvb_ibank2ua_str(&nomer_zap,fio.ravno(),kartshet.ravno(),inn.ravno(),suma,ff,ffr);

 }
 
zvb_ibank2ua_end(sumai,vidnahis.ravno(),ffr,ff);

class spis_oth oth;
oth.spis_imaf.plus(imafr);
oth.spis_naim.plus(gettext("Распечатка документов для перечисления на карт-счета"));

oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Документ в электронном формате"));

iceb_t_ustpeh(oth.spis_imaf.ravno(0),1);

iceb_t_rabfil(&oth,"");

}
