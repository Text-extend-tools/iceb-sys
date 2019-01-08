/* $Id: dirobq.c,v 5.30 2013/05/17 14:55:58 sasa Exp $ */
/*27.04.2013    15.04.1997      Белых А.И.      dirobq.c
Просмотр объявок на сдачу денег в банк
*/
#include        "buhg.h"

void            shsmn6(short,short,const char*,short,short,short,short,short,short);
void		nalish(VVOD*,short);
void		soznalish(VVOD*);
int provrp1(short d,short m,short g,short dn,short mn,short gn,short dk,short mk,short gk,const char *nomdokv,const char *npp,short mpn,short prov,short podt);


void            dirobq()
{
class iceb_tu_str bros("");
short           d,m,g,dn,mn,gn,dk,mk,gk;
int             K;
short           x=0,y=2;
short           i,kom;
short           metpoi;
static class iceb_tu_str dat1(""),dat2("");
short		mpn; /*0-все 1-без проводок*/
short		kl;
class iceb_tu_str nomdokv("");
class iceb_tu_str npp("");
class iceb_tu_str datpr("");
char sumc[64];
class iceb_tu_str kop("");
long		pozz,pozz1;
long		kolstr;
SQL_str         row,row1;
char		strsql[1024];
short		prov=0,podt=0;
int		prc;
double		sum=0.;
class iceb_tu_str fam("");
class iceb_tu_str naim("");
class iceb_t_mstr spis_menu(0);
SQLCURSOR       cur1;
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);

//Создаем класс меню
VVOD DANET(1);
VVOD VV(0);
mpn=kom=0;

metpoi=0;
dn=dk=0;
 
pozz=dn=mn=dk=mk=gk=0;
gn=gt;
mn=mt;
dn=1;

naz1:;

clear();
GDITE();

short           kls=LINES-5;

sprintf(strsql,"select * from Obqvka where datd >= '%d-%02d-%02d'",
gn,mn,dn);

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {

lll:;
  shsmn6(mpn,metpoi,nomdokv.ravno(),dn,mn,gn,dk,mk,gk);

  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == FK2 || K == FK10 || K == ESC || K == FK4 || K == FK1)
   goto vper;

  if(K == KEY_RESIZE)
   goto naz1;
   
  goto naz1;
 }

naz:;
GDITE();
if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;

cur.poz_cursor(pozz);
pozz1=0;
spis_menu.spis_str.free_class();

for(i=0;i<=kls-1;)
 {
  
  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;
  rsdat(&d,&m,&g,row[0],2);
  prov=atoi(row[5]);
  podt=atoi(row[6]);  

  if((prc=provrp1(d,m,g,dn,mn,gn,dk,mk,gk,nomdokv.ravno(),row[1],mpn,prov,podt)) != 0)
   {
/*
    printw("prc=%d\n",prc);
    OSTANOV();
*/
    continue;  
   }

  bros.new_plus("");
  if(prov == 0)
   bros.plus("*");
/*
  if(podt == 0)
   strcat(bros,"?");
*/
  sprintf(strsql,"%02d.%02d.%d|%-*s|%-*s|%-4s|%8.2f|%-*s|%s",
  d,m,g,
  iceb_tu_kolbait(3,row[4]),row[3],
  iceb_tu_kolbait(4,row[1]),row[1],
  row[7],atof(row[2]),
  iceb_tu_kolbait(10,row[4]),row[4],
  bros.ravno());

  spis_menu.spis_str.plus(strsql);
  i++;

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


shsmn6(mpn,metpoi,nomdokv.ravno(),dn,mn,gn,dk,mk,gk);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,prc,&K,"",0);


if(kom == -7) //Изменение размеров экрана
  goto naz1;

