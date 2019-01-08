/* $Id: dirskont.c,v 5.56 2014/07/31 07:09:27 sasa Exp $ */
/*15.02.2016    14.09.1993      Белых А.И.      dirskont.c
Подпрограмма просмотра по счёту кодов организации
Если вернули 0 значит выбрали
*/
#include        <errno.h>
#include        "buhl.h"

class dirskontr_rp
 {
  public:
    /*Реквизиты поиска*/
    class iceb_tu_str naim_kont; /*Наименование организации*/
    class iceb_tu_str gorod; /*Город организации*/
    class iceb_tu_str gorodb; /*Город банка*/
    class iceb_tu_str kod;   /*Код*/
    class iceb_tu_str naimb; /*Наименование банка*/
    class iceb_tu_str mfo;   /*МФО*/
    class iceb_tu_str nsh;   /*Номер счета*/
    class iceb_tu_str inn;   /*Индивидуальный налоговый номер*/
    class iceb_tu_str kodgrkon; /*Код группы контрагента*/
    short metpoi; //0-без поиска 1-с поиском

    /*Нужные переменные*/
    class iceb_tu_str nm_kontr;
      
  dirskontr_rp()
   {
    clear();
    nm_kontr.plus("");
   }
  
  void clear()
   {
    metpoi=0;
    naim_kont.new_plus("");
    gorod.new_plus("");
    gorodb.new_plus("");
    kod.new_plus("");
    naimb.new_plus("");
    mfo.new_plus("");
    nsh.new_plus("");
    inn.new_plus("");
    kodgrkon.new_plus("");
   }
 };

void dirskont_copy(const char *shet);

void            shbh3(const char*,int);
void		udnkks(const char*,SQLCURSOR*,long);
int dirskont_prov(char *kod_kontr,class dirskontr_rp *rpoi);
void dirskont_rs(class SQLCURSOR *cur,int kolstr,class dirskontr_rp *rpoi);


int dirskont(const char *sht,/*Счет для которого нужен список организаций*/
class iceb_tu_str *kodo, /*Код организации*/
class iceb_tu_str *nai, /*Наименование выбранной организации*/
int metk) /*0-ввод и просмотр 1-выбор*/
{
class iceb_tu_str kodkontr("");
class dirskontr_rp rpoi;
char            bros[512];
short           x=0,y=2;  /*Позиция вывода списка*/
time_t          tmm;
int             i,i1;
int             kom;
int             K,kl;
double          db,kr;
char            imaf[64];
char            imafact[64];
char            imaf_act[64];
char            imaf_act_bi[64];
class iceb_tu_str koz("");
short           d,m,g,dk,mk,gk;
class iceb_tu_str shsv("");
short           par;
static iceb_tu_str dat1("");
static iceb_tu_str dat2("");
class iceb_tu_str naior("");
class iceb_tu_str kodoz("");
short		mkr=0;
long		kolstr;
long		pozz,pozz1;
SQL_str         row,row1;
char		strsql[1024];
time_t		vr;
class iceb_tu_str kodvk("");
short		mspz; /*метка соответствия первой записи*/
int		Y,X;
int		maxkz; //Максимальное количество знаков в коде контрагента с лидирующимим нолями
int kod_start=1;
struct OPSHET	shetv;
class iceb_t_mstr spis_menu(0);


VVOD	VV(0);
VVOD	VVOD1(2);
VVOD	DANET(1);


rpoi.naim_kont.new_plus(nai->ravno());
if(nai->getdlinna() > 1)
 rpoi.metpoi=1;


memset(bros,'\0',sizeof(bros));
iceb_t_poldan("Максимальное количество знаков в коде контрагента с лидирующими нолями",bros,"nastrb.alx");
maxkz=atoi(bros);

pozz=kom=K=0;

naz1:;
GDITE();

short           kls=LINES-5; /*Количество строк в меню*/

sprintf(strsql,"select * from Skontr where ns='%s' order by kodkon asc",sht);

SQLCURSOR cur;
SQLCURSOR cur1;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,"",strsql);
  return(1);
 }  

