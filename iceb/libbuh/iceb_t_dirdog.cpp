/*$Id: iceb_t_dirdog.c,v 5.5 2013/12/31 11:48:19 sasa Exp $*/
/*29.04.2018	16.02.2011	Белых А.И.	iceb_t_dirdog.c
Просмотр списка договоров для конкретного контрагента
Если выбрали запись то возвращается её уникальный номер в списке
Иначе возвращается -1
*/
#include <errno.h>

#include "buhl.h"


void iceb_t_dirdog_ras(SQLCURSOR *cur,const char *kodkon,const char *naim_kontr);
void iceb_t_dirdog_sap(const char *kodkon,const char *naim_kontr);
int iceb_t_dirdog_vk(int un_nom_zap,const char *kodkon);


int iceb_t_dirdog(int metka_r, /*0-работа со списком 1-выбор*/
const char *kodkon)
{
int nom_str=0;
class iceb_t_mstr spis_menu(0);
int K=0,pozz=0,mspz=0,pozz1=0,kom=0,prc=0;
int Y=3;
int X=0;
char strsql[512];
int kolstr=0;
class SQLCURSOR cur;
SQL_str row;
class iceb_tu_str naim_kontr("");
class VVOD DANET(1);
short d,m,g;
class iceb_tu_str metka("");
if(kodkon[0] == '\0')
 {
  iceb_t_soob(gettext("Не введён код контрагента!"));
  return(-1);
 }
sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kodkon);
if(readkey(strsql,&row,&cur) == 1)
 naim_kontr.new_plus(row[0]);


naz1:;

short           kls=LINES-6;
int un_nom_zap[kls]; /*уникальный номер записей*/
int metka_pu[kls]; /*метка по умолчанию*/

sprintf(strsql,"select * from Kontragent2 where kodkon='%s' order by datd desc",kodkon);



if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
lll:;
  iceb_t_dirdog_sap(kodkon,naim_kontr.ravno());
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
for(nom_str=0;nom_str<=kls-1;)
 {
  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;

  rsdat(&d,&m,&g,row[2],2);

  un_nom_zap[nom_str]=atoi(row[0]);  
  metka_pu[nom_str]=atoi(row[8]);
  if(metka_pu[nom_str] == 1)
   metka.new_plus("*");
  else
   metka.new_plus(" ");
   
  sprintf(strsql,"%02d.%02d.%04d|%-*s|%-*s|%s|%s",
  d,m,g,
  iceb_tu_kolbait(10,row[3]),
  row[3],
  iceb_tu_kolbait(5,row[7]),
  row[7],
  metka.ravno(),
  row[4]);

  nom_str++;
  spis_menu.spis_str.plus(strsql);
  
  if(pozz1 == 1) /*Первая запись удовлетворяет условиям поиска*/
   mspz=1;

 }

if(nom_str == 0)
 {
  if(kolstr != 0 && pozz > 0)
   {
    pozz--;
    goto naz;
   }
  goto lll;
 }  
if(kom >= nom_str)
  kom=nom_str-1;

if(kom < 0)
 kom=0;

if(nom_str <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

iceb_t_dirdog_sap(kodkon,naim_kontr.ravno());

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));

kom=spis_menu.menu(kom,Y,X,NULL,strsql,prc,&K,"",COLS-21);


if(kom == -7) //Изменение границы экрана
  goto naz1;

if(kom == -3) /*Стрелка вниз*/
 {
  if(nom_str <kls)
   {
    beep();
    goto naz;
   }
  for(nom_str=0; nom_str< mspz   ; )
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
/*******
    if(naimpoi.getdlinna() > 1 && strstrm(row[0],naimpoi.ravno()) == 0)
     continue;
*******************/
    nom_str++;
   }
  kom=kls-1;
  goto naz;
 }
if(kom == -2) /*Стрелка вверх*/
 {
  for(nom_str=0; nom_str < 1 ; )
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
/************
    if(naimpoi.getdlinna() > 1 && strstrm(row[0],naimpoi.ravno()) == 0)
     continue;
***********/
    nom_str++;
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
  if(nom_str == kls)
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
  for(nom_str=0; nom_str<kls ; )
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
/**********
    if(naimpoi.getdlinna() > 1 && strstrm(row[0],naimpoi.ravno()) == 0)
     continue;
***************/
    nom_str++;
   }
  goto naz;
 }

