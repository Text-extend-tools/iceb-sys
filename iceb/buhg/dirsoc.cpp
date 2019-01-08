/* $Id: dirsoc.c,v 5.36 2013/09/26 09:43:31 sasa Exp $ */
/*13.07.2015	03.04.2001	Белых А.И.	dirsoc.c
Просмотр ввод и корректировка соц-отчислений на фонд зарплаты
*/
#include        <errno.h>
#include        "buhg.h"

void	sapsoc(long,short,short,const char*,double);
void	vvodsoc(VVOD*,int,short,short,short);
void	sozvvodsoc(VVOD*);


void dirsoc(int tabn,//Табельный номер
short mp,short gp) //Дата просмотра
{
int		kom,kom1;
long		pozz,pozz1;
long		kolstr;
SQL_str         row,row1;
char		strsql[1024];
int		i;
int		K;
int		prc;
short		mspz; /*метка соответствия первой записи*/
class iceb_tu_str kodz("");
class iceb_tu_str shetz("");
class iceb_tu_str sumas("");
class iceb_tu_str sumap("");
class iceb_tu_str proc("");
class iceb_tu_str sumapb("");
class iceb_tu_str fio("");
class iceb_tu_str naim("");
class iceb_t_mstr spis_menu (0);
double suma_itog=0.;

SQLCURSOR cur;
SQLCURSOR cur1;

//Читаем фамилию
sprintf(strsql,"select fio from Kartb where tabn=%d",tabn);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 fio.new_plus(row[0]);
 
//Создаем класс меню
VVOD DANET(1);
VVOD VV(0);
K=kom=kom1=pozz=pozz1=0;

naz1:;
suma_itog=0.;

short		kls=LINES-5;
int ktoz[kls];
time_t vremzap[kls];
int nom_zap[kls];
sprintf(strsql,"select * from Zarsocz where datz='%d-%d-01' and tabn=%d",
gp,mp,tabn);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {

lll:;
  clear();
  sapsoc(tabn,mp,gp,fio.ravno(),suma_itog);

  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == FK3 || K == FK5 || K == ENTER || K == SFK2)
    goto lll;

  if(K == KEY_RESIZE)
   goto naz1;
   
  goto vper;

 }

naz:;
GDITE();
clear();
suma_itog=0.;

if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;

cur.poz_cursor(pozz);
mspz=2;
pozz1=0;
spis_menu.spis_str.free_class();

for(i=0; i < kls ;)
 {

  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;
  naim.new_plus("");
  sprintf(strsql,"select naik from Zarsoc where kod=%s",row[2]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 0)
    naim.new_plus(row1[0]);
  suma_itog+=atof(row[5]);

  ktoz[i]=atoi(row[7]);
  vremzap[i]=atol(row[8]);
  nom_zap[i]=atoi(row[11]);

  sprintf(strsql,"%-2s|%-*.*s|%-*s|%8s|%8s|%6s|%8s",
  row[2],
  iceb_tu_kolbait(12,naim.ravno()),iceb_tu_kolbait(12,naim.ravno()),naim.ravno(),
  iceb_tu_kolbait(5,row[3]),row[3],
  row[4],row[5],row[6],row[9]);  

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
  if(kolstr > 0 && pozz > 0)
   {
    pozz--;
    goto naz;
   }
  goto lll;
 }

if(i <= kls-1)

sapsoc(tabn,mp,gp,fio.ravno(),suma_itog);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,2,0,NULL,strsql,prc,&K,"",0);


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

//    if(provp(mp,row[2],obr,row[1],row[0],mp1,mpz,inn,row[3],mprov) != 0)
//      continue;

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


//    if(provp(mp,row[2],obr,row[1],row[0],mp1,mpz,inn,row[3],mprov) != 0)
//      continue;
    i++;
   }

  goto naz;
 }
 
if(kom == -5) /*Страница вниз*/
 {
/*
  printw("\n вперед pozz=%d kolst=%d kls=%d pozz1=%d\n",
  pozz,kolstr,kls,pozz1);
  OSTANOV();
*/
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


//    if(provp(mp,row[2],obr,row[1],row[0],mp1,mpz,inn,row[3],mprov) != 0)
//      continue;
    i++;
   }
  goto naz;
 }