if(kolstr == 0)
 {

kkkk:;

  shbh3(sht,kolstr);
  
  beep();
  if(rpoi.naim_kont.ravno()[0] != '\0')
   {

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Не найдено записeй по заказанному наименованию !"));
    DANET.VVOD_SPISOK_add_MD(gettext("      Будете искать в общем списке ?"));
    if(danet(&DANET,2,stdscr) == 1)
     {
      K=SFK8;
      goto vper;
     }
    rpoi.naim_kont.new_plus("");
    rpoi.metpoi=0;
    goto naz1;
   }

  if(rpoi.nsh.ravno()[0] != '\0' || rpoi.mfo.ravno()[0] != '\0' || rpoi.gorodb.ravno()[0] != '\0' || \
    rpoi.gorod.ravno()[0] != '\0' || rpoi.kod.ravno()[0] != '\0' || rpoi.naimb.ravno()[0] != '\0' || \
    rpoi.inn.ravno()[0] != '\0' || rpoi.kodgrkon.ravno()[0] != '\0')
   {
    iceb_t_soob(gettext("Не найдено записей по заданым реквизитам поиска !"));

    rpoi.clear();
    
    goto naz1;
   }
  else
   {
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
    K=soobshw(&SOOB,stdscr,-1,-1,1,1);
   }

  if(K == SFK2 || K == ENTER || K == FK3 || K == FK6 || K == FK4 || \
  K == FK7 || K == FK5)
    goto kkkk;

  if(K == KEY_RESIZE)
    goto naz1;
    
  goto vper;
 }




if(rpoi.naim_kont.ravno()[0] == '\0' && kodvk.getdlinna() > 1 && kolstr > kls && pozz < kolstr-kls)
 {
  pozz=poivcur(kodvk.ravno(),1,&cur);
  kom=0;
  kodvk.new_plus("");
 }

naz:;
GDITE();


if(kolstr != 0 && pozz >= kolstr)
  pozz = kolstr-1;

cur.poz_cursor(pozz);
mspz=2;
pozz1=0;
spis_menu.spis_str.free_class();
for(i=0;i < kls;)
 {
  if(cur.read_cursor(&row) == 0)
    break;

  pozz1++;
  if(dirskont_prov(row[1],&rpoi) != 0)
   continue;

  sprintf(strsql,"%-*s| %s",iceb_tu_kolbait(10,row[1]),row[1],rpoi.nm_kontr.ravno());

  i++;
  spis_menu.spis_str.plus(strsql);

  if(pozz1 == 1) /*Первая запись удовлетворяет условиям поиска*/
   mspz=1;

 }

if(i == 0)
 {
  if(kolstr > 0 && pozz > 0)
   {
    pozz--;
    goto naz;
   }
  goto kkkk;
 }
if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

shbh3(sht,kolstr);
if(rpoi.metpoi == 1)
 {
  attron(A_BLINK);
  move(Y=5,X=COLS-20);
  printw(gettext("Поиск !"));
  if(rpoi.naim_kont.ravno()[0] != '\0')
   {
    move(++Y,X);
    printw(gettext("Наи.-%s"),rpoi.naim_kont.ravno());
   }
  if(rpoi.nsh.ravno()[0] != '\0')
   {
    move(++Y,X);
    printw(gettext("Сче.-%s"),rpoi.nsh.ravno());
   }
  if(rpoi.mfo.ravno()[0] != '\0')
   {
    move(++Y,X);
    printw(gettext("МФО -%s"),rpoi.mfo.ravno());
   }
  if(rpoi.gorod.ravno()[0] != '\0')
   {
    move(++Y,X);
    printw(gettext("Гор.-%s"),rpoi.gorod.ravno());
   }
  if(rpoi.kod.ravno()[0] != '\0')
   {
    move(++Y,X);
    printw(gettext("Код -%s"),rpoi.kod.ravno());
   }
  if(rpoi.naimb.ravno()[0] != '\0')
   {
    move(++Y,X);
    printw(gettext("Н.б.-%s"),rpoi.naimb.ravno());
   }
  if(rpoi.inn.ravno()[0] != '\0')
   {
    move(++Y,X);
    printw(gettext("ИНН -%s"),rpoi.inn.ravno());
   }
  if(rpoi.gorodb.ravno()[0] != '\0')
   {
    move(++Y,X);
    printw(gettext("Г.б.-%s"),rpoi.gorodb.ravno());
   }
  if(rpoi.kodgrkon.ravno()[0] != '\0')
   {
    move(++Y,X);
    printw(gettext("К.г.-%s"),rpoi.kodgrkon.ravno());
   }

  attroff(A_BLINK);
 }


