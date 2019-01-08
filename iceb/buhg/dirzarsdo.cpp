/*$Id: dirzarsdo.c,v 5.27 2013/08/13 05:49:47 sasa Exp $*/
/*18.09.2018	27.05.2016	Белых А.И.	dirzarsdo.c
Ввод и корректирвка списка должостных окладов, почасовых ставок и тарифов
*/
#include        <errno.h>
#include        "buhg.h"

#define KOL_STR_MENU 7

class dirzarsdo_menu_mr
 {
  public:
   class SPISOK strsr;

   dirzarsdo_menu_mr()
    {
     strsr.plus(gettext("Оплата за месяц, расчёт по дням"));//0
     strsr.plus(gettext("Оплата за месяц, расчёт по часам"));//1
     strsr.plus(gettext("Оплата за месяц, без расчёта"));//2
     strsr.plus(gettext("Оплата за час"));//3
     strsr.plus(gettext("Оплата за день"));//4
     strsr.plus(gettext("Оплата по тарифу, расчёт по дням"));//5
     strsr.plus(gettext("Оплата по тарифу, расчёт по часам"));//6
     
    }
 };

class dirzarsdo_metka_str
 {
  public:
   class SPISOK strsr;

   dirzarsdo_metka_str()
    {
     strsr.plus(gettext("мес.дни"));//0
     strsr.plus(gettext("мес.часы"));//1
     strsr.plus(gettext("мес."));//2
     strsr.plus(gettext("часы"));//3
     strsr.plus(gettext("дни"));//4
     strsr.plus(gettext("тариф дни"));//5
     strsr.plus(gettext("тариф часы"));//6

    }
 };
 

class dirzarsdo_poi
 {
  public:
   short metka_poi; /*0-поиск не делать 1-делать*/
   class iceb_tu_str tabnom;
   class iceb_tu_str dataz;
   class iceb_tu_str kod_nah;
   class iceb_tu_str fio;
   class iceb_tu_str koment;
   class iceb_tu_str shet;
   class iceb_tu_str podr;         
   class iceb_tu_str sposob_r;
   
   int kod_sr; /*код способа расчёта*/

  dirzarsdo_poi()
   {
    metka_poi=0;
    clear();
   }

  void clear()
   {
    tabnom.new_plus("");
    dataz.new_plus("");
    kod_nah.new_plus("");
    fio.new_plus("");
    koment.new_plus("");
    shet.new_plus("");
    podr.new_plus("");
    kod_sr=0;
    sposob_r.new_plus("");
   }
 };
 
void dirzarsdo_vvod(const char *dataz,const char *tabnom,const char *kod_nah);
void dirzarsdo_sap(class dirzarsdo_poi*);
int   dirzarsdo_zap(VVOD *VV,const char *dataz,const char *tabnom,const char *kod_nah,int metka_sp);
void	dirzarsdo_ras(SQLCURSOR *cur,class dirzarsdo_poi *poisk);
int dirzarsdo_poi(SQL_str row,class dirzarsdo_poi *poi);
int dirzarsdo_vrp(class dirzarsdo_poi *poi);



void dirzarsdo(int tbn)
{
char		strsql[1024];
class iceb_t_mstr spis_menu(0);
SQL_str		row,row1;
int		kolstr=0;
int		K=0;
int		pozz=0,pozz1=0;
int		i=0,kom=0;
int		mspz=0;
int		prc;
int             Y=3;
int             X=0;
class iceb_tu_str dataz("");
class iceb_tu_str tabnom("");
class iceb_tu_str kod_nah("");
class iceb_tu_str koment("");
class dirzarsdo_poi poisk;
class iceb_tu_str naim_sp("");
class iceb_tu_str fio("");
int metka_sp=0;
class SQLCURSOR cur,cur1;
class dirzarsdo_metka_str metka_str;

if(tbn != 0)
 {
  poisk.tabnom.new_plus(tbn);
  poisk.metka_poi=1;
 }


//Создаем класс меню
VVOD DANET(1);
VVOD VVOD1(2);

naz1:;

short           kls=LINES-6;


sprintf(strsql,"select * from Zarsdo order by tn asc,dt desc,kn asc");

  

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
lll:;
  dirzarsdo_sap(&poisk);
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == SFK2 || K == ENTER || K == FK3 || K == FK5 || K == FK6)
    goto lll;

  if(K == KEY_RESIZE)
   goto naz1;   

  goto vper;
 }