if(kom >= 0)
 {
  if(SRAV(gettext("Конец списка"),spis_menu.spis_str.ravno(kom),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz;

  sprintf(strsql,"select * from Zarsocz where nz=%d",nom_zap[kom]);
  readkey(strsql,&row,&cur);

  kodz.new_plus(row[2]);
  shetz.new_plus(row[3]);
  sumas.new_plus(row[4]);
  sumap.new_plus(row[5]);
  proc.new_plus(row[6]);
  sumapb.new_plus(row[9]);

 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return;

  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("zarp2_9.txt");
   clear();
   if(kolstr > 0)
     goto naz;
   else
     goto lll;

  case FK2:   /*ВВод*/
  case PLU:
    if(iceb_t_pbpds(mp,gp) != 0)
     goto naz;
     
    VV.VVOD_delete();
    sozvvodsoc(&VV);
    vvodsoc(&VV,tabn,mp,gp,0);
    goto naz1;

  case SFK2:   /*Корректировка записи*/
  case ENTER:
//    if(iceb_t_pbpds(mp,gp,"zarblok.alx") != 0)
//     goto naz;
    

    VV.VVOD_delete();
    sozvvodsoc(&VV);
    kzvz(ktoz[kom],vremzap[kom],&VV);
    VV.VVOD_SPISOK_zapis_data(0,kodz.ravno());
    VV.VVOD_SPISOK_zapis_data(1,shetz.ravno());

    sprintf(strsql,"%.2f",sumas.ravno_atof());
    VV.VVOD_SPISOK_zapis_data(2,strsql);
    sprintf(strsql,"%.2f",sumap.ravno_atof());
    VV.VVOD_SPISOK_zapis_data(3,strsql);
    sprintf(strsql,"%.2f",proc.ravno_atof());
    VV.VVOD_SPISOK_zapis_data(4,strsql);
    sprintf(strsql,"%.2f",sumapb.ravno_atof());
    VV.VVOD_SPISOK_zapis_data(5,strsql);
    
    //Читем запись чтобы взять сумму с которой начислялся бюджет так как ее нет на экране
    sprintf(strsql,"select sumasb from Zarsocz where datz='%d-%d-%d' and tabn=%d and kodz=%s",    
    gp,mp,1,tabn,kodz.ravno());
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
      VV.VVOD_SPISOK_zapis_data(6,row1[0]);
    
    vvodsoc(&VV,tabn,mp,gp,1);
    goto naz1;

  case FK3:   /*Удалить запись*/
    if(iceb_t_pbpds(mp,gp) != 0)
     goto naz;
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      sprintf(strsql,"delete from Zarsocz where datz='%d-%d-01' and \
tabn=%d and kodz=%s",gp,mp,tabn,kodz.ravno());
      if(sql_zap(&bd,strsql) != 0)
        msql_error(&bd,"",strsql);      
      goto naz1;
     }
    goto naz;

  case FK4:   /*Расчёт*/
    if(iceb_t_pbpds(mp,gp) != 0)
     goto naz;
     
    zarsoc(mp,gp,tabn);

     
    goto naz1;

  case FK9:   /*Просмотр протокола расчёта*/
//     prosf(imaf_prot);
     zar_pr_show(3,tabn,mp,gp);
     goto naz;
                           
   default:
    if(kolstr > 0)
      goto naz;
    else
      goto lll;

 }  

}

/********/
/*Шапка */
/********/

void	sapsoc(long tabn,short mp,short gp,const char *fio,double suma_itog)
{
int		Y,X;

printw("%s %d %.30s %s %d.%d%s",
gettext("Просмотр соц-отчислений для"),
tabn,fio,
gettext("на"),
mp,gp,gettext("г."));
move(1,0);
printw("  1       2         3       4       5       6      7");

move(Y=2,X=COLS-20);
printw(gettext("Расшифровка полей:"));

move(++Y,X);
printw(gettext("1-код соц-отч."));

move(++Y,X);
printw(gettext("2-наим.соц-отч."));

move(++Y,X);
printw(gettext("3-счет соц-отч."));

move(++Y,X);
printw(gettext("4-с какой суммы"));

move(++Y,X);
printw(gettext("5-сумма соц-отч."));

move(++Y,X);
printw(gettext("6-процент соц-отч."));

move(++Y,X);
printw(gettext("7-бюджетное соц-отч."));

move(++Y,X);
printw("%.2f",suma_itog);

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("расч."),
"F9",gettext("прот."),
"F10",gettext("выход"),NULL);

}
/*****************************/
/*Ввод и корректировка записи*/
/*****************************/

