/* $Id: dirprzar.c,v 5.46 2013/11/05 10:50:48 sasa Exp $ */
/*13.07.2015	05.07.2000	Белых А.И.	dirprzar.c
Просмотр и корректировка проводок в зарплате по конкретному 
человеку
*/
#include <math.h>
#include        <errno.h>
#include        "buhg.h"

void sapprmu(long tabn,short m,short g,double sum,short mpr,const char *fam,short metpoi,const char *shetp,const char *shetkp,const char *kontrp,const char *koment,const char *debp,const char *krep,const char *dat1,const char *dat2);

int provprzarp(SQL_str row,short mpr,short metpoi,const char *shetp,const char *shetkp,const char *kontrp,const char *koment,const char *debp,const char *krep,const char *dat1,const char *dat2);



void dirprzar(int tabn,short mr,short gr,const char *fam)
{
int		ktozap=0;
short           x=0,y=4;
int             K;
int             kom;
int             i,i1;
long		kolstr;
SQL_str         row;
char		strsql[1024];
long		pozz,pozz1;
short		mspz; /*метка соответствия первой записи*/
short		mpr;
class iceb_tu_str kod("");
class iceb_tu_str nm("");
class iceb_tu_str nomdok("");
int		podr=0;
class iceb_tu_str kop(""); /*Код операции*/
class iceb_tu_str datp("");
class iceb_tu_str dpr("");
time_t		vrem=0;
class iceb_tu_str komen("");
class iceb_tu_str kor("");
class iceb_tu_str sh("");
class iceb_tu_str shk("");
double		deb=0.,kre=0.;
short		d1,m1,g1;
double		sum;
short		dr;
class iceb_tu_str shetp("");
class iceb_tu_str shetkp("");
class iceb_tu_str debp(""),krep("");
class iceb_tu_str kontrp("");
class iceb_tu_str koment("");
class iceb_tu_str dat1(""),dat2("");
short		metpoi;
class iceb_tu_str metkasys("");
char imaf_prot[64];
class iceb_tu_str kekv_char("");
class iceb_t_mstr spis_menu(0);
    
sprintf(imaf_prot,"zarpr%d.lst",getpid());

GDITE();


mpr=1;
metpoi=kom=0;
pozz=0;
VVOD VV(0);
//Создаем класс меню
VVOD DANET(1);

//sprintf(nomdok,"%d-%d",mr,tabn);
nomdok.new_plus(mr);
nomdok.plus("-");
nomdok.plus(tabn);
metkasys.new_plus(ICEB_MP_ZARP);

//Код операции в зарплате не назначается

dr=1;
dpm(&dr,&mr,&gr,5);

naz1:;
clear();
GDITE();
short           kls=LINES-6;
time_t		vremm[kls];
int		ktoz[kls];
int		val[kls];

sum=0;

K=0;
sprintf(strsql,"select datp,sh,shk,kodkon,deb,kre,vrem,komen,ktoi,val,kekv \
from Prov where kto='%s' and pod=%d and nomd='%s' and \
datd >= '%04d-%02d-1' and datd <= '%04d-%02d-%d' order by datp asc",
metkasys.ravno(),podr,nomdok.ravno(),gr,mr,gr,mr,dr);
SQLCURSOR cur;  
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

if(kolstr == 0)
 {
lll:;
  sapprmu(tabn,mr,gr,sum,mpr,fam,metpoi,shetp.ravno(),shetkp.ravno(),kontrp.ravno(),koment.ravno(),debp.ravno(),
  krep.ravno(),dat1.ravno(),dat2.ravno());

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
    if(provprzarp(row,mpr,metpoi,shetp.ravno(),shetkp.ravno(),kontrp.ravno(),koment.ravno(),debp.ravno(),
    krep.ravno(),dat1.ravno(),dat2.ravno()) != 0)
      continue;
    sum+=(atof(row[4])+atof(row[5]));
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
/*  
  printw("i=%d %s %s %s %s %s %s %s %s\n",
  i,row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7]);
*/  

  if(provprzarp(row,mpr,metpoi,shetp.ravno(),shetkp.ravno(),kontrp.ravno(),koment.ravno(),debp.ravno(),
  krep.ravno(),dat1.ravno(),dat2.ravno()) != 0)
    continue;

  rsdat(&d1,&m1,&g1,row[0],2);
  deb=atof(row[4]);
  kre=atof(row[5]);

  vremm[i]=atoi(row[6]);
  ktoz[i]=atoi(row[8]);
  val[i]=atoi(row[9]);
  kekv_char.new_plus("");
  if(atoi(row[10]) != 0)
   kekv_char.new_plus(row[10]);
   
  sprintf(strsql,"%02d.%02d.%d|%-*s|%-*s|%-*s|%7.2f|%7.2f|%1s|%s",
  d1,m1,g1,
  iceb_tu_kolbait(5,row[1]),row[1],
  iceb_tu_kolbait(5,row[2]),row[2],
  iceb_tu_kolbait(4,row[3]),row[3],
  deb,kre,kekv_char.ravno(),row[7]);

  i++;
  spis_menu.spis_str.plus(strsql);

  if(pozz1 == 1)
    mspz=1;
 }
/*
printw("i=%d kom=%d\n",i,kom);
printw("LL.MD[0]=%s\n",LL.MD[0]);
printw("LL.MD[1]=%s\n",LL.MD[1]);
OSTANOV();
*/
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

if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i <= kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));
/*  sprintf(MM5.MD[i],gettext("Конец списка"));*/

