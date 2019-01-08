/* $Id: dirprkas.c,v 5.39 2013/11/05 10:50:48 sasa Exp $ */
/*13.07.2015    05.10.2000      Белых А.И.      dirprkas.c
Ввод и просмотр проводок по определенному документу для учета 
кассовых ордеров
Если вернули 0 - завершить работу с документом
	     1 - нет
*/
#include        "buhg.h"

void sapprus(short d,short m,short g,const char *nomd,const char *kodop,const char *shetd,const char *shetk,
double sum,short mpr,double sumd,double sumpod,double sumd_gr,double sumpod_gr,const char *kod_val);
int provblokkas(short,short,short,short);


int dirprkas(short d,short m,short g, //Дата документа
const char *nomd, //Номер документа
const char *kassa, //Номер кассы
const char *kodop, //Код операции
int tipz,     //1-приход 2-расход
const char *shetd) //Счёт документа
{
short           d1,m1,g1;
int		ktozap=0;
time_t		vrem=0;
short           x=0,y=4;
int             K;
short           kom;
int             i,i1;
class iceb_tu_str sh("");
class iceb_tu_str shk("");
class iceb_tu_str komen("");
double          sum;
static class iceb_tu_str dpr(""); /*Дата проводки*/
double		deb=0.,kre=0.;
class iceb_tu_str datp("");
short		mpr;
class iceb_tu_str kor("");
class iceb_tu_str kod("");
class iceb_tu_str nm("");
long		kolstr;
SQL_str         row;
char		strsql[1024];
long		pozz,pozz1;
short		mspz; /*метка соответствия первой записи*/
class iceb_tu_str shetk(""); //Счёт кассы
double		sumd=0.,sumpod=0.;
double		sumd_gr=0.,sumpod_gr=0.;
class iceb_tu_str metkasys("");
class iceb_t_mstr spis_menu(0);
class iceb_tu_str kekv_char("");
SQLCURSOR curr;

GDITE();
VVOD VV(0);
//Создаем класс меню
VVOD DANET(1);

mpr=1;
kom=0;
metkasys.new_plus(ICEB_MP_KASA);
pozz=0;
//Читаем счёт кассы в коде операции если он там есть

if(tipz == 1)
 sprintf(strsql,"select shetkas from Kasop1 where kod='%s'",kodop);
if(tipz == 2)
 sprintf(strsql,"select shetkas from Kasop2 where kod='%s'",kodop);

if(sql_readkey(&bd,strsql,&row,&curr) == 1)
 {
  shetk.new_plus(row[0]);
 }

class iceb_tu_str kod_val("");
//Читаем счёт кассы
sprintf(strsql,"select shet,kv from Kas where kod=%s",kassa);
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найдена касса"),kassa);
  iceb_t_soob(strsql);
  return(0);
 }
else
 {
  kod_val.new_plus(row[1]);
  if(shetk.ravno()[0] == '\0')
   shetk.new_plus(row[0]);
 }  

if(shetk.ravno()[0] == '\0')
 {
  sprintf(strsql,gettext("Не введён счёт для кассы %s !"),kassa);
  iceb_t_soob(strsql);
  return(0);
 }

sumkasor(kassa,tipz,nomd,g,&sumd,&sumpod);
if(kod_val.getdlinna() > 1)
  sumkasorue(kassa,tipz,nomd,g,&sumd_gr,&sumpod_gr); /*в гривне eсли касса валютная*/
/*
printw("sumd=%f sumpod=%f\n",sumd,sumpod);
OSTANOV();
*/

naz1:;
clear();
GDITE();
short           kls=LINES-7;
time_t		vremm[kls];
int		ktoz[kls];
int		val[kls];

sum=0;

K=0;

sprintf(strsql,"select datp,sh,shk,kodkon,deb,kre,vrem,komen,ktoi,val,kekv \
from Prov where kto='%s' and pod=%s and nomd='%s' and oper='%s' and \
datd='%04d-%02d-%02d' and tz=%d order by datp asc",
metkasys.ravno(),kassa,nomd,kodop,g,m,d,tipz);

SQLCURSOR cur;  
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
lll:;
  sapprus(d,m,g,nomd,kodop,shetd,shetk.ravno(),sum,mpr,sumd,sumpod,sumd_gr,sumpod_gr,kod_val.ravno());

  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if( K == FK3 || K == SFK3 || K == ENTER || K == SFK5)
     goto lll;
  if(K == KEY_RESIZE)
    goto naz1;
    
  goto vper;
 }
