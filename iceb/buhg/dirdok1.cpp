/* $Id: dirdok1.c,v 5.78 2014/08/31 06:18:18 sasa Exp $ */
/*16.11.2016    14.04.1997      Белых А.И.      dirdok1.c
Просмотр документа в материальном учете 
*/
#include        <math.h>
#include        <errno.h>
#include	"buhg.h"
#define		RMY   8   /*Размер массива по вертикали*/
#define		RMX   64  /*Размер массива по горизонтали*/

void 		sozfap(short,short,short,int,const char*,int,char[]);
short           prkor(short,short,short,int,int,int,const char*,int*);

void sdirnr1(double suma,double sumabn,double ves,short regh,int mu,const char *naimkor,
double *sumkor,int blokpid,int pro,int skl,const char *naiskl,
const char *kprr,const char *naimpr,int tipz,int ktoi,const char *nomdok,const char *nomnn,
short dd,short md,short gd,const char *kpos,const char *naimo,int lnds,
const char *nomdokp,short mdd,short mvnp,double kolmat,int kolstr,short blokir,double *maskor,
int loginrash,double sumsnds,double *sumkopl,float pnds);

void		vmat2(short,short,short,int,const char*,int,float);
void	        blokdok(short,int,const char*);
void sozf1(short dg,short mg,short gg,int skl,const char *nomdok,int tipz,int lnds,char imaf[RMY][RMX],double ves,short mvnp,const char *naimo);

extern double	okrcn;  /*Округление цены*/
extern double	okrg1;  /*Округление суммы*/
extern short    srtnk; /*0-не включена 1-включена сортировка записей в накладной*/
extern short    obzap; /*0-не объединять записи 1-обединять*/
extern short     mdd;  /*0-обычный документ 1-двойной*/
extern short	konost;   /*Контроль остатка 0-включен 1-выключен*/
extern short	mborvd;    /*0-многопользовательская работа в документе разрешена 1- запрещена*/

int dirdok1(short dd,short md,short gd, //Дата документа
int tipz,
int skl,  	//Склад
class iceb_tu_str *nomdok,	//Номер документа
KASSA	kasr)	//handle кассового регистратора
{
short    mvnp=0; /*0-внешняя 1 -внутреняя 2-изменение стоимости*/
class iceb_t_mstr spis_menu(0);
int		ktoi;
double		*maskor;
int      	blokpid;  /*Кто заблокировал*/
class iceb_tu_str naimkor("");
short           voz;  /*Возврат*/
int             K;
int             kom,kom1;
int             i,i1,prc;
short           x=0,y=6;
short           d,m,g;
double          suma,sumabn;
double          bb,sm;
char		imaf[RMY][RMX];
class iceb_tu_str bros("");
double		ves;
double          kol1,kol2;
short		regh; /*0-основной 1-расширенный 2-цена с НДС 3-полное наименование*/
int		mu;
short		mpn; /*Метка пустой накладной*/
double          krat;
short           mest;
double		sumkor=0.;
class iceb_tu_str br1("");
char		strsql[1024];
long		kolstr,kolpr;
long		pozz;
SQL_str         row,row1;
class iceb_tu_str naimat("");
class iceb_tu_str kpos(""),naimo("");
class iceb_tu_str kprr("");
class iceb_tu_str naimpr("");
class iceb_tu_str naiskl("");
int		skl1;
int		kodm=0,nk=0;
double		cena=0.,kolih=0.;
float		nds=0;
int		pro;
class iceb_tu_str nomnn(""); /*Номер налоговой накладной*/
class iceb_tu_str nomdokp(""); /*номер документа поставщика*/
class iceb_tu_str sheta("");
class iceb_tu_str shetz("");
short		voztar=0;
int		blokir=0; /*0-ввод и корректировка 1-только просмотр*/
int		lnds;
double		kolmat;
int		loginrash; //Логин распечатавшего чек
double		sumsnds=0.;
int		metprov=0;
double		sumkopl=0.;
short		dpnn=0,mpnn=0,gpnn=0;
double		sumandspr=0.; //Сумма НДС для приходного документа введенная вручную
class iceb_tu_str nomz("");
int metka_blok=0; /*0-разблокирова 1-заблокирован*/
time_t vrem_zap=0;
float pnds=0.;
int max_dlina_nazv=0;
class iceb_tu_str printer("lpr -r");
class iceb_tu_str printerloc("prterm -r");
//imafn("nastsys.alx",&imaf_nast);
iceb_t_poldan("Системный принтер",&printer,"nastsys.alx");
iceb_t_poldan("Приэкранный принтер",&printerloc,"nastsys.alx");

short dt,mt,gt;
poltekdat(&dt,&mt,&gt);

SQLCURSOR curr;
//Создаем класс меню
VVOD MENU(3);
VVOD VVOD1(2);
VVOD DANET(1);

maskor=NULL;

naz2:;

GDITE();
if(mborvd == 1) /*Установить флаг работы с документом*/
 {
  sprintf(strsql,"%d.%d.%d %d %s",dd,md,gd,skl,nomdok->ravno());

  if(sql_flag(&bd,strsql,0,0) != 0)
   {
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("С документом уже работает другой оператор !"));
    SOOB.VVOD_SPISOK_add_MD(gettext("Многопользовательская работа с документами заблокирована !"));
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    return(1);
   }
 }
mu=pozz=0;

