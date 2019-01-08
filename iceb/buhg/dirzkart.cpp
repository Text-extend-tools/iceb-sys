/* $Id: dirzkart.c,v 5.78 2013/10/04 07:25:02 sasa Exp $ */
/*31.12.2017    18.04.1997      Белых А.И.      dirzkart.c
Просмотр всех записей по конкретному материалу на конкретной карточке
в материальном учете
*/
#include <math.h>
#include        "buhg.h"

void 		korkm(int,int,int,class SPISOK *rowk);

void dmtrs(int skl,int nk,short dn,short mn,short gn,
short dk,short mk,short gk,short mos,const char *naim,int kgrm,const char *naimg,
const char *naiskl,class SPISOK *rowk,short metpoi,short denv,short mesv,short godv,
const char *innom,const char *rnd,const char *nomz,
class ostatokt *ost,
short deng,short mesg,short godg);

void pnpz(int,int,int,const char*);
void raszkar(SQLCURSOR *cur,int skl,int nk,short dn,short mn,short gn,short dk,short mk,short gk,const char *nomdp,const char *kontrp,const char *kodop,int tipzz);
void sliqnie(int skl,int nkn,int kodm,const char *shu,double cena);

extern double   okrcn;
extern class iceb_tu_str shrt;    /*Счёта розничной торговли*/
extern short	startgod; /*Стартовый год просмотров*/
	
void            dirzkart(int skl,int nk)
{
class iceb_tu_str kod("");
class iceb_tu_str naimk("");
class iceb_tu_str naim("");
class ostatokt ost;
SQL_str         row,row1;
class SPISOK rowk;
char		strsql[1024];
int		kolstr;
long    	pozz,pozz1;
struct  tm      *bf;
int             K;
short           kom;
int             i;
short           x=0,y=9;
short           d,m,g;
short           dp,mp,gp;
short           dn,mn,gn;
short           dk,mk,gk;
int             kkkd;
short           mos=1; /*метка вывода остатков*/
short           tz=0;
double          kolm=0;
class iceb_tu_str datdok("");
class iceb_tu_str kor(""); /*Код контрагента*/
class iceb_tu_str kop(""); /*Код операции*/
class iceb_tu_str naiskl("");
class iceb_tu_str naimg("");
int		kgrm;
class iceb_tu_str ei("");
int		kodm;
double		kolih,cena;
class iceb_tu_str cenaprod("");
class iceb_tu_str bros("");
class iceb_tu_str tabl("");
class iceb_tu_str shu("");
class iceb_tu_str nomdp("");
char		dat1[32];
char		dat2[32];
class iceb_tu_str kontrp("");
class iceb_tu_str kodop("");
int		tipzz;
class iceb_tu_str datp("");
int		kom1,kom2;
short		metpoi; /*0-поиск не заказан 1-заказан*/
class iceb_tu_str nomdok("");
class iceb_tu_str ktozz("");
class iceb_tu_str nomkarp(""); //номер карточки парного документа
short denv=0,mesv=0,godv=0;
class iceb_tu_str innom("");
class iceb_tu_str rnd(""); //Регистрационный номер мед. препарата
short		regh=0; //0-основной 1-показ дополнений к наименованию
class iceb_tu_str dnaim("");
class iceb_tu_str nomz(""); //Номер заказа
short deng,mesg,godg;
class iceb_t_mstr spis_menu(0);
short		mspz; /*метка соответствия первой записи*/
char strmenu[1024];

short dt,mt,gt;
poltekdat(&dt,&mt,&gt);
SQLCURSOR cur;
SQLCURSOR cur1;

//Создаем класс меню
VVOD DANET(1);
VVOD VV(0);
VVOD MENU(3);
VVOD SOOB(1);

naz3:;

metpoi=0;

pozz=0;

memset(dat1,'\0',sizeof(dat1));
memset(dat2,'\0',sizeof(dat2));
tipzz=0;
clear();
printw("\n\n%s\n",gettext("Готовим просмотр карточки"));
GDITE();
kom=0;

/*Узнаем наименование склада материалла*/
sprintf(strsql,"select naik from Sklad where kod=%d",
skl);
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s-%s %d!",__FUNCTION__,gettext("Не найден склад"),skl);
  iceb_t_soob(strsql);
  return;
 }
naiskl.new_plus(row[0]);


/*Читаем реквизиты карточки материалла*/
sprintf(strsql,"select * from Kart where sklad=%d and nomk=%d",skl,nk);
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s-%s %d %s %d!",__FUNCTION__,gettext("Не найдена карточка материалла"),nk,gettext("на складе"),skl);
  iceb_t_soob(strsql);
  return;
 }

rowk.free_class();
for(i=0;i<19;i++)
  rowk.plus(row[i]);

ei.new_plus(rowk.ravno(4));
kodm=rowk.ravno_atoi(2);
cena=rowk.ravno_atof(6);
cena=okrug(cena,okrcn);
shu.new_plus(rowk.ravno(5));
rsdat(&denv,&mesv,&godv,rowk.ravno(14),2);
innom.new_plus(rowk.ravno(15));
rnd.new_plus(rowk.ravno(16));
nomz.new_plus(rowk.ravno(17));
rsdat(&deng,&mesg,&godg,rowk.ravno(18),2);

/*Узнаем наименование материалла*/
sprintf(strsql,"select kodgr,naimat  from Material where kodm=%d",kodm);
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {

  sprintf(strsql,"%s-%s %d %s!",__FUNCTION__,gettext("Не найден код материалла"),kodm,gettext("в справочнике материалов"));
  iceb_t_soob(strsql);
  return;
 }

kgrm=atoi(row[0]);
naim.new_plus(row[1]);


/*Узнаем наименование группы материалла*/
if(kgrm != 0)
 {
  sprintf(strsql,"select naik from Grup where kod=%d",kgrm);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s-%s %d %s!",__FUNCTION__,gettext("Не найдена группа материалла !"),kgrm,gettext("в справочнике групп"));
    iceb_t_soob(strsql);
    return;
   }
 }

naimg.new_plus(row[0]);


dn=1; mn=1; 
gn=startgod;
if(gn == 0)
 gn=gt;
sprintf(dat1,"%d.%d.%d",dn,mn,gn);
/*Текущая дата*/
gk=gt; dk=dt; mk=mt;
dpm(&dk,&mk,&gk,5); /*Установить последний день месяца*/
sprintf(dat2,"%d.%d.%d",dk,mk,gk);

naz1:;

GDITE();
short           kls=LINES-12;
time_t		vrem[kls];
unsigned int ktozap[kls];

ostkar(dn,mn,gn,dk,mk,gk,skl,nk,&ost);

K=0;