else
 {
  /*Суммиреум выполненные проводки*/
  sum=0.;
  while(cur.read_cursor(&row) != 0)
   {
    sum+=atof(row[4]);
   }
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
for(i=0; i < kls ;)
 {
  if(cur.read_cursor(&row) == 0)
   break;

  pozz1++;

  rsdat(&d1,&m1,&g1,row[0],2);
    
  deb=atof(row[4]);
  kre=atof(row[5]);

  if(mpr == 1 && kre != 0.)
    continue;

  if(mpr == 2 && deb != 0.)
    continue;

  vremm[i]=atoi(row[6]);
  ktoz[i]=atoi(row[8]);
  val[i]=atoi(row[9]);  
  kekv_char.new_plus("");
  if(atoi(row[10]) != 0)
   kekv_char.new_plus(row[10]);
   
  sprintf(strsql,"%02d.%02d.%d|%-*s|%-*s|%-*s|%8.2f|%8.2f|%1s|%s",
  d1,m1,g1,
  iceb_tu_kolbait(5,row[1]),row[1],
  iceb_tu_kolbait(5,row[2]),row[2],
  iceb_tu_kolbait(5,row[3]),row[3],
  deb,kre,kekv_char.ravno(),row[7]);

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

sapprus(d,m,g,nomd,kodop,shetd,shetk.ravno(),sum,mpr,sumd,sumpod,sumd_gr,sumpod_gr,kod_val.ravno());

i1=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,i1,&K,"",0);


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

    deb=atof(row[4]);
    kre=atof(row[5]);

    if(mpr == 1 && kre != 0.)
      continue;

    if(mpr == 2 && deb != 0.)
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

    deb=atof(row[4]);
    kre=atof(row[5]);

    if(mpr == 1 && kre != 0.)
      continue;

    if(mpr == 2 && deb != 0.)
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

    deb=atof(row[4]);
    kre=atof(row[5]);

    if(mpr == 1 && kre != 0.)
      continue;

    if(mpr == 2 && deb != 0.)
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
  polen(spis_menu.spis_str.ravno(kom),&sh,2,'|');
  polen(spis_menu.spis_str.ravno(kom),&shk,3,'|');

  polen(spis_menu.spis_str.ravno(kom),&kor,4,'|');

  polen(spis_menu.spis_str.ravno(kom),&deb,5,'|');
  deb=okrug(deb,0.01);

  polen(spis_menu.spis_str.ravno(kom),&kre,6,'|');
  kre=okrug(kre,0.01);

  polen(spis_menu.spis_str.ravno(kom),&komen,8,'|');
  polen(spis_menu.spis_str.ravno(kom),&kekv_char,7,'|');
  vrem=vremm[kom];
  ktozap=ktoz[kom];
  
 }

vper:;