if(readdok(&dd,&md,&gd,nomdok->ravno(),skl,&tipz,&kpos,&naimo,&kprr,&naimpr,
&metprov,&naiskl,&skl1,&lnds,&mvnp,&mdd,&pro,&ktoi,&nomnn,&nomdokp,
&sheta,&blokpid,&loginrash,&dpnn,&mpnn,&gpnn,&sumandspr,&pnds,&vrem_zap) != 0)
 {
  return(1);
 }

if(mborvd == 1) /*Установить флаг работы с документом*/
 {
  sprintf(strsql,"%d.%d.%d %d %s",dd,md,gd,skl,nomdok->ravno());

  if(sql_flag(&bd,strsql,0,0) != 0)
   {
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("С документом уже работает другой оператор !"));
    SOOB.VVOD_SPISOK_add_MD(gettext("Многопользовательская работа с документами заблокирована !"));
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    return(1);
   }
 }

mpn=0;
naimkor.new_plus("");
//memset(nmc,'\0',sizeof(nmc));

memset(imaf,'\0',sizeof(imaf));

/*Имя файла накладной*/
if(tipz == 1)
  sprintf(imaf[0],"np%ld.lst",vrem_zap);
if(tipz == 2)
  sprintf(imaf[0],"nr%ld.lst",vrem_zap);


/*имя файла налоговой накладной*/
if(tipz == 1)
  sprintf(imaf[1],"nnp%ld.lst",vrem_zap);
if(tipz == 2)
  sprintf(imaf[1],"nnr%ld.lst",vrem_zap);

/*Имя файла со счетом на оплату*/
sprintf(imaf[3],"sh%ld.lst",vrem_zap);

/*Имя файла с кассовым ордером*/
sprintf(imaf[4],"ko%ld.lst",vrem_zap);

/*Имя файла с актом приемки*/
sprintf(imaf[6],"ap%ld.lst",vrem_zap);

regh=voz=0;
ves=0.;
kom=0;


naz1:;
clear();
GDITE();
short           kls=LINES-1-8;;
int		nomkarp[kls];
time_t		vrem[kls];
int		ktozap[kls];


kolmat=suma=sumabn=ves=0.;


/*Читаем сумму корректировки если она есть*/
sumkor=0.;
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and sklad=%d and nomd='%s' and nomerz=13",
gd,skl,nomdok->ravno());

if(sql_readkey(&bd,strsql,&row,&curr) == 1)
 {
  sumkor=atof(row[0]);
  sumkor=okrug(sumkor,okrg1);
 }

/*Читаем заблокирован документ для выписки или нет*/
blokir=0;
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and sklad=%d and nomd='%s' and nomerz=4",
gd,skl,nomdok->ravno());

if(sql_readkey(&bd,strsql,&row,&curr) == 1)
 {
  blokir=1;
 }

sprintf(strsql,"select * from Dokummat1 where datd='%04d-%02d-%02d' and \
sklad=%d and nomd='%s' order by kodm asc",
gd,md,dd,skl,nomdok->ravno());
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
if(kolstr == 0)
 {

lll:;
  blokir=0;

  sdirnr1(suma,sumabn,ves,regh,mu,naimkor.ravno(),
&sumkor,blokpid,pro,skl,naiskl.ravno(),kprr.ravno(),naimpr.ravno(),tipz,ktoi,
nomdok->ravno(),nomnn.ravno(),dd,md,gd,kpos.ravno(),naimo.ravno(),lnds,nomdokp.ravno(),mdd,mvnp,kolmat,kolstr,
blokir,maskor,loginrash,sumsnds,&sumkopl,pnds);

  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  mpn=1;
   
  if(K == FK4 || K == SFK2 || K == FK3 || K == FK7 || K == SFK7 ||
   K == FK5 || K == SFK5 || K == FK6)
      goto lll;

  if(K == KEY_RESIZE)
    goto naz1;
     
  goto vper;
 }
else
 {
   printw("%s\n",gettext("Вычисляем сумму по документу"));
   refresh();
   sumdok(dd,md,gd,skl,nomdok->ravno(),&cur,&suma,&sumabn,&ves,&mu,lnds,&kolmat,&sumsnds,pnds,&max_dlina_nazv);
   printw("%s\n",gettext("Конец расчёта"));
   refresh();
 }

naz:;