sprintf(strsql,"select * from Zkart where sklad=%d and nomk=%d and \
datdp >= '%04d-%02d-%02d' and datdp <= '%04d-%02d-%02d' order by datdp asc",
skl,nk,gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
if(kolstr == 0)
 {

lll:;

  dmtrs(skl,nk,dn,mn,gn,dk,mk,gk,mos,naim.ravno(),kgrm,naimg.ravno(),naiskl.ravno(),&rowk,metpoi,denv,mesv,godv,innom.ravno(),rnd.ravno(),nomz.ravno(),&ost,deng,mesg,godg);

  SOOB.VVOD_delete();

  if(metpoi == 0)
     sprintf(strsql,gettext("Нет записей в карточке за %d.%dг. !"),mn,gn);
  if(metpoi == 1)
     sprintf(strsql,gettext("Не найдено ни одной записи !"));

  SOOB.VVOD_SPISOK_add_MD(strsql);
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == FK3 || K == SFK3 || K == FK5 || K == TAB)
      goto lll;
  if(K == ENTER)
      goto lll;
  if(K == KEY_RESIZE)
   goto naz1;
   
  goto vper;
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

  kolih=atof(row[6]);
  vrem[i]=atol(row[9]);

  /*Читаем код контрагента, код операции*/
  
  sprintf(strsql,"select tip,kodop,kontr from Dokummat \
where datd='%s' and sklad=%d and nomd='%s'",
  row[4],skl,row[2]);
  tz=0;
  kop.new_plus("");
  kor.new_plus("");
  
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,gettext("Не найден документ ! Дата-%s Склад-%d Кар.-%d Док.-%s"),row[4],skl,nk,row[2]);
    iceb_t_soob(strsql);
   }
  else
   {
    tz=atoi(row1[0]);
    kop.new_plus(row1[1]);
    kor.new_plus(row1[2]);    
   }

  if(metpoi == 1)
   if(poizkart(nomdp.ravno(),row[2],kontrp.ravno(),kor.ravno(),kodop.ravno(),kop.ravno(),tipzz,tz) != 0)
    continue;
  
  nomkarp.new_plus("");
  
  if(kor.ravno()[0] == '0' && kor.ravno()[1] == '0' && kor.ravno()[2] == '-')
   {
    //читаем номер встречной карточки
    sprintf(strsql,"select nomkarp from Dokummat1 \
where datd='%s' and sklad=%d and nomd='%s' and kodm=%d and nomkar=%d",
    row[4],skl,row[2],kodm,nk);
    if(sql_readkey(&bd,strsql,&row1,&cur1) > 0)
      nomkarp.new_plus(row1[0]);
   }

  bros.new_plus("+");
  if(tz == 2)
   {
    kolih*=-1;
    bros.new_plus("-");
    
   }  
  rsdat(&d,&m,&g,row[4],2);
  rsdat(&dp,&mp,&gp,row[3],2);

  memset(strmenu,'\0',sizeof(strmenu));
  if(regh == 0)  
    sprintf(strmenu,"%02d.%02d.%d|%02d.%02d.%d|%-*s|%-*s|%-*s|%10.10g|\
%s|%.10g|%s|%s",
    dp,mp,gp,d,m,g,
    iceb_tu_kolbait(8,row[2]),row[2],
    iceb_tu_kolbait(3,kop.ravno()),kop.ravno(),
    iceb_tu_kolbait(8,kor.ravno()),kor.ravno(),
    kolih,bros.ravno(),atof(row[7]),row[8],nomkarp.ravno());

  if(regh == 1)
   {
    dnaim.new_plus("");
    sprintf(strmenu,"select dnaim from Dokummat1 where sklad=%d and \
nomd='%s' and kodm=%d and nomkar=%d and datd='%s'",skl,row[2],kodm,nk,row[4]);

    if(sql_readkey(&bd,strsql,&row1,&cur1) > 0)
      dnaim.new_plus(row1[0]);

    sprintf(strmenu,"%02d.%02d.%d|%02d.%02d.%d|%-*s|%-s",
    dp,mp,gp,d,m,g,
    iceb_tu_kolbait(8,row[2]),row[2],
    dnaim.ravno());
   }

  spis_menu.spis_str.plus(strmenu);
  i++;

  if(pozz1 == 1) /*Первая запись удовлетворяет условиям поиска*/
     mspz=1;

 }
if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i == 0)
 {
  if(kolstr > 0 && pozz > 0)
   {
    pozz--;
    goto naz;
   }
  goto lll;
 }  

if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

dmtrs(skl,nk,dn,mn,gn,dk,mk,gk,mos,naim.ravno(),kgrm,naimg.ravno(),naiskl.ravno(),&rowk,metpoi,denv,mesv,godv,innom.ravno(),rnd.ravno(),nomz.ravno(),&ost,deng,mesg,godg);

move(8,0);
printw(gettext("1-дата под. 2-дата док. 3-н.док. 4-код п/р 5-код пос. 6-кол. 7-п/р 8-цена"));

if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

kkkd=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,kkkd,&K,"",0);


if(kom == -7) //Изменение размеров экрана
  goto naz1;
  
if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
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

    /*Читаем код контрагента, код операции*/
    tz=0;
    kop.new_plus("");
    kor.new_plus("");
  
    sprintf(strsql,"select tip,kodop,kontr from Dokummat \
where datd='%s' and sklad=%d and nomd='%s'",
    row[4],skl,row[2]);

    if(sql_readkey(&bd,strsql,&row1,&cur1) > 0)
     {
      tz=atoi(row1[0]);
      kop.new_plus(row1[1]);
      kor.new_plus(row1[2]);    
     }

  
    if(metpoi == 1)
    if(poizkart(nomdp.ravno(),row[2],kontrp.ravno(),kor.ravno(),kodop.ravno(),kop.ravno(),tipzz,tz) != 0)
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

    /*Читаем код контрагента, код операции*/
    tz=0;
    kop.new_plus("");
    kor.new_plus("");
  
    sprintf(strsql,"select tip,kodop,kontr from Dokummat \
where datd='%s' and sklad=%d and nomd='%s'",
    row[4],skl,row[2]);

    if(sql_readkey(&bd,strsql,&row1,&cur1) > 0)
     {
      tz=atoi(row1[0]);
      kop.new_plus(row1[1]);
      kor.new_plus(row1[2]);    
     }

    if(metpoi == 1)
    if(poizkart(nomdp.ravno(),row[2],kontrp.ravno(),kor.ravno(),kodop.ravno(),kop.ravno(),tipzz,tz) != 0)
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
  goto naz;

 }