void	vvodsoc(VVOD *VV,int tabn,short mp,short gp,
short metkaz) //0-новая запись 1-корректировка старой
{
int		K=0;
int		N=0;
char		strsql[512];
SQL_str		row;
SQLCURSOR       cur1;
time_t 		vrem;
int		kodz=atoi(VV->VVOD_SPISOK_return_data(0));

VV->VVOD_SPISOK_add_MD(gettext("Код соц-отчисления........................"));
VV->VVOD_SPISOK_add_MD(gettext("Счёт соц-отчисления......................."));
VV->VVOD_SPISOK_add_MD(gettext("Сумма с которой начислено соц-отчисление.."));
VV->VVOD_SPISOK_add_MD(gettext("Сумма соц-отчисления......................"));
VV->VVOD_SPISOK_add_MD(gettext("Процент соц-отчисления...................."));
VV->VVOD_SPISOK_add_MD(gettext("Сумма соц-отчисления с бюджета............"));
VV->VVOD_SPISOK_add_MD(gettext("Сумма с которой начислено соц-от.бюджета.."));

naz:;

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F10",gettext("выход"),NULL);

VV->vramka(-1,-1,0);

attron(VV->VVOD_return_cs(iceb_CFS)); 

VV->vprintw(N,stdscr);

for(;;)
 {
  refresh();
  K=getch();

  attroff(VV->VVOD_return_cs(iceb_CFS));
  attron(VV->VVOD_return_cs(iceb_CFM));

  VV->vprintw(N,stdscr);

  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV->PY,VV->PX,VV->MDLS,VV->KLST,stdscr);

  if(K == ENTER)
   {
    attroff(VV->VVOD_return_cs(iceb_CFM));
    attron(VV->VVOD_return_cs(iceb_CFV));
    VV->vprintw_clear(N,stdscr);

//    mygetstr(VV->VVOD_SPISOK_return_data(N),VV->VVOD_SPISOK_return_dls(N),stdscr);
    VV->vgetstr(N,stdscr);
    
    attroff(VV->VVOD_return_cs(iceb_CFV));
    attron(VV->VVOD_return_cs(iceb_CFM));

    if(N == 0)
     {
     }

    VV->vprintw(N,stdscr);
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
  if(N > VV->KLST-1)
   {
    N=0;
   }
  if(N < 0)
   {
    N=VV->KLST-1;
   }
  attron(VV->VVOD_return_cs(iceb_CFS));
  VV->vprintw(N,stdscr);

   if(K == ESC || K == FK10)
    {
      attroff(VV->VVOD_return_cs(iceb_CFS));
      attron(VV->VVOD_return_cs(iceb_CFM));
     N=-1;
     return;
    }
   if(K == FK2 || K == PLU) /*Записываем*/
    {
     attroff(VV->VVOD_return_cs(iceb_CFS));
     attron(VV->VVOD_return_cs(iceb_CFM));

     if(VV->VVOD_SPISOK_return_data(0)[0] == '\0')
      {
       VVOD SOOB(1);
       SOOB.VVOD_SPISOK_add_MD(gettext("Не введён код соц. страхования !"));
       soobshw(&SOOB,stdscr,-1,-1,0,1);
       goto naz;
      }
     if(iceb_t_pbpds(mp,gp) != 0)
       goto naz;
     

     //Проверяем есть ли такой код 
     sprintf(strsql,"select kod from Zarsoc where kod=%s",VV->VVOD_SPISOK_return_data(0));
     if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
      {
       VVOD SOOB(1);
       sprintf(strsql,"%s %s !",
       gettext("Не найден код соц. страхования"),VV->VVOD_SPISOK_return_data(0));
       SOOB.VVOD_SPISOK_add_MD(strsql);
       soobshw(&SOOB,stdscr,-1,-1,0,1);
       goto naz;
      }

     //Проверяем есть ли такой счет
     struct OPSHET shetv;
     if(prsh1(VV->VVOD_SPISOK_return_data(1),&shetv) != 0)
       goto naz;
     
     time(&vrem);
     if(metkaz == 0)
      sprintf(strsql,"insert into Zarsocz (datz,tabn,kodz,shet,sumas,sumap,proc,ktoz,vrem,sumapb,sumasb) \
values ('%d-%d-01',%d,%s,'%s',%.2f,%.2f,%.2f,%d,%ld,%.2f,%.2f)",
      gp,mp,tabn,VV->VVOD_SPISOK_return_data(0),VV->VVOD_SPISOK_return_data(1),ATOFM(VV->VVOD_SPISOK_return_data(2)),ATOFM(VV->VVOD_SPISOK_return_data(3)),
      ATOFM(VV->VVOD_SPISOK_return_data(4)),iceb_t_getuid(),vrem,ATOFM(VV->VVOD_SPISOK_return_data(5)),
      ATOFM(VV->VVOD_SPISOK_return_data(6)));

     if(metkaz == 1)
       sprintf(strsql,"update Zarsocz set \
shet='%s',\
sumas=%.2f,\
sumap=%.2f,\
proc=%.2f,\
ktoz=%d,\
vrem=%ld,\
sumapb=%.2f,\
sumasb=%.2f \
where datz='%04d-%d-01' and tabn=%d and kodz=%d",
       VV->VVOD_SPISOK_return_data(1),ATOFM(VV->VVOD_SPISOK_return_data(2)),ATOFM(VV->VVOD_SPISOK_return_data(3)),ATOFM(VV->VVOD_SPISOK_return_data(4)),
      iceb_t_getuid(),vrem,ATOFM(VV->VVOD_SPISOK_return_data(5)),ATOFM(VV->VVOD_SPISOK_return_data(6)),
      gp,mp,tabn,kodz);

     GDITE();
     if(sql_zap(&bd,strsql) != 0)
      {
       msql_error(&bd,"",strsql);
       goto naz;
      }
     return;
    }
 }
}

/**************************************/

void	sozvvodsoc(VVOD *VV)
{
VV->VVOD_SPISOK_add_data(2);
VV->VVOD_SPISOK_add_data(20);
VV->VVOD_SPISOK_add_data(20);
VV->VVOD_SPISOK_add_data(20);
VV->VVOD_SPISOK_add_data(6);
VV->VVOD_SPISOK_add_data(20);
VV->VVOD_SPISOK_add_data(20);

}

