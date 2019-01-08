/*$Id: dirkcnsl.c,v 5.21 2013/09/26 09:43:30 sasa Exp $*/
/*08.03.2018	22.09.2006	Белых А.И.	dirkcnsl.c
Ввод и корректировка стартового сальдо по кодам целевого назначения
*/

#include        <errno.h>
#include        "buhg.h"
class dirkcnsl_poi
 {
  public:
  
   class iceb_tu_str god;
   class iceb_tu_str kod;
   class iceb_tu_str kas;
   short metka_poi; //0-без поиска 1-с поиском
     
   dirkcnsl_poi()
    {
     clear_rek();
     metka_poi=0;
    }
   void clear_rek()
    {
     god.new_plus("");
     kod.new_plus("");
     kas.new_plus("");
    }
 };
 


void dirkcnsl_sap(class dirkcnsl_poi*,double);
int dirkcnsl_udzap(const char *god,const char *kod,const char *kas);
void dirkcnsl_ras(SQLCURSOR *cur,class dirkcnsl_poi *poi);
int dirkcnsl_prov(SQL_str row,class dirkcnsl_poi *poi);
int dirkcnsl_vvod(class iceb_tu_str *god,class iceb_tu_str *kod,class iceb_tu_str *kas);
int dirkcnsl_zap(const char *god,const char *kod,const char *kas,class VVOD *VV);
void dirkcnsl_vvod_rp(class dirkcnsl_poi *poi);