sapprmu(tabn,mr,gr,sum,mpr,fam,metpoi,shetp.ravno(),shetkp.ravno(),kontrp.ravno(),koment.ravno(),debp.ravno(),
krep.ravno(),dat1.ravno(),dat2.ravno());

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

    if(provprzarp(row,mpr,metpoi,shetp.ravno(),shetkp.ravno(),kontrp.ravno(),koment.ravno(),debp.ravno(),
    krep.ravno(),dat1.ravno(),dat2.ravno()) != 0)
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

    if(provprzarp(row,mpr,metpoi,shetp.ravno(),shetkp.ravno(),kontrp.ravno(),koment.ravno(),debp.ravno(),
    krep.ravno(),dat1.ravno(),dat2.ravno()) != 0)
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

    if(provprzarp(row,mpr,metpoi,shetp.ravno(),shetkp.ravno(),kontrp.ravno(),koment.ravno(),debp.ravno(),
    krep.ravno(),dat1.ravno(),dat2.ravno()) != 0)
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

  class iceb_tu_str str_out(spis_menu.spis_str.ravno(kom));
  polen(str_out.ravno(),&datp,1,'|');
  polen(str_out.ravno(),&sh,2,'|');
  polen(str_out.ravno(),&shk,3,'|');

  polen(str_out.ravno(),&kor,4,'|');

  polen(str_out.ravno(),&deb,5,'|');
  deb=okrug(deb,0.01);

  polen(str_out.ravno(),&kre,6,'|');
  kre=okrug(kre,0.01);

  vrem=vremm[kom];
  ktozap=ktoz[kom];
  polen(str_out.ravno(),&komen,8,'|');
  polen(str_out.ravno(),&kekv_char,7,'|');
  
 }

vper:;