if(kom == -4) /*Страница вверх*/
 {
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

    /*Читаем код контрагента, код операции*/
    tz=0;
    kop.new_plus("");
    kor.new_plus("");
  
    sprintf(strsql,"select tip,kodop,kontr from Dokummat \
where datd='%s' and sklad=%d and nomd='%s'",
    row[4],skl,row[2]);

    if(sql_readkey(&bd,strsql,&row1,&cur1) > 0)
     {
      tz=atoi(row1[0]);
      kop.new_plus(row1[1]);
      kor.new_plus(row1[2]);    
     }

    if(metpoi == 1)
    if(poizkart(nomdp.ravno(),row[2],kontrp.ravno(),kor.ravno(),kodop.ravno(),kop.ravno(),tipzz,tz) != 0)
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
  polen(str_vib.ravno(),&datdok,2,'|');
  polen(str_vib.ravno(),&nomdok,3,'|');
  polen(str_vib.ravno(),&kop,4,'|');
  polen(str_vib.ravno(),&kor,5,'|');
  polen(str_vib.ravno(),&kolm,6,'|');

  polen(str_vib.ravno(),&bros,7,'|');
  polen(str_vib.ravno(),&cenaprod,8,'|');
  tz=1;
  if(bros.ravno()[0] == '-')
    tz=2;
  polen(str_vib.ravno(),&ktozz,9,'|');
  polen(str_vib.ravno(),&nomkarp,10,'|');

 }

vper:;
switch (K)
 {

  case UMN: //Смена режима просмотра
    regh++;
    if(regh == 2)
      regh=0;
    goto naz;

  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return;

  case TAB : //Перейти в парную карточку если такая есть
     if(nomkarp.ravno_atoi() == 0) 
      {
       iceb_t_soob(gettext("Запись не имеет парной карточки !"));
       goto naz;
      }      
     if(polen(kor.ravno(),&skl,2,'-') != 0)
      {
       iceb_t_soob(gettext("Запись не имеет парной карточки !"));
       goto naz;
      }
     nk=nomkarp.ravno_atoi();          
     goto naz3;
     
  case ENTER: /*Расшифровать запись*/
    GDITE();
    clearstr(LINES-5,0);

    SOOB.VVOD_delete();
    sprintf(strsql,"%s:%s",gettext("Дата подтверждения"),datp.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);
    sprintf(strsql,"%s:%s",gettext("Дата документа"),datdok.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);
    sprintf(strsql,"%s:%s",gettext("Номер документа"),nomdok.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);

    /*Расшифровываем код операции*/
    if(tz == 1)
      tabl.new_plus("Prihod");
    if(tz == 2)
      tabl.new_plus("Rashod");
      
    sprintf(strsql,"select naik from %s where kod='%s'",
    tabl.ravno(),kop.ravno());
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      sprintf(strsql,gettext("Не найден код операции %s"),kop.ravno());
      SOOB.VVOD_SPISOK_add_MD(strsql);
     }
    else
     {
      sprintf(strsql,gettext("Операция: %s %s"),kop.ravno(),row1[0]);
      SOOB.VVOD_SPISOK_add_MD(strsql);
     }    

    bros.new_plus("");
    if(ATOFM(kor.ravno()) == 0. && polen(kor.ravno(),&bros,2,'-') == 0 && kor.ravno()[0] == '0')
      bros.new_plus("00");
    else
      bros.new_plus(kor.ravno());

    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
    bros.ravno());
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),kor.ravno());
      SOOB.VVOD_SPISOK_add_MD(strsql);
     }
    else
     {
      sprintf(strsql,gettext("Контрагент: %s %s"),kor.ravno(),row1[0]);
      SOOB.VVOD_SPISOK_add_MD(strsql);
     }

    sprintf(strsql,"%s:%.10g",gettext("Количество"),kolm);
    SOOB.VVOD_SPISOK_add_MD(strsql);
    if(tz == 1)
      sprintf(strsql,"%s",gettext("Приход"));
    if(tz == 2)
      sprintf(strsql,"%s",gettext("Расход"));
    SOOB.VVOD_SPISOK_add_MD(strsql);

    sprintf(strsql,"%s:%s",gettext("Цена"),cenaprod.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);

    sprintf(strsql,"%s: %s",gettext("Записал"),iceb_t_kszap(ktozz.ravno()));
    SOOB.VVOD_SPISOK_add_MD(strsql);

    bf=localtime(&vrem[kom]);

    sprintf(strsql,"%s %d.%d.%d%s %s - %d:%d:%d",
    gettext("Дата записи"),
    bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
    gettext("г."),
    gettext("Время"),
    bf->tm_hour,bf->tm_min,bf->tm_sec);
    SOOB.VVOD_SPISOK_add_MD(strsql);

    if(nomkarp.ravno()[0] != '\0')
     {
      sprintf(strsql,"%s:%s",gettext("Номер парной карточки"),nomkarp.ravno());
      SOOB.VVOD_SPISOK_add_MD(strsql);
     }
    soobshw(&SOOB,stdscr,-1,-1,0,1);

    goto naz;

  case FK1:       /*Помощь*/
   GDITE();
   
   iceb_t_pdoc("matu3.txt");
   if(kolstr > 0)
      goto naz;
   else
      goto lll;
      
  case SFK2:       /*Корректировать*/
   korkm(skl,nk,kodm,&rowk);
   goto naz3;

  case FK2:       /*Слияние двух карточек*/
   sliqnie(skl,nk,kodm,shu.ravno(),cena);
   goto naz3;

  case FK3:       /*Удалить запись*/
  case SFK3:       /*Безусловное удаление записи*/
   
    rsdat(&d,&m,&g,datp.ravno(),0);
    if(iceb_t_pbpds(m,g) != 0)
      goto naz;

    if(mu_pvku(ktozap[kom]) != 0) /*проверка принадлежности записи*/
     goto naz;

    ostkar(dn,mn,gn,dk,mk,gk,skl,nk,&ost);

    if(tz == 1)
     {

      mos=1;

     if(K == FK3)   
     if(ost.ostg[3]-kolm < -0.0000000001)
      {
       SOOB.VVOD_delete();
       sprintf(strsql,gettext("Отрицательный остаток ! Удаление невозможно !"));
       SOOB.VVOD_SPISOK_add_MD(strsql);
       sprintf(strsql,gettext("Остаток: %10.10g"),ost.ostg[3]-kolm);
       SOOB.VVOD_SPISOK_add_MD(strsql);
       soobshw(&SOOB,stdscr,-1,-1,0,1);
       goto naz;
      }

     if(K == SFK3)   
      if(parolv(gettext("Введите пароль"),0) != 0)
        goto naz1;
     }

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("                   В Н И М А Н И Е  !!!"));
    DANET.VVOD_SPISOK_add_MD(gettext("Документ будет помечен, как не подтвержденный и без проводок !"));
    DANET.VVOD_SPISOK_add_MD(gettext("            Удалить запись ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      goto naz1;

      
    GDITE();   
    /*Удаляем запись в карточке*/
    rsdat(&d,&m,&g,datp.ravno(),1);
    sprintf(strsql,"delete from Zkart where sklad=%d and nomk=%d and nomd='%s' and datdp='%d-%02d-%02d'",skl,nk,nomdok.ravno(),g,m,d);

    if(sql_zap(&bd,strsql) != 0)
     {
     if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR)
      {
       iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
       goto naz; 
      }
     else
      msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);
     }
    kom--;

    rsdat(&d,&m,&g,datdok.ravno(),1);

    podvdok(d,m,g,nomdok.ravno(),skl);

    if(tz == 1 && kop.ravno()[0] != '\0')
     {
      sprintf(strsql,"select vido from Prihod where kod='%s'",kop.ravno());
      if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
       {
        sprintf(strsql,"%s %s!",gettext("Не найдена операция"),kop.ravno());
        iceb_t_soob(strsql);
        goto naz1;
       }
     /*Если операция внутреннего перемещения или изменения
       стоимости то проверять проводки не надо для прихода*/
      if(row1[0][0] != '0')
        goto naz1;
     }

    if(SRAV(nomdok.ravno(),"000",0) != 0 )
      prospr(3,skl,d,m,g,nomdok.ravno(),tz,0,0,"",kop.ravno());

    goto naz1;


  case FK4:  /*Режим поиска*/

    VV.VVOD_delete();
    VV.VVOD_SPISOK_add_ZAG(gettext("Введите данные для поиска нужных записей в карточке"));
    VV.VVOD_SPISOK_add_MD(gettext("Номер документа (,,).."));//0
    VV.VVOD_SPISOK_add_MD(gettext("Дата начала поиска...."));//1
    VV.VVOD_SPISOK_add_MD(gettext("Дата конца поиска....."));//2
    VV.VVOD_SPISOK_add_MD(gettext("Код контрагента.(,,).."));//3
    VV.VVOD_SPISOK_add_MD(gettext("Код операции (,,)....."));//4
    VV.VVOD_SPISOK_add_MD(gettext("Приход/расход (+/-)..."));//5

    VV.VVOD_SPISOK_add_data(nomdp.ravno(),20);
    VV.VVOD_SPISOK_add_data(dat1,11);
    VV.VVOD_SPISOK_add_data(dat2,11);
    VV.VVOD_SPISOK_add_data(kontrp.ravno(),20);
    VV.VVOD_SPISOK_add_data(kodop.ravno(),10);
    VV.VVOD_SPISOK_add_data(2);
    if(tipzz == 1)
      VV.VVOD_SPISOK_zapis_data(5,"+");
    if(tipzz == 2)
      VV.VVOD_SPISOK_zapis_data(5,"-");