void dirkcnsl()
{
char		strsql[512];
class iceb_t_mstr spis_menu(0);
SQL_str		row,row1;
int		kolstr=0;
int		K=0;
long		pozz=0,pozz1=0;
int		i=0,kom=0;
short           XK=0,YK=3;
short 		mspz=0;
int		prc=0;
class iceb_tu_str god_v("");
class iceb_tu_str kod_v("");
class iceb_tu_str kas_v("");
class iceb_tu_str god_tv("");
class iceb_tu_str kod_tv("");
class iceb_tu_str kas_tv("");
class iceb_tu_str naim_kodcn("");
class SQLCURSOR cur,cur1;
class dirkcnsl_poi poi;
double itogo=0.;


VVOD VVOD1(2);

naz1:;

GDITE();
clear();
short           kls=LINES-6;


K=0;
sprintf(strsql,"select * from Kascnsl order by god,kod asc");
/*
printw("\n%s\n",strsql);
refresh();
*/

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {  
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
itogo=0.;
if(kolstr == 0)
 {
lll:;

  dirkcnsl_sap(&poi,itogo);
  
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == FK3 || K == ENTER || K == SFK3)
     goto lll;

  if(K == KEY_RESIZE)
    goto naz1;
    
  goto vper;
 }
else
 {
  int nomer=0;
  while(cur.read_cursor(&row) != 0)
   {
    if(dirkcnsl_prov(row,&poi) != 0)
     continue;
    itogo+=atof(row[3]);
    
    if(poi.metka_poi == 0 && kolstr > kls  && pozz < kolstr-kls \
    && SRAV(god_tv.ravno(),row[0],0) == 0  && SRAV(kod_tv.ravno(),row[1],0) == 0  && SRAV(kas_tv.ravno(),row[2],0) == 0) 
     { 
      pozz=nomer;  
      god_tv.new_plus("");
      kod_tv.new_plus("");
      kas_tv.new_plus("");
     }
    nomer++;
   }
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

  if(dirkcnsl_prov(row,&poi) != 0)
   continue;

  naim_kodcn.new_plus("");
  sprintf(strsql,"select naik from Kascn where kod=%s",row[1]);
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_kodcn.new_plus(row1[0]);
   
  sprintf(strsql,"%s|%-*s|%-*.*s|%-*s|%s",
  row[0],
  iceb_tu_kolbait(3,row[1]),row[1],
  iceb_tu_kolbait(20,naim_kodcn.ravno()),iceb_tu_kolbait(20,naim_kodcn.ravno()),naim_kodcn.ravno(),
  iceb_tu_kolbait(3,row[2]),row[2],
  row[3]);
  
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


dirkcnsl_sap(&poi,itogo);

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

    if(dirkcnsl_prov(row,&poi) != 0)
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

    if(dirkcnsl_prov(row,&poi) != 0)
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

    if(dirkcnsl_prov(row,&poi) != 0)
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

  polen(str_vib.ravno(),&god_v,1,'|');
  polen(str_vib.ravno(),&kod_v,2,'|');
  polen(str_vib.ravno(),&kas_v,4,'|');

 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return;

  case FK1: //Просмотр описания режима
    GDITE();
    iceb_t_pdoc("kas3_5.txt");
    clear();

    goto naz1;

  case FK2: //Ввод новой записи
     
    kod_tv.new_plus("");
    god_tv.new_plus("");
    kas_tv.new_plus("");
    
    dirkcnsl_vvod(&god_tv,&kod_tv,&kas_tv);
    
    goto naz1;

  case ENTER: /*корректировка*/
  case SFK2: //Корректировка записи
    dirkcnsl_vvod(&god_v,&kod_v,&kas_v);
    kod_tv.new_plus("");
    goto naz1;

  case FK3: //Удаление записи
    dirkcnsl_udzap(god_v.ravno(),kod_v.ravno(),kas_v.ravno());
    kod_tv.new_plus("");
    goto naz1;

  case FK4:       /*Отобрать по образцу*/
      dirkcnsl_vvod_rp(&poi);
      goto naz1; //Всегда сюда, чтобы пересчитала сумму

  case FK5:    //Распечатать
    dirkcnsl_ras(&cur,&poi);
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
void dirkcnsl_sap(class dirkcnsl_poi *poi,double itogo)
{
int	i1;
int	X;

move(0,0);
printw(gettext("Ввод и корректировка сальдо по кодам целевого назначения"));
move(2,0);
printw("   1    2             3          4    5");

mvprintw(i1=2,X=COLS-21,gettext("Расшифровка полей:"));

mvprintw(++i1,X,"1-%s",gettext("год"));
mvprintw(++i1,X,"2-%s",gettext("код"));
mvprintw(++i1,X,"3-%s",gettext("наименование"));
mvprintw(++i1,X,"4-%s",gettext("касса"));
mvprintw(++i1,X,"5-%s",gettext("сальдо"));
++i1;
mvprintw(++i1,X,"%s:%.2f",gettext("Сумма"),itogo);
++i1;

if(poi->metka_poi == 1)
 {
//  attron(A_BLINK);
  mvprintw(i1+=2,X,gettext("Поиск"));

  if(poi->god.getdlinna() > 1)
    mvprintw(++i1,X,"%s:%s",gettext("год"),poi->god.ravno());
  if(poi->kod.getdlinna() > 1)
    mvprintw(++i1,X,"%s:%s",gettext("код"),poi->kod.ravno());
  if(poi->kas.getdlinna() > 1)
    mvprintw(++i1,X,"%s:%s",gettext("каcса"),poi->kas.ravno());

//  attroff(A_BLINK);

 }
 
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F10",gettext("выход"),NULL);

}


/***************************/
/*Ввод и корректировка записи*/
/*****************************/
int dirkcnsl_vvod(class iceb_tu_str *god,class iceb_tu_str *kod,class iceb_tu_str *kas)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
int		N=0;
int             K=0;


VVOD VV(0);
VV.VVOD_SPISOK_add_data(6);
VV.VVOD_SPISOK_add_data(10);
VV.VVOD_SPISOK_add_data(10);
VV.VVOD_SPISOK_add_data(20);


if(kod->ravno()[0] != '\0')
 {
    
  VV.VVOD_SPISOK_add_ZAG(gettext("Коректировка записи"));
  sprintf(strsql,"select * from Kascnsl where god=%d and kod=%s and ks=%d",god->ravno_atoi(),kod->ravno(),kas->ravno_atoi());
  if(readkey(strsql,&row,&cur) != 1)
   {
    iceb_t_soob(gettext("Не найдена запись для корректировки !"));
    return(1);  
   }
  VV.VVOD_SPISOK_zapis_data(0,row[0]);
  VV.VVOD_SPISOK_zapis_data(1,row[1]);
  VV.VVOD_SPISOK_zapis_data(2,row[2]);
  VV.VVOD_SPISOK_zapis_data(3,row[3]);

  kzvz(atoi(row[4]),atol(row[5]),&VV);
           
 }


VV.VVOD_SPISOK_add_MD(gettext("Год......................"));
VV.VVOD_SPISOK_add_MD(gettext("Код целевого назначения.."));
VV.VVOD_SPISOK_add_MD(gettext("Код кассы................"));
VV.VVOD_SPISOK_add_MD(gettext("Сальдо..................."));

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
        sprintf(strsql,"select kod from Kascnsl where kod=%s",VV.VVOD_SPISOK_return_data(0));
        if(readkey(strsql) > 0)
         {
          attroff(VV.VVOD_return_cs(iceb_CFM));
          sprintf(strsql,gettext("Код %s уже есть в списке !"),VV.VVOD_SPISOK_return_data(0));
          iceb_t_soob(strsql);
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

    if(dirkcnsl_zap(god->ravno(),kod->ravno(),kas->ravno(),&VV) != 0)
      goto naz;


    god->new_plus(VV.data_ravno(0));
    kod->new_plus(VV.data_ravno(1));
    kas->new_plus(VV.data_ravno(2));

    return(0);
   }
 }

}
/**********************/
/*запись */
/*****************/
int dirkcnsl_zap(const char *god,const char *kod,const char *kas,class VVOD *VV)
{
char strsql[512];

if(VV->data_ravno_atoi(1) == 0)
 {
  iceb_t_soob(gettext("Не введено код !"));
  return(1);

 }
//проверяем код целевого назначения
sprintf(strsql,"select kod from Kascn where kod=%d",VV->data_ravno_atoi(1));
if(readkey(strsql) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден код целевого назначения"),VV->data_ravno_atoi(1));
  iceb_t_soob(strsql);
  return(1);
 }
//Проверяем код кассы
sprintf(strsql,"select kod from Kas where kod=%d",VV->data_ravno_atoi(2));
if(readkey(strsql) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найдена касса"),VV->data_ravno_atoi(2));
  iceb_t_soob(strsql);
  return(1);
 }

if(SRAV(kod,VV->data_ravno(1),0) != 0 || SRAV(god,VV->data_ravno(0),0) != 0 || SRAV(kas,VV->data_ravno(2),0) != 0)
 {
  sprintf(strsql,"select kod from Kascnsl where god=%d and kod=%d and ks=%d",
  VV->data_ravno_atoi(0),
  VV->data_ravno_atoi(1),
  VV->data_ravno_atoi(2));
  
  if(readkey(strsql) > 0)
   {
    iceb_t_soob(gettext("Такая запись уже есть !"));
    return(1);
   }
 }

if(iceb_t_pbpds(1,VV->data_ravno_atoi(0)) != 0)
 return(1);

if(atoi(god) != 0)
 if(atoi(god) != VV->data_ravno_atoi(0))
  if(iceb_t_pbpds(1,atoi(god)) != 0)
   return(1);
  
time_t vrem;
time(&vrem);

if(kod[0] == '\0')  //Новая запись
 {
  sprintf(strsql,"insert into Kascnsl \
values (%d,%d,%d,%.2f,%d,%ld)",
  VV->data_ravno_atoi(0),
  VV->data_ravno_atoi(1),
  VV->data_ravno_atoi(2),
  VV->data_ravno_atof(3),
  iceb_t_getuid(),vrem);
 }
else
 {
  sprintf(strsql,"update Kascnsl \
set \
god=%d,\
kod=%d,\
ks=%d,\
saldo=%.2f,\
ktoz=%d,\
vrem=%ld \
where god=%d and kod=%d and ks=%d",
  VV->data_ravno_atoi(0),
  VV->data_ravno_atoi(1),
  VV->data_ravno_atoi(2),
  VV->data_ravno_atof(3),
  iceb_t_getuid(),vrem,
  atoi(god),atoi(kod),atoi(kas));
 }
//printw("\n%s\n",strsql);
//OSTANOV();
return(sql_zapis(strsql,0,0));

}
/*************************/
/*Удаление записи*/
/************************/
int dirkcnsl_udzap(const char *god,const char *kod,const char *kas)
{
if(iceb_t_pbpds(1,atoi(god)) != 0)
 return(1);

VVOD DANET(1);
DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

if(danet(&DANET,2,stdscr) == 2)
  return(1);

char strsql[512];
sprintf(strsql,"delete from Kascnsl where god=%s and kod=%s and ks=%s",god,kod,kas);

return(sql_zapis(strsql,1,0));

}

