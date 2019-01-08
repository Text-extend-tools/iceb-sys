/*$Id: imp_zar_kr.c,v 5.21 2013/05/17 14:56:03 sasa Exp $*/
/*20.04.2016	21.09.2005	Белых А.И.	imp_zar_kr.c
Импорт командировочных расходов в подсистему "Расчёт зарплаты"
*/
#include "buhg.h"


void imp_zar_kr(int tipz, //1-начисления 2-удержания
int kod_nah,short mes,short god)
{
char strsql[1024];
static class iceb_tu_str dat1(""),dat2("");
static class iceb_tu_str denz("");
static class iceb_tu_str kodkom("");
class iceb_tu_str kod("");
class iceb_tu_str naim("");


SQL_str row;
SQLCURSOR cur;
class iceb_tu_str naim_nah("");

int kom=0;

//читаем наименование начисления
sprintf(strsql,"select naik from Nash where kod=%d",kod_nah);
if(readkey(strsql,&row,&cur) == 1)
 naim_nah.new_plus(row[0]);

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Импорт начислений из подсистемы \"Учёт командировочных расходов\""));//1

sprintf(strsql,"%d %s",kod_nah,naim_nah.ravno());
VV.VVOD_SPISOK_add_ZAG(strsql);

sprintf(strsql,"%s:%d.%d",gettext("Дата"),mes,god);
VV.VVOD_SPISOK_add_ZAG(strsql);

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.(д.м.г)..................."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..(д.м.г)..................."));//1
VV.VVOD_SPISOK_add_MD(gettext("День записи..........................."));//2
VV.VVOD_SPISOK_add_MD(gettext("Коды командировочных начислений.(,,).."));//3

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11); 
VV.VVOD_SPISOK_add_data(denz.ravno(),3); 
VV.VVOD_SPISOK_add_data(kodkom.ravno(),512); 

naz:;

clear();
 
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("загрузить"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

move(LINES-1,0);

dat1.new_plus(VV.VVOD_SPISOK_return_data(0));
dat2.new_plus(VV.VVOD_SPISOK_return_data(1));
denz.new_plus(VV.VVOD_SPISOK_return_data(2));
kodkom.new_plus(VV.VVOD_SPISOK_return_data(3));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("zarp2_8_1_5.txt");
   goto naz;

  case ESC:
  case FK10:
   return;

  case FK2:
  case PLU:
     break;

  case FK3:
    kod.new_plus("");
    naim.new_plus("");
    if(dirkkr(&kod,&naim,1) == 0)
     VV.data_z_plus(3,kod.ravno());
    goto naz;

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

GDITE();
short dn,mn,gn;
short dk,mk,gk;

if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
 goto naz;

short den=denz.ravno_atoi();
short d=1,m=mes,g=god;
dpm(&d,&m,&g,5);

if(den < 1 || den > d)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Неправильно введён день !"));
  if(den > d)
   sprintf(strsql,"%d > %d",den,d);
  if(den < 1)
   sprintf(strsql,"%d < 1",den);
  SOOB.VVOD_SPISOK_add_MD(strsql);
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  goto naz;
 }

int dlina_prist=strlen(ICEB_ZAR_PKTN);


SQL_str row1;
SQLCURSOR cur1;
int kolstr,kolstr1=0;

sprintf(strsql,"select nomd,datd,kont from Ukrdok where \
datao >= '%04d-%02d-%02d' and datao <= '%04d-%02d-%02d'",
gn,mn,dn,gk,mk,dk);


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
//Смотрим есть ли счёт в начислении
class iceb_tu_str shet_nah("");

sprintf(strsql,"select shet from Nash where kod=%d",kod_nah);
if(readkey(strsql,&row1,&cur1) == 1)
 shet_nah.new_plus(row1[0]); 

class iceb_tu_str tabnom("");
short dnr,mnr,gnr;
short dkr,mkr,gkr;
class iceb_tu_str shetkar("");
int podrkar;
int kateg;
int sovm;
int zvan;
class iceb_tu_str lgot("");
class iceb_tu_str dolg("");
class ZARP     zp;

zp.dz=0; //Если день равен нулю, то удаление старой записи перед вставкой новой не происходит
int nz=0;
int vernuli;
double suma=0;

class iceb_tu_str shet("");

shet.new_plus(shet_nah.ravno());
int tabn;
int kolstr2=0;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s\n",row[0],row[1],row[2]);
    
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(strstrm(row[2],ICEB_ZAR_PKTN) != 1)
   continue;
   
  tabnom.new_plus(&row[2][dlina_prist]);
  tabn=tabnom.ravno_atoi();
  printw("tabnom=%s\n",tabnom.ravno());
  
  //Проверяем есть ли такой табельный номер  
  sprintf(strsql,"select datn,datk,shet,podr,kodg,kateg,\
sovm,zvan,lgoti,dolg from Kartb where tabn=%d",tabn);
  if(readkey(strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"%s %d !\n",gettext("Не найден табельный номер"),tabn);
    iceb_t_soob(strsql);
    continue;
   }

  rsdat(&dnr,&mnr,&gnr,row1[0],2);
  rsdat(&dkr,&mkr,&gkr,row1[1],2);
  shetkar.new_plus(row1[2]);

  podrkar=atoi(row1[3]);
  kateg=atoi(row1[5]);
  sovm=atoi(row1[6]);
  zvan=atoi(row1[7]);
  lgot.new_plus(row1[8]);
  dolg.new_plus(row1[9]);

  if(shet_nah.ravno()[0] == '\0')
   shet.new_plus(shetkar.ravno());
   

  sprintf(strsql,"select kodr,cena from Ukrdok1 where \
datd='%s' and nomd='%s'",row[1],row[0]);
//  printw("strsql=%s\n",strsql);
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    return;
   }
  if(kolstr2 == 0)
   continue;

  while(cur1.read_cursor(&row1) != 0)
   {

    if(proverka(kodkom.ravno(),row1[0],0,0) != 0)
     continue;
//    printw("%s %s\n",row1[0],row1[1]);
    suma=atof(row1[1]);
    if(tipz == 2)
     suma*=-1;    
    nz=0;
povtor:;

    if((vernuli=zapzarp(den,mes,god,tabn,tipz,kod_nah,suma,shet.ravno(),mes,god,0,nz++,"",podrkar,"",zp)) == 1)
     goto povtor;

    if(vernuli != 0)
     continue;
     
    zapzarn1(tabn,tipz,kod_nah,0,0,0,shet.ravno());

    zarsoc(mes,god,tabn);
    zaravpr(tabn,mes,god,NULL);

    //Проверяем есть ли запись карточки по этой дате и если нет делаем запись
    sprintf(strsql,"select god from Zarn where tabn=%d and god=%d and mes=%d",tabn,god,mes);
    if(readkey(strsql) == 0)  
      zapzarn(mes,god,podrkar,tabn,kateg,sovm,zvan,shetkar.ravno(),lgot.ravno(),dnr,mnr,gnr,dkr,mkr,gkr,0,dolg.ravno());
    
   }

 }
OSTANOV();

}
