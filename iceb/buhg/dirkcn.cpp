/*$Id: dirkcn.c,v 5.22 2013/05/17 14:55:57 sasa Exp $*/
/*13.07.2015	22.09.2006	Белых А.И.	dirkcn.c
Ввод и корректировка списка целевых назначений расходования денежных 
средств по кассе.
*/

#include        <errno.h>
#include        "buhg.h"

void dirkcn_sap(int metkareg,const char *naimpoi);
int dirkcn_vvod(class iceb_tu_str *kod);
int dirkcn_zap(const char *kod,class VVOD *VV);
void dirkcn_korkod(const char *skod,const char *nkod);
int dirkcn_udzap(const char *kod);
void dirkcn_ras(SQLCURSOR *cur,const char*);
int dirkcn_prov(SQL_str row,const char *naimpoi);


int dirkcn(int metkareg, //0-ввод и корректировка 1-выбор
class iceb_tu_str *kod,
class iceb_tu_str *naim)
{
class iceb_t_mstr spis_menu(0);
char		strsql[1024];
SQL_str		row;
int		kolstr=0;
int		K=0;
long		pozz=0,pozz1=0;
int		i=0,kom=0,kom1=0;
short           XK=0,YK=3;
short 		mspz=0;
int		prc=0;
class iceb_tu_str vstr("");
class iceb_tu_str naimpoi("");
class iceb_tu_str kodmv(""); /*Код вновь введенного расхода*/
class iceb_tu_str kodkr("");
class iceb_tu_str naimkr("");


VVOD VVOD1(2);

naz1:;

GDITE();
clear();
short           kls=LINES-6;


K=0;
sprintf(strsql,"select * from Kascn order by kod asc");
/*
printw("\n%s\n",strsql);
refresh();
*/
class SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {  
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
lll:;

  dirkcn_sap(metkareg,naimpoi.ravno());
  
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == FK3 || K == ENTER || K == SFK3)
     goto lll;

  if(K == KEY_RESIZE)
    goto naz1;
    
  goto vper;
 }
if(naimpoi.ravno()[0] == '\0' && kodmv.ravno()[0] != '\0' && kolstr > kls  && \
pozz < kolstr-kls)
 {
  pozz=poivcur(kodmv.ravno(),0,&cur);
  kodmv.new_plus("");
 }

naz:;
GDITE();
clear();

cur.poz_cursor(pozz);
mspz=2;
pozz1=0;
spis_menu.spis_str.free_class();
for(i=0; i < kls ;)
 {
  
  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;

  if(dirkcn_prov(row,naimpoi.ravno()) != 0)
   continue;

  sprintf(strsql,"%-*s|%s",iceb_tu_kolbait(2,row[0]),row[0],row[1]);
  
  spis_menu.spis_str.plus(strsql);
  i++;
  if(pozz1 == 1) /*Первая запись удовлетворяет условиям поиска*/
     mspz=1;

 }

if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i == 0)
 {
  if(kolstr > 0 && pozz > 0 )
   {
    pozz--;
    goto naz;
   }
  goto lll;
 }
if(i <= kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));


dirkcn_sap(metkareg,naimpoi.ravno());

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,YK,XK,NULL,strsql,prc,&K,"",0);


if(kom == -7) //Изменение размеров экрана
  goto naz1;
if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  GDITE();
  for(i=0; i < mspz  ; )
   {
    pozz++;
    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;

    if(dirkcn_prov(row,naimpoi.ravno()) != 0)
     continue;
    i++;
   }
  kom=kls;
  goto naz;
 }
