/*$Id: diruospod.c,v 5.15 2013/05/17 14:55:59 sasa Exp $*/
/*14.07.2015	29.11.2008	Белых А.И.	diruospod.c
Ввод и корректировка списка подразделений
*/
#include <errno.h>
#include <ctype.h>
#include "buhg.h"

void diruospod_sap(int metkar,const char *naimpoi);
void diruospod_kk(int skod,int nkod);
int diruospod_zap(const char *kod_k,int,class VVOD *VV);
void diruospod_vk(const char *kod_mo);
void diruospod_rs(class SQLCURSOR *cur,const char *naim_poi);
int diruospod_pvu(int kod);


int diruospod(int metkar, /*0-работа 1-выбор из списка с корректировкой 2-чистый выбор*/
class iceb_tu_str *kod,
class iceb_tu_str *naim,
int metka_isp_mo)  /*0-все  1-только используемые 2-только неиспользуемые*/
{
char strsql[1024];
SQL_str		row;
int		kolstr=0;
int		K=0;
int		pozz=0,pozz1=0;
int		i=0,kom=0;
int		mspz=0;
int		prc;
int             Y=3;
int             X=0;
class iceb_tu_str kod_vibr("");
class iceb_tu_str naim_vibr("");
class iceb_t_mstr spis_menu(0);
class iceb_tu_str metka_sost("");
class iceb_tu_str naimpoi("");
class iceb_tu_str koment("");

if(metkar == 2)
 {
  Y=-1;
  X=-1;
  koment.new_plus(gettext("Выберите нужное и нажмите Enter"));
 }

//Создаем класс меню
class VVOD DANET(1);
class VVOD VVOD1(2);

naz1:;

short           kls=LINES-6;

sprintf(strsql,"select * from Uospod order by kod asc");
if(metka_isp_mo == 1)
 sprintf(strsql,"select * from Uospod where mi=0 order by kod asc");
if(metka_isp_mo == 2)
 sprintf(strsql,"select * from Uospod where mi=1 order by kod asc");

class SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
lll:;

  diruospod_sap(metkar,naimpoi.ravno());

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

  if(naimpoi.ravno()[0] != '\0' && strstrm(row[1],naimpoi.ravno()) == 0)
   continue;
  
  metka_sost.new_plus("");
  if(atoi(row[4]) == 1)
   metka_sost.new_plus("*");
  
  sprintf(strsql,"%-5s|%-*.*s|%s",
  row[0],
  iceb_tu_kolbait(30,row[1]),iceb_tu_kolbait(30,row[1]),row[1],
  metka_sost.ravno());
  
  i++;
  spis_menu.spis_str.plus(strsql);
  
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


diruospod_sap(metkar,naimpoi.ravno());

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,Y,X,NULL,strsql,prc,&K,koment.ravno(),0);


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
    if(naimpoi.ravno()[0] != '\0' && strstrm(row[1],naimpoi.ravno()) == 0)
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
    if(naimpoi.ravno()[0] != '\0' && strstrm(row[1],naimpoi.ravno()) == 0)
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
    if(naimpoi.ravno()[0] != '\0' && strstrm(row[1],naimpoi.ravno()) == 0)
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

  polen(str_vib.ravno(),&kod_vibr,1,'|');
  polen(str_vib.ravno(),&naim_vibr,2,'|');

 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return(1);
     break;

  case FK1:  /*Получить подсказку*/
    GDITE();
    iceb_t_pdoc("uos5_3.txt");
    goto naz;

  case ENTER:
    if(metkar != 0)
     {
      kod->new_plus(kod_vibr.ravno());
      naim->new_plus(naim_vibr.ravno());
      return(0);
     } 

  case SFK2:   /*Корректировать*/
    diruospod_vk(kod_vibr.ravno());
    goto naz1;

  case FK2:  /*Ввести запись*/
  case PLU:
    diruospod_vk("");
    goto naz1;

  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      if(diruospod_pvu(kod_vibr.ravno_atoi()) != 0)
        goto naz;
      sprintf(strsql,"delete from Uospod where kod=%d",kod_vibr.ravno_atoi());
      sql_zapis(strsql,0,0);
     }
    goto naz1;

  case FK4:       /*Поиск*/

     helstr(LINES-1,0," F10 ",gettext("выход"),NULL);

     VVOD1.VVOD_delete();
     VVOD1.VVOD_SPISOK_add_MD(gettext("Введите образец для поиска"));
     
     naimpoi.new_plus("");
     
     vvod1(&naimpoi,112,&VVOD1,NULL,stdscr,-1,-1);

     GDITE();

     clear();
     kom=pozz=0;
     goto naz;

  case FK5:    //Распечатать
    diruospod_rs(&cur,naimpoi.ravno());
    goto naz;

  case FK7:    /*Переключение 0-все 1-неиспользуемые 2-используемые*/
    metka_isp_mo++;
    if(metka_isp_mo > 2)
     metka_isp_mo=0;

    goto naz1;

  default:
    goto naz;
    break;
 }