if(kom >= 0)
 {
  if(SRAV(gettext("Конец списка"),spis_menu.spis_str.ravno(kom),0) == 0)
     goto naz;

  if(pozz+kom >= kolstr)
   goto naz; /*Стали на конец списка*/

//  polen(spis_menu.spis_str.ravno(kom),&imafil,1,'|');

 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return(-1);
     break;

  case FK1:  /*Получить подсказку*/
    GDITE();
    iceb_t_pdoc("buhg4_2_3.txt");
    goto naz;


     
  case FK2:  /*Ввести запись*/
  case PLU:
    iceb_t_dirdog_vk(-1,kodkon);
    goto naz1;

  case ENTER:

    if(metka_r == 1)
     return(un_nom_zap[kom]);
  case SFK2:  /*Ввести запись*/
    iceb_t_dirdog_vk(un_nom_zap[kom],kodkon);
    goto naz1;

  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      memset(strsql,'\0',sizeof(strsql));
      sprintf(strsql,"delete from Kontragent2 where nz=%d",un_nom_zap[kom]);
      if(sql_zapis(strsql,0,0) == 0)
        goto naz1;
     }
    goto naz;


  case FK5:    //Распечатать
    iceb_t_dirdog_ras(&cur,kodkon,naim_kontr.ravno());
    goto naz;

  case FK6:    /*договор по умолчанию*/

    sprintf(strsql,"update Kontragent2 set \
pu=0 \
where kodkon='%s'",
    kodkon);

    sql_zapis(strsql,0,0);

    if(metka_pu[kom] == 0)
     {
      sprintf(strsql,"update Kontragent2 set \
pu=1,\
ktoz=%d,\
vrem=%ld \
where nz=%d",
      iceb_t_getuid(),
      time(NULL),
      un_nom_zap[kom]);

      sql_zapis(strsql,0,0);

     }

    goto naz1;
    
  default:
    goto naz;
    break;
 }
}
/**************************/
/*Шапка                   */
/**************************/

void iceb_t_dirdog_sap(const char *kodkon,const char *naim_kontr)
{
short		y=2;
short           x=COLS-20;



clear();
printw("%s %s %s",gettext("Список договоров для контрагента"),kodkon,naim_kontr);



mvprintw(2,0,"      1          2         3    4");
mvprintw(y++,x,"%s:",gettext("Наименование полей"));
mvprintw(y++,x,"1-%s",gettext("дата договора"));
mvprintw(y++,x,"2-%s",gettext("номер договора"));
mvprintw(y++,x,"3-%s",gettext("вид нал.нак."));
mvprintw(y++,x,"4-%s",gettext("вид договора"));
y++;
mvprintw(y++,x,"*-%s",gettext("по умолчанию"));

helstr(LINES-1,0,
"F1",gettext("помощь"),
"F2/+",gettext("ввод"),
"F3",gettext("удалить"),
"F5",gettext("печать"),
"F6",gettext("по умолчанию"),
"F10",gettext("выход"),NULL);

}
/********************/
/*Распечатать       */
/********************/

