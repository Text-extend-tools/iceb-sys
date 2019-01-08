/* $Id: dirprmu.c,v 5.51 2013/11/05 10:50:48 sasa Exp $ */
/*13.09.2018    02.02.1994      Белых А.И.      dirprmu.c
Ввод и просмотр проводок по определенному документа
Если вернули 0 - завершить работу с накладной
	     1 - нет
*/
#include <errno.h>
#include        "buhg.h"

void avtpromu(int,short,short,short,const char*,const char*,int,const char*,double,const char*,float,FILE *ff_prot);
void sapprmu(short,short,short,const char*,const char*,const char*,double,double,short);


short dirprmu(short d,short m,short g, //Дата документа
const char *nn, //Номер документа
int skk,  //Код склада
const char *kop, //Код операции
const char *ko, //Код контрагента
int tp, //1-приход 2-расход
double sumkor, //Величина корректировки суммы документа
int lnds,
const char *nn1, //Номер встречного документа
int skl1, //Встречный склад
float pnds)
{
short           d1,m1,g1;
int		ktozap=0;
short           x=0,y=5;
int             K;
short           kom;
int             i,prc;
class iceb_tu_str sh("");
class iceb_tu_str shk("");
class iceb_tu_str komen("");
double          sumd=0.,sumk=0.;
class iceb_tu_str dpr(""); /*Дата проводки*/
time_t		vrem=0;
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
class iceb_tu_str metkasys("");
class iceb_tu_str kekv_char("");
class iceb_t_mstr spis_menu(0);
FILE *ff_prot=NULL;
char imaf_prot[64];

GDITE();
sprintf(imaf_prot,"avpmu%d.lst",getpid());

int metka_avp=iceb_t_poldan_vkl("Автоматическое выполнение проводок по списку сделать основным","matnast.alx"); /*0-включено 1-выключено*/

sprintf(strsql,"select ns from Alx where fil='avtpro.alx' limit 1");
if(readkey(strsql) <= 0) /*если нет старого файла настройки проводок то проводки делать по новой настройке*/
   metka_avp=0;

VVOD VV(0);

//Создаем класс меню
VVOD DANET(1);

mpr=1;
kom=0;
metkasys.new_plus(ICEB_MP_MATU);
   
/*Список необходимых проводок*/
prospr(1,skk,d,m,g,nn,tp,lnds,skl1,nn1,kop);

pozz=0;

naz1:;
clear();
GDITE();

short           kls=LINES-7;
time_t		vremm[kls];
int		ktoz[kls];
int		val[kls];

sumd=sumk=0.;

K=0;

sprintf(strsql,"select datp,sh,shk,kodkon,deb,kre,vrem,komen,ktoi,val,kekv \
from Prov where kto='%s' and pod=%d and nomd='%s' and \
datd='%04d-%02d-%02d' and oper='%s' and tz=%d order by datp asc",
metkasys.ravno(),skk,nn,g,m,d,kop,tp);

class SQLCURSOR cur;  

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
if(kolstr == 0)
 {
lll:;
  clear();

  prospr(2,skk,d,m,g,nn,tp,lnds,skl1,nn1,kop);

  sapprmu(d,m,g,nn,kop,ko,sumd,sumk,mpr);

  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if( K == FK3 || K == SFK3 || K == ENTER || K == SFK2 \
  || K == SFK5)
     goto lll;
  if(K == KEY_RESIZE)
   goto naz1;
   
  goto vper;
 }
else
 {
  /*Суммиреум выполненные проводки*/
  sumd=sumk=0.;
  while(cur.read_cursor(&row) != 0)
   {
    sumd+=atof(row[4]);
    sumk+=atof(row[5]);
   }
 }

naz:;
GDITE();
clear();
prospr(2,skk,d,m,g,nn,tp,lnds,skl1,nn1,kop);

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

sapprmu(d,m,g,nn,kop,ko,sumd,sumk,mpr);

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
   iceb_t_pdoc("matu2_1_2a.txt");
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
    VV.VVOD_SPISOK_zapis_data(4,komen.ravno());
    VV.VVOD_SPISOK_zapis_data(5,kekv_char.ravno());
    dpr.new_plus(datp.ravno());

    vprov1(&VV,metkasys.ravno(),0,&dpr,nn,kop,skk,d,m,g,1,vrem,kor.ravno(),ktozap,1,tp,val[kom]);
    goto naz1;

  case PLU:  /*Ввести запись*/
  case FK2:  /*Ввести запись*/
//    sprintf(strsql,"%s",iceb_t_imafnsi("blokmak.alx"));
    if(iceb_t_pbpds(m,g) != 0)
      goto naz1;
    VV.VVOD_delete();
    sozvprov1(&VV);
    if(tp == 1)
     {
      sprintf(strsql,"N%s",nn1);
      VV.VVOD_SPISOK_zapis_data(4,strsql);
     }
    vprov1(&VV,metkasys.ravno(),0,&dpr,nn,kop,skk,d,m,g,0,0L,"\0",0,1,tp,0);
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
     // sprintf(strsql,"%s",iceb_t_imafnsi("blokmak.alx"));
      

      udprov(val[kom],g1,m1,d1,sh.ravno(),shk.ravno(),vrem,deb,kre,komen.ravno_filtr(),2);
      goto naz1;      
     }
    clear();
    goto naz;

  case SFK3:       /*Удалить Все проводки*/

    if(iceb_t_pbpds(m,g) != 0)
      goto naz;
    rsdat(&d1,&m1,&g1,datp.ravno(),0);
    if(iceb_t_pvglkni(m1,g1) != 0)
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
    if(iceb_t_pbpds(m1,g1) != 0)
      goto naz;
    if((ff_prot = fopen(imaf_prot,"w")) == NULL)
     {
      error_op_nfil(imaf_prot,errno,0);
      ff_prot=NULL;
     }
    if(metka_avp == 1)
      avtpromu(tp,d,m,g,kop,nn,skk,ko,sumkor,nn1,pnds,ff_prot);
    else
      avtpromu1(tp,d,m,g,nn,ff_prot);

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
      menuvnp(1,tp);
      goto naz1;

  case SFK5: //Распечатка проводок
     raspprov(&cur,d,m,g,nn,mpr,1);
     goto naz;

  case FK6: /*Автоматическое выполнение проводок*/
    GDITE();
    if(iceb_t_pbpds(m1,g1) != 0)
      goto naz;
    if((ff_prot = fopen(imaf_prot,"w")) == NULL)
     {
      error_op_nfil(imaf_prot,errno,0);
      ff_prot=NULL;
     }

    if(metka_avp == 0)
      avtpromu(tp,d,m,g,kop,nn,skk,ko,sumkor,nn1,pnds,ff_prot);
    else
      avtpromu1(tp,d,m,g,nn,ff_prot);

    if(ff_prot != NULL)
      fclose(ff_prot);
    goto naz1;
     
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

  case FK9:  /*Список оплат по документа*/
     diropld(1,d,m,g,nn,skk,skoplmu(d,m,g,nn,skk));
     
     if(kolstr > 0)
       goto naz;  
     else
       goto lll;


  case ESC : /*Выход*/
     unlink(imaf_prot);
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
void sapprmu(short d,short m,short g,const char *nn,const char *kop,
const char *ko,double sumd,double sumk,short mpr)
{

move(0,0);
printw(gettext("Ввод и корректировка проводок"));
#if 0
############################################3
if(mpr == 0)
 {
  move(0,COLS-35);
  printw("        ");
 }

if(mpr == 1)
 {
  move(0,COLS-35);
  attron(A_BLINK); /*Влючение мерцания*/
  printw(gettext("Дебеты "));
  attroff(A_BLINK); /*Выключение мерцания*/
 }
if(mpr == 2)
 {
  move(0,COLS-35);
  attron(A_BLINK); /*Влючение мерцания*/
  printw(gettext("Кредиты"));
  attroff(A_BLINK); /*Выключение мерцания*/
 }
############################################
#endif
move(1,0);
printw(gettext("Номер документа: %s Дата: %d.%d.%dг."),nn,d,m,g);
move(2,0);
printw(gettext("Код операции: %s Код контрагента: %s"),kop,ko);

move(3,0);
printw(gettext("Сумма сделанных проводок: %s"),prnbr(sumd));
printw(" / %s",prnbr(sumk));

move(4,0);
if(mpr == 0)
 printw("%s",gettext("Показаны все проводки"));
if(mpr == 1)
 printw("%s",gettext("Показаны только дебетовые проводки"));
if(mpr == 2)
 printw("%s",gettext("Показаны только кредитовые проводки"));
 
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("авт.пр."),
"F5",gettext("нас.пров."),
"F6",gettext("авт.пров."),
"F7",gettext("пер."),
"F8",gettext("кон-ты"),
"F9",gettext("оплата"),
"F10",gettext("выход"),NULL);
}
