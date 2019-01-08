/*$Id: dirzarsoc.c,v 5.7 2014/01/31 12:11:33 sasa Exp $*/
/*14.07.2015	14.01.2011	Белых А.И.	dirzarsoc.c
Ввод и корректировка фондов социальных отчислений
*/
#include <errno.h>
#include "buhg.h"

void dirzarsoc_sap(int dlinna_nazv);
int dirzarsoc_vz(int kod);
int dirzarsoc_pu(int kod);
void diresnf(int kod);
void diresnr(int kod_fonda);
void dirzarsoc_rs(class SQLCURSOR *cur);


int dirzarsoc()
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
class SQLCURSOR cur;
int kod=0;
int dlinna_nazv=0;
int max_dlinna_nazv=0;

//Создаем класс меню
VVOD DANET(1);
VVOD VVOD1(2);

naz1:;


short           kls=LINES-6;

sprintf(strsql,"select * from Zarsoc order by kod asc");
  

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
lll:;

   
  dirzarsoc_sap(20);

  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);


  if(K == SFK2 || K == ENTER || K == FK3 || K == FK5 || K == FK6)
    goto lll;

  if(K == KEY_RESIZE)
   goto naz1;   

  goto vper;
 }

max_dlinna_nazv=0;

while(cur.read_cursor(&row) != 0)
 {
   
  if(iceb_tu_strlen(row[1]) > max_dlinna_nazv)
   max_dlinna_nazv=iceb_tu_strlen(row[1]);

 }


if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;

naz:;

if(kolstr != 0 && pozz >= kolstr)
  pozz = kolstr-1;

cur.poz_cursor(pozz);
mspz=2;
pozz1=0;

spis_menu.spis_str.free_class();

dlinna_nazv=COLS-55;
if(max_dlinna_nazv < dlinna_nazv)
 dlinna_nazv=max_dlinna_nazv;

for(i=0;i<=kls-1;)
 {
  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;

  sprintf(strsql,"%2s|%-*.*s|%-*s|%-*s|%6.6g|%s",
  row[0],
  iceb_tu_kolbait(dlinna_nazv,row[1]),  
  iceb_tu_kolbait(dlinna_nazv,row[1]),  
  row[1],
  iceb_tu_kolbait(6,row[2]),
  row[2],
  iceb_tu_kolbait(6,row[7]),
  row[7],
  atof(row[3]),
  row[4]);
  
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


dirzarsoc_sap(dlinna_nazv);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,Y,X,NULL,strsql,prc,&K,"",COLS-26);


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

  polen(spis_menu.spis_str.ravno(kom),&kod,1,'|');
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
    iceb_t_pdoc("zarp3_1_14.txt");
    goto naz;

  case FK2:  
    dirzarsoc_vz(0);
    goto naz1;

  case ENTER: 
  case SFK2: 
    
    dirzarsoc_vz(kod);
    goto naz1;

  case FK3:       /*Удалить запись*/
    if(dirzarsoc_pu(kod) != 0)
     goto naz;

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      goto naz;
    sprintf(strsql,"delete from Zarsoc where kod=%d",kod);
    sql_zapis(strsql,1,0);
    goto naz1;

  case FK5:       /*Печать*/
   dirzarsoc_rs(&cur);
    goto naz;

  case FK6:       /*Работа с настройками*/
    diresnf(kod);
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
void dirzarsoc_sap(int dlinna_nazv)
{
short		y=2;
short           x=COLS-20;

clear();
printw(gettext("Перечень социальных фондов"));


// mvprintw(2,0," 1                          2                             3       4      5     6 ");
mvprintw(2,0,"   1  %*s2%*s   3       4      5     6 ",dlinna_nazv/2-1,"",dlinna_nazv/2-1,"");

mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,"1-%s",gettext("код"));
mvprintw(y++,x,"2-%s",gettext("наименование"));
mvprintw(y++,x,"3-%s",gettext("счёт хоз-ный"));
mvprintw(y++,x,"4-%s",gettext("счёт бюд-ный"));
mvprintw(y++,x,"5-%s",gettext("процент"));
mvprintw(y++,x,"6-%s",gettext("коды не вх.в рас."));


helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F5",gettext("печать"),
"F6",gettext("проценты"),
"F10",gettext("выход"),NULL);
 

}
/***********************/
/*Ввод и корректировка записи*/
/*****************************/

int dirzarsoc_vz(int kod)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
int kom=0;
class iceb_tu_str kods("");
class iceb_tu_str naim("");
class iceb_tu_str proc("");
class iceb_tu_str shet("");
class iceb_tu_str shetb("");
class iceb_tu_str knvr("");
VVOD VV(0);

if(kod != 0) /*Корректировка записи*/
 {
  sprintf(strsql,"select * from Zarsoc  where kod=%d",kod);
  if(readkey(strsql,&row,&cur) != 1)
   {
    iceb_t_soob(gettext("Не найдена запись для корректировки!"));
    return(1);
   }
  kods.new_plus(kod);
  naim.new_plus(row[1]);
  shet.new_plus(row[2]);
  shetb.new_plus(row[7]);
  proc.new_plus(row[3]);
  knvr.new_plus(row[4]);
  VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка записи"));
  kzvz(row[5],row[6],&VV);
 }
else
 {
  VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой записи"));
  int nom=0;
  for(nom=1; ;nom++)
   {
    sprintf(strsql,"select kod from Zarsoc where kod=%d",nom);
    if(readkey(strsql) == 0)
     break;  
   }
  kods.new_plus(nom);
 }


