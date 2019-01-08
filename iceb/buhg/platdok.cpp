/* $Id: platdok.c,v 5.113 2013/12/31 11:48:10 sasa Exp $ */
/*15.07.2015    12.04.1993      Белых А.И.      platdok.c
Работа с платёжными документами
*/
#include        <errno.h>
#include        "buhg.h"
#include        "dok4.h"

int dirtipd(const char *tablica);
void platdok_v(const char *tabl,int tipz);
void smpld(const char*,short,int);
int pgpld(const char*,class spis_oth*);
int paspddos(const char*);
void bnadra(const char*);
void b_privatinvest(const char*);
void b_pervinv(const char*);
void b_b2_bank_on_line(const char*);
void baval(const char*);
int bukrinbank(const char*);
int b_brokbiznes(const char*);
int b_soft_review(const char*);
int b_ukrprom(const char*);
int b_ukrexim(const char*);
int b_ukrsoc(const char*);
int b_iBank_2_UA(const char*);
int b_privat(const char*);
int b_tiny(const char*);
int b_skarb(const char*);
int b_akb_imexbank(const char*);
int b_corp2(const char*,class iceb_tu_str *imafdbf);
int dirbank(int *nomb,class iceb_tu_str *imamah,class iceb_tu_str *katalog,class iceb_tu_str *imafil,class iceb_tu_str *parol,class iceb_tu_str *comanda,class iceb_tu_str *script);
void platdok_pp(const char *tabl);
int b_ukrbiznes(const char *tabl);

extern class REC rec;
extern double   okrg1; /*Округление*/
extern short	startgodd;
extern short    kp;    /*Количество повторов*/

void  platdok(const char *tabl) //Имя таблицы
{
class iceb_tu_str vstr("");
char            bros[1024];
class iceb_tu_str kod("");
class iceb_tu_str naim("");
int             kom=0;
class iceb_tu_str kodo("");
class iceb_tu_str naimk("");
SQLCURSOR       cur;
char		strsql[1024];
int		poz,komv;
short		godn;

int tipz=0;

short dt,mt,gt;

poltekdat(&dt,&mt,&gt);

kom=0;
nnn:;

clear();
VVOD MENU(3);
VVOD SOOB(1);

if(SRAV(tabl,"Pltp",0) == 0)
 {
  MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0));
  MENU.VVOD_SPISOK_add_ZAG(gettext("Работа с платежными поручениями"));
  
  MENU.VVOD_SPISOK_add_MD(gettext("Ввод нового платежного поручения"));//0
  MENU.VVOD_SPISOK_add_MD(gettext("Ввод с использованием типового платежного поручения"));//1
  MENU.VVOD_SPISOK_add_MD(gettext("Поиск платежного поручения"));//2
  MENU.VVOD_SPISOK_add_MD(gettext("Просмотр журнала платежных поручений"));//3

  sprintf(strsql,"%s %d.%d.%d%s",gettext("Просмотр журнала платежных поручений за"),//4
  dt,mt,gt,gettext("г."));
  MENU.VVOD_SPISOK_add_MD(strsql);

  MENU.VVOD_SPISOK_add_MD(gettext("Распечатка журнала платежных поручений"));//5
  MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка контрагентов в файл"));//6
  MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка контрагентов в базу"));//7
  MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка операций"));//8
  MENU.VVOD_SPISOK_add_MD(gettext("Переслать файл платёжек для передачи в банк"));//9
  MENU.VVOD_SPISOK_add_MD(gettext("Установить/снять блокировку"));//10
  MENU.VVOD_SPISOK_add_MD(gettext("Настройка выполнения проводок"));//11
  MENU.VVOD_SPISOK_add_MD(gettext("Настройка списка банков для передачи документов"));//12
  MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//13
  tipz=2;
 }