i1=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,i1,&K,"",0);


if(kom == -7)  //Изменение размеров экрана
  goto naz1;

if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }

  for(i=0; i<mspz  ; )
   {
    pozz++;
    if(pozz >= kolstr)
     {
      pozz=kolstr-1;
      break;
     }

    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;

    if(dirskont_prov(row[1],&rpoi) != 0)
     continue;
    i++;
   }
  kom=kls-1;
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

    if(dirskont_prov(row[1],&rpoi) != 0)
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
/*  
  printw("\n вперед pozz=%d\n",pozz);
  OSTANOV();
*/
  goto naz;

 }

if(kom == -4) /*Страница вверх*/
 {
/*
  printw("\npozz=%d\n",pozz);
  refresh();
*/
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
    if(cur.read_cursor(&row) == 0)
      break;

    if(dirskont_prov(row[1],&rpoi) != 0)
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

  polen(spis_menu.spis_str.ravno(kom),kodo,1,'|');

  kodoz.new_plus(kodo->ravno());
  polen(spis_menu.spis_str.ravno(kom),nai,2,'|');
 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return(1);

  case ENTER : /*Выход*/
     if(metk == 0)
      goto sfk2;
      
     return(0);

  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("buhg4_3.txt");
   clear();
   if(kolstr > 0)
      goto naz;
   else
      goto kkkk;

  case SFK2:   /*Корректировать*/

sfk2:;
    vkontr(sht,&kodoz,1);
    goto naz1;

  case FK2:
  case PLU:

    kodkontr.new_plus("");
nazv:;

    helstr(LINES-1,0,"Enter",gettext("получение кода контрагента"),
    "F10",gettext("выход"),NULL);


    VVOD1.VVOD_delete();
    VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код контрагента"));
         
    if(vvod1(&kodkontr,24,&VVOD1,NULL,stdscr,-1,-1) == FK10)
      goto naz;


    if(kodkontr.getdlinna() <= 1)
     {
      GDITE();

      if(maxkz == 0)
       {
        kodkontr.new_plus(iceb_t_getnkontr(kod_start));  /*dirkontr*/
        kod_start=kodkontr.ravno_atoi();
       }
      else
        {
         sprintf(strsql,"%0*d",maxkz,iceb_t_getnkontr(1));
         kodkontr.new_plus(strsql);
        }

      goto nazv;
     }
    mkr=0;
    clearstr(22,0);
    kodvk.new_plus(kodkontr.ravno());
    //Читаем наименование организации
    sprintf(bros,"select * from Kontragent where kodkon='%s'",kodkontr.ravno());
    
    if(readkey(bros) == 1)
     {
      mkr=1;
     }
    if(vkontr(sht,&kodvk,mkr) != 0)
     kodvk.new_plus("");
    rpoi.naim_kont.new_plus("");
    goto naz1;

  case FK3:

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));
    if(danet(&DANET,2,stdscr) == 1)
     {
      sprintf(bros,"Skontr#%s",sht);
      if(provud(bros,kodo->ravno(),1) != 0)
        goto naz;
      sprintf(strsql,"delete from Skontr where ns='%s' and kodkon ='%s'",
      sht,kodo->ravno());
/*      printw("\nstrsql=%s\n",strsql);*/
      if(sql_zap(&bd,strsql) != 0)
       {
       if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Толко чтение
        {
         VVOD SOOB(1);
         SOOB.VVOD_SPISOK_add_MD(gettext("У вас нет полномочий для выполнения этой операции !"));
         soobshw(&SOOB,stdscr,-1,-1,0,1);
        }
       else
        msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);
       }
      clear();
      goto naz1;
     }
    goto naz;
  
  case SFK3: /*Удалить не используемые коды*/
    udnkks(sht,&cur,kolstr);
    goto naz1;
      
  case FK4:       /*Отобрать по образцу*/

    VV.VVOD_delete();

    VV.VVOD_SPISOK_add_ZAG(gettext("Введите данные для поиска контрагента"));

    VV.VVOD_SPISOK_add_MD(gettext("Наименование контрагента..."));//0
    VV.VVOD_SPISOK_add_MD(gettext("Номер счета................"));//1
    VV.VVOD_SPISOK_add_MD(gettext("МФО........................"));//2
    VV.VVOD_SPISOK_add_MD(gettext("Город (контрагента)........"));//3
    VV.VVOD_SPISOK_add_MD(gettext("Код........................"));//4
    VV.VVOD_SPISOK_add_MD(gettext("Наименование банка........."));//5
    VV.VVOD_SPISOK_add_MD(gettext("Индивидуальный нал. номер.."));//6
    VV.VVOD_SPISOK_add_MD(gettext("Город (банка).............."));//7
    VV.VVOD_SPISOK_add_MD(gettext("Код группы контрагента....."));//8

    VV.VVOD_SPISOK_add_data(rpoi.naim_kont.ravno(),112);//0
    VV.VVOD_SPISOK_add_data(rpoi.nsh.ravno(),24);//1
    VV.VVOD_SPISOK_add_data(rpoi.mfo.ravno(),16);//2
    VV.VVOD_SPISOK_add_data(rpoi.gorod.ravno(),112);//3
    VV.VVOD_SPISOK_add_data(rpoi.kod.ravno(),26);//4
    VV.VVOD_SPISOK_add_data(rpoi.naimb.ravno(),112);//5
    VV.VVOD_SPISOK_add_data(rpoi.inn.ravno(),16);//6
    VV.VVOD_SPISOK_add_data(rpoi.gorodb.ravno(),112);//7
    VV.VVOD_SPISOK_add_data(rpoi.kodgrkon.ravno(),24);//8

