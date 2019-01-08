/* $Id: dirprpd.c,v 5.51 2013/11/05 10:50:48 sasa Exp $ */
/*13.08.2018    02.02.1994      Белых А.И.      dirprpd.c
Ввод и просмотр проводок по определенной платежке
Если вернули 0 - завершить работу с платежкой
	     1 - нет
*/
#include <errno.h>
#include        "buhg.h"
#include        "dok4.h"
 
void sapprv(short dd,short md,short gd,const char *nomdk,const char *kodop,const char *kodor1,const char *nsh,const char *shet,short mpr,double sump,double uslb,double sum,double sumd,int kolst);
void avtprpld(const char*,const char*,const char*,const char*,const char*,FILE*);
void xdkbanks(const char *tabl,short dd,short md,short gd,const char *nomdok,const char *kodop);

extern class REC  rec;

int dirprpd(const char *tabl)
{
static class iceb_tu_str datprov(""); /*Дата первой вручную сделанной проводки*/
short           x=0,y=5;
int             K;
short           kom=0;
int             i;
class iceb_tu_str sh("");
class iceb_tu_str shk("");
class iceb_tu_str komen("");
double          sum=0.,sump,sumpp;
time_t		vrem=0;
double		deb=0.,kre=0.;
class iceb_tu_str datp("");
short		mpr;
class iceb_tu_str kor("");
short		d,m,g;
short     	dp,mp,gp;
class iceb_tu_str shet("");
class iceb_tu_str tipd("");
long		pozz,pozz1;
long		kolstr;
SQL_str         row,row1;
char		strsql[1024];
int		prc;
int		podr=0;
short		mspz; /*метка соответствия первой записи*/
int		pods=0;
char imaf_n_prov[112];
class iceb_tu_str kekv_char("");
class iceb_t_mstr spis_menu(0);
//int podsist=0;
SQLCURSOR       cur1;
int tipz=0;
FILE *ff_prot=NULL;
char imaf_prot[32];

sprintf(imaf_prot,"avpxdk%d.lst",getpid());
VVOD VV(0);
VVOD SOOB(1);

//Создаем класс меню
VVOD DANET(1);

mpr=1;
pozz=0;

if(SRAV(tabl,"Pltp",0) == 0)
 {
  tipz=2;
  pods=4;
  tipd.new_plus(ICEB_MP_PPOR);
  sprintf(imaf_n_prov,"%s","avtprodok.alx");
//  podsist=4;
 }

if(SRAV(tabl,"Pltt",0) == 0)
 {
  pods=5;
  tipd.new_plus(ICEB_MP_PTRE);
  sprintf(imaf_n_prov,"%s","avtprodokt.alx");
  tipz=1;
//  podsist=5;
 } 

if(iceb_t_poldan(rec.nsh.ravno(),&shet,imaf_n_prov) != 0)
 {
  iceb_t_soob(gettext("Не найден бухгалтерский счёт расчётного счета !"));
 }

naz1:;

clear();
GDITE();
short           kls=LINES-7;
time_t	 	vremm[kls];
int		ktozap[kls];
int		val[kls];
/*MEN5 MM5(kls,COLS,0);*/


/*Узнаем сумму подтвержденных записей*/
sump=sumpzpd(tabl,&dp,&mp,&gp,&sumpp);

/*Суммиреум выполненные проводки*/

sum=sumprpd(rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),0,tipd.ravno(),shet.ravno());

K=0;

sprintf(strsql,"select datp,sh,shk,kodkon,deb,kre,vrem,komen,ktoi,val,kekv \
from Prov where kto='%s' and pod=%d and nomd='%s' and \
datd='%d-%02d-%02d' and tz=%d",tipd.ravno(),podr,rec.nomdk.ravno(),rec.gd,rec.md,rec.dd,tipz);

SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
if(kolstr == 0)
 {

lll:;

  sapprv(rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),rec.kodop.ravno(),rec.kodor1.ravno(),rec.nsh.ravno(),shet.ravno(),mpr,sump,rec.uslb,sum,rec.sumd,kolstr);

  SOOB.VVOD_delete();
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == FK3 || K == SFK3 || K == ENTER || K == SFK2 || K == SFK5)
     goto lll;
  if(K == KEY_RESIZE)
   goto naz1;
   
  goto vper;

 }

naz:;

if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;

/*MM5.clearMEN5();*/
cur.poz_cursor(pozz);
mspz=2;
pozz1=0;
spis_menu.spis_str.free_class();
for(i=0;i < kls;)
 {
  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;
  deb=atof(row[4]);
  kre=atof(row[5]);
       
  if(mpr == 1 && kre != 0.)
    continue;

  if(mpr == 2 && deb != 0.)
    continue;

  rsdat(&d,&m,&g,row[0],2);   

  vremm[i]=atol(row[6]);
  ktozap[i]=atol(row[8]);
  val[i]=atol(row[9]);
  kekv_char.new_plus("");
  if(atoi(row[10]) != 0)
   kekv_char.new_plus(row[10]);
   
  sprintf(strsql,"%d.%d.%d|%-*s|%-*s|%-*s|%8.2f|%8.2f|%1s|%s",
  d,m,g,
  iceb_tu_kolbait(5,row[1]),row[1],
  iceb_tu_kolbait(5,row[2]),row[2],
  iceb_tu_kolbait(5,row[3]),row[3],
  deb,kre,kekv_char.ravno(),row[7]);

  i++;
  spis_menu.spis_str.plus(strsql);

  if(pozz1 == 1)
    mspz=1;


 }