if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;

naz:;

cur.poz_cursor(pozz);
mspz=2;
pozz1=0;

spis_menu.spis_str.free_class();
for(i=0;i<=kls-1;)
 {
  if(cur.read_cursor(&row) == 0)
    break;

  pozz1++;
 
  if(dirzarsdo_poi(row,&poisk) != 0)
   continue;


  /*читаем фамилию*/
  sprintf(strsql,"select fio from Kartb where tabn=%d",atoi(row[0]));
  if(readkey(strsql,&row1,&cur1) == 1)
   fio.new_plus(row1[0]);
  else
   fio.new_plus("");

  metka_sp=atoi(row[4]);

  naim_sp.new_plus(metka_str.strsr.ravno(metka_sp));
  fio.new_plus(iceb_tu_getfiosin(fio.ravno(),0));
        
  sprintf(strsql,"%-6s|%-*.*s|%s|%3s|%10s|%-*.*s|%5s|%5s|%-6s|%s",
  row[0],
  iceb_tu_kolbait(15,fio.ravno()),
  iceb_tu_kolbait(15,fio.ravno()),
  fio.ravno(),
  iceb_tu_datzap_mg(row[1]),
  row[2],
  row[3],
  iceb_tu_kolbait(10,naim_sp.ravno()),
  iceb_tu_kolbait(10,naim_sp.ravno()),
  naim_sp.ravno(),
  row[5],
  row[6],
  row[7],            
  row[8]);  

  spis_menu.spis_str.plus(strsql);
  i++;
  
  if(pozz1 == 1) /*Первая запись удовлетворяет условиям поиска*/
   mspz=1;

 }

if(i == 0)
 {
  if(kolstr != 0 && pozz > 0)
   {
    pozz--;
    goto naz;
   }
  goto lll;
 }  
if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

dirzarsdo_sap(&poisk);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,Y,X,NULL,strsql,prc,&K,koment.ravno(),COLS-25);


if(kom == -7) //Изменение границы экрана
  goto naz1;

if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  for(i=0; i< mspz   ; )
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
    if(dirzarsdo_poi(row,&poisk) != 0)
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
    if(dirzarsdo_poi(row,&poisk) != 0)
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
  printw("\n вперед pozz=%d kolstr=%ld\n",pozz,kolstr);
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
    if(dirzarsdo_poi(row,&poisk) != 0)
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
   goto naz; /*Стали на конец списка*/

  polen(str_vib.ravno(),&tabnom,1,'|');
  polen(str_vib.ravno(),&dataz,3,'|');
  polen(str_vib.ravno(),&kod_nah,4,'|');

 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return;

  case FK1:  /*Получить подсказку*/
    GDITE();
    iceb_t_pdoc("zarp3_2_3.txt");
    goto naz;

  case ENTER:
  case SFK2:   /*Корректировать*/
    dirzarsdo_vvod(dataz.ravno(),tabnom.ravno(),kod_nah.ravno());
    goto naz1;

  case FK2:  /*Ввести запись*/
  case PLU:
    dirzarsdo_vvod("","","");
    goto naz1;

  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      sprintf(strsql,"delete from Zarsdo where dt='%s' and tn=%d and kn=%d",dataz.ravno_sqldata(),tabnom.ravno_atoi(),kod_nah.ravno_atoi());
      if(sql_zapis(strsql,0,0) == 0)
        goto naz1;
     }
    goto naz;


  case FK4:       /*Поиск*/

     dirzarsdo_vrp(&poisk);

     GDITE();

     clear();
     kom=pozz=0;
     goto naz;

  case FK5:    //Распечатать
    dirzarsdo_ras(&cur,&poisk);
    goto naz;


  default:
    goto naz;
    break;
 }

return;


}
/**********************/
/*Ввод и корректировки*/
/**********************/

