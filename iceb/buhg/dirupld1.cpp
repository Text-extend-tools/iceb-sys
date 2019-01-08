/*$Id: dirupld1.c,v 5.3 2013/10/04 07:25:01 sasa Exp $*/
/*14.07.2015	17.04.2003	Белых А.И.	dirupld1.c
Работа с записями документа
*/
#include        <math.h>
#include	"buhg.h"

void		dirupld1_duplds(short,short,short,const char*,const char*,const char*,const char*,const char*,const char*,const char*,SPISOK*,masiv_din_double*,double,double,double);
int		vvodtop(short,short,short,short,const char*,const char*,short,const char*,const char*,int);
double	        dirupld1_uplast1(short,short,short,int,const char*,const char*,const char*,const char*,double,SPISOK*,masiv_din_double*);
void	        dirupld1_uplast(short,short,short,int,const char*,const char*,const char*,const char*,double,SPISOK*,masiv_din_double*,SPISOK*,masiv_din_double*);
void            dirupld1_potspv(short,short,short,const char*,const char*,const char*,const char*,int);
int upl_sp_top(int metka,short dd,short md,short gd,const char *podr,const char *nomd,const char*,const char*,int nom_zap_v_dok,int nom_zap);
double uplztf(short dd,short md,short gd,int podr,const char *nom_dok);
void dirupldok2a(short dd,short md,short gd,const char *nom_dok,int kp);

extern short	startgodupl; //Стартовый год для "Учета путевых листов"

void dirupld1(short dd,short md,short gd, //Дата
class iceb_tu_str *nomd, //Номер документа
class iceb_tu_str *podr) //Подразделение
{
class spis_oth oth;
char		strsql[1024];
SQL_str		row;
long		pozz=0;
int		prc=0;
long		kolstr=0,kolpr=0;
int		K=0;
int		i=0;
int		kom=0;
short           X=0,Y=6;
class iceb_tu_str naimpodr("");
class iceb_tu_str kodvod("");
class iceb_tu_str naimvod("");
class iceb_tu_str kodavt("");
class iceb_tu_str naimavt("");
double		ztf=0.; //Затраты топлива фактические
char		metkazap[5];
double		kolih=0.;
int		nomer=0;
double		kolihost=0.;
double          kolihdok=0.;
short		tipz=0;
double		sumaspis=0.;
class iceb_t_mstr spis_menu(0);
class iceb_tu_str imaf_prot;
int nom_zap=0;
SQLCURSOR 	curr;

GDITE();


sprintf(strsql,"ostva%d.lst",getpid());
imaf_prot.plus(strsql);

oth.spis_imaf.plus(imaf_prot.ravno());
oth.spis_naim.plus(gettext("Протокол хода расчёта остака топлива"));
oth.spis_ms.plus(imaf_prot.ravno());



//Читаем наименование подразделения
sprintf(strsql,"select naik from Uplpodr where kod=%s",podr->ravno());
if(sql_readkey(&bd,strsql,&row,&curr) == 1)
  naimpodr.new_plus(row[0]);
  

//Читаем шапку документа
sprintf(strsql,"select ka,kv,nz from Upldok where datd='%d-%d-%d' and kp=%s and nomd='%s'",
gd,md,dd,podr->ravno(),nomd->ravno());

if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  sprintf(strsql,"%s !",gettext("Не найдена шапка документа"));
  iceb_t_soob(strsql);
  return;
 }

kodavt.new_plus(row[0]);
kodvod.new_plus(row[1]);
//ztf=atof(row[2])+atof(row[3])+atof(row[4])+atof(row[5]);
ztf=uplztf(dd,md,gd,podr->ravno_atoi(),nomd->ravno());
nom_zap=atoi(row[2]);

//Читаем наименование автомобиля
sprintf(strsql,"select naik from Uplavt where kod=%s",kodavt.ravno());
if(sql_readkey(&bd,strsql,&row,&curr) == 1)
 naimavt.new_plus(row[0]);

//Читаем водителя
sprintf(strsql,"select naik from Uplouot where kod=%s",kodvod.ravno());
if(sql_readkey(&bd,strsql,&row,&curr) == 1)
  naimvod.new_plus(row[0]);

//Создаем список остатков топлива
class SPISOK KTC;
class masiv_din_double OST;

