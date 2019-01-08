/*$Id: saldo_upl.c,v 5.18 2013/09/26 09:43:40 sasa Exp $*/
/*15.07.2015	10.02.2005	Белых А.И.	saldo_upl.c
Перенос сальдо для подсистемы "Учёт путевых листов"
*/
#include        "buhg.h"

extern short startgodupl; //Стартовый год для "Учета путевых листов"


void saldo_upl()
{
short	godn; //Стартовый год для "Учета путевых листов"
char strsql[512];
char bros[512];
int kom=0;

if(iceb_t_prn() != 0) /*Проверка возможности изменять настройки*/
 return;

iceb_t_poldan("Стартовый год",bros,"uplnast.alx");
startgodupl=godn=atoi(bros);
if(godn == 0)
 {
  iceb_t_soob(gettext("Не введён стартовый год!"));
  return;
 }

VVOD VV(0);

VV.VVOD_SPISOK_add_ZAG(gettext("Учёт путевых листов"));
VV.VVOD_SPISOK_add_ZAG(gettext("Перенос стартовых остатков топлива"));
sprintf(strsql,"%s:%d",gettext("Стартовый год"),godn);
VV.VVOD_SPISOK_add_ZAG(strsql);

VV.VVOD_SPISOK_add_MD(gettext("С какого года.."));
VV.VVOD_SPISOK_add_MD(gettext("На какой год..."));
VV.VVOD_SPISOK_add_data(godn,5);
VV.VVOD_SPISOK_add_data(godn+1,5);

naz:;
clear();


helstr(LINES-1,0,"F2/+",gettext("расчёт"),
"F5",gettext("блокировка"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

switch(kom)
 {
  case FK2:
  case PLU:
    break;

  case FK5:
    iceb_tl_blok();
    return;

  case FK10:
    return;

  default:
   goto naz;
 }

startgodupl=godn = (short)VV.data_ravno_atoi(0);
short godk = (short)VV.data_ravno_atoi(1);

if(godk < 1998 || godk > 3000)
 {
  iceb_t_soob(gettext("Не правильно введён год !"));
  return;
 }  

if(iceb_t_pbpds(1,godk) != 0)
 goto naz;

clear(); 
GDITE();

int kolstr=0;
SQL_str row,row1;
SQLCURSOR cur,cur1;

class SPISOK kv_ka;//Список кодов водителей-кодов автомобилей 

//Сальдо читаем так как могут быть остатки по которым небыло путевых листов
sprintf(strsql,"select distinct kv,ka from Uplsal where god=%d",godn);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s|%s",row[0],row[1]);
  kv_ka.plus(strsql);
 }

//Узнаём все комбинации код водителя-код автомобиля
sprintf(strsql,"select distinct kv,ka from Upldok1 where datd >= '%d-01-01' \
and datd <= '%d-12-31'",godn,godk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }


int kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
//  printw("%s %s\n",row[0],row[1]);
  
  sprintf(strsql,"%s|%s",row[0],row[1]);
  if(kv_ka.find(strsql) < 0)
    kv_ka.plus(strsql);

 }


time_t vrem;
time(&vrem);

//Удаляем сальдо
sprintf(strsql,"delete from Uplsal where god=%d",godk);
sql_zapis(strsql,0,0);

char kodvod[64];
char kodavt[64];
//while(cur.read_cursor(&row) != 0)
kolstr1=0; 
kolstr=kv_ka.kolih();
class iceb_tu_str fio("");
for(int in=0; in < kv_ka.kolih(); in++)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  polen(kv_ka.ravno(in),kodvod,sizeof(kodvod),1,'|');
  polen(kv_ka.ravno(in),kodavt,sizeof(kodavt),2,'|');

  SPISOK marka_topl;
  masiv_din_double ost;
  
  if(ostva(1,1,godk,kodvod,kodavt,0,&marka_topl,&ost,NULL) == 0.)
   continue;
    
  for(int ii=0; ii < marka_topl.kolih(); ii++)
   {
    if(ost.ravno(ii) == 0.)
     continue;

    fio.new_plus("");
    sprintf(strsql,"select naik from Uplouot where kod=%d",atoi(kodvod));
    if(readkey(strsql,&row1,&cur1) == 1)
     fio.new_plus(row1[0]);
          
    printw("%d %-3s %-3s %-*.*s %-*s %10.3f\n",
    godk,kodavt,
    kodvod,
    iceb_tu_kolbait(20,fio.ravno()),
    iceb_tu_kolbait(20,fio.ravno()),
    fio.ravno(),
    iceb_tu_kolbait(5,marka_topl.ravno(ii)),marka_topl.ravno(ii),ost.ravno(ii));

    refresh();


    sprintf(strsql,"insert into Uplsal values (%d,%s,%s,'%s',%.2f,%d,%ld)",
    godk,kodavt,kodvod,marka_topl.ravno(ii),ost.ravno(ii),iceb_t_getuid(),vrem);
//    printw("%s\n",strsql);

    sql_zapis(strsql,1,0);
    
   }
 }
 

short dt,mt,gt;
poltekdat(&dt,&mt,&gt);
if(godk <= gt)
 {
  sprintf(strsql,"update Alx set str='Стартовый год|%d\n' where fil='uplnast.alx' and str like 'Стартовый год|%d%%'",godk,godn);

  if(sql_zapis(strsql,0,0) == 0)
    startgodupl=godk;
 }

OSTANOV();
}