fk4:;

    rpoi.metpoi=0;


    helstr(LINES-1,0,"F2/+",gettext("поиск"),
    "F4",gettext("очистить"),
    "F10",gettext("выход"),NULL);

    kl=VV.vvod(0,1,1,-1,-1);
    switch(kl)
     {
      case FK10:
      case ESC:
         break;
      case FK2:
      case PLU:
        GDITE();
        pozz=0;
        rpoi.naim_kont.new_plus(VV.VVOD_SPISOK_return_data(0));
        rpoi.nsh.new_plus(VV.VVOD_SPISOK_return_data(1));
        rpoi.mfo.new_plus(VV.VVOD_SPISOK_return_data(2));
        rpoi.gorod.new_plus(VV.VVOD_SPISOK_return_data(3));
        rpoi.kod.new_plus(VV.VVOD_SPISOK_return_data(4));
        rpoi.naimb.new_plus(VV.VVOD_SPISOK_return_data(5));
        rpoi.inn.new_plus(VV.VVOD_SPISOK_return_data(6));
        rpoi.gorodb.new_plus(VV.VVOD_SPISOK_return_data(7));
        rpoi.kodgrkon.new_plus(VV.VVOD_SPISOK_return_data(8));

        if(rpoi.naim_kont.ravno()[0] != '\0' || rpoi.nsh.ravno()[0] != '\0' || rpoi.mfo.ravno()[0] != '\0' || rpoi.gorodb.ravno()[0] != '\0' ||\
         rpoi.gorod.ravno()[0] != '\0' || rpoi.kod.ravno()[0] != '\0' || rpoi.naimb.ravno()[0] != '\0' || rpoi.inn.ravno()[0] != '\0'\
         || rpoi.kodgrkon.ravno()[0] != '\0')
          rpoi.metpoi=1;

        break;
      
      case FK4: /*Очистить меню*/
        VV.VVOD_clear_data();
        goto fk4;      

      default:
        goto fk4;
     }
    kom=pozz=0;
    goto naz;

  case FK5:       /*Распечатать*/
    dirskont_rs(&cur,kolstr,&rpoi);
    clear();
    cur.poz_cursor(pozz);
    goto naz;


  case FK6:       /*Стать на номер */
    kom=0;
    rpoi.clear();
    
    helstr(LINES-1,0,"F10",gettext("выход"),NULL);


    kodoz.new_plus("");
     
    VVOD1.VVOD_delete();
    VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код контрагента"));
         
    if(vvod1(&kodoz,16,&VVOD1,NULL,stdscr,-1,-1) == FK10)
      goto naz;

    kom=pozz=0;
    pozz=poivcur(kodoz.ravno(),1,&cur);
    if(pozz == kolstr)
     {
      pozz=0;
      sprintf(strsql,"%s %s !",gettext("Не найдено код контрагента"),kodoz.ravno());
      iceb_t_soob(strsql);
     }
    goto naz;
  
  case FK7:       /*Получить сальдо по организации*/

    clear();

    VV.VVOD_delete();

    sprintf(strsql,"%s %s",gettext("Счет"),sht);
    VV.VVOD_SPISOK_add_ZAG(strsql);

    sprintf(strsql,"%s:",gettext("Получение сальдо для"));
    VV.VVOD_SPISOK_add_ZAG(strsql);

    sprintf(strsql,"%s %s",kodo->ravno(),nai->ravno());
    VV.VVOD_SPISOK_add_ZAG(strsql);

    VV.VVOD_SPISOK_add_MD(gettext("Начальная дата..."));
    VV.VVOD_SPISOK_add_MD(gettext("Конечная дата...."));
    VV.VVOD_SPISOK_add_MD(gettext("Счет для сверки.."));

    VV.VVOD_SPISOK_add_data(11);
    VV.VVOD_SPISOK_add_data(11);
    VV.VVOD_SPISOK_add_data(20);

    short    dt,mt,gt;
    poltekdat(&dt,&mt,&gt);
    if(dat1.getdlinna() <= 1)
     {
      sprintf(strsql,"01.01.%d",gt);
      dat1.new_plus(strsql);
     }
    if(dat2.getdlinna() <= 1)
     {
      sprintf(strsql,"%02d.%02d.%d",dt,mt,gt);
      dat2.new_plus(strsql);
     }
    VV.VVOD_SPISOK_zapis_data(0,dat1.ravno());
    VV.VVOD_SPISOK_zapis_data(1,dat2.ravno());

  nazz:;

    move(LINES-3,0);
    printw(gettext("Счет для сверки вводиться при необходимости свериться с этим счетом"));

    helstr(LINES-1,0,"F2/+",gettext("расчитать"),
    "F10",gettext("выход"),NULL);

    par=VV.vvod(0,1,1,-1,-1);

    dat1.new_plus(VV.VVOD_SPISOK_return_data(0));
    dat2.new_plus(VV.VVOD_SPISOK_return_data(1));

    if(par == FK2 || par == PLU)   /*Расчёт*/
     {
      if(rsdatp(&d,&m,&g,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
        goto naz;

       shsv.new_plus(VV.VVOD_SPISOK_return_data(2));
       if(shsv.getdlinna() > 1)
        if(prsh(shsv.ravno(),&shetv) == 0)
         {
          sprintf(strsql,gettext("Нет счета %s в плане счетов !"),shsv.ravno());
          iceb_t_soob(strsql);
  	  goto nazz;
         }

       if(shsv.ravno()[0] != '\0')
        {

         par=0;
         DANET.VVOD_delete();
         DANET.VVOD_SPISOK_add_MD(gettext("Встречные проводки учесть ?"));
         if(danet(&DANET,2,stdscr) == 1)
          par=1;
        }
       time(&tmm);
       clear();
       move(10,0);
       printw("%s %s %s\n\
%s %s %s %d.%d.%d%s %s %d.%d.%d%s\n",
       gettext("Получение сальдо для"),
       kodo->ravno(),nai->ravno(),
       gettext("Счет"),
       sht,
       gettext("Период с"),
       d,m,g,
       gettext("г."),
       gettext("до"),
       dk,mk,gk,
       gettext("г."));

       if(shsv.ravno()[0] != '\0')
         printw("%s %s\n",
         gettext("Счет для сверки"),
         shsv.ravno());
       GDITE();
       salork(sht,kodo->ravno(),&db,&kr,1,imaf,imafact,nai->ravno(),d,m,g,dk,mk,gk,shsv.ravno(),par,imaf_act,imaf_act_bi);
       
       printw_vr(tmm);

       OSTANOV();

       class spis_oth oth;
       
       oth.spis_imaf.plus(imaf);
       oth.spis_naim.plus(gettext("Распечатка проводок по контрагенту"));
       oth.spis_imaf.plus(imafact);
       oth.spis_naim.plus(gettext("Акт сверки по контрагенту"));
       oth.spis_imaf.plus(imaf_act);
       oth.spis_naim.plus(gettext("Акт сверки с общими суммами по документу"));
       oth.spis_imaf.plus(imaf_act_bi);
       oth.spis_naim.plus(gettext("Акт сверки с общими суммами по документу без месячных итогов"));

       for(int nomer=0; nomer < oth.spis_imaf.kolih(); nomer++)
        iceb_t_ustpeh(oth.spis_imaf.ravno(nomer),1);

       iceb_t_rabfil(&oth,"");

     }
    goto naz;

  case FK8:    /*Общий список организаций*/
  case SFK8:    /*Общий список организаций*/

    clear(); /*Очистить экран и закрасить фоновым цветом*/
    koz.new_plus("");
    naior.new_plus("");
    if(K == SFK8)
      naior.new_plus(rpoi.naim_kont.ravno());

    rpoi.naim_kont.new_plus("");
    rpoi.metpoi=0;
    
    if(dirkontr(&koz,&naior,1) == 0)
     {
      /*Проверяем есть ли в списке счета*/
      sprintf(strsql,"select ns from Skontr where ns='%s' and \
kodkon='%s'",sht,koz.ravno());
      if(sql_readkey(&bd,strsql,&row1,&cur1) == 0)
       {
      

        DANET.VVOD_delete();
        sprintf(strsql,gettext("Ввести %s в список счета %s ?"),naior.ravno(),sht);
        DANET.VVOD_SPISOK_add_MD(strsql);
        if(danet(&DANET,2,stdscr) == 1)
         {

          time(&vr);

          sprintf(strsql,"insert into Skontr \
values ('%s','%s',%d,%ld)",sht,koz.ravno(),iceb_t_getuid(),vr);   

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
             goto naz1;
            }
           else
            {
             msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);
             goto naz1;
            }
           }
         }
        kodvk.new_plus(koz.ravno());
        goto naz1; 
       }
     }
    clear();   
    goto naz1; 

  case FK9:    /*Переписать список контрагентов из другого счёта*/
    dirskont_copy(sht);
    goto naz1;

  default:
    if(kolstr > 0)
      goto naz;
    else
      goto kkkk;
      
 }
}
/*************************/
/* Шапка                 */
/*************************/
void            shbh3(const char *sht,int kolstr)
{
clear();
move(0,0);
printw(gettext("Ввод и корректировка контрагентов для счёта %s"),sht);
printw(" %s:%d",gettext("Количество"),kolstr);
move(1,0);

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F6",gettext("на.код"),
"F7",gettext("сальдо"),
"F8",gettext("общ.спи."),
"F9",gettext("копировать"),
"F10",gettext("выход"),NULL);
}
  

