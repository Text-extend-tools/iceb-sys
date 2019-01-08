/*$Id: diropld.c,v 5.23 2013/05/17 14:55:58 sasa Exp $*/
/*13.07.2015	04.09.2007	Белых А.И.	diropld.c
Ввод и корректировка оплаты по документа
*/
#include <math.h>
#include	"buhg.h"

void diropld_prov_op(int metka_ps,short dd,short md,short gd,const char *nomdok,int podr,double sumaopl,double suma_dok);
void diropldok_s(short dd,short md,short gd,const char *nomdok,double suma_kopl,int podr);
void diropld_vz(int metka_ps,short dd,short md,short gd,int podr,const char *nomdok,time_t vrem_zap);


void diropld(int metka_ps, //1-материальный учёт 2-учёт услуг 3-учёт основных средств
short dd,short md,short gd, //Дата документа
const char *nomdok, //номер документа
int podr, //подразделение
double suma_opl) /*Сумма к оплате по документа*/
{
int		prc;
char strsql[512];
int kolstr=0;
int K=0;
short           x=0,y=2;
long		pozz=0; /*Позиция первой строчки меню*/
long            pozz1=0; /*Количество строк прочитанных для заполнения меню*/
SQL_str         row;
short		mspz; /*метка соответствия первой записи*/
int kom=0;
int i=0;
double itogo=0.;

class iceb_t_mstr spis_menu(0);

VVOD DANET(1);
VVOD SOOB(1);

naz1:;

clear();
GDITE();

short           kls=LINES-5;
time_t vrem_zap[kls];


sprintf(strsql,"select dato,shet,suma,kekv,nomp,kom,vrem from Opldok where ps=%d and datd = '%04d-%02d-%02d' \
and pd=%d and nomd='%s' order by datd asc",metka_ps,gd,md,dd,podr,nomdok);


SQLCURSOR cur;
SQLCURSOR cur1;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {

lll:;


  diropldok_s(dd, md,gd,nomdok,suma_opl,podr);

  SOOB.VVOD_delete();
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одного документа !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);
  
  if(K == FK5 || K == SFK2 || K == FK3 || K == ENTER)
     goto lll;
  if(K == KEY_RESIZE)
   goto naz1;
     
  goto vper;
 }
else
 {
  itogo=0.;
  while(cur.read_cursor(&row) != 0)
   itogo+=atof(row[2]);   
 }
naz:;
GDITE();

if(kolstr != 0 && pozz >= kolstr)
  pozz = kolstr-1;


cur.poz_cursor(pozz);
mspz=2;
pozz1=0;
short d,m,g;
spis_menu.spis_str.free_class();

for(i=0; i<kls ; )
 {
  
  if(cur.read_cursor(&row) == 0)
    break;

  pozz1++;

  
  rsdat(&d,&m,&g,row[0],2);

  sprintf(strsql,"%02d.%02d.%d|%-8s|%-*s|%-5s|%10.2f|%s",
  d,m,g,row[1],
  iceb_tu_kolbait(4,row[3]),row[3],
  row[4],atof(row[2]),row[5]);
  
  vrem_zap[i]=atol(row[6]);
  

  spis_menu.spis_str.plus(strsql);
  i++;

  if(pozz1 == 1) /*Первая запись удовлетворяет условиям поиска*/
   mspz=1;

 }

if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

if(i == 0)
 {
  if(kolstr != 0 && pozz != 0)
   {
    //Если записи есть но не подходят по условиям поиска
    pozz--;
    goto naz;
   }
  goto lll;
 }

diropldok_s(dd, md,gd,nomdok,suma_opl,podr);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,prc,&K,"",0);


if(kom == -7) //Изменение размера экрана
  goto naz1;
  
if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  for(i=0; i< mspz  ; )
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

    i++;
   }

  goto naz;
 }
 
if(kom == -5) /*Страница вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  if(pozz + kls >= kolstr)
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
  goto naz;

 }

if(kom == -4) /*Страница вверх*/
 {
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
    if(cur.read_cursor(&row) == 0)
      break;

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

  POLE(spis_menu.spis_str.ravno(kom),strsql,1,'|');

 }

vper:;

