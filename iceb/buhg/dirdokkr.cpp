/*$Id: dirdokkr.c,v 5.47 2013/09/26 09:43:29 sasa Exp $*/
/*13.07.2015	25.10.2002	Белых А.И.	dirdokkr.c
Работа с документом командировочных расходов
*/
#include        <errno.h>
#include	"buhg.h"

void	dirdokkrsp(short dd,short md,short gd,const char *nomdok,const char *kont,const char *naikont,const char *vkom,const char *naivkom,double sumadok,double isnds,double i_strah_sbor,int mprov);
void	vvodkr(short,short,short,const char*);
int     dirdokkkruz(short,short,short,const char*,const char*,const char*,int);
int     dirdokkkrud(short,short,short,const char*,const char*);
int     dirprkr(short,short,short,short,short,short,const char*,const char*,const char*,const char*,double,double);
void    ukrku(short,const char*);


void dirdokkr(short dd,short md,short gd,class iceb_tu_str *nomdok)
{
int		pozz=0;
int		kom=0;
class iceb_t_mstr spis_menu(0);
char		strsql[1024];
SQL_str		row,row1;
int		kolstr;
int		K=0;
short		mpn=0; //Метка пустого документа
int		kolpr=0;
int 		i=0;
int		prc=0;
short           XP=0,YP=4;
class iceb_tu_str naimkr("");
double		kolih=0.;
double		cena=0.;
double		suma=0.;
double          sumadok=0.;
double		snds=0.;
double		isnds=0.;
double i_strah_sbor=0.;
class iceb_tu_str kont("");
class iceb_tu_str naikont("");
class iceb_tu_str vkom("");
class iceb_tu_str naivkom("");
class iceb_tu_str kodkr("");
class iceb_tu_str shet("");
int		mprov=0;  //0-проводки не сделаны 1-сделаны
short		dao=0,mao=0,gao=0;
class iceb_tu_str nomao("");
double strah_sbor=0.;
SQLCURSOR curr;
naz2:;


sprintf(strsql,"select kont,mprov,vkom,nomao,datao from Ukrdok where god=%d and nomd='%s'",
gd,nomdok->ravno());
if(sql_readkey(&bd,strsql,&row,&curr) == 1)
 {
  kont.new_plus(row[0]);
  mprov=atoi(row[1]);
  vkom.new_plus(row[2]);
  nomao.new_plus(row[3]);
  rsdat(&dao,&mao,&gao,row[4],2);
  
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kont.ravno());
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
   naikont.new_plus(row[0]);

  sprintf(strsql,"select naik from Ukrvkr where kod='%s'",vkom.ravno());
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
   naivkom.new_plus(row[0]);
  
 }

naz1:;

clear();
GDITE();

mpn=0;
sumadok=0.;
isnds=0.;
i_strah_sbor=0.;

short           kls=LINES-YP-3;
int		nomz[kls];

sprintf(strsql,"select * from Ukrdok1 where datd='%04d-%02d-%02d' and \
nomd='%s' order by kodr asc",
gd,md,dd,nomdok->ravno());

SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
if(kolstr == 0)
 {

lll:;
  dirdokkrsp(dd,md,gd,nomdok->ravno(),kont.ravno(),naikont.ravno(),vkom.ravno(),naivkom.ravno(),sumadok,isnds,i_strah_sbor,mprov);

  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  mpn=0;
   
  if(K == FK4 || K == SFK2 || K == FK3 )
      goto lll;

  if(K == KEY_RESIZE)
    goto naz1;
    
  goto vper;
 }
else
 {
  printw("%s\n",gettext("Вычисляем сумму по документу"));
  refresh();
  while(cur.read_cursor(&row) != 0)
   {
    sumadok+=atof(row[5]);
    isnds+=atof(row[7]);
    i_strah_sbor+=atof(row[16]);
   }    
  printw(gettext("Конец расчёта.\n"));
  refresh();
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
  mpn=1;
  naimkr.new_plus("");
  //Читаем наименование командировочных расходов
  sprintf(strsql,"select naim from Ukrkras where kod=%s",row[2]);
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
    naimkr.new_plus(row1[0]);

  kolih=atof(row[4]);
  cena=atof(row[5]);
  snds=atof(row[7]);        
  strah_sbor=atof(row[16]);
  suma=cena+snds+strah_sbor;
  nomz[i]=atoi(row[10]);

  sprintf(strsql,"%-2s|%-*.*s|%-*s|%-*s|%7.7g|%6.6g|%6.6g|%6.6g|%.6g",
  row[2],
  iceb_tu_kolbait(20,naimkr.ravno()),iceb_tu_kolbait(20,naimkr.ravno()),naimkr.ravno(),
  iceb_tu_kolbait(4,row[3]),row[3],
  iceb_tu_kolbait(4,row[6]),row[6],
  kolih,cena,snds,strah_sbor,suma);

  spis_menu.spis_str.plus(strsql);
  i++;

 }

if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;
    
if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

dirdokkrsp(dd,md,gd,nomdok->ravno(),kont.ravno(),naikont.ravno(),vkom.ravno(),naivkom.ravno(),sumadok,isnds,i_strah_sbor,mprov);

prc=kolpr*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,YP,XP,NULL,strsql,prc,&K,"",0);


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

if(kom >= 0)
 {
  class iceb_tu_str str_vib(spis_menu.spis_str.ravno(kom));
  if(SRAV(gettext("Конец списка"),str_vib.ravno(),0) == 0)
     goto naz;

  if(pozz+kom >= kolstr)
   goto naz; /*Стали на конец списка*/

  polen(str_vib.ravno(),&kodkr,1,'|');
  polen(str_vib.ravno(),&shet,3,'|');
 }

