/* $Id: dirkasords.c,v 5.57 2013/12/31 11:48:10 sasa Exp $ */
/*16.01.2017	26.09.2000	Белых А.И.	dirkasords.c
Просмотр списка кассовых ордеров
*/
#include        <errno.h>
#include	"buhg.h"

class kas_poi
 {
  public:
  short dn,mn,gn; //Дата с которой запущен режим просмотра
  class iceb_tu_str nomdpoi;
  class iceb_tu_str kassapoi;
  class iceb_tu_str kodoppoi;
  class iceb_tu_str shetpoi;
  class iceb_tu_str nomer_bl;
  class iceb_tu_str dat1,dat2;
  class iceb_tu_str fio;
  short		tipzpoi;
  short           metpoi;  /*0-не задан поиск 1-задан*/
  kas_poi()
   {
    clear();
   }  

  void clear()
   {
    fio.new_plus("");
    nomdpoi.new_plus("");
    kassapoi.new_plus("");
    kodoppoi.new_plus("");
    shetpoi.new_plus("");
    nomer_bl.new_plus("");
    dat1.new_plus("");
    dat2.plus_tek_dat();
    tipzpoi=0;
   
    metpoi=0;
   }
 };
 
void		sapdkord(class kas_poi *poisk,short mpn);
int		poikord(SQL_str row,class kas_poi *poisk,short mpn);
void kasord_ras(SQLCURSOR *cur,int kolstr,class kas_poi *poisk,short mpn);
void dirkasords_eks(SQLCURSOR *cur,int kolstr,class kas_poi *poisk,short mpn);

void            dirkasords(short dnp,short mnp,short gnp)
{
class iceb_t_mstr spis_menu(0);
time_t		tmm;
class iceb_tu_str bros("");
class iceb_tu_str kodv("");
long		kolstr;
long		pozz; /*Позиция первой строчки меню*/
long            pozz1; /*Количество строк прочитанных для заполнения меню*/
SQL_str         row,row1;
char		strsql[1024];
struct  tm      *bf;
short           dn=0,mn=0,gn=0,dk=0,mk=0,gk=0;
short		d,m,g;
int             K;
short           kls;
short           x=0,y=2;
int             i,kom,kom1,kl;
int		prc;
short		mspz; /*метка соответствия первой записи*/
class iceb_tu_str nomd("");
class iceb_tu_str kassa("");
short		tipz=0;
class iceb_tu_str naimkas(""),naimkodop("");
short		mpn=0; /*0-все 1-не подтвержденный 2-без проводок*/
class iceb_tu_str kodop("");
class iceb_tu_str fio("");
class kas_poi  poisk;
short metka_sort_den=0; /*0-сортировка по возростанию дат 1- по убыванию дат*/
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);

sprintf(strsql,"01.01.%d",gt);
poisk.dat1.new_plus(strsql);

SQLCURSOR curr;
VVOD SOOBP(1);
VVOD VV(0);
VVOD MENU(3);
VVOD DANET(1);

DANET.VVOD_SPISOK_add_MD(gettext("Сделать привязку к карточкам ?"));

mpn=kom=0;
 
poisk.dn=dn=dk=dnp;
poisk.mn=mn=mk=mnp;
poisk.gn=gn=gk=gnp;


pozz=0;

naz1:;
clear();
GDITE();

kls=LINES-5;


if(gn == 0)
 gn=gt;

sprintf(strsql,"select * from Kasord where datd >= '%04d-01-01'",gn);

if(dn != 0)
  sprintf(strsql,"select * from Kasord where datd >= '%04d-%02d-%02d'",gn,mn,dn);

if(dn != 0 && dk != 0)
  sprintf(strsql,"select * from Kasord where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d'",gn,mn,dn,gk,mk,dk);

if(metka_sort_den == 0)
  strcat(strsql," order by datd asc,nomd asc");
else
  strcat(strsql," order by datd desc,nomd asc");


SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
if(kolstr == 0)
 {

lll:;
  sapdkord(&poisk,mpn);

   VVOD SOOB(1);
   SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
   K=soobshw(&SOOB,stdscr,-1,-1,1,1);
//  printw("\nkolstr=%ld %s\n",kolstr,strsql);
//  K=getch();
  
  if(K == FK5 || K == SFK5 || K == SFK2 || K == SFK3 || K == ENTER)
     goto lll;
  if(K == KEY_RESIZE)
    goto naz1;  
  goto vper;
 }

naz:;
GDITE();

if(kolstr != 0 && pozz >= kolstr)
  pozz = kolstr-1;


cur.poz_cursor(pozz);
mspz=2;
spis_menu.spis_str.free_class();
pozz1=0;
for(i=0; i<kls ; )
 {
  
  if(cur.read_cursor(&row) == 0)
    break;
/*
  printw("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  refresh();
*/
  pozz1++;
  if(poikord(row,&poisk,mpn) != 0)
   continue;

  bros.new_plus("");
  if(row[1][0] == '1')
   bros.plus("+");
  if(row[1][0] == '2')
   bros.plus("-");
  if(row[6][0] == '0')
   bros.plus("*");
  if(row[7][0] == '0')
   bros.plus("?");

  rsdat(&d,&m,&g,row[2],2);

  fio.new_plus("");
  if(row[10][0] != '\0')
    fio.new_plus(row[10]);
  else
   {

    /*Если в ордере 1 контрагент то берем его*/
    sprintf(strsql,"select kontr from Kasord1 where kassa=%s and \
god=%s and tp=%s and nomd='%s'",row[0],row[12],row[1],row[3]);
    if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
     {
      sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
      row1[0]);
      if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
        fio.new_plus(row1[0]);
     }
   }    

  sprintf(strsql,"%s|%02d.%02d.%d|%-*s|%-*s|%-*s|%-*.*s|%s",
  row[0],d,m,g,
  iceb_tu_kolbait(4,row[3]),row[3],
  iceb_tu_kolbait(8,row[4]),row[4],
  iceb_tu_kolbait(3,row[5]),row[5],
  iceb_tu_kolbait(20,fio.ravno()),iceb_tu_kolbait(20,fio.ravno()),fio.ravno(),
  bros.ravno());

  spis_menu.spis_str.plus(strsql);
  i++;

  if(pozz1 == 1) /*Первая запись удовлетворяет условиям поиска*/
   mspz=1;

 }

if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

if(i == 0)
 {
  if(kolstr != 0 && pozz != 0)
   {
    //Если записи есть но не подходят по условиям поиска
    pozz--;
    goto naz;
   }
  goto lll;
 }

sapdkord(&poisk,mpn);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,prc,&K,"",0);


if(kom == -7) //Изменение размера экрана
  goto naz1;

if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  for(i=0; i< mspz  ; )
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

    if(poikord(row,&poisk,mpn) != 0)
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
    if(poikord(row,&poisk,mpn) != 0)
     continue;
    i++;
   }

  goto naz;
 }
 
if(kom == -5) /*Страница вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
/*
  printw("\npozz=%d kls=%d kolstr=%d i=%d\n",
  pozz,kls,kolstr,i);
  OSTANOV();
*/
  if(pozz + kls >= kolstr)
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
  printw("\n вперед pozz=%d pozz1=%d i=%d kls=%d\n",pozz,pozz1,i,kls);
  OSTANOV();
*/
  goto naz;

 }

if(kom == -4) /*Страница вверх*/
 {
  GDITE();
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
    if(poikord(row,&poisk,mpn) != 0)
     continue;
    i++;
   }
  goto naz;
 }