switch(K)
 {  
    
  case FK10:
  case ESC:
    diropld_prov_op(metka_ps,dd,md,gd,nomdok,podr,itogo,suma_opl);
    return;

  case FK1: /*Помощь*/
    GDITE();
//    prosf(strsql);
    if(kolstr > 0)
      goto naz;
    else
      goto lll;

  case FK2:  /*Ввод новой записи*/
      diropld_vz(metka_ps,dd,md,gd,podr,nomdok,0);
      goto naz1;

  case SFK2:  /*Корректировка записи*/
  case ENTER:
    diropld_vz(metka_ps,dd,md,gd,podr,nomdok,vrem_zap[kom]);

    goto naz1;

  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      goto naz;
    sprintf(strsql,"delete from Opldok where vrem=%ld",vrem_zap[kom]);
    sql_zapis(strsql,0,0);
    goto naz1;


  case FK5: /*Расшифровка*/

    goto naz;

           
  default:
    if(kolstr > 0)
      goto naz;
    else
      goto lll; 
 }

}
/*************************************/
/*Шапка меню*/
/***********************/

void diropldok_s(short dd,short md,short gd,const char *nomdok,double suma_opl,int podr)
{
int		i1;
int		X;
clear();
printw("%s:%s %s %d.%d.%d %s:%.2f %s:%d",gettext("Оплата по документу"),nomdok,
gettext("от"),dd,md,gd,
gettext("Сумма к оплате"),
suma_opl,
gettext("Подразделение"),podr);


move(1,0);
printw("      1          2     3     4       5        6");

move(i1=2,X=COLS-21);
printw(gettext("Расшифровка полей:"));
move(++i1,X);
printw("1 %s",gettext("дата оплаты"));
move(++i1,X);
printw("2 %s",gettext("счёт"));
move(++i1,X);
printw("3 %s",gettext("КЭКЗ"));
move(++i1,X);
printw("4 %s",gettext("номер документа"));
move(++i1,X);
printw("5 %s",gettext("сумма"));
move(++i1,X);
printw("6 %s",gettext("комментарий"));

helstr(LINES-1,0,
"F1",gettext("помощь"),
"F2",gettext("запись"),
"F3",gettext("удалить"),
"F10",gettext("выход"),NULL);


}
/**************************/
/*Запись в базу*/
/***********************/
int diropld_zap(int metka_ps,
short dd,short md,short gd,
const char *nomdok,
int podr,
class VVOD *VV,time_t vrem_zap)
{
char strsql[512];

attroff(VV->VVOD_return_cs(iceb_CFS));
attron(VV->VVOD_return_cs(iceb_CFM));

if(VV->VVOD_SPISOK_return_data(0)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(1)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(2)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(4)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
  return(1);
 }       
struct OPSHET op_shet;

if(prsh1(VV->data_ravno(1),&op_shet) != 0)
 return(1);
short d,m,g;
if(rsdat(&d,&m,&g,VV->data_ravno(0),1) != 0)
 {
  iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
  return(1);
 }
 
if(vrem_zap == 0) //Ввод новой записи
  sprintf(strsql,"insert into Opldok values(%d,'%04d-%02d-%02d','%d','%s','%04d-%02d-%02d','%s',%.2f,%d,'%s','%s',%d,%ld)",
  metka_ps,
  gd,md,dd,
  podr,
  nomdok,
  g,m,d,
  VV->data_ravno(1),
  VV->data_ravno_atof(2),
  VV->data_ravno_atoi(3),
  VV->data_ravno(4),
  VV->data_ravno_filtr(5),
  iceb_t_getuid(),
  time(NULL));
else
  sprintf(strsql,"update Opldok set \
dato='%04d-%02d-%02d',\
shet='%s',\
suma=%.2f,\
kekv=%d,\
nomp='%s',\
kom='%s',\
ktoi=%d,\
vrem=%ld \
where vrem=%ld",  
  g,m,d,
  VV->data_ravno(1),
  VV->data_ravno_atof(2),
  VV->data_ravno_atoi(3),
  VV->data_ravno(4),
  VV->data_ravno_filtr(5),
  iceb_t_getuid(),
  time(NULL),
  vrem_zap);


if(sql_zapis(strsql,0,0) != 0)
 return(1);
 
return(0);
}

/*****************************************/
/*ввод и корректировка записи*/
/******************************/

