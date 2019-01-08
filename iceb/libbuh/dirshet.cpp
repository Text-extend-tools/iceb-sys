/* $Id: dirshet.c,v 5.67 2014/07/31 07:09:27 sasa Exp $ */
/*29.11.2017    18.02.1993      Белых А.И.      dirshet.c
Подпрограмма просмотра плана счетов
Если вернули 0 значит выбрали
*/
#include        <errno.h>
#include	"buhl.h"
class dirshet_poi
 {
  public:
   class iceb_tu_str naim;
   class iceb_tu_str shet;
   class iceb_tu_str kod_subbal;
   short metka_poi; //0-не искать 1-искать
   int vplsh; /*0-двух уровневый план счетов 1-многоуровневый*/
   dirshet_poi()
    {
     clear_data();
     vplsh=0;
    }

   void clear_data()
    {
     metka_poi=0;
     naim.new_plus("");
     shet.new_plus("");
     kod_subbal.new_plus("");
    }
 };
 
void shbh1(int,class dirshet_poi*,int kolshet);
void korshet(const char*,const char*);
int vsn1(const char *shet);
int dirshet_prov(SQL_str row,class dirshet_poi *poir);
void dirshet_vrp(class dirshet_poi *poir);
void dirshet_vksb(const char *shet,int);
void dirshet_ras(int kolstr,class SQLCURSOR *cur,class dirshet_poi *poir);
void dirshet_menu_imp();
void dirshet_uns();


int dirshet(class iceb_tu_str *ns, //Номер счета
class iceb_tu_str *naimshet, //Наименование счета
int vp) //0-ввод и корректировка 1-просмотр с выбором
{
int             x=0,y=2;  /*Позиция вывода списка*/
class iceb_tu_str na(""); /*Наименование счета*/
int             i,prc;
int             kom;
class iceb_tu_str tip("");
class iceb_tu_str vid("");
int             tp;
int             vd;
int             K;
SQL_str         row,row1;
int		kolstr,pozz1;
long    	pozz;
char		strsql[1024];
short		mspz;
class iceb_tu_str bros("");
class iceb_tu_str msald("");
short metka_bs=0;
class dirshet_poi poir;
class iceb_t_mstr spis_menu(0);
class iceb_tu_str kod_subbal("");
class iceb_tu_str blokir("");
int dlinna_nazv=0;
int max_dlinna_nazv=0;
if(iceb_t_poldan("Многопорядковый план счетов",strsql,"nastrb.alx") == 0)
 if(SRAV("Вкл",strsql,1) == 0)
  poir.vplsh=1;

class VVOD DANET(1);
class VVOD VVOD1(2);


kom=pozz=0;

naz1:;
clear();

GDITE();
short           kls=LINES-5; /*Количество строк в меню*/

K=0;
sprintf(strsql,"select * from Plansh order by ns asc");
SQLCURSOR cur;
SQLCURSOR cur1;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,"",strsql);
  return(1);
 }  
if(kolstr == 0)
 {
lll:;
  shbh1(vp,&poir,0);

  class VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  SOOB.VVOD_SPISOK_add_MD(gettext("Вы можете ввести любой план счетов нажимая клавишу F2"));
  SOOB.VVOD_SPISOK_add_MD(gettext("Вы можете импортировать готовый план счетов нажав клавишу F7"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == FK2 || K == PLU || K == FK1 || K == FK4 || K == FK7 || K == FK10 || K == ESC)
    goto vper;

  if(K == KEY_RESIZE)
    goto naz1;
    
  goto lll;
 }
else
 {
  max_dlinna_nazv=0;
  while(cur.read_cursor(&row) != 0)
   {
    if(iceb_tu_strlen(row[1]) > max_dlinna_nazv)
     max_dlinna_nazv=iceb_tu_strlen(row[1]);    
   }
 }
 
naz:;
GDITE();
clear();
if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;

cur.poz_cursor(pozz);
mspz=2;
pozz1=0;

spis_menu.spis_str.free_class();
dlinna_nazv=COLS-40;
if(max_dlinna_nazv < dlinna_nazv)
  dlinna_nazv=max_dlinna_nazv;
  
for(i=0;i<=kls-1;)
 {
  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;

  if(dirshet_prov(row,&poir) != 0)
   continue;
  
  tp=atoi(row[2]);
  vd=atoi(row[3]);
  
  tip.new_plus("");
  if(tp == 0)
    tip.new_plus(gettext("Активный"));
  if(tp == 1)
    tip.new_plus(gettext("Пасивный"));
  if(tp == 2)
    tip.new_plus(gettext("Ак.-пас."));
  if(tp == 3)
    tip.new_plus(gettext("Контрактивный"));
  if(tp == 4)
    tip.new_plus(gettext("Контрпасивный"));

  msald.new_plus("");
  if(row[6][0] == '3')
    msald.new_plus(gettext("Р/с"));

  vid.new_plus("");
  if(row[7][0] == '1')
   vid.plus("*");   
  if(vd == 0)
    vid.plus(gettext("Счет"));
  if(vd == 1)
    vid.plus(gettext("Субс."));

  if(row[9][0] != '0')
   kod_subbal.new_plus(row[9]);
  else
   kod_subbal.new_plus("");

  if(atoi(row[10]) != 0)
   blokir.new_plus("B");
  else
   blokir.new_plus("");
   
  sprintf(strsql,"%-*s|%-*.*s|%1s|%-*.*s|%-*s|%-*.*s|%s",
  iceb_tu_kolbait(6,row[0]),row[0],
  iceb_tu_kolbait(dlinna_nazv,row[1]),iceb_tu_kolbait(dlinna_nazv,row[1]),row[1],
  blokir.ravno(),
  iceb_tu_kolbait(6,vid.ravno()),iceb_tu_kolbait(6,vid.ravno()),vid.ravno(),
  iceb_tu_kolbait(3,msald.ravno()),msald.ravno(),
  iceb_tu_kolbait(8,tip.ravno()),
  iceb_tu_kolbait(8,tip.ravno()),
  tip.ravno(),
  kod_subbal.ravno());

  i++;
  spis_menu.spis_str.plus(strsql);

  if(pozz1 == 1) /*Первая запись удовлетворяет условиям поиска*/
     mspz=1;
 }

if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i == 0)
 {
  if(kolstr > 0 && pozz > 0)
   {
    pozz--;
    goto naz;
   }
  goto lll;
 }
