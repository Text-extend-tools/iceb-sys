/*$Id: dirsb.c,v 5.14 2014/08/31 06:18:18 sasa Exp $*/
/*07.07.2016	09.09.2009	Белых А.И.	dirsb.c
Работа со списком банков
*/
#include <errno.h>
#include "buhg.h"

void dirsb_sap(int metkar);
void dirsb_vvod(const char *kodban,int kolzap);
void dirsb_ras(SQLCURSOR *cur);


int dirsb(int metkar, //0-работа 1-выбор из списка с корректировкой 2-чистый выбор
class iceb_tu_str *kod_ban,class iceb_tu_str *naim_ban)
{
class iceb_t_mstr spis_menu(0);
char strsql[1024];
int  Y=3;
int X=0;
class iceb_tu_str koment("");
int kolstr=0;
int K=0;
int pozz=0,pozz1=0;
int mspz=0;
SQL_str row;
int prc;
int kom=0;
int i=0;
class VVOD PAR(2);
short dt,mt,gt;
if(metkar == 2)
 {
  Y=-1;
  X=-1;
  koment.new_plus(gettext("Выберите нужное и нажмите Enter"));
 }

//Создаем класс меню
VVOD DANET(1);
VVOD VVOD1(2);

naz1:;

short           kls=LINES-6;


sprintf(strsql,"select * from Zarsb");

  
class SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
lll:;

   
  dirsb_sap(metkar);

  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(metkar == 2)
   return(1);

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

  
  sprintf(strsql,"%-3s|%-*s",row[0],iceb_tu_kolbait(20,row[1]),row[1]);
  

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


dirsb_sap(metkar);

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
  if(SRAV(gettext("Конец списка"),spis_menu.spis_str.ravno(kom),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz; /*Стали на конец списка*/

  polen(spis_menu.spis_str.ravno(kom),kod_ban,1,'|');
  polen(spis_menu.spis_str.ravno(kom),naim_ban,2,'|');

 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     kod_ban->new_plus("");
     naim_ban->new_plus("");
     return(1);
     break;

  case FK1:  /*Получить подсказку*/
    GDITE();
    iceb_t_pdoc("zarp3_1_13.txt");
    goto naz;

  case ENTER:
    if(metkar != 0)
      return(0);
      
  case SFK2:   /*Корректировать*/
    dirsb_vvod(kod_ban->ravno(),kolstr);
    goto naz1;

  case FK2:  /*Ввести запись*/
  case PLU:
    dirsb_vvod("",kolstr);
    goto naz1;

  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      goto naz;
    poltekdat(&dt,&mt,&gt);
    PAR.VVOD_delete();
    PAR.VVOD_SPISOK_add_MD(gettext("Введите пароль"));
    if(parolv(&PAR,dt,mt,gt,0) != 0)
      goto naz1;

    sprintf(strsql,"delete from Zarsb where kod=%d",kod_ban->ravno_atoi());
    if(sql_zapis(strsql,0,0) == 0)
        goto naz1;

    sprintf(strsql,"update Kartb set kb=0 where kb=%d",kod_ban->ravno_atoi());
    sql_zapis(strsql,0,0);

    sprintf(strsql,"delete from Zarkh where kb=%d",kod_ban->ravno_atoi());
    sql_zapis(strsql,0,0);

    goto naz;

  case SFK3:       //Удалить неиспользуемые банки
  
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить неиспользуемые записи ? Вы уверены ?"));
    if(danet(&DANET,2,stdscr) == 2)
      goto naz;
//    dirsklad_ud_neisp();
    goto naz1;
    


  case FK5:    //Распечатать
    dirsb_ras(&cur);
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
void dirsb_sap(int metkar)
{
short		y=2;
short           x=COLS-20;

if(metkar != 2)
 {
  clear();
  printw(gettext("Ввод и корректировка списка банков"));
 }
if(metkar == 1)
 {
  move(1,0);
  printw(gettext("Найдите нужное и нажмите \"Enter\""));
 }



if(metkar < 2)
 {
  mvprintw(2,0,"   1           2");
  mvprintw(y++,x,gettext("Наименование полей:"));
  mvprintw(y++,x,"1-%s",gettext("код банка"));
  mvprintw(y++,x,"2-%s",gettext("наименование"));

  helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F5",gettext("печать"),
"F10",gettext("выход"),NULL);
 }
 
if(metkar == 2)
  helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
  "F10",gettext("выход"),NULL);

}
/***************************/
/*Корректрировка кода в таблицах*/
/*********************************/
void dirsklad_korkod(const char *nkod,const char *skod)
{
char strsql[512];
GDITE();
sprintf(strsql,"update Kartb set kb=%s where kb=%s",nkod,skod);
if(sql_zapis(strsql,1,0) != 0)
 return;

sprintf(strsql,"update Zarkh set kb=%s where kb=%s",nkod,skod);
sql_zapis(strsql,1,0);


}

/*********************/
/*Запись             */
/*********************/
//Если вернули 0 - записали 1-нет

int   dirsb_zap(VVOD *VV,const char *kodban,int kolzap)
{
char	strsql[512];

attroff(VV->VVOD_return_cs(iceb_CFS));
attron(VV->VVOD_return_cs(iceb_CFM));

if(VV->VVOD_SPISOK_return_data(0)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(1)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
  return(1);
 }       

if(VV->data_ravno_atoi(0) == 0)
 {
  iceb_t_soob(gettext("Код банка должен быть цифровым!"));
  return(1);
 }


if(SRAV(kodban,VV->data_ravno(0),0) != 0)
 {
  //Проверяем может такой банк уже введён
  sprintf(strsql,"select kod from Zarsb where kod=%s",VV->data_ravno(0));
  if(readkey(strsql) >= 1)
   {
    iceb_t_soob(gettext("Такой код банка уже есть !"));
    return(1);
   }

  if(kodban[0] != '\0')
   {
    VVOD DANET(1);
    DANET.VVOD_SPISOK_add_MD(gettext("Корректировать код ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      return(1);
   }

 }
 
time_t vrem;
time(&vrem);

if(kodban[0] == '\0') //Ввод новой записи
  sprintf(strsql,"insert into Zarsb values(%s,'%s',%d,%ld)",
  VV->VVOD_SPISOK_return_data(0),  
  VV->VVOD_SPISOK_return_data(1),  
  iceb_t_getuid(),vrem);
else
  sprintf(strsql,"update Zarsb set \
kod=%s,\
naik='%s',\
ktoz=%d,\
vrem=%ld \
where kod=%s",  
  VV->data_ravno(0),
  VV->data_ravno(1),  
  iceb_t_getuid(),
  vrem,
  kodban);


if(sql_zapis(strsql,0,0) != 0)
 return(1);

if(kolzap == 0)
 {
  sprintf(strsql,"update Kartb set kb=%d where bankshet <> ''",VV->data_ravno_atoi(0));
  sql_zapis(strsql,0,0);

  sprintf(strsql,"select kb from Zarkh limit 1");
  if(readkey(strsql) > 0)
   {
    sprintf(strsql,"update Zarkh set kb=%d",VV->data_ravno_atoi(0));
    sql_zapis(strsql,0,0);
   }
  else
   {
    /*для старых баз в которых таблица Zarkh появилась после того как были введены карт-счета*/
    int kolstr=0;
    class SQLCURSOR cur;
    SQL_str row;

    sprintf(strsql,"select tabn,bankshet from Kartb where bankshet <> ''");
      
    if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
      msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

    if(kolstr > 0)
     while(cur.read_cursor(&row) != 0)
      {
       sprintf(strsql,"insert into Zarkh values(%s,%s,'%s',%d,%ld)",row[0],VV->data_ravno(0),row[1],iceb_t_getuid(),time(NULL));
       sql_zapis(strsql,0,0);
      }  
   }
 }
else 
  if(kodban[0] != '\0' && SRAV(kodban,VV->VVOD_SPISOK_return_data(0),0) != 0)
    dirsklad_korkod(VV->data_ravno(0),kodban);


return(0);


}

/**************************/
/*Ввод и корректировка*/
/**********************/
void dirsb_vvod(const char *kodban,int kolzap)
{
char		strsql[512];
int		N=1,K=0;
SQL_str row;
class SQLCURSOR cur;

VVOD VV(0);

VV.VVOD_SPISOK_add_data(10);//0
VV.VVOD_SPISOK_add_data(80);//1

if(kodban[0] != '\0')
 {

  sprintf(strsql,"select * from Zarsb where kod=%s",kodban);
  if(readkey(strsql,&row,&cur) != 1)
   return;   

  VV.VVOD_SPISOK_zapis_data(0,row[0]);
  VV.VVOD_SPISOK_zapis_data(1,row[1]);
   
  kzvz(row[2],row[3],&VV);
 }
else
 {
  sprintf(strsql,"%d",oprsk("Zarsb"));
  VV.VVOD_SPISOK_zapis_data(0,strsql);
 }

VV.VVOD_SPISOK_add_MD(gettext("Код банка.............."));//0
VV.VVOD_SPISOK_add_MD(gettext("Наименование банка....."));//1

naz:;

 helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
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

    if(N == 1 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')
     {
     }

    if(N == 5 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')
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

  if(K == FK1) //Помощь
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    GDITE();
//    prosf(strsql);
    clear();
    goto naz;
   }

  if(K == FK2 || K == PLU) //Записываем
   {
    if(dirsb_zap(&VV,kodban,kolzap) != 0)
     goto naz;
    break;
   }

 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
clear();
refresh();
return;
}
/**********************************/
/*Распечатка */
/**********************************/
void dirsb_ras(SQLCURSOR *cur)
{
char	imaf[64];
FILE	*ff;
SQL_str row;

sprintf(imaf,"sb%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }


iceb_t_zagolov(gettext("Список банков"),ff);


fprintf(ff,"\
------------------------------------------\n");
fprintf(ff,"\
Код | Наименование банка                 |\n");

fprintf(ff,"\
------------------------------------------\n");

cur->poz_cursor(0);

while(cur->read_cursor(&row) != 0)
 {
  fprintf(ff,"%-4s %s\n",row[0],row[1]);
 }

fprintf(ff,"\
------------------------------------------\n");

podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Список банков"));

iceb_t_ustpeh(imaf,0);

iceb_t_rabfil(&oth,"");

}