/*********************/
/*Проверка реквизитов*/
/*********************/
int dirskont_prov(char *kod_kontr,class dirskontr_rp *rpoi)
{

SQL_str row;
class SQLCURSOR cur;
char strsql[512];
rpoi->nm_kontr.new_plus("");

sprintf(strsql,"select * from Kontragent where kodkon='%s'",kod_kontr);
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,gettext("Не найден код контрагента %s в общем списке!"),kod_kontr);
  iceb_t_soob(strsql);
  return(0); /*чтобы вставило всписок и можно было удалить*/
 }

rpoi->nm_kontr.new_plus(row[1]);

if(rpoi->metpoi == 0)
 return(0);
 
if(rpoi->naim_kont.ravno()[0] != '\0')
 if(strstrm(row[1],rpoi->naim_kont.ravno()) == 0)
  if(strstrm(row[16],rpoi->naim_kont.ravno()) == 0)
   return(1);

if(rpoi->gorod.ravno()[0] != '\0')
 if( strstrm(row[3],rpoi->gorod.ravno()) == 0)
  return(2);

if(rpoi->gorodb.ravno()[0] != '\0')
 if(strstrm(row[4],rpoi->gorodb.ravno()) == 0)
  return(3);

if(rpoi->kod.ravno()[0] != '\0')
 if(strstrm(row[5],rpoi->kod.ravno()) == 0)
  return(4);