if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

shbh1(vp,&poir,kolstr);

prc=(pozz1+pozz)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,prc,&K,"",0);


if(kom == -7) //Изменение размеров экрана
  goto naz1;
  
if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  for(i=0; i < mspz  ; )
   {
    pozz++;

    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;

    if(dirshet_prov(row,&poir) != 0)
     continue;
    i++;
   }
  kom=kls;
  goto naz;
 }
if(kom == -2) /*Стрелка вверх*/
 {
  for(i=0; i < 1 ; )
   {

    pozz--;

    if(pozz <= 0)
     {
      beep();
      pozz=0;
      goto naz;
     }

    cur.poz_cursor(pozz);
    cur.read_cursor(&row);

    if(dirshet_prov(row,&poir) != 0)
     continue;
    i++;
   }

  goto naz;
 }
 
if(kom == -5) /*Страница вниз*/
 {
  if(pozz + kls == kolstr)
   {
    pozz=kolstr-1;
    beep();
    goto naz;  
   }
  if(i == kls)
    pozz+=pozz1;
  else
    beep();
  if(pozz >= kolstr)
   {
    pozz=kolstr-pozz1;
    beep();
    goto naz;  
   }
  goto naz;

 }

if(kom == -4) /*Страница вверх*/
 {
  for(i=0; i<kls ; )
   {
    pozz--;

    if(pozz <= 0)
     {
      beep();
      pozz=0;
      goto naz;
     }

    cur.poz_cursor(pozz);
    cur.read_cursor(&row);

    if(dirshet_prov(row,&poir) != 0)
     continue;
    i++;
   }
  goto naz;
 }

