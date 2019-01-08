/*$Id: dirrashod.c,v 5.7 2013/09/26 09:43:30 sasa Exp $*/
/*25.01.2017	06.12.2011	Белых А.И.	dirrashod.c
Работа со справочником расходов в подсистеме "Материальный учёт"
*/
#include <errno.h>
#include "buhg.h"

void dirrashod_sap(int metka_rr,const char *poisk);
int dirrashod_vz(const char *kod);
void dirrashod_kk(const char *skod,const char *nkod);
int dirrashod_pvu(const char *kod);
void dirrashod_ras(class SQLCURSOR *cur,const char *poisk);


int dirrashod(int metka_rr, /*0-ввод и корректировка 1-выбор*/
class iceb_tu_str *kod_op,class iceb_tu_str *naim_kod)
{
class iceb_t_mstr spis_menu(0);
char strsql[1024];
int  Y=3;
int X=0;
int kolstr=0;
int K=0;
int pozz=0,pozz1=0;
int mspz=0;
SQL_str row;
int prc;
int kom=0;
int i=0;
class iceb_tu_str yesno("");
class iceb_tu_str vido("");
class SQLCURSOR cur;
class iceb_tu_str poisk("");
class iceb_tu_str kod_opv("");
//Создаем класс меню
VVOD DANET(1);
VVOD VVOD1(2);

naz1:;


short           kls=LINES-6;

sprintf(strsql,"select * from Rashod order by kod asc");
  

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
lll:;

   
  dirrashod_sap(metka_rr,poisk.ravno());

  K=iceb_t_soob(gettext("Не найдено ни одной записи !"));


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

  if(proverka(poisk.ravno(),row[1],4,0) != 0)
   continue;

  vido.new_plus("");
  if(atoi(row[4]) == 0)
    vido.new_plus(gettext("Внешняя"));
  if(atoi(row[4]) == 1)
    vido.new_plus(gettext("Внутренняя"));
  if(atoi(row[4]) == 2)
    vido.new_plus(gettext("Изм.реквиз."));
       
  if(atoi(row[6]) == 0)
   yesno.new_plus(gettext("Да"));
  else
   yesno.new_plus(gettext("Нет"));
    
  sprintf(strsql,"%-*s|%-*.*s|%-*.*s|%-*s|%-*s|%s",
  iceb_tu_kolbait(4,row[0]),row[0],
  iceb_tu_kolbait(33,row[1]),
  iceb_tu_kolbait(33,row[1]),
  row[1],
  iceb_tu_kolbait(10,vido.ravno()),
  iceb_tu_kolbait(10,vido.ravno()),
  vido.ravno(),
  iceb_tu_kolbait(4,yesno.ravno()),
  yesno.ravno(),
  iceb_tu_kolbait(4,row[7]),
  row[7],
  row[5]);


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


dirrashod_sap(metka_rr,poisk.ravno());

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,Y,X,NULL,strsql,prc,&K,"",COLS-24);


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
/****
    if(poisk.naimpoi.ravno()[0] != '\0' && strstrm(row[5],poisk.naimpoi.ravno()) == 0)
     continue;
********/
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
/*******
    if(poisk.naimpoi.ravno()[0] != '\0' && strstrm(row[5],poisk.naimpoi.ravno()) == 0)
     continue;
*******/
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
/*********
    if(poisk.naimpoi.ravno()[0] != '\0' && strstrm(row[5],poisk.naimpoi.ravno()) == 0)
     continue;
**************/
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

  polen(spis_menu.spis_str.ravno(kom),&kod_opv,1,'|');
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
    iceb_t_pdoc("matu4_5.txt");
    goto naz;

  case PLU:
  case FK2:  
    dirrashod_vz("");
    goto naz1;

  case ENTER: 
    if(metka_rr != 0)
     {
      sprintf(strsql,"select naik from Rashod where kod='%s'",kod_opv.ravno());
      if(readkey(strsql,&row,&cur) == 1)
       naim_kod->new_plus(row[0]);
      kod_op->new_plus(kod_opv.ravno());
      return(0);
     }
  case SFK2: 
    
    dirrashod_vz(kod_opv.ravno());
    goto naz1;

  case FK3:       /*Удалить запись*/
    if(dirrashod_pvu(kod_opv.ravno()) != 0)
     goto naz;

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      goto naz;

    sprintf(strsql,"delete from Rashod where kod='%s'",kod_opv.ravno());
    sql_zapis(strsql,1,0);
    goto naz1;

  case FK4:       /*Отобрать по образцу*/

     helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
     
     poisk.new_plus("");
     
     VVOD1.VVOD_delete();
     VVOD1.VVOD_SPISOK_add_MD(gettext("Введите образец для поиска"));

     if(vvod1(&poisk,40,&VVOD1,NULL,stdscr,-1,-1) == FK10)
       poisk.new_plus("");

     GDITE();

     goto naz1;

  case FK5:       /*Распечатать*/
   dirrashod_ras(&cur,poisk.ravno());
    goto naz;

  default:
    goto naz;
    break;
 }

