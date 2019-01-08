/*$Id: zvb_ukreksim.c,v 5.25 2013/09/26 09:43:46 sasa Exp $*/
/*30.06.2013	08.07.2005	Белых А.И.	zvb_ukreksim.c
Выгрузка сумм для зачисления на карт-счета для Укрпромбанка
*/

#include    "buhg.h"



void	zvb_ukreksim(short prn,
struct dirnupoi *poisk,
int kolstr,
SQLCURSOR *cur)
{
char strsql[512];
char imafdbf[64];
char imaftmp[64];
char imafr[64];
FILE  *fftmp;
FILE  *ffr;

GDITE();

SQLCURSOR cur1;
SQLCURSOR curr;
SQL_str row,row1;

class iceb_t_fioruk_rk ruk;
class iceb_t_fioruk_rk glbuh;
iceb_t_fioruk(1,&ruk);
iceb_t_fioruk(2,&glbuh);


if(ukreksim_hap(imafr,imaftmp,&fftmp,&ffr) != 0)
 return;


class iceb_tu_str fio("");

int kolstr1=0;
int nomer=0;

class iceb_tu_str kartshet("");
double suma;
double sumai=0.;
cur->poz_cursor(0);
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);

while(cur->read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(poidirnu(row,poisk,&fio) != 0)
     continue;

  suma=atof(row[4]);
  if(suma == 0.)
   continue;

  
  sprintf(strsql,"select fio,inn,bankshet from Kartb where tabn=%s",row[1]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
   {
    kartshet.new_plus(row1[2]);
   }
  else
   {
    kartshet.new_plus("");
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

   
  if(prn == 2)
    suma*=(-1);   
  
  ukreksim_zstr(fftmp,ffr,&nomer,suma,fio.ravno(),kartshet.ravno(),row[1],dt,mt,gt);


  sumai+=suma;
    
 }

ukreksim_end(nomer,imaftmp,imafdbf,sumai,ruk.fio.ravno(),glbuh.fio.ravno(),fftmp,ffr);

sprintf(strsql,"%s %s",imafr,imafdbf);
class spis_oth oth;

oth.spis_imaf.plus(imafr);
oth.spis_naim.plus(gettext("Распечатка документов для перечисления на карт-счета."));

oth.spis_imaf.plus(imafdbf);
oth.spis_naim.plus(gettext("Документ в электронном формате"));

iceb_t_rabfil(&oth,"");

}