if(kom >= 0)
 {
  if(SRAV(gettext("Конец списка"),spis_menu.spis_str.ravno(kom),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz; /*Стали на конец списка*/
 
  polen(spis_menu.spis_str.ravno(kom),&bros,1,'|');
  sprintf(strsql,"select * from Plansh where ns='%s'",bros.ravno());
  if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
   {

    sprintf(strsql,gettext("Не найден счёт %s в плане счетов !"),bros.ravno());
    iceb_t_soob(strsql);
    goto naz;
   }
  ns->new_plus(row[0]);
  na.new_plus(row[1]);
  metka_bs=atoi(row[10]);
 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return(1);
  case ENTER : /*Выход*/
     if(vp == 0)
       goto sfk2;          

     if(naimshet != NULL)
       naimshet->new_plus(na.ravno());

     return(0);

  case FK1:   /*Получить подсказку*/
   
   GDITE();
   iceb_t_pdoc("buhg4_1.txt");
   clear();
   goto naz;

  case SFK2:   /*Корректировать*/

sfk2:;
    vsn1(ns->ravno());

    goto naz1;

  case FK2:  /*Ввести */
  case PLU:

    ns->new_plus("");
    vsn1("");
  
    goto naz1;

  case FK3: /*Удалить*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      if(provud("Plansh",ns->ravno(),1) != 0)
        goto naz;
      sprintf(strsql,"delete from Plansh where ns='%s'",ns->ravno());
      if(sql_zap(&bd,strsql) != 0)
       {
       if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Толко чтение
        {
         iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
        }
       else
        msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);
       }
     }
    clear();

    goto naz1;

  case SFK3: /*Удалить не используемые счета*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить не используемые счета? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      goto naz;
    dirshet_uns();
    clear();

    goto naz1;

  case FK4:     //ввод реквизитов поиска
    dirshet_vrp(&poir);

    clear();
    kom=pozz=0;
    if(kolstr == 0)
       goto lll;
    else
       goto naz;

  case FK5: /*Распечатка*/
    dirshet_ras(kolstr,&cur,&poir);
    clear(); /*Очистить экран и закрасить фоновым цветом*/
    goto naz;

  case FK6: //Ввести код суббаланса

    clearstr(LINES-1,0);
    dirshet_vksb(ns->ravno(),poir.vplsh);
    goto naz1;

  case FK7: //Импорт плана счетоа
    clearstr(LINES-1,0);
    dirshet_menu_imp();
    goto naz1;

  case FK8: /*блокировка/разблокировка счёта*/

    sprintf(strsql,"select gkb from icebuser where login='%s'",iceb_tu_getlogin());

    if(readkey(strsql,&row1,&cur1) != 1)
     goto naz1;
    if(atoi(row1[0]) == 1)
     {
      iceb_t_soob(gettext("Блокировать/разблокировать счёт может тот кому разрешена блокировка/разблокировка проводок!"));
      goto naz1;
     }
    if(metka_bs == 0)
     sprintf(strsql,"update Plansh set bs=1 where ns='%s'",ns->ravno_filtr());
    else
     sprintf(strsql,"update Plansh set bs=0 where ns='%s'",ns->ravno_filtr());

    sql_zapis(strsql,0,0);
    goto naz1;
      
  default:
    if(kolstr > 0)
     goto naz;
    else
     goto lll;
 }
}

/****************************/
/*Ввод и корректировка счета*/
/*если вернули 0- записали 1-нет*/
/****************************/

int vsn1(const char *shet)
{
int tips=0;
int vids=0;
int saldo=0;
uid_t ktoi=0;
time_t vremv=0;
int             st;
int             N;
int             K;
class iceb_tu_str shz("");
time_t  	vrem;
char		bros[512];
int		kl,dan;
char		strsql[1024];
class iceb_tu_str naishet("");
short		metkor;
int		saldoz=saldo;
short		status=0,statusz=0;
class VVOD VV(0);
if(shet[0] != '\0') /*корректировка записи*/
 {
  SQL_str row;
  class SQLCURSOR cur;
  sprintf(strsql,"select * from Plansh where ns='%s'",shet);
  if(readkey(strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %s %s!",gettext("Не найден счёт"),shet,gettext("в плане счетов"));
    iceb_t_soob(strsql);
    return(1);
   }   
  naishet.new_plus(row[1]);
  tips=atoi(row[2]);
  vids=atoi(row[3]);  
  ktoi=atoi(row[4]);
  vremv=atol(row[5]);
  saldo=atoi(row[6]);
  status=statusz=atoi(row[7]);
  
 }

VV.VVOD_SPISOK_add_data(shet,24);
VV.VVOD_SPISOK_add_data(naishet.ravno(),255);
VV.VVOD_SPISOK_add_data(32);
VV.VVOD_SPISOK_add_data(32);
VV.VVOD_SPISOK_add_data(32);
VV.VVOD_SPISOK_add_data(32);

if(tips == 0)
  VV.VVOD_SPISOK_zapis_data(2,gettext("Активный"));
if(tips == 1)
  VV.VVOD_SPISOK_zapis_data(2,gettext("Пасивный"));
if(tips == 2)
  VV.VVOD_SPISOK_zapis_data(2,gettext("Активно-пасивный"));
if(tips == 3)
  VV.VVOD_SPISOK_zapis_data(2,gettext("Контрактивный"));
if(tips == 4)
  VV.VVOD_SPISOK_zapis_data(2,gettext("Контрпасивный"));

if(vids == 0)
  VV.VVOD_SPISOK_zapis_data(3,gettext("Счет"));
if(vids == 1)
  VV.VVOD_SPISOK_zapis_data(3,gettext("Субсчёт"));

if(saldo == 0)
  VV.VVOD_SPISOK_zapis_data(4,gettext("Свернутое"));
if(saldo == 3)
  VV.VVOD_SPISOK_zapis_data(4,gettext("Развернутое"));

if(status == 0)
  VV.VVOD_SPISOK_zapis_data(5,gettext("Балансовый")); 
if(status == 1)
  VV.VVOD_SPISOK_zapis_data(5,gettext("Внебалансовый")); 


shz.new_plus(VV.VVOD_SPISOK_return_data(0));

metkor=N=0;

VV.VVOD_SPISOK_add_ZAG(gettext("Ввод и корректировка счетов"));
kzvz(ktoi,vremv,&VV);

VV.VVOD_SPISOK_add_MD(gettext("Номер счета........"));
VV.VVOD_SPISOK_add_MD(gettext("Наименование счета."));
VV.VVOD_SPISOK_add_MD(gettext("Тип счета.........."));
VV.VVOD_SPISOK_add_MD(gettext("Вид счета.........."));
VV.VVOD_SPISOK_add_MD(gettext("Сальдо............."));
VV.VVOD_SPISOK_add_MD(gettext("Статус............."));

ooo:;


helstr(LINES-1,0,"F2/+",gettext("запись"),"F10",
gettext("выход"),NULL);


VV.vramka(-1,-1,0);

attron(VV.VVOD_return_cs(iceb_CFS)); 


VV.vprintw(N,stdscr);

for(;;)
 {
  K=getch();
  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));

  VV.vprintw(N,stdscr);

  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));
    VV.vprintw_clear(N,stdscr);

    if(N == 0)  /*Номер счета*/
     {
      VV.vgetstr(N,stdscr);
      
      /*Убираем пробелы в конце счета*/
      if((kl=strlen(VV.VVOD_SPISOK_return_data(N))) > 0)
      {
       memset(bros,'\0',sizeof(bros));
       strcpy(bros,VV.VVOD_SPISOK_return_data(N));
       for(dan=kl-1; bros[dan] == ' '|| bros[dan] == '\t';dan--)
	bros[dan]='\0';

       VV.VVOD_SPISOK_zapis_data(N,bros);
      }

      N++;
      attroff(VV.VVOD_return_cs(iceb_CFV));
      attron(VV.VVOD_return_cs(iceb_CFM));
      goto ooo;
     }

    if(N == 1)   /*Наименование счета*/
     {
      VV.vgetstr(N,stdscr);
      N++;
      attroff(VV.VVOD_return_cs(iceb_CFV));
      attron(VV.VVOD_return_cs(iceb_CFM));
      goto ooo;
     }

    if(N == 2)   /*Тип счета*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFV));
      attron(VV.VVOD_return_cs(iceb_CFM));

      clearstr(LINES-1,0);

      VVOD MENUW(3);
      MENUW.VVOD_SPISOK_add_ZAG(gettext("Выберите тип счета"));

      MENUW.VVOD_SPISOK_add_MD(gettext("Активный"));
      MENUW.VVOD_SPISOK_add_MD(gettext("Пасивный"));
      MENUW.VVOD_SPISOK_add_MD(gettext("Активно-пасивный"));
      MENUW.VVOD_SPISOK_add_MD(gettext("Контрактивный"));
      MENUW.VVOD_SPISOK_add_MD(gettext("Контрпасивный"));
      st=0;
      if(menu3w(stdscr,&MENUW,&st,-1,-1,0) != 0)
       goto ooo;
      if(st < 0)
       goto ooo;
             
      VV.VVOD_SPISOK_zapis_data(N,MENUW.VVOD_SPISOK_return_MD(st));
      
      tips=st;
      N++;
      attroff(VV.VVOD_return_cs(iceb_CFV));
      attron(VV.VVOD_return_cs(iceb_CFM));
      goto ooo;
     }
    if(N == 3)   /*Вид счета*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFV));
      attron(VV.VVOD_return_cs(iceb_CFM));
      
      clearstr(LINES-1,0);
      VVOD MENUW(3);
      MENUW.VVOD_SPISOK_add_ZAG(gettext("Выберите вид счета"));
      MENUW.VVOD_SPISOK_add_MD(gettext("Счет"));
      MENUW.VVOD_SPISOK_add_MD(gettext("Субсчёт"));
      st=0;
      if(menu3w(stdscr,&MENUW,&st,-1,-1,0) != 0)
       goto ooo;
      if(st < 0)
       goto ooo;

      VV.VVOD_SPISOK_zapis_data(N,MENUW.VVOD_SPISOK_return_MD(st));
      vids=st;
      N++;
      goto ooo;
     }

    if(N == 4)   /*Сальдо*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFV));
      attron(VV.VVOD_return_cs(iceb_CFM));

      clearstr(LINES-1,0);
      VVOD MENUW(3);
      MENUW.VVOD_SPISOK_add_ZAG(gettext("Выберите тип сальдо"));
      MENUW.VVOD_SPISOK_add_MD(gettext("Свернутое"));
      MENUW.VVOD_SPISOK_add_MD(gettext("Развернутое"));
      st=0;
      if(menu3w(stdscr,&MENUW,&st,-1,-1,0) != 0)
       goto ooo;
      if(st < 0)
       goto ooo;

      VV.VVOD_SPISOK_zapis_data(N,MENUW.VVOD_SPISOK_return_MD(st));
      
      saldo=st;
      if(st == 1)
       saldo=3;
      if(saldoz == 0 && saldo == 3 && shz.ravno()[0] != '\0')
       {
        GDITE();
        //Проверяем есть ли проводки по этому счету
        sprintf(strsql,"select distinct sh from Prov where sh='%s' \
and kodkon='' limit 1",shz.ravno());
        if(sql_readkey(&bd,strsql) > 0)
         {
          VVOD SOOB(1);
          SOOB.VVOD_SPISOK_add_MD(gettext("По этому счету уже введены проводки без кода контрагента !"));
          SOOB.VVOD_SPISOK_add_MD(gettext("Сальдо можно развернуть после удаления этих проводок"));
          soobshw(&SOOB,stdscr,-1,-1,0,1);
          saldo=0;
          VV.VVOD_SPISOK_zapis_data(N,gettext("Свернутое"));
         }
       }       
      goto ooo;
     }
    if(N == 5)   /*Статус счета*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFV));
      attron(VV.VVOD_return_cs(iceb_CFM));

      clearstr(LINES-1,0);
      VVOD MENUW(3);
      MENUW.VVOD_SPISOK_add_MD(gettext("Балансовый"));
      MENUW.VVOD_SPISOK_add_MD(gettext("Внебалансовый"));
      MENUW.VVOD_SPISOK_add_ZAG(gettext("Выберите статус счета"));
      st=0;
      if(menu3w(stdscr,&MENUW,&st,-1,-1,0) != 0)
       goto ooo;
      if(st < 0)
       goto ooo;

      VV.VVOD_SPISOK_zapis_data(N,MENUW.VVOD_SPISOK_return_MD(st));
      status=st;
      if(statusz != status)
       {
        GDITE();
        //Проверяем есть ли проводки по этому счету
        sprintf(strsql,"select distinct sh from Prov where sh='%s' limit 1",
        shz.ravno());
        if(sql_readkey(&bd,strsql) > 0)
         {
          VVOD SOOB(1);
          SOOB.VVOD_SPISOK_add_MD(gettext("По этому счету уже введены проводки !"));
          SOOB.VVOD_SPISOK_add_MD(gettext("Статус можно изменить только после удаления всех проводок по этому счету"));
          soobshw(&SOOB,stdscr,-1,-1,0,1);
          if(statusz == 0)
           {
            status=0;
            VV.VVOD_SPISOK_zapis_data(N,gettext("Балансовый"));
           }
          if(statusz == 1)
           {
            status=1;
            VV.VVOD_SPISOK_zapis_data(N,gettext("Внебалансовый"));
           }
         }
       }       
      goto ooo;
     }
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));
   }
  if(K==SDN) /*Стрелка вниз*/
   {
    N++;
   }
  if(K==SUP) /*Стрелка вверх*/
   {
    N--;
   }
  if(N > VV.KLST-1)
   {
    N=0;
   }
  if(N < 0)
   {
    N=VV.KLST-1;
   }
  attron(VV.VVOD_return_cs(iceb_CFS)); 
  VV.vprintw(N,stdscr);

  if(K == FK10)
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    return(1);
   }
  if(K == ESC)
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    N=-1;
    return(1);
   }
  if(K == FK2 || K == PLU) /*Записать*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    
    if(VV.VVOD_SPISOK_return_data(0)[0] == '\0')
     {
      iceb_t_soob(gettext("Не введён номер счета !"));
      goto ooo;
     }

    if(shet[0] == '\0')
     {
      /*Проверяем есть ли такой счёт в плане счетов*/
      sprintf(strsql,"select ns from Plansh where ns='%s'",
      VV.VVOD_SPISOK_return_data(0));

      if(sql_readkey(&bd,strsql) >= 1)
       {
        sprintf(strsql,gettext("Счет %s уже есть в плане счетов !"),VV.VVOD_SPISOK_return_data(0));
        iceb_t_soob(strsql);
        goto ooo;
       }      
     }
    metkor=0;
    if(shet[0] != '\0' && SRAV(VV.VVOD_SPISOK_return_data(0),shz.ravno(),0) != 0)
     {
      /*Проверяем есть ли такой счёт в плане счетов*/
      sprintf(strsql,"select ns from Plansh where ns='%s'",
      VV.VVOD_SPISOK_return_data(0));

      if(sql_readkey(&bd,strsql) >= 1)
       {
        sprintf(strsql,gettext("Счет %s уже есть в плане счетов !"),VV.VVOD_SPISOK_return_data(0));
        iceb_t_soob(strsql);
        goto ooo;
       }      

      VVOD DANET(1);
      DANET.VVOD_SPISOK_add_MD(gettext("Кооректировать номер счета ? Вы уверены !"));

      if(danet(&DANET,2,stdscr) == 2)
        goto ooo;

      metkor=1;
      
     }

    time(&vrem);
    
    naishet.new_plus(VV.VVOD_SPISOK_return_data(1));
        


    if(shet[0] == '\0')        
      sprintf(strsql,"insert into Plansh \
values ('%s','%s',%d,%d,%d,%ld,%d,%d,%d,%d,0)",
      VV.VVOD_SPISOK_return_data(0),naishet.ravno_filtr(),tips,vids,iceb_t_getuid(),
      vrem,saldo,status,0,0);
    else
      sprintf(strsql,"update Plansh \
set \
ns='%s',\
nais='%s',\
tips=%d,\
vids=%d,\
ktoi=%d,\
vrem=%ld,\
saldo=%d,\
stat=%d,\
val=%d \
where ns='%s'",
       VV.VVOD_SPISOK_return_data(0),naishet.ravno_filtr(),tips,vids,iceb_t_getuid(),
       vrem,saldo,status,0,shz.ravno());
