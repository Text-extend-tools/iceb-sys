/* $Id: dirdok.c,v 5.169 2014/08/31 06:18:17 sasa Exp $ */
/*24.04.2018    14.04.1997      Белых А.И.      dirdok.c
Просмотр документа в материальном учете 
Если вернули 0 записи были
	     если > 0 не было
*/
#include        <math.h>
#include        <errno.h>
#include	"buhg.h"
#define IMA_FIL_ERROR_SHTRIH "error_shtrih.dat"

int rasnak(short dg,short mg,short gg,int skl,const char *nomdok,const char *imaf,short lnds,int *simv_plus,FILE *f1,double ves);
void            rasdok1(short,short,short,int,const char*,short,double);
void sozfap(short dg,short mg,short gg,int skl,const char *nomdok,int lnds,const char *imaf);
short           prkor(short,short,short,int,int);
extern void 	sdirnr(double,double,double,int,const char*,double*,int,int,int,
const char*,const char*,const char*,int,int,const char*,const char*,short,short,short,const char*,const char*,
int,const char*,short,short,double,int,double*,int,double,double*,double,float,int max_dlinna_stroki_menu);
double		provkol(short,short,short,int,const char*);
void            rasprop(short dd,short md,short gd,int skl,const char *nomdok);
void            dirmusl(short d,short m,short g,int skl,const char *nomdok,float);
void            rozkor(const char*,short,short,short,int);
void            rasgrup(short); 
void            rasmat(void); 
void	dooc(short,short,short,const char*,int);
int 	provudvdok(int,short,short,short,const char*,int,int);
int	udpardok(short,int,short,short,short,int,const char*,int,const char*);
void   rasmedpr(short,short,short,int,const char*);
void   cennikd(short,short,short,int,const char*);
void   perecenp(short,short,short,const char*,int);
void   rasactmu(short,short,short,int,const char*);
short    dirprmu(short,short,short,const char*,int,const char*,const char*,int,double,int,const char*,int,float);
void dirdok_kas_or(int tipz,short dd,short md,short gd,double suma,const char *kontr,const char *naimkont);
void mu_shet_fak(short dd,short md,short gd,int sklad,const char *nomdok,char *imafil,int tipz);
int mat_act_vr(short dg,short mg,short gg,int skl,const char nomdok[],char *imafil);
void raspril(short dg,short mg,short gg,int skl,const char *nomdok);
int rasdoksp(short dg,short mg,short gg,int skl,const char *nomdok,class iceb_tu_str *imaf_ras);
void rasalk(short dg,short mg,short gg,int skl,const char *nomdok);
void deletdokum(class SPISOK *imaf);
void sozf(short dg,short mg,short gg,int skl,const char *nomdok,int tipz,int lnds,class SPISOK *imaf,double ves,short ,const char *naimo);
void dirdok_r1(int tipz,short dd,short md,short gd,const char *nomdok,class SPISOK *imaf,double sumkopl,const char *naimo,int skl,int lnds,double ves,int mvnp);
void dirdok_r2(int tipz,short dd,short md,short gd,const char *nomdok,class SPISOK *imaf,double sumkopl,const char *naimo,int skl,int lnds,double ves,int mvnp);
void oplmu(short d,short m,short g,int tipz,const char *nn,int skll,const char *kodop);
int dirdok_snn(short dg,short mg,short gg,int skl,const char *nomdok,int lnds,const char *imaf);
void muso(short dd,short md,short gd,const char *nomdok,int sklad,const char *kodkontr,const char *nomdokp);
void dirdok_f9r(short dd,short md,short gd,const char *nomdok,int skl,float pnds,class SPISOK *imaf,double **maskor,const char *nomdokp,const char *kpos);
void impmatost(int tipz,int sklad,short dd,short md,short gd,const char *nomdok);
void dirdok_f9p(short dd,short md,short gd,const char *nomdok,int skl,double *sumandspr,class SPISOK *imaf);
int dirdok_vhk(int tipz,short dd,short md,short gd,const char *nomdok,const char *nomdokp,int sklad,int skladp,const char *kontr,int metka_prkk,int *koloh);
void dirdok_ohout(int tipz,short dd,short md,short gd,const char *nomdok,const char *strout);
int rasttn(short dd,short md,short gd,const char *nomdok,int sklad,class spis_oth *oth);
int dirdokummat4(short dd,short md,short gd,int skl,const char *nomdok);
int rasmushet(short dg,short mg,short gg,int skl,const char *nomdok,const char *imaf,short lnds,int *simv_plus,FILE *f1,double ves);

extern double	okrcn;  /*Округление цены*/
extern double	okrg1;  /*Округление суммы*/
extern short    srtnk; /*0-не включена 1-включена сортировка записей в накладной*/
extern short    obzap; /*0-не объединять записи 1-обединять*/
extern short     mdd;  /*0-обычный документ 1-двойной*/
extern short	mborvd;    /*0-многопользовательская работа в документе разрешена 1- запрещена*/
extern class iceb_tu_str nameprinter;
extern char *imabaz;
class iceb_rnfxml_data rek_zag_nn;

int dirdok(short dd,short md,short gd, //Дата документа
int tipz,  //1-приход 2-расход
int skl,  //Склад
class iceb_tu_str *nomdok) //Номер документа
{
class iceb_tu_str vid_dok("");
int koloh=0; /*количество ошибок при сканерном вводе*/
int metka_prkk=0; /*0-привязывать к карточкам при вводе по штрихкоду для расходных документов 1-нет*/
short    mvnp=0; /*0-внешняя 1 -внутреняя 2-изменение стоимости*/
class iceb_tu_str printer("lpr -r");
class iceb_tu_str printerloc("prterm -r");
class spis_oth oth;
class ostatokt ost;
double		*maskor;
int      	blokpid;  /*Кто заблокировал*/
class iceb_tu_str naimkor("");
short           voz;  /*Возврат*/
int             K;
int             kom,kom1;
int             i,prc;
short           x=0,y=6;
short           d,m,g;
short		dp,mp,gp;
double          suma,sumabn;
double          bb=0.,sm=0.;
class SPISOK imaf;
class iceb_tu_str bros("");
double		ves=0.;
double          kol1=0.,kol2=0.;
short		regh; /*0-основной 1-расширенный 2-цена с НДС 3-полное наименование*/
int		mu;
short		mpn=0; /*Метка пустой накладной*/
double          krat;
short           mest;
double		sumkor=0.;
class iceb_tu_str br1("");
char		strsql[1024];
long		kolstr,kolpr=0;
long		pozz=0;
SQL_str         row,row1;
class iceb_tu_str naimat("");
class iceb_tu_str dnaim("");
int		lnds;
class iceb_tu_str kpos(""),naimo("");
class iceb_tu_str kprr("");
class iceb_tu_str naimpr("");
class iceb_tu_str naiskl("");
int		skl1;
int		kodm=0,nk=0;
double		cena=0.,kolih=0.;
float		nds=0.;
int		pro;
int		ktoi;
class iceb_tu_str nomnn(""); /*Номер налоговой накладной*/
class iceb_tu_str nomdokp(""); /*номер документа поставщика*/
class iceb_tu_str sheta("");
class iceb_tu_str shetz("");
short		voztar=0;
short		mnds;
double		kolmat; /*Количество материалов в документе*/
int		loginrash; //Логин распечатавшего чек
double		sumsnds=0.;
int		metprov=0;
double		sumkopl=0.;
short		dpnn=0,mpnn=0,gpnn=0; //дата получения налоговой накладной
double		sumandspr=0.;
class iceb_tu_str nomz(""); //Номер заказа
int metka_blok=0; /*0-разблокировано 1-заблокировано*/
int razmer=0;
time_t vrem_zap;
int max_dlina_nazv=0;
int metka_shtrih=0; /*если равно 1 то включен режим ввода по штрихкоду*/
class iceb_tu_str imaf_nn_xml("");
int pnd=0;
class iceb_t_mstr spis_menu(0);
int max_dlinna_stroki_menu=0; /*максимальная длинна строки меню*/

float   pnds=0.; //процент НДС
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);


/*
printw("%d.%d.%d %d %d %s\n",
dd,md,gd,tipz,skl,nomdok);
OSTANOV();
*/
//Создаем класс меню
class VVOD DANET(1);
class VVOD MENU(3);
class VVOD VV(0);
class VVOD PAR(2);
class VVOD VVOD1(2);

maskor=NULL;

naz2:;

GDITE();

mnds=0;
mu=pozz=0;

if(readdok(&dd,&md,&gd,nomdok->ravno(),skl,&tipz,&kpos,&naimo,&kprr,&naimpr,
&metprov,&naiskl,&skl1,&lnds,&mvnp,&mdd,&pro,&ktoi,&nomnn,&nomdokp,&sheta,
&blokpid,&loginrash,&dpnn,&mpnn,&gpnn,&sumandspr,&pnds,&vrem_zap) != 0)
  return(1);

/*Делаем после чтения подтому что там иногда определяется дата*/
if(mborvd == 1) /*Установить флаг работы с документом*/
 {
  sprintf(strsql,"%s%d.%d.%d%d%s",imabaz,dd,md,gd,skl,nomdok->ravno());

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

imaf.free_class();

/*Имя файла накладной*/
if(tipz == 1)
   sprintf(strsql,"np%ld.lst",vrem_zap);
if(tipz == 2)
   sprintf(strsql,"nr%ld.lst",vrem_zap);

imaf.plus(strsql);/*0*/

/*имя файла налоговой накладной*/
if(tipz == 1)
     sprintf(strsql,"nnp%ld.lst",vrem_zap);
if(tipz == 2)
     sprintf(strsql,"nnr%ld.lst",vrem_zap);
imaf.plus(strsql);/*1*/

/*Имя файла со счетом на оплату*/
sprintf(strsql,"sh%ld.lst",vrem_zap);
imaf.plus(strsql);/*2*/

/*Имя файла с кассовым ордером*/
sprintf(strsql,"ko%ld.lst",vrem_zap);
imaf.plus(strsql);/*3*/

/*Имя файла с актом приемки*/
sprintf(strsql,"ap%ld.lst",vrem_zap);
imaf.plus(strsql);/*4*/

/*Имя файла с актом списания*/
sprintf(strsql,"acts%ld.lst",vrem_zap);
imaf.plus(strsql);/*5*/

regh=voz=0;
ves=0.;
kom=0;


naz1:;

clear();
GDITE();

short           kls=LINES-1-8;
int		nomkarp[kls];
time_t		vrem[kls];
int		ktozap[kls];
max_dlinna_stroki_menu=0;

/*MEN5 MM5(kls,COLS,0);*/

kolmat=suma=sumabn=ves=0.;
/*Читаем сумму корректировки если она есть*/
sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=13",
gd,skl,nomdok->ravno());

SQLCURSOR cur;
SQLCURSOR cur1;

sumkor=0.;
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  sumkor=ATOFM(row[0]);
  sumkor=okrug(sumkor,0.01);
 }

sprintf(strsql,"select * from Dokummat1 where datd='%04d-%02d-%02d' and \
sklad=%d and nomd='%s' order by kodm asc",
gd,md,dd,skl,nomdok->ravno());
//printw("\n%s\n",strsql);
//OSTANOV();
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
//Могут быть введены только услуги

printw(gettext("Вычисляем сумму по документу\n"));
refresh();

sumdok(dd,md,gd,skl,nomdok->ravno(),&cur,&suma,&sumabn,&ves,&mu,lnds,&kolmat,&sumsnds,pnds,&max_dlina_nazv);
printw(gettext("Конец расчёта\n"));
refresh();

if(kolstr == 0 )
 {

lll:;

  sdirnr(suma,sumabn,ves,mu,naimkor.ravno(),&sumkor,blokpid,pro,skl,naiskl.ravno(),kprr.ravno(),naimpr.ravno(),tipz,ktoi,
nomdok->ravno(),nomnn.ravno(),dd,md,gd,kpos.ravno(),naimo.ravno(),lnds,nomdokp.ravno(),mdd,mvnp,kolmat,kolstr,
maskor,loginrash,sumsnds,&sumkopl,sumandspr,pnds,max_dlinna_stroki_menu);

  VVOD SOOB(1);

  if(suma == 0.)
    SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  else
    SOOB.VVOD_SPISOK_add_MD(gettext("Введены только услуги !"));
  
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(suma == 0.)
    mpn=1;
   
  if(K == SFK2 || K == FK3 || K == FK7 || K == SFK7  || K == FK6)
      goto lll;

  if(suma == 0.)
   if(K == FK4 || K == FK5 || K == SFK5)
      goto lll;
 
  if(K == KEY_RESIZE)
    goto naz1;

  if(metka_shtrih != 0)
   goto vvod_shtrih;    

  goto vper;
 }


