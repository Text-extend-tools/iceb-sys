/* $Id: dirmusl.c,v 5.28 2013/05/17 14:55:58 sasa Exp $ */
/*13.07.2015    02.11.1993      Белых А.И.       dirmusl.c
Просмотр ввод и корректировка услуг
*/
#include        <errno.h>
#include        "buhg.h"

void            vmak130(short mr,int d,int m,int g,int skl,const char *nn,float pnds,int nomer_zap);
void            shmk130(short,short,short,const char*,double);

extern double	okrcn;  /*Округление цены*/
extern double	okrg1;

int dirmusl(short d,short m,short g,int skl,const char *nn,float pnds)
{
int             K;
short           kom;
int             i;
double          bb;
class iceb_tu_str naiusl("");
double		kolih,cena;
class iceb_tu_str ei("");
double		nds;
double		suma;
long		kolstr;
long		pozz,pozz1;
SQL_str         row,row1;
class iceb_t_mstr spis_menu(0);
char		strsql[1024];
int		prc;

kom=0;
pozz=pozz1=0;

//Создаем класс меню
VVOD VV(0);
VVOD DANET(1);

naz1:;
clear();
GDITE();
short           kls=LINES-5;
int nomer_zap[kls];

suma=0.;
K=0;
memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"select * from Dokummat3 where datd='%04d-%02d-%02d' and \
sklad=%d and nomd='%s'",g,m,d,skl,nn);
/*printw("\nstrsql=%s\n",strsql);*/
SQLCURSOR cur;
SQLCURSOR cur1;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
if(kolstr == 0)
 {
lll:;
  shmk130(d,m,g,nn,suma);

  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == FK3 || K == SFK3) 
      goto lll;

  if(K == KEY_RESIZE)
   goto naz1;
   
  goto vper;
 }

naz:;
GDITE();
if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;
cur.poz_cursor(pozz);
pozz1=0;
suma=0.;
spis_menu.spis_str.free_class();
for(i=0;i<=kls-1;i++)
 {
  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;
  
  ei.new_plus(row[3]);
  kolih=atof(row[4]);
  cena=atof(row[5]);
  cena=okrug(cena,okrcn);
  nds=atof(row[6]);
  naiusl.new_plus("");
  if(atoi(row[10]) != 0)
   {
    sprintf(strsql,"select naius from Uslugi where kodus=%s",row[10]);
    if(readkey(strsql,&row1,&cur1) == 1)
     naiusl.new_plus(row1[0]);
   }  
  if(naiusl.getdlinna() <= 1)
   naiusl.new_plus(row[7]);
  else
   {
    naiusl.plus("+");
    naiusl.plus(row[7]);
   }

  bb=kolih*cena;
  bb=okrug(bb,okrg1);
  suma+=bb;  
  
  sprintf(strsql,"%4s|%-*.*s|%5.5g|%8.8g|%8.8g|%-*s|%.4g",
  row[10],
  iceb_tu_kolbait(25,naiusl.ravno()),iceb_tu_kolbait(COLS-55,naiusl.ravno()),naiusl.ravno(),
  kolih,cena,bb,
  iceb_tu_kolbait(4,ei.ravno()),ei.ravno(),
  nds);
  
  spis_menu.spis_str.plus(strsql);

  nomer_zap[i]=atoi(row[11]);
 }

if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i <= kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

shmk130(d,m,g,nn,suma);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,1,0,NULL,strsql,prc,&K,"",0);


if(kom == -7)
  goto naz1;
  
if(kom == -3) /*Стрелка вниз*/
 {
  if(i < kls)
   {
    beep();
    goto naz;
   }
  if(i == kls)
    pozz++;
  kom=kls;
  goto naz;
 }
if(kom == -2) /*Стрелка вверх*/
 {
  if(pozz == 0)
   {
    beep();
    goto naz;
   }
  if(pozz != 0)
    pozz--;
  goto naz;
 }
 
if(kom == -5) /*Страница вниз*/
 {
  if(i == kls)
    pozz+=kls;
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
  pozz-=kls;
  if(pozz < 0)  
   {
    beep();
    pozz=0;
   }
/*
  printw("\n назад pozz=%d\n",pozz);
  OSTANOV();
*/
  goto naz;
 }