return(1);




}
/*************************/
/*Шапка*/
/********************/

void diruospod_sap(int metkar,const char *naimpoi)
{
short		y=2;
short           x=COLS-20;



if(metkar != 2)
 {
  clear();
  printw(gettext("Ввод и корректировка подразделений"));
 }
if(metkar == 1)
 {
  move(1,0);
  printw(gettext("Найдите нужное и нажмите \"Enter\""));
 }

if(metkar < 2)
 {
  mvprintw(2,0,"   1       2");
  mvprintw(y++,x,gettext("Наименование полей:"));
  mvprintw(y++,x,"1-%s",gettext("код"));
  mvprintw(y++,x,"2-%s",gettext("подразделение"));
  mvprintw(y+=2,x,"*-%s",gettext("неисп."));

  if(naimpoi[0] != '\0')
   {
    y++;
    attron(A_BLINK);
    mvprintw(y++,x,gettext("Поиск:"));
    mvprintw(y++,x,"%s",naimpoi);
    attroff(A_BLINK);
    
   }

  helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F7",gettext("состояние"),
"F10",gettext("выход"),NULL);
 }

if(metkar == 2)
  helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
  "F10",gettext("выход"),NULL);

}
/*******************************/
/*Ввод и корректировка материально-ответственных*/
/*********************************************/
void diruospod_vk(const char *kod_mo)
{
char		strsql[512];
int		N=1,K=0;
SQL_str		row;
class SQLCURSOR cur;
int metka_is=0;

class VVOD VV(0);

VV.VVOD_SPISOK_add_data(10);//0
VV.VVOD_SPISOK_add_data(60);//1
VV.VVOD_SPISOK_add_data(40);//2


if(kod_mo[0] != '\0')
 {
  sprintf(strsql,"select * from Uospod where kod=%s",kod_mo);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    iceb_t_soob(gettext("Не найдена запись для корректировки!"));
    return;   
   }
  VV.VVOD_SPISOK_zapis_data(0,row[0]);
  VV.VVOD_SPISOK_zapis_data(1,row[1]);

  metka_is=atoi(row[4]);
  VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка записи"));
  kzvz(atoi(row[2]),atol(row[3]),&VV);

 }
else
 {

  VV.VVOD_SPISOK_add_ZAG(gettext("Ввод нового подразделения"));


  sprintf(strsql,"%d",oprsk("Uospod"));
  VV.VVOD_SPISOK_zapis_data(0,strsql);
 }


VV.VVOD_SPISOK_add_MD(gettext("Код подразделения..........."));//0
VV.VVOD_SPISOK_add_MD(gettext("Наименование подразделения.."));//1
VV.VVOD_SPISOK_add_MD(gettext("Состояние..................."));//2