naz:;


cur.poz_cursor(pozz);
kolpr=pozz;
spis_menu.spis_str.free_class();
for(i=0; i<=kls-1;)
 {
  if(cur.read_cursor(&row) == 0)
    break;

  kolpr++;  
  mnds=atoi(row[10]);
  nds=atof(row[9]);

  mpn=0;

  krat=0.;
  naimat.new_plus("");
  /*Читаем наименование материалла*/
  sprintf(strsql,"select * from Material where kodm=%s",row[4]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"%s %s",row[4],gettext("Не найдено наименование материалла !"));
    iceb_t_soob(strsql);
   }
  else
   {
    naimat.new_plus(row1[2]);
    krat=atof(row1[7]);
    if(row[16][0] != '\0')
     {
      naimat.plus("+",row[16]);
     }
   }  

  /*Читаем карточку материалла*/
  kol2=0;

  nk=atoi(row[3]);
  nomkarp[i]=atoi(row[14]);

  if(nk > 0)
   {
    sprintf(strsql,"select krat,mnds,nds,innom from Kart where sklad=%d and nomk=%d",
    skl,nk);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
       beep();
       printw(gettext("Не нашли карточки %d склад %d !\n"),
       nk,skl);
       OSTANOV();
     }
    else
     {
      krat=atof(row1[0]);
      mnds=atoi(row1[1]);
      nds=atof(row1[2]);
      if(row1[3][0] != '\0')
       {
        naimat.plus(" N",row1[3]);
       }
     }
   }
   
  /*Читаем в карточке количество реально выданное-полученное*/
  rsdat(&d,&m,&g,row[0],2); 
 
  kol2=readkolk(skl,nk,d,m,g,nomdok->ravno());  

  bb=atof(row[5])*atof(row[6]);
  sm=okrug(bb,okrg1);
  vrem[i]=atol(row[12]);
  ktozap[i]=atoi(row[11]);

  memset(strsql,'\0',sizeof(strsql));      
  if(regh == 1) /*расширенный режим показа */
   {
    razmer=COLS-68;
    if(razmer < 20)
     razmer=20;
    sprintf(strsql,"%-3s|%-3s|%-*.*s|%-*s|%7.7g|%6.6g|%6.6g|%s|%s|%s",
    row[4],row[3],
    iceb_tu_kolbait(20,naimat.ravno()),iceb_tu_kolbait(razmer,naimat.ravno()),naimat.ravno(),
    iceb_tu_kolbait(2,row[7]),row[7],
    atof(row[5]),atof(row[6]),sm,
    row[14],row[15],row[17]);
   }

  if(regh == 0) /*Основной режим показа */
   {
    razmer=COLS-60;
    if(razmer < 20)
     razmer=20;
    if(max_dlina_nazv < razmer)
     razmer=max_dlina_nazv;
    sprintf(strsql,"%-5s|%-4s|%-*.*s|%-*s|%8.8g|%8.8g|%8.8g|%8.8g",
    row[4],row[3],
    iceb_tu_kolbait(razmer,naimat.ravno()),iceb_tu_kolbait(razmer,naimat.ravno()),naimat.ravno(),
    iceb_tu_kolbait(4,row[7]),row[7],
    atof(row[5]),atof(row[6]),sm,kol2);
   }
  if(regh == 3) //Длинное наименование
   {
    sprintf(strsql,"%-3s|%-3s|%-*s|%-*s|%8.8g|%8.8g|%8.8g",
    row[4],row[3],
    iceb_tu_kolbait(20,naimat.ravno()),naimat.ravno(),
    iceb_tu_kolbait(4,row[7]),row[7],
    atof(row[5]),atof(row[6]),sm);
   }

  if(regh == 2) /*Цена с НДС */
   {
    mest=0;
    if(atof(row[5]) != 0. && krat != 0.)
      mest=(short int)(atof(row[5])/krat);

    bb=atof(row[6])+atof(row[6])*pnds/100.;
    bb=okrug(bb,0.01);
    sm=bb*atof(row[5]);
    sm=okrug(sm,0.01);

    razmer=COLS-64;
    if(razmer < 20)
     razmer=20;
    
    sprintf(strsql,"%-3s|%-3s|%-*.*s|%-*s|%8.8g|%8.8g|%8.8g|%.5g%%|%d/%d/%.10g",
    row[4],row[3],
    iceb_tu_kolbait(20,naimat.ravno()),iceb_tu_kolbait(razmer,naimat.ravno()),naimat.ravno(),
    iceb_tu_kolbait(4,row[7]),row[7],
    atof(row[5]),bb,sm,atof(row[9]),
    atoi(row[11]),mest,krat);
   }


  if(atoi(row[8]) == 1) /*Возвратная тара*/
   strcat(strsql,"#");  

  if(nds == 0) /*НДС*/
   strcat(strsql,"*"); 

  if(mnds == 1) /*НДС в том числе*/
   strcat(strsql,"+");  

  if(max_dlinna_stroki_menu < iceb_tu_strlen(strsql))
   max_dlinna_stroki_menu=iceb_tu_strlen(strsql);
   
  spis_menu.spis_str.plus(strsql);
  i++;
 }

if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;
    
if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

clear();

sdirnr(suma,sumabn,ves,mu,naimkor.ravno(),
&sumkor,blokpid,pro,skl,naiskl.ravno(),kprr.ravno(),naimpr.ravno(),tipz,ktoi,
nomdok->ravno(),nomnn.ravno(),dd,md,gd,kpos.ravno(),naimo.ravno(),lnds,nomdokp.ravno(),mdd,mvnp,kolmat,
kolstr,maskor,loginrash,sumsnds,&sumkopl,sumandspr,pnds,max_dlinna_stroki_menu);


/*Корректировка на сумму накладной*/
sprintf(strsql,"delete from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=13",gd,skl,nomdok->ravno());

if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_DBACCESS_DENIED_ERROR) //У оператора доступ только на чтение
   msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);

/*
printw("sumkor=%f\n",sumkor);
OSTANOV();
*/

if(fabs(sumkor) > 0.009)
 {
  sprintf(strsql,"insert into Dokummat2 values (%d,%d,'%s',%d,'%.2f')",g,skl,nomdok->ravno(),13,sumkor);
  sql_zapis(strsql,1,0);
 }   

prc=kolpr*100/kolstr;
memset(strsql,'\0',sizeof(strsql));

if(metka_shtrih == 0)
 {
  kom=spis_menu.menu(kom,y,x,NULL,strsql,prc,&K,"",0);
 }
else
 {
  spis_menu.menu(-10,y,x,NULL,strsql,prc,&K,"",0);

vvod_shtrih:;

  if(dirdok_vhk(tipz,dd,md,gd,nomdok->ravno(),nomdokp.ravno(),skl,skl1,kpos.ravno(),metka_prkk,&koloh) != 0)
   metka_shtrih=0;
  goto naz1;
 }

if(kom == -7) //Изменение размера экрана
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
  cena=okrug(cena,okrcn);
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

   if(korsdok(tipz,&dd,&md,&gd,&skl,nomdok,nomnn.ravno(),nomdokp.ravno(),kpos.ravno(),lnds,&kprr,dpnn,mpnn,gpnn) == 0)
    {
     deletdokum(&imaf);
    }
   goto naz2;


  case PLU:   /*Ввести материал как FK2*/
  case FK2:   /*Ввести материал*/

   if(prkor(0,md,gd,blokpid,loginrash) != 0)
    {
     if(kolstr > 0)
       goto naz;
     else
      goto lll;
    }
   nomz.new_plus("");
   kolih=0.;
   vmat(tipz,kpos.ravno(),dd,md,gd,skl,nomdok->ravno(),lnds,0,0.,0,sheta.ravno(),nomdokp.ravno(),kolih,kprr.ravno(),&nomz,pnds);
   deletdokum(&imaf);

   goto naz1;

  case SFK2:   /*Корректировать запись*/
   metka_blok=0;
   if(prkor(0,md,gd,blokpid,loginrash) != 0)
    {
     metka_blok=1;
    }
    
   korznkr(tipz,dd,md,gd,skl,nomdok->ravno(),kodm,nk,kolih,cena,skl1,nomdokp.ravno(),nomkarp[kom],shetz.ravno(),nomz.ravno(),vrem[kom],ktozap[kom],pnds,metka_blok);

   deletdokum(&imaf);
   kom++;
   goto naz1;

  case FK3:   /*Удалить запись в накладной*/
    if(prkor(0,md,gd,blokpid,loginrash) != 0)
     {
      if(kolstr > 0)
        goto naz;
      else
       goto lll;
     }

    if(mu_pvku(ktozap[kom]) != 0) /*проверка принадлежности записи*/
     goto naz;
     
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить запись в документе ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      GDITE();
      if(mdd == 1) //Двойной документ
       {
        int tipzp=1;
        if(tipz == 1)
         tipzp=2;
        if(matudd(1,0,1,tipzp,dd,md,gd,nomdokp.ravno(),skl1,nomkarp[kom],kodm) != 0)
          goto naz;
       }      

      matudd(1,0,1,tipz,dd,md,gd,nomdok->ravno(),skl,nk,kodm);
      deletdokum(&imaf);
     }

    goto naz1;  
    break;

  case SFK3:   /*Удалить весь документ*/
    if(prkor(0,md,gd,blokpid,loginrash) != 0)
      goto naz;

    /*Смотрим заблокированы ли проводки к этому документу*/
    if(iceb_t_pbp(1,dd,md,gd,nomdok->ravno(),skl,tipz,gettext("Удалить документ невозможно!")) != 0)
      goto naz;

    /*узнаём кто сделал запись*/
    sprintf(strsql,"select ktoi from Dokummat where tip=%d and datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",
    tipz,gd,md,dd,skl,nomdok->ravno());
     
    if(readkey(strsql,&row1,&cur1) == 1)
     {
      if(mu_pvku(atoi(row1[0])) != 0) /*проверка возможности корректировки записи*/
       return(1);
     }


    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить документ ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      GDITE();
      deletdokum(&imaf);

      if(udpardok(mdd,tipz,dd,md,gd,skl,nomdok->ravno(),skl1,nomdokp.ravno()) != 0)
            goto naz1;
       
      if(mborvd == 1) /*Снять флаг работы с документом*/
       {
        sprintf(strsql,"%s%d.%d.%d%d%s",imabaz,dd,md,gd,skl,nomdok->ravno());
        sql_flag(&bd,strsql,0,1);
       }
      return(0);
     }
    if(kolstr > 0)
      goto naz;
    else
     goto lll;
    
    break;

  case FK4:   /*Работа с проводками*/
    if(metprov == 1)
     {
      iceb_t_soob(gettext("Для этой операции проводки не выполняются !"));
      goto naz;
     }

    if(SRAV(nomdok->ravno(),"000",0) == 0)
      goto naz;

    if(mvnp != 0 && tipz == 1)
     {
      VVOD SOOB(1);
      SOOB.VVOD_SPISOK_add_MD(gettext("Для операций внутренних перемещений и изменения учётных данных,"));
      SOOB.VVOD_SPISOK_add_MD(gettext("проводки выполняются только для расходных документов"));
      soobshw(&SOOB,stdscr,-1,-1,0,1);
      goto naz;
     }    
    i=dirprmu(dd,md,gd,nomdok->ravno(),skl,kprr.ravno(),kpos.ravno(),tipz,sumkor,lnds,nomdokp.ravno(),skl1,pnds);
    pro=prospr(0,skl,d,m,g,nomdok->ravno(),tipz,lnds,skl1,nomdokp.ravno(),kprr.ravno());
    if(i == 0)
     {
      goto kon;
     }

    if(kolstr > 0)
      goto naz;
    else
     goto lll;


  case FK5:   /*Распечатать*/
    if(tipz == 1)
     dirdok_r1(tipz,dd,md,gd,nomdok->ravno(),&imaf,sumkopl,naimo.ravno(),skl,lnds,ves,mvnp);
    if(tipz == 2)
     dirdok_r2(tipz,dd,md,gd,nomdok->ravno(),&imaf,sumkopl,naimo.ravno(),skl,lnds,ves,mvnp);
    goto naz1; //Только сюда

  case SFK5:   /*Просмотр*/

    sozf(dd,md,gd,skl,nomdok->ravno(),tipz,lnds,&imaf,ves,mvnp,naimo.ravno());

    MENU.VVOD_delete();

    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр накладной"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр налоговой накладной"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр счета"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//3
    clearstr(LINES-1,0);
    i=0;
    while(menu3w(stdscr,&MENU,&i,-1,-1,0) != 0);
   
    oth.clear();
    switch(i)
     {
      case 3:
      case -1:
        goto naz1;

      case 0:
        oth.spis_imaf.plus(imaf.ravno(0));
        oth.spis_naim.plus(gettext("Распечатка накладной"));
        oth.spis_ms.plus(imaf.ravno(0));
        iceb_t_rabfil(&oth,"");                                   
        goto naz1;

      case 1:
        if(dirdok_snn(dd,md,gd,skl,nomdok->ravno(),lnds,imaf.ravno(1)) != 0)
         goto naz1;

        oth.spis_imaf.plus(imaf.ravno(1));
        oth.spis_naim.plus(gettext("Распечатка налоговой накладной"));
        oth.spis_ms.plus(imaf.ravno(1));

        /*узнаём условие продажи*/
        sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=7",gd,skl,nomdok->ravno());
        if(readkey(strsql,&row1,&cur1) == 1)
         {
          polen(row1[0],&vid_dok,4,':');
         }        
        else
          vid_dok.new_plus("");
        iceb_t_rnfxml(&rek_zag_nn);
        if(xml_nn_mat(&pnd,tipz,dd,md,gd,nomdok->ravno(),nomnn.ravno(),md,gd,md,1,vid_dok.ravno(),&imaf_nn_xml) == 0)
         {
          oth.spis_imaf.plus(imaf_nn_xml.ravno());
          oth.spis_naim.plus(gettext("Налоговая накладная в формате XML"));
         } 
        iceb_t_rabfil(&oth,"");                                   
        goto naz1;

      case 2:
        oth.spis_imaf.plus(imaf.ravno(2));
        oth.spis_naim.plus(gettext("Распечатка счёта"));
        oth.spis_ms.plus(imaf.ravno(2));
        iceb_t_rabfil(&oth,"");                                   
        goto naz1;

       default:
        break;
     }

    goto naz1;


  case FK6:  /*Просмотр карточки*/
    if(nk == 0)
     {
      iceb_t_soob(gettext("Запись не привязана к карточке !"));
      goto naz;
     }    

    clear();

    /*Просмотр карточки*/
    dirzkart(skl,nk);

    clear();
    goto naz1;  
  
  case SFK6: /*Ввод и корректировка услуг*/
    dirmusl(dd,md,gd,skl,nomdok->ravno(),pnds);
    deletdokum(&imaf);
    /*Чтобы пересчитала сумму вместе с услугами*/
    goto naz1;  

  case FK7:  /*Запись в карточку материалла*/
    if(prkor(1,md,gd,blokpid,loginrash) != 0)
     goto naz;
   
    kom++;
   
    if(nk == 0)
     {
      int kodv=vmat(tipz,kpos.ravno(),dd,md,gd,skl,nomdok->ravno(),lnds,kodm,cena,0,sheta.ravno(),nomdokp.ravno(),kolih,kprr.ravno(),&nomz,pnds);
      if(nomz.ravno()[0] != '\0' && kodv != 0)
       {
        DANET.VVOD_delete();
        DANET.VVOD_SPISOK_add_MD(gettext("Показать все карточки по этому материалу ?"));

        if(danet(&DANET,2,stdscr) == 1)
         {
          class iceb_tu_str nomer_zak("");
          vmat(tipz,kpos.ravno(),dd,md,gd,skl,nomdok->ravno(),lnds,kodm,cena,0,sheta.ravno(),nomdokp.ravno(),kolih,kprr.ravno(),&nomer_zak,pnds);
         }
       }
      deletdokum(&imaf);
      goto naz1;
     }    

    VV.VVOD_delete();
    VV.VVOD_SPISOK_add_MD(gettext("Дата........."));
    VV.VVOD_SPISOK_add_MD(gettext("Количество..."));

    VV.VVOD_SPISOK_add_data(11);
    VV.VVOD_SPISOK_add_data(20);    
    rsdat(&dp,&mp,&gp,row[0],2);
    if(iceb_t_pbpds(mp,gp) != 0)
     goto naz1;

    kol2=readkolk(skl,nk,dp,mp,gp,nomdok->ravno());  
    if(kol2 == 0.)
     {
      sprintf(strsql,"%d.%d.%d",dp,mp,gp);
      VV.VVOD_SPISOK_zapis_data(0,strsql);
     }
    sprintf(strsql,"%.10g",kol1-kol2);
    VV.VVOD_SPISOK_zapis_data(1,strsql);