class iceb_fopen fil;
fil.start(imaf_prot.ravno(),"w");

kolihost=ostva(dd,md,gd,kodvod.ravno(),kodavt.ravno(),nom_zap,&KTC,&OST,fil.ff);

fil.end();

VVOD DANET(1);

naz1:;

GDITE();

class SPISOK KTCPL(0); // Список полученного код топлива-цена
class masiv_din_double OSTPL(0); //Количество полученного топлива

short           kls=LINES-1-8;
short		nomz[kls];
sumaspis=kolihdok=0.;


sprintf(strsql,"select * from Upldok1 where datd='%d-%d-%d' and kp=%s and nomd='%s' order by tz asc",
gd,md,dd,podr->ravno(),nomd->ravno());
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
if(kolstr == 0)
 {

lll:;

  dirupld1_duplds(dd,md,gd,nomd->ravno(),podr->ravno(),naimpodr.ravno(),kodvod.ravno(),naimvod.ravno(),kodavt.ravno(),naimavt.ravno(),&KTC,&OST,ztf,kolihost,sumaspis);

  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == FK4 || K == SFK2 || K == FK3 || K == ENTER)
      goto lll;

  if(K == KEY_RESIZE)
    goto naz1;
    
  goto vper;

 }
else
 {
  kolihdok=0.;
  while(cur.read_cursor(&row) != 0)
   {

    kolih=atof(row[9]);
    

    if(row[7][0] == '1') //Приход
     {
      if((nomer=KTCPL.find(row[8])) < 0)
         KTCPL.plus(row[8]);
      OSTPL.plus(kolih,nomer);
     }
    else
     {
      sumaspis+=kolih;      
      kolih*=-1;      
     }

    kolihdok+=kolih;
   }
 }

naz:;


cur.poz_cursor(pozz);
kolpr=pozz;
spis_menu.spis_str.free_class();
for(i=0; i<=kls-1;)
 {
  if(cur.read_cursor(&row) == 0)
    break;
   
  if(row[7][0] == '1')
   strcpy(metkazap,"+");

  if(row[7][0] == '2')
   strcpy(metkazap,"-");

  nomz[i]=atoi(row[3]);

  kolih=atof(row[9]);
  
  sprintf(strsql,"%-4s|%8.3f|%s|%.*s",
  row[8],kolih,metkazap,
  iceb_tu_kolbait(20,row[10]),row[10]);

  spis_menu.spis_str.plus(strsql);
  i++;


 }

if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;
    
if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

dirupld1_duplds(dd,md,gd,nomd->ravno(),podr->ravno(),naimpodr.ravno(),kodvod.ravno(),naimvod.ravno(),kodavt.ravno(),naimavt.ravno(),&KTC,&OST,ztf,kolihost+kolihdok,sumaspis);

prc=kolpr*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,Y,X,NULL,strsql,prc,&K,"",0);


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
  goto naz;
 }

