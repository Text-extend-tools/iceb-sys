/*$Id: dirzarrud.c,v 5.5 2013/05/17 14:56:00 sasa Exp $*/
/*14.07.2015	02.11.2011	Белых А.И.	dirzarrud.c
Работа со справочником регулярных удержаний
*/
#include <errno.h>
#include "buhg.h"

void dirzarrud_sap();
void dirzarrud_rs(class SQLCURSOR *cur);
int dirzarrud_vvod(int metka_r,int tabnom);


void dirzarrud()
{
class iceb_t_mstr spis_menu(0);
char strsql[1024];
char stroka[1024];
int kolstr=0;
class SQLCURSOR cur,cur1;
SQL_str row,row1;
int  Y=3;
int X=0;
int K=0;
int pozz=0,pozz1=0;
int mspz=0;
int prc;
int kom=0;
int i=0;
class iceb_tu_str fio("");
char metka_s[8];
int tabnom=0;
//Создаем класс меню
class VVOD DANET(1);
class VVOD VVOD1(2);



naz1:;


short           kls=LINES-6;


sprintf(strsql,"select * from Zarrud order by tn asc");

  

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
lll:;

   
  dirzarrud_sap();

  K=iceb_t_soob(gettext("Не найдено ни одной записи !"),1,1);


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

  fio.new_plus("");
  sprintf(strsql,"select fio from Kartb where tabn=%s",row[0]);
  if(readkey(strsql,&row1,&cur1) == 1)
   fio.new_plus(row1[0]);

  memset(metka_s,'\0',sizeof(metka_s));
  if(atoi(row[3]) == 1)
   strcpy(metka_s,"%");  

  sprintf(strsql,"%6s|%*.*s|%3s|%10s|%1s|%s",
  row[0],
  iceb_tu_kolbait(20,fio.ravno()),
  iceb_tu_kolbait(20,fio.ravno()),
  fio.ravno(),
  row[1],
  row[2],
  metka_s,
  iceb_tu_datzap_mg(row[4]));

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


dirzarrud_sap();

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
  polen(str_vibr.ravno(),&tabnom,1,'|');
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
    dirzarrud_vvod(1,tabnom);
    goto naz1;

  case FK2:  /*Редактировать запись*/
  case PLU:  /*Редактировать запись*/
    dirzarrud_vvod(0,0);
    goto naz1;

  case FK3:  /*удалить*/
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
     goto naz;

    sprintf(strsql,"delete from Zarrud where tn=%d",tabnom);
    sql_zapis(strsql,0,0);
    goto naz1;
    
  case FK5:  /*Распечатать*/
    dirzarrud_rs(&cur);
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
void dirzarrud_sap()
{
short		y=2;
short           x=COLS-20;

clear();
printw("%s",gettext("Справочник регулярных удержаний с работников"));

mvprintw(2,0,"     1             2           3      4      5    6");
mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,"1-%s",gettext("табельный номер"));
mvprintw(y++,x,"2-%s",gettext("фамилия"));
mvprintw(y++,x,"3-%s",gettext("код удержания"));
mvprintw(y++,x,"4-%s",gettext("сумма/процент"));
mvprintw(y++,x,"5-%s",gettext("метка"));
mvprintw(y++,x,"6-%s",gettext("дата действия"));


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
int dirzarrud_zap(int metka_r,class VVOD *VV,int tabnomz)
{
char strsql[512];
short d=0,m=0,g=0;

if(VV->data_ravno(4)[0] != '\0')
 {
  if(rsdat1(&m,&g,VV->data_ravno(4)) != 0)
   {
    iceb_t_soob(gettext("Не правильно введена дата"));
    return(1);
   }
  else
   d=1;
 }
 
/*Проверяем код удержания*/
sprintf(strsql,"select kod from Uder where kod=%d",VV->data_ravno_atoi(1));
if(readkey(strsql) != 1)
 {
  sprintf(strsql,"%s %d!",gettext("Не найден код удержания"),VV->data_ravno_atoi(1));
  iceb_t_soob(strsql);
  return(1);
 }

/*проверяем табельный номер*/
sprintf(strsql,"select tabn from Kartb where tabn=%d",VV->data_ravno_atoi(0));
if(readkey(strsql) != 1)
 {
  sprintf(strsql,"%s %d!",gettext("Не найден табельный номер"),VV->data_ravno_atoi(0));
  iceb_t_soob(strsql);
  return(1);
 }

/*проверяем может уже такая запись есть*/
if(VV->data_ravno_atoi(0) != tabnomz)
 {
  sprintf(strsql,"select tn from Zarrud where tn=%d",VV->data_ravno_atoi(0));
  if(readkey(strsql) > 0)
   {
    iceb_t_soob(gettext("Такая запись уже есть!"));
    return(1);
   }
 }

int metka_pr=0;
if(strstrm(VV->data_ravno(3),gettext("Процент")) == 1)
 metka_pr=1;
if(metka_r == 0) /*новая запись*/
 {
  sprintf(strsql,"insert into Zarrud values(%d,%d,%.2f,%d,'%04d-%02d-%02d','%s',%d,%ld)",
  VV->data_ravno_atoi(0),
  VV->data_ravno_atoi(1),
  VV->data_ravno_atof(2),
  metka_pr,
  g,m,d,  
  VV->data_ravno_filtr(5),
  iceb_t_getuid(),time(NULL));
  
 }
else /*корректировка записи*/
 {
  sprintf(strsql,"update Zarrud set \
tn=%d,\
ku=%d,\
sp=%.2f,\
mu=%d,\
dd='%04d-%02d-%02d',\
kom='%s',\
ktoz=%d,\
vrem=%ld \
where tn=%d",
  VV->data_ravno_atoi(0),
  VV->data_ravno_atoi(1),
  VV->data_ravno_atof(2),
  metka_pr,
  g,m,d,  
  VV->data_ravno_filtr(5),
  iceb_t_getuid(),
  time(NULL),
  tabnomz);      
 }

if(sql_zapis(strsql,0,0) != 0)
 return(1);
 
return(0);
}
/***********************/
/*Корректировка записи*/
/************************/

int dirzarrud_vvod(int metka_r, /*0-новая запись 1-корректировка записис*/
int tabnomz)
{
int N=0,K=0;
char strsql[512];
class VVOD VV(0);

if(metka_r == 0)
  VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой записи"));
else
  VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка записи"));

VV.VVOD_SPISOK_add_data(10);//0
VV.VVOD_SPISOK_add_data(4);//1
VV.VVOD_SPISOK_add_data(20);//2
VV.VVOD_SPISOK_add_data(50);//3
VV.VVOD_SPISOK_add_data(8);//4
VV.VVOD_SPISOK_add_data(255);//5

if(metka_r == 1)
 {
  SQL_str row;
  class SQLCURSOR cur;
  sprintf(strsql,"select * from Zarrud where tn=%d",tabnomz);
  if(readkey(strsql,&row,&cur) != 1)
   {
    iceb_t_soob(gettext("Не найдена запись для корректировки!"));
    return(1); 
   }

  VV.data_plus(0,row[0]);
  VV.data_plus(1,row[1]);
  VV.data_plus(2,row[2]);  

  if(atoi(row[3]) == 1)
   VV.data_plus(3,gettext("Процент"));
  else
   VV.data_plus(3,gettext("Сумма"));
  VV.data_plus(4,iceb_tu_datzap_mg(row[4]));
  VV.data_plus(5,row[5]);      
  kzvz(row[6],row[7],&VV); 
 }
else
  VV.data_plus(3,gettext("Сумма"));


VV.VVOD_SPISOK_add_MD(gettext("Табельный номер................."));//0
VV.VVOD_SPISOK_add_MD(gettext("Код удержания..................."));//1
VV.VVOD_SPISOK_add_MD(gettext("Значение........................"));//2
VV.VVOD_SPISOK_add_MD(gettext("Метка..........................."));//3
VV.VVOD_SPISOK_add_MD(gettext("По какую дату действует..(м.г).."));//4
VV.VVOD_SPISOK_add_MD(gettext("Коментарий......................"));//5


naz:;
//clear();

helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("таб.номер"),
"F4",gettext("очистить"),
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

    if(N == 3) /*метка значения*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFM));
      attron(VV.VVOD_return_cs(iceb_CB));
      VVOD MENU(3);
      MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное"));
      MENU.VVOD_SPISOK_add_MD(gettext("Сумма")); //0
      MENU.VVOD_SPISOK_add_MD(gettext("Процент"));//1
      MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2
      int metka_z=0;

      while(menu3w(stdscr,&MENU,&metka_z,-1,-1,0) != 0);

      if(metka_z == 0)
       VV.data_plus(3,gettext("Сумма"));
      else
       VV.data_plus(3,gettext("Процент"));
       
      N++;
      goto naz;
      
     }

    VV.vprintw_clear(N,stdscr);

    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 0)
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

  if(K == FK2 || K == PLU) /*Записываем*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    if(dirzarrud_zap(metka_r,&VV,tabnomz) != 0)
      goto naz;
    break;
   }
  if(K == FK3)
   { 
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));

    class iceb_tu_str fio("");
    int tabnom=0;

    if(dirtab(&tabnom,&fio,0,1) == 0)
     VV.data_plus(0,tabnom);
    clear();
    goto naz;

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

return(0);
}



/****************************/
/*Распечтака*/
/************************/
void dirzarrud_rs(class SQLCURSOR *cur)
{
char strsql[512];
SQL_str row,row1;
FILE *ff;
char imaf[64];
class SQLCURSOR cur1;
sprintf(imaf,"%s%d.lst",__FUNCTION__,getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }
 
zagolov(gettext("Справочник регулярных удержаний с работников"),0,0,0,0,0,0,iceb_t_get_pnk("00",0),ff);


fprintf(ff,"\
----------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Таб.номер|     Фамилия        |Код уд.| Значение |Метка| Дата  |Коментарий\n"));
/**********
123456789 12345678901234567890 1234567 1234567890 12345
***********/
fprintf(ff,"\
----------------------------------------------------------------------------------------\n");
cur->poz_cursor(0);
class iceb_tu_str fio("");
class iceb_tu_str metka("");

while(cur->read_cursor(&row) != 0)
 {
  fio.new_plus("");
  sprintf(strsql,"select fio from Kartb where tabn=%s",row[0]);
  if(readkey(strsql,&row1,&cur1) == 1)
   fio.new_plus(row1[0]);

  metka.new_plus("");
  if(atoi(row[3]) == 1)
   metka.new_plus(gettext("Процент"));

  fprintf(ff,"%9s %*.*s %7s %10s %*.*s %s %s\n",
  row[0],
  iceb_tu_kolbait(20,fio.ravno()),
  iceb_tu_kolbait(20,fio.ravno()),
  fio.ravno(),
  row[1],
  row[2],
  iceb_tu_kolbait(5,metka.ravno()),  
  iceb_tu_kolbait(5,metka.ravno()),  
  metka.ravno(),
  iceb_tu_datzap_mg(row[4]),
  row[5]);  
 }

fprintf(ff,"\
----------------------------------------------------------------------------------------\n");


podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Справочник регулярных удержаний с работников"));
iceb_t_ustpeh(oth.spis_imaf.ravno(0),3);
iceb_t_rabfil(&oth,"");

}