if(kom >= 0)
 {
  class iceb_tu_str str_vib(spis_menu.spis_str.ravno(kom));
  if(SRAV(gettext("Конец списка"),str_vib.ravno(),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz;

  polen(str_vib.ravno(),&kassa,1,'|');
  polen(str_vib.ravno(),&bros,2,'|');
  rsdat(&d,&m,&g,bros.ravno(),1);
  polen(str_vib.ravno(),&nomd,3,'|');
  polen(str_vib.ravno(),&kodop,5,'|');

  polen(str_vib.ravno(),&bros,7,'|');
  tipz=0;
  if(bros.ravno()[0] == '+')
   tipz=1;  
  if(bros.ravno()[0] == '-')
   tipz=2;  
 }

vper:;
switch(K)
 {  
  case ENTER:
  
     clear();

     dirkasord(kassa.ravno(),nomd.ravno(),d,m,g,tipz);
     clear();
     goto naz1;

  case FK10:
  case ESC:
    return;

  case FK1: /*Помощь*/
    GDITE();
    iceb_t_pdoc("kas2_4.txt");
    if(kolstr > 0)
      goto naz;
    else
      goto lll;

  case FK2:  /*Не подтвержденные накладные*/
    if(mpn == 1)
      mpn=0;
    else
      mpn=1;  

    if(kolstr > 0)
      goto naz;
    else
      goto lll;

  case SFK2:  /*Снять метку не подтвержденного документа*/
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Снять отметку не подтвержденного документа ? Вы уверены ?"));
    if(danet(&DANET,2,stdscr) == 2)
     goto naz;

    sprintf(strsql,"update Kasord \
set \
podt=1 \
where god=%d and kassa=%s and nomd='%s' and tp=%d",
    g,kassa.ravno(),nomd.ravno(),tipz);

    sql_zapis(strsql,0,0);
    goto naz1;

  case FK3:  /*Документы без проводок*/
    if(mpn == 2)
      mpn=0;
    else
      mpn=2;  

    if(kolstr > 0)
      goto naz;
    else
      goto lll;

  case SFK3:  /*Снять отметку о не выполненых проводках*/
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Снять отметку о невыполненых проводках ? Вы уверены ?"));
    if(danet(&DANET,2,stdscr) == 2)
     goto naz;


    sprintf(strsql,"update Kasord \
set \
prov=1 \
where god=%d and kassa=%s and nomd='%s' and tp=%d",
    g,kassa.ravno(),nomd.ravno(),tipz);

    if(sql_zap(&bd,strsql) != 0)
      msql_error(&bd,gettext("Ошибка записи !"),strsql);

    goto naz1;


  case FK4:  /*Поиск*/
    VV.VVOD_delete();
    VV.VVOD_SPISOK_add_ZAG(gettext("Введите данные для поиска документов"));

    VV.VVOD_SPISOK_add_MD(gettext("Номер документа..........."));//0
    VV.VVOD_SPISOK_add_MD(gettext("Дата начала поиска........"));//1
    VV.VVOD_SPISOK_add_MD(gettext("Дата конца поиска........."));//2
    VV.VVOD_SPISOK_add_MD(gettext("Код прихода/расхода.(,,).."));//3
    VV.VVOD_SPISOK_add_MD(gettext("Номер кассы..............."));//4
    VV.VVOD_SPISOK_add_MD(gettext("Приход/расход.....(+/-)..."));//5
    VV.VVOD_SPISOK_add_MD(gettext("Счет......................"));//6
    VV.VVOD_SPISOK_add_MD(gettext("Номер бланка.............."));//7
    VV.VVOD_SPISOK_add_MD(gettext("Фамилия.............(,,).."));//8
      
    VV.VVOD_SPISOK_add_data(poisk.nomdpoi.ravno(),64);
    VV.VVOD_SPISOK_add_data(poisk.dat1.ravno(),11);
    VV.VVOD_SPISOK_add_data(poisk.dat2.ravno(),11);
    
    VV.VVOD_SPISOK_add_data(poisk.kodoppoi.ravno(),128);

    VV.VVOD_SPISOK_add_data(poisk.kassapoi.ravno(),64);

    VV.VVOD_SPISOK_add_data(2);

    if(poisk.tipzpoi == 1)
     VV.VVOD_SPISOK_zapis_data(5,"+");
    if(poisk.tipzpoi == 2)
     VV.VVOD_SPISOK_zapis_data(5,"-");

    VV.VVOD_SPISOK_add_data(poisk.shetpoi.ravno(),64);
    VV.VVOD_SPISOK_add_data(poisk.nomer_bl.ravno(),64);
    VV.VVOD_SPISOK_add_data(poisk.fio.ravno(),255);
    
naz2:;
    poisk.metpoi=0;
    
    helstr(LINES-1,0,"F2/+",gettext("поиск"),
    "F3",gettext("реквизиты"),
    "F4",gettext("очистить"),
    "F10",gettext("выход"),NULL);

         
    kl=VV.vvod(0,1,1,-1,-1);

    poisk.nomdpoi.new_plus(VV.VVOD_SPISOK_return_data(0));
    poisk.dat1.new_plus(VV.VVOD_SPISOK_return_data(1));
    poisk.dat2.new_plus(VV.VVOD_SPISOK_return_data(2));

    poisk.kodoppoi.new_plus(VV.VVOD_SPISOK_return_data(3));
    poisk.kassapoi.new_plus(VV.VVOD_SPISOK_return_data(4));

    poisk.tipzpoi=0;
    if(VV.VVOD_SPISOK_return_data(5)[0] == '+')
      poisk.tipzpoi=1;
    if(VV.VVOD_SPISOK_return_data(5)[0] == '-')
      poisk.tipzpoi=2;
    poisk.shetpoi.new_plus(VV.VVOD_SPISOK_return_data(6));
    poisk.nomer_bl.new_plus(VV.VVOD_SPISOK_return_data(7));
    poisk.fio.new_plus(VV.VVOD_SPISOK_return_data(8));

    dn=mn=gn=dk=mk=gk=0;
    switch(kl)
     {
      case FK10:
      case ESC:
        break;
        
      case FK2:
      case PLU:
        GDITE();
        poisk.metpoi=1;
        
        if(poisk.dat1.getdlinna() > 1)
         {
          if(rsdat(&d,&m,&g,poisk.dat1.ravno(),1) != 0)
           {
            iceb_t_soob(gettext("Не верно введена дата начала !"));
            goto naz2;
           }
          dk=dn=d; mn=mk=m; gn=gk=g;
         }

        if(poisk.dat2.getdlinna() <= 1)
         {
          dpm(&dk,&mk,&gk,5); 
         }
        else
         if(rsdat(&d,&m,&g,poisk.dat2.ravno(),1) != 0)
          {
           iceb_t_soob(gettext("Не верно введена дата конца !"));
           goto naz2;
          }
         else
          {
           dk=d; mk=m; gk=g;
          }
        pozz=0;
        goto naz1;

      case FK3:
        MENU.VVOD_delete();
        MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка касс"));
        MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций приходов"));
        MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций расходов"));
        MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

        clearstr(LINES-1,0);
        kom1=0;
        while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

        switch (kom1)
         {
          case 4 :
          case -1:
           goto naz2;

          case 0 :
            bros.new_plus("");
            if(dirkas(1,&kodv,&bros) == 0)
              VV.data_plus(4,bros.ravno());
            goto naz2;

          case 1 :
            bros.new_plus("");
            if(dirkasop(1,1,&kodv,&bros) == 0)
              VV.data_z_plus(3,bros.ravno());
            goto naz2;

          case 2 :
            bros.new_plus("");
            if(dirkasop(2,1,&kodv,&bros) == 0)
              VV.data_z_plus(3,bros.ravno());
            goto naz2;
         }
        goto naz1;

      case FK4:
        VV.VVOD_clear_data();
        memset(&poisk,'\0',sizeof(poisk));
        goto naz2;
                   
      default:
        goto naz2;
     }

    goto naz1;

  case FK5: /*Расшифровка*/

    SOOBP.VVOD_delete();

    /*Читаем наименование операции*/
    if(tipz == 1)
      bros.new_plus("Kasop1");
    if(tipz == 2)
      bros.new_plus("Kasop2");
    naimkodop.new_plus("");
    sprintf(strsql,"select naik from %s where kod='%s'",
    bros.ravno(),kodop.ravno());
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      beep();
      printw(gettext("Не найден код операции %s !"),kodop.ravno());
      printw("\n");
      OSTANOV();
     }
    else
     {
      naimkodop.new_plus(row1[0]);
     } 

    /*Читаем наименование кассы*/
    naimkas.new_plus("");
    sprintf(strsql,"select naik from Kas where kod=%s",kassa.ravno());
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      beep();
      printw("%s %s !\n",gettext("Не найдена касса"),kassa.ravno());
      OSTANOV();
     }
    else
     {
      naimkas.new_plus(row1[0]);
     } 

    i=0;
    sprintf(strsql,"%s %d.%d.%d",gettext("Дата докум. :"),d,m,g);
    SOOBP.VVOD_SPISOK_add_MD(strsql);
    sprintf(strsql,"%s %s",      gettext("Документ    :"),nomd.ravno());
    SOOBP.VVOD_SPISOK_add_MD(strsql);
    sprintf(strsql,"%s %s %s",   gettext("Касса       :"),kassa.ravno(),naimkas.ravno());
    SOOBP.VVOD_SPISOK_add_MD(strsql);
    sprintf(strsql,"%s %s %s",   gettext("Операция    :"),kodop.ravno(),naimkodop.ravno());
    SOOBP.VVOD_SPISOK_add_MD(strsql);

    /*Читаем запись целиком*/

    sprintf(strsql,"select * from Kasord where nomd='%s' and kassa=%s and \
god=%d and tp=%d",nomd.ravno(),kassa.ravno(),g,tipz);
    if(sql_readkey(&bd,strsql,&row,&curr) != 1)
     {
      VVOD SOOB(1);
      sprintf(strsql,"%s %s %s %s %s %d.%d.%d%s %s !",
      gettext("Документ"),
      nomd.ravno(),
      gettext("Касса"),
      kassa.ravno(),
      gettext("Дата"),  
      d,m,g,
      gettext("г."),gettext("не найден"));
      SOOB.VVOD_SPISOK_add_MD(strsql);

      if(tipz == 1)
        sprintf(strsql,gettext("Приход"));
      else   
        sprintf(strsql,gettext("Расход"));
      SOOB.VVOD_SPISOK_add_MD(strsql);
      soobshw(&SOOB,stdscr,-1,-1,0,1);
      goto naz;
     }

    if(atoi(row[13]) != 0)
     {
      SOOBP.VVOD_SPISOK_add_MD(iceb_t_kszap(row[13]));
      
     }

    if(atoi(row[14]) > 0)
     {
      tmm=atoi(row[14]);
      bf=localtime(&tmm);
      sprintf(strsql,"%s %d.%d.%d%s %s - %d:%d:%d",
      gettext("Дата записи :"),
      bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
      gettext("г."),
      gettext("Время"),
      bf->tm_hour,bf->tm_min,bf->tm_sec);
      SOOBP.VVOD_SPISOK_add_MD(strsql);
     }
    
    soobshw(&SOOBP,stdscr,-1,-1,0,1);
    goto naz;

  case SFK5: //распечатать все найденные кассовые ордера
    kasord_ras(&cur,kolstr,&poisk,mpn);
    goto naz;

  case FK6: //Увеличить на день
    if(poisk.dn != 0)
     {
      dpm(&dn,&mn,&gn,1);
      poisk.dn=dk=dn;
      poisk.mn=mk=mn;
      poisk.gn=gk=gn;
     }
    goto naz1;

  case FK7: //Уменьшить на день
    if(poisk.dn != 0)
     { 
      dpm(&dn,&mn,&gn,2);
      poisk.dn=dk=dn;
      poisk.mn=mk=mn;
      poisk.gn=gk=gn;
     }
    goto naz1;
      
  case FK8: //экспорт кассовых ордеров в файл
    dirkasords_eks(&cur,kolstr,&poisk,mpn);
    goto naz1;

  case TAB: /*включть/выключить сортировку в порядке убывания дат*/
    metka_sort_den++;
    if(metka_sort_den >= 2)
     metka_sort_den=0;
    goto naz1;
   
  default:
    if(kolstr > 0)
      goto naz;
    else
      goto lll; 
 }


}

