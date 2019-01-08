/*$Id:$*/
/*22.03.2017    26.10.1999      Белых А.И.      saldomu.c
Программа переноса стартовых остатков для "Материального учета"
*/
#include        "buhg.h"

extern short startgod; /*Стартовый год просмотров в Материальном учёте*/

void		saldomu()
{
class ostatokt ost;
time_t          tmm;
time_t		vrem;
short           god,god1;
int		skl,nkk;
double		ccn;
double		nds;
int		sklz;
int		mnds;
short		par;
SQL_str         row;
long		kolstr,kolstr1;
char		strsql[512];
class iceb_tu_str ostmat("");
class iceb_tu_str ei("");
int		kodm;
class iceb_tu_str sgod("");
SQLCURSOR curr;

if(iceb_t_prn() != 0) /*Проверка возможности изменять настройки*/
 return;


iceb_t_poldan("Стартовый год",&sgod,"matnast.alx");
god1=sgod.ravno_atoi();

if(god1 == 0)
 {
  iceb_t_soob(gettext("Не введён стартовый год!"));
  return;
 }

VVOD VV(0);

VV.VVOD_SPISOK_add_ZAG(gettext("Материальный учёт"));
VV.VVOD_SPISOK_add_ZAG(gettext("Перенос стартовых остатков по карточкам"));
sprintf(strsql,"%s:%d",gettext("Стартовый год"),god1);
VV.VVOD_SPISOK_add_ZAG(strsql);

VV.VVOD_SPISOK_add_MD(gettext("С какого года.."));
VV.VVOD_SPISOK_add_MD(gettext("На какой год..."));
VV.VVOD_SPISOK_add_data(god1,5);
VV.VVOD_SPISOK_add_data(god1+1,5);

naz:;
clear();


helstr(LINES-1,0,"F2/+",gettext("расчёт"),
"F5",gettext("блокировка"),
"F10",gettext("выход"),NULL);

par=VV.vvod(0,1,1,-1,-1);

switch(par)
 {
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

god1 = (short)ATOFM(VV.VVOD_SPISOK_return_data(0));
god = (short)ATOFM(VV.VVOD_SPISOK_return_data(1));
if(god1 == 0)
 god1=god-1;

short dt=0,mt=0,gt=0;
poltekdat(&dt,&mt,&gt);

if(god1 < gt-100 || god1 > gt)
 {
  iceb_t_soob(gettext("Не правильно введён год начала !"));
  goto naz;
 }

if(god < gt-100 || god > gt)
 {
  iceb_t_soob(gettext("Не правильно введён год на который переносим остатки !"));
  goto naz;
 }

if(iceb_t_pbpds(1,god) != 0)
 goto naz;
 

clear();

VVOD DANET(1);
DANET.VVOD_SPISOK_add_ZAG(gettext("Перенос стартовых остатков по карточкам материалов"));

sprintf(strsql,"%s %d %s %s %d %s ? %s",
gettext("Перенести остатки с"),
god1,
gettext("г."),
gettext("на"),
god,
gettext("г."),
gettext("Вы уверены ?"));

DANET.VVOD_SPISOK_add_MD(strsql);

if(danet(&DANET,2,stdscr) == 2)
  return;

clear();
time(&tmm);

if(matnast() != 0)
 return;

printw("Год %d %d\n",god1,god);

ostmat.new_plus(gettext("ОСТ"));
/*Проверяем есть ли операция стартовый остаток и если нет то записываем ее*/
sprintf(strsql,"select kod from Prihod where kod='%s'",ostmat.ravno());

if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  sprintf(strsql,"insert into Prihod \
values ('%s','%s',%d,%ld,0,1)",ostmat.ravno(),gettext("Стартовый остаток"),\
  iceb_t_getuid(),tmm);   

  sql_zapis(strsql,0,0);
 }

sprintf(strsql,"select sklad,nomk,kodm,cena,nds,mnds,ei from Kart \
order by sklad asc");
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return;
 }

kolstr1=sklz=0;
float pnds=iceb_t_pnds(1,1,god);

while(cur.read_cursor(&row) != 0)
 {
  kolstr1++;
  strzag(LINES-1,0,kolstr,kolstr1);
  skl=atoi(row[0]);
  nkk=atoi(row[1]);  
  ostkar(1,1,god1,31,12,god-1,skl,nkk,&ost);

  printw("%s: %-5d %s: %-5d %s: %.2f\n",
  gettext("Склад"),
  skl,
  gettext("Карточка"),
  nkk,
  gettext("Остаток"),
  ost.ostg[3]);

  if(ost.ostg[3] > 0.0000001 )
   {
    if(sklz != skl)
     {
      //На всякий случай удаляем, возможно это повторный перенос остатков
      sprintf(strsql,"delete from Zkart where datd='%04d-01-01' and nomd='000' and sklad=%d",god,skl);
      sql_zapis(strsql,0,0);

      sprintf(strsql,"delete from Dokummat1 where datd='%04d-01-01' and nomd='000' and sklad=%d",god,skl);
      sql_zapis(strsql,0,0);

      sprintf(strsql,"delete from Dokummat where datd='%04d-01-01' and nomd='000' and sklad=%d",god,skl);
      sql_zapis(strsql,0,0);
      
      time(&vrem);

      if(zap_s_mudok(1,1,1,god,skl,"00","000","",ostmat.ravno(),1,1,"",0,0,0,1,"00",pnds) != 0)
       return;

      sklz=skl;
     }
    kodm=atoi(row[2]);
    ccn=atof(row[3]);
    nds=atof(row[4]);
    mnds=atoi(row[5]);
    ei.new_plus(row[6]);
    
    zapvdok(1,1,god,skl,nkk,kodm,"000",ost.ostg[3],ccn,ei.ravno(),nds,mnds,0,1,0,"","",0,"");
    zapvkr(1,1,god,"000",skl,nkk,1,1,god,1,ost.ostg[3],ccn);

   }

 }

printw_vr(tmm);

if(god <= gt)
 {
  sprintf(strsql,"update Alx set str='Стартовый год|%d\n' where fil='matnast.alx' and str like 'Стартовый год|%d%%'",
  god,god1);

  if(sql_zapis(strsql,0,0) == 0)
    startgod=god;
 }

}