switch (K)
 {


  case FK1:   /*Получить подсказку*/
   GDITE();
   iceb_t_pdoc("zarp2_1_1.txt");
   clear();
   goto naz1;
     
  case ENTER:
  case SFK2:   /*Корректировать запись*/

    if(provblokzarp(tabn,mr,gr) != 0) //dirzarp.c
     goto naz;

    rsdat(&d1,&m1,&g1,datp.ravno(),0);
    if(iceb_t_pvglkni(m1,g1) != 0)
     goto naz;

    VV.VVOD_delete();
    sozvprov1(&VV);
    VV.data_plus(0,sh.ravno());    
    VV.data_plus(1,shk.ravno());    
    sprintf(strsql,"%.2f",deb);
    VV.data_plus(2,strsql);    
    //VV.VVOD_SPISOK_add_data(strsql,20);
    if(deb == 0.)
     {
      VV.VVOD_SPISOK_zapis_data(0,shk.ravno());
      VV.VVOD_SPISOK_zapis_data(1,sh.ravno());
      sprintf(strsql,"%.2f",kre);
      VV.VVOD_SPISOK_zapis_data(2,strsql);
     }
    dpr.new_plus(datp.ravno());

    VV.data_plus(4,komen.ravno());    
    VV.data_plus(5,kekv_char.ravno());    

//    sprintf(nomdok,"%d-%d",mr,tabn);

    vprov1(&VV,metkasys.ravno(),0,&dpr,nomdok.ravno(),kop.ravno(),podr,dr,mr,gr,1,vrem,kor.ravno(),ktozap,2,0,val[kom]);
    goto naz1;

  case PLU:  /*Ввести запись*/
  case FK2:  /*Ввести запись*/

    if(provblokzarp(tabn,mr,gr) != 0) //dirzarp.c
     goto naz1;

    rsdat(&d1,&m1,&g1,datp.ravno(),0);
    if(iceb_t_pvglkni(m1,g1) != 0)
     goto naz;

    dpr.new_plus("");
    VV.VVOD_delete();
    sozvprov1(&VV);

    vprov1(&VV,metkasys.ravno(),0,&dpr,nomdok.ravno(),kop.ravno(),podr,dr,mr,gr,0,0L,"\0",0,2,0,0);
    goto naz1;

  case FK3:       /*Удалить запись*/

    if(provblokzarp(tabn,mr,gr) != 0) //dirzarp.c
     goto naz;

    rsdat(&d1,&m1,&g1,datp.ravno(),0);
    if(iceb_t_pvglkni(m1,g1) != 0)
     goto naz;

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {

      rsdat(&d1,&m1,&g1,datp.ravno(),0);
      udprov(val[kom],g1,m1,d1,sh.ravno(),shk.ravno(),vrem,deb,kre,komen.ravno_filtr(),2);
      goto naz1;      
     }
    clear();
    goto naz;

  case SFK3:       /*Удалить Все проводки*/

    if(provblokzarp(tabn,mr,gr) != 0) //dirzarp.c
     goto naz;
      
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить все проводки ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      /*проверка блокировки в подпрограмме*/
      udprgr(metkasys.ravno(),dr,mr,gr,nomdok.ravno(),0,0);
      goto naz1;
     }
    goto naz;

  case SFK4: /*Выполнение автоматических проводок*/

    if(provblokzarp(tabn,mr,gr) != 0) //dirzarp.c
     goto naz;

    rsdat(&d1,&m1,&g1,datp.ravno(),0);
    if(iceb_t_pvglkni(m1,g1) != 0)
     goto naz;


    FILE *ff_prot;
    if((ff_prot = fopen(imaf_prot,"w")) == NULL)
     {
      error_op_nfil(imaf_prot,errno,0);
      ff_prot=NULL;
     }

    zaravpr(tabn,mr,gr,ff_prot);
    if(ff_prot != NULL)
      fclose(ff_prot);
    
    goto naz1;

  case FK4: /*Поиск проводок*/
    VV.VVOD_delete();
    VV.VVOD_SPISOK_add_ZAG(gettext("Введите нужные реквизиты"));
    VV.VVOD_SPISOK_add_MD(gettext("Счёт..........(,,)...."));
    VV.VVOD_SPISOK_add_MD(gettext("Счёт к-нт.....(,,)...."));
    VV.VVOD_SPISOK_add_MD(gettext("Код контрагента..(,,)."));
    VV.VVOD_SPISOK_add_MD(gettext("Коментарий............"));
    VV.VVOD_SPISOK_add_MD(gettext("Дебет................."));
    VV.VVOD_SPISOK_add_MD(gettext("Кредит................"));
    VV.VVOD_SPISOK_add_MD(gettext("Дата начала поиска...."));
    VV.VVOD_SPISOK_add_MD(gettext("Дата конца поиска....."));

    VV.VVOD_SPISOK_add_data(shetp.ravno(),64);
    VV.VVOD_SPISOK_add_data(shetkp.ravno(),64);
    VV.VVOD_SPISOK_add_data(kontrp.ravno(),128);
    VV.VVOD_SPISOK_add_data(koment.ravno(),128);
    VV.VVOD_SPISOK_add_data(debp.ravno(),20);
    VV.VVOD_SPISOK_add_data(krep.ravno(),20);
    VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
    VV.VVOD_SPISOK_add_data(dat2.ravno(),11);