if(SRAV(tabl,"Pltt",0) == 0)
 {
  MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0));
  MENU.VVOD_SPISOK_add_ZAG(gettext("Работа с платежными требованиями"));

  MENU.VVOD_SPISOK_add_MD(gettext("Ввод нового платежного требования"));//0
  MENU.VVOD_SPISOK_add_MD(gettext("Ввод c использованием типового платежного требования"));//1
  MENU.VVOD_SPISOK_add_MD(gettext("Поиск платежного требования"));//2
  MENU.VVOD_SPISOK_add_MD(gettext("Просмотр журнала платежных требований"));//3

  sprintf(strsql,"%s %d.%d.%d%s",gettext("Просмотр журнала платежных требований за"),
  dt,mt,gt,gettext("г."));//4
  MENU.VVOD_SPISOK_add_MD(strsql);

  MENU.VVOD_SPISOK_add_MD(gettext("Распечатка журнала платежных требований"));//5
  MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка контрагентов у файл"));//6
  MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка контрагентов в базу"));//7
  MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка операций"));//8
  MENU.VVOD_SPISOK_add_MD(gettext("Переслать файл платежных требований для передачи в банк"));//9
  MENU.VVOD_SPISOK_add_MD(gettext("Установка/снятие блокировки"));//10
  MENU.VVOD_SPISOK_add_MD(gettext("Настройка выполнения проводок"));//11
  MENU.VVOD_SPISOK_add_MD(gettext("Настройка списка банков для передачи документов"));//12
  MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//13
  tipz=1;
 }

while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

rec.clear();

class spis_oth oth;
switch (kom)
 {
  case 13 :
  case -1 :
   sprintf(bros,"pl%d.tmp",getpid());
   unlink(bros);
   return;

  case 0 :
   platdok_v(tabl,tipz);
   break;
   
  case 1 :
    clear();
    if(SRAV(tabl,"Pltp",0) == 0)
     strcpy(bros,"Ktnp");
    if(SRAV(tabl,"Pltt",0) == 0)
     strcpy(bros,"Ktnt");

    dirtipd(bros);
    goto nnn;

  case 2 :
    poiplatdok(tabl);
    goto nnn;

  case 3 :
    godn=startgodd;
    if(godn == 0)
     godn=gt;
    smpld(tabl,godn,0);
    goto nnn;

  case 4 :
    godn=startgodd;
    if(godn == 0)
     godn=gt;
    smpld(tabl,godn,1);
    goto nnn;

  case 5 :
    if(pgpld(tabl,&oth) == 0)
      iceb_t_rabfil(&oth,"");

    goto nnn;

  case 6 :
    iceb_redfil("platpor.alx",0);
    goto nnn;

  case 7 :

    naimk.new_plus("");
    dirkontr(&kodo,&naimk,0);
    goto nnn;

  case 8 :
    clear();
    kod.new_plus("");
    naim.new_plus("");
    poz=komv=0;

    if(SRAV(tabl,"Pltp",0) == 0)
      dirmat1("Opltp",&kod,&naim,LINES-5,0,2,0,&poz,&komv,"","\0");    
    if(SRAV(tabl,"Pltt",0) == 0)
      dirmat1("Opltt",&kod,&naim,LINES-5,0,2,0,&poz,&komv,"","\0");

    goto nnn;

  case 9 :
    
    platdok_pp(tabl);

    goto nnn;

  case 10 :
      iceb_tl_blok();
      goto nnn;

  case 11:
      if(SRAV(tabl,"Pltp",0) == 0)
        iceb_redfil("avtprodok.alx",0);
      if(SRAV(tabl,"Pltt",0) == 0)
        iceb_redfil("avtprodokt.alx",0);
      clear();
      goto nnn;

  case 12 :
    iceb_redfil("banki.alx",0);
    goto nnn;
     
    default:
      kom=0;
      goto nnn;
 }

}

