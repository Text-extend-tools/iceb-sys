/*$Id: dirzargr.c,v 5.4 2013/05/17 14:56:00 sasa Exp $*/
/*28.03.2017	02.11.2011	Белых А.И.	dirzargr.c
Работа со справочником пенсионеров ивналидов и работающих по договорам подряда
*/
#include <errno.h>
#include "buhg.h"

void dirzargr_sap(int metka_s,const char *naim_spr);
void dirzargr_rs(class SQLCURSOR *cur,const char *naim_spr);
int dirzargr_vvod(int metka_s,int metka_r,int tabnom,const char *dnd);


void dirzargr(int metka_s) /*1-пенсионер 2-инвалид 3-договор подряда*/
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
int tabnom=0;
class iceb_tu_str dnd("");
class iceb_tu_str dkd("");
class iceb_tu_str naim_spr(gettext("Неизвестный справочник"));
//Создаем класс меню
class VVOD DANET(1);
class VVOD VVOD1(2);

if(metka_s == 1)
  naim_spr.new_plus(gettext("Справочник пенсионеров"));
if(metka_s == 2)
  naim_spr.new_plus(gettext("Справочник инвалидов"));
if(metka_s == 3)
  naim_spr.new_plus(gettext("Справочник работающих по договорам подряда"));



naz1:;


short           kls=LINES-6;


sprintf(strsql,"select * from Zargr where kg=%d order by tn asc,dnd asc",metka_s);

  

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
lll:;

   
  dirzargr_sap(metka_s,naim_spr.ravno());

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
  sprintf(strsql,"select fio from Kartb where tabn=%s",row[1]);
  if(readkey(strsql,&row1,&cur1) == 1)
   fio.new_plus(row1[0]);
  
  dnd.new_plus(iceb_tu_datzap_mg(row[2]));
  dkd.new_plus(iceb_tu_datzap_mg(row[3]));
  
  sprintf(strsql,"%6s|%*.*s|%7s|%7s|%.*s",
  row[1],
  iceb_tu_kolbait(20,fio.ravno()),
  iceb_tu_kolbait(20,fio.ravno()),
  fio.ravno(),
  dnd.ravno(),
  dkd.ravno(),
  iceb_tu_kolbait(20,row[4]),
  row[4]);
  
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