nz:;

    helstr(LINES-1,0,"F2/+",gettext("запись"),
    "F10",gettext("выход"),NULL);
    
    kom1=VV.vvod(0,1,1,-1,-1);

    switch(kom1)
     {
      case FK10:
      case ESC:
         goto naz;
      case FK2:
      case PLU:
        GDITE();
        if(rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(0),0) != 0)
         {
          iceb_t_soob(gettext("Не верно введена дата !"));
     	  goto nz;
         }
        
        if(iceb_t_pbpds(m,g) != 0)
         {
          sprintf(strsql,gettext("Дата %d.%dг. заблокирована !"),m,g);
          iceb_t_soob(strsql);
          goto nz;
         }

        if(SRAV1(g,m,d,gd,md,dd) > 0)
         {
          iceb_t_soob(gettext("Дата меньше чем дата документа ! Подтверждение невозможно !"));
          goto nz;
         }

        if(kol1-kol2 - ATOFM(VV.VVOD_SPISOK_return_data(1)) < -0.00001)
         {
          iceb_t_soob(gettext("Не верно введено количество !"));
   	  goto nz;
         }

        ostkar(1,1,gt,31,mt,gt,skl,nk,&ost);

        if(tipz == 2 && ATOFM(VV.VVOD_SPISOK_return_data(1)) - ost.ostg[3] > 0.000000001)
         {
          iceb_t_soob(gettext("В карточке нет такого количества на остатке !"));
	  goto nz;
         }
        /*Проверяем дату первого прихода, расход не может быть раньше прихода*/

        if(tipz == 2)
         {

          prdpp(skl,nk,&dp,&mp,&gp);
          
       	  if(SRAV1(g,m,d,gp,mp,dp) > 0)
       	   {
            VVOD SOOB(1);
            sprintf(strsql,gettext("Дата расхода (%d.%d.%dг.) меньше чем дата первого прихода (%d.%d.%dг."),
       	    d,m,g,dp,mp,gp);
            SOOB.VVOD_SPISOK_add_MD(strsql);
            SOOB.VVOD_SPISOK_add_MD(gettext("Такой расход не может быть !"));
            soobshw(&SOOB,stdscr,-1,-1,0,1);
       	    goto nz;
       	  }
         }

        GDITE();
//        kolih=ATOFM(VV.VVOD_SPISOK_return_data(1));
        kolih=VV.data_ravno_atof(1);
        zapvkr(d,m,g,nomdok->ravno(),skl,nk,dd,md,gd,tipz,kolih,cena);

        if(skl1 != 0 && nomdokp.ravno()[0] != '\0')
         {
          iceb_t_soob(gettext("Не забудте подтвердить соответствующюю запись и в парном документе !"));
         }
        
        goto naz1;
      
      default:
        goto nz;
     }
    goto naz;


  case SFK7:  /*Подтверждение всего документа*/

    if(prkor(1,md,gd,blokpid,loginrash) != 0)
      goto naz;


    podtdok(dd,md,gd,skl,nomdok->ravno(),tipz,skl1,nomdokp.ravno());

    goto naz1;


  case FK8:  /*Меню*/

    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Перейти в другой документ"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Установить/снять блокировку даты"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Расчёт цен"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Отметить запись"));//3
    MENU.VVOD_SPISOK_add_MD(gettext("Найти нужный код материалла"));//4
    if(srtnk == 0)
      MENU.VVOD_SPISOK_add_MD(gettext("Включить сортировку записей в распечатке"));//5
    if(srtnk == 1)
      MENU.VVOD_SPISOK_add_MD(gettext("Выключить сортировку записей в распечатке"));//5

    if(blokpid == 0)
      MENU.VVOD_SPISOK_add_MD(gettext("Заблокировать документ"));//6
    if(blokpid != 0)
      MENU.VVOD_SPISOK_add_MD(gettext("Разблокировать документ"));//6
    MENU.VVOD_SPISOK_add_MD(gettext("Снять подтверждение со всего документа"));//7
    
    if(obzap == 0)
      MENU.VVOD_SPISOK_add_MD(gettext("Включить объединение одинаковых мат-лов в распечатке"));//8
    if(obzap == 1)
      MENU.VVOD_SPISOK_add_MD(gettext("Выключить объединение одинаковых мат-лов в распечатке"));//8
    MENU.VVOD_SPISOK_add_MD(gettext("Ввести сумму или процент корректировки"));//9
    MENU.VVOD_SPISOK_add_MD(gettext("Копировать записи из других документов"));//10
    if(loginrash != 0)
      MENU.VVOD_SPISOK_add_MD(gettext("Снятие метки распечатанного чека"));//11
    if(loginrash == 0)
      MENU.VVOD_SPISOK_add_MD(gettext("Установка метки распечатанного чека"));//11
    MENU.VVOD_SPISOK_add_MD(gettext("Добавить к наименованию материалла"));//12
    MENU.VVOD_SPISOK_add_MD(gettext("Экспортировать кассовый ордер в подсистему \"Учёт кассовых ордеров\""));//13
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//14
   
    clearstr(LINES-1,0);
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 14 :
      case -1 :
        if(kolstr > 0)
          goto naz;
        else
          goto lll;

      case 0 :

        helstr(LINES-1,0,"F10",gettext("выход"),NULL);

        br1.new_plus("");
        VVOD1.VVOD_delete();
        VVOD1.VVOD_SPISOK_add_MD(gettext("Введите номер документа"));

        if(vvod1(&br1,24,&VVOD1,NULL,stdscr,-1,-1) == FK10)
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
  
          br1.new_plus("");
          VVOD1.VVOD_delete();
          VVOD1.VVOD_SPISOK_add_MD(gettext("Введите номер склада"));

          if(vvod1(&br1,20,&VVOD1,NULL,stdscr,-1,-1) == FK10)
            break;

          skl=br1.ravno_atoi();
         }