void diropld_vz(int metka_ps,
short dd,short md,short gd,
int podr,
const char *nomdok,
time_t vrem_zap)
{
short dop,mop,gop;
char strsql[512];
int		ktozap=0;
struct  tm      *bf;
time_t vremz=0;
int N=0,K=0;

VVOD VV(0);

VV.VVOD_SPISOK_add_data(11);//0
VV.VVOD_SPISOK_add_data(20);//1
VV.VVOD_SPISOK_add_data(20);//2
VV.VVOD_SPISOK_add_data(6);//3
VV.VVOD_SPISOK_add_data(20);//4
VV.VVOD_SPISOK_add_data(100);//5


if(vrem_zap != '\0')
 {
  class SQLCURSOR cur;
  SQL_str row;
  
  sprintf(strsql,"select * from Opldok where vrem=%ld",vrem_zap);
  if(readkey(strsql,&row,&cur) != 1)
   return;   

  rsdat(&dop,&mop,&gop,row[4],2);

  
  sprintf(strsql,"%d.%d.%d",dop,mop,gop);
  VV.VVOD_SPISOK_zapis_data(0,strsql);

  VV.VVOD_SPISOK_zapis_data(1,row[5]);
  
  sprintf(strsql,"%.2f",atof(row[6]));
  VV.VVOD_SPISOK_zapis_data(2,strsql);
  
  VV.VVOD_SPISOK_zapis_data(3,row[7]);

  VV.VVOD_SPISOK_zapis_data(4,row[8]);

  VV.VVOD_SPISOK_zapis_data(5,row[9]);
   
  ktozap=atoi(row[10]);
  vremz=atol(row[11]);
     
 }



VV.VVOD_SPISOK_add_MD(gettext("Дата оплаты......"));//0
VV.VVOD_SPISOK_add_MD(gettext("Счёт............."));//1
VV.VVOD_SPISOK_add_MD(gettext("Сумма............"));//2
VV.VVOD_SPISOK_add_MD(gettext("КЭКЗ............."));//3
VV.VVOD_SPISOK_add_MD(gettext("Номер документа.."));//4
VV.VVOD_SPISOK_add_MD(gettext("Коментарий......."));//5

VV.VVOD_SPISOK_add_ZAG(gettext("Ввод и корректировка оплат по документу"));




if(ktozap != 0)
 {
  VV.VVOD_SPISOK_add_ZAG(iceb_t_kszap(ktozap));
 }

if(vremz != 0)
 {
  bf=localtime(&vremz);
  sprintf(strsql,"%s: %d.%d.%d%s %s:%02d:%02d:%02d",
  gettext("Дата записи"),
  bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
  gettext("г."),
  gettext("Время"),
  bf->tm_hour,bf->tm_min,bf->tm_sec);
  VV.VVOD_SPISOK_add_ZAG(strsql);
 }

naz:;

 helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("реквизиты"),
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
    if(diropld_zap(metka_ps,dd,md,gd,nomdok,podr,&VV,vrem_zap) != 0)
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

/********************************/
/*Проверка полной оплаты документа*/
/**********************************/
void diropld_prov_op(int metka_ps, //1-материальный учёт 2-учёт услуг 3-учёт основных средств
short dd,short md,short gd, //Дата документа
const char *nomdok, //номер документа
int podr, //подразделение
double sumaopl,
double suma_dok)
{
char strsql[512];
int metka_opl=0;
    
if(fabs(sumaopl - suma_dok) > 0.009)
 {
  sprintf(strsql,"%s %f != %f",gettext("Документ не оплачен"),sumaopl,suma_dok);
  iceb_t_soob(strsql);
  metka_opl=0;
 }
else 
 metka_opl=1;

if(metka_ps == 1)
 {   
  sprintf(strsql,"update Dokummat set mo=%d where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",
  metka_opl,gd,md,dd,podr,nomdok);
 }  
if(metka_ps == 2)
 {   
  sprintf(strsql,"update Usldokum set mo=%d where datd='%04d-%02d-%02d' and podr=%d and nomd='%s'",
  metka_opl,gd,md,dd,podr,nomdok);
 }  
if(metka_ps == 3)
 {   
  sprintf(strsql,"update Uosdok set mo=%d where datd='%04d-%02d-%02d' and nomd='%s'",
  metka_opl,gd,md,dd,nomdok);
 }  

sql_zapis(strsql,0,0);

return;
}