void dirzarsdo_vvod(const char *dataz,const char *tabnom,const char *kod_nah)
{
char		strsql[2048];
SQL_str		row;
int		N=0,K=0;
int metka_sp=0; /*метка способа расчёта*/
class iceb_tu_str fio("");
class iceb_tu_str naim_nah("");
class iceb_tu_str kod("");
class iceb_tu_str naim("");
int ktoz=0;
time_t vrem=0;
class SQLCURSOR cur;

class dirzarsdo_menu_mr menu_vrr; /*строки для меню выбора режима расчёта*/


VVOD VV(0);

VV.VVOD_SPISOK_add_data(10);//0
VV.VVOD_SPISOK_add_data(8);//1
VV.VVOD_SPISOK_add_data(5);//2
VV.VVOD_SPISOK_add_data(20);//3
VV.VVOD_SPISOK_add_data(128);//4
VV.VVOD_SPISOK_add_data(5);//5
VV.VVOD_SPISOK_add_data(6);//6
VV.VVOD_SPISOK_add_data(10);//7
VV.VVOD_SPISOK_add_data(100);//8



if(tabnom[0] != '\0')
 {

  sprintf(strsql,"select * from Zarsdo where tn=%d and dt='%s' and kn=%d",atoi(tabnom),iceb_tu_tosqldat(dataz),atoi(kod_nah));
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    printw("%s-%s\n",strsql);
    iceb_t_soob(gettext("Не найдена запись для корректировки!"));
    
    return;   
   }

  VV.VVOD_SPISOK_zapis_data(0,row[0]);
  VV.VVOD_SPISOK_zapis_data(1,iceb_tu_datzap_mg(row[1]));
  VV.VVOD_SPISOK_zapis_data(2,row[2]);
  VV.VVOD_SPISOK_zapis_data(3,row[3]);
  
  metka_sp=atoi(row[4]);
  VV.VVOD_SPISOK_zapis_data(4,menu_vrr.strsr.ravno(atoi(row[4])));
  
  VV.VVOD_SPISOK_zapis_data(5,row[5]);
  VV.VVOD_SPISOK_zapis_data(6,row[6]);
  VV.VVOD_SPISOK_zapis_data(7,row[7]);
  VV.VVOD_SPISOK_zapis_data(8,row[8]);
  ktoz=atoi(row[9]);
  vrem=atol(row[10]);


  sprintf(strsql,"select fio from Kartb where tabn=%d",atoi(tabnom));
  if(readkey(strsql,&row,&cur) == 1)
   fio.new_plus(row[0]);  

  sprintf(strsql,"select naik from Nash where kod=%d",atoi(kod_nah));     
  if(readkey(strsql,&row,&cur) == 1)
   naim_nah.new_plus(row[0]);  
 }
else
 {
  VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой записи"));

  VV.VVOD_SPISOK_zapis_data(4,menu_vrr.strsr.ravno(0));
  VV.VVOD_SPISOK_zapis_data(6,"1");
  VV.VVOD_SPISOK_zapis_data(7,"*");

 }