naz:;

if(metka_is == 0)
   VV.data_plus(2,gettext("Используется"));
else
   VV.data_plus(2,gettext("Неиспользуется"));

 helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F10",gettext("выход"),NULL);


VV.vramka(2,0,0);

for(;;)
 {

  attron(VV.VVOD_return_cs(iceb_CFS)); 
  VV.vprintw(N,stdscr);
  
  K=getch();


  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));

  VV.vprintw(N,stdscr);

  if(K == KEY_RESIZE)
   {
//    getch();
    goto naz;
   }


  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

   
  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));
    VV.vprintw_clear(N,stdscr);

    if(N == 2) /*Состояние материально-ответственного*/
     {
      VVOD MENUW(3);
      MENUW.VVOD_SPISOK_add_MD(gettext("Используеться"));
      MENUW.VVOD_SPISOK_add_MD(gettext("Неиспользуется"));
     
      MENUW.VVOD_SPISOK_add_ZAG(gettext("Выберите состояние подразделения"));
      menu3w(stdscr,&MENUW,&metka_is,-1,-1,0);
      goto naz;
            
     }

//    my_getstr(VV.VVOD_SPISOK_return_data(N),dlinastr,VV.VVOD_SPISOK_return_dls(N),stdscr);
    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 1) 
     {

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
    if(diruospod_zap(kod_mo,metka_is,&VV) != 0)
     goto naz;
    break;
   }

 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