/*********************************************/
/*Распечатка списка*/
/*********************************************/
void dirkcnsl_ras(SQLCURSOR *cur,class dirkcnsl_poi *poi)
{
FILE	*ff;
char    imaf[64];
SQL_str row,row1;
class SQLCURSOR cur1;
char strsql[512];

GDITE();

sprintf(imaf,"skcn%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

iceb_t_zagolov(gettext("Стартовое сальдо по кодам целевого назначения"),0,0,0,0,0,0,ff);

fprintf(ff,"\
---------------------------------------------------------\n");
fprintf(ff,gettext("\
Год |Код|        Наименование          |Касса|  Сальдо  |\n"));
fprintf(ff,"\
---------------------------------------------------------\n");

cur->poz_cursor(0);
class iceb_tu_str naim_kodcn("");
double itogo=0.;
while(cur->read_cursor(&row) != 0)
 {
  if(dirkcnsl_prov(row,poi) != 0)
     continue;

  naim_kodcn.new_plus("");
  sprintf(strsql,"select naik from Kascn where kod=%s",row[1]);
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_kodcn.new_plus(row1[0]);

  fprintf(ff,"%*s %-*s %-*.*s %-*s %10.2f\n",
  iceb_tu_kolbait(4,row[4]),row[0],
  iceb_tu_kolbait(3,row[1]),row[1],
  iceb_tu_kolbait(30,naim_kodcn.ravno()),iceb_tu_kolbait(30,naim_kodcn.ravno()),naim_kodcn.ravno(),
  iceb_tu_kolbait(5,row[2]),row[2],
  atof(row[3]));
  itogo+=atof(row[3]);
 }

fprintf(ff,"\
---------------------------------------------------------\n");
fprintf(ff,"%45s %10.2f\n",gettext("Итого"),itogo);

podpis(ff);
fclose(ff);
class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Стартовое сальдо по кодам целевого назначения"));
iceb_t_ustpeh(imaf,0);
iceb_t_rabfil(&oth,"");

}
/******************************/
/*Проверка условий поиска*/
/****************************/

int dirkcnsl_prov(SQL_str row,class dirkcnsl_poi *poi)
{
if(poi->metka_poi == 0)
 return(0);
if(proverka(poi->god.ravno(),row[0],0,0) != 0)
 return(1);
if(proverka(poi->kod.ravno(),row[1],0,0) != 0)
 return(1);
if(proverka(poi->kas.ravno(),row[2],0,0) != 0)
 return(1);
 
return(0);
}

/*******************************/
/*ввод реквизитов поиска*/
/******************************/

void dirkcnsl_vvod_rp(class dirkcnsl_poi *poi)
{
class iceb_tu_str kod("");
class iceb_tu_str naim("");
class iceb_tu_str kodch("");

int kom1=0;

VVOD MENU(3);
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Введите данные для поиска."));

VV.VVOD_SPISOK_add_MD(gettext("Год......................"));
VV.VVOD_SPISOK_add_MD(gettext("Код целевого назначения.."));
VV.VVOD_SPISOK_add_MD(gettext("Код кассы................"));

VV.VVOD_SPISOK_add_data(poi->god.ravno(),8);
VV.VVOD_SPISOK_add_data(poi->kod.ravno(),32); 
VV.VVOD_SPISOK_add_data(poi->kas.ravno(),32);

naz:;

 
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

int kom=VV.vvod(0,1,1,-1,-1);

move(LINES-1,0);

poi->god.new_plus(VV.VVOD_SPISOK_return_data(0));
poi->kod.new_plus(VV.VVOD_SPISOK_return_data(1));
poi->kas.new_plus(VV.VVOD_SPISOK_return_data(2));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   prosf(strsql);
   goto naz;

  case ESC:
  case FK10:
   poi->metka_poi=0;
   return;

  case FK2:
  case PLU:
    poi->metka_poi=1;
    break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка кодов целевого назначения"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка касс"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    kod.new_plus("");
    naim.new_plus("");

    switch (kom1)
     {
      case 2 :
      case -1:
        break;
 
      case 0:
        naim.new_plus("");
        if(dirkcn(1,&kod,&naim) == 0)
         VV.data_plus(1,kod.ravno());
        break;

      case 1:
        naim.new_plus("");
        if(dirkas(1,&kodch,&naim) == 0)
           VV.data_plus(2,kodch.ravno());
        break;

     }      
    goto naz;

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }






}
