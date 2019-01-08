/*$Id: xdkbanks.c,v 5.21 2013/09/26 09:43:44 sasa Exp $*/
/*30.03.2016	20.02.2008	Белых А.И.	xdkbanks.c
Автоматическое выполнение проводок для платёжного поручения
на основе сальдо контрагентов по счёту
*/

#include "buhg.h"
#include "dok4.h"

int xdkbanks_kk(const char *kontr);

extern short	startgodb; /*Стартовый год*/

extern class REC rec;

void xdkbanks(const char *tabl,short dd,short md,short gd,const char *nomdok,const char *kodop)
{
char strsql[1024];
int kolstr;
int kolkontr=0,kolstr1=0;
SPISOK KONT;
SQLCURSOR cur;
short dk,mk,gk;
static class iceb_tu_str shet("");
static class iceb_tu_str kontr("");
static class iceb_tu_str dat2("");
static class iceb_tu_str grupk("");
static class iceb_tu_str shet_kor("");
static char data_prov[11];

int kom=0,kom1=0;
short       gods=startgodb;

int metkadk=0;

VVOD MENU(3);
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Автоматическое выполнение проводок"));

VV.VVOD_SPISOK_add_MD(gettext("Счёт........................"));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата сальдо........(д.м.г).."));//1
VV.VVOD_SPISOK_add_MD(gettext("Код контрагента........(,,)."));//2
VV.VVOD_SPISOK_add_MD(gettext("Код группы контрагента.(,,)."));//3
VV.VVOD_SPISOK_add_MD(gettext("Счёт корреспондент.........."));//4
VV.VVOD_SPISOK_add_MD(gettext("Дата проводки......(д.м.г).."));//5

VV.VVOD_SPISOK_add_data(shet.ravno(),32);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11); 
VV.VVOD_SPISOK_add_data(kontr.ravno(),256); 
VV.VVOD_SPISOK_add_data(grupk.ravno(),256); 
VV.VVOD_SPISOK_add_data(shet_kor.ravno(),32); 
VV.VVOD_SPISOK_add_data(data_prov,sizeof(data_prov)); 

naz:;
clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

shet.new_plus(VV.VVOD_SPISOK_return_data(0));
dat2.new_plus(VV.VVOD_SPISOK_return_data(1));
kontr.new_plus(VV.VVOD_SPISOK_return_data(2));
grupk.new_plus(VV.VVOD_SPISOK_return_data(3));
shet_kor.new_plus(VV.VVOD_SPISOK_return_data(4));
strcpy(data_prov,VV.VVOD_SPISOK_return_data(5));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("dokum2_1_2_1.txt");
   goto naz;

  case ESC:
  case FK10:
   return;

  case FK2:
  case PLU:
     break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп контрагентов"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Выход."));//3

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 3 :
      case -1:
       goto naz;

      case 0 :
        vibrek("Gkont",&grupk);
        VV.VVOD_SPISOK_zapis_data(3,grupk.ravno());
      
        goto naz;


      case 1 :
        vibrek("Plansh",&shet);
        VV.VVOD_SPISOK_zapis_data(0,shet.ravno());
        goto naz;

      case 2 :
        vibrek("Kontragent",&kontr);
        VV.VVOD_SPISOK_zapis_data(2,kontr.ravno());
        goto naz;
     }


  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

if(shet.ravno()[0] == '\0')
 {
  iceb_t_soob(gettext("Не введён счёт !"));
  goto naz;
 }

if(rsdat(&dk,&mk,&gk,dat2.ravno(),1) != 0)
 {
  iceb_t_soob(gettext("Неправильно ввели дату сальдо!"));
  goto naz;
 }
short dr=0,mr=0,gr=0;
if(rsdat(&dr,&mr,&gr,data_prov,1) != 0)
 {
  iceb_t_soob(gettext("Неправильно ввели дату проводки!"));
  goto naz;
 }
 
struct OPSHET rek_shet;
if(prsh1(shet.ravno(),&rek_shet) != 0)
 goto naz;

if(rek_shet.saldo != 3)
 {
  iceb_t_soob(gettext("Счёт имеет свёрнутое сальдо!"));
  goto naz;
 }

if(prsh1(shet_kor.ravno(),&rek_shet) != 0)
 goto naz;

if(iceb_t_pvglkni(mr,gr) != 0)
 goto naz;
if(iceb_t_pbsh(mr,gr,shet.ravno(),shet_kor.ravno(),"") != 0)
 goto naz;

if(gods == 0)
 gods=gk;

MENU.VVOD_delete();
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное и нажмите Enter"));

MENU.VVOD_SPISOK_add_MD(gettext("Дебет"));
MENU.VVOD_SPISOK_add_MD(gettext("Кредит"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));
metkadk=0;
while(menu3w(stdscr,&MENU,&metkadk,-1,-1,0) != 0);
switch (metkadk)
 {
  case  2 :
  case -1 :
   goto naz;

 }


GDITE();