spis_menu.spis_str.free_class();
char strmenu[500];
cur.poz_cursor(pozz);
kolpr=pozz;
for(i=0; i<=kls-1;i++)
 {

  if(cur.read_cursor(&row) == 0)
    break;
  kolpr++;  
  mpn=0;
  nomkarp[i]=atoi(row[14]);
  krat=0.;
  naimat.new_plus("");
  /*Читаем наименование материалла*/
  sprintf(strsql,"select * from Material where kodm=%s",
  row[4]);
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
    {
     beep();
     printw(gettext("Не найдено наименование материалла !"));
     OSTANOV();
    }
  else
   {
    naimat.new_plus(row1[2]);
    krat=atof(row1[7]);
   }  
  /*Читаем карточку материалла*/
  kol2=0;

  nk=atoi(row[3]);
  if(nk > 0)
   {
    sprintf(strsql,"select krat from Kart where sklad=%d and nomk=%d",
    skl,nk);
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
       beep();
       printw(gettext("Не нашли карточки %d склад %d !\n"),
       nk,skl);
       OSTANOV();
       i--;
       continue;
     }
    krat=atof(row1[0]);
   }
   
  /*Читаем в карточке количество реально выданное-полученное*/
  rsdat(&d,&m,&g,row[0],2); 
 
  kol2=readkolk(skl,nk,d,m,g,nomdok->ravno());  


  bb=atof(row[5])*atof(row[6]);
  sm=okrug(bb,okrg1);
  vrem[i]=atol(row[12]);
  ktozap[i]=atoi(row[11]);
  memset(strmenu,'\0',sizeof(strmenu));  
  if(regh == 1) /*расширенный режим показа */
   {
    sprintf(strmenu,"%-3s|%-3s|%-*.*s|%-*s|%7.7g|%6.6g|%6.6g|%s",
    row[4],row[3],
    iceb_tu_kolbait(12,naimat.ravno()),iceb_tu_kolbait(12,naimat.ravno()),naimat.ravno(),
    iceb_tu_kolbait(2,row[7]),row[7],
    atof(row[5]),atof(row[6]),sm,row[14]);
   }

  if(regh == 0) /*Основной режим показа */
   {
    int razmer=COLS-60;
    if(razmer < 20)
     razmer=20;
    if(max_dlina_nazv < razmer)
     razmer=max_dlina_nazv;
    sprintf(strmenu,"%-5s|%-4s|%-*.*s|%-*s|%8.8g|%8.8g|%8.8g|%8.8g",
    row[4],row[3],
    iceb_tu_kolbait(razmer,naimat.ravno()),iceb_tu_kolbait(razmer,naimat.ravno()),naimat.ravno(),
    iceb_tu_kolbait(4,row[7]),row[7],
    atof(row[5]),atof(row[6]),sm,kol2);
   }
  if(regh == 3)
   {
    sprintf(strmenu,"%-3s|%-3s|%-*s|%-*s|%8.8g|%8.8g|%8.8g",
    row[4],row[3],
    iceb_tu_kolbait(30,naimat.ravno()),naimat.ravno(),
    iceb_tu_kolbait(4,row[7]),row[7],
    atof(row[5]),atof(row[6]),sm);
   }

  if(regh == 2) /*Цена с НДС */
   {
    mest=0;
    if(atof(row[5]) != 0 && krat != 0.)
      mest=(short)(atof(row[5])/krat);

    bb=atof(row[6])+atof(row[6])*pnds/100.;
    bb=okrug(bb,0.01);
    sm=bb*atof(row[5]);
    sm=okrug(sm,0.01);
    sprintf(strmenu,"%-3s|%-3s|%-*.*s|%-*s|%8.8g|%8.8g|%8.8g|%.5g%%|%d/%d/%.10g",
    row[4],row[3],
    iceb_tu_kolbait(16,naimat.ravno()),iceb_tu_kolbait(16,naimat.ravno()),naimat.ravno(),
    iceb_tu_kolbait(4,row[7]),row[7],
    atof(row[5]),bb,sm,atof(row[9]),
    atoi(row[11]),mest,krat);
   }


  if(atof(row[9]) == 0.)
   strcat(strmenu,"*");  
  if(atoi(row[8]) == 1)
   strcat(strmenu,"#");  

  spis_menu.spis_str.plus(strmenu);
 }


if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;
    
if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

clear();

sdirnr1(suma,sumabn,ves,regh,mu,naimkor.ravno(),
&sumkor,blokpid,pro,skl,naiskl.ravno(),kprr.ravno(),naimpr.ravno(),tipz,ktoi,
nomdok->ravno(),nomnn.ravno(),dd,md,gd,kpos.ravno(),naimo.ravno(),lnds,nomdokp.ravno(),mdd,mvnp,kolmat,kolstr,
blokir,maskor,loginrash,sumsnds,&sumkopl,pnds);


/*Корректировка на сумму накладной*/
sprintf(strsql,"delete from Dokummat2 \
where god=%d and sklad=%d and nomd='%s' and nomerz=13",
gd,skl,nomdok->ravno());
/*
printw("strsql=%s\n",strsql);
refresh();
*/

sql_zapis(strsql,1,0);
/*
printw("sumkor=%f\n",sumkor);
OSTANOV();
*/
if(fabs(sumkor) > 0.009)
 {
  sprintf(strsql,"insert into Dokummat2 \
values (%d,%d,'%s',%d,'%.10g')",
g,skl,nomdok->ravno(),13,sumkor);
  sql_zapis(strsql,1,0);
 }   

prc=kolpr*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,prc,&K,"",0);


if(kom == -7) //Изменение размеров экрана
 goto naz1;
 
if(kom == -3) /*Стрелка вниз*/
 {
  if(i == kls)
    pozz++;
  kom=kls;
  goto naz;
 }
if(kom == -2) /*Стрелка вверх*/
 {
  if(pozz != 0)
    pozz--;
  goto naz;
 }
 
if(kom == -5) /*Страница вниз*/
 {
  if(i == kls)
    pozz+=kls;
  else
    beep();
/*
  printw("\n вперед pozz=%d\n",pozz);
  OSTANOV();
*/
  goto naz;

 }

if(kom == -4) /*Страница вверх*/
 {
  pozz-=kls;
  if(pozz < 0)  
   {
    beep();
    pozz=0;
   }
/*
  printw("\n назад pozz=%d\n",pozz);
  OSTANOV();
*/
  goto naz;
 }

if(kom == -1)
  voz=1;
