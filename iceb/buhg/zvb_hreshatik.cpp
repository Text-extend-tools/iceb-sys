/*$Id: zvb_hreshatik.c,v 5.15 2013/09/26 09:43:46 sasa Exp $*/
/*29.06.2013	04.04.2004	Белых А.И.	zvb_hreshatik.c
Выгузка для банка Хрещатик
телефон 59-20-30
        59-20-31
        59-20-39
        59-20-40
Тюренков Алексей Валерьевич моб. 8068 8386276 <Tyurenkov@xbank.com.ua>
Начальник Кашпрук Валерий Валерьевич <Kashpruk@xbank.com.ua>

*/
#include    "buhg.h"




void zvb_hreshatik(short prn,const char *zapros,struct dirnupoi *poisk)
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
double suma=0.;

if((kolstr=cur.make_cursor(&bd,zapros)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),zapros);
  return;
 }
class iceb_tu_str fio("");
double sum=0.;
while(cur.read_cursor(&row) != 0)
 {
  if(poidirnu(row,poisk,&fio) != 0)
     continue;

  sum=atof(row[4]);
  if(prn == 2)
    sum*=(-1);   
  suma+=sum;
 }

short d_pp=0,m_pp=0,g_pp=0;

if(zvb_hreshatik_start(imafr,imaf,suma,&d_pp,&m_pp,&g_pp,&ff,&ffr) != 0)
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


  zvb_hreshatik_str(d_pp,m_pp,g_pp,&nomer_zap,fio.ravno(),kartshet.ravno(),inn.ravno(),suma,ff,ffr);

    
 }

zvb_hreshatik_end(sumai,nomer_zap,ffr,ff);

class spis_oth oth;
oth.spis_imaf.plus(imafr);
oth.spis_naim.plus(gettext("Распечатка документов для перечисления на карт-счета"));

oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Документ в электронном формате"));

iceb_t_ustpeh(oth.spis_imaf.ravno(0),1);

iceb_t_rabfil(&oth,"");

}
