/* $Id: dirprus.c,v 5.47 2013/11/05 10:50:48 sasa Exp $ */
/*13.09.2018    02.02.1994      Белых А.И.      dirprus.c
Ввод и просмотр проводок по определенному документа для услуг
Если вернули 0 - завершить работу с документом
	     1 - нет
*/
#include <errno.h>
#include        "buhg.h"

int		provprusl(SQL_str,short);
void		sapprus(short,short,short,const char[],const char[],const char[],double,double,short,float);

extern short           *dp,*mp,*gp; /*Даты последнего подтверждения*/
extern double          *snn; /*Суммы по накладных*/
extern class masiv_din_double sumpokart; /*Суммы по карточкам*/
extern double          *sp; /*Суммы в проводках*/
extern char            *spsh; /*Список счетов*/
extern double          *sbnds; /*Суммы без НДС*/
extern double	okrg1;  /*Округление суммы*/

int dirprus(short d,short m,short g, //Дата
const char *nn, //Номер документа
int podr, //Код подразделения
const char *kop, //Код операции
const char *kontr, //Код контрагента
int tp,       //1-приход 2-расход
double sumad, //Сумма по документа
double sumkor, //Величина корректировки суммы документа
int lnds,
const char *vnomd, //Номер встречного документа
float pnds)
{
FILE *ff_prot=NULL;
short           d1,m1,g1;
int		ktozap=0;
time_t		vrem=0;
short           x=0,y=6;
int             K;
short           kom;
int             i,prc;
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
char imaf_prot[64];
class iceb_tu_str metkasys("");
class iceb_tu_str kekv_char("");
class iceb_t_mstr spis_menu(0);
int metka_avp=1; /*метка какой файл брать для автоматического выполнения проводок 0-новый 1-старый*/

GDITE();

sprintf(strsql,"select ns from Alx where fil='avtprouslp.alx' limit 1");
if(readkey(strsql) <= 0)
 metka_avp=0;

sprintf(imaf_prot,"uslrp%d.tmp",getpid());
VVOD VV(0);

//Создаем класс меню
VVOD DANET(1);

mpr=1;
kom=0;
sum=0.;
metkasys.new_plus(ICEB_MP_USLUGI);

/*Список необходимых проводок*/

prosprus(1,podr,d,m,g,nn,tp,lnds,kop,pnds);

pozz=0;

naz1:;
GDITE();
clear();
short           kls=LINES-8;
time_t		vremm[kls];
int		ktoz[kls];
int		val[kls];

sum=0;

K=0;

sprintf(strsql,"select datp,sh,shk,kodkon,deb,kre,vrem,komen,ktoi,val,kekv \
from Prov where kto='%s' and pod=%d and nomd='%s' and \
datd='%04d-%02d-%02d' and oper='%s' and tz=%d order by datp asc",
metkasys.ravno(),podr,nn,g,m,d,kop,tp);

SQLCURSOR cur;  
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(0);
 }