/*проверяем список банков в таблице. Если их больше одного то нужно проверять есть ли карточка 
этого банка у работкика или нет*/
sprintf(strsql,"select kod from Zarsb");
int kolbank=readkey(strsql);
int kod_banka=0;

if(kolbank > 1)
 if((kod_banka=xdkbanks_kk(rec.kodor.ravno())) < 0)
  {
   sprintf(strsql,"%s %s %s zarbanks.alx!",
   gettext("Не найден код банка для кода контрагента"),
   rec.kodor.ravno(),
   gettext("в настройках"));

   iceb_t_soob(strsql);
   goto naz;
  }

//Составляем список всех контрагентов
sprintf(strsql,"select ns,kodkon from Skontr where ns='%s'",shet.ravno());

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

SQL_str row;
kolkontr=0;
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(shet.ravno(),row[0],0,0) != 0)
    continue;
  if(proverka(kontr.ravno(),row[1],0,0) != 0)
    continue;
  if(KONT.find_r(row[1]) < 0)
   {
    kolkontr++;
    KONT.plus(row[1]);
   }
 }
GDITE();
double deb[kolkontr];
double kre[kolkontr];
memset(&deb,'\0',sizeof(deb));
memset(&kre,'\0',sizeof(kre));

//Читаем сальдо
sprintf(strsql,"select ns,kodkon,deb,kre from Saldo where kkk=1 and gs=%d and ns='%s'",gods,shet.ravno());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
int nomer=0;
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(shet.ravno(),row[0],0,0) != 0)
    continue;
  if(proverka(kontr.ravno(),row[1],0,0) != 0)
    continue;
//  printw("%s %s %s %s\n",row[0],row[1],row[2],row[3]);

  nomer=KONT.find_r(row[1]);
  deb[nomer]+=atof(row[2]);
  kre[nomer]+=atof(row[3]);
 }
GDITE();
//Читаем все проводки
sprintf(strsql,"select sh,kodkon,deb,kre from Prov where datp >= '%d-%d-%d' \
and datp <= '%d-%d-%d' and kodkon <> '' and sh='%s'",gods,1,1,gk,mk,dk,shet.ravno());

//printw("%s\n",strsql);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(shet.ravno(),row[0],0,0) != 0)
    continue;
  if(proverka(kontr.ravno(),row[1],0,0) != 0)
    continue;

  nomer=KONT.find_r(row[1]);
  deb[nomer]+=atof(row[2]);
  kre[nomer]+=atof(row[3]);
 }

GDITE();


int dlpris=iceb_tu_strlen(ICEB_ZAR_PKTN);

class iceb_tu_str bankshet("");
class iceb_tu_str fio("");
class iceb_tu_str tabnom("");
class iceb_tu_str inn("");
double sum=0.;
double debp=0.;
double krep=0.;
class iceb_tu_str kontr1("");
class iceb_tu_str kontr2("");
class iceb_tu_str koment("");
class iceb_tu_str podsyst("");

int tipz=0;
if(SRAV(tabl,"Pltp",0) == 0)
 {
  podsyst.new_plus(ICEB_MP_PPOR);
  tipz=2;
 }
if(SRAV(tabl,"Pltt",0) == 0)
 {
  podsyst.new_plus(ICEB_MP_PTRE);
  tipz=1;
 }

for(int snom=0 ; snom < kolkontr; snom++)
 {
  
  if(pvbanks_kprov(&KONT,snom,ICEB_ZAR_PKTN,metkadk,&sum,&tabnom,deb,kre,dlpris,&fio,&bankshet,&inn,grupk.ravno(),kod_banka) != 0)
   continue;

  debp=0.;
  krep=0.;
  kontr1.new_plus(KONT.ravno(snom));
  kontr2.new_plus(KONT.ravno(snom));

  if(metkadk == 0) /*Дебет*/
   krep=sum;
  if(metkadk == 1) /*кредит*/
   debp=sum;

  koment.new_plus(gettext("Перечислено"));
  koment.plus(" ");
  koment.plus(fio.ravno());   
  zapprov(0,gr,mr,dr,shet.ravno(),shet_kor.ravno(),kontr1.ravno(),kontr2.ravno(),podsyst.ravno(),nomdok,kodop,debp,krep,koment.ravno_filtr(),2,0,time(NULL),gd,md,dd,tipz,0);

 }

}
/****************************/
/*определяем код банка для заданного кода контрагента*/
/****************************/
int xdkbanks_kk(const char *kontr)
{
class iceb_tu_str kod_kontr("");
class SQLCURSOR cur_alx;
SQL_str row_alx;
int kolstr=0;
char strsql[512];
int kod_banka=0;
sprintf(strsql,"select str from Alx where fil='zarbanks.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(-1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s zarbanks.alx",gettext("Не найдены настройки"));
  iceb_t_soob(strsql);
  return(-1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  if(polen(row_alx[0],&kod_kontr,3,'|') != 0)
   continue;
  if(SRAV(kod_kontr.ravno(),kontr,0) != 0)
   continue;
  int voz=0;
  if((voz=polen(row_alx[0],&kod_banka,4,'|')) == 0)
   return(kod_banka);
   
 }

return(-1);
}