fff4:;


    helstr(LINES-1,0,"F2/+",gettext("поиск"),
    "F3",gettext("реквизиты"),
    "F4",gettext("очистить"),
    "F10",gettext("выход"),NULL);
    
    kom1=VV.vvod(0,1,1,-1,-1);


    switch(kom1)
     {
      case FK10:
      case ESC:
        metpoi=0;
        break;

      case FK2:
      case PLU:
        nomdp.new_plus(VV.VVOD_SPISOK_return_data(0));
        strncpy(dat1,VV.VVOD_SPISOK_return_data(1),sizeof(dat1)-1);
        if(dat1[0] != '\0')
         {
          if(rsdat(&dp,&mp,&gp,dat1,1) != 0)
           {
            iceb_t_soob(gettext("Не верно введена дата начала !"));
            goto fff4;
           }
          dn=dp;
          mn=mp;
          gn=gp;
         }
        else
         {
          dn=1; mn=1; gn=gt;
         }
        strncpy(dat2,VV.VVOD_SPISOK_return_data(2),sizeof(dat2)-1);
        if(dat2[0] != '\0')
         {
          if(rsdat(&dp,&mp,&gp,dat2,1) != 0)
           {
            iceb_t_soob(gettext("Не верно введена дата конца !"));
            goto fff4;
           }
          dk=dp;
          mk=mp;
          gk=gp;
         }
        else
         {
          dk=dn; mk=mn; gk=gn;
          dpm(&dk,&mk,&gk,5);
         } 
        kontrp.new_plus(VV.VVOD_SPISOK_return_data(3));
        kodop.new_plus(VV.VVOD_SPISOK_return_data(4));
        tipzz=0;
        if(VV.VVOD_SPISOK_return_data(5)[0] == '+')
          tipzz=1;
        if(VV.VVOD_SPISOK_return_data(5)[0] == '-')
          tipzz=2;

        metpoi=0;
        if(nomdp.ravno()[0] != '\0')
         metpoi=1;
        if(kontrp.ravno()[0] != '\0')
         metpoi=1;
        if(dat1[0] != '\0')
         metpoi=1;
        if(dat2[0] != '\0')
         metpoi=1;
        if(kodop.ravno()[0] != '\0')
         metpoi=1;
        if(tipzz != 0)
         metpoi=1;
        kom=pozz=0;

        goto naz1;

      case FK3:
        MENU.VVOD_delete();

        MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));
        MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций приходов"));
        MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций расходов"));
        MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

        clearstr(LINES-1,0);

        kom2=0;        
        while(menu3w(stdscr,&MENU,&kom2,-1,-1,0) != 0);
        switch(kom2)
         {
          case 3 :
          case -1:
           break;

          case 0 :
            vibrek("Kontragent",&kontrp);
            goto fff4;

          case 1 :
//            vibrek("Prihod",&kodop);
           if(dirprihod(1,&kod,&naimk) == 0)
             VV.data_z_plus(4,kod.ravno());
            goto fff4;

          case 2 :
            kod.new_plus("");
            naimk.new_plus("");
            if(dirrashod(1,&kod,&naimk) == 0)
             VV.data_z_plus(4,kod.ravno());
            goto fff4;

         }
        break;
        
      case FK4:
        VV.VVOD_clear_data();
        goto fff4;              
         
      default:
        goto fff4; 
     }
   if(kolstr > 0)
     goto naz;
   else 
     goto lll;
     
  case FK5:  /*Распечатать*/

    raszkar(&cur,skl,nk,dn,mn,gn,dk,mk,gk,nomdp.ravno(),kontrp.ravno(),kodop.ravno(),tipzz);
    goto naz;

  case SFK5:  /*Распечатать список документов*/

    pnpz(skl,nk,kodm,naim.ravno());
    goto naz;

  case FK6:  /*Следующая дата*/
    dpm(&dn,&mn,&gn,3);
    dpm(&dk,&mk,&gk,3);
    goto naz1;
  
  case SFK6:  /*Предыдущая дата*/
    dpm(&dn,&mn,&gn,4);
    dpm(&dk,&mk,&gk,4);
    goto naz1;
    break;


  case FK7:  /*Вывод остатков по накладным*/
   if(mos == 1)
    {
     ostdok(dn,mn,gn,dk,mk,gk,skl,nk,&ost);
     mos=2;
    }
   else
    {
     ostkar(dn,mn,gn,dk,mk,gk,skl,nk,&ost);
     mos=1;
    }
   goto naz1;


  default:
   if(kolstr > 0)
     goto naz;
   else
     goto lll;
 }
}

