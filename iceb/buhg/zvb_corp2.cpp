/*$Id:*$*/
/*02.10.2017	18.08.2014	Белых А.И.	zvb_corp2.c
Формирование файлов для зачисления на картсчета зарплаты
*/
#include    "buhg.h"




void zvb_corp2(short prn,const char *zapros,struct dirnupoi *poisk)
{
char strsql[512];
char imafr[64];
char imaf_dbf[64];
char imaf_dbf_tmp[64];

FILE  *ff_dbf_tmp;
FILE  *ffr;
int   kolstr;
int kolstr1=0;
class iceb_tu_str kartshet("");
class iceb_tu_str kshet("");
double sumai=0.;
class iceb_tu_str inn("");
int nomer_zap=0;


class SQLCURSOR cur;
class SQLCURSOR cur1;
SQL_str row,row1;
double suma=0.;
int metka_suma_0;

class VVOD DANET(1);
DANET.VVOD_SPISOK_add_MD(gettext("С нулевыми суммами ?"));

metka_suma_0=danet(&DANET,2,stdscr);

if((kolstr=cur.make_cursor(&bd,zapros)) < 0)
 {
  msql_error(&bd,__FUNCTION__,zapros);
  return;
 }
class iceb_tu_str fio("");

if(zvb_corp2_start(imafr,imaf_dbf,imaf_dbf_tmp,&ff_dbf_tmp,&ffr) != 0)
 return;


while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(poidirnu(row,poisk,&fio) != 0)
     continue;

  suma=atof(row[4]);

  if(metka_suma_0 == 2)
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

  if(polen(kartshet.ravno(),&kshet,1,'_') != 0)
   kshet.plus(kartshet.ravno());

  zvb_corp2_str(++nomer_zap,fio.ravno(),kshet.ravno(),suma,inn.ravno(),row[1],ff_dbf_tmp,ffr);


    
 }

zvb_corp2_end(imaf_dbf,imaf_dbf_tmp,sumai,nomer_zap,ffr,ff_dbf_tmp);

class spis_oth oth;
oth.spis_imaf.plus(imafr);
oth.spis_naim.plus(gettext("Распечатка документов для перечисления на карт-счета"));

oth.spis_imaf.plus(imaf_dbf);
oth.spis_naim.plus(gettext("Документ в электронном формате"));

iceb_t_ustpeh(oth.spis_imaf.ravno(0),1);

iceb_t_rabfil(&oth,"");

}