VV.VVOD_SPISOK_add_MD(gettext("Табельный номер................."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата.(м.г)......................"));//1
VV.VVOD_SPISOK_add_MD(gettext("Код начисления.................."));//2
VV.VVOD_SPISOK_add_MD(gettext("Сумма..........................."));//3
VV.VVOD_SPISOK_add_MD(gettext("Способ расчёта.................."));//4
VV.VVOD_SPISOK_add_MD(gettext("Количество часов в рабочем дне.."));//5
VV.VVOD_SPISOK_add_MD(gettext("Коэффициент ставки.............."));//6
VV.VVOD_SPISOK_add_MD(gettext("Бухгалтерский счёт..(*)........."));//7
VV.VVOD_SPISOK_add_MD(gettext("Коментарий......................"));//8


naz:;

VV.VVOD_delete_ZAG();

if(tabnom[0] == '\0')
  VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой записи"));
else
 {
  VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка записи"));
  kzvz(ktoz,vrem,&VV);
 }

VV.VVOD_SPISOK_add_ZAG(fio.ravno());
VV.VVOD_SPISOK_add_ZAG(naim_nah.ravno());

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("реквизиты"),
"F10",gettext("выход"),NULL);

VV.vramka(-1,-1,0);

for(;;)
 {

  attron(VV.VVOD_return_cs(iceb_CFS)); 

  VV.vprintw(N,stdscr);
  
  K=getch();


  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));

  VV.vprintw(N,stdscr);

  if(K == KEY_RESIZE)
    goto naz;


  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

   
  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));

    VV.vprintw_clear(N,stdscr);

    if(N == 4) //Способ расчёта
     {
      VVOD MENUW(3);
      for(int nom=0; nom < KOL_STR_MENU;nom++)
        MENUW.VVOD_SPISOK_add_MD(menu_vrr.strsr.ravno(nom));

      MENUW.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное"));
      if(menu3w(stdscr,&MENUW,&metka_sp,-1,-1,0) != 0)
       goto naz;

      VV.VVOD_SPISOK_zapis_data(4,menu_vrr.strsr.ravno(metka_sp));
/***************
      switch(metka_sp)
       {
        case 0:
         VV.VVOD_SPISOK_zapis_data(4,menu_vrr.sr[0].ravno());
         break;     
        case 1:
         VV.VVOD_SPISOK_zapis_data(4,menu_vrr.sr[1].ravno());
         break;     
        case 2:
         VV.VVOD_SPISOK_zapis_data(4,menu_vrr.sr[2].ravno());
         break;     
        case 3:
         VV.VVOD_SPISOK_zapis_data(4,menu_vrr.sr[3].ravno());
         break;     
        case 4:
         VV.VVOD_SPISOK_zapis_data(4,menu_vrr.sr[4].ravno());
         break;     
        case 5:
         VV.VVOD_SPISOK_zapis_data(4,menu_vrr.sr[5].ravno());
         break;     
        case 6:
         VV.VVOD_SPISOK_zapis_data(4,menu_vrr.sr[6].ravno());
         break;     
       };
********************/
      goto naz;
            
     }


    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 0 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')
     {
      sprintf(strsql,"select fio from Kartb where tabn=%d",VV.data_ravno_atoi(N));
      
      if(readkey(strsql,&row,&cur) == 1)
       {
        fio.new_plus(row[0]);  
        N++;
       }
      else
       {
        sprintf(strsql,"%s %d",gettext("Не найден табельный номер"),VV.data_ravno_atoi(N));
        iceb_t_soob(strsql);
        VV.data_plus(N,"");
        fio.new_plus("");
       }
      goto naz;
     }

    if(N == 2 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')
     {
      sprintf(strsql,"select naik from Nash where kod=%d",VV.data_ravno_atoi(N));     
      if(readkey(strsql,&row,&cur) == 1)
       {
        naim_nah.new_plus(row[0]);  
        N++;
       }
      else
       {
        sprintf(strsql,"%s %d",gettext("Не найден код начисления в справочнике"),VV.data_ravno_atoi(N));
        iceb_t_soob(strsql);
        VV.data_plus(N,"");
        naim_nah.new_plus("");
       }
      goto naz;
     }

    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    VV.vprintw(N,stdscr);
    N++;
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

  if(K == ESC || K == FK10)
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    N=-1;
    break;
   }

  if(K == FK1) //Помощь
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    GDITE();
//    prosf(strsql);
    clear();
    goto naz;
   }

  if(K == FK2 || K == PLU) //Записываем
   {
    if(dirzarsdo_zap(&VV,dataz,tabnom,kod_nah,metka_sp) != 0)
     goto naz;
    break;
   }

  if(K == FK3)
   {
    int tbn=0;
    class VVOD MENU(3);
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка работников"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка кодов начислений"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр плана счетов"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//3

    clearstr(LINES-1,0);
    
    int kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 3 :
      case -1:
       goto naz;

      case 0:

       kod.new_plus("");
       if(dirtab(&tbn,&kod,0,1) == 0)
        {
         VV.data_plus(0,tbn);
         fio.new_plus(kod.ravno());
        }       
       clear();
       goto naz;

     case 1:
       naim.new_plus("");
       kod.new_plus("");
       if(dirnach(1,&kod,&naim) == 0)
        {
         VV.data_plus(2,kod.ravno());
         naim_nah.new_plus(naim.ravno());
        }       
       clear();
       goto naz;
       
      case 2 :
        kod.new_plus("");
        if(vibrek("Plansh",&kod) == 0)
          VV.VVOD_SPISOK_zapis_data(7,kod.ravno());
        clear();
        goto naz;
     }

   }
 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
clear();
refresh();
return;
}

/*********************/
/*Запись             */
/*********************/
//Если вернули 0 - записали 1-нет