return(1);

}
/*********/
/*Шапка*/
/*********/
void dirrashod_sap(int metka_rr,const char *poisk)
{
short		y=2;
short           x=COLS-20;

clear();
printw(gettext("Справочник операций расходов"));
if(metka_rr == 1)
 mvprintw(1,0,gettext("Выберите нужную операцию"));

mvprintw(2,0,"   1                   2                     3       4    5    6");
mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,"1-%s",gettext("код операции"));
mvprintw(y++,x,"2-%s",gettext("наименование"));
mvprintw(y++,x,"3-%s",gettext("вид операции"));
mvprintw(y++,x,"4-%s",gettext("проводки"));
mvprintw(y++,x,"5-%s",gettext("контрагент"));
mvprintw(y++,x,"6-%s",gettext("cчета"));

y+=2;

if(poisk[0] != '\0')
 {
  mvprintw(y++,x,"%s:",gettext("Поиск"));
  mvprintw(y++,x,"%s",poisk);
 }

helstr(LINES-1,0,
"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F5",gettext("печать"),
"F10",gettext("выход"),NULL);
 

}
/***********************/
/*Ввод и корректировка записи*/
/*****************************/

int dirrashod_vz(const char *kod_opv)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
class iceb_tu_str kodn("");
class iceb_tu_str naim("");
class iceb_tu_str yesno(gettext("Да"));
class iceb_tu_str vido(gettext("Внешняя"));
class iceb_tu_str sheta("");
class iceb_tu_str kod_kontr("");
int N=0,K=0;
int metka_vido=0;
int metka_prov=0;
VVOD VV(0);

if(kod_opv[0] != '\0') /*Корректировка записи*/
 {
  sprintf(strsql,"select * from Rashod where kod='%s'",kod_opv);
  if(readkey(strsql,&row,&cur) != 1)
   {
    iceb_t_soob(gettext("Не найдена запись для корректировки!"));
    return(1);
   }
  kodn.new_plus(kod_opv);
  naim.new_plus(row[1]);

  metka_vido=atoi(row[4]);
  if(metka_vido == 0)
   vido.new_plus(gettext("Внешняя"));

  if(metka_vido == 1)
   vido.new_plus(gettext("Внутренняя"));

  if(metka_vido == 2)
   vido.new_plus(gettext("Изменение реквизитов учёта"));

  sheta.new_plus(row[5]);   

  if(atoi(row[6]) == 0)
    yesno.new_plus(gettext("Да"));
  else
    yesno.new_plus(gettext("Нет"));
  
  VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка записи"));
  kzvz(row[2],row[3],&VV);
  kod_kontr.new_plus(row[7]);
 }
else
 kod_kontr.new_plus("00");

VV.VVOD_SPISOK_add_MD(gettext("Код операции.."));//0
VV.VVOD_SPISOK_add_MD(gettext("Наименование.."));//1
VV.VVOD_SPISOK_add_MD(gettext("Вид операции.."));//2
VV.VVOD_SPISOK_add_MD(gettext("Проводки......"));//3
VV.VVOD_SPISOK_add_MD(gettext("Счёта учёта..."));//4
VV.VVOD_SPISOK_add_MD(gettext("Код контра-та."));//5

VV.VVOD_SPISOK_add_data(kodn.ravno(),16);
VV.VVOD_SPISOK_add_data(naim.ravno(),60);
VV.VVOD_SPISOK_add_data(vido.ravno(),60);
VV.VVOD_SPISOK_add_data(yesno.ravno(),8);
VV.VVOD_SPISOK_add_data(sheta.ravno(),99);
VV.VVOD_SPISOK_add_data(kod_kontr.ravno(),16);



naz:;

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);