sfk4:;
    metpoi=0;


    helstr(LINES-1,0,"F1",gettext("помощь"),
    "F2/+",gettext("поиск"),
    "F10",gettext("выход"),NULL);

    kom=VV.vvod(0,1,1,-1,-1);

    shetp.new_plus(VV.VVOD_SPISOK_return_data(0));
    shetkp.new_plus(VV.VVOD_SPISOK_return_data(1));
    kontrp.new_plus(VV.VVOD_SPISOK_return_data(2));
    koment.new_plus(VV.VVOD_SPISOK_return_data(3));
    debp.new_plus(VV.VVOD_SPISOK_return_data(4));
    krep.new_plus(VV.VVOD_SPISOK_return_data(5));
    dat1.new_plus(VV.VVOD_SPISOK_return_data(6));
    dat2.new_plus(VV.VVOD_SPISOK_return_data(7));

    switch (kom)
     {
      case FK10:
      case ESC:
        goto naz;
        
      case FK1:
        GDITE();
        iceb_t_pdoc("zarp2_1_1_1.txt");
        clear();
        goto sfk4;

      case FK2:
      case PLU:
         metpoi=1;
         break;

      default:
        goto sfk4;
        break;
     }

//Для того чтобы пересчитала сумму в соответствии с просмотром  
    goto naz1;  

  case FK5: /*Настройка автоматических проводок*/

//      iceb_redfil("zarnpr.alx",0);
      zarnvp();
      clear();
      goto naz1;

  case SFK5: //Распечатка проводок
      sprintf(strsql,"%d %s",tabn,fam);
      raspprov(&cur,1,mr,gr,strsql,mpr,2);
      goto naz;


  case FK7:  /*Переключить режим просмотра*/
  
    mpr++;
    if(mpr == 3)
     mpr=0;
//Для того чтобы пересчитала сумму в соответствии с просмотром  
    goto naz1;

  case FK8:  /*Список контрагентов*/
     clear();
     nm.new_plus("");
     dirkontr(&kod,&nm,1);
     clear();
     if(kolstr > 0)
       goto naz;  
     else
       goto lll;

  case FK9:  //Просмотр файла протокола расчёта проводок
    GDITE();
    prosf(imaf_prot);
    goto naz;  

  case ESC : /*Выход*/
  case FK10:  /*Выйти*/
    unlink(imaf_prot);
    return;

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
void sapprmu(long tabn,short m,short g,double sum,short mpr,
const char *fam,short metpoi,
const char *shetp,const char *shetkp,const char *kontrp,const char *koment,
const char *debp,const char *krep,const char *dat1,const char *dat2)
{
int		Y,X;

move(0,0);
printw(gettext("Ввод и корректировка проводок"));

#if 0
#########################################################3
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
###################################################
#endif
if(metpoi == 1)
 {
  attron(A_BLINK); /*Влючение мерцания*/
  move(Y=2,X=COLS-20);
  printw(gettext("Поиск:"));
  if(shetp[0] != '\0')
   {
    move(++Y,X);
    printw("%s:%.10s",gettext("Счёт"),shetp);
   }
  if(shetkp[0] != '\0')
   {
    move(++Y,X);
    printw("%s:%.10s",gettext("Счёт к-нт"),shetkp);
   }
  if(kontrp[0] != '\0')
   { 
    move(++Y,X);
    printw("%s:%.10s",gettext("Конт-гент"),kontrp);
   }
  if(koment[0] != '\0')
   {
    move(++Y,X);
    printw("%s:%.10s",gettext("Коментар."),koment);
   }
  if(debp[0] != '\0')
   {
    move(++Y,X);
    printw("%s:%.10s",gettext("Дебет"),debp);
   }
  if(krep[0] != '\0')
   {
    move(++Y,X);
    printw("%s:%.10s",gettext("Кредит"),krep);
   }
  if(dat1[0] != '\0')
   {
    move(++Y,X);
    printw("%s:%.10s",gettext("Дата нач."),dat1);
   }
  if(dat2[0] != '\0')
   {
    move(++Y,X);
    printw("%s:%.10s",gettext("Дата кон."),dat2);
   }
  attroff(A_BLINK); /*Выключение мерцания*/

 }

move(1,0);
printw("%s:%d.%d %d %s",gettext("Дата"),m,g,tabn,fam);

move(2,0);
printw(gettext("Сумма сделанных проводок: %s"),prnbr(sum));

move(3,0);
if(mpr == 0)
 printw("%s",gettext("Показаны все проводки"));
if(mpr == 1)
 printw("%s",gettext("Показаны только дебетовые проводки"));
if(mpr == 2)
 printw("%s",gettext("Показаны только кредитовые проводки"));

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("нас.пров."),
"F7",gettext("пер."),
"F8",gettext("кон-ты"),
"F9",gettext("про."),
"F10",gettext("выход"),NULL);
}