VV.VVOD_SPISOK_add_MD(gettext("Код фонда......................"));
VV.VVOD_SPISOK_add_MD(gettext("Наименование..................."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт хозрасчётный.............."));
VV.VVOD_SPISOK_add_MD(gettext("Процент........................"));
VV.VVOD_SPISOK_add_MD(gettext("Коды не входящие в расчёт.(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт бюджетный................."));

VV.VVOD_SPISOK_add_data(kods.ravno(),16);
VV.VVOD_SPISOK_add_data(naim.ravno(),60);
VV.VVOD_SPISOK_add_data(shet.ravno(),24);
VV.VVOD_SPISOK_add_data(proc.ravno(),24);
VV.VVOD_SPISOK_add_data(knvr.ravno(),255);
VV.VVOD_SPISOK_add_data(shetb.ravno(),24);

naz:;

 
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F4",gettext("очистить"),
"F6",gettext("настройка"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

kods.new_plus(VV.VVOD_SPISOK_return_data(0));
naim.new_plus(VV.VVOD_SPISOK_return_data(1));
shet.new_plus(VV.VVOD_SPISOK_return_data(2));
proc.new_plus(VV.VVOD_SPISOK_return_data(3));
knvr.new_plus(VV.VVOD_SPISOK_return_data(4));
shetb.new_plus(VV.VVOD_SPISOK_return_data(5));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   iceb_t_pdoc("buhg5_1.txt");
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

if(shet.getdlinna() <= 1)
 {
  iceb_t_soob(gettext("Не введено счёт!"));
  goto naz;
 }
struct OPSHET ops;
if(prsh1(shet.ravno(),&ops) != 0)
 goto naz;

if(shetb.getdlinna() > 1)
if(prsh1(shetb.ravno(),&ops) != 0)
 goto naz;
 
if(kod != kods.ravno_atoi())
 {
  /*Проверяем может уже есть такая запись*/
  sprintf(strsql,"select kod from Zarsoc where kod=%d",kods.ravno_atoi());

  if(readkey(strsql) == 1)
   {
    iceb_t_soob(gettext("Такая запись уже есть!"));
    goto naz;
   }
 }
int metka_kor_kod=0;
if(kod == 0) /*Ввод новой записи*/
 {
  sprintf(strsql,"insert into Zarsoc values (%d,'%s','%s',%.2f,'%s',%d,%ld,'%s')",
  kods.ravno_atoi(),naim.ravno_filtr(),shet.ravno(),proc.ravno_atof(),knvr.ravno(),iceb_t_getuid(),time(NULL),shetb.ravno());
  
 }
else /*Корректировка существующей*/
 {
  if(kod != kods.ravno_atoi())
   {
    VVOD DANET(1);
    DANET.VVOD_SPISOK_add_MD(gettext("Корректировать код? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      goto naz;
    metka_kor_kod=1;   
   }

  sprintf(strsql,"update Zarsoc set \
kod=%d,\
naik='%s',\
shet='%s', \
shetb='%s', \
proc=%.2f,\
kodn='%s',\
ktoz=%d, \
vrem=%ld \
where kod=%d",
  kods.ravno_atoi(),naim.ravno_filtr(),
  shet.ravno(),
  shetb.ravno(),
  proc.ravno_atof(),knvr.ravno(),iceb_t_getuid(),time(NULL),
  kod);
 }

if(sql_zapis(strsql,1,0) != 0)
 return(1);

if(metka_kor_kod == 1) 
 {
  sprintf(strsql,"update Zarsocz set kodz=%d where kodz=%d",kods.ravno_atoi(),kod);
  sql_zapis(strsql,1,0);
  sprintf(strsql,"update Zaresv set kf=%d where kf=%d",kods.ravno_atoi(),kod);
  sql_zapis(strsql,1,0);
 }

return(0);

}
/*************************************/
/*Проверка возможности удаления*/
int dirzarsoc_pu(int kod)
{
char strsql[512];

sprintf(strsql,"select kodz from Zarsocz where kodz=%d limit 1",kod);
if(readkey(strsql) > 0)
 {
  sprintf(strsql,"%s Zarsocz !",gettext("Удалить нельзя, потому, что есть записи в таблице"));
  iceb_t_soob(strsql);
  return(1);
 }

sprintf(strsql,"select kf from Zaresv where kf=%d limit 1",kod);
if(readkey(strsql) > 0)
 {
  sprintf(strsql,"%s Zaresv !",gettext("Удалить нельзя, потому, что есть записи в таблице"));
  iceb_t_soob(strsql);
  return(1);
 }
return(0);
}
/******************************************/
/*Распечатка*/
/*******************************************/
void dirzarsoc_rs(class SQLCURSOR *cur)
{
SQL_str row;
FILE *ff;
char imaf[64];
class iceb_tu_str danet("");

sprintf(imaf,"zarsoc%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
  {
   error_op_nfil(imaf,errno,"");
   return;
  }

iceb_t_zagolov(gettext("Список фондов социальных начислений"),0,0,0,0,0,0,ff);
fprintf(ff,"\
----------------------------------------------------------------------\n");
fprintf(ff,"\
 Код |    Нименование фонда         |  Счёт    |Процент|Коды не входящие в расчёт\n");
fprintf(ff,"\
----------------------------------------------------------------------\n");

/*
12345 123456789012345678901234567890 1234567890 1234567
*/
cur->poz_cursor(0);
while(cur->read_cursor(&row) != 0)
 {
  
  fprintf(ff,"%5s %-*.*s %-*s %-6s %s\n",
  row[0],
  iceb_tu_kolbait(30,row[1]),
  iceb_tu_kolbait(30,row[1]),
  row[1],
  iceb_tu_kolbait(10,row[2]),
  row[2],
  row[3],
  row[4]);

  if(iceb_tu_strlen(row[1]) > 30)
   fprintf(ff,"%5s %s\n","",iceb_tu_adrsimv(30,row[1]));
 }
fprintf(ff,"\
----------------------------------------------------------------------\n");
podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Список фондов социальных начислений"));
iceb_t_ustpeh(oth.spis_imaf.ravno(0),3);
iceb_t_rabfil(&oth,"");
 


}