if(kolstr == 0)
 {
lll:;
  prosprus(2,podr,d,m,g,nn,tp,lnds,kop,pnds);
  sapprus(d,m,g,nn,kop,kontr,sumad,sum,mpr,pnds);
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
  /*Суммиреум выполненные проводки*/
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
prosprus(2,podr,d,m,g,nn,tp,lnds,kop,pnds);

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

  if(provprusl(row,mpr) != 0)
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

sapprus(d,m,g,nn,kop,kontr,sumad,sum,mpr,pnds);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,prc,&K,"",COLS-26);


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
  class iceb_tu_str str_out(spis_menu.spis_str.ravno(kom));
  if(SRAV(gettext("Конец списка"),str_out.ravno(),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz;

  polen(str_out.ravno(),&datp,1,'|');
  polen(str_out.ravno(),&sh,2,'|');
  polen(str_out.ravno(),&shk,3,'|');

  polen(str_out.ravno(),&kor,4,'|');

  polen(str_out.ravno(),&deb,5,'|');
  deb=okrug(deb,0.01);

  polen(str_out.ravno(),&kre,6,'|');
  kre=okrug(kre,0.01);

  polen(str_out.ravno(),&komen,8,'|');
  polen(str_out.ravno(),&kekv_char,7,'|');
  vrem=vremm[kom];
  ktozap=ktoz[kom];
 }

vper:;

switch (K)
 {

  case FK1:   /*Получить подсказку*/
   GDITE();

   iceb_t_pdoc("usl2_1_1_1.txt");

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
    VV.data_plus(0,sh.ravno());
    VV.data_plus(1,shk.ravno());
    
    sprintf(strsql,"%.2f",deb);
    VV.data_plus(2,strsql);
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

    vprov1(&VV,metkasys.ravno(),0,&dpr,nn,kop,podr,d,m,g,1,vrem,kor.ravno(),ktozap,6,tp,val[kom]);
    goto naz1;

  case PLU:  /*Ввести запись*/
  case FK2:  /*Ввести запись*/
    if(iceb_t_pbpds(m,g) != 0)
     goto naz1;

    VV.VVOD_delete();
    sozvprov1(&VV);
    if(tp == 1 && vnomd[0] != '\0')
     {
      sprintf(strsql,"N%s",vnomd);
      VV.VVOD_SPISOK_zapis_data(4,strsql);
     }
    vprov1(&VV,metkasys.ravno(),0,&dpr,nn,kop,podr,d,m,g,0,0L,"\0",0,6,tp,0);
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
      udprgr(metkasys.ravno(),d,m,g,nn,podr,tp);
      goto naz1;
     }
    goto naz;

  case FK4:  /*Автоматические проводки*/
    GDITE();
    if(iceb_t_pbpds(m,g) != 0)
     goto naz;

    ff_prot=NULL;
    if((ff_prot = fopen(imaf_prot,"w")) == NULL)
     {
      error_op_nfil(imaf_prot,errno,0);
      ff_prot=NULL;
     }
    if(metka_avp == 1)
      avtprousl(tp,d,m,g,kop,nn,podr,kontr,sumkor,vnomd,pnds,ff_prot);
    else
      avtprousl1(tp,d,m,g,nn,ff_prot);

    if(ff_prot != NULL)
      fclose(ff_prot);
    goto naz1;

  case SFK4:  /*Просмотр проторола рассчёта проводок*/
    GDITE();
    prosf(imaf_prot);

     if(kolstr > 0)
       goto naz;  
     else
       goto lll;
     

  case FK5: /*Настройка автоматических проводок*/
    menuvnp(2,tp);
    goto naz1;

  case SFK5: //Распечатка проводок
    raspprov(&cur,d,m,g,nn,mpr,4);
    goto naz;
    

  case FK6: /*Новая подпрограмма автоматического выполнения проводок*/

    GDITE();
    if(iceb_t_pbpds(m,g) != 0)
     goto naz;

    ff_prot=NULL;
    if((ff_prot = fopen(imaf_prot,"w")) == NULL)
     {
      error_op_nfil(imaf_prot,errno,0);
      ff_prot=NULL;
     }

    avtprousl1(tp,d,m,g,nn,ff_prot);

    if(ff_prot != NULL)
      fclose(ff_prot);
    goto naz1;
    
    goto naz1;
    
  case FK7:  /*Переключить режим просмотра*/
    mpr++;
    if(mpr == 3)
     mpr=0;
    goto naz2;  

  case FK8:  /*Список контрагентов*/
     clear();
     nm.new_plus("");
     dirkontr(&kod,&nm,1);
     clear();
     if(kolstr > 0)
       goto naz;  
     else
       goto lll;

  case FK9:  /*работа со списком оплат*/
     diropld(2,d,m,g,nn,podr,skoplusl(d,m,g,nn,podr));

     clear();
     if(kolstr > 0)
       goto naz;  
     else
       goto lll;

  case ESC : /*Выход*/
     return(0);
       
  case FK10:  /*Выйти из накладной*/
    unlink(imaf_prot);
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
void sapprus(short d,short m,short g,const char nn[],const char kop[],
const char kontr[],double sumad,double sum,short mpr,float pnds)
{
double		sumasnds;

move(0,0);
printw("%s (%s)",gettext("Ввод и корректировка проводок"),ICEB_MP_USLUGI);
#if 0
##########################################################3
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
########################################################
#endif
move(1,0);
printw(gettext("Номер документа: %s Дата: %d.%d.%dг."),nn,d,m,g);
move(2,0);
printw(gettext("Код операции: %s Код контрагента: %s"),kop,kontr);

sumasnds=sumad+sumad*pnds/100.;
sumasnds=okrug(sumasnds,okrg1);

move(3,0);
printw("%s:%s/",gettext("Сумма по документу"),prnbr(sumad));
printw("%s",prnbr(sumasnds));
move(4,0);
printw(gettext("Сумма сделанных проводок:%s"),prnbr(sum));

move(5,0);
if(mpr == 0)
 printw("%s",gettext("Показаны все проводки"));
if(mpr == 1)
 printw("%s",gettext("Показаны только дебетовые проводки"));
if(mpr == 2)
 printw("%s",gettext("Показаны только кредитовые проводки"));
 
helstr(LINES-1,0,
"F1",gettext("помощь"),
"F2+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("авт.пр."),
"F5",gettext("нас.пров."),
"F6",gettext("авт.пр."),
"F7",gettext("пер."),
"F8",gettext("кон-ты"),
"F9",gettext("оплата"),
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
