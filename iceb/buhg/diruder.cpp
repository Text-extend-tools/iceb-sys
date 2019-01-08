/*$Id: diruder.c,v 5.9 2014/08/31 06:18:18 sasa Exp $*/
/*14.07.2015	14.10.2010	Белых А.И.	diruder.c
Работа со справочником удержаний
*/
#include <errno.h>
#include "buhg.h"

void diruder_sap(int metkar,const char *poisk);
int diruder_vz(int kod);
void diruder_kk(int skod,int nkod);
int diruder_pvu(int kod);
void diruder_ras(class SQLCURSOR *cur,const char *poisk);


int diruder(int metkar, //0-работа 1-выбор из списка с корректировкой 2-чистый выбор
class iceb_tu_str *kod_ud,class iceb_tu_str *naim_kod)
{
class iceb_t_mstr spis_menu(0);
char strsql[2048];
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
class SQLCURSOR cur;
class iceb_tu_str poisk("");
class iceb_tu_str koment("");
class iceb_tu_str naim_vib("");

int kod_udv=0;
//Создаем класс меню
VVOD DANET(1);
VVOD VVOD1(2);
if(metkar == 2)
 {
  Y=-1;
  X=-1;
  koment.new_plus(gettext("Выберите нужное и нажмите Enter"));
 }

naz1:;


short           kls=LINES-6;

sprintf(strsql,"select * from Uder order by kod asc");
  

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
lll:;

   
  diruder_sap(metkar,poisk.ravno());

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

  if(atoi(row[5]) == 0)
   yesno.new_plus(gettext("Да"));
  else
   yesno.new_plus(gettext("Нет"));

  if(metkar == 2)
   sprintf(strsql,"%3s|%s",row[0],row[1]);
  else    
   sprintf(strsql,"%3s|%-*s|%-*s|%-*s|%s",
   row[0],
   iceb_tu_kolbait(6,row[2]),
   row[2],
   iceb_tu_kolbait(6,row[6]),
   row[6],
   iceb_tu_kolbait(3,yesno.ravno()),
   yesno.ravno(),
   row[1]);
      
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


diruder_sap(metkar,poisk.ravno());

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,Y,X,NULL,strsql,prc,&K,koment.ravno(),COLS-26);


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
    if(proverka(poisk.ravno(),row[1],4,0) != 0)
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
    if(proverka(poisk.ravno(),row[1],4,0) != 0)
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
    if(cur.read_cursor(&row) == 0)
      break;
    if(proverka(poisk.ravno(),row[1],4,0) != 0)
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

  polen(str_vib.ravno(),&kod_udv,1,'|');

  if(metkar == 2)
   polen(str_vib.ravno(),&naim_vib,2,'|');
  else
   polen(str_vib.ravno(),&naim_vib,5,'|');
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
    iceb_t_pdoc("zarp3_1_4.txt");
    goto naz;

  case FK2:  
    diruder_vz(0);
    goto naz1;

  case ENTER: 
    if(metkar != 0)
     {
      kod_ud->new_plus(kod_udv);
      naim_kod->new_plus(naim_vib.ravno());
      return(0);
     }

  case SFK2: 
    
    diruder_vz(kod_udv);
    goto naz1;

  case FK3:       /*Удалить запись*/
    if(diruder_pvu(kod_udv) != 0)
     goto naz;

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      goto naz;

    sprintf(strsql,"delete from Uder where kod=%d",kod_udv);
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
   diruder_ras(&cur,poisk.ravno());
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
void diruder_sap(int metkar,const char *poisk)
{
short		y=2;
short           x=COLS-20;
if(metkar != 2)
 {
  clear();
  printw(gettext("Cписок удержаний"));
 }
if(metkar == 1)
 {
  move(1,0);
  printw(gettext("Найдите нужное и нажмите \"Enter\""));
 }

if(metkar < 2)
 {



  mvprintw(2,0,"   1     2      3    4      5");
  mvprintw(y++,x,gettext("Наименование полей:"));
  mvprintw(y++,x,"1-%s",gettext("код удержания"));
  mvprintw(y++,x,"2-%s",gettext("счёт хоз-ный"));
  mvprintw(y++,x,"3-%s",gettext("счёт бюд-ный"));
  mvprintw(y++,x,"4-%s",gettext("проводки"));
  mvprintw(y++,x,"5-%s",gettext("наименование"));

  y+=2;

  if(poisk[0] != '\0')
   {
    mvprintw(y++,x,"%s:",gettext("Поиск"));
    mvprintw(y++,x,"%s",poisk);
   }

  helstr(LINES-1,0,
  //"F1",gettext("помощь"),
  "F2/+",gettext("запись"),
  "F3",gettext("удалить"),
  "F5",gettext("печать"),
  "F10",gettext("выход"),NULL);
 } 

if(metkar == 2)
  helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
  "F10",gettext("выход"),NULL);

}
/***********************/
/*Ввод и корректировка записи*/
/*****************************/