else
 {
  if(SRAV(gettext("Конец списка"),spis_menu.spis_str.ravno(kom),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz; /*Стали на конец списка*/

  cur.poz_cursor(pozz+kom);
  cur.read_cursor(&row);

  kodm=atoi(row[4]);
  nk=atoi(row[3]);

  kolih=kol1=atof(row[5]);
  cena=atof(row[6]);
  voztar=atoi(row[8]);
  shetz.new_plus(row[15]);
  nomz.new_plus(row[17]);
 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     break;

  case ENTER:
     goto naz1;

  case UMN:  /*Переключение режима вывода на экран*/
    if(tipz == 1)
     {
       if(kolstr > 0)
         goto naz;
       else
        goto lll;
     }
    if(regh == 1)
     regh=0; 
    else
     regh=1;

    if(kolstr > 0)
      goto naz;
    else
     goto lll;

  case OTN:  /*Переключение режима вывода на экран*/
    
    if(regh == 2)
     regh=0; 
    else
     regh=2; 
     

    if(kolstr > 0)
      goto naz;
    else
     goto lll;

  case DEL:  /*Переключение режима вывода на экран*/
    
    if(regh == 3)
     regh=0; 
    else
     regh=3; 
     

    if(kolstr > 0)
      goto naz;
    else
     goto lll;

  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("matu2_1_2.txt");
   clear();
   if(kolstr > 0)
     goto naz;
   else
     goto lll;
     
  case SFK1:   /*Корректировать шапку накладной*/
   if(prkor(1,md,gd,blokpid,0,skl,nomdok->ravno(),&blokir) != 0)
    {
     if(kolstr > 0)
       goto naz;
     else
      goto lll;
    }
   korsdok(tipz,&dd,&md,&gd,&skl,nomdok,nomnn.ravno(),nomdokp.ravno(),kpos.ravno(),lnds,&kprr,dpnn,mpnn,gpnn);
   goto naz2;


  case PLU:   /*Ввести материал как FK2*/
  case FK2:   /*Ввести материал*/

   if(prkor(0,md,gd,blokpid,loginrash,skl,nomdok->ravno(),&blokir) != 0)
    {
     if(kolstr > 0)
       goto naz;
     else
      goto lll;
    }


   vmat2(dd,md,gd,skl,nomdok->ravno(),tipz,pnds);

   goto naz1;

  case SFK2:   /*Корректировать запись*/
   metka_blok=0;
   if(prkor(0,md,gd,blokpid,loginrash,skl,nomdok->ravno(),&blokir) != 0)
    {
/************ 
     if(kolstr > 0)
       goto naz;
     else
      goto lll;
****************/
     metka_blok=1;
    }
    
   korznkr(tipz,dd,md,gd,skl,nomdok->ravno(),kodm,nk,kolih,cena,skl,nomdokp.ravno(),nomkarp[kom],shetz.ravno(),nomz.ravno(),vrem[kom],ktozap[kom],pnds,metka_blok);
   kom++;

   goto naz1;

  case FK3:   /*Удалить запись в накладной*/
    if(prkor(0,md,gd,blokpid,loginrash,skl,nomdok->ravno(),&blokir) != 0)
     {
      if(kolstr > 0)
        goto naz;
      else
       goto lll;
     }
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить запись в документе ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      matudd(1,0,1,tipz,dd,md,gd,nomdok->ravno(),skl,nk,kodm);
     }

    goto naz1;  
    break;


  case FK5:   /*Распечатать*/
    sozf1(dd,md,gd,skl,nomdok->ravno(),tipz,lnds,imaf,ves,mvnp,naimo.ravno());
    MENU.VVOD_delete();

    MENU.VVOD_SPISOK_add_MD(gettext("Распечатка накладной"));
    MENU.VVOD_SPISOK_add_MD(gettext("Распечатка налоговой накладной"));
    MENU.VVOD_SPISOK_add_MD(gettext("Распечатка счета"));
    MENU.VVOD_SPISOK_add_MD(gettext("Распечатка кассового ордера"));
    MENU.VVOD_SPISOK_add_MD(gettext("Распечатка акта приемки"));
    MENU.VVOD_SPISOK_add_MD(gettext("Распечатка счета в две колонки"));
    MENU.VVOD_SPISOK_add_MD(gettext("Распечатка накладной форма М20"));
    MENU.VVOD_SPISOK_add_MD(gettext("Распечатка кассового чека"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

fff5:;
    clearstr(LINES-1,0);
    i1=0;
    while(menu3w(stdscr,&MENU,&i1,-1,-1,0) != 0);
    switch(i1)
     {
      case FK10:
      case -1:
      case 8:
        goto naz;

      case 0:
        if((i1=vibprint()) == 0)
          goto naz;
        if(i1 == 1)
         pehf(imaf[0],0,printer.ravno());
        if(i1 == 2)
         pehf(imaf[0],0,printerloc.ravno());
        blokir=1;
	blokdok(gd,skl,nomdok->ravno());

        break;      

      case 1:
        if((i1=vibprint()) == 0)
          goto naz;
        if(i1 == 1)
         pehf(imaf[1],0,printer.ravno());
        if(i1 == 2)
         pehf(imaf[1],0,printerloc.ravno());

        blokir=1;
	blokdok(gd,skl,nomdok->ravno());
        break;      

      case 2:
        if((i1=vibprint()) == 0)
          goto naz;
        if(i1 == 1)
         pehf(imaf[3],0,printer.ravno());
        if(i1 == 2)
         pehf(imaf[3],0,printerloc.ravno());
        blokir=1;
	blokdok(gd,skl,nomdok->ravno());
        break;      

      case 3:
        kasord1(imaf[4],"",dd,md,gd,"","",sumkopl,naimo.ravno(),"","","",NULL);
        if((i1=vibprint()) == 0)
          goto naz;
        if(i1 == 1)
         pehf(imaf[4],0,printer.ravno());
        if(i1 == 2)
         pehf(imaf[4],0,printerloc.ravno());
        blokir=1;
	blokdok(gd,skl,nomdok->ravno());
        break;      

      case 4:
        sozfap(dd,md,gd,skl,nomdok->ravno(),lnds,imaf[6]);
        if((i1=vibprint()) == 0)
          goto naz;
        if(i1 == 1)
         pehf(imaf[6],0,printer.ravno());
        if(i1 == 2)
         pehf(imaf[6],0,printerloc.ravno());
        break;      

      case 5:

        rasdok2(dd,md,gd,skl,nomdok->ravno(),lnds);
        blokir=1;
	blokdok(gd,skl,nomdok->ravno());
        break;

      case 6:
        rasdm20(dd,md,gd,skl,nomdok->ravno(),lnds);
        blokir=1;
	blokdok(gd,skl,nomdok->ravno());
        break;

      case 7:
        if(kolstr == 0)
         break;

        //Проверяем код операции
        if(iceb_t_poldan("Коды операций для распечатки чека",&bros,"matnast.alx") == 0)
         {
          if(proverka(bros.ravno(),kprr.ravno(),0,0) != 0)
           {
            sprintf(strsql,gettext("Кода операции %s нет в списке операций для которых разрешена печать чека !"),kprr.ravno());
            iceb_t_soob(strsql);
            break;
           }
          }
        if(sumkor != 0.)
         {
          iceb_t_soob(gettext("Перецените документ на процент скидки и отмените скидку !"));
          break;
         }
                          
        if(rasshek(&cur,dd,md,gd,tipz,skl,nomdok->ravno(),lnds,kasr,pnds) == 0)
          loginrash=iceb_t_getuid();
        blokir=1;
	blokdok(gd,skl,nomdok->ravno());
        break;
               
      default:
        goto fff5;
     }

    if(kolstr > 0)
      goto naz;
    else
     goto lll;


  case SFK5:   /*Просмотр*/

    sozf1(dd,md,gd,skl,nomdok->ravno(),tipz,lnds,imaf,ves,mvnp,naimo.ravno());
    if(tipz == 1)
     {
      MENU.VVOD_delete();
      MENU.VVOD_SPISOK_add_MD(gettext("Просмотр накладной"));
      MENU.VVOD_SPISOK_add_MD(gettext("Просмотр акта приемки"));
      MENU.VVOD_SPISOK_add_MD(gettext("Выход"));
      
      clearstr(LINES-1,0);
      i=0;
      while(menu3w(stdscr,&MENU,&i,-1,-1,0) != 0);

      if( i == -1 || i == 4 || i == FK10)
       {
        goto naz1;
       }

      if(i == 0)
       {
        prosf(imaf[0]);
       }
      if(i == 1)
       {
        sozfap(dd,md,gd,skl,nomdok->ravno(),lnds,imaf[6]);
        prosf(imaf[6]);
       }
      if(kolstr > 0)
        goto naz;
      else
       goto lll;
     }

    MENU.VVOD_delete();

    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр накладной"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр налоговой накладной"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр счета"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр кассового ордера"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));
    
    clearstr(LINES-1,0);
    i=0;
    while(menu3w(stdscr,&MENU,&i,-1,-1,0) != 0);

    if( i == -1 || i == 4 || i == FK10)
     {
      goto naz1;
     }

    if(i == 0)
      bros.new_plus(imaf[0]);
    if(i == 1)
      bros.new_plus(imaf[1]);
    if(i == 2)
      bros.new_plus(imaf[3]);
    if(i == 3)
      bros.new_plus(imaf[4]);
    prosf(bros.ravno());

    if(kolstr > 0)
      goto naz;
    else
     goto lll;


  case FK8:  /*Меню*/

    MENU.VVOD_delete();

    MENU.VVOD_SPISOK_add_MD(gettext("Перейти в другой документ"));
    MENU.VVOD_SPISOK_add_MD(gettext("Отметить запись"));
    MENU.VVOD_SPISOK_add_MD(gettext("Найти нужный код материалла"));
    if(srtnk == 0)
      MENU.VVOD_SPISOK_add_MD(gettext("Включить сортировку записей в распечатке"));
    if(srtnk == 1)
      MENU.VVOD_SPISOK_add_MD(gettext("Выключить сортировку записей в распечатке"));

    MENU.VVOD_SPISOK_add_MD(gettext("Ввести сумму или процент корректировки"));
    MENU.VVOD_SPISOK_add_MD(gettext("Снятие метки распечатанного чека"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));
   
    clearstr(LINES-1,0);
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 6 :
      case -1 :
        if(kolstr > 0)
          goto naz;
        else
          goto lll;

      case 0 :
        if(mborvd == 1) /*Снять флаг*/
         {
          sprintf(strsql,"%d.%d.%d %d %s",dd,md,gd,skl,nomdok->ravno());
          sql_flag(&bd,strsql,0,1);
         }

        helstr(LINES-1,0,"F10","вихід",NULL);

        br1.new_plus("");
        VVOD1.VVOD_delete();
        VVOD1.VVOD_SPISOK_add_MD(gettext("Введите номер документа"));

        if(vvod1(&br1,32,&VVOD1,NULL,stdscr,-1,-1) == FK10)
         {
          if(kolstr == 0)
           goto lll;
          else
           goto naz;
         }

        skl=0;
        nomdok->new_plus(br1.ravno());
        if(polen(nomdok->ravno(),&bros,1,'+') == 0)
          skl=bros.ravno_atoi();
        if(polen(nomdok->ravno(),&bros,1,'-') == 0)
          skl=bros.ravno_atoi();

        if(skl == 0)
         {
  
          VVOD1.VVOD_delete();
          VVOD1.VVOD_SPISOK_add_MD(gettext("Введите номер склада"));
          br1.new_plus("");
          if(vvod1(&br1,16,&VVOD1,NULL,stdscr,-1,-1) == FK10)
            break;

          skl=br1.ravno_atoi();
         }


        goto naz2;


     case 1 :  /*Пометить запись*/

       MENU.VVOD_delete();
       if(voztar == 0)
         MENU.VVOD_SPISOK_add_MD(gettext("Отметить как оборотную тару"));
       if(voztar == 1)
         MENU.VVOD_SPISOK_add_MD(gettext("Снять отметку оборотной тары"));
       MENU.VVOD_SPISOK_add_MD(gettext("Отметить с 0 проц. НДС"));
       MENU.VVOD_SPISOK_add_MD(gettext("Отметить с 20 проц. НДС"));
       MENU.VVOD_SPISOK_add_MD(gettext("Выход"));
       kom1=0;
       while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);
       
       if(kom1 == -1 || kom1 == 3)
        {
         if(kolstr > 0)
          goto naz;
         else
           goto lll;
        }

       if(kom1 == 0)
        {
         voztar++;
         if(voztar > 1)
           voztar=0;
        }
       if(kom1 == 1)
	 nds=0.;
       if(kom1 == 2)
	 nds=20.;

       if(kom1 == 0)
        sprintf(strsql,"update Dokummat1 \
set voztar=%d where datd='%d-%02d-%02d' and sklad=%d and nomd='%s' and \
kodm=%d and nomkar=%d",
       voztar,gd,md,dd,skl,nomdok->ravno(),kodm,nk);       

       if(kom1 == 1 || kom1 == 2)
        sprintf(strsql,"update Dokummat1 \
set nds=%.10g where datd='%d-%02d-%02d' and sklad=%d and nomd='%s' and \
kodm=%d and nomkar=%d",
       nds,gd,md,dd,skl,nomdok->ravno(),kodm,nk);       

       sql_zapis(strsql,1,0);

       goto naz1;

      case 2 : /*Найти материал по коду*/

        helstr(LINES-1,0,"F10","вихід",NULL);

        VVOD1.VVOD_delete();
        VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код материалла"));
        br1.new_plus("");
        if((i=vvod1(&br1,16,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
          goto naz;
        if(i == ESC)
          goto naz;

	kom=0;
        kodm=br1.ravno_atoi();
        pozz=0;
        cur.poz_cursor(pozz);

        while(cur.read_cursor(&row) != 0)
         {
          if(kodm == atoi(row[4]))
            break;
          pozz++;          
         }
        if(pozz == kolstr)
         {
          sprintf(strsql,"%s %d !",gettext("Не найден код материалла"),kodm);
          iceb_t_soob(strsql);
          pozz=0;       
         }

        goto naz;  
        break;

      case 3 : /*включение/выключние сортировки*/

        if(srtnk == 0)
         {
          srtnk=1;
          goto naz;  
         }
        if(srtnk == 1)
          srtnk=0;
        if(kolstr > 0)
          goto naz;  
        else
          goto lll;

      case 4 :  /*Ввести сумму корректировки*/

        helstr(LINES-1,0,"F10","вихід",NULL);

        VVOD1.VVOD_delete();
        VVOD1.VVOD_SPISOK_add_MD(gettext("Введите сумму или процент (%%)"));
        br1.new_plus("");
        if((i=vvod1(&br1,16,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
          goto naz;
        if(i == ESC)
          goto naz;

        naimkor.new_plus("");
        sumkor=br1.ravno_atof();
        if(polen(br1.ravno(),&bros,1,'%') == 0)
          sumkor=(suma+sumabn)*bros.ravno_atof()/100.;

        if(kolstr > 0)
         goto naz;
        else
          goto lll;

      case 5 : 
        VVOD PAR(2);
        PAR.VVOD_SPISOK_add_MD(gettext("Введите пароль"));
        if(parolv(&PAR,dt,mt,gt,1) != 0)
         {
         if(kolstr > 0)
          goto naz;
         else
           goto lll;
         }
                 
        sprintf(strsql,"delete from Dokummat2 where god=%d and \
nomd='%s' and sklad=%d and nomerz=%d",gd,nomdok->ravno(),skl,5);
 
        sql_zapis(strsql,1,0);
        goto naz2;
         
     }
    goto naz1;


  case FK9:  /*Меню*/
    if(tipz == 1)
     {
      if(kolstr > 0)
       goto naz;
      else
        goto lll;
     }      
    MENU.VVOD_delete();

    MENU.VVOD_SPISOK_add_MD(gettext("Переценить документ на заданный процент"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выбрать вид корректировки суммы по документу"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));
   
    clearstr(LINES-1,0);
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {

      case 2 :
      case -1 :
        if(kolstr > 0)
          goto naz;
        else
          goto lll;

      case 0 :  /*Переценить накладную на заданный процент*/
       perecen(d,m,g,nomdok->ravno(),skl,pnds);
       break;

      case 1 :  /*Выбрать вид снижек*/
       MENU.VVOD_delete();

       naimkor.new_plus("");
       
       if(makkor(&MENU,1,"",&maskor,"makkor.alx") != 0)
        break;       

       helstr(LINES-1,0,"F10",gettext("выход"),NULL);
       i=0;
       while(menu3w(stdscr,&MENU,&i,-1,-1,0) != 0);

       if( i == -1 || i == FK10)
        {
         if(kolstr > 0)
           goto naz;
         else
          goto lll;
        }

       naimkor.new_plus(MENU.VVOD_SPISOK_return_MD(i));
       makkor(&MENU,2,naimkor.ravno(),&maskor,"makkor.alx");
       if(kolstr > 0)
         goto naz;
       else
        goto lll;

        
     }
  
     goto naz1;
    
  default:
    if(kolstr > 0)
      goto naz;
    else
      goto lll;
 }

if(mpn == 1)
 {
  DANET.VVOD_delete();
  DANET.VVOD_SPISOK_add_MD(gettext("Удалить документ ?"));

  if(danet(&DANET,2,stdscr) == 1)
   {
    GDITE();
    if(udprgr(ICEB_MP_MATU,dd,md,gd,nomdok->ravno(),skl,tipz) != 0)
     goto naz1;
    matudd(0,0,0,tipz,dd,md,gd,nomdok->ravno(),skl,nk,kodm);
   }

  if(mborvd == 1) /*Снять флаг*/
   {
    sprintf(strsql,"%d.%d.%d %d %s",dd,md,gd,skl,nomdok->ravno());
    sql_flag(&bd,strsql,0,1);
   }
  return(voz);
 }

GDITE();

if(mborvd == 1) /*Снять флаг*/
 {
  sprintf(strsql,"%d.%d.%d %d %s",dd,md,gd,skl,nomdok->ravno());
  sql_flag(&bd,strsql,0,1);
 }

podvdok(dd,md,gd,nomdok->ravno(),skl);

if(metprov == 1)
  return(voz);

if(SRAV(nomdok->ravno(),"000",0) == 0)
  return(voz);

/*Если операция внутренняя и накладная на приход
Проводки выполняются только для расходных накладных*/

if(mvnp != 0 && tipz == 1)
  return(voz);

prospr(3,skl,dd,md,gd,nomdok->ravno(),tipz,lnds,skl1,nomdokp.ravno(),kprr.ravno());
//prospr(3,skl,dd,md,gd,nomdok,tipz,lnds,skl1,nomdokp);

return(voz);

}

/**************/
/*Выдача шапки*/
/**************/
void sdirnr1(double suma,  //Сумма по накладной
double sumabn,  //Сумма по возвратной таре без НДС
double ves,short regh,int mu,const char *naimkor,
double *sumkor,int blokpid,int pro,int skl,const char *naiskl,
const char *kprr,const char *naimpr,int tipz,int ktoi,const char *nomdok,const char *nomnn,
short dd,short md,short gd,const char *kpos,const char *naimo,int lnds,
const char *nomdokp,
short mdd,  //0-обычный документ 1-двойной
short mvnp, //0-внешняя 1 -внутреняя 2-изменение стоимости
double kolmat,  //Количество материалов
int kolstr,  //Количество записей в документе без услуг
short blokir,  //0-ввод и корректировка 1-только просмотр
double *maskor,
int loginrash,  //Логин распечатавшего кассовый чек
double sumsnds, //Сумма с вложенным НДС
double *sumkopl, //Сумма к оплате
float pnds)
{
/*extern double	*maskor; / *Массив с диапазонами и процентами корректировки*/
double		bb,bb1,bb2;
double		proc=0.;

*sumkopl=0.;

move(0,0);
printw("%s:%s %.40s",gettext("Организация"),kpos,naimo);
if(loginrash != 0)
 {
  move(0,COLS-5);
  attron(A_BLINK);
  printw("%d",loginrash);
  attroff(A_BLINK);
 }

if(blokpid != 0)
 {
  move(2,59);
  attron(A_BLINK);
  printw("%s %d",gettext("Заблокирована !"),blokpid);
  attroff(A_BLINK);
 }

if(naimkor[0] != '\0')
 {
  attron(A_BLINK);
  printw(" %s",naimkor);
  attroff(A_BLINK);
  
 }

if(mu != 0)
 {
  attron(A_BLINK);
  printw(gettext(" Услуги !"));
  attroff(A_BLINK);
 }

if(blokir == 1)
 {
  move(1,49);
  attron(A_BLINK);
  printw(gettext("Корректировка заблокирована !"));
  attroff(A_BLINK);
 }

move(1,0);
printw("%s: %d %s",gettext("Склад"),skl,naiskl);
move(2,0);
printw(gettext("Операция: %s %s"),kprr,naimpr);
if(tipz == 1)
 printw(gettext("(приход)"));
if(tipz == 2)
 printw(gettext("(расход)"));
move(3,0);
printw("%s: %s / %s %s: %d.%d.%d%s %s %d",
gettext("Номер документа"),
nomdok,
nomnn,
gettext("Дата"),
dd,md,gd,
gettext("г."),
gettext("Записал"),
ktoi);

/*Вычисляем сумму корректировки*/
if(maskor != NULL && naimkor[0] != '\0')
 {
   *sumkor=makkorr(suma+sumabn,maskor);
//  printw("\nsumkor=%f suma=%f\n",*sumkor,suma);
//  OSTANOV();
 }
 
bb1=okrug(suma,okrg1);
bb=suma+(*sumkor);
bb=bb+bb*pnds/100.;
bb=okrug(bb,okrg1);

bb2=okrug(sumabn,okrg1);

move(4,0);
if(lnds == 0)
 {
  printw("%s:%.2f/",gettext("Сумма всех записей"),
  bb1);

  if(bb2 > 0)
   printw("%s %.2f/",gettext("Б.НДС"),bb2);

  if(*sumkor != 0)
   {
    if(*sumkor > 0.)
      proc=(*sumkor)*100./bb1;
    if(*sumkor < 0.)
      proc=100*(*sumkor)/(bb1-(*sumkor)*-1);
    proc=okrug(proc,0.1);
    printw("КС %.2f %.1f%% / ",*sumkor,proc);
   }   
  *sumkopl=bb+bb2;
  printw("%.2f(%.2f)",*sumkopl,sumsnds);

  if(ves > 0)
    printw(" %s:%.1f",gettext("Вес"),ves);
 }
else
 {
  attron(A_BLINK);
  *sumkopl=bb1+bb2;
  printw("%s:%.2f(%.2f) %s %s:%.1f",
  gettext("Сумма всех записей"),*sumkopl,sumsnds,
  gettext("Б.НДС"),
  gettext("Вес"),ves);
  attroff(A_BLINK);
  if(*sumkor != 0.)
   {
    proc=(*sumkor)*100/(bb1+bb2);
    proc=okrug(proc,0.1);
    printw(" КС %.2f %.1f%%",*sumkor,proc);
    *sumkopl+=*sumkor;
    printw("/%.2f",*sumkopl);
   }   
 }

move(5,0);
printw("%s:%.10g/%d ",gettext("Количество"),kolmat,kolstr);
printw("%s:%s",gettext("Номер парного документа"),nomdokp);

if(mdd == 1 && mvnp == 1)
 {
  move(0,55);
  attron(A_BLINK); /*Влючение мерцания*/
  printw(gettext("Двойной документ"));
  attroff(A_BLINK); /*Выключение мерцания*/
 }
if(mvnp == 2)
 {
  move(0,55);
  attron(A_BLINK); /*Влючение мерцания*/
  printw(gettext("Изменение учётных данных"));
  attroff(A_BLINK); /*Выключение мерцания*/
 }


helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F5",gettext("печать"),
"F8",gettext("меню"),
"F9",gettext("меню"),
"F10",gettext("выход"),NULL);

}

/************************************************/
/*Проверка возможности корректировать накладную*/
/************************************************/
/*Если вернули 0 - можно корректировать
	       1 - нет
*/

short prkor(short mp, //0-все проверять 1-только блокировку накладной
short md,short gd,int blokpid,int loginrash,int skl,const char *nomdok,int *blokir)
{
char		strsql[512];
SQL_str		row;
SQLCURSOR curr;
if(blokpid != 0 )
 {
  VVOD SOOB(1);

  SOOB.VVOD_SPISOK_add_MD(iceb_t_kszap(blokpid));

  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return(1);
 }


if(mp == 1)
 return(0);

/*Читаем заблокирован документ для выписки или нет*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and sklad=%d and nomd='%s' and nomerz=4",
gd,skl,nomdok);
*blokir=0;
if(sql_readkey(&bd,strsql,&row,&curr) == 1)
 {
  *blokir=1;

  sprintf(strsql,gettext("Документ заблокирован !"));

  iceb_t_soob(strsql);

  return(1);
 }

if(loginrash != 0 )
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Изменения не возможны ! Распечатан чек !"));
  SOOB.VVOD_SPISOK_add_MD(iceb_t_kszap(loginrash));

  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return(1);
 }
 
if(iceb_t_pbpds(md,gd) != 0)
  return(1);
return(0);
}

/********************************/
/*Если файлов нет то создаем их*/
/********************************/
void sozf1(short dg,short mg,short gg,int skl,const char *nomdok,int tipz,int lnds,char imaf[RMY][RMX],double ves,
short mvnp, //0-внешняя 1 -внутреняя 2-изменение стоимости
const char *naimo)
{
FILE            *ff;

/*Создание накладной*/


if((ff = fopen(imaf[0],"w")) == NULL)
 {
  error_op_nfil(imaf[0],errno,"");
  return;
 }
startfil(ff);

rasdok(dg,mg,gg,skl,nomdok,imaf[0],lnds,0,ff,ves);
fclose(ff);
refresh();

if(tipz == 1)
  return;

/*Сделать анализ операции*/
if(mvnp != 0)
 return;

/*Создание налоговой накладной*/
rasnaln(dg,mg,gg,skl,nomdok,imaf[1],lnds);

/*Создание счета на оплату*/
if((ff = fopen(imaf[3],"w")) == NULL)
 {
  error_op_nfil(imaf[3],errno,"");
  return;
 }
startfil(ff);
rasdok(dg,mg,gg,skl,nomdok,imaf[3],lnds,1,ff,ves);
fclose(ff);

}

/*****************************/
/*Создание файла акта приемки*/
/*****************************/
void sozfap(short dg,short mg,short gg,int skl,const char *nomdok,int lnds,char imaf[])
{

FILE		*ff;
double		ves=0.;


errno=0;

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }
startfil(ff);

rasdok(dg,mg,gg,skl,nomdok,imaf,lnds,2,ff,ves);
fclose(ff);
refresh();

}

/*********************************/
/*Блокировка документа           */
/*********************************/
void	blokdok(short gd,int skl,const char *nomdok)
{
char	strsql[512];
sprintf(strsql,"insert into Dokummat2 \
values (%d,%d,'%s',4,%d)",gd,skl,nomdok,iceb_t_getuid());   

if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_DUP_ENTRY)
 {
  
  msql_error(&bd,__FUNCTION__,strsql);
 }
}