if(kom == -2) /*Стрелка вверх*/
 {
  GDITE();
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

    if(dirkcn_prov(row,naimpoi.ravno()) != 0)
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
  GDITE();
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

    if(dirkcn_prov(row,naimpoi.ravno()) != 0)
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
   goto naz;

  polen(str_vib.ravno(),&kodkr,1,'|');
  polen(str_vib.ravno(),&naimkr,2,'|');

 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return(1);

  case FK1: //Просмотр описания режима
    GDITE();
    iceb_t_pdoc("kas3_4.txt");
    clear();

    goto naz1;

  case FK2: //Ввод новой записи
    kodmv.new_plus("");
    dirkcn_vvod(&kodmv);
    naimpoi.new_plus("");
    goto naz1;

  case ENTER: /*Выход или корректировка*/
    if(metkareg == 1)
     {
      kod->new_plus(kodkr.ravno());
      naim->new_plus(naimkr.ravno());
      return(0);
     }

  case SFK2: //Корректировка записи
    dirkcn_vvod(&kodkr);
    kodmv.new_plus("");
    goto naz1;

  case FK3: //Удаление записи
    dirkcn_udzap(kodkr.ravno());
    kodmv.new_plus("");
    goto naz1;

  case FK4:       /*Отобрать по образцу*/

    helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
    
    naimpoi.new_plus("");
    
    VVOD1.VVOD_delete();
    VVOD1.VVOD_SPISOK_add_MD(gettext("Введите образец для поиска"));

    if(vvod1(&naimpoi,40,&VVOD1,NULL,stdscr,-1,-1) == FK10)
      naimpoi.new_plus("");

    GDITE();

    clear();
    kom=pozz=0;
    if(kolstr == 0)
      goto lll;
    else
      goto naz;

  case FK5:    //Распечатать
    dirkcn_ras(&cur,naimpoi.ravno());
    goto naz;
    
  case FK6:       /*Стать на номер */

    helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
    
    strsql[0]='\0';
    
    VVOD1.VVOD_delete();
    VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код материалла"));
    vstr.new_plus("");
    if((kom1=vvod1(&vstr,16,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
      goto naz;
    if(kom1 == ESC)
      goto naz;
    if(vstr.getdlinna() <= 1)
      goto naz;
      
    naimpoi.new_plus("");
    pozz=0;
    kom=0;
    pozz=poivcur(vstr.ravno(),0,&cur);
    if(pozz == kolstr)
     {
      sprintf(strsql,"%s %s !",gettext("Не найден код материалла"),strsql);
      iceb_t_soob(strsql);
      pozz=0;
     }
    clear();
    goto naz;
    

  default:
   if(kolstr > 0)
     goto naz;
   else
     goto lll;
 }
}


/********************/
/*шапка*/
/********************/
void dirkcn_sap(int metkareg,const char *naimpoi)
{
int	i1;
int	X;

move(0,0);
printw(gettext("Ввод и корректировка целевых назначений"));
move(2,0);
printw("   1               2");

mvprintw(i1=2,X=COLS-21,gettext("Расшифровка полей:"));

mvprintw(++i1,X,"1-%s",gettext("код"));
mvprintw(++i1,X,"2-%s",gettext("наименование"));

if( naimpoi[0] != '\0')
 {
//  attron(A_BLINK);
  mvprintw(i1+=2,X,gettext("Поиск"));

  if(naimpoi[0] != '\0')
    mvprintw(++i1,X,"%s:%s",gettext("Наим."),naimpoi);

//  attroff(A_BLINK);

 }
 
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F6",gettext("код"),
"F10",gettext("выход"),NULL);

}


/***************************/
/*Ввод и корректировка записи*/
/*****************************/
int dirkcn_vvod(class iceb_tu_str *kod)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
int		N=1;
int             K=0;


VVOD VV(0);
VV.VVOD_SPISOK_add_data(10);
VV.VVOD_SPISOK_add_data(60);


if(kod->ravno()[0] == '\0')
 {
  VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой записи"));
  sprintf(strsql,"%d",oprsk("Kascn"));
  VV.VVOD_SPISOK_zapis_data(0,strsql);
  
 }
else
 {
    
  VV.VVOD_SPISOK_add_ZAG(gettext("Коректировка записи"));
  sprintf(strsql,"select * from Kascn where kod=%s",kod->ravno());
  if(readkey(strsql,&row,&cur) != 1)
   {
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("Не найдена запись для корректировки !"));
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    return(1);  
   }
  VV.VVOD_SPISOK_zapis_data(0,row[0]);
  VV.VVOD_SPISOK_zapis_data(1,row[1]);

  kzvz(atoi(row[2]),atol(row[3]),&VV);
           
 }


VV.VVOD_SPISOK_add_MD(gettext("Код целевого назначения..........."));
VV.VVOD_SPISOK_add_MD(gettext("Наименование целевого назначения.."));

naz:;

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F4",gettext("найти код"),
"F10",gettext("выход"),NULL);


VV.vramka(-1,-1,1);

refresh();
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


