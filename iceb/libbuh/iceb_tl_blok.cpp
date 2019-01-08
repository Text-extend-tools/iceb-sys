/*$Id: iceb_tl_blok.c,v 5.7 2013-04-07 10:22:54 sasa Exp $*/
/*09.11.2010	05.09.2010	Белых А.И.	iceb_tl_blok.c
Установка/отмена блокировки подсистем
*/
#include <errno.h>
#include "buhl.h"

void iceb_tl_blok_hap();
int iceb_tl_blok_vik(const char *mes,const char *god);
void iceb_tl_blok_rs(class SQLCURSOR *cur);
int iceb_tl_blok_prov();

int iceb_kod_podsystem=0; /*текущий код подсистемы - устанавливается при запуске подсистемы*/

void iceb_tl_blok()
{
char mes[32];
short X=0,Y=3;  /*Позиция вывода списка*/
int prc=0;
class iceb_t_mstr spis_menu(0);
short mspz=0;
char strsql[512];
int K=0,kolstr=0,kom=0,pozz=0,pozz1=0,i=0;
class iceb_tu_str godv("");
class iceb_tu_str mesv("");
SQL_str row;

class SQLCURSOR cur;
VVOD DANET(1);
VVOD VVOD1(2);
VVOD VV(0);


kom=pozz=0;

naz1:;
clear();

GDITE();
short           kls=LINES-6; /*Количество строк в меню*/

K=0;

sprintf(strsql,"select * from Blok where kod=%d order by god desc",iceb_kod_podsystem);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,"",strsql);
  return;
 }  

if(kolstr == 0)
 {
lll:;

  iceb_tl_blok_hap();

  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == FK2 || K == PLU || K == FK1 || K == FK4 || K == FK10 \
  || K == ESC)
    goto vper;

  if(K == KEY_RESIZE)
    goto naz1;
    
  goto lll;
 }
 
naz:;
GDITE();
clear();
if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;


cur.poz_cursor(pozz);
mspz=2;
pozz1=0;

spis_menu.spis_str.free_class();
for(i=0;i < kls;)
 {
  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;
  if(atoi(row[2]) == 0)
   strcpy(mes,"**");
  else
   sprintf(mes,"%02d",atoi(row[2]));

  if(iceb_kod_podsystem == ICEB_PS_GK)   
    sprintf(strsql,"%2s.%d|%-*.*s|%s",
    mes,atoi(row[1]),
    iceb_tu_kolbait(20,row[3]),
    iceb_tu_kolbait(20,row[3]),
    row[3],
    row[4]);
  else  
    sprintf(strsql,"%2s.%d|%-*s",
    mes,atoi(row[1]),
    iceb_tu_kolbait(20,row[3]),
    row[3]);

 i++;
  spis_menu.spis_str.plus(strsql);

  if(pozz1 == 1) /*Первая запись удовлетворяет условиям поиска*/
     mspz=1;
 }

if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i == 0)
 {
  if(kolstr > 0 && pozz > 0)
   {
    pozz--;
    goto naz;
   }
  goto lll;
 }
if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));


iceb_tl_blok_hap();

prc=(pozz1+pozz)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,Y,X,NULL,strsql,prc,&K,"",0);

if(kom == -7) //Изменение размеров экрана
  goto naz1;
  
if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  for(i=0; i < mspz  ; )
   {
    pozz++;

    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;

//    if(dirshet_prov(row,&poir) != 0)
//     continue;
    i++;
   }
  kom=kls;
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

//    if(dirshet_prov(row,&poir) != 0)
//     continue;
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
  printw("\n вперед pozz=%d\n",pozz);
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
    cur.read_cursor(&row);

//    if(dirshet_prov(row,&poir) != 0)
//     continue;
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
 
  polen(spis_menu.spis_str.ravno(kom),strsql,sizeof(strsql),1,'|');
  polen(strsql,&mesv,1,'.');
  polen(strsql,&godv,2,'.');    
 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return;

  case FK1:   /*Получить подсказку*/
   
   GDITE();
   iceb_t_pdoc("blok.txt");
   clear();
   goto naz;

  case ENTER : /*Выход*/
  case SFK2:   /*Корректировать*/

    iceb_tl_blok_vik(mesv.ravno(),godv.ravno());
    goto naz1;

  case FK2:  /*Ввести */
  case PLU:
    if(iceb_tl_blok_prov() != 0)
     goto naz;

    iceb_tl_blok_vik("","");
  
    goto naz1;

  case FK3: /*Удалить*/
    if(iceb_tl_blok_prov() != 0)
     goto naz;

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
     goto naz;

    sprintf(strsql,"delete from Blok where kod=%d and god=%d and mes=%d",
    iceb_kod_podsystem,godv.ravno_atoi(),mesv.ravno_atoi());

    sql_zapis(strsql,0,0);

    goto naz1;


  case FK5: /*Распечатка*/
    iceb_tl_blok_rs(&cur);
    goto naz;

      
  default:
    if(kolstr > 0)
     goto naz;
    else
     goto lll;
 }
}