/*
        printw("skl=%d nomdok=%s\n",skl,nomdok);
        OSTANOV();
*/
        dd=md=0;
        gd=gt;
        goto naz2;


      case 1 :
        iceb_tl_blok();
        if(kolstr > 0)
          goto naz;
        else
         goto lll;

     case 2 :
        rscena();
        if(kolstr > 0)
          goto naz;
        else
         goto lll;

     case 3 :  /*Пометить запись*/
       MENU.VVOD_delete();

       if(voztar == 0)
         MENU.VVOD_SPISOK_add_MD(gettext("Отметить как оборотную тару"));
       if(voztar == 1)
         MENU.VVOD_SPISOK_add_MD(gettext("Снять отметку оборотной тары"));
       MENU.VVOD_SPISOK_add_MD(gettext("Отметить с 0 проц. НДС"));
       sprintf(strsql,gettext("Отметить с %.1f проц. НДС"),pnds);
       MENU.VVOD_SPISOK_add_MD(strsql);
       MENU.VVOD_SPISOK_add_MD(gettext("Отметить с НДС в том числе"));
       MENU.VVOD_SPISOK_add_MD(gettext("Отметить без НДС в том числе"));
       MENU.VVOD_SPISOK_add_MD(gettext("Выход"));
       kom1=0;
       while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);
       if(kom1 == -1 || kom1 == 5)
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
	 nds=pnds;

       if(kom1 == 0)
        sprintf(strsql,"update Dokummat1 \
set voztar=%d where datd='%d-%02d-%02d' and sklad=%d and nomd='%s' and \
kodm=%d and nomkar=%d",
       voztar,gd,md,dd,skl,nomdok->ravno(),kodm,nk);

       if(kom1 == 1 || kom1 == 2)
        {
         if(nk != 0)
          {
           VVOD SOOB(1);
           SOOB.VVOD_SPISOK_add_MD(gettext("Запись привязана к карточке"));
           SOOB.VVOD_SPISOK_add_MD(gettext("Корректировка не возможна !"));
           soobshw(&SOOB,stdscr,-1,-1,0,1);
           goto naz;
          }
         sprintf(strsql,"update Dokummat1 \
set nds=%.10g where datd='%d-%02d-%02d' and sklad=%d and nomd='%s' and \
kodm=%d and nomkar=%d",
         nds,gd,md,dd,skl,nomdok->ravno(),kodm,nk);       
        }

       if(kom1 == 3)
        {
         if(nk != 0)
          {
           VVOD SOOB(1);
           SOOB.VVOD_SPISOK_add_MD(gettext("Запись привязана к карточке"));
           SOOB.VVOD_SPISOK_add_MD(gettext("Корректировка не возможна !"));
           soobshw(&SOOB,stdscr,-1,-1,0,1);
           goto naz;
          }

         sprintf(strsql,"update Dokummat1 \
set mnds=1 where datd='%d-%02d-%02d' and sklad=%d and nomd='%s' and \
kodm=%d and nomkar=%d",
         gd,md,dd,skl,nomdok->ravno(),kodm,nk);       
        }
       if(kom1 == 4)
        {
         if(nk != 0)
          {
           VVOD SOOB(1);
           SOOB.VVOD_SPISOK_add_MD(gettext("Запись привязана к карточке"));
           SOOB.VVOD_SPISOK_add_MD(gettext("Корректировка не возможна !"));
           soobshw(&SOOB,stdscr,-1,-1,0,1);
           goto naz;
          }
         sprintf(strsql,"update Dokummat1 \
set mnds=0 where datd='%d-%02d-%02d' and sklad=%d and nomd='%s' and \
kodm=%d and nomkar=%d",
         gd,md,dd,skl,nomdok->ravno(),kodm,nk);       
        }
/*
       printw("\n%s\n",strsql);
       OSTANOV();
*/
       if(sql_zap(&bd,strsql) != 0)
         msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

       deletdokum(&imaf);

       goto naz1;

      case 4 :

        helstr(LINES-1,0,"F10",gettext("выход"),NULL);

        br1.new_plus("");
        VVOD1.VVOD_delete();
        VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код материалла"));

        if(vvod1(&br1,20,&VVOD1,NULL,stdscr,-1,-1) == FK10)
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

      case 5 :

        deletdokum(&imaf);
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
          
      case 6 : /*Установить / снять блокировку с документа*/


          if(blokpid != iceb_t_getuid())
           if(prkor(0,md,gd,blokpid,loginrash) != 0)
            {
             if(kolstr > 0)
              goto naz;
             else
               goto lll;
            }

          if(blokpid == 0)
           blokpid=iceb_t_getuid();
          else
           blokpid=0;

          memset(strsql,'\0',sizeof(strsql)); 
          sprintf(strsql,"delete from Dokummat2 \
where god=%d and sklad=%d and nomd='%s' and nomerz=12",
          gd,skl,nomdok->ravno());

          sql_zapis(strsql,0,0);

          if(blokpid != 0.)
           {
            sprintf(strsql,"insert into Dokummat2 \
values (%d,%d,'%s',%d,'%d')",
              gd,skl,nomdok->ravno(),12,blokpid);
/*
          printw("strsql=%s\n",strsql);
          OSTANOV();
*/
            sql_zapis(strsql,0,0);
           }
          if(kolstr > 0)
           goto naz;
          else
            goto lll;

       case 7 : /*Снять подтверждение с документа*/
        if(blokpid != iceb_t_getuid())
        if(prkor(0,md,gd,blokpid,0) != 0)
            goto naz;  
        sprintf(strsql,"delete from Zkart where datd='%d-%02d-%02d' \
and nomd='%s' and sklad=%d",gd,md,dd,nomdok->ravno(),skl);
        sql_zapis(strsql,0,0);
        break;  

      case 8 :
        deletdokum(&imaf);
        if(obzap == 0)
         {
          obzap=1;
          if(kolstr > 0)
           goto naz;
          else
            goto lll;
         }
        if(obzap == 1)
          obzap=0;
        if(kolstr > 0)
         goto naz;
        else
          goto lll;


      case 9 :  /*Ввести сумму корректировки*/

        helstr(LINES-1,0,"F10",gettext("выход"),NULL);

        br1.new_plus("");
        VVOD1.VVOD_delete();
        VVOD1.VVOD_SPISOK_add_MD(gettext("Введите сумму или процент (%)"));

        if(vvod1(&br1,20,&VVOD1,NULL,stdscr,-1,-1) == FK10)
          goto naz;
        if(i == ESC)
          goto naz;

        deletdokum(&imaf);
        naimkor.new_plus("");
        sumkor=br1.ravno_atof();
        if(polen(br1.ravno(),&bros,1,'%') == 0)
          sumkor=(suma+sumabn)*bros.ravno_atof()/100.;
        sumkor=okrug(sumkor,0.01);
        if(kolstr > 0)
         goto naz;
        else
          goto lll;


      case 10 : 


        copdok(dd,md,gd,nomdok->ravno(),skl,tipz);

        goto naz1;  

      case 11 : 
        PAR.VVOD_SPISOK_add_MD(gettext("Введите пароль"));
        if(parolv(&PAR,dt,mt,gt,0) != 0)
         {
         if(kolstr > 0)
          goto naz;
         else
           goto lll;
         }
        if(loginrash != 0)        
          sprintf(strsql,"delete from Dokummat2 where god=%d and \
nomd='%s' and sklad=%d and nomerz=%d",gd,nomdok->ravno(),skl,5);

        if(loginrash == 0)        
         {
          sprintf(strsql,"insert into Dokummat2 \
values(%d,%d,'%s',%d,'%d')",gd,skl,nomdok->ravno(),5,iceb_t_getuid());
          loginrash=iceb_t_getuid();
         }

        sql_zapis(strsql,0,0);
        goto naz2;

      case 12 :  //Добавить к наименованию материалла
        if(kolstr == 0)
          goto lll;

        helstr(LINES-1,0,"F10",gettext("выход"),NULL);

        dnaim.new_plus(row[16]);
        VVOD1.VVOD_delete();
        VVOD1.VVOD_SPISOK_add_MD(gettext("Введите дополнение к наименованию материалла"));

        if(vvod1(&dnaim,69,&VVOD1,NULL,stdscr,-1,-1) == FK10)
         goto naz;
        sprintf(strsql,"update Dokummat1 set dnaim='%s' where \
datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' and kodm=%d and nomkar=%d",
        dnaim.ravno_filtr(),gd,md,dd,skl,nomdok->ravno(),kodm,nk);

//        printw("\n%s\n",strsql);
//        OSTANOV();
        
        if(sql_zap(&bd,strsql) != 0)
         msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

        deletdokum(&imaf);
        goto naz1;


      case 13 :  //Экспортировать кассовый ордер
        dirdok_kas_or(tipz,dd,md,gd,sumkopl,kpos.ravno(),naimo.ravno());
        break;
     }
    goto naz1;

  case SFK8:  /*Снятие блокировки по паролю*/
       if(blokpid != 0)
        {

/************

         helstr(LINES-1,0,"F10",gettext("выход"),NULL);

         VVOD PAROL(2);
         PAROL.VVOD_SPISOK_add_ZAG(gettext("Режим снятия блокировки"));
         PAROL.VVOD_SPISOK_add_MD(gettext("Введите пароль"));

         if(vparol(&PAROL,&bros,20,stdscr,-1,-1,1) == FK10)
********************/
         if(parolv(gettext("Для снятия блокировки введите пароль"),0) != 0)
          {
           if(kolstr > 0)
            goto naz;
           else
            goto lll;
          }
          
         sprintf(strsql,"delete from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=12",
         gd,nomdok->ravno(),skl);

         sql_zapis(strsql,0,0);

         blokpid=0;  
        }

        if(kolstr > 0)
         goto naz;
        else
          goto lll;

  case FK9:  /*Меню*/
    if(tipz == 1)
     {
      dirdok_f9p(dd,md,gd,nomdok->ravno(),skl,&sumandspr,&imaf);
      goto naz1;
     }      

    dirdok_f9r(dd,md,gd,nomdok->ravno(),skl,pnds,&imaf,&maskor,nomdokp.ravno(),kpos.ravno());

    goto naz1;

  case SFK9:  /*ввод по штрихкоду*/
    if(prkor(0,md,gd,blokpid,loginrash) != 0)
     {
      if(kolstr > 0)
        goto naz;
      else
       goto lll;
     }

    metka_prkk=0;
    if(tipz == 2)
     {
      DANET.VVOD_delete();
      DANET.VVOD_SPISOK_add_MD(gettext("Привязку к карточкам делать?"));

      if(danet(&DANET,2,stdscr) == 2)
       metka_prkk=1;
     }
    metka_shtrih=1;
    goto vvod_shtrih;
    
  case TAB:  /*Перейти в парный документ*/
    if(skl1 != 0 && nomdokp.ravno()[0] != '\0')
      {
       DANET.VVOD_delete();
       DANET.VVOD_SPISOK_add_MD(gettext("Перейти в парный документ ?"));

       if(danet(&DANET,2,stdscr) == 1)
        {
         if(tipz == 1)     
          tipz=2;

         if(tipz == 2)     
          tipz=1;
         nomdok->new_plus(nomdokp.ravno());
         skl=skl1;
         goto naz2;
        }
      }

    if(kolstr > 0)
     goto naz;
    else
     goto lll;

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
    if(udpardok(mdd,tipz,dd,md,gd,skl,nomdok->ravno(),skl1,nomdokp.ravno()) != 0)
            goto naz1;
    deletdokum(&imaf);
   }
  if(mborvd == 1) /*Снять флаг работы с документом*/
   {
    sprintf(strsql,"%s%d.%d.%d%d%s",imabaz,dd,md,gd,skl,nomdok->ravno());
    sql_flag(&bd,strsql,0,1);
   }
  return(voz);
 }

kon:;

GDITE();

if(mborvd == 1) /*Снять флаг работы с документом*/
 {
  sprintf(strsql,"%s%d.%d.%d%d%s",imabaz,dd,md,gd,skl,nomdok->ravno());
  sql_flag(&bd,strsql,0,1);
 }


if(mdd == 1 && mvnp > 0) /*Двойной документ*/
 {
  if(skl1 != 0)
    bb=provkol(dd,md,gd,skl1,nomdokp.ravno());
  if(skl1 == 0)
    bb=provkol(dd,md,gd,skl,nomdokp.ravno());
  if(fabs(bb-kolmat) > 0.00000001)
   {
    if(skl1 != 0)
     i=skl1;
    if(skl1 == 0)
     i=skl;
              
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("Другое количество материалла во встречном документе !"));
    sprintf(strsql,"%s:%d.%d.%d %s:%d %s:%s",
    gettext("Дата"),dd,md,gd,
    gettext("Склад"),i,
    gettext("Номер документа"),nomdokp.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);

    sprintf(strsql,"%.10g != %.10g",bb,kolmat);
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
   }
 } 

podvdok(dd,md,gd,nomdok->ravno(),skl);
/*Cнятие метки неоплаты для внутренних документов*/
oplmu(dd,md,gd,tipz,nomdok->ravno(),skl,kprr.ravno());

if(metprov == 1)
  return(voz);

if(SRAV(nomdok->ravno(),"000",0) == 0)
  return(voz);

/*Если операция внутренняя и накладная на приход
Проводки выполняются только для расходных накладных*/

if(mvnp != 0 && tipz == 1)
  return(voz);

prospr(3,skl,dd,md,gd,nomdok->ravno(),tipz,lnds,skl1,nomdokp.ravno(),kprr.ravno());


