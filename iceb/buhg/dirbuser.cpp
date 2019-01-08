/*$Id: dirbuser.c,v 5.8 2014/08/31 06:18:17 sasa Exp $*/
/*20.03.2016	01.09.2010	Белых А.И.	dirbuser.c
Работа со списком операторов для конкретной базы 
*/
#include <pwd.h>
#include <errno.h>
#include "buhg.h"

void dirbuser_sap(const char *imabaz);
int dirbuser_koru(int user);
void dirbuserd(int user,const char *imabaz_v);
void dirbuser_rs(const char *imabaz,class SQLCURSOR *cur);

extern class iceb_tu_str kodirovka_iceb;
extern char *imabaz;

void dirbuser(const char *imabazv)
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
int user=0;
class iceb_tu_str naimorg("");
class iceb_tu_str login;
class iceb_tu_str metka_vdb("");
//Создаем класс меню
class VVOD DANET(1);
class VVOD VVOD1(2);


/*устанавливаем базу данных по умолчанию*/
sprintf(strsql,"USE %s",imabazv);
sql_zapis(strsql,1,1);
sprintf(strsql,"SET NAMES %s",kodirovka_iceb.ravno_udsimv("-"));
sql_zap(&bd,strsql); /*только так - старые версии базы не поддерживают эту команду*/


naz1:;


short           kls=LINES-6;


sprintf(strsql,"select * from icebuser");

  

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
lll:;

   
  dirbuser_sap(imabazv);

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

  if(atoi(row[42]) == 0)
   metka_vdb.new_plus("*");
  else
   metka_vdb.new_plus("");

  sprintf(strsql,"%-10s|%-6s|%1s|%-*.*s|%5s|%-*.*s|%s|%s",
  row[0],
  row[2],
  metka_vdb.ravno(),
  iceb_tu_kolbait(20,row[1]),
  iceb_tu_kolbait(20,row[1]),
  row[1],
  row[43],
  iceb_tu_kolbait(10,row[3]),
  iceb_tu_kolbait(10,row[3]),
  row[3],
  iceb_tu_vremzap(row[5]),
  iceb_tu_kszap(row[4]));

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