helstr(LINES-1,0,"F2/+",gettext("запись"),
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

    if(N == 2) /*Вид операции*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFM));
      attron(VV.VVOD_return_cs(iceb_CB));

      VVOD MENUW(3);

      MENUW.VVOD_SPISOK_add_MD(gettext("Внешняя"));
      MENUW.VVOD_SPISOK_add_MD(gettext("Внутренняя"));
      MENUW.VVOD_SPISOK_add_MD(gettext("Изменение реквизитов учёта"));

      MENUW.VVOD_SPISOK_add_ZAG(gettext("Выберите вид операции"));
      metka_vido=0;
      if(menu3w(stdscr,&MENUW,&metka_vido,-1,-1,0) != 0)
       goto naz;

      switch (metka_vido)
       {
        case -1 :
         break;

        case 0 :
         VV.VVOD_SPISOK_zapis_data(2,gettext("Внешняя"));
         break;

        case 1 :
         VV.VVOD_SPISOK_zapis_data(2,gettext("Внутренняя"));
         break;

        case 2 :
         VV.VVOD_SPISOK_zapis_data(2,gettext("Изменение реквизитов учёта"));
         break;
       }

      
      goto naz;
            
     }

    if(N == 3) /*проводки*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFM));
      attron(VV.VVOD_return_cs(iceb_CB));

      VVOD MENUW(3);

      sprintf(strsql,"%s (%s)",gettext("Да"),gettext("Проводки должны выполняться"));
      MENUW.VVOD_SPISOK_add_MD(strsql);

      sprintf(strsql,"%s (%s)",gettext("Нет"),gettext("Проводки недолжны выполняться"));
      MENUW.VVOD_SPISOK_add_MD(strsql);

      MENUW.VVOD_SPISOK_add_ZAG(gettext("Должны ли выполняться проводки для этого удержания ?"));
      metka_prov=0;
      if(menu3w(stdscr,&MENUW,&metka_prov,-1,-1,0) != 0)
       goto naz;

      switch (metka_prov)
       {
        case -1 :
         break;

        case 0 :
         VV.VVOD_SPISOK_zapis_data(3,gettext("Да"));
         break;

        case 1 :
         VV.VVOD_SPISOK_zapis_data(3,gettext("Нет"));
         break;
       }

      
      goto naz;
            
     }

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


    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CB));
    return(1);
   }

  if(K == FK2 || K == PLU) /*Записываем*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    break;
   }
 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
//clear();
refresh();

if(VV.data_ravno(0)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введён код!"));
  goto naz;
 }
if(VV.data_ravno(1)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введёно наименование!"));
  goto naz;
 }

if(VV.data_ravno(5)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введён код контрагента!"));
  goto naz;
 }
/*проверяем код контрагента*/
sprintf(strsql,"select kodkon from Kontragent where kodkon='%s'",VV.data_ravno(5));
if(readkey(strsql) != 1)
 {
  sprintf(strsql,"%s %s %s!",gettext("Нет кода контрагента"),VV.data_ravno(5),gettext("в списке контрагентов"));
  iceb_t_soob(strsql);
  goto naz;
 }

if(SRAV(kod_opv,VV.data_ravno(0),0) != 0)
 {
  sprintf(strsql,"select kod from Rashod where kod='%s'",VV.data_ravno(0));
  if(readkey(strsql) == 1)
   {
    iceb_t_soob(gettext("Такой код операции уже есть!"));
    goto naz;
   }

 }



int metka_kor_kod=0;

if(kod_opv[0] == '\0') /*Ввод новой записи*/
 {
  sprintf(strsql,"insert into Rashod values ('%s','%s',%d,%ld,%d,'%s',%d,'%s')",
  VV.data_ravno(0),
  VV.data_ravno_filtr(1),
  iceb_t_getuid(),time(NULL),
  metka_vido,
  VV.data_ravno(4),
  metka_prov,
  VV.data_ravno(5));

 }
else /*Корректировка существующей*/
 {
  if(SRAV(kod_opv,VV.data_ravno(0),0) != 0)
   {
    VVOD DANET(1);
    DANET.VVOD_SPISOK_add_MD(gettext("Корректировать код? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      goto naz;
    metka_kor_kod=1;   
   }  

  sprintf(strsql,"update Rashod set \
kod='%s',\
naik='%s',\
ktoz=%d, \
vrem=%ld, \
vido=%d,\
sheta='%s',\
prov=%d,\
kk='%s' \
where kod='%s'",
  VV.data_ravno(0),
  VV.data_ravno_filtr(1),
  iceb_t_getuid(),time(NULL),
  metka_vido,
  VV.data_ravno(4),
  metka_prov,
  VV.data_ravno(5),
  kod_opv);

 }

if(sql_zapis(strsql,1,0) != 0)
 goto naz;

if(metka_kor_kod == 1)
  dirrashod_kk(kod_opv,VV.data_ravno(0));

if(kod_opv[0] == '\0')
 if(metka_vido > 0) /*Пишем парную запись в приходные операции*/
  {
   sprintf(strsql,"insert into Prihod values ('%s','%s',%d,%ld,%d,%d)",
   VV.data_ravno(0),
   VV.data_ravno_filtr(1),
   iceb_t_getuid(),time(NULL),
   metka_vido,
   1);

   sql_zapis(strsql,1,0);
  }

return(0);

}
/******************************************/
/*Корректировка кода удержания в таблицах*/
/*******************************************/
void dirrashod_kk(const char *skod,const char *nkod)
{
char strsql[512];
VVOD SOOBN(1);
SOOBN.VVOD_SPISOK_add_MD(gettext("Выполняется каскадная корректировка кода в таблицах !"));
soobsh(&SOOBN,-1,-1,1);


sprintf(strsql,"update Dokummat set kodop='%s' where kodop='%s' and tip=2",nkod,skod);
if(sql_zapis(strsql,1,0) != 0)
 return;

sprintf(strsql,"update Prov set oper='%s' where oper='%s' and tz=2 and kto='%s'",nkod,skod,ICEB_MP_MATU);
sql_zapis(strsql,1,0);

sprintf(strsql,"update Reenn set ko='%s' where ko='%s' and mi=1",nkod,skod);
sql_zapis(strsql,1,0);


}
/*********************************/
/*Проверка возможности удаления*/
/********************************/
int dirrashod_pvu(const char *kod)
{
char strsql[512];
sprintf(strsql,"select kodop from Dokummat where tip=2 and kodop='%s' limit 1",kod);

if(readkey(strsql) > 0)
 {
  sprintf(strsql,"%s Dokummat !",gettext("Удалить нельзя, потому, что есть записи в таблице"));
  iceb_t_soob(strsql);
  return(1);
 }

sprintf(strsql,"select ko from Reenn where ko='%s' and mi=1  limit 1",kod);

if(readkey(strsql) > 0)
 {
  sprintf(strsql,"%s Reenn !",gettext("Удалить нельзя, потому, что есть записи в таблице"));
  iceb_t_soob(strsql);
  return(1);
 }

sprintf(strsql,"select oper from Prov where oper='%s' and tz=2 and kto='%s' limit 1",kod,ICEB_MP_MATU);

if(readkey(strsql) > 0)
 {
  sprintf(strsql,"%s Prov !",gettext("Удалить нельзя, потому, что есть записи в таблице"));
  iceb_t_soob(strsql);
  return(1);
 }

return(0);
}
/********************************************/
/*Распечатка */
/***********************************/
void dirrashod_ras(class SQLCURSOR *cur,const char *poisk)
{
SQL_str row;
FILE *ff;
char imaf[64];
class iceb_tu_str danet("");
class iceb_tu_str vido("");
sprintf(imaf,"kodopr%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
  {
   error_op_nfil(imaf,errno,"");
   return;
  }

iceb_t_zagolov(gettext("Справочник кодов операций расходов"),ff);
fprintf(ff,"%s:%s\n",gettext("Подсистема"),gettext("Материальный учёт"));
if(poisk[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Поиск"),poisk);
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
 Код |Наименование операции         |Вид операц| Проводки |Конт|    Счета учёта     |Дата и время записи| Кто записал\n"));
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------\n");

/*
12345 123456789012345678901234567890 1234567890 1234567890
*/
cur->poz_cursor(0);
while(cur->read_cursor(&row) != 0)
 {
  if(proverka(poisk,row[1],4,0) != 0)
   continue;

  vido.new_plus("");
  if(atoi(row[4]) == 0)
    vido.new_plus(gettext("Внешняя"));
  if(atoi(row[4]) == 1)
    vido.new_plus(gettext("Внутренняя"));
  if(atoi(row[4]) == 2)
    vido.new_plus(gettext("Изм.стоим."));

  if(atoi(row[6]) == 0)
   danet.new_plus("Да");
  else
   danet.new_plus("Нет");
  
  fprintf(ff,"%-*s|%-*.*s|%-*s|%-*s|%-*s|%-*s %s %s\n",
  iceb_tu_kolbait(5,row[0]),row[0],
  iceb_tu_kolbait(30,row[1]),
  iceb_tu_kolbait(30,row[1]),
  row[1],
  iceb_tu_kolbait(10,vido.ravno()),
  vido.ravno(),
  iceb_tu_kolbait(10,danet.ravno()),
  danet.ravno(),
  iceb_tu_kolbait(4,row[7]),
  row[7],
  iceb_tu_kolbait(20,row[5]),
  row[5],
  iceb_tu_vremzap(row[3]),
  iceb_t_kszap(row[2]));

  for(int nom=30; nom < iceb_tu_strlen(row[1]); nom+=30)
   fprintf(ff,"%5s|%-*.*s|%10s|%10s|%4s|\n",
   "",
   iceb_tu_kolbait(30,iceb_tu_adrsimv(nom,row[1])),
   iceb_tu_kolbait(30,iceb_tu_adrsimv(nom,row[1])),
   iceb_tu_adrsimv(nom,row[1]),
   "","","");
 }
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------\n");
podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Справочник кодов операций расходов"));
iceb_t_ustpeh(oth.spis_imaf.ravno(0),3);
iceb_t_rabfil(&oth,"");
 

}