/************/
/*Шапка меню*/
/*************/

void		sapdkord(class kas_poi *poisk,
short mpn) //0-все 1-не подтвержденный 2-без проводок
{
short		Y,X;

clear();

if(mpn == 0)
 printw(gettext("Просмотр списка кассовых ордеров"));
if(mpn == 1)
 {
//  attron(A_BLINK);
  printw(gettext("Просмотр не подтвержденных кассовых ордеров"));
//  attroff(A_BLINK);
 }
if(mpn == 2)
 {
//  attron(A_BLINK);
  printw(gettext("Просмотр кассовых ордеров без проводок"));
//  attroff(A_BLINK);
 }
if(poisk->dn != 0)
 printw(" %d.%d.%d",poisk->dn,poisk->mn,poisk->gn);
 
move(1,0);
printw("  1     2         3       4      5          6");

move(Y=2,X=COLS-21);
printw(gettext("Расшифровка полей:"));
move(++Y,X);
printw(gettext("1:касса"));
move(++Y,X);
printw(gettext("2:дата"));
move(++Y,X);
printw(gettext("3:номер документа"));
move(++Y,X);
printw(gettext("4:счет"));
move(++Y,X);
printw(gettext("5:код операции"));
move(++Y,X);
printw(gettext("6:контрагент"));
move(++Y,X);
printw(gettext("+:приход"));
move(++Y,X);
printw(gettext("-:расход"));
move(++Y,X);
printw(gettext("?:не подтвержден"));
move(++Y,X);
printw(gettext("*:не сделаны проводки"));


if(poisk->metpoi == 1)
 {
  move(Y+=2,X);
//  attron(A_BLINK);
  printw(gettext("Поиск:"));
  if(poisk->nomdpoi.getdlinna() > 1)
   {
    move(++Y,X);
    printw("%s:%s",gettext("Н.д."),poisk->nomdpoi.ravno());
   }
  if(poisk->dat1.getdlinna() > 1)
   {
    move(++Y,X);
    printw("%s:%s",gettext("Дата н."),poisk->dat1.ravno());
   }

  if(poisk->dat2.getdlinna() > 1)
   {
    move(++Y,X);
    printw("%s:%s",gettext("Дата к."),poisk->dat2.ravno());
   }
  if(poisk->kodoppoi.getdlinna() > 1)
   {
    move(++Y,X);
    printw("%s:%s",gettext("Код оп."),poisk->kodoppoi.ravno());
   }
  if(poisk->kassapoi.getdlinna() > 1)
   {
    move(++Y,X);
    printw("%s:%s",gettext("Касса"),poisk->kassapoi.ravno());
   }
  if(poisk->nomer_bl.getdlinna() > 1)
   {
    move(++Y,X);
    printw("%s:%s",gettext("Номер бл."),poisk->nomer_bl.ravno());
   }
  if(poisk->fio.getdlinna() > 1)
   {
    move(++Y,X);
    printw("%s:%s",gettext("Фамилия"),poisk->fio.ravno());
   }
  if(poisk->tipzpoi != 0)
   {
    move(++Y,X);
    if(poisk->tipzpoi == 1)
     printw("%s %s",gettext("Вид оп."),"+");
    if(poisk->tipzpoi == 2)
     printw("%s %s",gettext("Вид оп."),"-");
   }
//  attroff(A_BLINK);

 }

if(poisk->dn == 0)
 {
  helstr(LINES-1,0,
"F1",gettext("помощь"),
"F2"," ? ",
"F3"," * ",
"F4",gettext("поиск"),
"F5",gettext("расшифровка"),
"F8",gettext("экспорт"),
"F10",gettext("выход"),NULL);
 }
else
 {
  helstr(LINES-1,0,
"F1",gettext("помощь"),
"F2"," ? ",
"F3"," * ",
"F4",gettext("поиск"),
"F5",gettext("расшифровка"),
"F6",gettext("след.день"),
"F7",gettext("пред.день"),
"F8",gettext("экспорт"),
"F10",gettext("выход"),NULL);
 }

}
/*************************/
/*Проверка условий поиска*/
/*************************/
//Если вернули 0 запись подходит под условия поиска
int poikord(SQL_str row,
class kas_poi *poisk,
short mpn) //0-все 1-не подтвержденный 2-без проводок
{
short		pro,pod;


pro=atoi(row[6]);
if(mpn == 2 && pro == 1)
   return(1);
  
pod=atoi(row[7]);
if(mpn == 1 && pod == 1)
   return(1);

if(poisk->metpoi == 0)
 return(0);


if(poisk->kassapoi.getdlinna() > 1)
 if(SRAV(poisk->kassapoi.ravno(),row[0],0) != 0)
   return(1);

if(poisk->nomdpoi.getdlinna() > 1)
 if(SRAV(poisk->nomdpoi.ravno(),row[3],0) != 0)
   return(1);

if(proverka(poisk->kodoppoi.ravno(),row[5],0,0) != 0)
 return(1);

if(poisk->shetpoi.getdlinna() > 1)
 if(SRAV(poisk->shetpoi.ravno(),row[4],0) != 0)
   return(1);

if(poisk->tipzpoi != 0)
  if(poisk->tipzpoi != atoi(row[1]))
   return(1);

if(poisk->nomer_bl.getdlinna() > 1)
 if(strstrm(row[15],poisk->nomer_bl.ravno()) == 0)
   return(1);

if(poisk->fio.getdlinna() > 1)
 {
  SQL_str row1;
  class SQLCURSOR cur;
  char strsql[512];
  class iceb_tu_str fio("");
  if(row[10][0] != '\0')
    fio.new_plus(row[10]);
  else
   {

    /*Если в ордере 1 контрагент то берем его*/
    sprintf(strsql,"select kontr from Kasord1 where kassa=%s and \
god=%s and tp=%s and nomd='%s'",row[0],row[12],row[1],row[3]);
    if(sql_readkey(&bd,strsql,&row1,&cur) == 1)
     {
      sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[0]);
      if(readkey(strsql,&row1,&cur) == 1)
        fio.new_plus(row1[0]);
     }
   }
  if(proverka(poisk->fio.ravno(),fio.ravno(),4,0) != 0)
   return(1);
 }    