if(rpoi->naimb.ravno()[0] != '\0')
 if(strstrm(row[2],rpoi->naimb.ravno()) == 0)
 {
  return(5);
 }
if(rpoi->mfo.ravno()[0] != '\0')
 if(strstrm(row[6],rpoi->mfo.ravno()) == 0)
  return(6);

if(rpoi->nsh.ravno()[0] != '\0')
 if(strstrm(row[7],rpoi->nsh.ravno()) == 0)
  return(7);

if(rpoi->inn.ravno()[0] != '\0')
 if(strstrm(row[8],rpoi->inn.ravno()) == 0)
  return(8);

if(rpoi->kodgrkon.ravno()[0] != '\0')
 if(SRAV(row[11],rpoi->kodgrkon.ravno(),0) != 0)
  return(9);

return(0);


}

/**************************************/
/*Удаление не используемых контрагентов*/
/****************************************/
void 	udnkks(const char *sht,SQLCURSOR *cur,long kolstr)
{
int		koludk;
SQL_str         row;
long		kolstr1;
char		imaf[112];
char		strsql[512];


VVOD DANET(2);
DANET.VVOD_SPISOK_add_MD(gettext("Удалить не используемые контрагеты ? Вы уверены ?"));
if(danet(&DANET,2,stdscr) == 2)
  return;



clear();
cur->poz_cursor(0);
koludk=kolstr1=0;
while(cur->read_cursor(&row) != 0)
 {

  kolstr1++;
  strzag(LINES-1,0,kolstr,kolstr1);

  printw("%-5s %-40s",row[0],row[1]);
  sprintf(strsql,"Skontr#%s",sht);
  if(provud(strsql,row[1],0) != 0)
   {
    printw(" %s\n",gettext("используется"));
    continue;
   }
  koludk++;
  printw(" %s\n",gettext("удален"));

  sprintf(strsql,"delete from Skontr where ns='%s' and kodkon ='%s'",
  row[0],row[1]);
  if(sql_zap(&bd,strsql) != 0)
   {
   if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Толко чтение
    {
     iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
     break;
    }
   else
    msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);
   }
 }