if(i <= kls-1)
 {
  spis_menu.spis_str.plus(gettext("Конец списка"));
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


sapprv(rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),rec.kodop.ravno(),rec.kodor1.ravno(),rec.nsh.ravno(),shet.ravno(),mpr,sump,rec.uslb,sum,rec.sumd,kolstr);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,prc,&K,"",COLS-21);


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
   goto naz; /*Стали на конец списка*/

  polen(spis_menu.spis_str.ravno(kom),&datp,1,'|');
  polen(spis_menu.spis_str.ravno(kom),&sh,2,'|');
  polen(spis_menu.spis_str.ravno(kom),&shk,3,'|');

  polen(spis_menu.spis_str.ravno(kom),&kor,4,'|');

  polen(spis_menu.spis_str.ravno(kom),&deb,5,'|');
  deb=okrug(deb,0.01);

  polen(spis_menu.spis_str.ravno(kom),&kre,6,'|');
  kre=okrug(kre,0.01);

  polen(spis_menu.spis_str.ravno(kom),&kekv_char,7,'|');
  polen(spis_menu.spis_str.ravno(kom),&komen,8,'|');
  
  vrem=vremm[kom];
  
 }

vper:;

switch (K)
 {
  case ESC : /*Выход*/
     provpd(tabl);
     return(0);


  case FK1:   /*Получение подсказки*/
    GDITE();
    iceb_t_pdoc("dokum2_1_2.txt");
    clear();
    if(kolstr > 0)
      goto naz;
    else
      goto lll;


  case PLU:  /*Ввести запись*/
  case FK2:  /*Ввести запись*/
//   sprintf(strsql,"%s",iceb_t_imafnsi("blokdok.alx"));
   if(iceb_t_pbpds(rec.md,rec.gd) != 0)
     goto naz1;

    VV.VVOD_delete();
    sozvprov1(&VV);
    vprov1(&VV,tipd.ravno(),1,&datprov,rec.nomdk.ravno(),rec.kodop.ravno(),0,rec.dd,rec.md,rec.gd,
    0,0,rec.kodor1.ravno(),0,pods,tipz,0);

    goto naz1;

  case FK3:       /*Удалить запись*/

    if(iceb_t_pbpds(rec.md,rec.gd) != 0)
     goto naz;

    rsdat(&d,&m,&g,datp.ravno(),1);
    if(iceb_t_pvglkni(m,g) != 0)
     goto naz;

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      udprov(val[kom],g,m,d,sh.ravno(),shk.ravno(),vrem,deb,kre,komen.ravno_filtr(),2);
      goto naz1;
     }
    goto naz;

  case ENTER:
  case SFK2:   /*Корректировать запись*/

    if(iceb_t_pbpds(rec.md,rec.gd) != 0)
     goto naz;

    VV.VVOD_delete();
    sozvprov1(&VV);
    VV.VVOD_SPISOK_zapis_data(0,sh.ravno());
    VV.VVOD_SPISOK_zapis_data(1,shk.ravno());
    sprintf(strsql,"%.2f",deb);
    VV.VVOD_SPISOK_zapis_data(2,strsql);
    VV.VVOD_SPISOK_zapis_data(3,datp.ravno());

    if(deb == 0.)
     {
      VV.VVOD_SPISOK_zapis_data(0,shk.ravno());
      VV.VVOD_SPISOK_zapis_data(1,sh.ravno());
      sprintf(strsql,"%.2f",kre);
      VV.VVOD_SPISOK_zapis_data(2,strsql);
     }

    datprov.new_plus(datp.ravno());
    VV.VVOD_SPISOK_zapis_data(4,komen.ravno());
    VV.VVOD_SPISOK_zapis_data(5,kekv_char.ravno());

    vprov1(&VV,tipd.ravno(),1,&datprov,rec.nomdk.ravno(),rec.kodop.ravno(),0,rec.dd,rec.md,rec.gd,
    1,vremm[kom],kor.ravno(),ktozap[kom],pods,tipz,val[kom]);
    goto naz1;

  case SFK3:       /*Удалить Все проводки*/

    if(iceb_t_pbpds(rec.md,rec.gd) != 0)
       goto naz;

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить все проводки ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      
      udprgr(tipd.ravno(),rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),0,tipz);
      goto naz1;
     }

    goto naz;

  case FK4:  /*Автоматические проводки*/
    
    if(iceb_t_pbpds(rec.md,rec.gd) != 0)
       goto naz1;
    if(iceb_t_pvglkni(mp,gp) != 0)
      goto naz1;

    if((ff_prot = fopen(imaf_prot,"w")) == NULL)
     {
      error_op_nfil(imaf_prot,errno,0);
      ff_prot=NULL;
     }

    avtprpld(tabl,rec.kodop.ravno(),rec.nomdk.ravno(),rec.kodor1.ravno(),shet.ravno(),ff_prot);

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

      if(SRAV(tabl,"Pltp",0) == 0)
        iceb_redfil("avtprodok.alx",0);
      if(SRAV(tabl,"Pltt",0) == 0)
        iceb_redfil("avtprodokt.alx",0);

      shet.new_plus("");
      if(iceb_t_poldan(rec.nsh.ravno(),&shet,imaf_n_prov) != 0)
       {
        iceb_t_soob(gettext("Не найден бухгалтерский счёт расчётного счета !"));
       }

      clear();

      if(kolstr > 0)
        goto naz;
      else
        goto lll;

  case SFK5: //Распечатка проводок
    raspprov(&cur,rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),mpr,6);
    goto naz1;
    
  case FK7:  /*Переключить режим просмотра*/
    mpr++;
    if(mpr == 3)
     mpr=0;
    if(kolstr > 0)
      goto naz;  
    else
      goto lll;
      
  case FK8:   /*Расшифровка записей в проводке*/
    SOOB.VVOD_delete();

    i=0;
    sprintf(strsql,"select nais from Plansh where ns='%s'",sh.ravno());

    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      beep();
      printw(gettext("Не найден счёт %s в плане счетов !"),sh.ravno());
      printw("\n");
      OSTANOV();
     }
    else    
     {
      sprintf(strsql,"%-5s %s",sh.ravno(),row1[0]);
      SOOB.VVOD_SPISOK_add_MD(strsql);
     }
    sprintf(strsql,"select nais from Plansh where ns='%s'",shk.ravno());

    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      beep();
      printw(gettext("Не найден счёт %s в плане счетов !"),shk.ravno());
      printw("\n");
      OSTANOV();
     }
    else    
     {
      sprintf(strsql,"%-5s %s",shk.ravno(),row1[0]);
      SOOB.VVOD_SPISOK_add_MD(strsql);
     }

    if(kor.ravno()[0] != '\0')
     {

      sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
      kor.ravno());

      if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
       {
        beep();
        printw("%s %s !\n",gettext("Не найден код контрагента"),kor.ravno());
        OSTANOV();
       }
      else    
       {
        sprintf(strsql,"%-5s %s",kor.ravno(),row1[0]);
        SOOB.VVOD_SPISOK_add_MD(strsql);
       }
     }
    soobshw(&SOOB,stdscr,-1,-1,0,1);

    goto naz;     

  case FK9:  
    xdkbanks(tabl,rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),rec.kodop.ravno());
    goto naz1;
    
  case FK10:  /*Выйти из накладной*/
      unlink(imaf_prot);
      provpd(tabl);
      return(1);

  default:
    if(kolstr > 0)
      goto naz;
    else
      goto lll;

 }

}