/**********************/
/*Отбор нужных записей*/
/***********************/

int provprzarp(SQL_str row,short mpr,short metpoi,
const char *shetp,const char *shetkp,const char *kontrp,const char *koment,
const char *debp,const char *krep,const char *dat1,const char *dat2)
{
double	deb,kre;
short	d1,m1,g1,d2,m2,g2;
int		i1;

  
deb=atof(row[4]);
kre=atof(row[5]);

if(mpr == 1 && kre != 0.)
  return(1);

if(mpr == 2 && deb != 0.)
  return(2);

if(metpoi == 0)
 return(0);

if(shetp[0] != '\0')
 {
  if(shetp[0] != '-')
   {
    if(pole1(shetp,row[1],',',0,&i1) != 0)
     if(SRAV(shetp,row[1],0) != 0)
      return(3);
   }
  else
   if(pole1(shetp,row[1],',',0,&i1) == 0 ||
    SRAV(shetp,row[1],0) == 0)
     return(3);
 }

if(shetkp[0] != '\0')
 {
  if(shetkp[0] != '-')
   {
    if(pole1(shetkp,row[2],',',0,&i1) != 0)
     if(SRAV(shetkp,row[2],0) != 0)
      return(4);
   }
  else
    if(pole1(shetkp,row[2],',',0,&i1) == 0 ||
     SRAV(shetkp,row[2],0) == 0)
      return(4);
 }
if(kontrp[0] != '\0')
 {
  if(kontrp[0] != '-')
   {
    if(pole1(kontrp,row[3],',',0,&i1) != 0)
     if(SRAV(kontrp,row[3],0) != 0)
      return(5);
   }
  else
    if(pole1(kontrp,row[3],',',0,&i1) == 0 ||
     SRAV(kontrp,row[3],0) == 0)
      return(5);
 }
if(koment[0] != '\0')
 {
  if(strstrm(row[7],koment) == 0)
   return(6);
 }

if(debp[0] != '\0')
 {
  if(fabs(ATOFM(debp)-atof(row[4])) > 0.009)
    return(7);
 }

if(krep[0] != '\0')
 {
  if(fabs(ATOFM(krep)-atof(row[5])) > 0.009)
    return(8);
 }

if(dat1[0] != '\0')
 {
  rsdat(&d1,&m1,&g1,row[0],2);
  rsdat(&d2,&m2,&g2,dat1,1);
 // printw("\n%d.%d.%d %d.%d.%d\n",g2,m2,d2,g1,m1,d1);
  if((i1=SRAV1(g2,m2,d2,g1,m1,d1)) < 0)
   {
//    printw("i1=%d\n",i1);
//    OSTANOV();      
    return(9);
   }
//    printw("*i1=%d\n",i1);
//    OSTANOV();      
 }
if(dat2[0] != '\0')
 {
  rsdat(&d1,&m1,&g1,row[0],2);
  rsdat(&d2,&m2,&g2,dat2,1);
  if(SRAV1(g2,m2,d2,g1,m1,d1) > 0)
   return(10);
 }
return(0);
}