/**********************************/
/*Корректировка карточки материалла*/
/**********************************/
void		korkm(int skl,int nk,int kodm,class SPISOK *row)
{
short		par;
double		cenaz;
class iceb_tu_str eiz("");
class iceb_tu_str shuz("");
double		ndsz;
int		mnds,mndsz;
double		cenavz;
double		krtz;
int             kodt;
double          cena,krt,fas,nds,cenav;
int		i1;
short		d,m,g;
time_t		vrem;
char		strsql[1024];
SQL_str         row1;
SQLCURSOR       cur1;
struct OPSHET	shetv;
struct  tm      *bf;
short deng,mesg,godg;

short denv=0,mesv=0,godv=0;
VVOD VV(0);
VVOD SOOB(1);

VV.VVOD_SPISOK_add_data(10);//0
VV.VVOD_SPISOK_add_data(10);//1
VV.VVOD_SPISOK_add_data(20);//2
VV.VVOD_SPISOK_add_data(10);//3
VV.VVOD_SPISOK_add_data(10);//4
VV.VVOD_SPISOK_add_data(10);//5
VV.VVOD_SPISOK_add_data(10);//6
VV.VVOD_SPISOK_add_data(20);//7
VV.VVOD_SPISOK_add_data(10);//8
VV.VVOD_SPISOK_add_data(11);//9

VV.VVOD_SPISOK_add_data(11);//10
VV.VVOD_SPISOK_add_data(16);//11
VV.VVOD_SPISOK_add_data(20);//12

krtz=atof(row->ravno(8));

cenavz=atof(row->ravno(7));
cenavz=okrug(cenavz,okrcn);


cenaz=atof(row->ravno(6));
cenaz=okrug(cenaz,okrcn);

eiz.new_plus(row->ravno(4));
shuz.new_plus(row->ravno(5));
ndsz=atof(row->ravno(9));
mndsz=atoi(row->ravno(3));


sprintf(strsql,"%.10g",cenaz);
VV.VVOD_SPISOK_zapis_data(0,strsql);
VV.VVOD_SPISOK_zapis_data(1,eiz.ravno());

if(mndsz == 1)
  sprintf(strsql,"+%.10g",ndsz);
else
  sprintf(strsql,"%.10g",ndsz);
VV.VVOD_SPISOK_zapis_data(2,strsql);

sprintf(strsql,"%.10g",krtz);
VV.VVOD_SPISOK_zapis_data(3,strsql);
VV.VVOD_SPISOK_zapis_data(4,row->ravno(11));
VV.VVOD_SPISOK_zapis_data(5,row->ravno(10));
VV.VVOD_SPISOK_zapis_data(6,shuz.ravno());

if(cenavz != 0.)
 {
  sprintf(strsql,"%.10g",cenavz);
  VV.VVOD_SPISOK_zapis_data(7,strsql);
 }

VV.VVOD_SPISOK_zapis_data(8,row->ravno(17));

rsdat(&deng,&mesg,&godg,row->ravno(18),2);
if(deng != 0)
 {
  sprintf(strsql,"%d.%d.%d",deng,mesg,godg);
  VV.VVOD_SPISOK_zapis_data(9,strsql);
 }

rsdat(&denv,&mesv,&godv,row->ravno(14),2);
if(denv != 0)
 {
  sprintf(strsql,"%d.%d.%d",denv,mesv,godv);
  VV.VVOD_SPISOK_zapis_data(10,strsql);
 }

VV.VVOD_SPISOK_zapis_data(11,row->ravno(15));
VV.VVOD_SPISOK_zapis_data(12,row->ravno(16));

VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка карточки материалов"));
VV.VVOD_SPISOK_add_ZAG(gettext("Операция не для безтолковых. Убедитесь, что все будет правильно."));
sprintf(strsql,"%s: %s",gettext("Записал"),iceb_t_kszap(row->ravno(12)));
VV.VVOD_SPISOK_add_ZAG(strsql);             

time_t tmm=atoi(row->ravno(13));
bf=localtime(&tmm);
sprintf(strsql,"%s %d.%d.%d%s %s: %d:%d:%d",
gettext("Дата записи :"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min,bf->tm_sec);
VV.VVOD_SPISOK_add_ZAG(strsql);             

VV.VVOD_SPISOK_add_MD(gettext("Учетная цена.................."));//0
VV.VVOD_SPISOK_add_MD(gettext("Единица измерения............."));//1
VV.VVOD_SPISOK_add_MD(gettext("НДС..........................."));//2
VV.VVOD_SPISOK_add_MD(gettext("Кратность....................."));//3
VV.VVOD_SPISOK_add_MD(gettext("Код тары......................"));//4
VV.VVOD_SPISOK_add_MD(gettext("Фасовка......................."));//5
VV.VVOD_SPISOK_add_MD(gettext("Счёт учета...................."));//6
VV.VVOD_SPISOK_add_MD(gettext("Цена розничная................"));//7
VV.VVOD_SPISOK_add_MD(gettext("Номер заказа.................."));//8
VV.VVOD_SPISOK_add_MD(gettext("Конечная дата использования..."));//9

if(VV.VVOD_SPISOK_return_data(11)[0] == '\0')
 {
  VV.VVOD_SPISOK_add_MD(gettext("Дата ввода в эксп-цию........."));//10
  VV.VVOD_SPISOK_add_MD(gettext("Инвентарный номер............."));//11
  VV.VVOD_SPISOK_add_MD(gettext("Регистрационный номер........."));//12
 }  
else
 {
  VV.VVOD_SPISOK_add_MD(gettext("Дата регистрации.............."));//10
  VV.VVOD_SPISOK_add_MD(gettext("Серия мед.препарата..........."));//11
  VV.VVOD_SPISOK_add_MD(gettext("Регистрационный номер........."));//12
 }

naz:;

clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F10",gettext("выход"),NULL);

par=VV.vvod(0,1,1,-1,-1);

switch(par)
 {
  case ESC:
  case FK10:
    clear();
    return;

  case FK1:
    GDITE();
    iceb_t_pdoc("matu3_1.txt");
    goto naz;

  case PLU:
  case FK2:
    break;    

  default:
    goto naz;
 }

/*Проверяем не является ли счёт счетом розничной торговли*/
if(shrt.getdlinna() > 1)
 {
  if(proverka(shrt.ravno(),VV.VVOD_SPISOK_return_data(6),1,1) != 0)
   if(VV.VVOD_SPISOK_return_data(7)[0] != '\0')
    {
     SOOB.VVOD_delete();
     sprintf(strsql,gettext("Счёт %s не является счетом розничной торговли !"),VV.VVOD_SPISOK_return_data(6));
     SOOB.VVOD_SPISOK_add_MD(strsql);
     sprintf(strsql,gettext("Цена розничная не вводится"));
     SOOB.VVOD_SPISOK_add_MD(strsql);
     soobshw(&SOOB,stdscr,-1,-1,0,1);
     goto naz;
    }

  if(proverka(shrt.ravno(),VV.VVOD_SPISOK_return_data(6),1,1) == 0)
   if(VV.VVOD_SPISOK_return_data(7)[0] == '\0')
    {
     SOOB.VVOD_delete();
     sprintf(strsql,gettext("Счёт %s является счетом розничной торговли !"),VV.VVOD_SPISOK_return_data(6));
     SOOB.VVOD_SPISOK_add_MD(strsql);
     sprintf(strsql,gettext("Цена розничная должна быть введена"));
     SOOB.VVOD_SPISOK_add_MD(strsql);
     soobshw(&SOOB,stdscr,-1,-1,0,1);
     goto naz;
    }

 }    


/*Проверяем дату первого прихода*/

cena=ATOFM(VV.VVOD_SPISOK_return_data(0));
cena=okrug(cena,okrcn);
if(SRAV(shuz.ravno(),VV.VVOD_SPISOK_return_data(6),0) != 0 || cena != cenaz)
 {
  prdpp(skl,nk,&d,&m,&g);
  if(iceb_t_pbpds(m,g) != 0)
    goto naz;
 }

/*Проверяем правильность введения единицы измерения*/
sprintf(strsql,"select naik from Edizmer where kod='%s'",VV.VVOD_SPISOK_return_data(1));
if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
 {
  sprintf(strsql,gettext("Не найдено единицу измерения %s !"),VV.VVOD_SPISOK_return_data(1));
  iceb_t_soob(strsql);
  goto naz;
 }    

/*Проверяем правильность введения счета*/
if(prsh1(VV.VVOD_SPISOK_return_data(6),&shetv) != 0)
 goto naz;

denv=mesv=godv=0;
if(VV.VVOD_SPISOK_return_data(10)[0] != '\0')
 if(rsdat(&denv,&mesv,&godv,VV.VVOD_SPISOK_return_data(10),1) != 0)
  {
   iceb_t_soob(gettext("Не верно введена дата !"));
   goto naz;

  }

deng=mesg=godg=0;
if(VV.VVOD_SPISOK_return_data(9)[0] != '\0')
 if(rsdat(&deng,&mesg,&godg,VV.VVOD_SPISOK_return_data(9),1) != 0)
  {
   iceb_t_soob(gettext("Не верно введена дата конечного использования !"));
   goto naz;

  }

mnds=0;
if(VV.VVOD_SPISOK_return_data(2)[0] == '+')
 mnds=1;

cena=ATOFM(VV.VVOD_SPISOK_return_data(0));
nds=ATOFM(VV.VVOD_SPISOK_return_data(2));
krt=ATOFM(VV.VVOD_SPISOK_return_data(3));
kodt=(int)ATOFM(VV.VVOD_SPISOK_return_data(4));
fas=ATOFM(VV.VVOD_SPISOK_return_data(5));
cenav=ATOFM(VV.VVOD_SPISOK_return_data(7));

if(zapkar(skl,nk,kodm,cena,VV.VVOD_SPISOK_return_data(1),
VV.VVOD_SPISOK_return_data(6),krt,fas,kodt,nds,mnds,cenav,denv,mesv,godv,
VV.VVOD_SPISOK_return_data(11),VV.VVOD_SPISOK_return_data(12),VV.VVOD_SPISOK_return_data(8),deng,mesg,godg,1) != 0)
  goto naz;

if(SRAV(shuz.ravno(),VV.VVOD_SPISOK_return_data(6),0) != 0 || cena != cenaz)
 {
  i1=0;
  if(cena == cenaz)
   i1=1; /*Пометить только документы*/
  cena=okrug(cena,okrcn);
  korcen(skl,nk,cena,i1);
 }       

/*Исправляем единицу измерения в документах*/
if(SRAV(eiz.ravno(),VV.VVOD_SPISOK_return_data(1),0) != 0)
 {
  time(&vrem);
  sprintf(strsql,"update Dokummat1 \
set \
ei='%s',\
ktoi=%d,\
vrem= %ld \
where sklad=%d and kodm=%d and nomkar=%d",
  VV.VVOD_SPISOK_return_data(1),iceb_t_getuid(),vrem,
  skl,kodm,nk);

  if(sql_zap(&bd,strsql) != 0)
   {
   if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR)
    {
     iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
    }
   else
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
   }  
 }

clear();

}