if(kom >= 0)
 {
  if(SRAV(gettext("Конец списка"),spis_menu.spis_str.ravno(kom),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz;
 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return(1); 
     break;

  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("matu2_1_2b.txt");
   clear();
   if(kolstr > 0)
     goto naz;
   else
    goto lll;

  case ENTER: 
  case SFK2:   /*Корректировать запись*/

    vmak130(0,d,m,g,skl,nn,pnds,nomer_zap[kom]);
    goto naz1;
    break;

  case FK2:  /*Ввести запись*/
  case PLU:

    vmak130(1,d,m,g,skl,nn,pnds,-1);

    goto naz1;
    break;

  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      sprintf(strsql,"delete from Dokummat3 where nz=%d",nomer_zap[kom]); \

      GDITE();
      if(sql_zap(&bd,strsql) != 0)
       {
       if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
        {
         iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
        }
       else
        msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);
       }
      goto naz1;    
     }
    goto naz;
    break;

  case SFK3:       /*Удалить все записи*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить все записи ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      sprintf(strsql,"delete from Dokummat3 where datd='%d-%02d-%02d' \
and sklad=%d and nomd='%s'",g,m,d,skl,nn);
/*      printw("\nstrsql=%s\n",strsql);*/
      GDITE();
      if(sql_zap(&bd,strsql) != 0)
       {
       if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
        {
         iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
        }
       else
        msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);
       }
      goto naz1;    
     }
    goto naz;
    break;

  default:
   if(kolstr > 0)
     goto naz;
   else
    goto lll;
    
 }
}

/*******/
/*Ввод */
/*******/

void vmak130(short mr, //0-корректировка 1- ввод новой записи
int d,int m,int g,int skl,const char *nn,float pnds,int nomer_zap)
{
char		strsql[1024];
time_t		vrem;
class SQLCURSOR cur,cur1;
SQL_str row,row1;
int		K,N;


VVOD VV(0);

VV.VVOD_SPISOK_add_ZAG(gettext("Ввод и корректировка услуг"));

VV.VVOD_SPISOK_add_data(100);
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(8);
VV.VVOD_SPISOK_add_data(10);
VV.VVOD_SPISOK_add_data(100);

if(mr == 0)
 {
  sprintf(strsql,"select ei,kolih,cena,nds,naimu,ktoi,vrem,ku from Dokummat3 where nz=%d",nomer_zap);
  if(readkey(strsql,&row,&cur) != 1)
   {
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("Не найдена запись !"));
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    return;
   }

  VV.VVOD_SPISOK_zapis_data(0,row[4]);
  
  sprintf(strsql,"%.10g",atof(row[1]));
  VV.VVOD_SPISOK_zapis_data(1,strsql);
  
  sprintf(strsql,"%.10g",atof(row[2]));
  VV.VVOD_SPISOK_zapis_data(2,strsql);
  
  VV.VVOD_SPISOK_zapis_data(3,row[0]);
  
  sprintf(strsql,"%.4g",atof(row[3]));
  VV.VVOD_SPISOK_zapis_data(4,strsql);
  if(atoi(row[7]) > 0)
   {
    sprintf(strsql,"select naius from Uslugi where kodus=%s",row[7]);
    if(readkey(strsql,&row1,&cur1) != 1)
     {
      sprintf(strsql,"%s %s !",gettext("Не найден код услуги"),row[7]);
      iceb_t_soob(strsql);
     }
    else
     {
      sprintf(strsql,"%s %s",row[7],row1[0]);
      VV.data_plus(5,strsql);
     }
   }
  kzvz(atoi(row[5]),atol(row[6]),&VV);
 }



VV.VVOD_SPISOK_add_MD(gettext("Наименование услуги.."));//0
VV.VVOD_SPISOK_add_MD(gettext("Количество..........."));//1
VV.VVOD_SPISOK_add_MD(gettext("Цена/сумма..........."));//2
VV.VVOD_SPISOK_add_MD(gettext("Единица измерения...."));//3
VV.VVOD_SPISOK_add_MD(gettext("НДС.(процент)........"));//4
VV.VVOD_SPISOK_add_MD(gettext("Код услуги..........."));//5



if(mr == 1)
 {
  sprintf(strsql,"%.4g",pnds);
  VV.VVOD_SPISOK_zapis_data(4,strsql);
 }

naz:;
 

helstr(LINES-1,0,"F2/+",gettext("запись"),
"F3",gettext("просмотр кодов"),
"F10",gettext("выход"),NULL);