if(kom >= 0 )
 {
  if(SRAV(gettext("Конец списка"),spis_menu.spis_str.ravno(kom),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz; /*Стали на конец списка*/

  cur.poz_cursor(pozz+kom);
  cur.read_cursor(&row);
  tipz=atoi(row[7]);  
 }

vper:;

switch (K)
 {
  case FK1:  /*Получить подсказку*/
    GDITE();
    iceb_t_pdoc("upl2_3.txt");
    goto naz1;

  case FK10: /*Выход*/
  case ESC : /*Выход*/
     unlink(imaf_prot.ravno());
     break;


  case ENTER: //Корректировка записи
     if(tipz == 2)
       upl_sp_top(1,dd,md,gd,podr->ravno(),nomd->ravno(),kodavt.ravno(),kodvod.ravno(),nomz[kom],nom_zap);
     else
       vvodtop(1,dd,md,gd,podr->ravno(),nomd->ravno(),nomz[kom],kodavt.ravno(),kodvod.ravno(),nom_zap);
     goto naz1;

  case FK2: //Ввод получения топлива
  case PLU: //Ввод получения топлива
    if(iceb_t_pbpds(md,gd) != 0)
     goto naz;
    

   vvodtop(0,dd,md,gd,podr->ravno(),nomd->ravno(),0,kodavt.ravno(),kodvod.ravno(),nom_zap);
   goto naz1;
   
  case FK3:  //Удаление записи
    if(iceb_t_pbpds(md,gd) != 0)
     goto naz;

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить запись в документе ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      goto naz;

    sprintf(strsql,"delete from Upldok1 where datd='%d-%d-%d' and kp=%d and nomd='%s' and nzap=%d",
    gd,md,dd,podr->ravno_atoi(),nomd->ravno(),nomz[kom]);      
    sql_zapis(strsql,0,0);
    goto naz1;


  case FK4:  //Ввод объектов списания топлива
    dirupldok2a( dd,md,gd,nomd->ravno(),podr->ravno_atoi());
    goto naz1;

  case FK6:  //Списание топлива

    if(iceb_t_pbpds(md,gd) != 0)
     goto naz;
    dirupld1_uplast(dd,md,gd,nom_zap,nomd->ravno(),podr->ravno(),kodavt.ravno(),kodvod.ravno(),ztf,&KTC,&OST,&KTCPL,&OSTPL);
    goto naz1;

  case FK7:  //перенос остатков топлива с предыдущего водителя

    if(iceb_t_pbpds(md,gd) != 0)
     goto naz;
    dirupld1_potspv(dd,md,gd,podr->ravno(),nomd->ravno(),kodavt.ravno(),kodvod.ravno(),nom_zap);
    goto naz1;

  case FK8:  //ввод записи на списание топлива
  case OTN:
    upl_sp_top(0,dd,md,gd,podr->ravno(),nomd->ravno(),kodavt.ravno(),kodvod.ravno(),nomz[kom],nom_zap);
    goto naz1;

  case FK9:
   iceb_t_rabfil(&oth,"");
   goto naz1;
   
  default:
    if(kolstr > 0)
      goto naz;
    else
      goto lll;
 }

GDITE();
//Проверка полноты списания топлива
//provpst(dd,md,gd,nomd->ravno(),podr->ravno());
dirupldok2a_provpst(dd,md,gd,nomd->ravno(),podr->ravno_atoi());

  
}

/*****************************/
/* Шапка                     */
/*****************************/

void dirupld1_duplds(short dd,short md,short gd, //Дата
const char *nomd, //Номер документа
const char *podr, //Подразделение
const char *naimpodr, //Наименование подразделения
const char *kodvod,   //Код водителя
const char *naimvod,  //Наименование водителя
const char *kodavt,   //Код автомобиля
const char *naimavt, //наименование автомобиля
class SPISOK *KTC,   //Список код топлива-цена
class masiv_din_double *OST, //Массив остатков
double ztf, //Затраты топлива фактические
double kolost, //Количество на остатке
double sumaspis) //Сумма списанная по документа
{
int  Y=0;

clear();
mvprintw(Y++,0,"%s:%d.%d.%d %s:%s",
gettext("Дата документа"),
dd,md,gd,
gettext("Номер документа"),
nomd);

mvprintw(Y++,0,"%s:%s %s",gettext("Водитель"),kodvod,naimvod);

if(podr[0] != '\0')
 mvprintw(Y++,0,"%s:%s %s",gettext("Подразделение"),podr,naimpodr);

if(kodavt[0] != '\0')
 mvprintw(Y++,0,"%s:%s %s",gettext("Автомобиль"),kodavt,naimavt);

mvprintw(Y++,0,"%s:%.3f %s:%.10g",gettext("Затраты топлива"),ztf,
gettext("Списано"),sumaspis);

if(fabs(ztf-sumaspis) > 0.009)
 {
  attron(A_BLINK);
  printw(" %.2f",ztf-sumaspis);
  attroff(A_BLINK);
 }
mvprintw(Y++,0,"%s:%.3f",gettext("Остаток"),kolost);

int kolsp=KTC->kolih();
Y=0;
int X=COLS-20;

for(int i=0; i < kolsp; i++)
  mvprintw(Y++,X,"%-*s %6.3f",
  iceb_tu_kolbait(4,KTC->ravno(i)),
  KTC->ravno(i),
  OST->ravno(i));

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("счета"),
"F6",gettext("спис."),
"F7",gettext("остат."),
"F8/-",gettext("запись"),
"F9",gettext("протокол"),
"F10",gettext("вых."),NULL);



}
/****************************************/
/*Автоматическое списание топлива       */
/****************************************/