int   dirzarsdo_zap(VVOD *VV,const char *dataz,const char *tabnom,const char *kod_nah,int metka_sp)
{
char	strsql[2048];
short m,g;

attroff(VV->VVOD_return_cs(iceb_CFS));
attron(VV->VVOD_return_cs(iceb_CFM));

if(VV->VVOD_SPISOK_return_data(0)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(1)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(2)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(3)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
  return(1);
 }       


sprintf(strsql,"select fio from Kartb where tabn=%d",VV->data_ravno_atoi(0));
if(readkey(strsql) != 1)
 {
  sprintf(strsql,"%s %d",gettext("Не найден табельный номер"),VV->data_ravno_atoi(0));
  iceb_t_soob(strsql);
  return(1);
 }

sprintf(strsql,"select naik from Nash where kod=%d",VV->data_ravno_atoi(2));
if(readkey(strsql) != 1)
 {
  sprintf(strsql,"%s %d",gettext("Не найден код начисления в справочнике"),VV->data_ravno_atoi(2));
  iceb_t_soob(strsql);
  return(1);
 }

if(rsdat1(&m,&g,VV->data_ravno(1)) != 0)
 {
  iceb_t_soob(gettext("Не правильно введена дата!"));
  return(1);
 }

if(VV->data_ravno(7)[0] != '\0' && VV->data_ravno(7)[0] != '*')
 {
  if(prsh1(VV->data_ravno(7)) != 0)
   return(1);
 }


if(SRAV(tabnom,VV->data_ravno(0),0) != 0)
 {
  //Проверяем может такая запись уже есть
  sprintf(strsql,"select dt from Zarsdo where dt='%s' and tn=%d and kn=%d",iceb_tu_tosqldat(dataz),atoi(tabnom),atoi(kod_nah));
  if(readkey(strsql) >= 1)
   {
    iceb_t_soob(gettext("Такая запись уже есть!"));
    return(1);
   }

 }
 
if(tabnom[0] == '\0') //Ввод новой записи
  sprintf(strsql,"insert into Zarsdo values(%d,'%s',%d,%.2f,%d,%.2f,%.2f,'%s','%s',%d,%ld)",
  VV->data_ravno_atoi(0),  
  iceb_tu_tosqldat(VV->data_ravno(1)),
  VV->data_ravno_atoi(2),
  VV->data_ravno_atof(3),
  metka_sp,
  VV->data_ravno_atof(5),
  VV->data_ravno_atof(6),
  VV->data_ravno_filtr(7),
  VV->data_ravno_filtr(8),
  iceb_t_getuid(),time(NULL));
else
 {
  class iceb_tu_str datan(iceb_tu_tosqldat(dataz)); /*только так*/
  sprintf(strsql,"update Zarsdo set \
dt='%s',\
tn=%d,\
kn=%d,\
sm=%.2f,\
mt=%d,\
kh=%.2f,\
ks=%.2f,\
sh='%s',\
km='%s',\
ktoz=%d,\
vrem=%ld \
where dt='%s' and tn=%d and kn=%d",  
  iceb_tu_tosqldat(VV->data_ravno(1)),
  VV->data_ravno_atoi(0),  
  VV->data_ravno_atoi(2),
  VV->data_ravno_atof(3),
  metka_sp,
  VV->data_ravno_atof(5),
  VV->data_ravno_atof(6),
  VV->data_ravno_filtr(7),
  VV->data_ravno_filtr(8),
  iceb_t_getuid(),
  time(NULL),
  datan.ravno(),
  atoi(tabnom),
  atoi(kod_nah));
 }

if(sql_zapis(strsql,0,0) != 0)
 return(1);



return(0);


}


/**************************/
/*Шапка                   */
/**************************/

void	dirzarsdo_sap(class dirzarsdo_poi *poisk)
{
short		y=2;
short           x=COLS-20;



clear();
printw(gettext("Ввод и корректировка списка должностных окладов"));

                         
mvprintw(2,0,"    1          2            3     4      5          6        7     8     9    10");
mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,"1 -%s",gettext("табельный ном."));
mvprintw(y++,x,"2 -%s",gettext("фамилия"));
mvprintw(y++,x,"3 -%s",gettext("дата"));
mvprintw(y++,x,"4 -%s",gettext("код начисления"));
mvprintw(y++,x,"5 -%s",gettext("сумма"));
mvprintw(y++,x,"6 -%s",gettext("метка"));
mvprintw(y++,x,"7 -%s",gettext("кол.часов"));
mvprintw(y++,x,"8 -%s",gettext("коэф.ставки"));
mvprintw(y++,x,"9 -%s",gettext("счёт"));
mvprintw(y++,x,"10-%s",gettext("коментарий"));