void iceb_t_dirdog_ras(SQLCURSOR *cur,const char *kodkon,const char *naim_kontr)
{
char	imaf[64];
FILE	*ff;
SQL_str row;
short d,m,g;
sprintf(imaf,"kondog%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }


iceb_t_zagolov(gettext("Список договоров"),0,0,0,0,0,0,ff);
fprintf(ff,"%s:%s %s\n",gettext("Контрагент"),kodkon,naim_kontr);


fprintf(ff,"\
--------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
   Дата   |Номер договора|     Вид договора   |Вид нал.нак.|     Дата записи   |Оператор\n");
/*      
1234567890 12345678901234 12345678901234567890 123456789012
*/

fprintf(ff,"\
--------------------------------------------------------------------------------------------\n");

cur->poz_cursor(0);

while(cur->read_cursor(&row) != 0)
 {
  rsdat(&d,&m,&g,row[2],2);  
  fprintf(ff,"%02d.%02d.%04d|%-*s|%-*s|%-*s|%s|%s\n",
  d,m,g,
  iceb_tu_kolbait(14,row[3]),
  row[3],
  iceb_tu_kolbait(20,row[4]),
  row[4],
  iceb_tu_kolbait(12,row[7]),
  row[7],
  iceb_tu_vremzap(row[6]),
  row[5]);
  
 }
fprintf(ff,"\
--------------------------------------------------------------------------------------------\n");

podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Список договоров"));
iceb_t_ustpeh(imaf,3);
iceb_t_rabfil(&oth,"");
}
/**********************************************/
/*ввод и корректировка записи*/
/*****************************************/
int iceb_t_dirdog_vk(int un_nom_zap,const char *kodkon)
{
int kom=0;
class iceb_tu_str vid_dog("");
class iceb_tu_str dat_dog("");
class iceb_tu_str nom_dog("");
class iceb_tu_str vid_nn("");
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
VVOD VV(0);

if(un_nom_zap >= 0)
 {

  sprintf(strsql,"select * from Kontragent2 where nz=%d",un_nom_zap);
  if(readkey(strsql,&row,&cur) != 1)
   {
    iceb_t_soob(gettext("Не найдена запись для корректировки!"));
    return(1);
   }
  dat_dog.new_plus(iceb_tu_datzap(row[2]));
  nom_dog.new_plus(row[3]);
  vid_dog.new_plus(row[4]);
  vid_nn.new_plus(row[7]);
  
  kzvz(row[5],row[6],&VV);
 }
else
 VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой записи"));

VV.VVOD_SPISOK_add_MD(gettext("Вид договора............"));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата договора..(д.м.г).."));//1
VV.VVOD_SPISOK_add_MD(gettext("Номер договора.........."));//2
VV.VVOD_SPISOK_add_MD(gettext("Вид налоговой накладной."));//3

VV.VVOD_SPISOK_add_data(vid_dog.ravno(),112);
VV.VVOD_SPISOK_add_data(dat_dog.ravno(),11);
VV.VVOD_SPISOK_add_data(nom_dog.ravno(),64);
VV.VVOD_SPISOK_add_data(vid_nn.ravno(),16);

naz:;

//clear();
 
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("записать"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

vid_dog.new_plus(VV.VVOD_SPISOK_return_data(0));
dat_dog.new_plus(VV.VVOD_SPISOK_return_data(1));
nom_dog.new_plus(VV.VVOD_SPISOK_return_data(2));
vid_nn.new_plus(VV.VVOD_SPISOK_return_data(3));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   goto naz;

  case ESC:
  case FK10:
   return(1);

  case FK2:
  case PLU:
     break;



  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

if(VV.prov_dat(1,1) != 0)
 {
  iceb_t_soob(gettext("Не правильно введена дата!"));
  goto naz;
 }
if(VV.data_ravno(1)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введена дата!"));
  goto naz;
 }

if(un_nom_zap >= 0) /*Корректировка*/
 {
  sprintf(strsql,"update Kontragent2 set \
datd='%s',\
nomd='%s',\
vidd='%s',\
vnn='%s',\
ktoz=%d,\
vrem=%ld \
where nz=%d",
  VV.ravno_sqldat(1),
  VV.data_ravno(2),
  VV.data_ravno_filtr(0),
  VV.data_ravno_filtr(3),
  iceb_t_getuid(),
  time(NULL),
  un_nom_zap);
  
   
 }
else
 {
  sprintf(strsql,"insert into Kontragent2 (kodkon,datd,nomd,vidd,ktoz,vrem,vnn) values ('%s','%s','%s','%s',%d,%ld,'%s')",
  kodkon,
  VV.ravno_sqldat(1),
  VV.data_ravno(2),
  VV.data_ravno_filtr(0),
  iceb_t_getuid(),
  time(NULL),
  VV.data_ravno_filtr(3));
  
 }

if(sql_zapis(strsql,1,0) != 0)
 goto naz;

return(0);

}