dirzargr_sap(metka_s,naim_spr.ravno());

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
  polen(str_vibr.ravno(),&dnd,3,'|');
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
    dirzargr_vvod(metka_s,1,tabnom,dnd.ravno());
    goto naz1;

  case FK2:  /*Редактировать запись*/
  case PLU:  /*Редактировать запись*/
    dirzargr_vvod(metka_s,0,0,"");
    goto naz1;

  case FK3:  /*удалить*/
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
     goto naz;

    sprintf(strsql,"delete from Zargr where kg=%d and tn=%d and dnd='%s'",metka_s,tabnom,dnd.ravno_sqldata());
    sql_zapis(strsql,0,0);
    goto naz1;
    
  case FK5:  /*Распечатать*/
    dirzargr_rs(&cur,naim_spr.ravno());
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
void dirzargr_sap(int metka_s,const char *naim_spr)
{
short		y=2;
short           x=COLS-20;

clear();

printw("%s\n",naim_spr);

mvprintw(2,0,"     1             2             3      4      5");
mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,"1-%s",gettext("табельный номер"));
mvprintw(y++,x,"2-%s",gettext("фамилия"));
mvprintw(y++,x,"3-%s",gettext("дата начала"));
mvprintw(y++,x,"4-%s",gettext("дата конца"));
mvprintw(y++,x,"5-%s",gettext("коментарий"));


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
int dirzargr_zap(int metka_s,int metka_r,class VVOD *VV,int tabnomz,const char *dnd)
{
char strsql[1024];

if(VV->prov_dat1(1,1) != 0)
 {
  iceb_t_soob(gettext("Не правильно введена дата начала!"));
  return(1);
 }
if(VV->prov_dat1(2,1) != 0)
 {
  iceb_t_soob(gettext("Не правильно введена дата конца!"));
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
if(VV->data_ravno_atoi(0) != tabnomz || sravmydat(dnd,VV->data_ravno(1)) != 0)
 {
  sprintf(strsql,"select tn from Zargr where kg=%d and tn=%d and dnd='%s'",metka_s,VV->data_ravno_atoi(0),VV->ravno_sqldat(1));
  if(readkey(strsql) > 0)
   {
    iceb_t_soob(gettext("Такая запись уже есть!"));
    return(1);
   }
 }

if(metka_r == 0) /*новая запись*/
 {
  
  sprintf(strsql,"insert into Zargr values(%d,%d,'%s','%s','%s',%d,%ld)",
  metka_s,
  VV->data_ravno_atoi(0),
  VV->ravno_sqldat(1),
  VV->ravno_sqldat(2),
  VV->data_ravno_filtr(3),
  iceb_t_getuid(),time(NULL));
  refresh();
  
 }
else /*корректировка записи*/
 {
  class iceb_tu_str  dnd1(iceb_tu_tosqldat(dnd));
  
  sprintf(strsql,"update Zargr set \
tn=%d,\
dnd='%s',\
dkd='%s',\
kom='%s',\
ktoz=%d,\
vrem=%ld \
where kg=%d and tn=%d and dnd='%s'",
  VV->data_ravno_atoi(0),
  VV->ravno_sqldat(1),
  VV->ravno_sqldat(2),
  VV->data_ravno_filtr(3),
  iceb_t_getuid(),
  time(NULL),
  metka_s,tabnomz,dnd1.ravno());      
 }

if(sql_zapis(strsql,0,0) != 0)
 return(1);
 
return(0);
}
/***********************/
/*Корректировка записи*/
/************************/

int dirzargr_vvod(int metka_s,
int metka_r, /*0-новая запись 1-корректировка записис*/
int tabnomz,
const char *dnd)
{
int N=0,K=0;
char strsql[512];
class VVOD VV(0);

if(metka_r == 0)
  VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой записи"));
else
  VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка записи"));

VV.VVOD_SPISOK_add_data(10);//0
VV.VVOD_SPISOK_add_data(8);//1
VV.VVOD_SPISOK_add_data(8);//2
VV.VVOD_SPISOK_add_data(255);//3

if(metka_r == 1)
 {
  SQL_str row;
  class SQLCURSOR cur;
  sprintf(strsql,"select * from Zargr where kg=%d and tn=%d and dnd='%s'",metka_s,tabnomz,iceb_tu_tosqldat(dnd));
  if(readkey(strsql,&row,&cur) != 1)
   {
    printw("\n%s-%s\n",__FUNCTION__,strsql);
    iceb_t_soob(gettext("Не найдена запись для корректировки!"));
    return(1); 
   }

  VV.data_plus(0,row[1]);
  VV.data_plus(1,iceb_tu_datzap_mg(row[2]));
  VV.data_plus(2,iceb_tu_datzap_mg(row[3]));
  VV.data_plus(3,row[4]);
  kzvz(row[5],row[6],&VV); 
 }


VV.VVOD_SPISOK_add_MD(gettext("Табельный номер....."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата начала..(м.г).."));//1
VV.VVOD_SPISOK_add_MD(gettext("Дата конца...(м.г).."));//2
VV.VVOD_SPISOK_add_MD(gettext("Коментарий.........."));//3


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
    if(dirzargr_zap(metka_s,metka_r,&VV,tabnomz,dnd) != 0)
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
void dirzargr_rs(class SQLCURSOR *cur,const char *naim_spr)
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
 
zagolov(naim_spr,0,0,0,0,0,0,iceb_t_get_pnk("00",0),ff);


fprintf(ff,"\
----------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Таб.номер|          Фамилия             |Дата начал|Дата конца|Коментарий\n"));
/**********
123456789 12345678901234567890 1234567890 1234567890 12345
***********/
fprintf(ff,"\
----------------------------------------------------------------------------------------\n");
cur->poz_cursor(0);
class iceb_tu_str fio("");
class iceb_tu_str dnd("");
class iceb_tu_str dkd("");
while(cur->read_cursor(&row) != 0)
 {
  fio.new_plus("");
  sprintf(strsql,"select fio from Kartb where tabn=%s",row[1]);
  if(readkey(strsql,&row1,&cur1) == 1)
   fio.new_plus(row1[0]);

  dnd.new_plus(iceb_tu_datzap_mg(row[2]));
  dkd.new_plus(iceb_tu_datzap_mg(row[3]));
  

  fprintf(ff,"%9s %-*.*s %-10s %-10s %s\n",
  row[0],
  iceb_tu_kolbait(30,fio.ravno()),
  iceb_tu_kolbait(30,fio.ravno()),
  fio.ravno(),
  dnd.ravno(),
  dkd.ravno(),
  row[4]);
 }

fprintf(ff,"\
----------------------------------------------------------------------------------------\n");


podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(naim_spr);
iceb_t_ustpeh(oth.spis_imaf.ravno(0),3);
iceb_t_rabfil(&oth,"");

}