int diruder_vz(int kod_udv)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
class iceb_tu_str kodn("");
class iceb_tu_str naim("");
class iceb_tu_str shet_hoz("");
class iceb_tu_str shet_bud("");
class iceb_tu_str yesno(gettext("Да"));
int N=0,K=0;
int kom1=0;

VVOD VV(0);

if(kod_udv != 0) /*Корректировка записи*/
 {
  sprintf(strsql,"select * from Uder where kod=%d",kod_udv);
  if(readkey(strsql,&row,&cur) != 1)
   {
    iceb_t_soob(gettext("Не найдена запись для корректировки!"));
    return(1);
   }
  kodn.new_plus(kod_udv);
  naim.new_plus(row[1]);
  shet_hoz.new_plus(row[2]);
  shet_bud.new_plus(row[6]);

  if(atoi(row[5]) == 0)
    yesno.new_plus(gettext("Да"));
  else
    yesno.new_plus(gettext("Нет"));
  
  VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка записи"));
  kzvz(row[3],row[4],&VV);
 }
else
 {
  VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой записи"));
  int nom=0;
  for(nom=1; ;nom++)
   {
    sprintf(strsql,"select kod from Uder where kod=%d",nom);
    if(readkey(strsql) == 0)
     break;  
   }
  kodn.new_plus(nom);
 }