/**************/
/*Печать шапки*/
/**************/
void dmtrs(int skl,int nk,short dn,short mn,short gn,
short dk,short mk,short gk,short mos,const char *naim,int kgrm,const char *naimg,
const char *naiskl,class SPISOK *row,short metpoi,short denv,short mesv,short godv,
const char *innom,const char *rnd,const char *nomz,
class ostatokt *ost,
short deng,short mesg,short godg) //Дата конечного использования
{
clear();

printw("%s:%d/%.*s %s:%d",gettext("Склад"),skl,iceb_tu_kolbait(40,naiskl),naiskl,gettext("Карточка"),nk);


if(innom[0] != '\0' && rnd[0] == '\0')
 printw(" %s:%s",gettext("Ин-ный"),innom);
if(innom[0] != '\0' && rnd[0] != '\0')
 printw(" %s:%s",gettext("Серия"),innom);

if(denv != 0)
 printw(" %d.%d.%d",denv,mesv,godv);
if(rnd[0] != '\0')
 printw(" %s",rnd);
  
if(metpoi == 1)
 {
  move(1,COLS-20);
  attron(A_BLINK); /*Включение мерцания*/
  printw(gettext("Режим поиска !"));
  attroff(A_BLINK); /*Выключение мерцания*/
 }

move(1,0);
printw("%s:%d/%s  ",gettext("Группа"),kgrm,naimg);
if(atof(row->ravno(7)) != 0.)
 {
  attron(A_BLINK); /*Включение мерцания*/
  printw(" %s:%.6g",gettext("Розничная цена"),atof(row->ravno(7)));
  attroff(A_BLINK); /*Выключение мерцания*/
 }
move(2,0);
printw("%s:%s %s",gettext("Материал"),row->ravno(2),naim);
move(2,COLS-30);
printw(" %s:%d.%d.%d>%d.%d.%d\n",
gettext("Дата"),
dn,mn,gn,dk,mk,gk);

move(3,0);
printw(gettext("Цена: %.10g Ед.изм.: %s Счёт уч.: %s Фас.: %.3g Крат.: %.3g Код тар.: %d"),
atof(row->ravno(6)),row->ravno(4),row->ravno(5),atof(row->ravno(10)),atof(row->ravno(8)),atoi(row->ravno(11)));

if(row->ravno(3)[0] == '1')
  attron(A_BLINK); /*Включение мерцания*/
 
if(row->ravno(3)[0] == '1')
 printw(" %s: +%.6g",gettext("НДС"),atof(row->ravno(9)));
else
 printw(" %s: %.6g",gettext("НДС"),atof(row->ravno(9)));

attroff(A_BLINK); /*Выключение мерцания*/

vivost(4,0,mos,ost);

if(deng != 0)
 {
  move(4,COLS-25);
  printw(" %s:%d.%d.%d",gettext("Д.к.и."),deng,mesg,godg);
 }

if(nomz[0] != '\0')
  mvprintw(4,55,"%s:%s",gettext("Номер заказа"),nomz);

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2",gettext("объед."),
"F3",gettext("удаление"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F6",gettext("смен.даты"),
"F7",gettext("остат."),
"F10",gettext("выход"),NULL);
}