if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  for(i=0; i<1  ; )
   {
    pozz++;

    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) != 0)
      break;
    if(provrp1(d,m,g,dn,mn,gn,dk,mk,gk,nomdokv.ravno(),row[1],mpn,prov,podt) != 0)
      continue;  

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

    if(provrp1(d,m,g,dn,mn,gn,dk,mk,gk,nomdokv.ravno(),row[1],mpn,prov,podt) != 0)
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

    if(provrp1(d,m,g,dn,mn,gn,dk,mk,gk,nomdokv.ravno(),row[1],mpn,prov,podt) != 0)
      continue;  
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

  polen(spis_menu.spis_str.ravno(kom),&datpr,1,'|');
  rsdat(&d,&m,&g,datpr.ravno(),1);

  polen(spis_menu.spis_str.ravno(kom),&kop,2,'|');
  polen(spis_menu.spis_str.ravno(kom),&npp,3,'|');
  polen(spis_menu.spis_str.ravno(kom),&sum,5,'|');
  polen(spis_menu.spis_str.ravno(kom),&fam,6,'|');
 }

vper:;
    
switch(K)
 {
  case FK10:
  case ESC:
    return;

  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("dokum4.txt");
   clear();
   if(kolstr > 0)
     goto naz;
   else
     goto lll;
    
  case FK4: /*Поиск*/
    VV.VVOD_delete();
    VV.VVOD_SPISOK_add_ZAG(gettext("Введите нужные реквизиты"));

    VV.VVOD_SPISOK_add_MD(gettext("Номер документа.."));
    VV.VVOD_SPISOK_add_MD(gettext("Дата начала......"));
    VV.VVOD_SPISOK_add_MD(gettext("Дата конца......."));
    VV.VVOD_SPISOK_add_data(nomdokv.ravno(),20);
    VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
    VV.VVOD_SPISOK_add_data(dat2.ravno(),11);


    helstr(LINES-1,0,"F2/+",gettext("поиск"),
    "F10",gettext("выход"),NULL);

    
    kl=VV.vvod(0,1,1,-1,-1);

    if(kl == FK2 || kl == PLU)
     {
      GDITE();
      dn=mn=gn=dk=mk=gk=0;
      nomdokv.new_plus(VV.VVOD_SPISOK_return_data(0));
      dat1.new_plus(VV.VVOD_SPISOK_return_data(1));
      dat2.new_plus(VV.VVOD_SPISOK_return_data(2));
      metpoi=1;

      if(dat1.ravno()[0] != '\0')
       {
        rsdat(&dn,&mn,&gn,dat1.ravno(),1);
        if(dat2.ravno()[0] == '\0')
         {
          rsdat(&dk,&mk,&gk,dat1.ravno(),1);
          dpm(&dk,&mk,&gk,5);
         }
        else
          rsdat(&dk,&mk,&gk,dat2.ravno(),1);
       }
      goto naz1;
     }

    break;

  case SFK2:  /*Корректировать запись*/
  case ENTER:  /*Корректировать запись*/

    VV.VVOD_delete(); 
    soznalish(&VV);
    VV.VVOD_SPISOK_zapis_data(0,npp.ravno());
    sprintf(strsql,"%.2f",sum);
    VV.VVOD_SPISOK_zapis_data(1,strsql);
    sprintf(strsql,"%d.%d.%d",d,m,g);
    VV.VVOD_SPISOK_zapis_data(2,strsql);
    VV.VVOD_SPISOK_zapis_data(3,kop.ravno());
    VV.VVOD_SPISOK_zapis_data(4,fam.ravno());

    nalish(&VV,1);
    goto naz1;
 


  case FK2:  /*Ввести зпись*/

    prc=iceb_t_nomdok(gt,"Obqvka");


    VV.VVOD_delete(); 
    soznalish(&VV);
    sprintf(strsql,"%d",prc);
    VV.VVOD_SPISOK_zapis_data(0,strsql);
    sprintf(strsql,"%d.%d.%d",dt,mt,gt);
    VV.VVOD_SPISOK_zapis_data(2,strsql);

    /*Ищем операцию*/
    sprintf(strsql,"select kod from Oznvb");
    if(sql_readkey(&bd,strsql,&row1,&cur1) > 0)
     {
      VV.VVOD_SPISOK_zapis_data(3,row1[0]);
     }      

    nalish(&VV,0);
    
    goto naz1;

  case FK3:  /*Удалить запись*/
   
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      sprintf(strsql,"delete from Obqvka where datd='%d-%02d-%02d' and\
 nomd='%s'",g,m,d,npp.ravno());
/*      printw("\nstrsql=%s\n",strsql);*/

      sql_zapis(strsql,0,0);

      clear();
      goto naz1;
     }      
    goto naz;

  case FK5:  /*распечатать*/
    /*Ищем операцию*/
    sprintf(strsql,"select naik from Oznvb where kod='%s'",kop.ravno());
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
       sprintf(strsql,"%s %s!",gettext("Не найдена операция"),kop.ravno());
       iceb_t_soob(strsql);
       goto naz;
     }
    naim.new_plus(row1[0]);
    sprintf(sumc,"%.2f",sum);
    rasobqv(npp.ravno(),sumc,datpr.ravno(),naim.ravno(),fam.ravno());

    goto naz;

  case FK6:  /*без проводок*/
    
    if(mpn == 1)
      mpn=0;
    else
      mpn=1;  
    goto naz;