if(tipz == 2)
 {
  //Проверяем подтверждён документ или нет и сделаны к нему проводки или нет
  sprintf(strsql,"select pod,pro from Dokummat where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",
  gd,md,dd,skl,nomdok->ravno());
  if(readkey(strsql,&row,&cur) == 1)
   {
    if(atoi(row[0]) == 1 && atoi(row[1]) == 0)
     {
      char spis_op[1024];
      memset(spis_op,'\0',sizeof(spis_op));
      if(iceb_t_poldan("Коды операций расходов для которых запрещен выход из документа без проводок",spis_op,"matnast.alx") == 0)
       if(proverka(spis_op,kprr.ravno(),0,1) == 0)
        {
         iceb_t_soob(gettext("Не сделаны проводки ! Выход из документа запрещён !"));
         goto naz1;
        }
     }
   }

 }
return(voz);

}

/**************/
/*Выдача шапки*/
/**************/
void sdirnr(double suma,  //Сумма по накладной
double sumabn,  //Сумма по возвратной таре без НДС
double ves, //Вес материалов в документе
int mu,  //Метка введения услуг
const char *naimkor, //Наименование корректировки суммы по документа
double *sumkor, //Сумма корректировки
int blokpid,
int pro,int skl,
const char *naiskl,
const char *kprr,
const char *naimpr,
int tipz,int ktoi,
const char *nomdok,
const char *nomnn,
short dd,short md,short gd,
const char *kpos,
const char *naimo,
int lnds, //Метка НДС
const char *nomdokp,
short mdd,  //0-обычный документ 1-двойной
short mvnp, //0-внешняя 1 -внутреняя 2-изменение стоимости
double kolmat,  //Количество материалла в документе
int kolstr,  //Количество записей в документе без услуг
double *maskor,
int loginrash, //Логин распечатавшего чек
double sumsnds, //Сумма с вложенным НДС
double *sumkopl, //Сумма к оплате
double sumandspr, //Сумма НДС введенная вручную для приходного документа
float pnds, //Процент НДС
int max_dlinna_stroki_menu) /*максиальная длинна строки меню*/
{
double		bb;
double		proc=0.;
char		strsql[512];
SQL_str         row;
SQLCURSOR       cur;
double		suma_nds=0.;  //Сумма ндс
double		suma_snds=0.;  //Сумма с ндс
double		suma_bnds=0.; //Сумма без ндс
double		suma_zap_bnds=0.; //Сумма записей без ндс

clear();

*sumkopl=0.;

move(0,0);
printw("%s:%s %.*s",gettext("Организация"),kpos,iceb_tu_kolbait(40,naimo),naimo);

/*Читаем заблокирован документ для выписки или нет*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and sklad=%d and nomd='%s' and nomerz=4",
gd,skl,nomdok);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  move(0,COLS-2);
  attron(A_BLINK);
  printw("#");
  attroff(A_BLINK);
 }
if(loginrash != 0)
 {
  move(0,COLS-6);
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

if(pro == 0)
 {
  move(1,49);
  attron(A_BLINK);
  printw(gettext("Проводки не сделаны !"));
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

if(maskor != NULL && naimkor[0] != '\0')
   *sumkor=makkorr(suma+sumabn,maskor);

suma_bnds=okrug(suma,okrg1);
bb=suma+(*sumkor);

if(tipz == 1 && sumandspr != 0.)
  suma_nds=sumandspr;
else
  suma_nds=bb*pnds/100.;
bb=bb+suma_nds;
suma_snds=okrug(bb,okrg1);

suma_zap_bnds=okrug(sumabn,okrg1);

move(4,0);
if(lnds == 0 || lnds == 4)
 {
  printw("%s:%.2f",
  gettext("Сумма всех записей"),suma_bnds);
  if(tipz == 1 && sumandspr != 0.)
   {
    attron(A_BLINK);
    printw("/%.2f/",suma_nds);
    attroff(A_BLINK);
   }
  else
   printw("/%.2f/",suma_nds);

  if(suma_zap_bnds > 0)
   printw("%s %.2f/",gettext("Б.НДС"),suma_zap_bnds);

  if(*sumkor != 0)
   {
    if(*sumkor > 0.)
      proc=(*sumkor)*100./suma_bnds;
    if(*sumkor < 0.)
      proc=100.*(*sumkor)/(suma_bnds-(*sumkor)*-1);
    proc=okrug(proc,0.1);
    printw("КС %.2f %.1f%% / ",*sumkor,proc);
   }   
  *sumkopl=suma_snds+suma_zap_bnds;
  printw("%.2f(%.2f)",*sumkopl,sumsnds);

  if(ves > 0.09)
    printw(" %s:%.1f",gettext("Вес"),ves);

 }
else
 {
  attron(A_BLINK);

  *sumkopl=suma_bnds+suma_zap_bnds;
  printw("%s:%.2f(%.2f) %s",
  gettext("Сумма всех записей"),*sumkopl,sumsnds,
  gettext("Б.НДС"));
  attroff(A_BLINK);
  if(ves != 0.)
    printw(" %s:%.1f",
    gettext("Вес"),ves);

  if(*sumkor != 0.)
   {
    proc=(*sumkor)*100./(suma_bnds+suma_zap_bnds);
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

short		y=2;
short           x=COLS-20;

if(COLS-max_dlinna_stroki_menu-4 >= 20)
 {
  mvprintw(y++,x,"%s:",gettext("Наименование полей"));
  mvprintw(y++,x,"1-%s",gettext("код материала"));
  mvprintw(y++,x,"2-%s",gettext("номер карточки"));
  mvprintw(y++,x,"3-%s",gettext("наименование мат."));
  mvprintw(y++,x,"4-%s",gettext("единица измерения"));
  mvprintw(y++,x,"5-%s",gettext("количество"));
  mvprintw(y++,x,"6-%s",gettext("цена"));
  mvprintw(y++,x,"7-%s",gettext("сумма"));
  mvprintw(y++,x,"8-%s",gettext("подтвер-нное кол."));

 }

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("пров."),
"F5",gettext("печать"),
"F6",gettext("карт."),
"F7",gettext("подтв."),
"F8",gettext("меню"),
"F9",gettext("меню"),
"F10",gettext("вых."),NULL);
}

/************************************************/
/*Проверка возможности корректировать накладную*/
/************************************************/
/*Если вернули 0 - можно корректировать
	       1 - нет
*/

short prkor(short mp, //0-все проверять 1-только блокировку накладной
short md,short gd,int blokpid,int loginrash)
{
/*
printw("\nprkor=%d %d.%d %d\n",mp,md,gd,blokpid);
refresh();
*/
if(blokpid != 0 )
 {
  iceb_t_soob(gettext("Документ заблокирован"));
  return(1);
 }

if(mp == 1)
 return(0);

if(loginrash != 0 )
 {
  iceb_t_soob(gettext("Изменения не возможны ! Распечатан чек !"));
  return(1);
 }
 
if(iceb_t_pbpds(md,gd) != 0)
  return(1);

return(0);
}

/********************************/
/*Если файлов нет то создаем их*/
/********************************/
void sozf(short dg,short mg,short gg,int skl,const char *nomdok,
int tipz,int lnds,class SPISOK *imaf,double ves,
short mvnp,  //0-внешняя 1 -внутреняя 2-изменение стоимости
const char *naimo) //Наименование контрагента
{
FILE            *ff;

/*Создание накладной*/

if((ff = fopen(imaf->ravno(0),"r")) == NULL)
 {
  if(errno != ENOENT)
   {
    error_op_nfil(imaf->ravno(0),errno,"");
    return;
   }

  if(errno == ENOENT)
   {


    if((ff = fopen(imaf->ravno(0),"w")) == NULL)
     {
      error_op_nfil(imaf->ravno(0),errno,"");
      return;
     }
//    startfil(ff);
//    rasdok(dg,mg,gg,skl,nomdok,imaf->ravno(0),lnds,0,ff,ves);
    
    class iceb_rnl_c rh;

    rasnak(dg,mg,gg,skl,nomdok,imaf->ravno(0),lnds,&rh.simv_plus,ff,ves);
    fclose(ff);

    iceb_t_ustpeh(imaf->ravno(0),3,&rh.orient);
    iceb_t_rnl(imaf->ravno(0),&rh,&rasnak_sap);

    refresh();
   }
 }
else
  fclose(ff);

/*Создание счета на оплату*/
if((ff = fopen(imaf->ravno(2),"r")) == NULL)
 {
  if(errno != ENOENT)
   {
    error_op_nfil(imaf->ravno(2),errno,"");
    return;
   }

  if(errno == ENOENT)
   {  
    if((ff = fopen(imaf->ravno(2),"w")) == NULL)
     {
      error_op_nfil(imaf->ravno(2),errno,"");
      return;
     }
//    startfil(ff);
//    rasdok(dg,mg,gg,skl,nomdok,imaf->ravno(2),lnds,1,ff,ves);
    class iceb_rnl_c rh;

    rasmushet(dg,mg,gg,skl,nomdok,imaf->ravno(2),lnds,&rh.simv_plus,ff,ves);

    fclose(ff);

    iceb_t_ustpeh(imaf->ravno(2),3,&rh.orient);
    iceb_t_rnl(imaf->ravno(2),&rh,&rasmushet_sap);
   }
 }
else
  fclose(ff);


}

/*********************************/
/*Удаление всех файлов документов*/
/**********************************/

void deletdokum(class SPISOK *imaf)
{
for(int ii=0; ii < imaf->kolih(); ii++)
 unlink(imaf->ravno(ii));

}

/*****************************/
/*Создание файла акта приемки*/
/*****************************/
void sozfap(short dg,short mg,short gg,int skl,const char *nomdok,int lnds,const char *imaf)
{
FILE		*ff;
double		ves=0.;


if((ff = fopen(imaf,"r")) == NULL)
 {
  if(errno != ENOENT)
   {
    error_op_nfil(imaf,errno,"");
    return;
   }

  if(errno == ENOENT)
   {


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
 }
else
  fclose(ff);
}
/****************************/
/* Удаление ДОКУМЕНТА*/
/*****************************/
/*Если вернуди 0- порядок
               1- не удалили
*/

int udpardok(short mdd, //0-обычный документ 1-двойной
int tipz, //1-приход 2-расход
short dd,short md,short gd, //дата документа
int skl,const char *nomdok,
int skl1,
const char *nomdokp)
{

if(mdd == 1) //Двойной документ
 {
  if(tipz == 2) //Расход
   {        
    if(provudvdok(skl1,dd,md,gd,nomdokp,0,0) != 0)
      return(1);

    //Удаляем в приходном
    if(matudd(0,0,0,1,dd,md,gd,nomdokp,skl1,0,0) != 0)
      return(1);

   }
  if(tipz == 1) //Приход
   {
    if(udprgr(ICEB_MP_MATU,dd,md,gd,nomdokp,skl1,2) != 0)
     return(1);
    //Удаляем в расходном
    if(matudd(0,0,0,2,dd,md,gd,nomdokp,skl1,0,0) != 0)
      return(1);
   }
 }

if(udprgr(ICEB_MP_MATU,dd,md,gd,nomdok,skl,tipz) != 0)
 return(1);

//Удаляем документ из которого удаляем
if(matudd(0,0,0,tipz,dd,md,gd,nomdok,skl,0,0) != 0)
  return(1);
return(0);
}
/****************************************************************/
/*загрузка кассового ордера в подсистему "Учёт кассовых ордеров"*/
/****************************************************************/

void dirdok_kas_or(int tipz,
short dd,short md,short gd,
double suma,const char *kontr,const char *naimkont)
{
static class iceb_tu_str kassa("");
static class iceb_tu_str dat1("");
static class iceb_tu_str kodop("");
char strsql[512];
class iceb_tu_str naimkas("");
class iceb_tu_str naimop("");
int kom=0;
int kom1=0;
class iceb_tu_str kod("");
int tipko=0;
if(tipz == 1)
 tipko=2;
if(tipz == 2)
 tipko=1;
 
sprintf(strsql,"%d.%d.%d",dd,md,gd);
dat1.new_plus(strsql);
VVOD MENU(3);
VVOD VV(0);

VV.VVOD_SPISOK_add_MD(gettext("Касса..........."));
VV.VVOD_SPISOK_add_MD(gettext("Дата...(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Код операции...."));

VV.VVOD_SPISOK_add_data(kassa.ravno(),8);
VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(kodop.ravno(),16);

naz:;
clear();

VV.VVOD_delete_ZAG();
VV.VVOD_SPISOK_add_ZAG(gettext("Создание кассовых ордеров"));

sprintf(strsql,"%s:%d.%d.%d%s",
gettext("Дата"),dd,md,gd,gettext("г."));
VV.VVOD_SPISOK_add_ZAG(strsql);

VV.VVOD_SPISOK_add_ZAG(naimkont);

if(naimkas.ravno()[0] != '\0')
  VV.VVOD_SPISOK_add_ZAG(naimkas.ravno());

if(naimop.ravno()[0] != '\0')
  VV.VVOD_SPISOK_add_ZAG(naimop.ravno());

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("загруз."),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

kassa.new_plus(VV.VVOD_SPISOK_return_data(0));
dat1.new_plus(VV.VVOD_SPISOK_return_data(1));
kodop.new_plus(VV.VVOD_SPISOK_return_data(2));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   prosf(strsql);
   goto naz;

  case ESC:
  case FK10:
   return;

  case FK2:
  case PLU:
    break;

  case FK3:

    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка касс"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 8 :
      case -1:
       goto naz;

      case 0 :

        naimkas.new_plus("");
        if(dirkas(1,&kod,&naimkas) == 0)
          VV.VVOD_SPISOK_zapis_data(0,kod.ravno());
        goto naz;

      case 1 :
        naimop.new_plus("");
        if(dirkasop(tipz,1,&kod,&naimop) == 0)
          VV.VVOD_SPISOK_zapis_data(2,kod.ravno());
        goto naz;

     }

    break;
         
  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }
SQL_str row;
SQLCURSOR cur;

//Проверяем кассу
sprintf(strsql,"select kod from Kas where kod=%s",VV.VVOD_SPISOK_return_data(0));
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найдена касса"),VV.VVOD_SPISOK_return_data(0));
  iceb_t_soob(strsql);
  goto naz;
 }