/*********************************************************/
/*Корректировка цены на приход в карточках и в накладных*/
/*********************************************************/
void		korcen(int skl,int nk,double cn,
short mbz) //0-сена изменилась 1-нет*/
{
SQL_str         row,row1,row2;
time_t		vrem;
char		strsql[512];
long		kolstr;
short		d,m,g;
short		dp,mp,gp;
char		bros[512];

clear();
printw(gettext("Корректировка цены в приходных записях в карточках и документах\n"));
GDITE();
sprintf(strsql,"select nomd,datd,datdp,tipz from Zkart where sklad=%d and nomk=%d",
skl,nk);
SQLCURSOR cur;
SQLCURSOR cur1;
SQLCURSOR cur2;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  return;
 }
time(&vrem);

while(cur.read_cursor(&row) != 0)
 {
  rsdat(&dp,&mp,&gp,row[2],2);
  rsdat(&d,&m,&g,row[1],2);
  sprintf(strsql,"select kodop,nomd,pro from Dokummat \
where datd='%d-%02d-%02d' and sklad=%d and nomd='%s'",
  g,m,d,skl,row[0]);

  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    clearstr(22,0);
    printw(gettext("Не найден документ ! Дата-%s Склад-%d Кар.-%d Док.-%s"),
    row[2],skl,nk,row[0]);
    sprintf(bros,"%s F3-delete",gettext("Нажмите любую клавишу для продолжения"));
    if(ostanov1(bros) == FK3)
     {

     }
    continue;
   }


  /*Помечаем что проводки надо проверить*/
  if(row1[2][0] == '1' && SRAV(row1[1],"000",0) != 0)
   {
    /*Определяем тип операции*/
    if(row[3][0] == '1')
     {
      sprintf(strsql,"select vido,prov from Prihod where kod='%s'",
      row1[0]);
      if(sql_readkey(&bd,strsql,&row2,&cur2) != 1)
       {
        clearstr(20,0);
        beep();
        printw(gettext("Не найден код операции %s !"),row1[1]);
        OSTANOV();
       }


       if(row2[0][0] != '0' || row2[1][0] == '1')
        goto vp1;
      }

    /*Корректировка записи*/
    sprintf(strsql,"update Dokummat \
set \
pro=0,\
ktoi=%d,\
vrem= %ld \
where datd='%d-%02d-%02d' and sklad=%d and nomd='%s'",
    iceb_t_getuid(),vrem,
    g,m,d,skl,row[0]);

    if(sql_zap(&bd,strsql) != 0)
     {
      sprintf(bros,"1-%s",gettext("Ошибка корректировки записи !"));
      msql_error(&bd,bros,strsql);
     }
   }

vp1:;

  if(mbz == 1)
    continue;
    
  if(row[3][0] == '1')
   {
    
    sprintf(strsql,"update Zkart \
set \
cena=%.10g,\
ktoi=%d,\
vrem= %ld \
where sklad=%d and nomk=%d and nomd='%s' and datdp='%d-%02d-%02d'",
    cn,iceb_t_getuid(),vrem,
    skl,nk,row[0],gp,mp,dp);

    if(sql_zap(&bd,strsql) != 0)
     {
      sprintf(bros,"2-%s",gettext("Ошибка корректировки записи !"));
      msql_error(&bd,bros,strsql);
     }


    sprintf(strsql,"update Dokummat1 \
set \
cena=%.10g,\
ktoi=%d,\
vrem=%ld \
where sklad=%d and nomkar=%d and nomd='%s' and datd='%s'",
    cn,iceb_t_getuid(),vrem,
    skl,nk,row[0],row[1]);

    if(sql_zap(&bd,strsql) != 0)
     {
      sprintf(bros,"3-%s",gettext("Ошибка корректировки записи !"));
      msql_error(&bd,bros,strsql);
     }
  
   }

 }

}

/*******************************************************/
/*Режим слияния двух карточек на страх и риск оператора*/
/*******************************************************/


void sliqnie(int skl,int nkn,int kodm,const char *shu,double cena)
{
short		mpz;
char		bros[512];
SQL_str         row,row1;
SQLCURSOR       cur;
SQLCURSOR       cur1;
long		kolstr;
char		strsql[512];
short		i;
int		nk;
class iceb_tu_str ei("");
double          fas,krt,nds;
short           kodt;
double		cenaz;
short		d,m,g;
short		dd,md,gd;
int		tipz;
double		kolih;
class iceb_tu_str kop(""),kor("");
class iceb_tu_str vstr("");
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);

		
clear();
printw(gettext("Вы вошли в режим слияния записей двух карточек.\n\
Записи из карточки номер которой вы введете, перепишутся в текущюю карточку.\n\
После этого карточка будет удалена. Эта операция возможна, если карточка\n\
была заведена в текущем отчётном периоде.\n"));

helstr(LINES-1,0,"F10",gettext("выход"),NULL);

bros[0]='\0';

VVOD VVOD1(2);
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите номер карточки"));

if((i=vvod1(&vstr,16,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
  return;
if(i == ESC)
  return;

VVOD PAR(2);
PAR.VVOD_SPISOK_add_MD(gettext("Введите пароль"));

if(parolv(&PAR,dt,mt,gt,0) != 0)
  return;



nk=vstr.ravno_atoi();

/*Читаем заказанную карточку*/
sprintf(strsql,"select * from Kart where sklad=%d and nomk=%d",
skl,nk);

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,gettext("Нет на складе %d карточки N%d !"),skl,nk);
  iceb_t_soob(strsql);
  return;
 }

ei.new_plus(row[4]);
fas=atof(row[10]);
krt=atof(row[8]);
kodt=atoi(row[11]);
nds=atof(row[9]);
double cenask=atof(row[6]);

if(kodm != atoi(row[2]))
 {
  iceb_t_soob(gettext("Эта карточка с другим кодом материалла !"));
  return;
 }

clear();
printw(gettext("\
Материал.......... %d\n\
Цена.............. %f\n\
Единица измерения. %s\n\
Счёт учёта........ %s\n\
Фасовка........... %f\n\
Кратность......... %.3g\n\
Код тари.......... %d\n\
НДС............... %f\n"),
kodm,cenask,ei.ravno(),shu,fas,krt,kodt,nds);

VVOD DANET(1);
DANET.VVOD_SPISOK_add_MD(gettext("                       В Н И М А Н И Е  !!!"));
sprintf(strsql,gettext("Переписываем с удалением записей из карточки %d в карточку %d."),nk,nkn);
DANET.VVOD_SPISOK_add_MD(strsql);
DANET.VVOD_SPISOK_add_MD(gettext("                     Все верно ? Вы уверены ?"));

if(danet(&DANET,2,stdscr) == 2)
  return;



clear();
printw(gettext("Переписываем записи\n"));
GDITE();   
sprintf(strsql,"select * from Zkart where sklad=%d and nomk=%d \
order by datdp asc",skl,nk);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return;
 }