/*
    printw("\nstrsql=%s\n",strsql);
    refresh();
    OSTANOV();
*/
    GDITE();
    if(sql_zap(&bd,strsql) != 0)
     {
     if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Толко чтение
      {
       iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
       goto ooo;
      }
     else
      {
       msql_error(&bd,gettext("Ошибка записи !"),strsql);
       goto ooo;
      }
     }
    if(metkor == 1)
      korshet(shz.ravno(),VV.VVOD_SPISOK_return_data(0));

    break;
   }
 }
attroff(VV.VVOD_return_cs(iceb_CFS));

attron(VV.VVOD_return_cs(iceb_CB));
return(0);
}


/********************/
/*Шапка плана счетов*/
/********************/
void shbh1(int vp, //0-ввод и корректировка 1-просмотр с выбором
class dirshet_poi *poir,int kolshet)
{
/*set 11*/
if(vp == 0)
 {
  mvprintw(0,0,gettext("Ввод и корректировка плана счетов"));
  printw("  ");
  if(poir->vplsh == 0)
   printw(gettext("Двухпорядковый"));
  if(poir->vplsh == 1)
   printw(gettext("Многопорядковый"));
  printw(" %d",kolshet);   
 }
if(vp == 1)
  mvprintw(0,0,gettext("Выберите счёт с которым вы будете работать"));
int YY=1;
if(poir->metka_poi == 1)
 {
//  attron(A_BLINK);
  mvprintw(YY++,COLS-20,"%20s",gettext("Поиск:"));
  if(poir->naim.getdlinna() > 1)
   mvprintw(YY++,COLS-20,"%20s",poir->naim.ravno());
  if(poir->shet.getdlinna() > 1)
   mvprintw(YY++,COLS-20,"%20s",poir->shet.ravno());
  if(poir->kod_subbal.getdlinna() > 1)
   mvprintw(YY++,COLS-20,"%20s",poir->kod_subbal.ravno());


//  attroff(A_BLINK);
 }
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F6",gettext("код суббаланса"),
"F7",gettext("импорт"),
"F8",gettext("блокировка"),
"F10",gettext("выход."),NULL);
}
/*******************************************************************/
/*Подпрограмма каскадной корректировки номера счета во всех таблицах*/
/**********************************************************************/