//Проверяем код операции
if(tipz == 1)
 sprintf(strsql,"select naik,shetkor from Kasop1 where kod='%s'",VV.VVOD_SPISOK_return_data(2));
if(tipz == 2)
 sprintf(strsql,"select naik,shetkor from Kasop2 where kod='%s'",VV.VVOD_SPISOK_return_data(2));

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,gettext("Не найден код операции %s !"),VV.VVOD_SPISOK_return_data(2));
  iceb_t_soob(strsql);
  goto naz;
 }

naimop.new_plus(row[0]);
class iceb_tu_str shetk(row[1]);
short dko,mko,gko;
//Проверяем дату
if(rsdat(&dko,&mko,&gko,dat1.ravno(),1) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата !"));
  goto naz;
 }
class iceb_tu_str nomd("");

class lock_tables lt("LOCK TABLES Kasord WRITE,icebuser READ"); //блокировка таблицы

iceb_t_nomnak(gko,kassa.ravno(),&nomd,2,2,0);
zaphkor(0,kassa.ravno(),tipko,dko,mko,gko,nomd.ravno(),shetk.ravno(),kodop.ravno(),naimop.ravno(),"","","","","",0,0,0,"","");

lt.unlock(); //Разблокировка таблицы


zapzkor(0,kassa.ravno(),tipko,dko,mko,gko,nomd.ravno(),kontr,suma,"","");


