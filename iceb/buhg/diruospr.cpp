/* $Id: diruospr.c,v 5.38 2013/11/05 10:50:48 sasa Exp $ */
/*13.09.2018    02.02.1994      Белых А.И.      diruospr.c
Ввод и просмотр проводок по определенному документа
Если вернули 0 - завершить работу с документом
	     1 - нет
*/
#include <errno.h>
#include        "buhg.h"

void sapprmu1(short,short,short,const char*,const char*,const char*,double,short,int,class SPISOK*,class masiv_din_double*,class masiv_din_double*);
void avtprouos(int tipz,short dd,short md,short gd,const char *nomdok,FILE *ff_prot);

short           *dp,*mp,*gp; /*Даты последнего подтверждения*/
double          *snn; /*Суммы по накладных*/
extern double          *sn; /*Суммы по карточкам*/
extern double          *sp; /*Суммы в проводках*/
extern char            *spsh; /*Список счетов*/

int diruospr(short d,short m,short g, //Дата
const char *nn) //Номер документа
{
int		skk;
class iceb_tu_str kop(""); /*Код операции*/
class iceb_tu_str ko(""); /*Код контрагента*/
int		tp;
int		ktozap=0;
time_t		vrem=0;
short           x=0,y=5;
int             K;
int             kom;
int             i,i1;
class iceb_tu_str sh(""),shk("");
class iceb_tu_str komen("");
double          sum=0.;
static class iceb_tu_str dpr(""); /*Дата проводки*/
double		deb=0.,kre=0.;
class iceb_tu_str datp("");
short		mpr=1;
class iceb_tu_str kor("");
long		kolstr;
SQL_str         row,row1;
char		strsql[1024];
long		pozz,pozz1;
short		mspz;
short		d1,m1,g1;
class iceb_tu_str metkasys("");
class iceb_tu_str nn1("");
FILE *ff_prot=NULL;
char imaf_prot[64];
SQLCURSOR cur;  
class iceb_tu_str kekv_char("");
class iceb_t_mstr spis_menu(0);

sprintf(imaf_prot,"avpuos%d.lst",getpid());

/*******
printw("\ndiruospr- %d.%d.%d %s\n",d,m,g,nn);
refresh();
********/
VVOD VV(0);
//Создаем класс меню
VVOD DANET(1);

pozz=kom=0;
metkasys.new_plus(ICEB_MP_UOS);

/*Читаем шапку документа*/
sprintf(strsql,"select tipz,kontr,kodop,podr,nomdv from Uosdok where \
datd='%04d-%d-%d' and nomd='%s'",g,m,d,nn);

if(sql_readkey(&bd,strsql,&row1,&cur) != 1)
 {
  VVOD SOOB(1);
  sprintf(strsql,"diruospr-%s %s %d.%d.%d%s",
  gettext("Не найден документ !"),nn,d,m,g,
  gettext("г."));
  SOOB.VVOD_SPISOK_add_MD(strsql);
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return(1);
 }
else
 {
  tp=atoi(row1[0]);
  ko.new_plus(row1[1]);
  kop.new_plus(row1[2]);
  skk=atoi(row1[3]);
  nn1.new_plus(row1[4]);
 }  

/*Список необходимых проводок*/
class SPISOK spshet;
class masiv_din_double sumsh;
class masiv_din_double sumshp;
prpuos(d,m,g,nn,skk,&spshet,&sumsh,&sumshp,1);

naz1:;

short           kls=LINES-7;
time_t		vremm[kls];
int		ktoz[kls];
int		val[kls];

K=0;
sum=0.;

sprintf(strsql,"select datp,sh,shk,kodkon,deb,kre,vrem,komen,ktoi,val,kekv \
from Prov where datd='%04d-%02d-%02d' and kto='%s' and pod=%d and \
nomd='%s' and tz=%d order by datp asc",
g,m,d,metkasys.ravno(),skk,nn,tp);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
lll:;
  sapprmu1(d,m,g,nn,kop.ravno(),ko.ravno(),sum,mpr,skk,&spshet,&sumsh,&sumshp);

  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == FK7 || K == FK3 || K == ENTER || K == SFK5)
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

if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;

cur.poz_cursor(pozz);
mspz=2;
pozz1=0;
spis_menu.spis_str.free_class();
for(i=0;i<=kls-1;)
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


sapprmu1(d,m,g,nn,kop.ravno(),ko.ravno(),sum,mpr,skk,&spshet,&sumsh,&sumshp);

i1=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,i1,&K,"",COLS-26);


if(kom == -7) //Изменение размера экрана
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
    if(pozz >= kolstr)
     {
      pozz=kolstr-1;
      break;
     }

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
  class iceb_tu_str str_vib(spis_menu.spis_str.ravno(kom));
  if(SRAV(gettext("Конец списка"),str_vib.ravno(),0) == 0)
     goto naz;

  if(pozz+kom >= kolstr)
   goto naz; /*Стали на конец списка*/

  polen(str_vib.ravno(),&datp,1,'|');
  polen(str_vib.ravno(),&sh,2,'|');
  polen(str_vib.ravno(),&shk,3,'|');

  polen(str_vib.ravno(),&kor,4,'|');

  polen(str_vib.ravno(),&deb,5,'|');
  deb=okrug(deb,0.01);

  polen(str_vib.ravno(),&kre,6,'|');
  kre=okrug(kre,0.01);

  polen(str_vib.ravno(),&komen,8,'|');
  polen(str_vib.ravno(),&kekv_char,7,'|');
  vrem=vremm[kom];
  ktozap=ktoz[kom];
 }

vper:;