VV.VVOD_SPISOK_add_MD(gettext("Код удержания.."));
VV.VVOD_SPISOK_add_MD(gettext("Наименование..."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт хоз-ный..."));
VV.VVOD_SPISOK_add_MD(gettext("Проводки......."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт бюд-ный..."));

VV.VVOD_SPISOK_add_data(kodn.ravno(),16);
VV.VVOD_SPISOK_add_data(naim.ravno(),60);
VV.VVOD_SPISOK_add_data(shet_hoz.ravno(),12);
VV.VVOD_SPISOK_add_data(yesno.ravno(),8);
VV.VVOD_SPISOK_add_data(shet_bud.ravno(),12);



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
    goto naz;

  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));

    VV.vprintw_clear(N,stdscr);

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
      kom1=0;
      if(menu3w(stdscr,&MENUW,&kom1,-1,-1,0) != 0)
       goto naz;

      switch (kom1)
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

  if(K == FK4)
   {
    VV.VVOD_clear_data();
    goto naz;
   }
 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
clear();
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
if(VV.data_ravno(2)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введён счёт!"));
  goto naz;
 }

struct OPSHET opshet;
if(prsh1(VV.data_ravno(2),&opshet) != 0)
 goto naz;

if(VV.data_ravno(4)[0] != '\0')
 if(prsh1(VV.data_ravno(4),&opshet) != 0)
  goto naz;

if(kod_udv != VV.data_ravno_atoi(0))
 {
  sprintf(strsql,"select kod from Uder where kod=%d",VV.data_ravno_atoi(0));
  if(readkey(strsql) == 1)
   {
    iceb_t_soob(gettext("Такой код удержания уже есть!"));
    goto naz;
   }

 }

int prov_dn=0;
if(SRAV(VV.data_ravno(3),gettext("Да"),0) == 0)
 prov_dn=0;
else
 prov_dn=1;


int metka_kor_kod=0;

if(kod_udv == 0) /*Ввод новой записи*/
 {
  sprintf(strsql,"insert into Uder values (%d,'%s','%s',%d,%ld,%d,'%s')",
  VV.data_ravno_atoi(0),
  VV.data_ravno_filtr(1),
  VV.data_ravno_filtr(2),
  iceb_t_getuid(),time(NULL),
  prov_dn,
  VV.data_ravno_filtr(4));
  
 }
else /*Корректировка существующей*/
 {
  if(kod_udv != VV.data_ravno_atoi(0))
   {
    VVOD DANET(1);
    DANET.VVOD_SPISOK_add_MD(gettext("Корректировать код? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      goto naz;
    metka_kor_kod=1;   
   }  

  sprintf(strsql,"update Uder set \
kod=%d,\
naik='%s',\
shet='%s',\
ktoz=%d, \
vrem=%ld, \
prov=%d, \
shetb='%s' \
where kod=%d",
  VV.data_ravno_atoi(0),
  VV.data_ravno_filtr(1),
  VV.data_ravno_filtr(2),
  iceb_t_getuid(),time(NULL),
  prov_dn,
  VV.data_ravno_filtr(4),
  kod_udv);
 }

if(sql_zapis(strsql,1,0) != 0)
 goto naz;

if(metka_kor_kod == 1)
  diruder_kk(kod_udv,VV.data_ravno_atoi(0));

return(0);

}
/******************************************/
/*Корректировка кода удержания в таблицах*/
/*******************************************/
void diruder_kk(int skod,int nkod)
{
char strsql[512];
VVOD SOOBN(1);
SOOBN.VVOD_SPISOK_add_MD(gettext("Выполняется каскадная корректировка кода в таблицах !"));
soobsh(&SOOBN,-1,-1,1);

sprintf(strsql,"update Zarp set knah=%d where prn='2' and knah=%d",nkod,skod);
if(sql_zapis(strsql,1,0) != 0)
 return;

sprintf(strsql,"update Zarn1 set knah=%d where prn='2' and knah=%d",nkod,skod);
sql_zapis(strsql,1,0);


}
/*********************************/
/*Проверка возможности удаления*/
/********************************/
int diruder_pvu(int kod)
{
char strsql[512];
sprintf(strsql,"select knah from Zarp where prn='2' and knah=%d limit 1",kod);

if(readkey(strsql) > 0)
 {
  sprintf(strsql,"%s Zarp !",gettext("Удалить нельзя, потому, что есть записи в таблице"));
  iceb_t_soob(strsql);
  return(1);
 }

sprintf(strsql,"select knah from Zarn1 where prn='2' and knah=%d limit 1",kod);

if(readkey(strsql) > 0)
 {
  sprintf(strsql,"%s Zarn1 !",gettext("Удалить нельзя, потому, что есть записи в таблице"));
  iceb_t_soob(strsql);
  return(1);
 }
sprintf(strsql,"select ku from Zarrud where ku=%d limit 1",kod);

if(readkey(strsql) > 0)
 {
  sprintf(strsql,"%s Zarrud !",gettext("Удалить нельзя, потому, что есть записи в таблице"));
  iceb_t_soob(strsql);
  return(1);
 }
return(0);
}
/********************************************/
/*Распечатка */
/***********************************/
void diruder_ras(class SQLCURSOR *cur,const char *poisk)
{
SQL_str row;
FILE *ff;
char imaf[56];
class iceb_tu_str danet("");

sprintf(imaf,"uder%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
  {
   error_op_nfil(imaf,errno,"");
   return;
  }

iceb_t_zagolov(gettext("Список удержаний"),ff);

if(poisk[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Поиск"),poisk);
fprintf(ff,"\
----------------------------------------------------------------------\n");
fprintf(ff,gettext("\
 Код |Наименование удержания        |Счёт хозр.|Счёт бюдж.| Проводки |\n"));
fprintf(ff,"\
----------------------------------------------------------------------\n");

/*
12345 123456789012345678901234567890
*/
cur->poz_cursor(0);
while(cur->read_cursor(&row) != 0)
 {
  if(proverka(poisk,row[1],4,0) != 0)
   continue;

  if(atoi(row[5]) == 0)
   danet.new_plus("Да");
  else
   danet.new_plus("Нет");
  
  fprintf(ff,"%5s|%-*.*s|%-*s|%-*s|%s\n",
  row[0],
  iceb_tu_kolbait(30,row[1]),
  iceb_tu_kolbait(30,row[1]),
  row[1],
  iceb_tu_kolbait(10,row[2]),
  row[2],
  iceb_tu_kolbait(10,row[6]),
  row[6],
  danet.ravno());  
  
 }
fprintf(ff,"\
----------------------------------------------------------------------\n");
podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Список удержаний"));
iceb_t_ustpeh(oth.spis_imaf.ravno(0),3);
iceb_t_rabfil(&oth,"");
 

}