sprintf(strsql,"%s N%s",gettext("Кассовый ордер"),nomd.ravno());
iceb_t_soob(strsql);

}
/***************************************/
/*Распечатка актов приходных документов*/
/***************************************/
void dirdok_r1_act(short dd,short md,short gd,int skl,const char *nomdok,class SPISOK *imaf,int lnds,double ves)
{
char imaf_sh[64];
class spis_oth oth;
char imaf_act_vr[64];
class iceb_tu_str printer("lpr -r");
class iceb_tu_str printerloc("prterm -r");
iceb_t_poldan("Системный принтер",&printer,"nastsys.alx");
iceb_t_poldan("Приэкранный принтер",&printerloc,"nastsys.alx");

int i1=0;
int kom=0;
VVOD MENU(3);

MENU.VVOD_SPISOK_add_ZAG(gettext("Распечатка актов"));

MENU.VVOD_SPISOK_add_MD(gettext("Распечатка акта приемки"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка акта приемки-передачи"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка акта выполненных работ"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка акта выполненных работ, списания материалов"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//4

naz:;


while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

switch (kom)
 {
  case 4 :
  case -1 :
    return;

  case 0 :
    sozfap(dd,md,gd,skl,nomdok,lnds,imaf->ravno(4));
    if((i1=vibprint()) == 0)
      return;
    if(i1 == 1)
      pehf(imaf->ravno(4),0,printer.ravno());
    if(i1 == 2)
      pehf(imaf->ravno(4),0,printerloc.ravno());
    return;
    
  case 1:
    rasactmu(dd,md,gd,skl,nomdok);
    return;

  case 2: 
   sprintf(imaf_act_vr,"avr%d.lst",getpid());
   rasdok(dd,md,gd,skl,nomdok,imaf_act_vr,lnds,4,NULL,ves);
   oth.clear();
   oth.spis_imaf.plus(imaf_act_vr);
   oth.spis_naim.plus(gettext("Акт выполненых работ"));
   iceb_t_rabfil(&oth,"");
   return;

  case 3:
    if(mat_act_vr(dd,md,gd,skl,nomdok,imaf_sh) == 0)
     {
      oth.clear();
      oth.spis_imaf.plus(imaf_sh);
      oth.spis_naim.plus(gettext("Распечатка акта выполненных работ, списания материалов"));
      iceb_t_rabfil(&oth,"");
     }
    return;
       
  default:
    kom=0;
    goto naz;
 }


}

/***************************************/
/*Распечатка накладных приходных документов*/
/***************************************/
void dirdok_r1_nakl(short dd,short md,short gd,int skl,const char *nomdok,class SPISOK *imaf,int lnds,double ves,int mvnp)
{
//class spis_oth oth;
//char imaf_act_vr[64];
class iceb_tu_str printer("lpr -r");
class iceb_tu_str printerloc("prterm -r");
iceb_t_poldan("Системный принтер",&printer,"nastsys.alx");
iceb_t_poldan("Приэкранный принтер",&printerloc,"nastsys.alx");

int i1=0;
int kom=0;
VVOD MENU(3);

MENU.VVOD_SPISOK_add_ZAG(gettext("Распечатка накладных"));

MENU.VVOD_SPISOK_add_MD(gettext("Распечатка накладной"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка накладной форма М20"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка накладной на мед-препараты"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка накладной с ценами учета/реализации"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка накладной на перемещение алкогольных напитков"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка налоговой накладной"));//5
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//6

naz:;

//clear();

while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

switch (kom)
 {
  case 6:
  case -1:
    return;

  case 0 :
    if((i1=vibprint()) == 0)
      return;
    if(i1 == 1)
      pehf(imaf->ravno(0),0,printer.ravno());
    if(i1 == 2)
      pehf(imaf->ravno(0),0,printerloc.ravno());
    return;
    
  case 1:
    rasdm20(dd,md,gd,skl,nomdok,lnds);
    return;

  case 2: 
   rasmedpr(dd,md,gd,skl,nomdok);
   return;

  case 3:
    rasdok1(dd,md,gd,skl,nomdok,lnds,ves);
    return;

  case 4:
    rasalk(dd,md,gd,skl,nomdok);
    return;

  case 5:
    if(mvnp != 0)
     {
      iceb_t_soob(gettext("Для этой операции налоговая накладная не печатается !"));
      break;
     } 
    if(dirdok_snn(dd,md,gd,skl,nomdok,lnds,imaf->ravno(1)) != 0)
     break;

    if((i1=vibprint()) == 0)
      return;
    if(i1 == 1)
     pehf(imaf->ravno(1),0,printer.ravno());
    if(i1 == 2)
      pehf(imaf->ravno(1),0,printerloc.ravno());
    return;       

  default:
    kom=0;
    goto naz;
 }


}




/*************************************/
/*Распечатки для приходных документов*/
/*************************************/
void dirdok_r1(int tipz,short dd,short md,short gd,const char *nomdok,class SPISOK *imaf,double sumkopl,const char *naimo,int skl,int lnds,double ves,int mvnp)
{
class spis_oth oth;
char strsql[1024];
class iceb_tu_str imaf_spec;
class iceb_tu_str printer("lpr -r");
class iceb_tu_str printerloc("prterm -r");
char imaf_sh[64];

class iceb_tu_str imaf_s_logotip("");
class iceb_tu_str klush_peh("");
//читаем имя файла с логотипом
iceb_t_poldan("Путь на файл с логотипом",&imaf_s_logotip,"nastsys.alx");
if(imaf_s_logotip.ravno()[0] != '\0')
 {
  klush_peh.new_plus("-o logo=");
  klush_peh.plus(imaf_s_logotip.ravno());
 }
iceb_t_poldan("Системный принтер",&printer,"nastsys.alx");
iceb_t_poldan("Приэкранный принтер",&printerloc,"nastsys.alx");
sozf(dd,md,gd,skl,nomdok,tipz,lnds,imaf,ves,mvnp,naimo);

VVOD MENU(3);


MENU.VVOD_SPISOK_add_MD(gettext("Распечатка накладых"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка кассового ордера"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка актов"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка ценников для товаров"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка счёта-фактуры"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка приложения к документу"));//5
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка спецификации"));//6
MENU.VVOD_SPISOK_add_MD(gettext("Удалить распечатки документов"));//7
MENU.VVOD_SPISOK_add_MD(gettext("Сменить принтер"));//8
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//9

naz:;

MENU.VVOD_delete_ZAG();
memset(strsql,'\0',sizeof(strsql));

if(nameprinter.getdlinna() > 1)
 sprintf(strsql,"%s:%s",gettext("Принтер"),nameprinter.ravno());
else
 sprintf(strsql,"%s:%s",gettext("Принтер"),gettext("По умолчанию"));

MENU.VVOD_SPISOK_add_ZAG(strsql);



clearstr(LINES-1,0);
int i1=0;
while(menu3w(stdscr,&MENU,&i1,-1,-1,0) != 0);
switch(i1)
 {
  case FK10:
  case -1:
  case 9:
    return;
    
  case 0:
    dirdok_r1_nakl(dd,md,gd,skl,nomdok,imaf,lnds,ves,mvnp);
    goto naz;


  case 1:
    kasord1(imaf->ravno(3),"",dd,md,gd,"","",sumkopl,naimo,"","","",NULL);
    if((i1=vibprint()) == 0)
      return;
    if(i1 == 1)
     pehf(imaf->ravno(3),0,printer.ravno());
    if(i1 == 2)
     pehf(imaf->ravno(3),0,printerloc.ravno());
    break;      

  case 2:
    dirdok_r1_act(dd,md,gd,skl,nomdok,imaf,lnds,ves);
    goto naz;


  case 3:
    cennikd(dd,md,gd,skl,nomdok);
    break;
                   

  case 4: //Распечатка счёта-фактуры


    mu_shet_fak(dd,md,gd,skl,nomdok,imaf_sh,tipz);

    oth.clear();
    oth.spis_imaf.plus(imaf_sh);
    oth.spis_naim.plus(gettext("Счёт-фактура"));

    iceb_t_rabfil(&oth,klush_peh.ravno());
    break;

   
  case 5:
    raspril(dd,md,gd,skl,nomdok);
    break;
  
  case 6:
   if(rasdoksp(dd,md,gd,skl,nomdok,&imaf_spec) == 0)
    {
     oth.clear();
     oth.spis_imaf.plus(imaf_spec.ravno());
     oth.spis_naim.plus(gettext("Распечатка спецификации"));
     iceb_t_rabfil(&oth,"");
    }
   break;      
  
    
  case 7:
   deletdokum(imaf);
   break;      


  case 8:  //Сменить имя принтера
    mprintcap();
    goto naz;


  default:
    goto naz;
 }

}

/****************************/
/*Распечатка актов расходных документов*/
/*************************/
void dirdok_r2_act(short dd,short md,short gd,const char *nomdok,class SPISOK *imaf,int skl,int lnds,double ves)
{ 
char imaf_act_vr[64];
int		kom;
char imaf_sh[64];
FILE *ff;
class spis_oth oth;
class iceb_tu_str imaf_s_logotip("");
class iceb_tu_str klush_peh("");
//читаем имя файла с логотипом
iceb_t_poldan("Путь на файл с логотипом",&imaf_s_logotip,"nastsys.alx");
if(imaf_s_logotip.ravno()[0] != '\0')
 {
  klush_peh.new_plus("-o logo=");
  klush_peh.plus(imaf_s_logotip.ravno());
 }
VVOD MENU(3);

kom=0;



MENU.VVOD_SPISOK_add_ZAG(gettext("Распечатка актов"));

MENU.VVOD_SPISOK_add_MD(gettext("Распечатка акта списания"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка акта приемки-передачи"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка акта выполненных работ, списания материалов"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка акта выполненных работ"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//4

naz:;

//clear();

while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

switch (kom)
 {
  case 4 :
  case -1 :
    return;

  case 0 :
    if((ff = fopen(imaf->ravno(5),"w")) == NULL)
     {
      error_op_nfil(imaf->ravno(5),errno,"");
      break;
     }
    startfil(ff);
    rasdok(dd,md,gd,skl,nomdok,imaf->ravno(5),lnds,3,ff,ves);
    fclose(ff);
    oth.clear();
    oth.spis_imaf.plus(imaf->ravno(5));
    oth.spis_naim.plus(gettext("Распечатка акта списания"));//8
    iceb_t_rabfil(&oth,"");
    goto naz;

  case 1:
    rasactmu(dd,md,gd,skl,nomdok);
    goto naz;

  case 2: //Распечатка акта выполненных работ, списания материалов
    if(mat_act_vr(dd,md,gd,skl,nomdok,imaf_sh) == 0)
     {
      oth.clear();
      oth.spis_imaf.plus(imaf_sh);
      oth.spis_naim.plus(gettext("Распечатка акта выполненных работ, списания материалов"));
      iceb_t_rabfil(&oth,klush_peh.ravno());
     }
    break;

  case 3: //Распечатка акта выполненных работ
   sprintf(imaf_act_vr,"avr%d.lst",getpid());
   rasdok(dd,md,gd,skl,nomdok,imaf_act_vr,lnds,4,NULL,ves);
   oth.clear();
   oth.spis_imaf.plus(imaf_act_vr);
   oth.spis_naim.plus(gettext("Акт выполненых работ"));
   iceb_t_rabfil(&oth,"");
   break;
    
  default:
    kom=0;
    goto naz;
 }



}

/****************************/
/*Распечатка счетов расходных документов*/
/*************************/
void dirdok_r2_schet(short dd,short md,short gd,const char *nomdok,class SPISOK *imaf,int skl,int lnds,int mvnp,int tipz)
{ 
class spis_oth oth;
int i1=0;
int kom=0;
class iceb_tu_str printer("lpr -r");
class iceb_tu_str printerloc("prterm -r");
iceb_t_poldan("Системный принтер",&printer,"nastsys.alx");
iceb_t_poldan("Приэкранный принтер",&printerloc,"nastsys.alx");

VVOD MENU(3);

kom=0;

MENU.VVOD_SPISOK_add_ZAG(gettext("Распечатка счетов"));

MENU.VVOD_SPISOK_add_MD(gettext("Распечатка счёта"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка счета в две колонки"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка счёта фактуры"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//3

naz:;

//clear();

while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

switch (kom)
 {
  case 3:
  case -1 :
    return;

  case 0 :
    if(mvnp != 0)
     {
      iceb_t_soob(gettext("Для этой операции счета не распечатываются !"));
      break;
     }
    if((i1=vibprint()) == 0)
      return;
    if(i1 == 1)
     pehf(imaf->ravno(2),0,printer.ravno());
    if(i1 == 2)
     pehf(imaf->ravno(2),0,printerloc.ravno());
    goto naz;

  case 1:
    rasdok2(dd,md,gd,skl,nomdok,lnds);
    goto naz;

  case 2: 
    char imaf_sh[64];
    mu_shet_fak(dd,md,gd,skl,nomdok,imaf_sh,tipz);
    oth.clear();
    oth.spis_imaf.plus(imaf_sh);
    oth.spis_naim.plus(gettext("Счёт-фактура"));
    iceb_t_rabfil(&oth,"");
    break;

    
  default:
    kom=0;
    goto naz;
 }

}

/****************************/
/*Распечатка накладных расходных документов*/
/*************************/
void dirdok_r2_nakl(short dd,short md,short gd,const char *nomdok,class SPISOK *imaf,int skl,int lnds,int mvnp,double ves)
{ 
class spis_oth oth;
int i1=0;
int kom=0;
class iceb_tu_str printer("lpr -r");
class iceb_tu_str printerloc("prterm -r");
iceb_t_poldan("Системный принтер",&printer,"nastsys.alx");
iceb_t_poldan("Приэкранный принтер",&printerloc,"nastsys.alx");

VVOD MENU(3);

kom=0;

MENU.VVOD_SPISOK_add_ZAG(gettext("Распечатка накладных"));

MENU.VVOD_SPISOK_add_MD(gettext("Распечатка накладной"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка налоговой накладной"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка товано-транспортной накладной"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка накладной форма М20"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка накладной на перемещение алкогольных напитков"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка накладных с ценами учета/реализации"));//5
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//6

naz:;

//clear();

while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

switch (kom)
 {
  case 6:
  case -1 :
    return;

  case 0 :
    if((i1=vibprint()) == 0)
      return;
    if(i1 == 1)
      pehf(imaf->ravno(0),0,printer.ravno());
    if(i1 == 2)
      pehf(imaf->ravno(0),0,printerloc.ravno());
    goto naz;

  case 1:
    if(mvnp != 0)
     {
      iceb_t_soob(gettext("Для этой операции налоговая накладная не печатается !"));
      break;
     } 
    dirdok_snn(dd,md,gd,skl,nomdok,lnds,imaf->ravno(1));
    if((i1=vibprint()) == 0)
      return;
    if(i1 == 1)
     pehf(imaf->ravno(1),0,printer.ravno());
    if(i1 == 2)
      pehf(imaf->ravno(1),0,printerloc.ravno());
    goto naz;

  case 2: 
    if(rasttn(dd,md,gd,nomdok,skl,&oth) == 0)
      iceb_t_rabfil(&oth,"");
    
    break;

  case 3:
    rasdm20(dd,md,gd,skl,nomdok,lnds);
    break;    

  case 4:
    rasalk(dd,md,gd,skl,nomdok);
    break;

  case 5:
   rasdok1(dd,md,gd,skl,nomdok,lnds,ves);
   break;
    
  default:
    kom=0;
    goto naz;
 }

}



/*************************************/
/*Распечатки для расходных документов*/
/*************************************/
void dirdok_r2(int tipz,short dd,short md,short gd,const char *nomdok,class SPISOK *imaf,double sumkopl,const char *naimo,int skl,int lnds,double ves,int mvnp)
{
class spis_oth oth;
char strsql[1024];
class iceb_tu_str imaf_spec;
class iceb_tu_str printer("lpr -r");
class iceb_tu_str printerloc("prterm -r");
iceb_t_poldan("Системный принтер",&printer,"nastsys.alx");
iceb_t_poldan("Приэкранный принтер",&printerloc,"nastsys.alx");

sozf(dd,md,gd,skl,nomdok,tipz,lnds,imaf,ves,mvnp,naimo);

VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(strsql);

MENU.VVOD_SPISOK_add_MD(gettext("Распечатка накладных"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка счетов"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка кассового ордера"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка пропуска"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка актов"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка накладной на мед-препараты"));//5
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка ценников для товаров"));//6
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка приложения к документу"));//7
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка спецификации"));//8
MENU.VVOD_SPISOK_add_MD(gettext("Удалить распечатки документов"));//9
MENU.VVOD_SPISOK_add_MD(gettext("Сменить принтер"));//10
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//11

naz:;

MENU.VVOD_delete_ZAG();
memset(strsql,'\0',sizeof(strsql));
if(nameprinter.getdlinna() > 1)
 sprintf(strsql,"%s:%-.*s",gettext("Принтер"),iceb_tu_kolbait(20,nameprinter.ravno()),nameprinter.ravno());
else
 sprintf(strsql,"%s:%-.*s",gettext("Принтер"),iceb_tu_kolbait(20,gettext("По умолчанию")),gettext("По умолчанию"));

MENU.VVOD_SPISOK_add_ZAG(strsql);


clearstr(LINES-1,0);
int i1=0;
while(menu3w(stdscr,&MENU,&i1,-1,-1,0) != 0);
switch(i1)
 {
  case FK10:
  case -1:
  case 11:
    return;
    
  case 0:
    dirdok_r2_nakl(dd,md,gd,nomdok,imaf,skl,lnds,mvnp,ves);
    goto naz;

  case 1:
    dirdok_r2_schet(dd,md,gd,nomdok,imaf,skl,lnds,mvnp,tipz);
    goto naz;
    
  case 2:
    kasord1(imaf->ravno(3),"",dd,md,gd,"","",sumkopl,naimo,"","","",NULL);
    if((i1=vibprint()) == 0)
      return;
    if(i1 == 1)
     pehf(imaf->ravno(3),0,printer.ravno());
    if(i1 == 2)
     pehf(imaf->ravno(3),0,printerloc.ravno());
    break;      

  case 3:
    rasprop(dd,md,gd,skl,nomdok);
    break;

  case 4:
    dirdok_r2_act(dd,md,gd,nomdok,imaf,skl,lnds,ves);
    goto naz;
    
  case 5:
    rasmedpr(dd,md,gd,skl,nomdok);
    break;

  case 6:
    cennikd(dd,md,gd,skl,nomdok);
    break;
                   
  case 7: 
    raspril(dd,md,gd,skl,nomdok);
    break;

  case 8:
   if(rasdoksp(dd,md,gd,skl,nomdok,&imaf_spec) == 0)
    {
     oth.clear();
     oth.spis_imaf.plus(imaf_spec.ravno());
     oth.spis_naim.plus(gettext("Распечатка спецыфикации"));
     iceb_t_rabfil(&oth,"");
    }
   break;      
  
    
  case 9:
   deletdokum(imaf);
   break;      

  case 10:  //Сменить имя принтера
    mprintcap();
    goto naz;

  default:
    goto naz;
 }

}
/*******************************/
/*создание налоговой накладной*/
/******************************/
int dirdok_snn(short dg,short mg,short gg,
int skl,
const char *nomdok,
int lnds,
const char *imaf)
{
FILE *ff=NULL;
/*Создание налоговой накладной*/
if((ff = fopen(imaf,"r")) == NULL)
 {
  if(errno != ENOENT)
   {
    error_op_nfil(imaf,errno,"");
    return(1);
   }

  if(errno == ENOENT)
   {  
    return(rasnaln(dg,mg,gg,skl,nomdok,imaf,lnds));
   }
 }
else
 fclose(ff);
return(0);
}
/*****************************************************/
/******************************************************/
void dirdok_f9p(short dd,short md,short gd,const char *nomdok,int skl,double *sumandspr,class SPISOK *imaf)
{
int kom1=0;
class iceb_tu_str br1("");
class VVOD MENU(3);
class VVOD VVOD1(2);
char strsql[512];

MENU.VVOD_delete();

MENU.VVOD_SPISOK_add_MD(gettext("Оформить возврат"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Дооценить документ"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Ввести сумму НДС для документа"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Переценить документ на заданный процент"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Загрузить записи из файла"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//5

clearstr(LINES-1,0);
kom1=0;
while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0) ;

switch (kom1)
 {

  case 5:
  case -1:
   break;
  case 0 :  /*Оформить возврат*/
    rozkor(nomdok,dd,md,gd,skl);
    break;

  case 1 :  /*Дооценить документ*/
    dooc(dd,md,gd,nomdok,skl);
    break;

  case 2 :  //Ввести сумму НДС для приходного документа
    helstr(LINES-1,0,"F10",gettext("выход"),NULL);

    br1.new_plus("");

    if(*sumandspr != 0.)
     {
      br1.new_plus(*sumandspr);
     }
    VVOD1.VVOD_delete();
    VVOD1.VVOD_SPISOK_add_MD(gettext("Введите сумму НДС"));

    if(vvod1(&br1,16,&VVOD1,NULL,stdscr,-1,-1) == FK10)
      return;

    *sumandspr=br1.ravno_atof();
    deletdokum(imaf);
    if(*sumandspr != 0.)
     sprintf(strsql,"replace into Dokummat2 values (%d,%d,'%s',6,'%s')",
     gd,skl,nomdok,br1.ravno());
    else
     sprintf(strsql,"delete from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=6",
     gd,skl,nomdok);
     

    
    if(sql_zap(&bd,strsql) != 0)
     msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

    break;

  case 3 :  /*Переценить накладную на заданный процент*/
   perecenp(dd,md,gd,nomdok,skl);
   deletdokum(imaf);
   break;

  case 4:
    impmatost(1,skl,dd,md,gd,nomdok);
    return;   
 }


}



/*******************************************/
/********************************************/
void dirdok_f9r(short dd,short md,short gd,const char *nomdok,int skl,float pnds,class SPISOK *imaf,double **maskor,const char *nomdokp,const char *kpos)
{

class iceb_tu_str naimkor("");
int kom1=0;
int i=0;
class spis_oth oth;
char strsql[1024];

class VVOD MENU(3);
class VVOD DANET(1);

MENU.VVOD_delete();
MENU.VVOD_SPISOK_add_MD(gettext("Переценить документ на заданный процент"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Выбрать вид корректировки суммы по документу"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать остатки"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Снять привязку к карточкам"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Снять блокировку с документа для выписки"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Остатки по складу в документ"));//5
MENU.VVOD_SPISOK_add_MD(gettext("Списать материалы на изделия"));//6
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//7

clearstr(LINES-1,0);
kom1=0;

while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

switch (kom1)
 {

  case 7 :
  case -1 :
    return;

  case 0 :  /*Переценить накладную на заданный процент*/

   perecen(dd,md,gd,nomdok,skl,pnds);
   deletdokum(imaf);
   return;
   
  case 1 :  /*Выбрать вид снижек*/
   MENU.VVOD_delete();

   naimkor.new_plus("");
   if(makkor(&MENU,1,"",maskor,"makkor.alx") != 0)
    break;       

   helstr(LINES-1,0,"F10",gettext("выход"),NULL);
   i=0;
   while(menu3w(stdscr,&MENU,&i,-1,-1,0) != 0);

   if( i == -1 || i == FK10)
    {
     return;
    }
   deletdokum(imaf);

   naimkor.new_plus(MENU.VVOD_SPISOK_return_MD(i));
   makkor(&MENU,2,naimkor.ravno(),maskor,"makkor.alx");
   return;
   
  case 2 :  /*Получить остатки по группе*/
    MENU.VVOD_delete();

    MENU.VVOD_SPISOK_add_MD(gettext("Распечатать остатки по складу"));
    MENU.VVOD_SPISOK_add_MD(gettext("Распечатать остатки по группе"));
    MENU.VVOD_SPISOK_add_MD(gettext("Распечатать остатки по материалу"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0); 
    switch (kom1)
     {
      case -1 :
        break;

      case 0 :
       oth.clear();
       if(rasoskr(&oth) == 0)
        iceb_t_rabfil(&oth,"");
       break;

      case 1 :
       rasgrup(1); 
       break;

      case 2 :
       rasmat(); 
       break;
  
     } 
    return;
    
  case 3 :  /*Снять привязку к карточкам*/
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Будут удалены все подтверждающие записи в карточках !"));
    DANET.VVOD_SPISOK_add_MD(gettext("         Снять привязку ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
     {
     return;
     }

    sprintf(strsql,"delete from Zkart where datd='%d-%02d-%02d' \
and nomd='%s' and sklad=%d",gd,md,dd,nomdok,skl);
    sql_zapis(strsql,0,0);

    sprintf(strsql,"update Dokummat1 set nomkar=0 where \
datd='%d-%02d-%02d' and nomd='%s' and sklad=%d",gd,md,dd,nomdok,skl);
    sql_zapis(strsql,0,0);
    return;
    
  case 4 :  /*Снять блокировку документа для выписки*/

     sprintf(strsql,"delete from Dokummat2 where god=%d and \
sklad=%d and nomd='%s' and nomerz=4",gd,skl,nomdok);
    sql_zapis(strsql,0,0);

    return;
    
  case 5:
    muso(dd,md,gd,nomdok,skl,kpos,nomdokp);
    return;      
  case 6:
    dirdokummat4(dd,md,gd,skl,nomdok);
    return;
 }
}
/*************************************************/
/*Ввод штрихкода со сканера*/
/**************************************/
int dirdok_vhk(int tipz,short dd,short md,short gd,const char *nomdok,const char *nomdokp,int sklad,int skladp,const char *kontr,
int metka_prkk, /*0-привязывать 1-нет*/
int *kol_oh)
{
class iceb_tu_str shtrihkod("");
class VVOD VVOD1(2);
int kom=0;
int tipzp=0;

if(tipz == 1)
 {
  tipzp=2;

 }
else
 {
  tipzp=1;
  if(metka_prkk == 0)
   VVOD1.VVOD_SPISOK_add_ZAG(gettext("С привязкой к карточкам"));
  else
   VVOD1.VVOD_SPISOK_add_ZAG(gettext("Без привязки к карточкам"));
 }

VVOD1.VVOD_SPISOK_add_MD(gettext("Введите штрихкод"));
class spis_oth oth;

oth.spis_imaf.plus(IMA_FIL_ERROR_SHTRIH);
oth.spis_naim.plus(gettext("Протокол ошибок"));
oth.spis_ms.plus(IMA_FIL_ERROR_SHTRIH);

naz:;

VVOD1.VVOD_delete_ZAG();

if(tipz == 2)
 {
  if(metka_prkk == 0)
   VVOD1.VVOD_SPISOK_add_ZAG(gettext("С привязкой к карточкам"));
  else
   VVOD1.VVOD_SPISOK_add_ZAG(gettext("Без привязки к карточкам"));
 }

if(*kol_oh > 0)
 {
  class iceb_tu_str strout(gettext("Количество ошибок"));
  strout.plus(":");
  strout.plus(*kol_oh);
  VVOD1.VVOD_SPISOK_add_ZAG(strout.ravno());
 }

helstr(LINES-1,0,"F10",gettext("выход"),"F5",gettext("ошибки"),NULL);

kom=vvod1(&shtrihkod,64,&VVOD1,NULL,stdscr,-1,-1);

switch(kom)
 {
  case ENTER:
   break;
  case FK5:
    iceb_t_rabfil(&oth,"");
    return(0); /*Для того чтобы обновился экран*/
  case ESC:
  case FK10:
    return(1);
  default:
   goto naz;
 };
 
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select kodm,cenapr,ei from Material where strihkod='%s'",shtrihkod.ravno());
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %s %s",gettext("Не найден материал со штрихкодом"),shtrihkod.ravno(),gettext("в справочнике материалов"));
  dirdok_ohout(tipz,dd,md,gd,nomdok,strsql);
  *kol_oh+=1;
  beep();
//  iceb_t_soob(strsql);
  goto naz;
 }


class iceb_tu_str kodmat(row[0]);
class iceb_tu_str cena(row[1]);
class iceb_tu_str ei(row[2]);
int nomkart=0;

if(ei.getdlinna() <= 1)
 {
  sprintf(strsql,"%s %d %s",gettext("Для материала с кодом"),kodmat.ravno_atoi(),gettext("не введена единица измерения"));
  dirdok_ohout(tipz,dd,md,gd,nomdok,strsql);
  *kol_oh+=1;
  beep();
//  iceb_t_soob(strsql);
  goto naz;
 } 

class iceb_tu_str str_error;

if(tipz == 2 && metka_prkk == 0)
 {
  int kolkart=0;
  /*проверяем есть ли на складе хоть одна карточка с этим материалом*/
  sprintf(strsql,"select nomk from Kart where sklad=%d and kodm=%d",sklad,kodmat.ravno_atoi());
  if((kolkart=readkey(strsql)) <= 0)
   {
    sprintf(strsql,"%s %d %s %d",gettext("На складе"),sklad,gettext("нет ни одной карточки с кодом материала"),kodmat.ravno_atoi());
    str_error.plus(strsql);
   }

  if(kolkart > 0)
   {
    double ost=0.;
    if((ost=ostkar1(kodmat.ravno_atoi(),sklad,dd,md,gd)) < 1.)
     {
      sprintf(strsql,"%s %d %.10g",gettext("Нет остатка по коду материала"),kodmat.ravno_atoi(),ost);

      str_error.plus_v_spisok(strsql);
     }
   } 

  if(kolkart == 1)
   {
    nomkart=atoi(row[0]);
   }
 }

if(cena.ravno_atof() == 0.)
 {
  sprintf(strsql,"%s %d",gettext("Внимание! Не введена цена для материала с кодом"),kodmat.ravno_atoi());
  str_error.plus_v_spisok(strsql);
 }

if(str_error.getdlinna() > 1)
 {
//  iceb_t_soob(str_error.ravno());
  dirdok_ohout(tipz,dd,md,gd,nomdok,str_error.ravno());
  *kol_oh+=1;
  
 } 
/*проверяем может уже введён*/
sprintf(strsql,"select nomkar from Dokummat1 where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' and kodm=%d and cena=%.10g",
gd,md,dd,sklad,nomdok,kodmat.ravno_atoi(),cena.ravno_atof());
if(readkey(strsql,&row,&cur) > 0) /*есть в документе*/
 {
  sprintf(strsql,"update Dokummat1 set kolih=kolih+1. where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' and kodm=%d and cena=%.10g and nomkar=%d",
  gd,md,dd,sklad,nomdok,kodmat.ravno_atoi(),cena.ravno_atof(),atoi(row[0]));

  sql_zapis(strsql,0,0);  

  if(skladp != 0)
   {
    sprintf(strsql,"update Dokummat1 set kolih=kolih+1. where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' and kodm=%d and cena=%.10g",
    gd,md,dd,skladp,nomdokp,kodmat.ravno_atoi(),cena.ravno_atof());

    sql_zapis(strsql,0,0);  
   }
     
 }
else
 {
  sprintf(strsql,"insert into Dokummat1 (datd,sklad,nomd,nomkar,kodm,kolih,cena,ei,ktoi,vrem,tipz) \
values ('%04d-%02d-%02d',%d,'%s',%d,%d,%.10g,%.10g,'%s',%d,%ld,%d)",
  gd,md,dd,sklad,nomdok,nomkart,kodmat.ravno_atoi(),1.,cena.ravno_atof(),ei.ravno_filtr(),iceb_t_getuid(),time(NULL),tipz);    

  sql_zapis(strsql,0,0);

  if(skladp != 0)
   {
    sprintf(strsql,"insert into Dokummat1 (datd,sklad,nomd,nomkar,kodm,kolih,cena,ei,ktoi,vrem,tipz) \
values ('%04d-%02d-%02d',%d,'%s',%d,%d,%.10g,%.10g,'%s',%d,%ld,%d)",
    gd,md,dd,skladp,nomdokp,nomkart,kodmat.ravno_atoi(),1.,cena.ravno_atof(),ei.ravno_filtr(),iceb_t_getuid(),time(NULL),tipzp);    

    sql_zapis(strsql,0,0);
   }
 }

return(0);
}
/*************************************/
/*вывод в файл сообщения об ошибке*/
/*************************************/
void dirdok_ohout(int tipz,short dd,short md,short gd,const char *nomdok,const char *strout)
{

class iceb_fopen ffoh;
if(ffoh.start(IMA_FIL_ERROR_SHTRIH,"a") != 0)
  return; 

struct tm *bf;
time_t vrem;
vrem=time(NULL);
bf=localtime(&vrem);

fprintf(ffoh.ff,"\n%s %d.%d.%d%s  %s:%02d:%02d\n",gettext("Дата"),bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,gettext("г."),gettext("Время"),bf->tm_hour,bf->tm_min);
fprintf(ffoh.ff,"%s:%d %s\n",gettext("Оператор"),iceb_t_getuid(),iceb_t_getfioop());

if(tipz == 1)
 fprintf(ffoh.ff,"%s %s %s %s %d.%d.%d\n",gettext("Приход"),gettext("Документ"),nomdok,gettext("Дата документа"),dd,md,gd);
if(tipz == 2)
 fprintf(ffoh.ff,"%s %s %s %s %d.%d.%d\n",gettext("Расход"),gettext("Документ"),nomdok,gettext("Дата документа"),dd,md,gd);

fprintf(ffoh.ff,"%s\n",strout);


}