void korshet(const char *sshet, //Старый номер счета
const char *nshet)  //Новый номер счета
{
char		strsql[512];

VVOD SOOBN(1);
SOOBN.VVOD_SPISOK_add_MD(gettext("Выполняется каскадная корректировка номера счета в таблицах !"));
soobsh(&SOOBN,-1,-1,1);

GDITE();

sprintf(strsql,"update Skontr set ns='%s' where ns='%s'",nshet,sshet);

if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Толко чтение
  {
   iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
   return;
  }
 else
  {
   msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
   return;
  }
 }
sprintf(strsql,"update Zarsdo set sh='%s' where sh='%s'",nshet,sshet);
sql_zapis(strsql,1,0);

sprintf(strsql,"update Saldo set ns='%s' where ns='%s'",nshet,sshet);

if(sql_zap(&bd,strsql) != 0)
   msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);


sprintf(strsql,"update Prov set sh='%s' where sh='%s'",nshet,sshet);

if(sql_zap(&bd,strsql) != 0)
   msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

sprintf(strsql,"update Prov set shk='%s' where shk='%s'",nshet,sshet);

if(sql_zap(&bd,strsql) != 0)
   msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

sprintf(strsql,"update Kart set shetu='%s' where shetu='%s'",nshet,sshet);

if(sql_zap(&bd,strsql) != 0)
   msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);


sprintf(strsql,"update Kartb set shet='%s' where shet='%s'",nshet,sshet);

if(sql_zap(&bd,strsql) != 0)
   msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

sprintf(strsql,"update Nash set shet='%s' where shet='%s'",nshet,sshet);

if(sql_zap(&bd,strsql) != 0)
   msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);


sprintf(strsql,"update Uder set shet='%s' where shet='%s'",nshet,sshet);

if(sql_zap(&bd,strsql) != 0)
   msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);


sprintf(strsql,"update Zarp set shet='%s' where shet='%s'",nshet,sshet);

if(sql_zap(&bd,strsql) != 0)
   msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);


sprintf(strsql,"update Zarn set shet='%s' where shet='%s'",nshet,sshet);

if(sql_zap(&bd,strsql) != 0)
   msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

sprintf(strsql,"update Zarn1 set shet='%s' where shet='%s'",nshet,sshet);

if(sql_zap(&bd,strsql) != 0)
   msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

sprintf(strsql,"update Zarsoc set shet='%s' where shet='%s'",nshet,sshet);

if(sql_zap(&bd,strsql) != 0)
   msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

sprintf(strsql,"update Zarsocz set shet='%s' where shet='%s'",nshet,sshet);

if(sql_zap(&bd,strsql) != 0)
   msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

sprintf(strsql,"update Uosinp set shetu='%s' where shetu='%s'",nshet,sshet);

if(sql_zap(&bd,strsql) != 0)
   msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

sprintf(strsql,"update Uslugi set shetu='%s' where shetu='%s'",nshet,sshet);

if(sql_zap(&bd,strsql) != 0)
   msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
sprintf(strsql,"update Usldokum1 set shetu='%s' where shetu='%s'",nshet,sshet);

if(sql_zap(&bd,strsql) != 0)
   msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
sprintf(strsql,"update Usldokum2 set shetu='%s' where shetu='%s'",nshet,sshet);

if(sql_zap(&bd,strsql) != 0)
   msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

sprintf(strsql,"update Kas set shet='%s' where shet='%s'",nshet,sshet);

if(sql_zap(&bd,strsql) != 0)
   msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

sprintf(strsql,"update Kasop1 set shetkas='%s' where shetkas='%s'",nshet,sshet);

if(sql_zap(&bd,strsql) != 0)
   msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

sprintf(strsql,"update Kasop1 set shetkor='%s' where shetkor='%s'",nshet,sshet);

if(sql_zap(&bd,strsql) != 0)
   msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

sprintf(strsql,"update Kasop2 set shetkas='%s' where shetkas='%s'",nshet,sshet);

if(sql_zap(&bd,strsql) != 0)
   msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

sprintf(strsql,"update Kasop2 set shetkor='%s' where shetkor='%s'",nshet,sshet);

if(sql_zap(&bd,strsql) != 0)
   msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

sprintf(strsql,"update Kasord set shetk='%s' where shetk='%s'",nshet,sshet);

if(sql_zap(&bd,strsql) != 0)
   msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

sprintf(strsql,"update Ukrkras set shet='%s' where shet='%s'",nshet,sshet);