if(poisk->metka_poi == 1)
 {
  y++;
  attron(A_BLINK);
  mvprintw(y++,x,gettext("Поиск:"));
 
  if(poisk->tabnom.getdlinna() > 1)
   mvprintw(y++,x,"%s:%s",gettext("Таб.н."),poisk->tabnom.ravno());
  if(poisk->dataz.getdlinna() > 1)
   mvprintw(y++,x,"%s:%s",gettext("Дата"),poisk->dataz.ravno());
  if(poisk->kod_nah.getdlinna() > 1)
   mvprintw(y++,x,"%s:%s",gettext("Код н."),poisk->kod_nah.ravno());
  if(poisk->shet.getdlinna() > 1)
   mvprintw(y++,x,"%s:%s",gettext("Счёт"),poisk->shet.ravno());
  if(poisk->fio.getdlinna() > 1)
   mvprintw(y++,x,"%s:%s",gettext("Фамилия"),poisk->fio.ravno());
  if(poisk->koment.getdlinna() > 1)
   mvprintw(y++,x,"%s:%s",gettext("Комент."),poisk->koment.ravno());
  if(poisk->podr.getdlinna() > 1)
   mvprintw(y++,x,"%s:%s",gettext("Подр."),poisk->podr.ravno());

  if(poisk->sposob_r.getdlinna() > 1)
  {
   class dirzarsdo_metka_str metka_str;
   mvprintw(y++,x,"%s:%s",gettext("Способ рас."),metka_str.strsr.ravno(poisk->kod_sr));

  }  
 
  attroff(A_BLINK);
  
 }

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F10",gettext("выход"),NULL);


}

/********************/
/*Распечатать       */
/********************/