/*****************************/
/*Шапка*/
/***************************/
void iceb_tl_blok_hap()
{
clear();
short		y=2;
short           x=COLS-20;
mvprintw(0,0,gettext("Установка/снятие блокировки подсистемы"));

mvprintw(1,0,iceb_t_getnps());

if(iceb_kod_podsystem == ICEB_PS_GK)
  mvprintw(2,0,"     1            2             3");
else
  mvprintw(2,0,"     1            2");

mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,"1-%s",gettext("дата"));
mvprintw(y++,x,"2-%s",gettext("операторы"));
if(iceb_kod_podsystem == ICEB_PS_GK)
  mvprintw(y++,x,"3-%s",gettext("счета"));

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
//"F4",gettext("поиск"),
"F5",gettext("печать"),
//"F6",gettext("код суббаланса"),
"F10",gettext("выход."),NULL);
}

/*************************************/
/*ввод и корректировка записи*/
/**************************************/
int iceb_tl_blok_vik(const char *mes,const char *god)
{
int metka_bg=0; /*метка блокировки всего года*/
short mesi=0,godi=0;
int kom=0;
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
class iceb_tu_str databl(mes);
databl.plus(".",god);

VVOD VV(0);
VV.VVOD_SPISOK_add_data(8);
VV.VVOD_SPISOK_add_data(255);
VV.VVOD_SPISOK_add_data(255);
if(god[0] != '\0')
 {
  sprintf(strsql,"select * from Blok where kod=%d and god=%d and mes=%d",
  iceb_kod_podsystem,atoi(god),atoi(mes));
  if(readkey(strsql,&row,&cur) != 1)
   {
    iceb_t_soob(gettext("Не найдена запись для корректировки!"));
    return(1);
   }

  VV.VVOD_SPISOK_add_ZAG("Корректировка записи");
  kzvz(row[5],row[6],&VV); 
  VV.data_plus(0,databl.ravno());
  VV.data_plus(1,row[3]);
  VV.data_plus(2,row[4]);
 }



VV.VVOD_SPISOK_add_MD(gettext("Дата блокировки.............................(м.г)."));//0
VV.VVOD_SPISOK_add_MD(gettext("Логины операторов для которых разблокировано.(,,)."));//1
if(iceb_kod_podsystem == ICEB_PS_GK)
  VV.VVOD_SPISOK_add_MD(gettext("Заблокированные счета............................."));//2


naz:;
//clear();

helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

//databl.new_plus(VV.data_ravno(0));

switch (kom)
 {
  case ESC:
  case FK10:
     return(1);
     break;

  case FK1:
    GDITE();
//    prosf(iceb_t_imafdoc("zarp4_1_1.txt"));
    goto naz;

  case PLU:
  case FK2:
     if(iceb_tl_blok_prov() != 0)
       goto naz;
     if(VV.data_ravno(0)[0] != '*')
      {
       if(rsdat1(&mesi,&godi,VV.data_ravno(0)) != 0)
        {
         iceb_t_soob(gettext("Неправильно введена дата!"));
         goto naz;
        }     
      }       
    else
      rsdat1(&mesi,&godi,VV.data_ravno(0));
     
     if(godi == 0)
      {
       iceb_t_soob(gettext("Неправильно введена дата!"));
       goto naz;
      }     


     /*проверяем может такая запись уже есть*/
     if(god[0] == '\0' || (mesi != atoi(mes) || godi != atoi(god)))
      {
       sprintf(strsql,"select kod from Blok where kod=%d and god=%d and mes=%d",
       iceb_kod_podsystem,mesi,godi);
       if(readkey(strsql) == 1)
        {
         iceb_t_soob(gettext("Такая запись уже есть!"));
         goto naz;
        }
      }      

     metka_bg=0;
     if(VV.data_ravno(0)[0] == '*')
      {
       VVOD DANET(1);
       DANET.VVOD_SPISOK_add_MD(gettext("Заблокирвать весь год? Вы уверены ?"));

       if(danet(&DANET,2,stdscr) == 2)
        goto naz;               
       metka_bg=1;
      }

     if(god[0] != '\0') /*корректировка записи*/
      {
       

       sprintf(strsql,"update Blok set god=%d,mes=%d,log='%s',shet='%s',ktoz=%d,vrem=%ld\
 where kod=%d and god=%d and mes=%d",
       godi,mesi,VV.data_ravno(1),VV.data_ravno(2),iceb_t_getuid(),time(NULL),
       iceb_kod_podsystem,atoi(god),atoi(mes));
              
      
      }
     else  /*новая запись*/
      {
       sprintf(strsql,"insert into Blok values(%d,%d,%d,'%s','%s',%d,%ld)",
       iceb_kod_podsystem,godi,mesi,VV.data_ravno(1),VV.data_ravno(2),iceb_t_getuid(),time(NULL));
      }

     if(sql_zapis(strsql,1,0) != 0)
      return(1);

     if(metka_bg == 1)
      {
       sprintf(strsql,"delete from Blok where kod=%d and god=%d and mes != 0",iceb_kod_podsystem,godi);
       sql_zapis(strsql,1,0);
      }
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
/*****************************/
/*Распечатка */
/*******************************/
void iceb_tl_blok_rs(class SQLCURSOR *cur)
{
SQL_str row;

FILE *ff;
char imaf[56];
sprintf(imaf,"blok%d",getpid());
if((ff = fopen(imaf,"w")) == NULL)
  {
   error_op_nfil(imaf,errno,"");
   return;
  }

iceb_t_zagolov(gettext("Заблокированные даты"),0,0,0,0,0,0,ff);

fprintf(ff,"%s:%s\n",
gettext("Подсистема"),
iceb_t_getnps());

fprintf(ff,"\
---------------------------------------------------------\n");
if(iceb_kod_podsystem == ICEB_PS_GK)
  fprintf(ff,"\
 Дата  |     Оператор                 |  Счёт\n");
else
  fprintf(ff,"\
 Дата  |     Оператор\n");

fprintf(ff,"\
---------------------------------------------------------\n");

cur->poz_cursor(0);
while(cur->read_cursor(&row) != 0)
 {
  if(iceb_kod_podsystem == ICEB_PS_GK)
    fprintf(ff,"%2s.%s|%-*.*s|%s\n",
    row[2],row[1],
    iceb_tu_kolbait(30,row[3]),
    iceb_tu_kolbait(30,row[3]),
    row[3],
    row[4]);
  else
    fprintf(ff,"%2s.%s|%s\n",
    row[2],row[1],row[3]);
  
 }
fprintf(ff,"\
---------------------------------------------------------\n");
podpis(ff);
fclose(ff);

class spis_oth oth;

oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Заблокированные даты"));
iceb_t_ustpeh(imaf,3);
iceb_t_rabfil(&oth,"");

}
/*******************************************************************/
/*проверка разрешения оператору вводить и корректировать информацию*/
/********************************************************************/
int iceb_tl_blok_prov()
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
char name_kol[32];

switch(iceb_kod_podsystem)
 {
  case ICEB_PS_GK:
   strcpy(name_kol,"gkb");
   break;

  case ICEB_PS_MU:
   strcpy(name_kol,"mub");
   break;

  case ICEB_PS_PD:
   strcpy(name_kol,"pdb");
   break;

  case ICEB_PS_ZP:
   strcpy(name_kol,"zpb");
   break;

  case ICEB_PS_UOS:
   strcpy(name_kol,"osb");
   break;

  case ICEB_PS_UU:
   strcpy(name_kol,"uub");
   break;

  case ICEB_PS_UKO:
   strcpy(name_kol,"kob");
   break;

  case ICEB_PS_UKR:
   strcpy(name_kol,"krb");
   break;

  case ICEB_PS_UPL:
   strcpy(name_kol,"plb");
   break;

  case ICEB_PS_RNN:
   strcpy(name_kol,"nnb");
   break;

  case ICEB_PS_UD:
   strcpy(name_kol,"udb");
   break;

 };
 
sprintf(strsql,"select %s from icebuser where login='%s'",name_kol,iceb_tu_getlogin());

if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s\n%s %s",
  gettext("У Вас нет полномочий для выполнения этой операции!"),
  gettext("Ненайден логин"),
  iceb_tu_getlogin());

  iceb_t_soob(strsql);

  return(1);
  
 }
if(atoi(row[0]) == 1)
 {
  iceb_t_soob(gettext("У Вас нет полномочий для выполнения этой операции!"));
  return(1);
  
 }


return(0);

}