clear();
refresh();


}
/*************************/
/*Запись */
/*************************/
int diruospod_zap(const char *kod_k,int metka_is,class VVOD *VV)
{

if(VV->data_ravno(0)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введён код!"));

  return(1);
 }
if(VV->data_ravno(1)[0] == '\0')
 {
  iceb_t_soob(gettext("Не ввели наименование!"));
  return(1);
 }

if(isdigit(VV->data_ravno(0)[0]) == 0)  /*Значит символ не число*/
 {
  iceb_t_soob(gettext("Код должен быть цифровым !"));
  return(1);
 }
if(VV->data_ravno_atoi(0) == 0)
 {
  iceb_t_soob(gettext("Код не может быть нулевым !"));
  return(1);
 }



char strsql[512];

if(SRAV(kod_k,VV->data_ravno(0),0) != 0)
 {
  sprintf(strsql,"select naik from Uospod where kod=%d",VV->data_ravno_atoi(0));
  if(readkey(strsql) > 0)
   {
    iceb_t_soob(gettext("Такой код уже введён!"));
    return(1);
   }

  if(kod_k[0] != '\0')
   {
    VVOD DANET(1);
    DANET.VVOD_SPISOK_add_MD(gettext("Корректировать код ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      return(1);
   }

 }

if(kod_k[0] == '\0') //Ввод новой записи
  sprintf(strsql,"insert into Uospod values(%d,'%s',%d,%ld,%d)",
  VV->data_ravno_atoi(0),
  VV->data_ravno_filtr(1),
  iceb_t_getuid(),
  time(NULL),
  metka_is);
else
  sprintf(strsql,"replace into Uospod values(%d,'%s',%d,%ld,%d)",
  VV->data_ravno_atoi(0),
  VV->data_ravno_filtr(1),
  iceb_t_getuid(),
  time(NULL),
  metka_is);

if(sql_zapis(strsql,0,0) != 0)
 return(1);

if(kod_k[0] != '\0' && SRAV(kod_k,VV->data_ravno(0),0) != 0)
 diruospod_kk(atoi(kod_k),VV->data_ravno_atoi(0));

return(0);

}
/*************************/
/*Корректировка кода материально-ответственного*/
/***********************************************/
void diruospod_kk(int skod,int nkod)
{
char strsql[512];

VVOD SOOBN(1);
SOOBN.VVOD_SPISOK_add_MD(gettext("Выполняется каскадная корректировка кода в таблицах !"));
soobsh(&SOOBN,-1,-1,1);

GDITE();

sprintf(strsql,"update Uosdok set podr=%d where podr=%d",nkod,skod);
if(sql_zapis(strsql,1,0) != 0)
  return;


sprintf(strsql,"update Uosdok set podrv=%d where podrv=%d",nkod,skod);
sql_zapis(strsql,1,0);

sprintf(strsql,"update Uosdok1 set podr=%d where podr=%d",nkod,skod);
sql_zapis(strsql,1,0);

sprintf(strsql,"update Uosamor set podr=%d where podr=%d",nkod,skod);
sql_zapis(strsql,1,0);

sprintf(strsql,"update Uosamor1 set podr=%d where podr=%d",nkod,skod);
sql_zapis(strsql,1,0);

/*Удаляем старый код так как для записи применялось replace*/
sprintf(strsql,"delete from Uospod where kod=%d",skod);
sql_zapis(strsql,1,0);

}


/**************************/
/*Распечатка*/
/**************************/
void diruospod_rs(class SQLCURSOR *cur,const char *naim_poi)
{
char	imaf[64];
FILE	*ff;
SQL_str row;

sprintf(imaf,"uospod%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }


iceb_t_zagolov(gettext("Список подразделений в подсистеме \"Учёт основных средств\""),0,0,0,0,0,0,ff);

fprintf(ff,"\
-----------------------------------------------------\n");
fprintf(ff,gettext(" Код | Наименованиe подразделения\n"));

fprintf(ff,"\
-----------------------------------------------------\n");


cur->poz_cursor(0);

while(cur->read_cursor(&row) != 0)
 {
  if(naim_poi[0] != '\0' && strstrm(row[1],naim_poi) == 0)
   continue;
  fprintf(ff,"%5s %s\n",row[0],row[1]);
 }

fprintf(ff,"\
-----------------------------------------------------\n");

podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Список подразделений в подсистеме \"Учёт основных средств\""));
iceb_t_ustpeh(imaf,3);
iceb_t_rabfil(&oth,"");


}
/*******************************/
/*Проверка возможности удаления*/
/*******************************/
/*Если вернули 0-удалять можно*/
int diruospod_pvu(int kod)
{

char	strsql[1024];

sprintf(strsql,"select poldr from Uosdok where poldr=%d limit 1",kod);
if(sql_readkey(&bd,strsql) > 0)
 {
  sprintf(strsql,"%s Uosdok",gettext("Запись удалить невозможно, код используется в таблице"));
  iceb_t_soob(strsql);

  return(1);
 }

sprintf(strsql,"select poldrv from Uosdok where poldrv=%d limit 1",kod);
if(sql_readkey(&bd,strsql) > 0)
 {
  sprintf(strsql,"%s Uosdok",gettext("Запись удалить невозможно, код используется в таблице"));
  iceb_t_soob(strsql);

  return(1);
 }


sprintf(strsql,"select podr from Uosdok1 where podr=%d limit 1",kod);
if(sql_readkey(&bd,strsql) > 0)
 {
  sprintf(strsql,"%s Uosdok1",gettext("Запись удалить невозможно, код используется в таблице"));
  iceb_t_soob(strsql);
  return(1);
 }

sprintf(strsql,"select podr from Uosamor where podr=%d limit 1",kod);
if(sql_readkey(&bd,strsql) > 0)
 {
  sprintf(strsql,"%s Uosamor",gettext("Запись удалить невозможно, код используется в таблице"));
  iceb_t_soob(strsql);
  return(1);
 }


sprintf(strsql,"select podr from Uosamor1 where podr=%d limit 1",kod);
if(sql_readkey(&bd,strsql) > 0)
 {
  sprintf(strsql,"%s Uosamor1",gettext("Запись удалить невозможно, код используется в таблице"));
  iceb_t_soob(strsql);
  return(1);
 }

return(0);
}






