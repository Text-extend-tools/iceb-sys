/*$Id: dirzarmzpm.c,v 5.3 2013/05/17 14:56:00 sasa Exp $*/
/*14.07.2015	18.10.2011	Белых А.И.	dirzarmzpm.c
Работа со справочником минимальной зарплаты и прожиточного минимума
*/
#include <errno.h>
#include "buhg.h"

void dirzarmzpm_sap();
void dirzarmzpm_rs(class SQLCURSOR *cur);
int dirzarmzpm_vvod(int metka_r,const char *god_mes_den);


void dirzarmzpm()
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
class iceb_tu_str datazap("");
//Создаем класс меню
class VVOD DANET(1);
class VVOD VVOD1(2);



naz1:;


short           kls=LINES-6;


sprintf(strsql,"select * from Zarmzpm order by data desc");

  

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
lll:;

   
  dirzarmzpm_sap();

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

  rsdat(&d,&m,&g,row[0],2);
  
  sprintf(strsql,"%02d.%04d|%10.2f|%10.2f|%10.2f",
  m,g,
  atof(row[1]),
  atof(row[2]),  
  atof(row[3]));
  
  
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


dirzarmzpm_sap();

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,Y,X,NULL,strsql,prc,&K,"",0);


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
  polen(str_vibr.ravno(),&datazap,1,'|');
 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return;
     break;

  case FK1:  /*Получить подсказку*/
    GDITE();
//    prosf(strsql);
    goto naz;

  case SFK2:
  case ENTER:
    refresh();
    dirzarmzpm_vvod(1,datazap.ravno());
//    dirzarmzpmd(user);
    goto naz1;

  case FK2:  /*Редактировать запись*/
  case PLU:  /*Редактировать запись*/
    dirzarmzpm_vvod(0,"");
    goto naz1;

  case FK3:  /*удалить*/
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
     goto naz;

    rsdat1(&m,&g,datazap.ravno());
    sprintf(strsql,"delete from Zarmzpm where data='%04d-%02d-01'",g,m);
    sql_zapis(strsql,0,0);
    goto naz1;
    
  case FK5:  /*Распечатать*/
    dirzarmzpm_rs(&cur);
    goto naz;
     
  default:
    goto naz;
    break;
 }

return;

}
/********************/
/*Шапка*/
/******************/
void dirzarmzpm_sap()
{
short		y=2;
short           x=COLS-20;

clear();
printw("%s",gettext("Справочник минимальной зарплати и прожиточного минимума"));

mvprintw(2,0,"     1         2         3           4");
mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,"1-%s",gettext("дата"));
mvprintw(y++,x,"2-%s",gettext("мин.зарплата"));
mvprintw(y++,x,"3-%s",gettext("прож.минимум"));
mvprintw(y++,x,"4-%s",gettext("мак.сумма"));


helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("ввод"),
"F3",gettext("удалить"),
"F5",gettext("печать"),
"F10",gettext("выход"),NULL);
 

}
/****************************************/
/*запись*/
/*****************************************/
int dirzarmzpm_zap(int metka_r,class VVOD *VV,const char *mes_god)
{
char strsql[512];
short m=0,g=0;
short mz=0,gz=0;

if(rsdat1(&m,&g,VV->data_ravno(0)) != 0)
 {
  iceb_t_soob(gettext("Не правильно введена дата"));
  return(1);
 }

rsdat1(&mz,&gz,mes_god);

/*проверяем может уже такая запись есть*/
if(mz != m || gz != g)
 {
  sprintf(strsql,"select data from Zarmzpm where data='%04d-%02d-01'",g,m);
  if(readkey(strsql) > 0)
   {
    iceb_t_soob(gettext("Такая запись уже есть!"));
    return(1);
   }
 }


if(metka_r == 0) /*новая запись*/
 {
  sprintf(strsql,"insert into Zarmzpm values('%04d-%02d-01',%.10g,%.10g,%.10g,%d,%ld)",
  g,m,VV->data_ravno_atof(1),VV->data_ravno_atof(2),VV->data_ravno_atof(3),
  iceb_t_getuid(),time(NULL));
  
 }
else /*корректировка записи*/
 {
  sprintf(strsql,"update Zarmzpm set \
data='%04d-%02d-01',\
mz=%.10g,\
pm=%.10g,\
ms=%.10g,\
ktoz=%d,\
vrem=%ld \
where data='%04d-%02d-01'",
  g,m,
  VV->data_ravno_atof(1),
  VV->data_ravno_atof(2),
  VV->data_ravno_atof(3),
  iceb_t_getuid(),
  time(NULL),
  gz,mz);      
 }

if(sql_zapis(strsql,0,0) != 0)
 return(1);
 
return(0);
}
/***********************/
/*Корректировка записи*/
/************************/

int dirzarmzpm_vvod(int metka_r, /*0-новая запись 1-корректировка записис*/
const char *mes_god)
{
char strsql[512];
int kom=0;
class VVOD VV(0);

if(metka_r == 0)
  VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой записи"));
else
  VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка записи"));

VV.VVOD_SPISOK_add_data(8);
VV.VVOD_SPISOK_add_data(12);
VV.VVOD_SPISOK_add_data(12);
VV.VVOD_SPISOK_add_data(12);

if(metka_r == 1)
 {
  SQL_str row;
  class SQLCURSOR cur;
  short mz=0,gz=0;
  rsdat1(&mz,&gz,mes_god);
  sprintf(strsql,"select * from Zarmzpm where data='%04d-%02d-01'",gz,mz);
  if(readkey(strsql,&row,&cur) != 1)
   {
    iceb_t_soob(gettext("Не найдена запись для корректировки!"));
    return(1); 
   }
  sprintf(strsql,"%02d.%04d",mz,gz);
  VV.data_plus(0,strsql);
  VV.data_plus(1,row[1]);
  VV.data_plus(2,row[2]);  
  VV.data_plus(3,row[3]);  
  kzvz(row[4],row[5],&VV); 
 }



VV.VVOD_SPISOK_add_MD(gettext("Дата............................................(м.г)......"));//0
VV.VVOD_SPISOK_add_MD(gettext("Минимальная зарплата......................................."));//1
VV.VVOD_SPISOK_add_MD(gettext("Прожиточный минимум........................................"));//2
VV.VVOD_SPISOK_add_MD(gettext("Максимальная сумма на которую начисляются соц. отчисления.."));//3


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
     if(dirzarmzpm_zap(metka_r,&VV,mes_god) != 0)
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
void dirzarmzpm_rs(class SQLCURSOR *cur)
{
short d,m,g;
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
 
zagolov(gettext("Справочник минимальной зарплаты и прожиточного минимума"),0,0,0,0,0,0,iceb_t_get_pnk("00",0),ff);


fprintf(ff,"\
---------------------------------------------\n");
fprintf(ff,gettext("\
  Дата |Мин.зарплата|Прож.минимум|Макс.сумма|\n"));
/**********
1234567 123456789012 123456789012 1234567890
***********/
fprintf(ff,"\
---------------------------------------------\n");
cur->poz_cursor(0);

while(cur->read_cursor(&row) != 0)
 {
  rsdat(&d,&m,&g,row[0],2);
  
  fprintf(ff,"%02d.%04d %12s %12s %10s\n",
  m,g,row[1],row[2],row[3]);
 }
fprintf(ff,"\
---------------------------------------------\n");


podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Справочник минимальной зарплаты и прожиточного минимума"));
iceb_t_ustpeh(oth.spis_imaf.ravno(0),3);
iceb_t_rabfil(&oth,"");

}
