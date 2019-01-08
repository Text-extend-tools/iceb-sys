/*$Id: dirzvan.c,v 5.3 2013/05/17 14:56:00 sasa Exp $*/
/*14.07.2015	21.10.2011	Белых А.И.	dirzvan.c
Работа со справочником воиских званий
*/
#include <errno.h>
#include "buhg.h"

void dirzvan_sap(int metkar);
void dirzvan_rs(class SQLCURSOR *cur);
int dirzvan_vvod(int metka_r,const char *god_mes_den);
void dirzvan_korkod(const char *nkod,const char *skod);
int dirzvan_prov(const char *kod,int metka_s);


int dirzvan(int metkar, //0-работа 1-выбор из списка с корректировкой 2-чистый выбор
class iceb_tu_str *kod_zv,class iceb_tu_str *naim_zv)
{
class iceb_t_mstr spis_menu(0);
char strsql[1024];
char stroka[1024];
int kolstr=0;
class SQLCURSOR cur;
SQL_str row;
int  Y=3;
int X=0;
int K=0;
int pozz=0,pozz1=0;
int mspz=0;
int prc;
int kom=0;
int i=0;
short d,m,g;
class iceb_tu_str kod_zvan("");
//Создаем класс меню
class VVOD DANET(1);
class VVOD VVOD1(2);
class iceb_tu_str koment("");

if(metkar == 2)
 {
  Y=-1;
  X=-1;
  koment.new_plus(gettext("Выберите нужное и нажмите Enter"));
 }


naz1:;


short           kls=LINES-6;


sprintf(strsql,"select * from Zvan order by kod asc");

  

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
lll:;

   
  dirzvan_sap(metkar);

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
  if(kolstr == 1 && metkar == 2) /*если мы выбираем а запись только одна то она сразу отдаётся*/
   {
    kod_zv->new_plus(row[0]);
    return(0);
   }  
  pozz1++;

  rsdat(&d,&m,&g,row[0],2);

  if(metkar == 0)
    sprintf(strsql,"%2s|%-*.*s|%.10g",
    row[0],
    iceb_tu_kolbait(20,row[1]),
    iceb_tu_kolbait(20,row[1]),
    row[1],  
    atof(row[4]));
  else
    sprintf(strsql,"%2s|%s",
    row[0],
    row[1]);

  sprintf(stroka,"%.*s",iceb_tu_kolbait(COLS-25,strsql),strsql);