/******************************/
/*Создание пустого комментария*/
/******************************/
void sozkom_pd(char *imaf,double nds,double sumad)
{
FILE            *ff2;
class iceb_tu_str shban(""); /*Банковский счет*/
double		smp;
extern double   smp0,smp1;
class SPISOK naznach;

char            bros[1024];
int		metkas=0;

/*Создаем комментарий*/
sprintf(imaf,"pl%d.tmp",getpid());
if((ff2 = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

iceb_t_poldan("Банковский счёт",&shban,"nastdok.alx");

iceb_t_poldan("Сумма без НДС в платежку",bros,"nastdok.alx");
metkas=0;
if(SRAV(bros,"Вкл",1) == 0)
  metkas=1;
class iceb_tu_str naz_pl("");

sprintf(bros,"Назначение платежа для операции %s",rec.kodop.ravno());
iceb_t_poldan(bros,&naz_pl,"nastdok.alx",1);

if(shban.ravno()[0] != '\0')
 {
  naznach.plus("N1");
  smp=smp0;
  if(SRAV(rec.mfo.ravno(),rec.mfo1.ravno(),0) != 0)
    smp=smp1;
  if(smp == 0)
    naznach.plus(gettext("N2 За расчётное обслуживание"));
  else
   {
    sprintf(bros,"%s %.2f",gettext("N2 За расчётное обслуживание"),smp);
    naznach.plus(bros);
  
   }  
 }
if(metkas == 1 && sumad != 0.)
 {
  naznach.plus("  \n"); /*пробелы чтобы курсок при вызове редактора встал на первую строку*/
  sprintf(bros,"%s-%.2fгрн. ",gettext("Сумма без НДС"),sumad-nds);
  naznach.plus(bros);
 }
if(nds > 0.009   )
 {
  naznach.plus("  \n");  
  sprintf(bros,"%s-%.2fгрн.",gettext("НДС"),nds);
//  strcat(MD[1],bros);
  naznach.plus(bros);
 }

int nom=0;
for(nom=0; nom < naznach.kolih(); nom++)
 fprintf(ff2,"%s\n",naznach.ravno(nom));

nom+=pole2(naz_pl.ravno(),'\n');

if(naz_pl.getdlinna() > 1)
 fprintf(ff2,"%s\n",naz_pl.ravno());

for(int nn=nom; nn < 4; nn++)
 fprintf(ff2,"\n");

fprintf(ff2,"\n\
#----------------------------------------------------------------------------\n\
#%s\n",
gettext("Введите назначение платежа и другую необходимую информацию"));

fclose(ff2);
}
/************************************************/
/*Передача платёжек*/
/**********************************************/
void platdok_pp(const char *tabl)
{
class iceb_tu_str script("");
class iceb_tu_str comanda;
class iceb_tu_str parol("");
class iceb_tu_str imafil("");
class iceb_tu_str imamah("");
class iceb_tu_str katalog("");
char bros[1024];
char strsql[1024];

sprintf(strsql,"select datd,nomd from %s where vidpl='1' limit 1",tabl);
if(readkey(strsql) <= 0)
 {
  iceb_t_soob(gettext("Не найдено ни одного документа отмеченного для передчи в банк"));
  return;
 }

int nomerb=0;
if(dirbank(&nomerb,&imamah,&katalog,&imafil,&parol,&comanda,&script) != 0)
  return;

if(script.getdlinna() <= 1)
 script.new_plus("bankcp");

if(nomerb == 1) //Проминвест банк
  paspddos(tabl);
if(nomerb == 2) //Банк Надра
  bnadra(tabl);
if(nomerb == 3) //Банк Аваль
  baval(tabl);
if(nomerb == 4) //Укринбанк
  bukrinbank(tabl);
if(nomerb == 5) //Приватинвест
  b_privatinvest(tabl);
if(nomerb == 6) //Брокбизнесбатк
  b_brokbiznes(tabl);
if(nomerb == 7) //Первый инвестиционный
  b_pervinv(tabl);

if(nomerb == 8) //Укрпромбанк
 {
  
  b_ukrprom(tabl);

  //Узнаём номер дня недели
  struct tm *bf;
  time_t vrem;
  time(&vrem);
  bf=localtime(&vrem);
  strcpy(bros,imafil.ravno());
  sprintf(strsql,"%d",bf->tm_wday+1); //Нумерация дней недели начинаестся с ноля
  //Третий знак в имени файла номер дня недели
  bros[2]=strsql[0];
  imafil.new_plus(bros);

  VVOD VVOD1(2);
 
  VVOD1.VVOD_SPISOK_add_ZAG(gettext("Введите имя файла"));
  int kom1=0;     

  if((kom1=vvod1(&imafil,112,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
   {
    return;
   }
  if(kom1 == ESC)
   {
    return;
   }

       
 }

if(nomerb == 9) //УкрЕсим банк
  b_ukrexim(tabl);

if(nomerb == 10) //Б2 bank on line
  b_b2_bank_on_line(tabl);

if(nomerb == 11) //Укрсоцбанк
  b_ukrsoc(tabl);

if(nomerb == 12) //iBank 2 UA
  b_iBank_2_UA(tabl);

if(nomerb == 13) //Приват Банк
 {
  b_privat(tabl); /*выгрузка в dbf файл*/
#if 0
###################################################33
  if(b_privat(tabl) == 0) /*выгрузка в таблицу базы данных*/
   {
    iceb_t_soob("Платёжки записаны в таблицу JBKL_SND !");
    unlink("plat.txt");

    /*Снимаем метку передачи в банк*/
    sprintf(strsql,"update %s set vidpl='' where vidpl='1'",tabl);
    sql_zapis(strsql,1,0);
   }
  return;
###########################3
#endif      
 }

if(nomerb == 14) //финансы и кредит
  b_soft_review(tabl);

if(nomerb == 15) //система клиент-банк "Tiny"
  b_tiny(tabl);

if(nomerb == 16) /*система клиент-банк АБС "СКАРБ"*/
 {
  b_skarb(tabl);
 }
if(nomerb == 17) /*система клиент-банк Укрбизнесбанк*/
 b_ukrbiznes(tabl);

if(nomerb == 18) /*АКБ "ИМЕКСБАНК"*/
 b_akb_imexbank(tabl);


if(nomerb == 19) /*CORP2*/
 b_corp2(tabl,&imafil);

clear();
GDITE();    
int kodoh=0;

if(comanda.getdlinna() <= 1)
 {
  sprintf(bros,"%s %s %s %s %s %s 2>&1",script.ravno(),imamah.ravno(),"plat.txt",imafil.ravno(),katalog.ravno(),parol.ravno());
  //    kodoh=system(bros);
  kodoh=iceb_t_system(bros,0);
 }
else
 {
  iceb_tu_zvstr(&comanda,"ICEB_FIL",imafil.ravno());
  kodoh=system(comanda.ravno());
 }

class iceb_tu_str repl("");

if(kodoh != 0)
 {
  sprintf(strsql,gettext("Передача файла не произошла ! Код ошибки %d"),kodoh);
  repl.new_plus(strsql);
  if(comanda.getdlinna() <= 1)
    sprintf(strsql,"%d %s %s %s %s %s %s",nomerb,imamah.ravno(),"plat.txt",imafil.ravno(),katalog.ravno(),parol.ravno(),script.ravno());
  else
   strncpy(strsql,comanda.ravno(),sizeof(strsql)-1);
  repl.ps_plus(strsql);
 }
if(kodoh == 0)
 {
  repl.new_plus(gettext("Передача файла произошла успешно"));
  if(comanda.getdlinna() <= 1)
    repl.ps_plus(imafil.ravno());
  
  unlink("plat.txt");

  /*Снимаем метку передачи в банк*/
  sprintf(strsql,"update %s set vidpl='' where vidpl='1'",tabl);
  sql_zapis(strsql,1,0);
 }     

iceb_t_soob(repl.ravno());


}