printw("\n%s: %d\n",gettext("Количество удаленных контрагентов"),koludk);
if(koludk != 0)
  printw("%s:\n%s\n",gettext("Список удаленных контрагентов выгружено в файл"),imaf);

const char *imatab={"Skontr"};

printw(gettext("Оптимизация таблицы %s !\n"),imatab);
GDITE();
SQLCURSOR cur1;
int		metkazap=0;
strcpy(strsql,"select VERSION()");
sql_readkey(&bd,strsql,&row,&cur1);
if(SRAV(row[0],"3.22",1) == 0)
  metkazap=1;  
printw("VERSION:%s\n",row[0]);
refresh();

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table %s",imatab);
if(metkazap == 1)
 {
  if(sql_zap(&bd,strsql) != 0)
   msql_error(&bd,gettext("Ошибка оптимизации таблицы !"),strsql);
 }
else
  sql_readkey(&bd,strsql,&row,&cur1);


OSTANOV();

}
/******************************************/
/*Распечатка*/
/**************************************/
void dirskont_rs(class SQLCURSOR *cur,int kolstr,class dirskontr_rp *rpoi)
{
FILE *ff,*ff1;
char imaf[64],imaf1[64];
 GDITE();

sprintf(imaf,"Kontr%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

sprintf(imaf1,"kont%d.lst",getpid());
if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  error_op_nfil(imaf1,errno,"");
  return;
 }

iceb_t_zagolov(gettext("Список контрагентов"),ff1);


struct  tm      *bf;
time_t tmm;

 time(&tmm);
 bf=localtime(&tmm);


 fprintf(ff,"%s\n\n%s\n\
%s %d.%d.%d%s %s: %d:%d\n",
 iceb_t_get_pnk("00",0),
 gettext("Список контрагентов"),
 gettext("По состоянию на"),
 bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
 gettext("г."),
 gettext("Время"),
 bf->tm_hour,bf->tm_min);

 fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
   fprintf(ff,gettext("\
 Код |           Наименование  контрагента    |\
   Наименование банка         |  Адрес контрагента           |\
     Адрес банка              |  Код     |   MФО    | Номер счета   |\
Нал. номер     |Н.пл. НДС |Телефон   |Группа|Рег.н.ЧП| Договор\n"));
   fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff1,"\
----------------------------------------------------------------------\n");
fprintf(ff1,gettext("\
 Н.п/п|Код контр.|       Наименование контрагента         |Примечание|\n"));
fprintf(ff1,"\
----------------------------------------------------------------------\n");

SQL_str row,row1;
class SQLCURSOR cur1;
int kl=0;
char strsql[512];
int i=0;
cur->poz_cursor(0);

while(cur->read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++i);
  if(dirskont_prov(row[1],rpoi) != 0)
      continue;

  sprintf(strsql,"select * from Kontragent where kodkon='%s'",row[1]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {

     sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),row[1]);
     iceb_t_soob(strsql);
     continue;
   }      
  kl++;