vper:;

switch (K)
 {
  case FK1: //Помощь
    GDITE();
    iceb_t_pdoc("ukr2_2.txt");
    clear();
    goto naz1;

  case FK10: /*Выход*/
  case ESC : /*Выход*/
     break;

  case ENTER:
     goto naz1;

  case SFK1: //Работа с шапкой документа
     vdkr(&dd,&md,&gd,nomdok);
     goto naz2;
     
  case FK2: //Ввод новой записи
    if(dirdokkrpiz(dd,md,gd) != 0)
     goto naz1;

    vvodkr(dd,md,gd,nomdok->ravno());
    goto naz1;

  case SFK2: //Корректировка записи
    vvodkr1(dd,md,gd,nomdok->ravno(),kodkr.ravno(),shet.ravno(),1,nomz[kom]);
    goto naz1;

  case FK3: //Удаление записи
    dirdokkkruz(dd,md,gd,nomdok->ravno(),kodkr.ravno(),shet.ravno(),nomz[kom]);
    goto naz1;

  case SFK3: //Удаление всего документа
    if(dirdokkkrud(dd,md,gd,nomdok->ravno(),vkom.ravno()) == 0)
      return;
    goto naz1;      

  case FK4: //Работа с проводками
    dirprkr(dd,md,gd,dao,mao,gao,nomdok->ravno(),nomao.ravno(),vkom.ravno(),kont.ravno(),sumadok,isnds);
    goto naz2; //Чтобы перечитала метку выполнения проводок

  case FK5: //Распечатать командировочное удостоверение
    ukrku(gd,nomdok->ravno());
    goto naz1; //В документе может не быть записей
 
  default:
    if(kolstr > 0)
      goto naz;
    else
      goto lll;
 }

prosprkr(dd,md,gd,nomdok->ravno(),1);

sprintf(strsql,"update Ukrdok set mpodt=%d where god=%d and nomd='%s'",
mpn,gd,nomdok->ravno());
if(sql_zap(&bd,strsql) != 0)
  msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);

}

/*******************************/
/*Шапка                        */
/*******************************/

void	dirdokkrsp(short dd,short md,short gd,
const char *nomdok,
const char *kont,
const char *naikont,
const char *vkom,
const char *naivkom,
double sumadok,
double isnds,
double i_strah_sbor,
int mprov)
{
clear();

printw("%s:%s %s:%d.%d.%d%s",gettext("Номер документа"),nomdok,
gettext("Дата"),dd,md,gd,
gettext("г."));

if(mprov == 0)
 {
  attron(A_BLINK);
  printw("  %s",gettext("Проводки не сделаны !"));
  attroff(A_BLINK);
 }

mvprintw(1,0,"%s:%s %s",gettext("Контрагент"),
kont,naikont);

mvprintw(2,0,"%s:%s %s",gettext("Вид командировки"),
vkom,naivkom);

mvprintw(3,0,"%s:%.2f+%.2f+%.2f=%.2f",gettext("Сумма"),sumadok,isnds,i_strah_sbor,sumadok+isnds+i_strah_sbor);

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("пров."),
"F5",gettext("печать"),
"F10",gettext("вых."),NULL);

}

/********************************/
/*Удалить запись                */
/********************************/

int  dirdokkkruz(short dd,short md,short gd,
const char *nomdok,
const char *kodkr,
const char *shet,
int nomz)
{
char	strsql[512];

if(dirdokkrpiz(dd,md,gd) != 0)
 return(1);

VVOD DANET(1);
DANET.VVOD_SPISOK_add_MD(gettext("Удалить запись в документе ? Вы уверены ?"));

if(danet(&DANET,2,stdscr) == 2)
  return(1);

sprintf(strsql,"delete from Ukrdok1 where datd='%04d-%d-%d' and nomd='%s' \
and kodr=%s and shet='%s' and nomz=%d",
gd,md,dd,nomdok,kodkr,shet,nomz);
//printw("\n%s\n",strsql);
//OSTANOV();

if(sql_zap(&bd,strsql) != 0)
  msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);

return(0);

}
/********************************************/
/*Проверка возможности изменений в документе*/
/********************************************/
int	dirdokkrpiz(short dd,short md,short gd)
{
if(iceb_t_pbpds(md,gd) != 0)
  return(1);

return(0);

}

/*********************************************/
/*Удаление всего документа                   */
/*********************************************/

int  dirdokkkrud(short dd,short md,short gd,
const char *nomdok,
const char *vkom)
{
char	strsql[512];

if(dirdokkrpiz(dd,md,gd) != 0)
 return(1);

/*Смотрим заблокированы ли проводки к этому документа*/
if(iceb_t_pbp(8,dd,md,gd,nomdok,0,0,gettext("Удалить документ невозможно!")) != 0)
 return(1);

VVOD DANET(1);
DANET.VVOD_SPISOK_add_MD(gettext("Удалить документ ? Вы уверены ?"));

if(danet(&DANET,2,stdscr) == 2)
 return(1);

GDITE();
if(udprgr(ICEB_MP_UKR,dd,md,gd,nomdok,0,0) != 0)
 return(1);
 
sprintf(strsql,"delete from Ukrdok1 where datd='%04d-%d-%d' and nomd='%s'",
gd,md,dd,nomdok);
if(sql_zap(&bd,strsql) != 0)
 {
  msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);
  return(1);
 }

sprintf(strsql,"delete from Ukrdok where god=%d and nomd='%s'",
gd,nomdok);
if(sql_zap(&bd,strsql) != 0)
  msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);

return(0);

}