if(sql_zap(&bd,strsql) != 0)
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

sprintf(strsql,"update Ukrdok1 set shet='%s' where shet='%s'",nshet,sshet);

if(sql_zap(&bd,strsql) != 0)
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

sprintf(strsql,"update Upldok2 set shet='%s' where shet='%s'",nshet,sshet);

if(sql_zap(&bd,strsql) != 0)
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

sprintf(strsql,"update Uplost set shet='%s' where shet='%s'",nshet,sshet);

if(sql_zap(&bd,strsql) != 0)
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

sprintf(strsql,"update Opldok set shet='%s' where shet='%s'",nshet,sshet);

if(sql_zap(&bd,strsql) != 0)
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

}
/****************************************/
/*Ввод и корректировка реквизитов поиска*/
/****************************************/

void dirshet_vrp(class dirshet_poi *poir)
{

VVOD  VV(0);


VV.VVOD_SPISOK_add_ZAG(gettext("Введите данные для поиска"));

VV.VVOD_SPISOK_add_MD(gettext("Наименование счёта..."));//0
VV.VVOD_SPISOK_add_MD(gettext("Номер счёта.........."));//1
VV.VVOD_SPISOK_add_MD(gettext("Код суббаланса..(,,)."));//2



VV.VVOD_SPISOK_add_data(poir->naim.ravno(),100);//0
VV.VVOD_SPISOK_add_data(poir->shet.ravno(),20);//1
VV.VVOD_SPISOK_add_data(poir->kod_subbal.ravno(),100);//2

naz:;

helstr(LINES-1,0,"F2/+",gettext("поиск"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

int kl=VV.vvod(0,1,1,-1,-1);

poir->naim.new_plus(VV.VVOD_SPISOK_return_data(0));
poir->shet.new_plus(VV.VVOD_SPISOK_return_data(1));
poir->kod_subbal.new_plus(VV.VVOD_SPISOK_return_data(2));

switch(kl)
 {
  case FK10:
  case ESC:
     poir->metka_poi=0;
     break;

  case FK2:
  case PLU:
     poir->metka_poi=1;
     break;

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();
    goto naz;      

  default:
    goto naz;

 }

}
/****************************/
/*Проверка на условия поиска*/
/****************************/

int dirshet_prov(SQL_str row,class dirshet_poi *poir)
{
if(poir->metka_poi == 0)
 return(0);
 
if(poir->naim.getdlinna() > 1)
  if(strstrm(row[1],poir->naim.ravno()) == 0)
   return(1);

if(poir->shet.getdlinna() > 1)
 if(SRAV(poir->shet.ravno(),row[0],1) != 0)
  return(1);
if(proverka(poir->kod_subbal.ravno(),row[9],0,0) != 0)
 return(1);
    
return(0);

}
/****************************/
/*Ввод кода суббаланса для счёта*/
/********************************/

void dirshet_vksb(const char *shet,int vplsh)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
class iceb_tu_str kod("");
class iceb_tu_str naim("");

sprintf(strsql,"select vids,ksb from Plansh where ns like '%s%%' \
order by ns asc",shet);

if(readkey(strsql,&row,&cur) > 1)
 {
   if(vplsh == 0 && atoi(row[0]) == 0) //Двух-порядковый план счетов
    {
     iceb_t_soob(gettext("Счёт имеет субсчета !"));
     return;     
    }

   if(vplsh == 1) //многопорядковый план счетов
    {
     iceb_t_soob(gettext("Счёт имеет субсчета !"));
     return;     
    }
 }
class iceb_tu_str kod_subbal("");
if(atoi(row[1]) != 0)
 kod_subbal.new_plus(row[1]);


VVOD  VVOD1(2);
sprintf(strsql,"%s %s",gettext("Введите код суббаланса для счёта"),shet);
VVOD1.VVOD_SPISOK_add_MD(strsql);

naz:;

helstr(LINES-1,0,
"F3",gettext("код суббаланса"),
"F10",gettext("выход"),NULL);

int kom=0;

kom=vvod1(&kod_subbal,32,&VVOD1,NULL,stdscr,-1,-1);

switch(kom)
 {
  case ENTER:
    break;
  case FK3:
   kod.new_plus("");
   naim.new_plus("");
   
   if(dirsubbal(2,&kod,&naim) == 0)
    kod_subbal.new_plus(kod.ravno());
//   clear();
   goto naz;
   
  case FK10:
   return;
  
  default:
   goto naz;
 }

//проверяем код суббаланса
if(kod_subbal.getdlinna() > 1)
 {
  sprintf(strsql,"select kod from Glksubbal where kod=%s",kod_subbal.ravno());
  if(readkey(strsql) <= 0)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код суббаланса"),kod_subbal.ravno());
    iceb_t_soob(strsql);
    goto naz;
   }
 }
sprintf(strsql,"update Plansh set ksb=%d,vrem=%ld,ktoi=%d where ns='%s'",
kod_subbal.ravno_atoi(),
time(NULL),
iceb_t_getuid(),
shet);
sql_zapis(strsql,0,0);

}
/*********************************/
void dirshet_ras_hap(int nom_str,int *kolstr,FILE *ff)
{
if(kolstr != NULL)
 *kolstr+=5;

fprintf(ff,"%70s %s N%d\n","",gettext("Страница"),nom_str);

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------------------\n");  
fprintf(ff,gettext("\
Номер |   Тип    |Сал|    Вид        |    Статус     |К.с.|Блокировка|                Наименование                      |Дата записи, кто записал\n\
счета |  счета   |ьдо|   счета       |     счета     |    |          |                  счета                           |\n"));

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------------------\n");  
}