switch (K)
 {
  case ESC : /*Выход*/
     prpuos(d,m,g,nn,skk,&spshet,&sumsh,&sumshp,0);
     unlink(imaf_prot);
     return(0);

  case FK10:  /*Выйти из накладной*/
     prpuos(d,m,g,nn,skk,&spshet,&sumsh,&sumshp,0);
     unlink(imaf_prot);
     return(1);

  case FK1:   /*Подсказка*/
     GDITE();
     iceb_t_pdoc("uos2_1_3_1.txt");

   if(kolstr > 0)
     goto naz;
   else
     goto lll;
   
  case ENTER:
  case SFK2:   /*Корректировать запись*/
    if(iceb_t_pbpds(m,g) != 0)
     goto naz;

    rsdat(&d1,&m1,&g1,datp.ravno(),0);
    if(iceb_t_pvglkni(m1,g1) != 0)
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

    vprov1(&VV,metkasys.ravno(),1,&dpr,nn,kop.ravno(),skk,d,m,g,1,vrem,kor.ravno(),ktozap,3,tp,val[kom]);
    goto naz1;

  case PLU:  /*Ввести запись*/
  case FK2:  /*Ввести запись*/
    if(iceb_t_pbpds(m1,g1) != 0)
      goto naz;
    VV.VVOD_delete();
    sozvprov1(&VV);
    if(tp == 1)
     {
      sprintf(strsql,"N%s",nn1.ravno());
      VV.VVOD_SPISOK_zapis_data(4,strsql);
     }
    vprov1(&VV,metkasys.ravno(),1,&dpr,nn,kop.ravno(),skk,d,m,g,0,0L,"",0,3,tp,0);
    goto naz1;

  case FK3:       /*Удалить запись*/
    if(iceb_t_pbpds(m,g) != 0)
     goto naz;

    rsdat(&d1,&m1,&g1,datp.ravno(),0);
    if(iceb_t_pvglkni(m1,g1) != 0)
     goto naz;

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      udprov(val[kom],g1,m1,d1,sh.ravno(),shk.ravno(),vrem,deb,kre,komen.ravno_filtr(),2);
      goto naz1;      
     }
    clear();
    goto naz;

  case SFK3:       /*Удалить Все проводки*/
    if(iceb_t_pbpds(m,g) != 0)
     goto naz;

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить все проводки ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {

      udprgr(metkasys.ravno(),d,m,g,nn,skk,tp);
      goto naz1;
     }
    goto naz;

  case FK4:  /*Автоматические проводки*/
    GDITE();
    if(iceb_t_pbpds(m,g) != 0)
      goto naz;
    if((ff_prot = fopen(imaf_prot,"w")) == NULL)
     {
      error_op_nfil(imaf_prot,errno,0);
      ff_prot=NULL;
     }

    avtprouos(tp,d,m,g,nn,ff_prot);

    if(ff_prot != NULL)
      fclose(ff_prot);
    goto naz1;

  case SFK4:  /*Протокол расчёта проводок*/
     GDITE();
     prosf(imaf_prot);
     
     if(kolstr > 0)
       goto naz;  
     else
       goto lll;

  case FK5: /*Настройка автоматических проводок*/
      attroff(A_UNDERLINE);  /*Выключить подчеркивание*/
      attroff(A_REVERSE);  /*Выключить инверсию*/
      menuvnp(3,tp);
      goto naz1;

  case SFK5:  //Распечатка проводок
    raspprov(&cur,d,m,g,nn,mpr,3);
    goto naz;
    
  case FK7:  /*Переключить режим просмотра*/
    mpr++;
    if(mpr == 3)
     mpr=0;
    pozz=0;
    goto naz;  

  case FK9:  /*работа со списком оплат*/
    diropld(3,d,m,g,nn,0,skopluos(d,m,g,nn));
    goto naz1;  

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
void sapprmu1(short d,short m,short g, //Дата
const char *nn, //Номер документа
const char *kop, //Код операции
const char *ko, //Код контрагента
double sum, //Сумма проводок
short mpr, //Метка просмотра
int skk, //Код подразделения
class SPISOK *spshet,
class masiv_din_double *sumsh,
class masiv_din_double *sumshp)
{

clear();
prpuos(d,m,g,nn,skk,spshet,sumsh,sumshp,2);
move(0,0);

printw(gettext("Ввод и корректировка проводок"));
#if 0
################################################33
if(mpr == 0)
 {
  move(0,COLS-25);
  printw(gettext("Все проводки"));
 }

if(mpr == 1)
 {
  move(0,COLS-25);
  attron(A_BLINK); /*Влючение мерцания*/
  printw(gettext("Дебеты"));
  attroff(A_BLINK); /*Выключение мерцания*/
 }
if(mpr == 2)
 {
  move(0,COLS-25);
  attron(A_BLINK); /*Влючение мерцания*/
  printw(gettext("Кредити"));
  attroff(A_BLINK); /*Выключение мерцания*/
 }
##################################################
#endif
move(1,0);
printw(gettext("Номер документа: %s Дата: %d.%d.%dг."),nn,d,m,g);

move(2,0);
printw(gettext("Код операции: %s Код контрагента: %s"),kop,ko);

move(3,0);
printw(gettext("Сумма сделанных проводок: %s"),prnbr(sum));

move(4,0);
if(mpr == 0)
 printw("%s",gettext("Показаны все проводки"));
if(mpr == 1)
 printw("%s",gettext("Показаны только дебетовые проводки"));
if(mpr == 2)
 printw("%s",gettext("Показаны только кредитовые проводки"));

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("авт.пр."),
"F5",gettext("нас.пров."),
"F7",gettext("пер."),
"F10",gettext("выход"),NULL);
}