switch (K)
 {


  case FK1:   /*Получить подсказку*/
   GDITE();

   iceb_t_pdoc("kas2_1_1_1.txt");

   clear();
   goto naz1;
     
  case ENTER:
  case SFK2:   /*Корректировать запись*/
    rsdat(&d1,&m1,&g1,datp.ravno(),0);
    if(provblokkas(m1,g1,m,g) != 0)
      goto naz;

    VV.VVOD_delete();
    sozvprov1(&VV);
    VV.VVOD_SPISOK_zapis_data(0,sh.ravno());
    VV.VVOD_SPISOK_zapis_data(1,shk.ravno());
    sprintf(strsql,"%.2f",deb);
    VV.VVOD_SPISOK_zapis_data(2,strsql);
    if(deb == 0.)
     {
      VV.VVOD_SPISOK_zapis_data(0,shk.ravno());
      VV.VVOD_SPISOK_zapis_data(1,sh.ravno());
      sprintf(strsql,"%.2f",kre);
      VV.VVOD_SPISOK_zapis_data(2,strsql);
     }
    dpr.new_plus(datp.ravno());

    VV.VVOD_SPISOK_zapis_data(4,komen.ravno());
    VV.VVOD_SPISOK_zapis_data(5,kekv_char.ravno());

    vprov1(&VV,metkasys.ravno(),0,&dpr,nomd,kodop,atoi(kassa),d,m,g,1,vrem,kor.ravno(),ktozap,7,tipz,val[kom]);
    goto naz1;

  case PLU:  /*Ввести запись*/
  case FK2:  /*Ввести запись*/
    if(provblokkas(0,0,m,g) != 0)
      goto naz1;
    VV.VVOD_delete();
    sozvprov1(&VV);
    vprov1(&VV,metkasys.ravno(),0,&dpr,nomd,kodop,atoi(kassa),d,m,g,0,0L,"\0",0,7,tipz,0);
    goto naz1;

  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      if(iceb_t_pbpds(m,g) != 0)
          goto naz;
      rsdat(&d1,&m1,&g1,datp.ravno(),0);
      if(provblokkas(m1,g1,m,g) != 0)
          goto naz;

      rsdat(&d1,&m1,&g1,datp.ravno(),0);
      udprov(val[kom],g1,m1,d1,sh.ravno(),shk.ravno(),vrem,deb,kre,komen.ravno_filtr(),2);
      goto naz1;      
     }
    clear();
    goto naz;

  case SFK3:       /*Удалить Все проводки*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить все проводки ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      rsdat(&d1,&m1,&g1,datp.ravno(),0);
      if(provblokkas(m1,g1,m,g) != 0)
        goto naz;
      
      udprgr(metkasys.ravno(),d,m,g,nomd,atoi(kassa),tipz);
       
      goto naz1;
     }
    goto naz;

  case FK4:  /*Автоматические проводки*/
    rsdat(&d1,&m1,&g1,datp.ravno(),0);
    if(provblokkas(m1,g1,m,g) != 0)
      goto naz;


    avtprkas(kassa,tipz,nomd,d,m,g,shetk.ravno(),shetd,kodop);

    goto naz1;

  case FK5: /*Настройка автоматических проводок*/
      attroff(A_UNDERLINE);  /*Выключить подчеркивание*/
      attroff(A_REVERSE);  /*Выключить инверсию*/

      if(tipz == 1)      
        sprintf(strsql,"%s","avtprokasp.alx");
      if(tipz == 2)      
        sprintf(strsql,"%s","avtprokasr.alx");
      iceb_redfil(strsql,0);

      clear();
      goto naz1;

  case SFK5: /*Настройка автоматических проводок*/
    raspprov(&cur,d,m,g,nomd,mpr,5);
    goto naz;
    
  case FK7:  /*Переключить режим просмотра*/
    mpr++;
    if(mpr == 3)
     mpr=0;
    goto naz;  

  case FK8:  /*Список контрагентов*/
     clear();
     nm.new_plus("");
     dirkontr(&kod,&nm,1);
     clear();
     if(kolstr > 0)
       goto naz;  
     else
       goto lll;

  case ESC : /*Выход*/
     provprkas(kassa,tipz,nomd,d,m,g,kodop);
     return(1);
       
  case FK10:  /*Выйти из накладной*/
    provprkas(kassa,tipz,nomd,d,m,g,kodop);
    return(0);

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
void sapprus(short d,short m,short g,const char *nomd,const char *kodop,const char *shetd,const char *shetk,
double sum,short mpr,double sumd,double sumpod,double sumd_gr,double sumpod_gr,const char *kod_val)
{

move(0,0);
printw("%s (%s)",gettext("Ввод и корректировка проводок"),
ICEB_MP_KASA);

if(mpr == 0)
 {
  move(0,COLS-25);
  printw("        ");
 }

if(mpr == 1)
 {
  move(0,COLS-25);
  attron(A_BLINK); /*Влючение мерцания*/
  printw(gettext("Дебеты "));
  attroff(A_BLINK); /*Выключение мерцания*/
 }
if(mpr == 2)
 {
  move(0,COLS-25);
  attron(A_BLINK); /*Влючение мерцания*/
  printw(gettext("Кредиты"));
  attroff(A_BLINK); /*Выключение мерцания*/
 }

move(1,0);
printw(gettext("Номер документа: %s Дата: %d.%d.%dг."),nomd,d,m,g);
move(2,0);
printw("%s: %s",gettext("Код операции"),kodop);
printw(" %s: %s => %s",gettext("Счёт"),shetk,shetd);

move(3,0);
printw(gettext("Сумма сделанных проводок: %s"),prnbr(sum));
printw(" %s(%.2f/%.2f)",kod_val,sumd,sumpod);
if(sumd_gr != 0.)
 printw(" %s(%.2f/%.2f)",gettext("грн."),sumd_gr,sumpod_gr);
 
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
/************************/
/*Проверка блокировки дат*/
/************************/
int provblokkas(short m,short g,short md,short gd)
{

if(iceb_t_pbpds(md,gd) != 0)
 return(1);

if(m != 0)
 {
  if(iceb_t_pvglkni(m,g) != 0)
   return(1);
 }
return(0);
}