mpz=0;
while(cur.read_cursor(&row) != 0)
 {
  rsdat(&d,&m,&g,row[3],2);

  if(mpz == 0)
   {

    if(iceb_t_pbpds(m,g) != 0)
      return;

   }
  mpz++;
  rsdat(&dd,&md,&gd,row[4],2);

  tipz=atoi(row[5]);
  kolih=atof(row[6]);
  cenaz=atof(row[7]);
  cenaz=okrug(cenaz,okrcn);
   
  if(tipz == 1)
     cenaz=cena;
      
  zapvkr(d,m,g,row[2],skl,nkn,dd,md,gd,tipz,kolih,cenaz);
  
  GDITE();
  kop.new_plus("");
  kor.new_plus("");
  sprintf(strsql,"select tip,kodop,kontr from Dokummat \
where datd='%s' and sklad=%d and nomd='%s' and tip=%d",
  row[4],skl,row[2],tipz);

  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    clearstr(22,0);
    printw(gettext("Не найден документ ! Дата-%s Склад-%d Кар.-%d Док.-%s"),
    row[4],skl,nk,row[2]);
    sprintf(bros,"%s F3-delete",gettext("Нажмите любую клавишу для продолжения"));
    if(ostanov1(bros) == FK3)
     {

     }
    continue;
   }
  kor.new_plus(row1[2]);
  kop.new_plus(row1[1]);

  printw("%02d.%02d.%d %02d.%02d.%d %-8s %-3s %-8s %10.10g %.6g\n",
  d,m,g,dd,md,gd,row[2],kop.ravno(),kor.ravno(),
  kolih,atof(row[7]));
  GDITE();
    
   /*Помечаем что проводки надо проверить*/
   if(tipz == 1 )
    {
     if(SRAV(row[2],"000",0) == 0 || SRAV(kop.ravno(),"ОСТ",0) == 0 )
        goto vp1;
     
     sprintf(strsql,"select vido from Prihod where kod='%s'",kop.ravno());
     if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
      {
       beep();
       printw("Не найдена операция %s\n",kop.ravno());
       OSTANOV();

      }
     else
      if(atoi(row1[0]) != 0)
        goto vp1;
    }
   sprintf(strsql,"update Dokummat \
set \
pro=0 \
where datd='%s' and sklad=%d and nomd='%s' and tip=%d",
   row[4],skl,row[2],tipz);

   if(sql_zap(&bd,strsql) != 0)
    {
     printw("Ошибка корректировки записи заголовка документа  !\n%s\n",sql_error(&bd));
     OSTANOV();
     break;
    }
      

vp1:;



   /*Исправляем запись в накладной*/

   sprintf(strsql,"update Dokummat1 \
set \
nomkar=%d, \
cena=%.10g, \
ktoi=%d \
where datd='%d-%d-%d' and sklad=%d and kodm=%d and nomkar=%d and \
tipz=%d",
   nkn,cenaz,iceb_t_getuid(),
   gd,md,dd,skl,kodm,nk,tipz);

   if(sql_zapis(strsql,1,0) != 0)
    {
     break;
    }



 } 

/*Удаляем записи в карточке*/
sprintf(strsql,"delete from Zkart where sklad=%d and nomk=%d",skl,nk);

sql_zapis(strsql,1,0);

/*Удаляем саму карточку*/
sprintf(strsql,"delete from Kart where sklad=%d and nomk=%d",skl,nk);

sql_zapis(strsql,1,0);


OSTANOV();
}

/****************************************/
/*Проверка соответствия условиям поиска*/
/***************************************/

int poizkart(const char *nomdp,const char *nomdp1,const char *kontrp,
const char *kontrp1,const char *kodop,const char *kodop1,int tipzz,int tipzz1)
{
int		i1;

if(nomdp[0] != '\0')
 if(pole1(nomdp,nomdp1,',',0,&i1) != 0)
  if(SRAV(nomdp,nomdp1,0) != 0)
    return(1);

if(kontrp[0] != '\0')
 if(pole1(kontrp,kontrp1,',',0,&i1) != 0)
  if(SRAV(kontrp,kontrp1,0) != 0)
    return(2);

if(kodop[0] != '\0')
 if(pole1(kodop,kodop1,',',0,&i1) != 0)
  if(SRAV(kodop,kodop1,0) != 0)
    return(3);

if(tipzz != 0 && tipzz != tipzz1)
   return(4);
return(0);
}
/****************************/
/*Просмотр не подтвержденных записей*/
/*************************************/

void pnpz(int skl,int nk,int kodm,const char *naim)
{
SQL_str         row;
char		strsql[512];
int		kolstr,kols;
short		d,m,g;
double		kol,kol1;
int		knz;
char		imaf[64];
class iceb_fopen fil;

clear();

printw(gettext("Поиск не подтвержденных записей в документах.\n"));
GDITE();
sprintf(strsql,"select datd,nomd,kolih from Dokummat1 where sklad=%d \
and nomkar=%d order by datd asc",skl,nk);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return;  
 }

sprintf(imaf,"npz%d.lst",getpid());
if(fil.start(imaf,"w") != 0)
 return;

iceb_t_zagolov(gettext("Распечатка не подтвержденных записей в документах"),fil.ff);

fprintf(fil.ff,"\n%s:%d %s:%d\n",gettext("Склад"),skl,gettext("Номер карточки"),nk);
fprintf(fil.ff,"%s:%d %s\n\n",gettext("Материал"),kodm,naim);
fprintf(fil.ff,"\
--------------------------------------------\n");
fprintf(fil.ff,gettext("\
 Дата     |Номер док.|Кол.в док.|Кол.подтв.|\n"));
fprintf(fil.ff,"\
--------------------------------------------\n");
double it1=0.,it2=0.;
knz=kols=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,kols++);
  rsdat(&d,&m,&g,row[0],2);
  kol=atof(row[2]);
  kol1=readkolk(skl,nk,d,m,g,row[1]);  
  if(fabs(kol-kol1) > 0.000001)
   {
    knz++;
    printw("%02d.%02d.%d %-*s %10.10g %10.10g\n",d,m,g,iceb_tu_kolbait(10,row[1]),row[1],kol,kol1);
    fprintf(fil.ff,"%02d.%02d.%d %-*s %10.10g %10.10g\n",d,m,g,iceb_tu_kolbait(10,row[1]),row[1],kol,kol1);
    it1+=kol;
    it2+=kol1;
   }
 }

fprintf(fil.ff,"\
--------------------------------------------\n");
fprintf(fil.ff,"\
%26s:%10.10g %10.10g\n",gettext("Итого"),it1,it2);

podpis(fil.ff);
fil.end();

if(knz == 0)
 {
  unlink(imaf);
  iceb_t_soob(gettext("Все записи подтверждены !"));
  return;  
 }

OSTANOV();

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка не подтвержденных записей в документах"));

iceb_t_ustpeh(imaf,3);

iceb_t_rabfil(&oth,"");

}