  spis_menu.spis_str.plus(stroka);
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


dirzvan_sap(metkar);

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
  class iceb_tu_str str_vibr(spis_menu.spis_str.ravno(kom));
  if(SRAV(gettext("Конец списка"),str_vibr.ravno(),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz; /*Стали на конец списка*/
  polen(str_vibr.ravno(),&kod_zvan,1,'|');
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
    iceb_t_pdoc("zarp3_1_5.txt");
    goto naz;

  case ENTER:
    if(metkar != 0)
     {
      kod_zv->new_plus(kod_zvan.ravno());
      return(0);
     }
  case SFK2:
    dirzvan_vvod(1,kod_zvan.ravno());
    goto naz1;

  case FK2:  /*Редактировать запись*/
  case PLU:  /*Редактировать запись*/
    dirzvan_vvod(0,"");
    goto naz1;

  case FK3:  /*удалить*/
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
     goto naz;

    if(dirzvan_prov(kod_zvan.ravno(),1) != 0)
     goto naz;
   
    rsdat1(&m,&g,kod_zvan.ravno());
    sprintf(strsql,"delete from Zvan where kod=%d",kod_zvan.ravno_atoi());
    sql_zapis(strsql,0,0);
    goto naz1;
    
  case FK5:  /*Распечатать*/
    dirzvan_rs(&cur);
    goto naz;
     
  default:
    goto naz;
    break;
 }


}
/********************/
/*Шапка*/
/******************/
void dirzvan_sap(int metkar)
{
short		y=2;
short           x=COLS-20;

if(metkar != 2)
 {
  clear();
  printw("%s",gettext("Справочник званий"));
 }
if(metkar == 1)
 {
  move(1,0);
  printw(gettext("Найдите нужное и нажмите \"Enter\""));
 }

if(metkar < 2)
 {
  mvprintw(2,0,"  1            2              3");
  mvprintw(y++,x,"%s:",gettext("Наименование полей"));
  mvprintw(y++,x,"1-%s",gettext("код звания"));
  mvprintw(y++,x,"2-%s",gettext("наименование звания"));
  mvprintw(y++,x,"3-%s",gettext("сумма доплаты"));

  helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("ввод"),
"F3",gettext("удалить"),
"F5",gettext("печать"),
"F10",gettext("выход"),NULL);
 }
if(metkar == 2)
  helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
  "F10",gettext("выход"),NULL);
 

}
/****************************************/
/*запись*/
/*****************************************/
int dirzvan_zap(int metka_r,class VVOD *VV,const char *kod_zvanz)
{
char strsql[512];
int metka_kk=0;

/*проверяем может уже такая запись есть*/
if(atoi(kod_zvanz) != VV->data_ravno_atoi(0))
 {
  sprintf(strsql,"select kod from Zvan where kod=%d",VV->data_ravno_atoi(0));
  if(readkey(strsql) > 0)
   {
    iceb_t_soob(gettext("Такая запись уже есть!"));
    return(1);
   }
  if(atoi(kod_zvanz) != 0)
   metka_kk=1;
 }


if(metka_r == 0) /*новая запись*/
 {
  sprintf(strsql,"insert into Zvan values(%d,'%s',%d,%ld,%.2f)",
  VV->data_ravno_atoi(0),
  VV->data_ravno_filtr(1),
  iceb_t_getuid(),time(NULL),
  VV->data_ravno_atof(2));
  
 }
else /*корректировка записи*/
 {
  if(metka_kk == 1)
   {
    VVOD DANET(1);
    DANET.VVOD_SPISOK_add_MD(gettext("Корректировать код ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      return(1);
    
   }

  sprintf(strsql,"update Zvan set \
kod=%d,\
naik='%s',\
ktoz=%d,\
vrem=%ld,\
sdzz=%.2f \
where kod=%d",
  VV->data_ravno_atoi(0),
  VV->data_ravno_filtr(1),
  iceb_t_getuid(),
  time(NULL),
  VV->data_ravno_atof(2),
  atoi(kod_zvanz));      
 }

if(sql_zapis(strsql,0,0) != 0)
 return(1);
if(metka_kk == 1)
 dirzvan_korkod(VV->data_ravno(0),kod_zvanz);
 
return(0);
}
/***********************/
/*Корректировка записи*/
/************************/

int dirzvan_vvod(int metka_r, /*0-новая запись 1-корректировка записис*/
const char *kod_zvanz)
{
char strsql[512];
int kom=0;
class VVOD VV(0);

if(metka_r == 0)
  VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой записи"));
else
  VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка записи"));

VV.VVOD_SPISOK_add_data(8);
VV.VVOD_SPISOK_add_data(60);
VV.VVOD_SPISOK_add_data(16);

if(metka_r == 1)
 {
  SQL_str row;
  class SQLCURSOR cur;
  sprintf(strsql,"select * from Zvan where kod=%d",atoi(kod_zvanz));
  if(readkey(strsql,&row,&cur) != 1)
   {
    iceb_t_soob(gettext("Не найдена запись для корректировки!"));
    return(1); 
   }
  VV.data_plus(0,row[0]);
  VV.data_plus(1,row[1]);
  VV.data_plus(2,row[4]);  
  kzvz(row[2],row[3],&VV); 
 }
else
 {
  sprintf(strsql,"%d",oprsk("Zvan"));
  VV.VVOD_SPISOK_zapis_data(0,strsql);
 }



VV.VVOD_SPISOK_add_MD(gettext("Код звания..............."));//0
VV.VVOD_SPISOK_add_MD(gettext("Наименование звания......"));//1
VV.VVOD_SPISOK_add_MD(gettext("Сумма доплаты за звание.."));//2


naz:;
//clear();

helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);


switch (kom)
 {
  case ESC:
  case FK10:
     return(1);
     break;

  case FK1:
    GDITE();
//    iceb_t_pdoc("zarp4_1_1.txt"));
    goto naz;

  case PLU:
  case FK2:
     if(dirzvan_zap(metka_r,&VV,kod_zvanz) != 0)
       goto naz;
     break;



  case FK4:
    VV.VVOD_clear_data();
    goto naz;

  default:
    goto naz;
    break;
 }



return(0);
}



/****************************/
/*Распечтака*/
/************************/
void dirzvan_rs(class SQLCURSOR *cur)
{
SQL_str row;
FILE *ff;
char imaf[64];
class SQLCURSOR cur1;
sprintf(imaf,"%s%d.lst",__FUNCTION__,getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }
 
zagolov(gettext("Справочник званий"),0,0,0,0,0,0,iceb_t_get_pnk("00",0),ff);


fprintf(ff,"\
---------------------------------------------------\n");
fprintf(ff,gettext("\
 Код |Наименование звания |Сумма доплаты за звание|\n"));
/**********
12345 12345678901234567890 12345678
***********/
fprintf(ff,"\
---------------------------------------------------\n");
cur->poz_cursor(0);

while(cur->read_cursor(&row) != 0)
 {
  
  fprintf(ff,"%5s %-*.*s %s\n",
  row[0],
  iceb_tu_kolbait(20,row[1]),
  iceb_tu_kolbait(20,row[1]),
  row[1],
  row[4]);  
 }
fprintf(ff,"\
---------------------------------------------------\n");


podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Справочник званий"));
iceb_t_ustpeh(oth.spis_imaf.ravno(0),3);
iceb_t_rabfil(&oth,"");

}
/**********************************/
/*корректировка кода звания*/
/********************************/
void dirzvan_korkod(const char *nkod,const char *skod)
{
char strsql[512];
GDITE();

sprintf(strsql,"update Kartb set zvan=%d where zvan=%d",atoi(nkod),atoi(skod));
sql_zapis(strsql,1,0);

sprintf(strsql,"update Zarn set zvan=%d where zvan=%d",atoi(nkod),atoi(skod));
sql_zapis(strsql,1,0);


}

/********************************/
/*Проверка возможности удаления*/
/********************************/
//Если вернули 0-можно удалять 1-нет
int dirzvan_prov(const char *kod,int metka_s)
{
char	strsql[512];


sprintf(strsql,"select zvan from Kartb where zvan=%s limit 1",kod);

if(readkey(strsql) > 0)
 {
  if(metka_s == 1)
    iceb_t_ndelkod("Kartb",kod);
  return(1);
 }

sprintf(strsql,"select zvan from Zarn where zvan=%s limit 1",kod);

if(readkey(strsql) > 0)
 {
  if(metka_s == 1)
    iceb_t_ndelkod("Zvan",kod);
  return(1);
 }


return(0);
}
