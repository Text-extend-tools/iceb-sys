/*$Id: zvb_alfabank.c,v 5.10 2014/08/31 06:18:19 sasa Exp $*/
/*29.06.2013	15.10.2009	Белых А.И.	zvb_alfabank.c
Формирование файлов для зачисления на картсчета зарплаты
*/
#include    "buhg.h"




void zvb_alfabank(short prn,const char *zapros,struct dirnupoi *poisk)
{
char strsql[512];
char imafr[64];
char imaf_dbf[64];
char imaf_dbf_tmp[64];
class iceb_tu_str kedrpou("");

FILE  *ff_dbf_tmp;
FILE  *ffr;
int   kolstr;


SQLCURSOR cur;
SQLCURSOR cur1;
SQLCURSOR curr;
SQL_str row,row1;
double suma=0.;


sprintf(strsql,"select kod from Kontragent where kodkon='00'");
if(readkey(strsql,&row,&cur) == 1)
 kedrpou.new_plus(row[0]);




if((kolstr=cur.make_cursor(&bd,zapros)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),zapros);
  return;
 }
class iceb_tu_str fio("");

if(zvb_alfabank_start(imafr,imaf_dbf,imaf_dbf_tmp,&ff_dbf_tmp,&ffr) != 0)
 return;


int kolstr1=0;

class iceb_tu_str kartshet("");
double sumai=0.;
class iceb_tu_str inn("");

cur.poz_cursor(0);
int nomer_zap=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(poidirnu(row,poisk,&fio) != 0)
     continue;

  suma=atof(row[4]);
  if(suma == 0.)
   continue;
  if(prn == 2)
    suma*=(-1);   

  sprintf(strsql,"select fio,bankshet,inn from Kartb where tabn=%s",row[1]);
  if(readkey(strsql,&row1,&cur1) == 1)
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

  zvb_alfabank_str(++nomer_zap,fio.ravno(),kartshet.ravno(),suma,inn.ravno(),kedrpou.ravno(),ff_dbf_tmp,ffr);


    
 }

zvb_alfabank_end(imaf_dbf,imaf_dbf_tmp,sumai,nomer_zap,ffr,ff_dbf_tmp);

class spis_oth oth;
oth.spis_imaf.plus(imafr);
oth.spis_naim.plus(gettext("Распечатка документов для перечисления на карт-счета"));

oth.spis_imaf.plus(imaf_dbf);
oth.spis_naim.plus(gettext("Документ в электронном формате"));

iceb_t_ustpeh(oth.spis_imaf.ravno(0),1);

iceb_t_rabfil(&oth,"");

}