/***********************/
/*Распечатка*/
/********************/
void dirshet_ras(int kolstr,class SQLCURSOR *cur,class dirshet_poi *poir)
{
class iceb_tu_str bros("");

FILE *ff;
char imaf[64];

sprintf(imaf,"plsh%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
  {
   error_op_nfil(imaf,errno,"");
   return;
  }

char imaf_eksport[64];

sprintf(imaf_eksport,"ps.csv");

class iceb_fopen fil;
if(fil.start(imaf_eksport,"w") != 0)
 return;

fprintf(fil.ff,"#%s\n#%s %s %s\n",
gettext("Экспорт плана счетов"),
gettext("Выгружено из базы"),
iceb_t_get_namebase(),
iceb_t_get_pnk("00",1));

time_t vrem;
time(&vrem);
struct tm *bf;
bf=localtime(&vrem);

fprintf(fil.ff,"\
#%s %d.%d.%d%s  %s:%02d:%02d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(fil.ff,"#Расшифровка полей записи:\n\
#1  Номер счета\n\
#2  Наименование счета\n\
#3  0-активный 1-пассивный 2-активно-пассивный\n\
#4  0-счет 1-субсчет\n\
#5  сальдо 0-свернутое 3-развернутое\n\
#6  0-балансовый счёт 1-вне балансовый счет\n\
#7  0-национальная валюта или номер валюты из справочника валют (пока не задействовано)\n\
#8  код суббаланса\n");



iceb_t_zagolov(gettext("План счетов"),ff);

if(poir->vplsh == 0)
 bros.new_plus(gettext("Двухпорядковый"));
if(poir->vplsh == 1)
 bros.new_plus(gettext("Многопорядковый"));
fprintf(ff,"%s\n",bros.ravno());

dirshet_ras_hap(1,NULL,ff);

int kolstr1=0;

cur->poz_cursor(0);
if(poir->kod_subbal.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Код суббалансу"),poir->kod_subbal.ravno());

SQL_str row;
int             tipsh; /*Тип счета*/
int             vidsh; /*Вид счета*/
class iceb_tu_str tip("");
class iceb_tu_str vid("");
class iceb_tu_str msald("");
class iceb_tu_str kod_subbal("");
class iceb_tu_str blokir("");
while(cur->read_cursor(&row) != 0)
 {
    strzag(LINES-1,0,kolstr,++kolstr1);
    if(dirshet_prov(row,poir) != 0)
     continue;

    tipsh=atoi(row[2]);
    vidsh=atoi(row[3]);

    tip.new_plus("");
    if(tipsh == 0)
      tip.new_plus(gettext("Активный"));
    if(tipsh == 1)
      tip.new_plus(gettext("Пасивный"));
    if(tipsh == 2)
      tip.new_plus(gettext("Ак.-пас."));
    if(tipsh == 3)
      tip.new_plus(gettext("Контрактивный"));
    if(tipsh == 4)
      tip.new_plus(gettext("Контрпасивный"));

    msald.new_plus("");
    if(row[6][0] == '3')
      msald.new_plus(gettext("Р/с"));

    vid.new_plus("");
    if(vidsh == 0)
      vid.new_plus(gettext("Счет"));
    if(vidsh == 1)
      vid.new_plus(gettext("Субсчёт"));

    if(row[7][0] == '0')
      bros.new_plus(gettext("Балансовый"));
    if(row[7][0] == '1')
      bros.new_plus(gettext("Внебалансовый"));

    if(atoi(row[9]) > 0)
     kod_subbal.new_plus(row[9]);
    else
     kod_subbal.new_plus("");
     
    if(atoi(row[10]) == 1)
     blokir.new_plus(gettext("Заблокирован"));
    else
     blokir.new_plus("");

    fprintf(ff,"%-*s|%-*s|%-*s|%-*s|%-*s|%-*s|%-*.*s|%-*.*s|%s %s\n",
    iceb_tu_kolbait(6,row[0]),row[0],
    iceb_tu_kolbait(10,vid.ravno()),vid.ravno(),
    iceb_tu_kolbait(3,msald.ravno()),msald.ravno(),
    iceb_tu_kolbait(15,tip.ravno()),tip.ravno(),
    iceb_tu_kolbait(15,bros.ravno()),bros.ravno(),
    iceb_tu_kolbait(4,kod_subbal.ravno()),kod_subbal.ravno(),
    iceb_tu_kolbait(10,blokir.ravno()),
    iceb_tu_kolbait(10,blokir.ravno()),
    blokir.ravno(),
    iceb_tu_kolbait(50,row[1]),
    iceb_tu_kolbait(50,row[1]),
    row[1],
    iceb_tu_vremzap(row[5]),iceb_t_kszap(row[4]));
    
    for(int nomer=50; nomer < iceb_tu_strlen(row[1]);nomer+=50)
     fprintf(ff,"%6s|%10s|%3s|%15s|%15s|%4s|%10s|%-*.*s|\n",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     iceb_tu_kolbait(50,iceb_tu_adrsimv(nomer,row[1])),
     iceb_tu_kolbait(50,iceb_tu_adrsimv(nomer,row[1])),
     iceb_tu_adrsimv(nomer,row[1]));

    fprintf(fil.ff,"%s|%s|%s|%s|%s|%s|%s|%s|\n",
    row[0],row[1],row[2],row[3],row[6],row[7],row[8],row[9]);
    
 }
fprintf(ff,"ICEB_LST_END\n\
-------------------------------------------------------------------------------------------------------------------------------------------------\n");  
fprintf(ff,"%s: %d\n",gettext("Количество счетов"),kolstr);
podpis(ff);
fclose(ff);

fil.end();

class spis_oth oth;

oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка плана счетов"));

int orient=iceb_t_ustpeh(imaf,3);
iceb_t_rnl(imaf,orient,NULL,&dirshet_ras_hap);

oth.spis_imaf.plus(imaf_eksport);
oth.spis_naim.plus(gettext("Файл для экспорта плана счетов в другую базу данных"));


iceb_t_rabfil(&oth,"");

}
/*******************************************/
/*Импорт плана счетов из файла*/
/******************************************/
void dirshet_import()
{

class iceb_tu_str imaf_imp("");

if(iceb_t_menu_import(&imaf_imp,gettext("Импорт плана счетов"),"","buhg4_1_1.txt") != 0)
 return;
class iceb_tu_str stroka("");
char strsql[1024];

FILE *ff;

if((ff = fopen(imaf_imp.ravno(),"r")) == NULL)
 {
  error_op_nfil(imaf_imp.ravno(),errno,"");
  return;
 }

class iceb_tu_str nomer_sheta("");
class iceb_tu_str naim_sheta("");
int tips=0;
int vids=0;
int saldo=0;
int stat=0;
int val=0;
int kodsubbal=0;
int kolih_zag_zap=0;
clear();
while(iceb_tu_fgets(&stroka,ff) == 0)
 {
  if(stroka.ravno()[0] == '#')
   continue;
     
  if(pole2(stroka.ravno(),'|') < 8) 
   continue;

  polen(stroka.ravno(),&nomer_sheta,1,'|');

  if(nomer_sheta.getdlinna() <= 1)
   continue;

  /*проверяем может уже такой номер есть в плане счетов*/
  sprintf(strsql,"select ns from Plansh where ns='%s'",nomer_sheta.ravno());
  if(readkey(strsql) > 0)
   {
    sprintf(strsql,"%s %s",gettext("В плане счетов уже есть счёт"),nomer_sheta.ravno());
    iceb_t_soob(strsql);
    continue;
   }
  polen(stroka.ravno(),&naim_sheta,2,'|');
  polen(stroka.ravno(),&tips,3,'|');
  polen(stroka.ravno(),&vids,4,'|');
  polen(stroka.ravno(),&saldo,5,'|');
  polen(stroka.ravno(),&stat,6,'|');
  polen(stroka.ravno(),&val,7,'|');
  polen(stroka.ravno(),&kodsubbal,7,'|');
  
  /*проверяем код суббаланса*/
  if(kodsubbal != 0)
   {
    sprintf(strsql,"select kod from Glksubbal where kod=%d",kodsubbal);
    if(readkey(strsql) != 1)
     kodsubbal=0;
   }
  printw("%*s %s\n",iceb_tu_kolbait(6,nomer_sheta.ravno()),nomer_sheta.ravno(),naim_sheta.ravno());
  
  sprintf(strsql,"insert into Plansh (ns,nais,tips,vids,ktoi,vrem,saldo,stat,val,ksb) \
values('%s','%s',%d,%d,%d,%ld,%d,%d,%d,%d)",
  nomer_sheta.ravno(),
  naim_sheta.ravno_filtr(),
  tips,
  vids,
  iceb_t_getuid(),
  time(NULL),
  saldo,
  stat,
  val,
  kodsubbal);
  
  if(sql_zapis(strsql,1,0) == 0)
   kolih_zag_zap++;
  
 }

fclose(ff);

sprintf(strsql,"%s %d",gettext("Количество загруженых записей"),kolih_zag_zap);
iceb_t_soob(strsql);

}
/*******************************************/
/*Импорт плана счетов из базы*/
/******************************************/
void dirshet_importb(const char *imaf)
{
SQL_str row;
class SQLCURSOR cur;
int kolstr=0;
char strsql[1024];
sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,"",strsql);
  return;
 }  
if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найден план счетов в базе!"));
  return;
 }
class iceb_tu_str nomer_sheta("");
class iceb_tu_str naim_sheta("");
int tips=0;
int vids=0;
int saldo=0;
int stat=0;
int val=0;
int kodsubbal=0;
int kolih_zag_zap=0;
clear();
int kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  
  strzag(LINES-1,0,kolstr,++kolstr1);
  
  if(pole2(row[0],'|') < 8) 
   continue;

  polen(row[0],&nomer_sheta,1,'|');

  if(nomer_sheta.getdlinna() <= 1)
   continue;

  /*проверяем может уже такой номер есть в плане счетов*/
  sprintf(strsql,"select ns from Plansh where ns='%s'",nomer_sheta.ravno());
  if(readkey(strsql) > 0)
   {
    printw("%s %s\n",gettext("В плане счетов уже есть счёт"),nomer_sheta.ravno());
    continue;
   }
  polen(row[0],&naim_sheta,2,'|');
  polen(row[0],&tips,3,'|');
  polen(row[0],&vids,4,'|');
  polen(row[0],&saldo,5,'|');
  polen(row[0],&stat,6,'|');
  polen(row[0],&val,7,'|');
  polen(row[0],&kodsubbal,7,'|');

  sprintf(strsql,"insert into Plansh (ns,nais,tips,vids,ktoi,vrem,saldo,stat,val,ksb) \
values('%s','%s',%d,%d,%d,%ld,%d,%d,%d,%d)",
  nomer_sheta.ravno(),
  naim_sheta.ravno_filtr(),
  tips,
  vids,
  iceb_t_getuid(),
  time(NULL),
  saldo,
  stat,
  val,
  kodsubbal);
  
  if(sql_zapis(strsql,1,0) == 0)
   kolih_zag_zap++;
  
 }


