/*$Id: zvb_ukrsib.c,v 5.35 2014/08/31 06:18:19 sasa Exp $*/
/*21.08.2014	04.04.2004	Белых А.И.	zvb_ukrsib.c
Выгузка для укрсиббанка
*/
#include    "buhg.h"

void		ukrsib_h(char *imaf,long kolz);



void	zvb_ukrsib(short prn,char *zapros,struct dirnupoi *poisk)
{
char strsql[512];
char imafdbf[64];
char imaftmp[64];
char imafr[64];
FILE  *fftmp;
FILE  *ffr;
int   kolstr;
class iceb_tu_str rukov("");
class iceb_tu_str glavbuh("");
class iceb_tu_str bros("");


SQLCURSOR cur;
SQLCURSOR cur1;
SQLCURSOR curr;
SQL_str row,row1;

if((kolstr=cur.make_cursor(&bd,zapros)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),zapros);
  return;
 }


iceb_t_poldan("Табельный номер руководителя",&bros,"zarnast.alx");
if(bros.ravno()[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",bros.ravno());
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
    rukov.new_plus(row1[0]);
 } 

iceb_t_poldan("Табельный номер бухгалтера",&bros,"zarnast.alx");

if(bros.ravno()[0] != '\0')
 {
  sprintf(strsql,"select fio,inn,telef from Kartb where tabn=%s",bros.ravno());
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
    glavbuh.new_plus(row1[0]);
 }

class iceb_tu_str ope_tor("");
if(ukrsib_start(imafr,imaftmp,&ope_tor,&fftmp,&ffr) != 0)
 return;

class iceb_tu_str fio("");

int kolstr1=0;
int nomer=0;

class iceb_tu_str kartshet("");
double suma;
double sumai=0.;
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(poidirnu(row,poisk,&fio) != 0)
     continue;

    
  sprintf(strsql,"select fio,bankshet from Kartb where tabn=%s",row[1]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
   {
    kartshet.new_plus(row1[1]);

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

  suma=atof(row[4]);
  if(prn == 2)
    suma*=(-1);   

  sumai+=suma;

  ukrsib_str(dt,mt,gt,suma,kartshet.ravno(),fio.ravno(),&nomer,row[1],ope_tor.ravno(),ffr,fftmp);

    
 }

ukrsib_end(imafdbf,imaftmp,sumai,nomer,rukov.ravno(),glavbuh.ravno(),ffr,fftmp);

sprintf(strsql,"%s %s",imafr,imafdbf);
class spis_oth oth;

oth.spis_imaf.plus(imafr);
oth.spis_naim.plus(gettext("Распечатка документов для перечисления на карт-счета."));

oth.spis_imaf.plus(imafdbf);
oth.spis_naim.plus(gettext("Документ в электронном формате"));

iceb_t_rabfil(&oth,"");

}