//    mygetstr(VV.VVOD_SPISOK_return_data(N),VV.VVOD_SPISOK_return_dls(N),stdscr);
    VV.vgetstr(N,stdscr);

    if(N == 0 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')  //Код объекта
     {
      if(SRAV(kod->ravno(),VV.VVOD_SPISOK_return_data(N),0) != 0)
       {
        sprintf(strsql,"select kod from Kascn where kod=%s",VV.VVOD_SPISOK_return_data(0));
        if(readkey(strsql) > 0)
         {
          attroff(VV.VVOD_return_cs(iceb_CFM));
          VVOD SOOB(1);
          sprintf(strsql,gettext("Код %s уже есть в списке !"),VV.VVOD_SPISOK_return_data(0));
          SOOB.VVOD_SPISOK_add_MD(strsql);
          soobshw(&SOOB,stdscr,-1,-1,0,1);
          goto naz;
         }
       }
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
    return(1);
   }

  if(K == FK2 || K == PLU) /*Записываем*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    short metkor=0;
    if(kod->ravno()[0] != '\0' && SRAV(kod->ravno(),VV.VVOD_SPISOK_return_data(0),0) != '\0')
     {
      VVOD DANET(1);
      DANET.VVOD_SPISOK_add_MD(gettext("Корректировать код ? Вы уверены ?"));
      if(danet(&DANET,2,stdscr) == 2)
        goto naz;
      metkor=1;
     }

   if(dirkcn_zap(kod->ravno(),&VV) != 0)
      goto naz;

    if(metkor == 1)
      dirkcn_korkod(kod->ravno(),VV.data_ravno(0));

    kod->new_plus(VV.VVOD_SPISOK_return_data(0));
    return(0);
   }
 }

}
/**********************/
/*запись */
/*****************/
int dirkcn_zap(const char *kod,class VVOD *VV)
{
char strsql[512];

if(VV->data_ravno_atoi(0) == 0)
 {
    iceb_t_soob(gettext("Не введено код !"));
    return(1);

 }

if(SRAV(kod,VV->data_ravno(0),0) != 0)
 {
  sprintf(strsql,"select kod from Kascn where kod=%s",VV->data_ravno(0));
  if(readkey(strsql) > 0)
   {
    sprintf(strsql,gettext("Код %s уже есть в списке !"),VV->data_ravno(0));
    iceb_t_soob(strsql);
    return(1);
   }
 }

time_t vrem;
time(&vrem);

if(kod[0] == '\0')  //Новая запись
 {
  sprintf(strsql,"insert into Kascn \
values (%d,'%s',%d,%ld)",
  VV->data_ravno_atoi(0),
  VV->data_ravno(1),
  iceb_t_getuid(),vrem);
 }
else
 {
  sprintf(strsql,"update Kascn \
set \
kod=%d,\
naik='%s',\
ktoz=%d,\
vrem=%ld \
where kod=%s",
  VV->data_ravno_atoi(0),
  VV->data_ravno(1),
  iceb_t_getuid(),vrem,
  kod);
 }

return(sql_zapis(strsql,0,0));

}
/*************************/
/*Корректировка кода*/
/********************/

void dirkcn_korkod(const char *skod,const char *nkod)
{


char strsql[512];

VVOD SOOBN(1);
SOOBN.VVOD_SPISOK_add_MD(gettext("Выполняется каскадная корректировка кода в таблицах !"));
soobsh(&SOOBN,-1,-1,1);

GDITE();

sprintf(strsql,"update Kasop1 set kcn=%s where kcn=%s",nkod,skod);
sql_zapis(strsql,0,0);

sprintf(strsql,"update Kasop2 set kcn=%s where kcn=%s",nkod,skod);
sql_zapis(strsql,0,0);

sprintf(strsql,"update Kascnsl set kod=%s where kod=%s",nkod,skod);
sql_zapis(strsql,0,0);

}
/*************************/
/*Удаление записи*/
/************************/
int dirkcn_udzap(const char *kod)
{
VVOD DANET(1);
DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

if(danet(&DANET,2,stdscr) == 2)
  return(1);

char	strsql[512];

VVOD SOOBN(1);
SOOBN.VVOD_SPISOK_add_MD(gettext("Выполняется проверка возможности удаления записи"));
soobsh(&SOOBN,-1,-1,0);

GDITE();

sprintf(strsql,"select kcn from Kasop1 where kcn=%s",kod);
if(readkey(strsql) > 0)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Запись удалить невозможно !"));
  sprintf(strsql,"%s Kasop1",gettext("Информация с этим кодом введена в таблицу"));
  SOOB.VVOD_SPISOK_add_MD(strsql);
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return(1);
 }
sprintf(strsql,"select kcn from Kasop2 where kcn=%s",kod);
if(readkey(strsql) > 0)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Запись удалить невозможно !"));
  sprintf(strsql,"%s Kasop1",gettext("Информация с этим кодом введена в таблицу"));
  SOOB.VVOD_SPISOK_add_MD(strsql);
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return(1);
 }
sprintf(strsql,"select kod from Kascnsl where kod=%s",kod);
if(readkey(strsql) > 0)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Запись удалить невозможно !"));
  sprintf(strsql,"%s Kascnsl",gettext("Информация с этим кодом введена в таблицу"));
  SOOB.VVOD_SPISOK_add_MD(strsql);
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return(1);
 }

sprintf(strsql,"delete from Kascn where kod=%s",kod);
return(sql_zapis(strsql,1,0));

}

/*********************************************/
/*Распечатка списка*/
/*********************************************/
void dirkcn_ras(SQLCURSOR *cur,const char *naimpoi)
{
FILE	*ff;
char    imaf[32];
SQL_str row;

GDITE();

sprintf(imaf,"skcn%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

iceb_t_zagolov(gettext("Список кодов целевого назначения"),0,0,0,0,0,0,ff);

fprintf(ff,"\
--------------------------------------\n");
fprintf(ff,"\
Код|           Наименование          |\n");
fprintf(ff,"\
--------------------------------------\n");

cur->poz_cursor(0);

while(cur->read_cursor(&row) != 0)
 {
  if(dirkcn_prov(row,naimpoi) != 0)
     continue;
  fprintf(ff,"%*s %s\n",iceb_tu_kolbait(3,row[0]),row[0],row[1]);
 }
fprintf(ff,"\
--------------------------------------\n");

podpis(ff);
fclose(ff);
class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Список кодов целевого назначения"));
iceb_t_ustpeh(imaf,0);
iceb_t_rabfil(&oth,"");

}
/******************************/
/*Проверка условий поиска*/
/****************************/

int dirkcn_prov(SQL_str row,const char *naimpoi)
{

if(naimpoi[0] != '\0')
 if(strstrm(row[1],naimpoi) == 0)
  return(1);
return(0);
}