VV.vramka(-1,-1,0);


for(;;)
 {
  if(N > VV.KLST-1)
   {
    N=0;
   }
  if(N < 0)
   {
    N=VV.KLST-1;
   }
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

//    my_getstr(VV.VVOD_SPISOK_return_data(N),dlinastr,VV.VVOD_SPISOK_return_dls(N),stdscr);
    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 5) //Код услуги
     {
      if(VV.data_ravno_atoi(5) > 0)
       {
        sprintf(strsql,"select naius from Uslugi where kodus=%d",VV.data_ravno_atoi(5));
        if(readkey(strsql,&row,&cur) == 1)
         {
          sprintf(strsql,"%d %s",VV.data_ravno_atoi(5),row[0]);
          VV.data_plus(5,strsql);
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

  if(K == ESC || K == FK10)
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    N=-1;
    break;
   }
  if(K == FK2 || K == PLU) /*Записываем*/
   {
    if(VV.VVOD_SPISOK_return_data(0)[0] == '\0' && VV.data_ravno_atoi(5)  <= 0)
     {
      iceb_t_soob(gettext("Не введено наименование услуги !"));
      goto naz;
     }
    
    //проверяем код услуги
    if(VV.data_ravno_atoi(5)  > 0)
     {
      sprintf(strsql,"select kodus from Uslugi where kodus=%d",VV.data_ravno_atoi(5));
      if(readkey(strsql,&row,&cur) != 1)
       {
        sprintf(strsql,"%s %d !",gettext("Не найден код услуги"),VV.data_ravno_atoi(5));
        iceb_t_soob(strsql);
        goto naz;
       }
     }

    time(&vrem);
    
    if(mr == 1)
     sprintf(strsql,"insert into Dokummat3 (datd,sklad,nomd,ei,kolih,cena,nds,naimu,ktoi,vrem,ku)\
  values ('%d-%02d-%02d',%d,'%s','%s',%.10g,%.10g,%.10g,'%s',%d,%ld,%d)",
     g,m,d,skl,nn,VV.VVOD_SPISOK_return_data(3),ATOFM(VV.VVOD_SPISOK_return_data(1)),ATOFM(VV.VVOD_SPISOK_return_data(2)),ATOFM(VV.VVOD_SPISOK_return_data(4)),VV.VVOD_SPISOK_return_data(0),
     iceb_t_getuid(),vrem,VV.data_ravno_atoi(5));

    if(mr == 0)
      sprintf(strsql,"update Dokummat3 \
  set \
  ei='%s',\
  kolih=%.10g,\
  cena=%.10g,\
  nds=%.10g,\
  naimu='%s',\
  ktoi=%d,\
  vrem=%ld,\
  ku=%d \
  where nz=%d",
  VV.VVOD_SPISOK_return_data(3),ATOFM(VV.VVOD_SPISOK_return_data(1)),ATOFM(VV.VVOD_SPISOK_return_data(2)),ATOFM(VV.VVOD_SPISOK_return_data(4)),VV.VVOD_SPISOK_return_data(0),
  iceb_t_getuid(),vrem,VV.data_ravno_atoi(5),
  nomer_zap);
  /*  printw("\nstrsql=%s\n",strsql);*/
    GDITE();
    if(sql_zap(&bd,strsql) != 0)
     {
     if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
      {
       iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
       goto naz;
      }
     else
      msql_error(&bd,gettext("Ошибка записи !"),strsql);
     }
    break;  
   }

  if( K == FK3)
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    class iceb_tu_str naiusl("");
    int kodgr=0;
    int kodusl=0;
    if(dirusl(&kodgr,&kodusl,&naiusl,1,"") == 0)
     {
      sprintf(strsql,"%d %s",kodusl,naiusl.ravno());
      VV.data_plus(5,strsql);
     }
    clear();
    goto naz;
   }

 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
refresh();

}
/**********************/
/*Шапка               */
/**********************/
void shmk130(short d,short m,short g,const char *nn,double suma)
{

attroff(A_REVERSE);  /*Выключить инверсию*/
attroff(A_UNDERLINE);  /*Выключить подчеркивание*/
clear();
move(0,0);
clrtoeol();  /*Очистить стороку*/
printw(gettext("Ввод и корректировка услуг. %d.%d.%d  Документ-%s Сумма - %.10g"),
d,m,g,nn,suma);
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F10",gettext("выход"),NULL);
}