default:
  if(kolstr > 0)
    goto naz;
  else
    goto lll;
    
}


}

/*******/
/*Шапка*/
/*******/
void		shsmn6(short mpn,short metpoi,const char *nomdokv,short dn,
short mn,short gn,short dk,short mk,short gk)
{
short		X=COLS-18,Y=2;
short		i1;
clear();

move(0,0);

if(mpn == 0)
 printw(gettext("Просмотр объявок"));

if(mpn == 1)
 {
  attron(A_BLINK); /*Влючение мерцания*/
  printw(gettext("Просмотр об'явок без проводок"));
  attroff(A_BLINK); /*Выключение мерцания*/
 }

move(1,0);
printw("        1     2    3   4      5         6");

move(Y++,X);
printw(gettext("Расшифровка полей:"));
move(Y++,X);
printw(gettext("1 дата документа"));
move(Y++,X);
printw(gettext("2 код операции"));
move(Y++,X);
printw(gettext("3 номер документа"));
move(Y++,X);
printw(gettext("4 кто записал"));
move(Y++,X);
printw(gettext("5 сумма платежа"));
move(Y++,X);
printw(gettext("6 через кого"));
move(Y++,X);
printw(gettext("* не вып-ны пров."));

if(metpoi == 1)
 {
  i1=13;
  move(i1,61);
  attron(A_BLINK);
  printw(gettext("Поиск:"));
  if(nomdokv[0] != '\0')
   {
    move(++i1,X);
    printw("%s %s",gettext("Н.д."),nomdokv);
   }

  if(gn != 0 || gk != 0)
   {
    move(++i1,X);
    printw("%s%d.%d.%d",gettext("Дата нач."),dn,mn,gn);
   }
  if( gk != 0)
   {
    move(++i1,X);
    printw("%s%d.%d.%d",gettext("Дата кон."),dk,mk,gk);
   }

  attroff(A_BLINK);
 }
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
//"F6","*",
"F10",gettext("выход"),NULL);

}
/***************************/
/*Проверка реквизитов поиска*/
/*****************************/

int provrp1(short d,short m,short g,short dn,short mn,
short gn,short dk,short mk,short gk,const char *nomdokv,const char *npp,
short mpn,short prov,short podt)
{
if(dn != 0)
if(SRAV1(gn,mn,dn,g,m,d) < 0)
  return(1);

if(dk != 0)
if(SRAV1(gk,mk,dk,g,m,d) > 0)
  return(2);

if(nomdokv[0] != '\0')
  if(strstrm(npp,nomdokv) == 0)
    return(3);

if(mpn == 1 && prov == 0)
     return(4);

if(mpn == 2 && podt != 0)
     return(5);

return(0);
}