return(0);

}
/*****************************************/
/*Распечатка кассовых ордеров списком*/
/***************************************/

void kasord_ras(SQLCURSOR *cur,int kolstr,class kas_poi *poisk,short mpn) //0-все 1-не подтвержденный 2-без проводок
{
SQL_str row;
SQL_str row1;
SQLCURSOR cur1;
int kolstr1=0;
class iceb_tu_str fio("");
char strsql[1024];
int tipz=0;
class iceb_tu_str dokum("");
class iceb_tu_str nomd("");
char imaf[64];
short dd,md,gd;

sprintf(imaf,"kasor%d.lst",getpid());
FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }
double sumd;
double sumpod;

cur->poz_cursor(0);
while(cur->read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(poikord(row,poisk,mpn) != 0)
   continue;

  fio.new_plus(row[10]);

  dokum.new_plus(row[11]);

  nomd.new_plus(row[3]);

  tipz=atoi(row[1]);
  rsdat(&dd,&md,&gd,row[2],2);

  if(sumkasor(row[0],tipz,nomd.ravno(),gd,&sumd,&sumpod) != 0)
   continue;
  
  if(fio.ravno()[0] == '\0' )
   {
    sprintf(strsql,"select kontr from Kasord1 where kassa=%s and datd='%s' and tp=%s \
and nomd='%s'",row[0],row[2],row[1],row[3]);     
    if(readkey(strsql,&row1,&cur1) == 1)
     {
      /*Читаем наименование контрагента*/
      sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row1[0]);
      
      if(readkey(strsql,&row1,&cur1) == 1)
        fio.new_plus(row1[0]);
      if(tipz == 2 && dokum.ravno()[0] == '\0')
       {
        class iceb_tu_str bros("");
        if(SRAV(ICEB_ZAR_PKTN,row[5],1) == 0)
          {
           int i=strlen(ICEB_ZAR_PKTN);
           sprintf(strsql,"select nomp,vidan,datvd from Kartb where tabn=%s",
           &row[5][i]);
           if(readkey(strsql,&row1,&cur1) > 0)
            {
             short d=0,m,g;
             rsdat(&d,&m,&g,row1[2],2);
             if(d == 0)
              {
               sprintf(strsql,"%s N%s %s",gettext("Паспорт"),row1[0],row1[1]);
               dokum.new_plus(strsql);
              }
             else
              {
               sprintf(strsql,"%s N%s %02d.%02d.%d%s %s",
               gettext("Паспорт"),row1[0],
               d,m,g,
               gettext("г."),
               row1[1]);
               dokum.new_plus(strsql);
              }
            }
          }
       }
     }
   }     

  if(kolstr1 > 1)
   fprintf(ff,"\f");

  if(tipz == 1)
   {
    class iceb_tu_str kod_cel_naz("");
    /*Узнаём код целевого назначения*/
    sprintf(strsql,"select kcn from Kasop1 where kod='%s'",row[5]);
    if(readkey(strsql,&row1,&cur1) == 1)
     kod_cel_naz.new_plus(row1[0]);
    kasord1("",nomd.ravno(),dd,md,gd,row[4],"",sumd,fio.ravno(),row[8],row[9],kod_cel_naz.ravno(),ff);
   }
  if(tipz == 2)
   {
    class iceb_tu_str kod_cel_naz("");
    /*Узнаём код целевого назначения*/
    sprintf(strsql,"select kcn from Kasop2 where kod='%s'",row[5]);
    if(readkey(strsql,&row1,&cur1) == 1)
     kod_cel_naz.new_plus(row1[0]);
    kasord2("",nomd.ravno(),dd,md,gd,row[4],"",sumd,fio.ravno(),row[8],row[9],dokum.ravno(),kod_cel_naz.ravno(),ff);
   }

 }