/*************
  fprintf(ff,"%-5s %-40.40s %-30.30s %-30.30s %-30.30s %-10s %-10s\
 %-15s %-15s %-10s %-10.10s %-6s %-8s %s\n",
  row1[0],row1[1],row1[2],row1[3],row1[4],row1[5],row1[6],row1[7],
  row1[8],row1[9],row1[10],row1[11],row1[15],row1[14]);
*************/
  fprintf(ff,"%-*s %-*.*s %-*.*s %-*.*s %-*.*s %-*s %-*s\
 %-*s %-*s %-*s %-*.*s %-*s %-*s %s\n",
  iceb_tu_kolbait(5,row1[0]),row1[0],
  iceb_tu_kolbait(40,row1[1]),iceb_tu_kolbait(40,row1[1]),row1[1],
  iceb_tu_kolbait(30,row1[2]),iceb_tu_kolbait(30,row1[2]),row1[2],
  iceb_tu_kolbait(30,row1[3]),iceb_tu_kolbait(30,row1[3]),row1[3],
  iceb_tu_kolbait(30,row1[4]),iceb_tu_kolbait(30,row1[4]),row1[4],
  iceb_tu_kolbait(10,row1[5]),row1[5],
  iceb_tu_kolbait(10,row1[6]),row1[6],
  iceb_tu_kolbait(15,row1[7]),row1[7],
  iceb_tu_kolbait(15,row1[8]),row1[8],
  iceb_tu_kolbait(10,row1[9]),row1[9],
  iceb_tu_kolbait(10,row1[10]),iceb_tu_kolbait(10,row1[10]),row1[10],
  iceb_tu_kolbait(6,row1[11]),row1[11],
  iceb_tu_kolbait(8,row1[15]),row1[15],
  row1[14]);

  fprintf(ff1,"%6d|%-*s|%-*.*s|%10s|\n",
  kl,
  iceb_tu_kolbait(10,row1[0]),row1[0],
  iceb_tu_kolbait(40,row1[1]),iceb_tu_kolbait(40,row1[1]),row1[1],
  "");

  for(int nom=40; nom < iceb_tu_strlen(row1[1]); nom+=40)
   fprintf(ff1,"%6s|%10s|%-*.*s|%10s|\n","","",
   iceb_tu_kolbait(40,iceb_tu_adrsimv(nom,row1[1])),
   iceb_tu_kolbait(40,iceb_tu_adrsimv(nom,row1[1])),
   iceb_tu_adrsimv(nom,row1[1]),
   "");

 }
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%s - %d\n",gettext("Итого"),kl);

podpis(ff);


fclose(ff);
podpis(ff1);

fclose(ff1);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Список контрагентов"));

oth.spis_imaf.plus(imaf1);
oth.spis_naim.plus(gettext("Список контрагентов без реквизитов"));
for(int nom=0; nom < oth.spis_imaf.kolih(); nom++)
  iceb_t_ustpeh(oth.spis_imaf.ravno(nom),3);


iceb_t_rabfil(&oth,"");

}
/******************/
/*Переписать список контрагентов из другого счёта*/
/***************************************************/

void dirskont_copy(const char *shet)
{
class iceb_tu_str shet_c("");
VVOD	VVOD1(2);

VVOD1.VVOD_SPISOK_add_MD("Введите номер счёта список контаргентов которого нужно скопировать");
         
if(vvod1(&shet_c,16,&VVOD1,NULL,stdscr,-1,-1) == FK10)
  return;

struct OPSHET	shetv;
char strsql[512];
if(prsh(shet_c.ravno(),&shetv) == 0)
 {
  sprintf(strsql,gettext("Нет счета %s в плане счетов !"),shet_c.ravno());
  iceb_t_soob(strsql);
  return;
 }
GDITE();
int kolstr=0;
class SQLCURSOR cur;
SQL_str row;
sprintf(strsql,"select kodkon from Skontr where ns='%s'",shet_c.ravno());
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,"",strsql);
  return;
 }  

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи!"));
  return;
 }
int kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);  
  sprintf(strsql,"insert into Skontr (ns,kodkon,ktoi,vrem) \
  values('%s','%s',%d,%ld)",shet,row[0],iceb_t_getuid(),time(NULL)); 
  sql_zapis(strsql,1,1);
 }
 

}