sprintf(strsql,"%s %d",gettext("Количество загруженых записей"),kolih_zag_zap);
iceb_t_soob(strsql);

}
/***************************/
/*меню выбора режима импорта*/
/**************************/
void dirshet_menu_imp()
{
int kom=0;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное"));

MENU.VVOD_SPISOK_add_MD(gettext("Импорт плана счетов для хозрасчётных организаций Украины"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Импорт плана счетов для бюджетных организаций Украины"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Импорт плана счетов для банков Украины"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Импорт плана счетов для хозрасчётных организаций России"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Импорт плана счетов из файла"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//5

while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0)

class spis_oth oth;   
switch (kom)
 {
  case 5:
  case -1 :
    return;

  case 0 :
    dirshet_importb("plansh_hz_ua.alx");
    break;

  case 1 :
    dirshet_importb("plansh_bd_ua.alx");
    break;

  case 2 :
    dirshet_importb("plansh_bn_ua.alx");
    break;

  case 3 :
    dirshet_importb("plansh_hz_ru.alx");
    break;

  case 4 :
    dirshet_import();
    break;

 }
 
}
/***********************************/
/*Удаление не используемых счетов*/
/***********************************/
void dirshet_uns()
{
SQL_str row;
class SQLCURSOR cur;
int kolstr=0;
char strsql[1024];

sprintf(strsql,"select ns,nais from Plansh order by ns asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,"",strsql);
  return;
 }  
if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найден план счетов в базе!"));
  return;
 }
int kolud=0;
clear();
int kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(provud("Plansh",row[0],0) != 0)
    continue;

  printw("%*s %s\n",iceb_tu_kolbait(6,row[0]),row[0],row[1]);

  sprintf(strsql,"delete from Plansh where ns='%s'",row[0]);
  if(sql_zap(&bd,strsql) != 0)
   {
    if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Толко чтение
     {
      iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
      return;
     }
    else
     msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);
    continue;
   }
  kolud++;
 }
sprintf(strsql,"%s %d",gettext("Количество удалённых счетов"),kolud);
iceb_t_soob(strsql);

}