/*********/
/*Шапка */
/********/
void sapprv(short dd,short md,short gd,const char *nomdk,
const char *kodop,const char *kodor1,const char *nsh,const char *shet,short mpr,double sump,
double uslb,double sum,double sumd,int kolst)
{

move(0,0);
printw(gettext("Ввод и корректировка проводок. Бух.счет %s"),shet);
refresh();
#if 0
#########################################################3
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
  printw(gettext("Кредиты"));
  attroff(A_BLINK); /*Выключение мерцания*/
 }
#######################################################
#endif
move(1,COLS-20);
printw("%s%10.2f",gettext("Сумма  :"),sumd);
move(2,COLS-20);
printw("%s%10.2f",gettext("Подт-но:"),sump);
move(3,COLS-20);
printw("%s%10.2f",gettext("Услуги :"),uslb);
refresh();

move(1,0);
printw("%s %s %s %d.%d.%d",
gettext("Номер документа:"),
nomdk,
gettext("Дата:"),
dd,md,gd);

move(2,0);
printw("%s %s %s %s %s %s",
gettext("Код операции:"),
kodop,
gettext("Код кон-та:"),
kodor1,
gettext("Р.с.:"),
nsh);
move(3,0);
printw("%s %.2f/%d",gettext("Сумма выполненных проводок:"),sum,kolst);

move(4,3);
if(mpr == 0)
 printw("%s",gettext("Показаны все проводки"));
if(mpr == 1)
 printw("%s",gettext("Показаны только дебетовые проводки"));
if(mpr == 2)
 printw("%s",gettext("Показаны только кредитовые проводки"));
 

refresh();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("ав.пров."),
"F5",gettext("наc.пров."),
"F7",gettext("перек."),
"F8",gettext("расшифровка"),
"F9",gettext("пров."),
"F10",gettext("выход"),NULL);

}
