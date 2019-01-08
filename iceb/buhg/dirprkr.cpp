/*$Id: dirprkr.c,v 5.38 2013/11/05 10:50:48 sasa Exp $*/
/*13.07.2015	28.10.2002	Белых А.И.	dirprkr.c
Работа с проводками для документов командировочных расходов
*/
#include        "buhg.h"

int provprusl(SQL_str,short);
void sapprus(short,short,short,short,short,short,const char*,const char*,const char*,const char*,double,double,double,short);
void avtprukr(short,short,short,const char*);

extern double	nds1;

int dirprkr(short d,short m,short g, //Дата документа
short dao,short mao,short gao, //Дата авансового отчёта
const char *nomdok, //Номер документа
const char *nomao,  //Номер авансового отчёта
const char *vkom, //Вид командировки
const char *kontr, //Код контрагента
double sumad,
double isnds)
{
class iceb_tu_str komen("");
char		strsql[1024];
int		kolstr;
SQL_str		row;
char		metkasys[24];
int		K=0;
int		pozz=0,pozz1=0;
short		mspz; /*метка соответствия первой записи*/
int		i;
short		d1,m1,g1;
int		kom=0;
short           x=0,y=5;
class iceb_tu_str datp("");
class iceb_tu_str shet("");
class iceb_tu_str shetk("");
class iceb_tu_str kor("");
time_t		vrem=0;
int		ktozap=0;
int		i1;
static class iceb_tu_str dpr(""); /*Дата проводки*/
class iceb_tu_str kod("");
class iceb_tu_str naim("");
class iceb_t_mstr spis_menu(0);
class iceb_tu_str kekv("");
VVOD VV(0);
VVOD DANET(1);

strcpy(metkasys,ICEB_MP_UKR);

naz1:;

GDITE();

clear();
short           kls=LINES-8;
time_t		vremm[kls];
int		ktoz[kls];
int		val[kls];
short		mpr=1;
double		sum=0.;
double		deb=0.,kre=0.;


sprintf(strsql,"select datp,sh,shk,kodkon,deb,kre,vrem,komen,ktoi,val,kekv \
from Prov where kto='%s' and pod=%d and nomd='%s' and \
datd='%04d-%02d-%02d' and oper='%s' order by datp asc",
metkasys,0,nomdok,g,m,d,vkom);
class SQLCURSOR cur;  
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
if(kolstr == 0)
 {
lll:;
  prosprkr(d,m,g,nomdok,0);
  
  sapprus(d,m,g,dao,mao,gao,nomdok,nomao,vkom,kontr,sumad,isnds,sum,mpr);
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if( K == FK3 || K == SFK3 || K == ENTER)
     goto lll;
  if(K == KEY_RESIZE)
   goto naz1;
  goto vper;
 }
else
 {
  /*Суммируем выполненные проводки*/
naz2:;
  sum=0.;
  cur.poz_cursor(0);
  while(cur.read_cursor(&row) != 0)
   {
    if(provprusl(row,mpr) != 0)
      continue;
    sum+=atof(row[4])+atof(row[5]);
   }
 }

naz:;
GDITE();
clear();

prosprkr(d,m,g,nomdok,0);

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
/*  
  printw("i=%d %s %s %s %s %s %s %s %s\n",
  i,row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7]);
*/  
  if(provprusl(row,mpr) != 0)
   continue;

  rsdat(&d1,&m1,&g1,row[0],2);
    
  deb=atof(row[4]);
  kre=atof(row[5]);
  vremm[i]=atoi(row[6]);
  ktoz[i]=atoi(row[8]);
  val[i]=atoi(row[9]);
  
  sprintf(strsql,"%02d.%02d.%d|%-*s|%-*s|%-*s|%8.2f|%8.2f|%1s|%s",
  d1,m1,g1,
  iceb_tu_kolbait(5,row[1]),row[1],
  iceb_tu_kolbait(5,row[2]),row[2],
  iceb_tu_kolbait(5,row[3]),row[3],
  deb,kre,row[10],row[7]);

  i++;
  spis_menu.spis_str.plus(strsql);
  if(pozz1 == 1)
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

if(i <= kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

sapprus(d,m,g,dao,mao,gao,nomdok,nomao,vkom,kontr,sumad,isnds,sum,mpr);

i1=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,i1,&K,"",COLS-26);


if(kom == -7) //Изменение размеров экрана
 goto naz1;
 
if(kom == -3) /*Стрелка вниз*/
 {
  if(i < kls)
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
    if(provprusl(row,mpr) != 0)
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

    if(provprusl(row,mpr) != 0)
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

    if(provprusl(row,mpr) != 0)
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
   goto naz;

  polen(spis_menu.spis_str.ravno(kom),&datp,1,'|');
  polen(spis_menu.spis_str.ravno(kom),&shet,2,'|');
  polen(spis_menu.spis_str.ravno(kom),&shetk,3,'|');

  polen(spis_menu.spis_str.ravno(kom),&kor,4,'|');

  polen(spis_menu.spis_str.ravno(kom),&deb,5,'|');
  deb=okrug(deb,0.01);

  polen(spis_menu.spis_str.ravno(kom),&kre,6,'|');
  kre=okrug(kre,0.01);

  polen(spis_menu.spis_str.ravno(kom),&komen,8,'|');
  polen(spis_menu.spis_str.ravno(kom),&kekv,7,'|');
  
  vrem=vremm[kom];
  ktozap=ktoz[kom];
 }

vper:;

switch (K)
 {

  case FK1:   /*Получить подсказку*/
   GDITE();

   iceb_t_pdoc("ukr2_2_1.txt");

   clear();
   goto naz1;
     
  case ENTER:
  case SFK2:   /*Корректировать запись*/
    
    if(iceb_t_pbpds(m,g) != 0)
     goto naz;

    rsdat(&d1,&m1,&g1,datp.ravno(),0);
    if(iceb_t_pvglkni(m1,g1) != 0)
     goto naz;

    VV.VVOD_delete();
    sozvprov1(&VV);
    
    VV.data_plus(0,shet.ravno());
    VV.data_plus(1,shetk.ravno());
    sprintf(strsql,"%.2f",deb);
    VV.data_plus(2,strsql);
    if(deb == 0.)
     {
      VV.data_plus(0,shetk.ravno());
      VV.data_plus(1,shet.ravno());
      sprintf(strsql,"%.2f",kre);
      VV.data_plus(2,strsql);
     }
    
    dpr.new_plus(datp.ravno());

    VV.data_plus(4,komen.ravno());
    VV.data_plus(5,kekv.ravno());

    vprov1(&VV,metkasys,0,&dpr,nomdok,vkom,0,d,m,g,1,vrem,kor.ravno(),ktozap,8,0,val[kom]);
    goto naz1;

  case PLU:  /*Ввести запись*/
  case FK2:  /*Ввести запись*/
    if(iceb_t_pbpds(m,g) != 0)
       goto naz1;
    VV.VVOD_delete();
    sozvprov1(&VV);
    vprov1(&VV,metkasys,0,&dpr,nomdok,vkom,0,d,m,g,0,0L,"\0",0,8,0,0);
    goto naz1;

  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      if(iceb_t_pbpds(m,g) != 0)
       goto naz;

      rsdat(&d1,&m1,&g1,datp.ravno(),0);
      if(iceb_t_pvglkni(m1,g1) != 0)
       goto naz;
       

      udprov(val[kom],g1,m1,d1,shet.ravno(),shetk.ravno(),vrem,deb,kre,komen.ravno_filtr(),2);
      goto naz1;      
     }
    clear();
    goto naz;

  case SFK3:       /*Удалить Все проводки*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить все проводки ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      if(iceb_t_pbpds(m,g) != 0)
       goto naz;
       
      udprgr(metkasys,d,m,g,nomdok,0,0);
      goto naz1;
     }
    goto naz;

  case FK4:  /*Автоматические проводки*/
    avtprukr(d,m,g,nomdok);
    goto naz1;

  case FK5: /*Настройка автоматических проводок*/
      iceb_redfil("avtprukr.alx",0);
      clear();
      goto naz1;

  case SFK5: //Распечатка проводок
    raspprov(&cur,d,m,g,nomdok,mpr,7);
    goto naz;
    

  case FK7:  /*Переключить режим просмотра*/
    mpr++;
    if(mpr == 3)
     mpr=0;
    goto naz2;  

  case FK8:  /*Список контрагентов*/
     clear();
     naim.new_plus("");
     dirkontr(&kod,&naim,1);
     clear();
     if(kolstr > 0)
       goto naz;  
     else
       goto lll;

  case ESC : /*Выход*/
     return(0);
       
  case FK10:  /*Выйти из накладной*/
    prosprkr(d,m,g,nomdok,1);
    return(1);

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
void sapprus(short d,short m,short g,
short dao,short mao,short gao,
const char *nomdok,
const char *nomao,
const char *vkom,
const char *kontr,double sumad,double isnds,double sum,short mpr)
{
double		sumasnds;

move(0,0);
printw("%s (%s)",gettext("Ввод и корректировка проводок"),
ICEB_MP_UKR);

if(mpr == 0)
 {
  move(0,COLS-40);
  printw("        ");
 }

if(mpr == 1)
 {
  move(0,COLS-40);
  attron(A_BLINK); /*Влючение мерцания*/
  printw(gettext("Дебеты "));
  attroff(A_BLINK); /*Выключение мерцания*/
 }
if(mpr == 2)
 {
  move(0,COLS-40);
  attron(A_BLINK); /*Влючение мерцания*/
  printw(gettext("Кредиты"));
  attroff(A_BLINK); /*Выключение мерцания*/
 }


move(1,0);
printw("%s:%s/%s %s:%d.%d.%d%s/%d.%d.%d%s",
gettext("Номер документа"),
nomdok,nomao,
gettext("Дата"),
d,m,g,
gettext("г."),
dao,mao,gao,
gettext("г."));

move(2,0);
printw(gettext("Код операции: %s Код контрагента: %s"),vkom,kontr);

//sumasnds=sumad+sumad*nds1/100.;
sumasnds=sumad+isnds;
move(3,0);
printw("%s:%.2f+%.2f=%.2f",gettext("Сумма по документу"),
sumad,isnds,sumasnds);

move(4,0);
printw(gettext("Сумма сделанных проводок:%s"),prnbr(sum));

helstr(LINES-1,0,
"F1",gettext("помощь"),
"F2+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("авт.пр."),
"F5",gettext("нас.пров."),
//"F6",gettext("таблица"),
"F7",gettext("пер."),
"F8",gettext("кон-ты"),
"F10",gettext("выход"),NULL);
}
/**************************/
/*Проверка показа проводок*/
/**************************/

int		provprusl(SQL_str row,short mpr)
{

if(mpr == 1 && atof(row[5]) != 0.)
  return(1);

if(mpr == 2 && atof(row[4]) != 0.)
  return(2);

return(0);
}
