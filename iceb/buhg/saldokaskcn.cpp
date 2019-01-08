/*$Id: saldokaskcn.c,v 5.11 2014/01/31 12:11:33 sasa Exp $*/
/*15.07.2015	25.09.2006	Белых А.И.	saldokaskcn.c
Перенос сальдо для подсистемы "Учёт кассовых ордеров." по кодам целевого назначения
*/
#include "buhg.h"


void saldokaskcn()
{
class iceb_tu_str god_ps("");
short godp;
int i=0;
short dt,mt,gt;


if(iceb_t_prn() != 0) /*Проверка возможности изменять настройки*/
 return;

poltekdat(&dt,&mt,&gt);
god_ps.new_plus(gt);

VVOD VVOD1(2);

VVOD1.VVOD_SPISOK_add_ZAG(gettext("Перенос сальдо по кассовым ордерам"));

VVOD1.VVOD_SPISOK_add_MD(gettext("Введите год на который нужно перенести сальдо"));
     
naz:;

clear();

helstr(LINES-1,0,
"F2",gettext("расчёт"),
"F5",gettext("блокировка"),
"F10",gettext("выход"),NULL);
     
if((i=vvod1(&god_ps,5,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
  return;

switch(i)
 {
  case ESC:
   return;

  case KEY_RESIZE:
   goto naz;

  case ENTER:
  case FK2:
  case PLU:
    break;

  case FK5:
    iceb_tl_blok();
    goto naz;
    
  case FK10:
    return;
  default:
   goto naz;
 }

godp=god_ps.ravno_atoi();


if( godp < gt-100 || godp > gt)
 { 
  iceb_t_soob(gettext("Не верно веден год на который нужно перенести сальдо !"));
  goto naz;
 } 

if(iceb_t_pbpds(1,godp) != 0)
 {
  goto naz;
 }
short dn=1;
short mn=1;
short gn=godp-1;
short dk=31;
short mk=12;
short gk=godp-1;

GDITE();

char strsql[512];
int kolstr=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;

class SPISOK kas_kodcn; //Список код кассы-код целевого назначения
class masiv_din_double mscn; //массив сумм сальдо целевого назначения


//Читаем сальдо по кодам целевого назначения
sprintf(strsql,"select kod,ks,saldo from Kascnsl where god=%d",gn);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }


int nomer=0;
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s|%s",row[1],row[0]);
  if((nomer=kas_kodcn.find(strsql)) < 0)
   kas_kodcn.plus(strsql);

  mscn.plus(atof(row[2]),nomer);
 }


//читаем документы

sprintf(strsql,"select kassa,tp,datd,nomd,kodop from Kasord where \
datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' order by datd,tp asc",
gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
int kolstr1=0;
double suma=0.,sumapod=0.;
short d,m,g;
short tipz;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  tipz=atoi(row[1]);
  //читаем код целевого назначения этой операции
  if(tipz == 1)
    sprintf(strsql,"select kcn from Kasop1 where kod='%s'",row[4]);
  if(tipz == 2)
    sprintf(strsql,"select kcn from Kasop2 where kod='%s'",row[4]);
  if(readkey(strsql,&row1,&cur1) == 1)
   {
    if(atoi(row1[0]) == 0)
     continue;
   }
  else 
   continue;

  sprintf(strsql,"%s|%s",row[0],row1[0]);  
  if((nomer=kas_kodcn.find(strsql)) < 0)
    kas_kodcn.plus(strsql);
    
  rsdat(&d,&m,&g,row[2],2);
  
  sumkasor(row[0],tipz,row[3],g,&suma,&sumapod);
  if(tipz == 1)  
    sumapod*=-1; 
  mscn.plus(sumapod,nomer);

 }

//удаляем записи если они есть

sprintf(strsql,"delete from Kascnsl where god=%d",godp);
sql_zapis(strsql,0,0);
int kassa;
char kodcn[10];

for(int ii=0; ii < kas_kodcn.kolih() ; ii++)
 {
  polen(kas_kodcn.ravno(ii),&kassa,1,'|');
  polen(kas_kodcn.ravno(ii),kodcn,sizeof(kodcn),2,'|');
   
  sprintf(strsql,"insert into Kascnsl values (%d,%s,%d,%.2f,%d,%ld)",
  godp,kodcn,kassa,mscn.ravno(ii),iceb_t_getuid(),time(NULL));
  sql_zapis(strsql,0,0);
 
 }

/*Создаём стартовый кассовый ордер*/

sprintf(strsql,"select kod from Kasop1 where kod='%s'",gettext("ОСТ"));
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"insert into Kasop1 (kod,naik,metkapr,ktoz,vrem) values('%s','%s',0,%d,%ld)",
  gettext("ОСТ"),
  gettext("Стартове сальдо"),
  iceb_t_getuid(),
  time(NULL));

  sql_zapis(strsql,0,0);
 }

sprintf(strsql,"select kassa,tp,suma from Kasord1 where \
datp >= '%04d-%02d-%02d' and datp <= '%04d-%02d-%02d' order by datd,tp asc",
gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

class masiv_din_int kod_kas;
class masiv_din_double suma_kas;
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  kassa=atoi(row[0]);
  tipz=atoi(row[1]);

  suma=atof(row[2]);  

  if(tipz == 2)
   suma*=-1;
   
  if((nomer=kod_kas.find(kassa)) < 0)
   kod_kas.plus(kassa,nomer);   
  suma_kas.plus(suma,nomer);
 }
move(0,0);
for(int nom=0; nom < kod_kas.kolih(); nom++)
 {
  sprintf(strsql,"delete from Kasord where datd='%d-01-01' and nomd='000' and kassa=%d",godp,kod_kas.ravno(nom));
  sql_zapis(strsql,1,0);
  sprintf(strsql,"delete from Kasord1 where datd='%d-01-01' and nomd='000' and kassa=%d",godp,kod_kas.ravno(nom));
  sql_zapis(strsql,1,0);
  
  sprintf(strsql,"insert into Kasord (kassa,tp,datd,nomd,kodop,podt,god,ktoi,vrem) values(%d,1,'%d-01-01','000','%s',1,%d,%d,%ld)",
  kod_kas.ravno(nom),godp,gettext("ОСТ"),godp,iceb_t_getuid(),time(NULL));

  sql_zapis(strsql,1,0);

  sprintf(strsql,"insert into Kasord1 (kassa,tp,datd,datp,nomd,kontr,suma,god,ktoi,vrem) values(%d,1,'%d-01-01','%d-01-01','000','00',%.2f,%d,%d,%ld)",
  kod_kas.ravno(nom),godp,godp,suma_kas.ravno(nom),godp,iceb_t_getuid(),time(NULL));

  sql_zapis(strsql,1,0);
  printw("%s:%d %s:%.2f\n",gettext("Касса"),kod_kas.ravno(nom),gettext("Сальдо"),suma_kas.ravno(nom));
 }

iceb_t_soob(gettext("Перенос сальдо закончен"));



}