dirbuser_sap(imabazv);

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
  if(SRAV(gettext("Конец списка"),spis_menu.spis_str.ravno(kom),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz; /*Стали на конец списка*/
  polen(spis_menu.spis_str.ravno(kom),&login,1,'|');
  polen(spis_menu.spis_str.ravno(kom),&user,2,'|');
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

  case ENTER:
    refresh();
    dirbuserd(user,imabazv);
    goto naz1;
  
  case SFK2:  /*Редактировать запись*/
  case FK2:  /*Редактировать запись*/
  case PLU:  /*Редактировать запись*/
      dirbuser_koru(user);
    goto naz1;

  case FK3:  /*удалить*/
    if(prov_uduser(login.ravno(),imabazv,1) == 0)
      iceb_t_soob(gettext("Этот логин нигде не применяется!"));
    goto naz1;
    
  case FK5:  /*Распечатать*/
    if(imabaz != NULL)
     delete [] imabaz;   
    imabaz=new char[strlen(imabazv)+1];
    strcpy(imabaz,imabazv);
    dirbuser_rs(imabazv,&cur);
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
void dirbuser_sap(const char *imabazv)
{
short		y=2;
short           x=COLS-20;

clear();
printw("%s:%s/%s",gettext("Список операторов для базы"),imabazv,iceb_t_get_pnk("00",0));

mvprintw(2,0,"   1           2              3              4        5             6             7");
mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,"1-%s",gettext("логин"));
mvprintw(y++,x,"2-%s",gettext("номер"));
mvprintw(y++,x,"3-%s",gettext("фамилия"));
mvprintw(y++,x,"4-%s",gettext("таб.номер"));
mvprintw(y++,x,"5-%s",gettext("коментарий"));
mvprintw(y++,x,"6-%s",gettext("дата"));
mvprintw(y++,x,"7-%s",gettext("оператор"));
mvprintw(y++,x,"*-%s",gettext("раз-на выг.дампа базы"));


helstr(LINES-1,0,
//"F1",gettext("помощь"),
"Enter",gettext("полномочия"),
"F2/+",gettext("коррек."),
"F3",gettext("проверить"),
"F5",gettext("печать"),
"F10",gettext("выход"),NULL);
 

}

/****************************/
/*Распечтака*/
/************************/
void dirbuser_rs(const char *imabaz,class SQLCURSOR *cur)
{
SQL_str row;
FILE *ff;
char imaf[64];
class SQLCURSOR cur1;
sprintf(imaf,"user%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }
class iceb_tu_str naimkontr("");

if(readkey("select naikon from Kontragent where kodkon='00'",&row,&cur1) == 1)
 naimkontr.new_plus(row[0]);
 
zagolov(gettext("Список операторов"),0,0,0,0,0,0,naimkontr.ravno(),ff);

fprintf(ff,"%s:%s\n",
gettext("База данных"),imabaz);

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
 Логин |Номер|      Фамилия       |Глав.|Матер|Плат.|Зарпл|Учёт |Учёт |Учёт |Учёт |Учёт |Реест|Учёт |Дамп |Коментарий\n\
       |     |                    |книга|учёт |докум| ата |основ|услуг|кассо|коман|путев|налог|довер|базы |\n"));
/*
1234567 12345 12345678901234567890 12345
*/
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------\n");
cur->poz_cursor(0);

while(cur->read_cursor(&row) != 0)
 {
  fprintf(ff,"%-7s %-5s %-*.*s|",
  row[0],
  row[2],
  iceb_tu_kolbait(20,row[1]),
  iceb_tu_kolbait(20,row[1]),
  row[1]);
  for(int ii=6; ii < 6+33; ii+=3)
   {
    if(atoi(row[ii]) == 0)
     fprintf(ff,"Y ");    
    else
     fprintf(ff,"N ");    

    if(atoi(row[ii+1]) == 0)
     fprintf(ff,"Y ");    
    else
     fprintf(ff,"N ");    

    if(atoi(row[ii+2]) == 0)
     fprintf(ff,"Y|");    
    else
     fprintf(ff,"N|");    
   }
  class iceb_tu_str dump(gettext("Разрешено"));
  if(row[42][0] == '1')
   dump.new_plus(gettext("Запрешено"));
   
  fprintf(ff,"%*.*s|%s\n",
  iceb_tu_kolbait(4,dump.ravno()),  
  iceb_tu_kolbait(4,dump.ravno()),  
  dump.ravno(),
  row[3]);

  fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------\n");
 }


fprintf(ff,"\
                                   | | |\n\
                                   | | ->%s\n\
                                   | ->%s\n\
                                   ->%s\n",
gettext("Разрешение на настроку подсистемы"),
gettext("Разрешение на блокировку подсистемы"),
gettext("Разрешение на работу с подсистемой"));

podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Список операторов"));
iceb_t_ustpeh(oth.spis_imaf.ravno(0),3);
iceb_t_rabfil(&oth,"");

}
/*****************************************/
/*корректировка */
/*****************************************/
int dirbuser_koru(int user)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
class iceb_tu_str fio;
class iceb_tu_str koment;
class iceb_tu_str tabnom("");
struct  passwd  *ktor=getpwuid(getuid()); /*Кто работает*/
struct  passwd  *ktoz; /*Кто записал*/
int vdb=0;


sprintf(strsql,"select fio,kom,logz,vrem,vdb,tn from icebuser where un=%d",user);
if(readkey(strsql,&row,&cur) != 1)
 {
  iceb_t_soob(gettext("Не найдена запись для корректировки!"));
  return(1); 
 }

fio.plus(row[0]);
koment.plus(row[1]);
ktoz=getpwnam(row[2]); /*узнаём реквизиты оператора*/
vdb=atof(row[4]);
tabnom.new_plus(row[5]);

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка записи"));
if(ktoz != NULL)
 kzvz(ktoz->pw_uid,atol(row[3]),&VV); 
else
 kzvz(0,atol(row[3]),&VV); 

int N=0;
int K=0;

VV.VVOD_SPISOK_add_data(fio.ravno(),255);
VV.VVOD_SPISOK_add_data(koment.ravno(),255);
VV.VVOD_SPISOK_add_data(tabnom.ravno(),10);

if(vdb == 0)
  VV.VVOD_SPISOK_add_data(gettext("Разрешенo"),128);
else
  VV.VVOD_SPISOK_add_data(gettext("Запрещено"),128); 

VV.VVOD_SPISOK_add_MD(gettext("Фамилия.............."));//0
VV.VVOD_SPISOK_add_MD(gettext("Коментарий..........."));//1
VV.VVOD_SPISOK_add_MD(gettext("Табельный номер......"));//2
VV.VVOD_SPISOK_add_MD(gettext("Выгружать дам базы..."));//3

VV.VVOD_SPISOK_add_data(20);//1

naz:;


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
    goto naz;
   }

  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));
    VV.vprintw_clear(N,stdscr);

    if(N == 3) /*меню */
     {
      class VVOD MENU(3);
      int kom1=0;
      MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное"));
      MENU.VVOD_SPISOK_add_MD(gettext("Разрешено"));//0
      MENU.VVOD_SPISOK_add_MD(gettext("Запрещено"));//1

      while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);
      switch (kom1)
       {
        case -1 :
         goto naz;

        case  0 :
          VV.data_plus(3,gettext("Разрешено"));
          vdb=kom1;
          break;
          
        case  1 :
          VV.data_plus(3,gettext("Запрещено"));
          vdb=kom1;
          break;

       }

      goto naz;

     }

    VV.vgetstr(N,stdscr);

    if(N == 2)
     {
      if(VV.data_ravno_atoi(N) != 0)
       {
        sprintf(strsql,"select tabn from Kartb where tabn=%d",VV.data_ravno_atoi(N));
        if(readkey(strsql) != 1)
         {
          sprintf(strsql,"%s! %d",gettext("Нет такого табельного номера"),VV.data_ravno_atoi(N));
          iceb_t_soob(strsql);
        
         }
       }
     }    
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
    N=-1;
    break;
   }

  if(K == FK2 || K == PLU) /*Записываем*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(VV.data_ravno_atoi(2) != 0)
     {
      sprintf(strsql,"select tabn from Kartb where tabn=%d",VV.data_ravno_atoi(2));
      if(readkey(strsql) != 1)
       {
        sprintf(strsql,"%s! %d",gettext("Нет такого табельного номера"),VV.data_ravno_atoi(2));
        iceb_t_soob(strsql);
        goto naz;
       }
     }
 
     sprintf(strsql,"update icebuser set fio='%s',kom='%s',logz='%s',vrem=%ld,vdb=%d,tn=%d where un=%d",
     VV.data_ravno_filtr(0),VV.data_ravno_filtr(1),ktor->pw_name,time(NULL),vdb,VV.data_ravno_atoi(2),user);
//     printw("\n%s-%s\n",__FUNCTION__,strsql);
 //    OSTANOV();
     if(sql_zapis(strsql,1,0) != 0)
      return(1);
     break;
   }
 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
return(0);
}