void	dirzarsdo_ras(SQLCURSOR *cur,class dirzarsdo_poi *poisk)
{
char	imaf[64];
SQL_str row,row1;
class SQLCURSOR cur1;
char strsql[1024];
class iceb_tu_str fio("");
short d,m,g;
int metka_sp=0;
class iceb_tu_str naim_sp("");
class dirzarsdo_metka_str metka_str;

sprintf(imaf,"sdo%d.lst",getpid());

class iceb_fopen fil;
if(fil.start(imaf,"w") != 0)
 return;

iceb_t_zagolov(gettext("Список должностных окладов"),fil.ff);

if(poisk->tabnom.getdlinna() > 1)
 fprintf(fil.ff,"%s:%s\n",gettext("Табельный номер"),poisk->tabnom.ravno());
if(poisk->dataz.getdlinna() > 1)
 fprintf(fil.ff,"%s:%s\n",gettext("Дата"),poisk->dataz.ravno());
if(poisk->kod_nah.getdlinna() > 1)
 fprintf(fil.ff,"%s:%s\n",gettext("Код начисления"),poisk->kod_nah.ravno());
if(poisk->fio.getdlinna() > 1)
 fprintf(fil.ff,"%s:%s\n",gettext("Фамилия"),poisk->fio.ravno());
if(poisk->koment.getdlinna() > 1)
 fprintf(fil.ff,"%s:%s\n",gettext("Коментарий"),poisk->koment.ravno());
if(poisk->shet.getdlinna() > 1)
 fprintf(fil.ff,"%s:%s\n",gettext("Счёт"),poisk->shet.ravno());
if(poisk->podr.getdlinna() > 1)
 fprintf(fil.ff,"%s:%s\n",gettext("Код подразделения"),poisk->podr.ravno());

if(poisk->sposob_r.getdlinna() > 1)
 {
  class dirzarsdo_menu_mr menu_mr;
    
  fprintf(fil.ff,"%s:%s\n",gettext("Способ расчёта"),menu_mr.strsr.ravno(poisk->kod_sr));
 }
  
fprintf(fil.ff,"\
------------------------------------------------------------------------------------------------------------\n");
fprintf(fil.ff,gettext("\
 Т.н. |          Фамилия             |Дата   |К.н.|  Сумма   |Способ расчёта|Кол.ч.|Коэф.| Счёт |Коментарий\n"));

/*
123456 123456789012345678901234567890 00.0000 1234 1234567890 12345678901234 123456 12345 123456
*/
fprintf(fil.ff,"\
------------------------------------------------------------------------------------------------------------\n");

cur->poz_cursor(0);

while(cur->read_cursor(&row) != 0)
 {
  if(dirzarsdo_poi(row,poisk) != 0)
    continue;

  sprintf(strsql,"select fio from Kartb where tabn=%d",atoi(row[0]));
  if(readkey(strsql,&row1,&cur1) == 1)
   fio.new_plus(row1[0]);

  rsdat(&d,&m,&g,row[1],2);
  
  metka_sp=atoi(row[4]);

  naim_sp.new_plus(metka_str.strsr.ravno(metka_sp));


  fprintf(fil.ff,"%6s %-*.*s %02d.%04d %4s %10s %-*.*s %6s %5s %6s %s\n",
  row[0],
  iceb_tu_kolbait(30,fio.ravno()),  
  iceb_tu_kolbait(30,fio.ravno()),  
  fio.ravno(),
  m,g,
  row[2],
  row[3],
  iceb_tu_kolbait(14,naim_sp.ravno()),
  iceb_tu_kolbait(14,naim_sp.ravno()),
  naim_sp.ravno(),
  row[5],
  row[6],
  row[7],
  row[8]);
    
 }
fprintf(fil.ff,"\
------------------------------------------------------------------------------------------------------------\n");

podpis(fil.ff);
fil.end();

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Список должностных окладов"));

iceb_t_ustpeh(imaf,3);

iceb_t_rabfil(&oth,"");

}
/************************************/
/*проверка на условия поиска*/
/*****************************/
int dirzarsdo_poi(SQL_str row,class dirzarsdo_poi *poi)
{
if(poi->metka_poi == 0)
 return(0);

//  printw("s-%s\n",__FUNCTION__);
//  refresh();    
 
if(proverka(poi->tabnom.ravno(),row[0],0,0) != 0)
 return(1);

if(proverka(poi->shet.ravno(),row[7],1,0) != 0)
 return(1);

if(proverka(poi->koment.ravno(),row[8],4,0) != 0)
 return(1);

if(proverka(poi->kod_nah.ravno(),row[2],0,0) != 0)
 return(1);

if(poi->dataz.getdlinna() > 1)
 {
  short d=0,m=0,g=0;
  short mp=0,gp=0;
  
  rsdat(&d,&m,&g,row[1],2);

  rsdat1(&mp,&gp,poi->dataz.ravno());
  if(m != mp || g != gp)
   return(1);  
 }


if(poi->fio.getdlinna() > 1)
 {
  SQL_str row1;
  class SQLCURSOR cur1;
  char strsql[512];
  class iceb_tu_str fio("");


  sprintf(strsql,"select fio from Kartb where tabn=%d",atoi(row[0]));

  if(readkey(strsql,&row1,&cur1) == 1)
   fio.new_plus(row1[0]);

  if(proverka(poi->fio.ravno(),fio.ravno(),4,0) != 0)
   return(1);
   
 } 
if(poi->podr.getdlinna() > 1)
 {
  SQL_str row1;
  class SQLCURSOR cur1;
  char strsql[512];
  class iceb_tu_str podr("");


  sprintf(strsql,"select podr from Kartb where tabn=%d",atoi(row[0]));

  if(readkey(strsql,&row1,&cur1) == 1)
   podr.new_plus(row1[0]);

  if(proverka(poi->podr.ravno(),podr.ravno(),0,0) != 0)
   return(1);

 }

if(poi->sposob_r.getdlinna() > 1)
 {
  if(atoi(row[4]) != poi->kod_sr)
   return(1);
 }

return(0);
}
/******************************/
/*Ввод реквизитов поиска*/
/***************************/
int dirzarsdo_vrp(class dirzarsdo_poi *poi)
{
int N=0,K=0;
class iceb_tu_str kod("");
class iceb_tu_str naim("");
class dirzarsdo_menu_mr menu_vrr; 

VVOD VV(0);

VV.VVOD_SPISOK_add_data(poi->tabnom.ravno(),128);//0
VV.VVOD_SPISOK_add_data(poi->dataz.ravno(),8);//1
VV.VVOD_SPISOK_add_data(poi->kod_nah.ravno(),128);//2
VV.VVOD_SPISOK_add_data(poi->shet.ravno(),128);//3
VV.VVOD_SPISOK_add_data(poi->koment.ravno(),128);//4
VV.VVOD_SPISOK_add_data(poi->fio.ravno(),128);//5
VV.VVOD_SPISOK_add_data(poi->podr.ravno(),128);//6
VV.VVOD_SPISOK_add_data(poi->sposob_r.ravno(),128);//6

VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой записи"));

VV.VVOD_SPISOK_add_MD(gettext("Табельный номер...(,,)."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата.(м.г)............."));//1
VV.VVOD_SPISOK_add_MD(gettext("Код начисления....(,,)."));//2
VV.VVOD_SPISOK_add_MD(gettext("Счёт..............(,,)."));//3
VV.VVOD_SPISOK_add_MD(gettext("Коментарий........(,,)."));//4
VV.VVOD_SPISOK_add_MD(gettext("Фамилия...........(,,)."));//5
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения.(,,)."));//6
VV.VVOD_SPISOK_add_MD(gettext("Способ расчёта........."));//7

naz:;

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("реквизиты"),
"F10",gettext("выход"),NULL);

VV.vramka(-1,-1,0);

for(;;)
 {

  attron(VV.VVOD_return_cs(iceb_CFS)); 

  VV.vprintw(N,stdscr);
  
  K=getch();


  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));

  VV.vprintw(N,stdscr);

  if(K == KEY_RESIZE)
    goto naz;


  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

     
  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));

    if(N == 7) /*способ расчёта*/
     {
      VVOD MENUW(3);
      for(int nom=0; nom < KOL_STR_MENU;nom++)
        MENUW.VVOD_SPISOK_add_MD(menu_vrr.strsr.ravno(nom));
      MENUW.VVOD_SPISOK_add_MD(gettext("Очистить строку"));

      MENUW.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное"));
      if(menu3w(stdscr,&MENUW,&poi->kod_sr,-1,-1,0) != 0)
       goto naz;

      if(poi->kod_sr >= menu_vrr.strsr.kolih())
         VV.VVOD_SPISOK_zapis_data(7,"");
      else
         VV.VVOD_SPISOK_zapis_data(7,menu_vrr.strsr.ravno(poi->kod_sr));

      continue;
     }

    VV.vprintw_clear(N,stdscr);


    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));



    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    VV.vprintw(N,stdscr);
    N++;
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

  if(K == ESC || K == FK10)
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    N=-1;
    poi->metka_poi=0;
    break;
   }

  if(K == FK1) //Помощь
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    GDITE();
//    prosf(strsql);
    clear();
    goto naz;
   }

  if(K == FK2 || K == PLU) //Записываем
   {
    poi->metka_poi=1;

    poi->tabnom.new_plus(VV.data_ravno(0));
    poi->dataz.new_plus(VV.data_ravno(1));
    poi->kod_nah.new_plus(VV.data_ravno(2));
    poi->shet.new_plus(VV.data_ravno(3));
    poi->koment.new_plus(VV.data_ravno(4));
    poi->fio.new_plus(VV.data_ravno(5));
    poi->sposob_r.new_plus(VV.data_ravno(7));
    break;
   }

  if(K == FK3)
   {
    int tbn=0;
    class VVOD MENU(3);
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка работников"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка кодов начислений"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр плана счетов"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));//3
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//4

    clearstr(LINES-1,0);
    
    int kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 4 :
      case -1:
       goto naz;

      case 0:

       kod.new_plus("");
       if(dirtab(&tbn,&kod,0,1) == 0)
        {
         VV.data_z_plus(0,tbn);
        }       
       clear();
       goto naz;

     case 1:
       naim.new_plus("");
       kod.new_plus("");
       if(dirnach(1,&kod,&naim) == 0)
        {
         VV.data_z_plus(2,kod.ravno());
        }       
       clear();
       goto naz;
       
      case 2 :
        kod.new_plus("");
        if(vibrek("Plansh",&kod) == 0)
          VV.data_z_plus(3,kod.ravno());
        clear();
        goto naz;

      case 3 :
        kod.new_plus("");
        naim.new_plus("");
        if(dirzarpodr(2,&kod,&naim) == 0)
          VV.data_z_plus(6,kod.ravno());
        clear();
        goto naz;
     }

   }

 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
clear();
refresh();

return(0);
}