fclose(ff);
iceb_t_ustpeh(imaf,1);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка кассовых ордеров"));
iceb_t_rabfil(&oth,"");
}
/********************/
/*Экспорт кассовых ордеров в файл*/
/**************************/
void dirkasords_eks(SQLCURSOR *cur,int kolstr,class kas_poi *poisk,short mpn) //0-все 1-не подтвержденный 2-без проводок
{
static class iceb_tu_str imaf("");

int kom=0;
VVOD VVOD1(2);
VVOD1.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);

VVOD1.VVOD_SPISOK_add_ZAG(gettext("Экспорт кассовых ордеров"));
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите имя файла"));


if((kom=vvod1(&imaf,112,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
 return;


FILE *ff;

if((ff = fopen(imaf.ravno(),"w")) == NULL)
 {
  error_op_nfil(imaf.ravno(),errno,"");
  return;
 }
int kolstr1=0;
SQL_str row;
cur->poz_cursor(0);
short d,m,g;
SQLCURSOR cur1;
SQLCURSOR cur2;
SQL_str row1;
SQL_str row2;
class iceb_tu_str fio("");
char strsql[512];
int kolstr2;
class iceb_tu_str ikod("");

while(cur->read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(poikord(row,poisk,mpn) != 0)
   continue;
  
  fprintf(ff,"\nKDOK|");
  
  if(atoi(row[1]) == 1) //приходный кассовый ордер
    fprintf(ff,"+|");
  
  if(atoi(row[1]) == 2) //расходный кассовый ордер
    fprintf(ff,"-|");
  rsdat(&d,&m,&g,row[2],2);
    
  fprintf(ff,"%s|%d.%d.%d|%s|%s|%s|%s|%s|%s|%s|%s|\n",
  row[0],
  d,m,g,
  row[3],
  row[4],
  row[5],
  row[8],
  row[9],
  row[10],
  row[11],
  row[15]);
  
  //читаем содержимое кассового ордера
  sprintf(strsql,"select kontr,suma from Kasord1 where \
  kassa=%s and god=%d and tp=%s and nomd='%s'",
  row[0],g,row[1],row[3]);

  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    return;
   }

  while(cur1.read_cursor(&row1) != 0)
   {
    fio.new_plus("");
    ikod.new_plus("");
    //читаем наименование контрагента
    sprintf(strsql,"select naikon,kod from Kontragent where kodkon='%s'",row1[0]);
    if(readkey(strsql,&row2,&cur2) == 1)
     {
      fio.new_plus(row2[0]);
      ikod.new_plus(row2[1]);
     }
    
    fprintf(ff,"KZAP|%s|%s|%s|%s|\n",
    row1[0],row1[1],ikod.ravno(),fio.ravno());
    
   }  
 }
fclose(ff);

//rabfil(gettext("Экспорт кассовых ордеров"),imaf.ravno());
class spis_oth oth;
oth.spis_imaf.plus(imaf.ravno());
oth.spis_naim.plus(gettext("Экспорт кассовых ордеров"));
iceb_t_rabfil(&oth,"");


}
