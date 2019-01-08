/*$Id: diruder.c,v 5.9 2014/08/31 06:18:18 sasa Exp $*/
/*13.03.2018	21.09.2014	Белых А.И.	iceb_tl_ei.c
Работа со справочником единиц измерения
*/
#include <errno.h>
#include "buhl.h"


void iceb_tl_ei_sap(int metkar,const char *poisk);
int iceb_tl_ei_vz(const char *kod);
void iceb_tl_ei_kk(const char *skod,const char *nkod);
int iceb_tl_ei_pvu(const char *kod);
void iceb_tl_ei_ras(class SQLCURSOR *cur,const char *poisk);


int iceb_tl_ei(int metkar, //0-работа 1-выбор из списка с корректировкой 2-чистый выбор
class iceb_tu_str *ed_izmer,class iceb_tu_str *naim_kod)
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
class iceb_tu_str naim_vib("");
class iceb_tu_str koment("");
class iceb_tu_str ed_izmerv("");
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

sprintf(strsql,"select * from Edizmer order by kod asc");
  

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
lll:;

   
  iceb_tl_ei_sap(metkar,poisk.ravno());

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
  

  sprintf(strsql,"%-*s|%-*s|%s",
  iceb_tu_kolbait(10,row[0]),  
  row[0],
  iceb_tu_kolbait(5,row[4]),
  row[4],
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


iceb_tl_ei_sap(metkar,poisk.ravno());

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

  polen(str_vib.ravno(),&ed_izmerv,1,'|');
  polen(str_vib.ravno(),&naim_vib,3,'|');
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
    iceb_t_pdoc("matu4_2.txt");
    goto naz;

  case FK2:  
    iceb_tl_ei_vz("");
    goto naz1;

  case ENTER: 
    if(metkar != 0)
     {
      ed_izmer->new_plus(ed_izmerv.ravno());
      naim_kod->new_plus(naim_vib.ravno());
      return(0);
     }

  case SFK2: 
    
    iceb_tl_ei_vz(ed_izmerv.ravno());
    goto naz1;

  case FK3:       /*Удалить запись*/
    if(iceb_tl_ei_pvu(ed_izmerv.ravno()) != 0)
     goto naz;

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      goto naz;

    sprintf(strsql,"delete from Edizmer where kod='%s'",ed_izmerv.ravno_filtr());
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
   iceb_tl_ei_ras(&cur,poisk.ravno());
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
void iceb_tl_ei_sap(int metkar,const char *poisk)
{
short		y=2;
short           x=COLS-20;


if(metkar != 2)
 {
  clear();
  printw(gettext("Список единиц измерения"));
 }
if(metkar == 1)
 {
  move(1,0);
  printw(gettext("Найдите нужное и нажмите \"Enter\""));
 }

if(metkar < 2)
 {



  mvprintw(2,0,"       1       2     3");
  mvprintw(y++,x,gettext("Наименование полей:"));
  mvprintw(y++,x,"1-%s",gettext("короткое наимен."));
  mvprintw(y++,x,"2-%s",gettext("код"));
  mvprintw(y++,x,"3-%s",gettext("полное наименование"));

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
 
if(metkar == 2)
  helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
  "F10",gettext("выход"),NULL);
 

}
/***********************/
/*Ввод и корректировка записи*/
/*****************************/

int iceb_tl_ei_vz(const char *ed_izmerv)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
class iceb_tu_str kodn("");
class iceb_tu_str naim("");
class iceb_tu_str kodei("");
int N=0,K=0;

VVOD VV(0);

if(ed_izmerv[0] != '\0') /*Корректировка записи*/
 {
  sprintf(strsql,"select * from Edizmer where kod='%s'",sql_escape_string(ed_izmerv,0));
  if(readkey(strsql,&row,&cur) != 1)
   {
    iceb_t_soob(gettext("Не найдена запись для корректировки!"));
    return(1);
   }
  kodn.new_plus(ed_izmerv);
  naim.new_plus(row[1]);
  kodei.new_plus(row[4]);
    
  VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка записи"));
  kzvz(row[2],row[3],&VV);
 }
else
  VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой записи"));


VV.VVOD_SPISOK_add_MD(gettext("Короткое наименование.."));//0
VV.VVOD_SPISOK_add_MD(gettext("Полное наименование...."));//1
VV.VVOD_SPISOK_add_MD(gettext("Код...................."));//2

VV.VVOD_SPISOK_add_data(kodn.ravno(),10);
VV.VVOD_SPISOK_add_data(naim.ravno(),60);
VV.VVOD_SPISOK_add_data(kodei.ravno(),5);



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


if(SRAV(ed_izmerv,VV.data_ravno(0),0) != 0)
 {
  sprintf(strsql,"select kod from Edizmer where kod='%s'",VV.data_ravno_filtr(0));
  if(readkey(strsql) == 1)
   {
    iceb_t_soob(gettext("Такая единица измерения уже есть!"));
    goto naz;
   }

 }


int metka_kor_kod=0;

if(ed_izmerv[0]  == '\0') /*Ввод новой записи*/
 {
  sprintf(strsql,"insert into Edizmer values ('%s','%s',%d,%ld,'%s')",
  VV.data_ravno_filtr(0),
  VV.data_ravno_filtr(1),
  iceb_t_getuid(),time(NULL),
  VV.data_ravno_filtr(2));
  
 }
else /*Корректировка существующей*/
 {
  if(SRAV(ed_izmerv,VV.data_ravno(0),0) != 0)
   {
    VVOD DANET(1);
    DANET.VVOD_SPISOK_add_MD(gettext("Корректировать код? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      goto naz;
    metka_kor_kod=1;   
   }  

  sprintf(strsql,"update Edizmer set \
kod='%s',\
naik='%s',\
ktoz=%d,\
vrem=%ld,\
kei='%s' \
where kod='%s'",
  VV.data_ravno_filtr(0),
  VV.data_ravno_filtr(1),
  iceb_t_getuid(),time(NULL),
  VV.data_ravno_filtr(2),
  sql_escape_string(ed_izmerv,0));
 }

if(sql_zapis(strsql,1,0) != 0)
 goto naz;

if(metka_kor_kod == 1)
  iceb_tl_ei_kk(sql_escape_string(ed_izmerv,0),VV.data_ravno_filtr(0));

return(0);

}
/******************************************/
/*Корректировка кода удержания в таблицах*/
/*******************************************/
void iceb_tl_ei_kk(const char *skod,const char *nkod)
{
char strsql[1024];
VVOD SOOBN(1);
SOOBN.VVOD_SPISOK_add_MD(gettext("Выполняется каскадная корректировка кода в таблицах !"));
soobsh(&SOOBN,-1,-1,1);

sprintf(strsql,"update Material set ei='%s' where ei='%s'",nkod,skod);
if(sql_zapis(strsql,1,0) != 0)
 return;

sprintf(strsql,"update Roznica set ei='%s' where ei='%s'",nkod,skod);
sql_zapis(strsql,1,0);


sprintf(strsql,"update Dokummat1 set ei='%s' where ei='%s'",nkod,skod);
sql_zapis(strsql,1,0);

sprintf(strsql,"update Dokummat3 set ei='%s' where ei='%s'",nkod,skod);
sql_zapis(strsql,1,0);

sprintf(strsql,"update Kart set ei='%s' where ei='%s'",nkod,skod);
sql_zapis(strsql,1,0);

sprintf(strsql,"update Uslugi set ei='%s' where ei='%s'",nkod,skod);
sql_zapis(strsql,1,0);

sprintf(strsql,"update Usldokum1 set ei='%s' where ei='%s'",nkod,skod);
sql_zapis(strsql,1,0);

sprintf(strsql,"update Usldokum2 set ei='%s' where ei='%s'",nkod,skod);
sql_zapis(strsql,1,0);

sprintf(strsql,"update Ukrkras set eiz='%s' where eiz='%s'",nkod,skod);
sql_zapis(strsql,1,0);

sprintf(strsql,"update Ukrdok1 set ediz='%s' where ediz='%s'",nkod,skod);
sql_zapis(strsql,1,0);

}
/*********************************/
/*Проверка возможности удаления*/
/********************************/
int iceb_tl_ei_pvu(const char *kod)
{
char strsql[1024];

sprintf(strsql,"select ei from Uplavt1 where ei='%s' limit 1",sql_escape_string(kod,0));

if(readkey(strsql) > 0)
 {
  sprintf(strsql,"%s Uplavt1 !",gettext("Удалить нельзя, потому, что есть записи в таблице"));
  iceb_t_soob(strsql);
  return(1);
 }

sprintf(strsql,"select ei from Upldok3 where ei='%s' limit 1",sql_escape_string(kod,0));
if(readkey(strsql) > 0)
 {
  sprintf(strsql,"%s Upldok3 !",gettext("Удалить нельзя, потому, что есть записи в таблице"));
  iceb_t_soob(strsql);
  return(1);
 }

sprintf(strsql,"select kodm,ei from Material where ei='%s' limit 1",sql_escape_string(kod,0));
if(readkey(strsql) > 0)
 {
  sprintf(strsql,"%s Material !",gettext("Удалить нельзя, потому, что есть записи в таблице"));
  iceb_t_soob(strsql);
  return(1);
 }



sprintf(strsql,"select ei from Dokummat1 where ei='%s' limit 1",sql_escape_string(kod,0));
if(readkey(strsql) > 0)
 {
  sprintf(strsql,"%s Dokummat1 !",gettext("Удалить нельзя, потому, что есть записи в таблице"));
  iceb_t_soob(strsql);
  return(1);
 }

sprintf(strsql,"select ei from Dokummat3 where ei='%s' limit 1",sql_escape_string(kod,0));
if(readkey(strsql) > 0)
 {
  sprintf(strsql,"%s Dokummat3 !",gettext("Удалить нельзя, потому, что есть записи в таблице"));
  iceb_t_soob(strsql);
  return(1);
 }


sprintf(strsql,"select ei from Kart where ei='%s' limit 1",sql_escape_string(kod,0));
if(readkey(strsql) > 0)
 {
  sprintf(strsql,"%s Kart !",gettext("Удалить нельзя, потому, что есть записи в таблице"));
  iceb_t_soob(strsql);
  return(1);
 }

sprintf(strsql,"select ei from Uslugi where ei='%s' limit 1",sql_escape_string(kod,0));
if(readkey(strsql) > 0)
 {
  sprintf(strsql,"%s Uslugi !",gettext("Удалить нельзя, потому, что есть записи в таблице"));
  iceb_t_soob(strsql);
  return(1);
 }

sprintf(strsql,"select ei from Usldokum1 where ei='%s' limit 1",sql_escape_string(kod,0));
if(readkey(strsql) > 0)
 {
  sprintf(strsql,"%s Usldokum1 !",gettext("Удалить нельзя, потому, что есть записи в таблице"));
  iceb_t_soob(strsql);
  return(1);
 }

sprintf(strsql,"select ei from Usldokum2 where ei='%s' limit 1",sql_escape_string(kod,0));
if(readkey(strsql) > 0)
 {
  sprintf(strsql,"%s Usldokum2 !",gettext("Удалить нельзя, потому, что есть записи в таблице"));
  iceb_t_soob(strsql);
  return(1);
 }

sprintf(strsql,"select ei from Roznica where ei='%s' limit 1",sql_escape_string(kod,0));
if(readkey(strsql) > 0)
 {
  sprintf(strsql,"%s Roznica !",gettext("Удалить нельзя, потому, что есть записи в таблице"));
  iceb_t_soob(strsql);
  return(1);
 }


sprintf(strsql,"select eiz from Ukrkras where eiz='%s' limit 1",sql_escape_string(kod,0));
if(readkey(strsql) > 0)
 {
  sprintf(strsql,"%s Ukrkras !",gettext("Удалить нельзя, потому, что есть записи в таблице"));
  iceb_t_soob(strsql);
  return(1);
 }

sprintf(strsql,"select ediz from Ukrdok1 where ediz='%s' limit 1",sql_escape_string(kod,0));
if(readkey(strsql) > 0)
 {
  sprintf(strsql,"%s Ukrdok1 !",gettext("Удалить нельзя, потому, что есть записи в таблице"));
  iceb_t_soob(strsql);
  return(1);
 }

sprintf(strsql,"select ei from Specif where ei='%s' limit 1",sql_escape_string(kod,0));
if(readkey(strsql) > 0)
 {
  sprintf(strsql,"%s Specif !",gettext("Удалить нельзя, потому, что есть записи в таблице"));
  iceb_t_soob(strsql);
  return(1);
 }


return(0);
}
/********************************************/
/*Распечатка */
/***********************************/
void iceb_tl_ei_ras(class SQLCURSOR *cur,const char *poisk)
{
SQL_str row;
FILE *ff;
char imaf[64];
class iceb_tu_str danet("");

sprintf(imaf,"edizmer%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
  {
   error_op_nfil(imaf,errno,"");
   return;
  }

iceb_t_zagolov(gettext("Список единиц измерения"),ff);

if(poisk[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Поиск"),poisk);
fprintf(ff,"\
---------------------------------------------\n");
fprintf(ff,gettext("\
Кор.н.|Код |   Наименование единицы измерения\n"));
//     123456789012345678901234567890 123456 12345678 1234567890123 1234567890123 
fprintf(ff,"\
---------------------------------------------\n");

cur->poz_cursor(0);
while(cur->read_cursor(&row) != 0)
 {
  if(proverka(poisk,row[1],4,0) != 0)
   continue;

  
  fprintf(ff,"%-*s|%-*s|%s\n",
  iceb_tu_kolbait(6,row[0]),
  row[0],
  iceb_tu_kolbait(4,row[4]),
  row[4],
  row[1]);
  
 }
fprintf(ff,"\
---------------------------------------------\n");
podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Список единиц измерения"));
iceb_t_ustpeh(oth.spis_imaf.ravno(0),3);
iceb_t_rabfil(&oth,"");
 

}