void dirupld1_uplast(short dd,short md,short gd, //Дата
int nom_zap, //порядковый номер документа в один и тотже день
const char *nomd, //Номер документа
const char *podr, //Подразделение
const char *kodavt, //Код автомобиля
const char *kodvod, //Код водителя
double sumasp, //Сумма которую надо списать
SPISOK *KTC,masiv_din_double *OST,
SPISOK *KTCPL,masiv_din_double *OSTPL)
{
char strsql[512];
double spisano=0;

//Удаляем все расходы в документе
sprintf(strsql,"delete from Upldok1 where datd='%d-%d-%d' and kp=%d and nomd='%s' and tz=2",
gd,md,dd,atoi(podr),nomd);

if(sql_zapis(strsql,0,0) != 0)
  return;

spisano=dirupld1_uplast1(dd,md,gd,nom_zap,nomd,podr,kodavt,kodvod,sumasp,KTC,OST);
dirupld1_uplast1(dd,md,gd,nom_zap,nomd,podr,kodavt,kodvod,spisano,KTCPL,OSTPL);

}

/*******************************/
/*Запись расходов из массива   */
/*******************************/

double	dirupld1_uplast1(short dd,short md,short gd, //Дата
int nom_zap, //порядковый номер документа в один и тотже день
const char *nomd, //Номер документа
const char *podr, //Подразделение
const char *kodavt, //Код автомобиля
const char *kodvod, //Код водителя
double sumasp, //Сумма которую надо списать
class SPISOK *KTC,class masiv_din_double *OST)
{
double          kolih=0.;
double		kolihz=0.;
class iceb_tu_str kodtp("");

//printw("dirupld1_uplast1\n");
//refresh();


int kolzap=KTC->kolih();
int i;


for(i=0; i<kolzap;i++)
 {
  if(sumasp < 0.0009)
   return(0.);

  kodtp.new_plus(KTC->ravno(i));

  kolih=OST->ravno(i);
  
  if(kolih > sumasp)
   {
    kolihz=sumasp;
    sumasp=0.;
   }
  else
   {
    kolihz=kolih;
    sumasp-=kolih;
   }


  if(kolihz > 0.0009)
   {
    upl_zapvdok(dd,md,gd,atoi(podr),nomd,atoi(kodavt),atoi(kodvod),0,2,kodtp.ravno(),kolihz,"",0,nom_zap);
   }
 }

return(sumasp);

}

/*************************************************/
/*Перенос остатков топлива с предыдущего водителя*/
/*************************************************/

void    dirupld1_potspv(short dd,short md,short gd,
const char *podr,
const char *nomd,
const char *kodavt,
const char *kodvod,
int nom_zap)
{
char		strsql[512];
SQL_str		row;
SQLCURSOR curr;
class iceb_tu_str kodvodp("");


//Определяем самый последний документ по данному автомобилю
if(nom_zap == 0)
 sprintf(strsql,"select kv from Upldok1 where \
datd < '%d-%d-%d' and ka=%s order by datd desc limit 1",
 gd,md,dd,kodavt);
else
 sprintf(strsql,"select kv from Upldok1 where \
datd = '%d-%d-%d' and ka=%s and nz < %d order by nz desc limit 1",
 gd,md,dd,kodavt,nom_zap);

if(sql_readkey(&bd,strsql,&row,&curr) > 0)
 {
  kodvodp.new_plus(row[0]); 
 }
else
 {
  //Читаем может есть сальдо
  sprintf(strsql,"select kv from Uplsal where god=%d and ka=%s",startgodupl,kodavt);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    kodvodp.new_plus(row[0]); 

 }
if(SRAV(kodvod,kodvodp.ravno(),0) == 0)
  return;
if(kodvodp.ravno()[0] == '\0')
  return;

//Создаем список остатков топлива
SPISOK KTC;
masiv_din_double OST;

ostva(dd,md,gd,kodvodp.ravno(),kodavt,nom_zap,&KTC,&OST,NULL);

int kolsp=KTC.kolih();
if(kolsp == 0)
  return;
  

for(int i=0; i < kolsp;i++)
 {



  upl_zapvdok(dd,md,gd,atoi(podr),nomd,atoi(kodavt),atoi(kodvod),kodvodp.ravno_atoi(),1,KTC.ravno(i),OST.ravno(i),gettext("Перенос остатка"),atoi(kodavt),nom_zap);

 }



}
