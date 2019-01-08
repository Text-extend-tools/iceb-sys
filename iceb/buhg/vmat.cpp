/*$Id:$ */
/*22.03.2017    28.01.1994      Белых А.И.      vmat.c
Поиск всех карточек на всех складах по заданному материалу
Возвращаем 0-если выбрали карточку или ввели новую
           1-не выбрали
*/
#include <ctype.h>
#include	"buhg.h"

void	vmatsap(short,short,short,int,int,const char*,int,short,double,double,int,int);
int  vmatmc(short*,short*,short*,class iceb_tu_str*,double);
int   rashod(short,short,short,int,const char*,int,int,double,const char*,const char*,const char*,float);

int  vmatmed(short*,short*,short*,class iceb_tu_str*,class iceb_tu_str*);
void	vmatkolp(int,const char*,int,int,short,short,short);
int  vmat_alkog(short *denv,short *mesv,short *godv,class iceb_tu_str *regnom);
void vkm(short,short,short,int,const char*,int,int,int,const char*,double,double,float);
void vbrp(short,short,short,int,const char*,int,int,const char*,float);
int muraskod(class spis_oth *oth,int kodm);

extern class iceb_tu_str spmatshets; /*список материальных счетов*/
extern double	okrcn;  /*Округление цены*/
extern double   okrg1;  /*Округление суммы*/
extern class iceb_tu_str shrt;    /*Счёта розничной торговли*/

int vmat(int tipz, //1-приход 2-расход
const char *kpos, //Код контрагента
short dd,short md,short gd, //Дата документа
int skl, //Код склада
const char *nomdok, //Номер документа
int lnds, //Метка документа НДС
int kodmm, //Код материалла если не равен нолю привязка к карточке
double cnn, //Цена в документе
short cp,  //Цель поиска 0-выбор карточки для записи 1-только просмотр карточек
const char *sheta, //перечень счетов возможных при данной операции
const char *nomdokp, //Номер встречного документа
double kolih, //Для привязки записи к карточке
const char *kodop, //Код операции
class iceb_tu_str *nomz, //Номер заказа
float pnds)
{
class spis_oth oth;
class ostatokt ost;
struct  tm      *bf;
int             i,i1;
short           kom,kom1;
class iceb_tu_str mat("");  /*Наименование материалла*/
int             skll=0;
int             K;
int		mndsz=0;
char            mnds=0;
double		ostpd; /*Остаток по документам*/
long		kolstr;
long		pozz,pozz1;
SQL_str         row,row1;
char		strsql[2048];
int		kodm,kgrm;
class iceb_tu_str naim("");
class iceb_tu_str bros("");
int		nk=0;
class iceb_tu_str shbm("");
class iceb_tu_str ei("");
double		ndsg=0;
double		cena=0;
int		skan=0;
class iceb_t_mstr spis_menu(0);
class iceb_tu_str grup("");
double		oostkar,oostdok;
short		deno,meso,godo;
short		d,m,g;
short		metkaost=0; //если 0 все карточки 1-только с остатками
short		mspz; /*метка соответствия первой записи*/
class iceb_tu_str innom(""); //Инвентарный номер для малоценки
int		metkaskl=0; 
class iceb_tu_str naiei("");
short		metka_sort=0; /*Сортировка 0-по номерам карточек 1-по цене учёта*/
class iceb_tu_str naishet("");
class iceb_tu_str naiskl("");
class iceb_tu_str vstr("");
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);
SQLCURSOR       cur1;
VVOD SOOB(1);
VVOD VVOD1(2);
if(tipz == 1)
 metka_sort=1;
deno=dt; meso=mt; godo=gt;

if(sravmydat(dt,mt,gt,dd,md,gd) < 0) /*Если выписывается документ на будущее*/
 {
  deno=dd; meso=md; godo=gd;
 } 

oostkar=oostdok=0.;

if(kodmm != 0)
 {
  kodm=kodmm;
  goto vppp;
 }


helstr(LINES-1,0,"Enter",gettext("материалы"),
"F10",gettext("выход"),"  ",
gettext(" + перед числом - поиск по числу в наименовании"),NULL);

//readscan(NULL); /*Сброс сканерного ввода*/
mat.new_plus("");

VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код или наименование материалла"));

//if((skan=vvod1(&mat,32,&VVOD1,&readscan,stdscr,-1,-1)) == FK10)
if((skan=vvod1(&mat,32,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
   return(1);
if(skan == ESC)
  return(1);

GDITE();
kgrm=0;
kodm=mat.ravno_atoi();

if(mat.ravno()[0] == '\0' || isdigit(mat.ravno()[0]) == 0)
 {
  clear();
  kgrm=kodm=0;
  if(polen(mat.ravno(),&naim,2,'+') != 0)
    naim.new_plus(mat.ravno());
  int metka=0;
  if(tipz == 2)
   metka=1;
  if((i=dirmatr(&kgrm,&kodm,&naim,1,skl,grup.ravno(),0,metka)) != 0)
   {
    if( i == NOL)
     {
      if(cp == 1)
       return(1);
    
      vbrp(dd,md,gd,skl,nomdok,kodm,tipz,kodop,pnds);
    
      return(1);
     }     
    clear();
    return(1);
   }
 }

vppp:;

/*Читаем материал*/
if(skan != -1) 
  sprintf(strsql,"select * from Material where kodm=%d",kodm);
else
  sprintf(strsql,"select * from Material where strihkod=%s",mat.ravno());

if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
 {
  if(skan != -1) 
    sprintf(strsql,"%s %d !",gettext("Не найден код материалла"),kodm);
  else
    sprintf(strsql,gettext("Не найден штрих код %s !"),mat.ravno());
  
  iceb_t_soob(strsql);

  return(1);
 }

kodm=atoi(row[0]);
naim.new_plus(row[2]);
kgrm=atoi(row[1]);
pozz=0;
kom=0;

naz1:;

GDITE();

short           kls=LINES-7;
time_t		vrem[kls];
int		ktozap[kls];
oostkar=oostdok=0.;

if(nomz->ravno()[0] == '\0')
 {
  if(metka_sort == 0)
    sprintf(strsql,"select * from Kart where kodm=%d order by sklad,nomk desc",kodm);

  if(metka_sort == 1)
    sprintf(strsql,"select * from Kart where kodm=%d order by sklad,cena desc",kodm);
 }
else
 {
  sprintf(strsql,"select * from Kart where kodm=%d and nomz='%s' order by sklad,nomk desc",
  kodm,nomz->ravno());
 }

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
 }

if(kolstr <= 0)
  goto vp;
  

naz:;
GDITE();

if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;

oostkar=oostdok=0.;

cur.poz_cursor(pozz);
mspz=2;
pozz1=0;

spis_menu.spis_str.free_class();
for(i=0;i<=kls-1;)
 {
  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;
/*
  printw("%s %s %s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4],
  row[5],row[6]);
  refresh();
*/
  skll=atoi(row[0]);
  if(metkaskl == 1 && skl != 0 && skl != skll)
    continue;
  nk=atoi(row[1]);
  cena=atof(row[6]);

  
  /*Остаток по документам*/
  ostdok(1,1,godo,deno,meso,godo,skll,nk,&ost);
  ostpd=ost.ostg[3];
  oostdok+=ost.ostg[3];

  /*Остаток по карточкам*/
  ostkar(1,1,godo,deno,meso,godo,skll,nk,&ost);
  if(metkaost == 1)
   if(ost.ostg[3] == 0.)
     continue;
  oostkar+=ost.ostg[3];
  bros.new_plus("");
  if(skll == skl)
    bros.new_plus("*");
  else
    bros.new_plus(" ");

  if(row[15][0] != '\0')
   bros.plus("#");
   
  if(row[3][0] == '0')
    mnds=' ';
  if(row[3][0] == '1')
    mnds='+';

  ktozap[i]=atoi(row[12]);
  vrem[i]=atol(row[13]);

  sprintf(strsql,"%-2s|%-4s|%-5s|%-4s|%c%2.4g|%-8.8g|%8.8g|%8.8g|%.6g|%.8g%s|%s",
  row[0],row[1],row[5],row[4],mnds,atof(row[9]),cena,
  ost.ostg[3],ostpd,atof(row[8]),atof(row[7]),bros.ravno(),row[17]);
  if(COLS > 90)
   {
    rsdat(&d,&m,&g,row[18],2);
    if(d != 0)
     {
      char dat[128];
      sprintf(dat,"|%02d.%02d.%d",d,m,g);
      strcat(strsql,dat);
     }
    else
     strcat(strsql,"|");
   }
      
  spis_menu.spis_str.plus(strsql);
  i++;
   
  if(pozz1 == 1) /*Первая запись удовлетворяет условиям поиска*/
   mspz=1;
 }

if(i == 0)
 {
  if(kolstr != 0 && pozz > 0)
   {
    pozz--;
    goto naz;
   }
lllp:;
  vmatsap(deno,meso,godo,tipz,kodm,naim.ravno(),metkaost,cp,oostkar,oostdok,skl,metkaskl);
  SOOB.VVOD_delete();
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == FK7 || K == FK8)
    goto vper;
  if(K == FK10 || K == ESC)
    return(1);
  goto lllp;
 }  
if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

naz2:;

vmatsap(deno,meso,godo,tipz,kodm,naim.ravno(),metkaost,cp,oostkar,oostdok,skl,metkaskl);

/*Меню с перечнем карточек материалов для выбора*/
i1=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,4,0,NULL,strsql,i1,&K,"",0);


if(kom == -7) //Изменение размеров экрана
 {
  goto naz1;  
 }

if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  for(i=0; i< mspz   ; )
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

    if(metkaost == 1)
     {
      ostkar(1,1,godo,deno,meso,godo,atoi(row[0]),atoi(row[1]),&ost);
      if(ost.ostg[3] == 0.)
        continue;
     }
    i++;
   }
  kom=kls-1;
  goto naz;
 }
if(kom == -2) /*Стрелка вверх*/
 {
  GDITE();
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

    if(metkaost == 1)
     {
      ostkar(1,1,godo,deno,meso,godo,atoi(row[0]),atoi(row[1]),&ost);
      if(ost.ostg[3] == 0.)
        continue;
     }

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
  printw("\n вперед pozz=%d kolstr=%ld\n",pozz,kolstr);
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
  GDITE();
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
    if(cur.read_cursor(&row) == 0)
      break;

    if(metkaost == 1)
     {
      ostkar(1,1,godo,deno,meso,godo,atoi(row[0]),atoi(row[1]),&ost);
      if(ost.ostg[3] == 0.)
        continue;
     }

    i++;
   }
  goto naz;
 }

if(kom == -1)
 {
  return(1);
 }
else
 {
  if(SRAV(gettext("Конец списка"),spis_menu.spis_str.ravno(kom),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz;
  polen(spis_menu.spis_str.ravno(kom),&skll,1,'|');
  polen(spis_menu.spis_str.ravno(kom),&nk,2,'|');

  sprintf(strsql,"select * from Kart where sklad=%d and nomk=%d",skll,nk);
  if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
   {
    sprintf(strsql,"%s %d %s %d !\n",gettext("Не найдена карточка"),nk,gettext("на складе"),skll);
    iceb_t_soob(strsql);
   }

  shbm.new_plus(row[5]);
  ei.new_plus(row[4]);
  ndsg=atof(row[9]);
  mndsz=atoi(row[3]);
  cena=atof(row[6]);
  cena=okrug(cena,okrcn);
  innom.new_plus(row[15]);
  nomz->new_plus(row[17]); //Передается в списке параметров и размер неизвестен
 }

vper:;
switch (K)
 {
  case ENTER:   /*Выбрали карточку*/
    if(cp == 1)
      goto fk3;

    GDITE();
    if(tipz == 1)
     {
      if(innom.ravno()[0] != '\0' && SRAV(kodop,"ОСТ",0) != 0) //корректировка стартового остатка возможна
       {
        SOOB.VVOD_delete();
        SOOB.VVOD_SPISOK_add_MD(gettext("Карточка с инвентарным номером"));
        SOOB.VVOD_SPISOK_add_MD(gettext("Приход в карточку не возможен !"));
        soobshw(&SOOB,stdscr,-1,-1,0,1);
        goto naz;
       }
      if(kodm != 0 && cnn != 0. && cnn != cena)
       {
        SOOB.VVOD_delete();
        SOOB.VVOD_SPISOK_add_MD(gettext("Цена в карточке не равна цене в документе"));
        soobshw(&SOOB,stdscr,-1,-1,0,1);
        goto naz;
       }
     }

    /*Проверяем соотвецтвие счета в выбранной карточке
    перечню счетов возможных при данном расходе*/

    if(sheta[0] != '\0')
    if(pole1(sheta,shbm.ravno(),',',0,&i1) != 0)
    if(SRAV(sheta,shbm.ravno(),0) != 0)
     {
      SOOB.VVOD_delete();
      sprintf(strsql,"%s %s !",
      gettext("Этот материал числится на счете"),
      shbm.ravno());
      sprintf(strsql,"%s %s !",
      gettext("Для данной операции можно:"),
      sheta);
      
      SOOB.VVOD_SPISOK_add_MD(strsql);
      soobshw(&SOOB,stdscr,-1,-1,0,1);

      goto naz;
     }


    /*Проверяем есть ли такая запись*/

    sprintf(strsql,"select datd from Dokummat1 where sklad=%d and \
nomd='%s' and kodm=%d and nomkar=%d and datd='%04d-%d-%d'",
    skl,nomdok,kodm,nk,gd,md,dd);
    if(sql_readkey(&bd,strsql,&row,&cur1) >= 1)
     {
      short vidop=0;
      if(tipz == 1)
       sprintf(strsql,"select vido from Prihod where kod='%s'",kodop);
      if(tipz == 2)
       sprintf(strsql,"select vido from Rashod where kod='%s'",kodop);
      if(readkey(strsql,&row1,&cur1) == 1)
        vidop=atoi(row1[0]);
        
      if(nomdokp[0] != '\0' && vidop != 0) //Если документ парный
       {
        iceb_t_soob(gettext("Такая запись в документе уже есть !"));
        goto naz2;
       }

      VVOD DANET(1);
      DANET.VVOD_SPISOK_add_MD(gettext("Такая запись в документе уже есть !"));
      DANET.VVOD_SPISOK_add_MD(gettext("Увеличить количество ?"));
      clearstr(LINES-1,0);
      if(danet(&DANET,2,stdscr) == 2)
        goto naz2;     

      vmatkolp(skl,nomdok,kodm,nk,gd,md,dd);
      return(0);
     }

    /*Если расход материалла или приход сторонние, то склад в выбранной
    карточке должен быть такой как во входном меню
    Если внутреннее перемещение:
    расход - карточка должна быть с того склада что указано во входном
     меню. С нее будет проведен расход на склад который указан в коде
     организации принимающей расход через тире. Необходимо найти на этом
     складе карточку с такимиже реквизитами или завести новую.
    приход - нужно найти карточку на складе с которого будет произведен расход
     (указан в коде организации через тире) найти на складе указанном
     во входном меню такую карточку или записать новую.
    То есть во всех случаях кроме внутреннего перемещения-прихода
    склад в выбранной карточке должен быть равен складу указанному во
    входном меню.
    */

    if(tipz == 1 && ATOFM(kpos) != 0. && polen(kpos,&bros,2,'-') == 0)
     {
      if(skl == skll)
       {
        sprintf(strsql,gettext("Карточка должна быть не со склада %d !"),skll);
        iceb_t_soob(strsql);
	goto naz2;
       }
     }
    else
     if(skl != skll)
      {
       sprintf(strsql,gettext("Эта карточка находиться на складе %d !"),skll);
       iceb_t_soob(strsql);
       goto naz2;
      }

    /*Документ с НДС а карточка без НДС*/
    if(lnds == 0 && ndsg < 0.00001)
     {
      VVOD DANET(1);
      DANET.VVOD_SPISOK_add_MD(gettext("Документ c НДС, а карточка без НДС"));
      DANET.VVOD_SPISOK_add_MD(gettext("    Все верно ? Вы уверены ?"));
      clearstr(LINES-1,0);
      if(danet(&DANET,2,stdscr) == 2)
        goto naz2;     
     }

    if(lnds != 0 && ndsg > 0.0001)
     {
      VVOD DANET(1);

      DANET.VVOD_SPISOK_add_MD(gettext("Документ без НДС, а карточка с НДС"));
      DANET.VVOD_SPISOK_add_MD(gettext("    Все верно ? Вы уверены ?"));
      beep();
      clearstr(LINES-1,0);
      if(danet(&DANET,2,stdscr) == 2)
        goto naz2;     
     }

    if(tipz == 1)
     {
      if(prihod(dd,md,gd,skl,nomdok,kodm,nk,cena,ei.ravno(),mndsz,ndsg,nomz->ravno(),kodmm) != 0)
        goto naz2;
     }

    if(tipz == 2)
     {

      if(rashod(dd,md,gd,skl,nomdok,nk,kodmm,kolih,kpos,nomdokp,kodop,pnds) != 0)
        goto naz2;
     }


   break;

  case FK1: /*Помощь*/
   if(cp == 1)
    {
     GDITE();
     iceb_t_pdoc("matu2_1_2f.txt");
     clear();
    }

   if(tipz == 1 && cp == 0)
    {
     GDITE();
     iceb_t_pdoc("matu2_1_2g.txt");
     clear();
    }
   if(tipz == 2 && cp == 0)
    {
     GDITE();
     iceb_t_pdoc("matu2_1_2s.txt");
     clear();
    }
   goto naz2;
 
  case PLU: /*Ввести новую карточку*/
  case FK2: /*Ввести новую карточку*/
    if(tipz == 2 || cp == 1)
      goto naz2;

vp:;
    if(cp == 1)
     {
      SOOB.VVOD_delete();
      sprintf(strsql,"%s %d!",gettext("Нет карточек по материалу"),kodmm);
      SOOB.VVOD_SPISOK_add_MD(strsql);
      if(nomz->ravno()[0] != '\0')
       {
        sprintf(strsql,"%s:%s",gettext("Номер заказа"),nomz->ravno());
        SOOB.VVOD_SPISOK_add_MD(strsql);
       }      
      soobshw(&SOOB,stdscr,-1,-1,0,1);
      return(1);        
     }
    if(tipz == 1)
     {
      clear();
      vkm(dd,md,gd,skl,nomdok,kodm,kodmm,lnds,kodop,cnn,kolih,pnds);
     }
    if(tipz == 2)
     {
      if(kodmm != 0)
       {
        SOOB.VVOD_delete();
        sprintf(strsql,"%s %d!",gettext("Нет карточек по материалу"),kodmm);
        SOOB.VVOD_SPISOK_add_MD(strsql);
        if(nomz->ravno()[0] != '\0')
         {
          sprintf(strsql,"%s:%s",gettext("Номер заказа"),nomz->ravno());
          SOOB.VVOD_SPISOK_add_MD(strsql);
         }      
        soobshw(&SOOB,stdscr,-1,-1,0,1);
        return(1);        
       }
      VVOD DANET(1);
      sprintf(strsql,"%s \"%.45s\" %s !",gettext("Нет материала"),naim.ravno(),gettext("на складах"));
      DANET.VVOD_SPISOK_add_MD(strsql);
      DANET.VVOD_SPISOK_add_MD(gettext("Будете делать не привязаный к карточкам расход ?"));
      clearstr(LINES-1,0);
      if(danet(&DANET,2,stdscr) == 2)
        return(1);
      vbrp(dd,md,gd,skl,nomdok,kodm,tipz,kodop,pnds);
    
       
     }
   break;

  case FK3: /*Просмотр карточки*/

fk3:;
    /*Карточки могут быть откорректированы*/
    clear();
    dirzkart(skll,nk);

    goto naz1;


  case FK4: /*Ввод не привязанных записей к карточкам*/
    if(cp == 1)
     goto naz2;
    
    vbrp(dd,md,gd,skl,nomdok,kodm,tipz,kodop,pnds);
    
    break;

  case FK5: /*Распечатка движения по карточкам*/
    oth.clear();
    if(muraskod(&oth,kodm) == 0)
       iceb_t_rabfil(&oth,"");
     
    goto naz1;

  case SFK6: /*Расшифровка*/
    
    //Читаем наименование склада
    sprintf(strsql,"select naik from Sklad where kod=%d",skll);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      naiskl.new_plus("");
      sprintf(strsql,gettext("Не найден склад %d в списке складов !"),skll);
      iceb_t_soob(strsql);
     }
    else
     {
//      sprintf(naiskl,"%s: %d %s",gettext("Склад"),skll,row1[0]);
      naiskl.new_plus(gettext("Склад"));
      naiskl.plus(": ");
      naiskl.plus(skll);
      naiskl.plus(" ",row1[0]);
     }
    //Читаем наименвание счета учета
    sprintf(strsql,"select nais from Plansh where ns='%s'",shbm.ravno());
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      sprintf(strsql,gettext("Не найден счёт %s в плане счетов !"),shbm.ravno());
      iceb_t_soob(strsql);
      naishet.new_plus("");
     }
    else
     {
//      sprintf(naishet,"%s:%s %s",gettext("Счёт"),shbm.ravno(),row1[0]);
      naishet.new_plus(gettext("Счёт"));
      naishet.plus(":",shbm.ravno());
      naishet.plus(" ",row1[0]);
     }
    //Читаем наименвание единицы измерения
//    naiei[0]='\0';
    sprintf(strsql,"select naik from Edizmer where kod='%s'",ei.ravno());
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      sprintf(strsql,gettext("Не найдено единицу измерения %s !"),ei.ravno());
      iceb_t_soob(strsql);
      naiei.new_plus("");
     }
    else
     {
  //   sprintf(naiei,"%s:%s %s",gettext("Единица измерения"),ei,row1[0]);
      naiei.new_plus(gettext("Единица измерения"));
      naiei.plus(":",ei.ravno());
      naiei.plus(" ",row1[0]);
     }
    SOOB.VVOD_delete();
    if(naiskl.ravno()[0] != '\0')
     SOOB.VVOD_SPISOK_add_MD(naiskl.ravno());
    if(naishet.ravno()[0] != '\0')
     SOOB.VVOD_SPISOK_add_MD(naishet.ravno());
    if(naiei.ravno()[0] != '\0')
      SOOB.VVOD_SPISOK_add_MD(naiei.ravno());

    if(ktozap[kom] != 0)
     {
      sprintf(strsql,"%s %s",gettext("Записал     :"),iceb_t_kszap(ktozap[kom]));
      SOOB.VVOD_SPISOK_add_MD(strsql);
             
     }
    if(vrem[kom] != 0)
     {
      bf=localtime(&vrem[kom]);
      sprintf(strsql,"%s %d.%d.%d%s %s %d:%d:%d",
      gettext("Дата записи :"),
      bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
      gettext("г."),
      gettext("Время"),
      bf->tm_hour,bf->tm_min,bf->tm_sec);
      SOOB.VVOD_SPISOK_add_MD(strsql);
     }
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    goto naz;

  case SFK5: /*Расчёт цены*/

    rscena();

    goto naz2;

  case FK6: /*Установить дату остатка*/

fk6:;
     helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
     
     
     VVOD1.VVOD_delete();
     VVOD1.VVOD_SPISOK_add_MD(gettext("Введите дату остатка на карточках"));
     vstr.new_plus("");
     if((kom1=vvod1(&vstr,11,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
      {
       goto naz2;
      }
     if(kom1 == ESC)
      {
       goto naz2;
      }

    if(rsdat(&d,&m,&g,vstr.ravno(),1) != 0)     
     {
      iceb_t_soob(gettext("Не верно введена дата !"));
      goto fk6;
     }
    deno=d; meso=m; godo=g;      
    goto naz1;

  case FK7: /*С отатками/без остатков*/
    metkaost++;
    if(metkaost > 1)
      metkaost=0;
    pozz=0;
    goto naz;

  case FK8: //карточки только соответствующего склада
    if(skl == 0)
      goto naz;
    metkaskl++;
    if(metkaskl > 1)
      metkaskl=0;
    pozz=0;
    goto naz;

  case FK9: //карточки c номером заказа

    metka_sort++;
    if(metka_sort > 1)
      metka_sort=0;
    pozz=0;
    goto naz1;
            
  default:
    goto naz2;
 }
return(0);
}

/*********************************************/
/*Подпрограмма ввода новой карточки материалла*/
/*********************************************/
/*Если записали возвращаем 1
  если нет 0
  */
void vkm(short dd,short md,short gd,int skl,const char *nomdok,
int kodm,
int kodmm, //Если не равен нолю, привязка к карточке
int lnds,
const char *kodop, //Код операции
double cenadok, //Цена в документе (для режима привязки к карточкам)
double kolihdok,   //Количество (для режима привязки к карточкам)
float pnds)
{
SQL_str         row;
double		cenav;
int             i1,K;
int             N;
double          krt;  /*Кратность или количество в упаковке*/
double          fas;  /*Фасовка или вес одной единицы*/
int             kodt; /*Код тары*/
double		nds,bb;
int		mnds; /* "+" Цена с НДС"*/
short		mshrt=0;  /*0-обычный счёт 1-розничной торговли*/
short		metkanz=0; //0-обычная операция 1-приход из производства
char		strsql[1024];
int		nk=0,nk1=0;
class iceb_tu_str shbm("");
double		cena;
class iceb_tu_str ei("");
double		kolih;
class iceb_tu_str bros("");
class iceb_tu_str naim("");
class iceb_tu_str naiei("");
class iceb_tu_str naimat("");
class iceb_tu_str naimskl("");
class iceb_tu_str kodgr("");
class iceb_tu_str naimgrup("");
class iceb_tu_str naimsh("");
short		msnk=0; /*0-создать новую карточку и привязать к ней
			1 - привязать к найденной*/
class iceb_tu_str kod("");
short		denv=0,mesv=0,godv=0; //Дата начала эксплуатации для малоценки
class iceb_tu_str innom(""); //Инвентарный номер для малоценки
class iceb_tu_str rnd(""); //Регистрационный номер мед. препарата
class iceb_tu_str nomz(""); //Номер заказа
struct OPSHET	shetv;
short deng=0,mesg=0,godg=0;

if(iceb_t_poldan("Коды операций прихода из производства",&bros,"matnast.alx") == 0)
 if(proverka(bros.ravno(),kodop,0,1) == 0)
   metkanz=1;

VVOD MENU(3);
VVOD VV(0);


VV.VVOD_SPISOK_add_data(10);//0
VV.VVOD_SPISOK_add_data(20);//1
VV.VVOD_SPISOK_add_data(20);//2
VV.VVOD_SPISOK_add_data(20);//3
VV.VVOD_SPISOK_add_data(10);//4
VV.VVOD_SPISOK_add_data(10);//5
VV.VVOD_SPISOK_add_data(10);//6
VV.VVOD_SPISOK_add_data(10);//7
VV.VVOD_SPISOK_add_data(10);//8
VV.VVOD_SPISOK_add_data(20);//9
VV.VVOD_SPISOK_add_data(20);//10
VV.VVOD_SPISOK_add_data(11);//11
VV.VVOD_SPISOK_add_data(20);//12 счёт получени
VV.VVOD_SPISOK_add_data(20);//13 Обязательно должно быть: это либо номер заказа либо цена продажи

msnk=0;
nk=nomkr(skl);

mshrt=mnds=0;
N=0;

cena=0.;
kolih=0.;
nds=0.;
SQLCURSOR cur1;
/*Читаем материал с его параметрами*/

sprintf(strsql,"select * from Material where kodm=%d",kodm);

if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден код материалла"),kodm);
  iceb_t_soob(strsql);
  return;
 }

naimat.new_plus(row[2]);
kodgr.new_plus(row[1]);


ei.new_plus(row[4]);
if(cenadok != 0.)
 {
  sprintf(strsql,"%.10g",cenadok);
  VV.VVOD_SPISOK_zapis_data(1,strsql);
 }
if(kolihdok > 0.)
 {
  sprintf(strsql,"%.10g",kolihdok);
  VV.VVOD_SPISOK_zapis_data(2,strsql);
 }
VV.VVOD_SPISOK_zapis_data(3,ei.ravno());
sprintf(strsql,"%d",nk);
VV.VVOD_SPISOK_zapis_data(4,strsql);

krt=atof(row[7]);
if(krt != 0)
 {
  sprintf(strsql,"%.8g",krt);
  VV.data_plus(5,strsql);
 }
fas=atof(row[8]);
if(fas != 0.)
 {
  sprintf(strsql,"%.8g",fas);
  VV.data_plus(6,strsql);
 }
kodt=atoi(row[9]);
if(kodt != 0)
  VV.data_plus(7,kodt);

nds=0.;
if(lnds == 0)
 {
  nds=pnds;
  sprintf(strsql,"%.5g",pnds);
  VV.data_plus(8,strsql);
 }

/*Читаем наименование группы*/
if(kodgr.ravno_atoi() != 0)
 {
  sprintf(strsql,"select naik from Grup where kod=%d",kodgr.ravno_atoi());

  if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найдено группу"),kodgr.ravno());
    iceb_t_soob(strsql);
   } 
  else
   naimgrup.new_plus(row[0]);
 }
 
/*Читаем наименование склада*/
sprintf(strsql,"select naik from Sklad \
where kod=%d",skl);

if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
 {
  sprintf(strsql,gettext("Не найден склад %d в списке складов !"),skl);
  iceb_t_soob(strsql);
 } 
else
 naimskl.new_plus(row[0]);
 
naz:;

VV.VVOD_delete_MD();
VV.VVOD_SPISOK_add_MD(gettext("Счёт учета..................."));//0
VV.VVOD_SPISOK_add_MD(gettext("Цена приобретения............"));//1
VV.VVOD_SPISOK_add_MD(gettext("Количество..................."));//2
VV.VVOD_SPISOK_add_MD(gettext("Единица измерения............"));//3
VV.VVOD_SPISOK_add_MD(gettext("Номер карточки..............."));//4
VV.VVOD_SPISOK_add_MD(gettext("Кратность...................."));//5
VV.VVOD_SPISOK_add_MD(gettext("Фасовка......................"));//6
VV.VVOD_SPISOK_add_MD(gettext("Код тары....................."));//7
sprintf(strsql,gettext("НДС.(+%2.f цена с НДС)........."),pnds);//8
VV.VVOD_SPISOK_add_MD(strsql);
VV.VVOD_SPISOK_add_MD(gettext("Сумма........................"));//9
VV.VVOD_SPISOK_add_MD(gettext("Цена единицы с НДС..........."));//10
VV.VVOD_SPISOK_add_MD(gettext("Конечная дата использования.."));//11
VV.VVOD_SPISOK_add_MD(gettext("Cчёт получения..............."));//12

if(mshrt == 1 && metkanz == 0)
  VV.VVOD_SPISOK_add_MD(gettext("Цена реализации.............."));//13
if(metkanz == 1)
  VV.VVOD_SPISOK_add_MD(gettext("Номер заказа................."));//13
  
VV.VVOD_delete_ZAG();
VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой карточки материалла"));
refresh();
sprintf(strsql,gettext("Материал: %d %s"),kodm,naimat.ravno());
VV.VVOD_SPISOK_add_ZAG(strsql);
sprintf(strsql,"%s: %s %s",gettext("Группа"),kodgr.ravno(),naimgrup.ravno());
VV.VVOD_SPISOK_add_ZAG(strsql);
sprintf(strsql,"%s: %d %s",gettext("Склад"),skl,naimskl.ravno());
VV.VVOD_SPISOK_add_ZAG(strsql);
if(naimsh.ravno()[0] != '\0')
 {
  sprintf(strsql,"%s: %s",gettext("Счёт учета"),naimsh.ravno());
  VV.VVOD_SPISOK_add_ZAG(strsql);
 }


helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("реквизиты"),
"F4",gettext("ном. карточки"),
"F5",gettext("расч.цены"),
"F6/-",gettext("запись"),
"F7",gettext("запись"),
"F8",gettext("запись"),
"F10",gettext("выход"),NULL);

VV.vramka(-1,-1,0);


VV.data_plus(0,shbm.ravno());
if(cena > 0.00000000009)
 {
  sprintf(strsql,"%.10g",cena);
  VV.VVOD_SPISOK_zapis_data(1,strsql);
 }

VV.VVOD_SPISOK_zapis_data(3,ei.ravno());
if(kolih > 0.)
 {
  sprintf(strsql,"%.10g",kolih);
  VV.VVOD_SPISOK_zapis_data(2,strsql);
 }

if(nk > 0)
 {
  sprintf(strsql,"%d",nk);
  VV.VVOD_SPISOK_zapis_data(4,strsql);
 }

for(;;)
 {
  attron(VV.VVOD_return_cs(iceb_CFS)); 

  VV.vprintw(N,stdscr);
    
  K=getch();

  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));


  VV.vprintw(N,stdscr);

  if(K == KEY_RESIZE)
   {
//    getch();
    goto naz;
   }

  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));

    VV.vprintw_clear(N,stdscr);

    if(N == 0) /*Проверяем есть ли такой счет*/
     {
      VV.vgetstr(N,stdscr);

      attroff(VV.VVOD_return_cs(iceb_CFV));
      attron(VV.VVOD_return_cs(iceb_CFM));

      if(VV.VVOD_SPISOK_return_data(N)[0] == '\0')
        goto naz;

      shbm.new_plus(VV.VVOD_SPISOK_return_data(N));

      if(prsh1(shbm.ravno(),&shetv) != 0)
	goto naz;

     /*Проверяем материальный это счёт или нет*/
//      if(proverka(mtsh,VV.VVOD_SPISOK_return_data(0),1,1) != 0)
      if(proverka(spmatshets.ravno(),VV.VVOD_SPISOK_return_data(0),1,1) != 0)
       {
        VVOD SOOB(1);
        sprintf(strsql,gettext("Счёт %s не материальный !"),shbm.ravno());
        SOOB.VVOD_SPISOK_add_MD(strsql);
        SOOB.VVOD_SPISOK_add_MD(spmatshets.ravno());
        soobshw(&SOOB,stdscr,-1,-1,0,1);

        VV.VVOD_SPISOK_zapis_data(N,"");
        shbm.new_plus("");
        goto naz;
       }
      
     /*Читаем наименование счета*/
     sprintf(strsql,"select nais from Plansh where ns='%s'",shbm.ravno());
     if(sql_readkey(&bd,strsql,&row,&cur1) == 1)
       naimsh.new_plus(row[0]);     
     else
       naimsh.new_plus("");
     /*Проверяем не является ли счёт счетом розничной торговли*/
     if(shrt.getdlinna() > 1 && metkanz == 0)
      {
       mshrt=1;
       if(pole1(shrt.ravno(),VV.VVOD_SPISOK_return_data(0),',',0,&i1) != 0)
         if(SRAV(shrt.ravno(),VV.VVOD_SPISOK_return_data(0),0) != 0)
          {
           mshrt=0;
           VV.data_plus(13,"");
          }
       N++;
       goto naz;
      }    

     }

    if(N == 1 ) /*Цена*/
     {
      VV.vgetstr(N,stdscr);
      
      attroff(VV.VVOD_return_cs(iceb_CFV));
      attron(VV.VVOD_return_cs(iceb_CFM));
      cena=ATOFM(VV.VVOD_SPISOK_return_data(N));
      cena=okrug(cena,okrcn);
      bb=cena+cena*nds/100.;
      bb=okrug(bb,okrg1);

      sprintf(strsql,"%.10g",bb);
      VV.VVOD_SPISOK_zapis_data(10,strsql);

      bb=kolih*cena;
      bb=okrug(bb,okrg1);
      sprintf(strsql,"%.10g",bb);
      VV.VVOD_SPISOK_zapis_data(9,strsql);


      N++;
      goto naz;
     }

    if(N == 2) /*Количество*/
     {
      VV.vgetstr(N,stdscr);
      attroff(VV.VVOD_return_cs(iceb_CFV));
      attron(VV.VVOD_return_cs(iceb_CFM));
      kolih=ATOFM(VV.VVOD_SPISOK_return_data(N));
      if(cena != 0.)
       { 
        bb=kolih*cena;
        bb=okrug(bb,okrg1);
        sprintf(strsql,"%.10g",bb);
        VV.VVOD_SPISOK_zapis_data(9,strsql);
       }
      else
       {
        if(ATOFM(VV.VVOD_SPISOK_return_data(9)) != 0.)
         {
          bb=ATOFM(VV.VVOD_SPISOK_return_data(N))/kolih;
          cena=okrug(bb,okrcn);
          sprintf(strsql,"%.10g",cena);
          VV.VVOD_SPISOK_zapis_data(1,strsql);
          goto naz;
         }
       }
      N++;
      goto naz;
     }

    if(N == 3) /*единица измерения*/
     {
      naiei.new_plus("");
      if(iceb_tl_ei(2,&ei,&naiei) == 0)
       {
        VV.VVOD_SPISOK_zapis_data(3,ei.ravno());
       }
      N++;
      goto naz;
     }

    if(N == 4) /*Номер карточки*/
     {
      VV.vgetstr(N,stdscr);
      attroff(VV.VVOD_return_cs(iceb_CFV));
      attron(VV.VVOD_return_cs(iceb_CFM));
      nk=(int)ATOFM(VV.VVOD_SPISOK_return_data(N));
      if(nk < 0)
       {
        iceb_t_soob(gettext("Номер карточки не может быть отрицательным !"));
        nk=0;
        VV.VVOD_SPISOK_zapis_data(N,"");
        goto naz;
       
       }

     }

    if(N == 5) /*Кратность*/
     {
      VV.vgetstr(N,stdscr);
      attroff(VV.VVOD_return_cs(iceb_CFV));
      attron(VV.VVOD_return_cs(iceb_CFM));
      krt=ATOFM(VV.VVOD_SPISOK_return_data(N));
     }

    if(N == 6) /*Фасовка*/
     {
      VV.vgetstr(N,stdscr);
      attroff(VV.VVOD_return_cs(iceb_CFV));
      attron(VV.VVOD_return_cs(iceb_CFM));
      fas=ATOFM(VV.VVOD_SPISOK_return_data(N));
     }

    if(N == 7) /*Код тары*/
     {
      VV.vgetstr(N,stdscr);
      attroff(VV.VVOD_return_cs(iceb_CFV));
      attron(VV.VVOD_return_cs(iceb_CFM));
      kodt=(int)ATOFM(VV.VVOD_SPISOK_return_data(N));
     }

    if(N == 8) /*НДС*/
     {
      VV.vgetstr(N,stdscr);
      attroff(VV.VVOD_return_cs(iceb_CFV));
      attron(VV.VVOD_return_cs(iceb_CFM));
      nds=ATOFM(VV.VVOD_SPISOK_return_data(N));
      mnds=0;
      if(VV.VVOD_SPISOK_return_data(N)[0] == '+')
        mnds=1;
      nds=okrug(nds,0.01);

      cena=ATOFM(VV.VVOD_SPISOK_return_data(1));
      cena=okrug(cena,okrcn);
      bb=cena+cena*nds/100.;
      bb=okrug(bb,okrg1);

      sprintf(strsql,"%.10g",bb);
      VV.VVOD_SPISOK_zapis_data(10,strsql);

      goto naz;
     }

    if(N == 9) /*Сумма*/
     {
      VV.vgetstr(N,stdscr);
      attroff(VV.VVOD_return_cs(iceb_CFV));
      attron(VV.VVOD_return_cs(iceb_CFM));
      if(kolih > 0.)
       {
        bb=ATOFM(VV.VVOD_SPISOK_return_data(N))/kolih;
        cena=okrug(bb,okrcn);
        sprintf(strsql,"%.10g",cena);
        VV.VVOD_SPISOK_zapis_data(1,strsql);

        bb=cena+cena*nds/100;
        bb=okrug(bb,okrg1);
        sprintf(strsql,"%.10g",bb);
        VV.VVOD_SPISOK_zapis_data(10,strsql);
        goto naz;
       }
     }
    if(N == 10) /*Ввод цены с ндс*/
     {
      VV.vgetstr(N,stdscr);
      attroff(VV.VVOD_return_cs(iceb_CFV));
      attron(VV.VVOD_return_cs(iceb_CFM));
      bb=ATOFM(VV.VVOD_SPISOK_return_data(N));
      sprintf(strsql,"%.10g",bb);
      VV.VVOD_SPISOK_zapis_data(10,strsql);

      bb=bb-bb*nds/(100.+nds);
      cena=okrug(bb,okrcn);
      sprintf(strsql,"%.10g",cena);
      VV.VVOD_SPISOK_zapis_data(1,strsql);
      bb=kolih*cena;
      bb=okrug(bb,okrg1);
      sprintf(strsql,"%.10g",bb);
      VV.VVOD_SPISOK_zapis_data(9,strsql);
      goto naz;
     }

    if( N == 11) //Дата конечного использования
     {
      VV.vgetstr(N,stdscr);
     }

    if(mshrt == 1 && metkanz == 0 && N == 13) /*Ввод входной цены*/
     {
      VV.vgetstr(N,stdscr);
     }

    if(metkanz == 1 && N == 13) //Ввод номера заказа
     {
      VV.vgetstr(N,stdscr);
     }

    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    VV.vprintw(N,stdscr);
    N++;
   }
  if(K==SDN) /*Стрелка вниз*/
   {
    N++;
   }
  if(K==SUP) /*Стрелка вверх*/
   {
    N--;
   }
  if(N > VV.KLST-1)
   {
    N=0;
   }
  if(N < 0)
   {
    N=VV.KLST-1;
   }

  if(K == ESC || K == FK10)
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    N=-1;
    break;
   }
  if(K == FK1) /*Помощь*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    GDITE();
    iceb_t_pdoc("matu2_1_2g1.txt");
    clear();
    goto naz;
   }

  if(K == FK2 || K == PLU || K == FK6 || K == OTN || K == FK7 || K == FK8) /*Записываем*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    if(K == FK6 || K == OTN) //Записываем как малоценку
     {
      
      if(vmatmc(&denv,&mesv,&godv,&innom,kolih) != 0)
        goto naz;
     }
    if(K == FK7) //Записываем как медицинский препарат
     {
      if(vmatmed(&denv,&mesv,&godv,&innom,&rnd) != 0)
        goto naz;
     }
    if(K == FK8) //Записываем сертификат соответствия и дату выдачи сертификата
     {
      if(vmat_alkog(&denv,&mesv,&godv,&rnd) != 0)
        goto naz;
     }

    GDITE();
    nk=VV.data_ravno_atoi(4);
    if( nk == 0 || shbm.ravno()[0] == '\0' ||\
       ei.getdlinna() <= 1 || kolih == 0.)
     {
      iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
      goto naz;
     }

    if(mshrt == 1 && metkanz == 0 && VV.VVOD_SPISOK_return_data(13)[0] == '\0')
     {
      iceb_t_soob(gettext("Не введена цена продажи !"));
      goto naz;
     }

    if(prsh1(shbm.ravno(),&shetv) != 0)
      goto naz;
    if(VV.data_ravno(12)[0] != '\0')
     if(prsh1(VV.data_ravno(12),&shetv) != 0)
       goto naz;

    if(VV.VVOD_SPISOK_return_data(11)[0] != '\0')
     if(rsdat(&deng,&mesg,&godg,VV.VVOD_SPISOK_return_data(11),1) != 0)
      {
        iceb_t_soob(gettext("Неправильно введена дата конечного использования !"));
        goto naz;
      }

//    if(proverka(mtsh,shbm.ravno(),1,1) != 0)
    if(proverka(spmatshets.ravno(),shbm.ravno(),1,1) != 0)
     {
      VVOD SOOB(1);
      sprintf(strsql,gettext("Счёт %s не материальный !"),shbm.ravno());
      SOOB.VVOD_SPISOK_add_MD(strsql);
      SOOB.VVOD_SPISOK_add_MD(spmatshets.ravno());
      soobshw(&SOOB,stdscr,-1,-1,0,1);

      VV.VVOD_SPISOK_zapis_data(0,"");
      shbm.new_plus("");
      goto naz;
     }
    
    /*Проверяем может карточка с таким номером уже есть на складе*/
    sprintf(strsql,"select nomk from Kart where sklad=%d and nomk=%d",
    skl,nk);
    if(sql_readkey(&bd,strsql,&row,&cur1) > 0)
     {
      VVOD SOOB(1);
      sprintf(strsql,gettext("Карточка с номером %d уже есть на складе %d !"),nk,skl);
      SOOB.VVOD_SPISOK_add_MD(strsql);
      soobshw(&SOOB,stdscr,-1,-1,0,1);
      goto naz;
     }    

    krt=ATOFM(VV.VVOD_SPISOK_return_data(5));
    fas=ATOFM(VV.VVOD_SPISOK_return_data(6));
    kodt=(int)ATOFM(VV.VVOD_SPISOK_return_data(7));
    cenav=0.;
    if(metkanz == 0)
     cenav=ATOFM(VV.VVOD_SPISOK_return_data(13));
    if(metkanz == 1)
     nomz.new_plus(VV.VVOD_SPISOK_return_data(13));

    if(nomz.ravno()[0] == '\0' && innom.ravno()[0] == '\0')
     {     
      /*Проверяем может карточка с введенными уникальными реквизитами уже есть на складе*/
      sprintf(strsql,"select nomk from Kart where kodm=%d and sklad=%d \
and shetu='%s' and cena=%.10g and ei='%s' and nds=%.10g and cenap=%.10g \
and krat=%.10g and mnds=%d and dat_god='%04d-%02d-%02d' and rnd='%s'",
      kodm,skl,shbm.ravno(),cena,ei.ravno(),nds,cenav,krt,mnds,godg,mesg,deng,rnd.ravno());
  /*
      printw("\n%s\n",strsql);
      OSTANOV();
  */
      if(sql_readkey(&bd,strsql,&row,&cur1) > 0)
       {
        nk1=atoi(row[0]);
        /*Проверяем есть ли уже такая запись в документе*/
        sprintf(strsql,"select datd from Dokummat1 where sklad=%d and \
nomd='%s' and kodm=%d and nomkar=%d and datd='%d-%d-%d'",
        skl,nomdok,kodm,nk1,gd,md,dd);

        if(sql_readkey(&bd,strsql,&row,&cur1) >= 1)
         {
          iceb_t_soob(gettext("Такая запись в документе уже есть !"));
          goto naz;
         }

        VVOD SOOB(1);
        sprintf(strsql,gettext("Карточка с номером %d имеет такиеже реквизиты !"),nk1);
        SOOB.VVOD_SPISOK_add_MD(strsql);
        SOOB.VVOD_SPISOK_add_MD(gettext("Привязываем запись к этой карточке"));
        soobshw(&SOOB,stdscr,-1,-1,0,1);
        nk=nk1;
        msnk=1;
       }    
     }    

    if(kodmm != 0)
     {
      /*Удяляем старую запись в документе*/
      sprintf(strsql,"delete from Dokummat1 where datd='%04d.%d.%d' and \
sklad=%d and nomd='%s' and kodm=%d and nomkar=0",
      gd,md,dd,skl,nomdok,kodm);
      if(sql_zap(&bd,strsql) != 0)
        msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);
             
     }      

    if(msnk == 1)
       zapvdok(dd,md,gd,skl,nk,kodm,nomdok,kolih,cena,ei.ravno(),nds,mnds,0,1,0,VV.data_ravno(12),nomz.ravno(),0,"");
    else    
     {
      if(innom.ravno()[0] != '\0' && K == FK6)
       if(provinnom(innom.ravno()) != 0)
        goto naz;

      if(zapkar(skl,nk,kodm,cena,ei.ravno(),shbm.ravno(),krt,fas,kodt,nds,mnds,cenav,denv,mesv,godv,innom.ravno(),rnd.ravno(),nomz.ravno(),deng,mesg,godg,0) == 0)
         zapvdok(dd,md,gd,skl,nk,kodm,nomdok,kolih,cena,ei.ravno(),nds,mnds,0,1,0,VV.data_ravno(12),nomz.ravno(),0,"");
      else
        goto naz;
     }    
    break;
   }

  if(K == FK3) /*Смотрим реквизиты*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS)); 
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите  нужное и нажмите Enter"));

    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка единиц измерения"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    
    int kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 2 :
      case -1:
       goto naz;


      case 0 :
        if(vibrek("Plansh",&kod,&naim) == 0)
         {
          VV.data_plus(0,kod.ravno());
          shbm.new_plus(kod.ravno());
          naimsh.new_plus(naim.ravno());
         }
        goto naz;

      case 1 :
        if(vibrek("Edizmer",&kod,&naim) == 0)
         {
          VV.data_plus(3,kod.ravno());
          ei.new_plus(kod.ravno());
          naiei.new_plus(naim.ravno());
         }
        goto naz;
      }
     goto naz;
   }
  if(K == FK4) /*Номер карточки*/
   {
     attroff(VV.VVOD_return_cs(iceb_CFS)); 
     nk=nomkr(skl);
     VV.data_plus(4,nk);
     goto naz;
   }
  if(K == FK5) /*Расчёт цены*/
   {
     attroff(VV.VVOD_return_cs(iceb_CFS)); 

     rscena();
     clear();
     goto naz;
   }
 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
clear();
refresh();
return;
}

/*****************/
/*Ввод в накладную без привязки к карточке*/
/******************************************/

void vbrp(short dd,short md,short gd,int skl,const char *nomdok,
int kodm,int tipz,const char *kodop,float pnds)
{
double		kolih,cena;
double          bb,bb1;
double		nds;
int		K,N;
double		krt; /*Кратность*/
int		mnds;
char		strsql[512];
SQL_str         row;
class iceb_tu_str bros("");
class iceb_tu_str ei("");
double		ndscen;
class iceb_tu_str naim("");
class iceb_tu_str nomz(""); //Номер заказа
short		metkanz=0; //1-операция прихода из производства
double		suma=0.;
struct OPSHET	shetv;


if(iceb_t_poldan("Коды операций прихода из производства",&bros,"matnast.alx") == 0)
 if(proverka(bros.ravno(),kodop,0,1) == 0)
   metkanz=1;

VVOD VV(0);
VV.VVOD_SPISOK_add_MD(gettext("Количество........."));//0
VV.VVOD_SPISOK_add_MD(gettext("Цена с НДС........."));//1
VV.VVOD_SPISOK_add_MD(gettext("Цена без НДС......."));//2
VV.VVOD_SPISOK_add_MD(gettext("Единица измерения.."));//3
VV.VVOD_SPISOK_add_MD(gettext("НДС................"));//4
VV.VVOD_SPISOK_add_MD(gettext("Количество тары...."));//5
VV.VVOD_SPISOK_add_MD(gettext("Сумма без НДС......"));//6
VV.VVOD_SPISOK_add_MD(gettext("Счёт получения....."));//7
if(metkanz == 1)
  VV.VVOD_SPISOK_add_MD(gettext("Номер заказа......."));//8

VV.VVOD_SPISOK_add_data(20);//0
VV.VVOD_SPISOK_add_data(20);//1
VV.VVOD_SPISOK_add_data(20);//2
VV.VVOD_SPISOK_add_data(8);//3
VV.VVOD_SPISOK_add_data(10);//4
VV.VVOD_SPISOK_add_data(10);//5
VV.VVOD_SPISOK_add_data(20);//6
VV.VVOD_SPISOK_add_data(20);//7
VV.VVOD_SPISOK_add_data(20);//8


/*Узнаем наименование материалла*/
mnds=0;

krt=N=0;
SQLCURSOR cur1;
sprintf(strsql,"select * from Material \
where kodm=%d",kodm);

if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден код материалла"),kodm);
  iceb_t_soob(strsql);
  return;
 }
  
naim.new_plus(row[2]);
krt=atof(row[7]);
ei.new_plus(row[4]);

bb=atof(row[6]);
ndscen=atoi(row[12]);

if( bb != 0. && ndscen == 0.)
 {
  bb=bb-bb*pnds/(100.+pnds);
  bb=okrug(bb,okrcn);
 }

if(bb != 0)
 {
  sprintf(strsql,"%.10g",bb);
  VV.data_plus(2,strsql);
 }
bb=bb+bb*pnds/100.;
bb=okrug(bb,okrg1);
if(bb != 0.)
 {
  sprintf(strsql,"%.10g",bb);
  VV.data_plus(1,strsql);
 }
VV.data_plus(3,ei.ravno());
VV.data_plus(4,pnds);

naz:;

clear();

VV.VVOD_delete_ZAG();

VV.VVOD_SPISOK_add_ZAG(gettext("Ввод материалла в документ без привязки к карточке"));

sprintf(strsql,gettext("Материал %d %s"),kodm,naim.ravno());
VV.VVOD_SPISOK_add_ZAG(strsql);

suma=ATOFM(VV.VVOD_SPISOK_return_data(0))*ATOFM(VV.VVOD_SPISOK_return_data(2));
suma=okrug(suma,okrg1);


helstr(LINES-1,0,"F2/+",gettext("запись"),
"F10",gettext("выход"),NULL);


VV.vramka(-1,-1,0);


for(;;)
 {
  attron(VV.VVOD_return_cs(iceb_CFS)); 

  VV.vprintw(N,stdscr);

  K=getch();

  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));

  VV.vprintw(N,stdscr);

  if(K == KEY_RESIZE)
   {
    goto naz;
   }

  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));
    VV.vprintw_clear(N,stdscr);

    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 0) /*Количество*/
     {
      if(VV.VVOD_SPISOK_return_data(2)[0] != '\0')
       {
        bb=atof(VV.VVOD_SPISOK_return_data(N))*atof(VV.VVOD_SPISOK_return_data(2));
        bb=okrug(bb,okrg1);
        sprintf(strsql,"%.10g",bb);
        VV.VVOD_SPISOK_zapis_data(6,strsql);
        
       }

      if(krt != 0.)
       {
        attroff(VV.VVOD_return_cs(iceb_CFM));
        attron(VV.VVOD_return_cs(iceb_CB));
        bb=ATOFM(VV.VVOD_SPISOK_return_data(N));
        bb=bb/krt;
        sprintf(strsql,"%.f",bb);
        VV.VVOD_SPISOK_zapis_data(5,strsql);
        
       }       
      N++;
      goto naz;
     }

    if(N == 1) /*Ввод цены с ндс*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFM));
      attron(VV.VVOD_return_cs(iceb_CB));

      /*Ввод цены с процентом наценки*/
      if(polen(VV.VVOD_SPISOK_return_data(N),&bb1,2,'/') == 0)
       {
//	bb1=ATOFM(bros);
	bb=ATOFM(VV.VVOD_SPISOK_return_data(N));
	bb=bb+bb*bb1/100.;
       }
      else
	bb=ATOFM(VV.VVOD_SPISOK_return_data(N));

      sprintf(strsql,"%.10g",bb);
      VV.VVOD_SPISOK_zapis_data(1,strsql);

      bb=bb-bb*pnds/(100.+pnds);
      bb=okrug(bb,okrcn);
      sprintf(strsql,"%.10g",bb);
      VV.VVOD_SPISOK_zapis_data(2,strsql);

      bb=bb*ATOFM(VV.VVOD_SPISOK_return_data(0));
      bb=okrug(bb,okrg1);
      sprintf(strsql,"%.10g",bb);
      VV.VVOD_SPISOK_zapis_data(6,strsql);
      
      N+=2;
      goto naz;
     }
    if(N == 2) /*Ввод цены без НДС*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFM));
      attron(VV.VVOD_return_cs(iceb_CB));
      /*Ввод цены с процентом наценки*/
      if(polen(VV.VVOD_SPISOK_return_data(N),&bb1,2,'/') == 0)
       {
//	bb1=ATOFM(bros);
	bb=ATOFM(VV.VVOD_SPISOK_return_data(N));
	bb=bb+bb*bb1/100.;
       }
      else
	bb=ATOFM(VV.VVOD_SPISOK_return_data(N));
      double cena=0.;
      cena=bb=okrug(bb,okrcn);
      sprintf(strsql,"%.10g",bb);
      VV.VVOD_SPISOK_zapis_data(2,strsql);

      bb=bb+bb*pnds/100.;
      bb=okrug(bb,okrg1);

      sprintf(strsql,"%.10g",bb);
      VV.VVOD_SPISOK_zapis_data(1,strsql);


      bb=cena*ATOFM(VV.VVOD_SPISOK_return_data(0));
      bb=okrug(bb,okrg1);
      sprintf(strsql,"%.10g",bb);
      VV.VVOD_SPISOK_zapis_data(6,strsql);

      goto naz;
            
     }

    if(N == 5) /*Количество тары*/
     {
      if(krt != 0.)
       {
        attroff(VV.VVOD_return_cs(iceb_CFM));
        attron(VV.VVOD_return_cs(iceb_CB));
        bb=ATOFM(VV.VVOD_SPISOK_return_data(N));
        bb=bb*krt;
        sprintf(strsql,"%.10g",bb);
        VV.VVOD_SPISOK_zapis_data(0,strsql);

        bb=bb*ATOFM(VV.VVOD_SPISOK_return_data(2));
        bb=okrug(bb,okrg1);
        sprintf(strsql,"%.10g",bb);
        VV.VVOD_SPISOK_zapis_data(6,strsql);
        
        goto naz;
        
       }       


     }

    if(N == 6) //Сумма без НДС
     {
      bb=ATOFM(VV.VVOD_SPISOK_return_data(N))/ATOFM(VV.VVOD_SPISOK_return_data(0));
      bb=okrug(bb,okrcn);
      sprintf(strsql,"%.10g",bb);
      VV.VVOD_SPISOK_zapis_data(2,strsql); //Цена без НДС

      bb=bb+bb*pnds/100.;
      bb=okrug(bb,okrg1);

      sprintf(strsql,"%.10g",bb);
      VV.VVOD_SPISOK_zapis_data(1,strsql);
      goto naz;
     }

    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));
    VV.vprintw(N,stdscr);
    N++;
   }
  if(K==SDN) /*Стрелка вниз*/
   {
    N++;
   }
  if(K==SUP) /*Стрелка вверх*/
   {
    N--;
   }
  if(N > VV.KLST-1)
   {
    N=0;
   }
  if(N < 0)
   {
    N=VV.KLST-1;
   }

  if(K == ESC || K == FK10)
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    N=-1;
    break;
   }

  if(K == FK2 || K == PLU) /*Записываем*/
   {
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));
     if(VV.VVOD_SPISOK_return_data(0)[0] == '\0' || VV.VVOD_SPISOK_return_data(1)[0] == '\0' || VV.VVOD_SPISOK_return_data(2)[0] == '\0' 
     || VV.VVOD_SPISOK_return_data(3)[0] == '\0')
      {
       iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
       goto naz;
      }
    if(VV.data_ravno(7)[0] != '\0')
     if(prsh1(VV.data_ravno(7),&shetv) != 0)
	goto naz;

    /*Проверяем единицу измерения*/
    sprintf(strsql,"select kod from Edizmer where kod='%s'",
    VV.VVOD_SPISOK_return_data(3));
    if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
     {
       sprintf(strsql,gettext("Не нашли единицу измерения %s !"),VV.VVOD_SPISOK_return_data(3));
       iceb_t_soob(strsql);
       goto naz;
     }

    cena=ATOFM(VV.VVOD_SPISOK_return_data(2));
    cena=okrug(cena,okrcn);

    kolih=ATOFM(VV.VVOD_SPISOK_return_data(0));
    nds=ATOFM(VV.VVOD_SPISOK_return_data(4));
    nds=okrug(nds,0.01);
    if(metkanz == 1)
      nomz.new_plus(VV.VVOD_SPISOK_return_data(8));
    zapvdok(dd,md,gd,skl,0,kodm,nomdok,kolih,cena,VV.VVOD_SPISOK_return_data(3),nds,mnds,0,tipz,0,VV.data_ravno(7),nomz.ravno(),0,"");
    break;
   }
 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
clear();
refresh();

}
/********/
/*Шапка */
/*********/
void	vmatsap(short deno,short meso,short godo,
int tipz,
int kodm, //Код материалла
const char *naim, //Наименование материалла
int metkaost, //если 0 все карточки 1-только с остатками
short cp,  //Цель поиска 0-выбор карточки для записи 1-только просмотр карточек
double oostkar,
double oostdok,
int skl,
int metkaskl)
{
char		mkl[10][10];
char		mst[10][64];
int		X,Y;
int		i1,i2;
clear();

move(0,0);
printw("%s %d.%d.%d%s ",
gettext("Остатки по состоянию на"),
deno,meso,godo,
gettext("г."));

if(tipz == 1)
  printw(gettext("Приход"));
if(tipz == 2)
    printw(gettext("Расход"));

move(1,0);

printw("%s: %d %s",gettext("Перечень карточек по материалу"),
kodm,naim);

move(2,0);

printw(gettext("Выберите нужную карточку"));


move(3,0);
printw("   1    2    3    4    5    6        7        8     9  10");

move(Y=2,X=COLS-18);
printw(gettext("Рас-ровка полей:"));

move(++Y,X);
printw(gettext("1-код склада"));
move(++Y,X);
printw(gettext("2-номер карточки"));
move(++Y,X);
printw(gettext("3-счет учета"));
move(++Y,X);
printw(gettext("4-ед. измерения"));
move(++Y,X);
printw(gettext("5-НДС"));
move(++Y,X);
printw(gettext("6-цена учета"));
move(++Y,X);
printw(gettext("7-остаток"));
move(++Y,X);
printw(gettext("8-остаток по док."));
move(++Y,X);
printw(gettext("9-кратность"));
move(++Y,X);
printw(gettext("10-цена розничная"));
move(++Y,X);
printw(gettext("#-к-ка с инв.ном."));

move(Y+=2,X);
printw("%.10g",oostkar);
move(++Y,X);
printw("%.10g",oostdok);
if(metkaost == 1)
 {
  move(++Y,X);
  attron(A_BLINK);
  printw("%s",gettext("C остатками"));
  attroff(A_BLINK);
 }

if(metkaskl == 1)
 {
  move(++Y,X);
  attron(A_BLINK);
  printw("%s:%d",gettext("Cклад"),skl);
  attroff(A_BLINK);
 }
i2=0;
i1=0;
memset(mkl,'\0',sizeof(mkl));
memset(mst,'\0',sizeof(mst));

sprintf(mkl[i2++],"F1");
sprintf(mst[i1++],gettext("пом."));

if(tipz == 1 && cp == 0)
 {
  sprintf(mkl[i2++],"F2/+");
  sprintf(mst[i1++],gettext("н.карт."));
 }

sprintf(mkl[i2++],"F3");
sprintf(mst[i1++],gettext("п.карт."));

if(cp == 0)
 {
  sprintf(mkl[i2++],"F4");
  sprintf(mst[i1++],gettext("б.карт."));
 }

sprintf(mkl[i2++],"F5");
sprintf(mst[i1++],gettext("печать"));

sprintf(mkl[i2++],"F6");
sprintf(mst[i1++],gettext("дата ост."));

sprintf(mkl[i2++],"F7");
sprintf(mst[i1++],gettext("с ост."));

if(skl != 0)
 {
  sprintf(mkl[i2++],"F8");
  sprintf(mst[i1++],gettext("склады"));
 }
sprintf(mkl[i2++],"F9");
sprintf(mst[i1++],gettext("сорт."));

sprintf(mkl[i2++],"F10");
sprintf(mst[i1++],gettext("выход"));

helstr(LINES-1,0,
mkl[0],mst[0],
mkl[1],mst[1],
mkl[2],mst[2],
mkl[3],mst[3],
mkl[4],mst[4],
mkl[5],mst[5],
mkl[6],mst[6],
mkl[7],mst[7],
mkl[8],mst[8],
mkl[9],mst[9],
NULL);

}
/******************************************************/
/*Ввод инвентарного номера и даты ввода в эксплуатацию*/
/*******************************************************/
int  vmatmc(short *denv,short *mesv,short *godv,class iceb_tu_str *innom,double kol)
{

if(kol != 1.)
 {
  iceb_t_soob(gettext("Внимание ! Количество не равно единице !"));
 }

VVOD VV(0);
VV.VVOD_SPISOK_add_MD(gettext("Дата начала эксплуатации.."));
VV.VVOD_SPISOK_add_MD(gettext("Инвентарный номер........."));
VV.VVOD_SPISOK_add_data(11);
VV.VVOD_SPISOK_add_data(16);

nz:;

helstr(LINES-1,0,"F2/+",gettext("запись"),
"F3",gettext("своб.номер"),
"F10",gettext("выход"),NULL);

int kom=VV.vvod(0,1,1,-1,-1);

switch(kom)
 {
  case FK10:
  case ESC:
     return(1);

  case FK2:
  case PLU:
    GDITE();
    *denv=*mesv=*godv=0;
    if(VV.VVOD_SPISOK_return_data(0)[0] != '\0')
     if(rsdat(denv,mesv,godv,VV.VVOD_SPISOK_return_data(0),1) != 0)
      {
       iceb_t_soob(gettext("Не верно введена дата !"));
       goto nz;
      }
   
    if(VV.VVOD_SPISOK_return_data(1)[0] != '\0')
     if(provinnom(VV.VVOD_SPISOK_return_data(1)) != 0)
      goto nz;

    innom->new_plus(VV.VVOD_SPISOK_return_data(1));   
    break;

  case FK3:
    char bros[512];
    sprintf(bros,"%d",invnomer(0));
    VV.VVOD_SPISOK_zapis_data(1,bros);
    goto nz;
          
  default:
    goto nz;
 }

return(0);

}
/******************************************************/
/*Ввод инвентарного номера и даты ввода в эксплуатацию*/
/*******************************************************/
int  vmat_alkog(short *denv,short *mesv,short *godv,class iceb_tu_str *regnom)
{


VVOD VV(0);
VV.VVOD_SPISOK_add_MD("Дата выдачи сертификата соответствия............");
VV.VVOD_SPISOK_add_MD("Регистрационный номер сертификата соответствия..");
VV.VVOD_SPISOK_add_data(11);
VV.VVOD_SPISOK_add_data(20);

nz:;

helstr(LINES-1,0,"F2/+",gettext("запись"),
"F10",gettext("выход"),NULL);

int kom=VV.vvod(0,1,1,-1,-1);

switch(kom)
 {
  case FK10:
  case ESC:
     return(1);

  case FK2:
  case PLU:
    GDITE();
    *denv=*mesv=*godv=0;
    if(VV.VVOD_SPISOK_return_data(0)[0] != '\0')
     if(rsdat(denv,mesv,godv,VV.VVOD_SPISOK_return_data(0),1) != 0)
      {
       iceb_t_soob(gettext("Не верно введена дата !"));
       goto nz;
      }

    regnom->new_plus(VV.VVOD_SPISOK_return_data(1));   
    break;

  default:
    goto nz;
 }

return(0);

}

/******************************************************/
/*Ввод медицинского препарата*/
/*******************************************************/
int  vmatmed(short *denv,short *mesv,short *godv,class iceb_tu_str *innom,class iceb_tu_str *rnd)
{

VVOD VV(0);
VV.VVOD_SPISOK_add_MD(gettext("Дата регистрации медицинского препарата........."));
VV.VVOD_SPISOK_add_MD(gettext("Серия медицинского препарата...................."));
VV.VVOD_SPISOK_add_MD(gettext("Регистрационный номер документа мед. препарата.."));
VV.VVOD_SPISOK_add_data(11);
VV.VVOD_SPISOK_add_data(16);
VV.VVOD_SPISOK_add_data(20);

nz:;

helstr(LINES-1,0,"F2/+",gettext("запись"),
"F10",gettext("выход"),NULL);

int kom=VV.vvod(0,1,1,-1,-1);

switch(kom)
 {
  case FK10:
  case ESC:
     return(1);
  case FK2:
  case PLU:
    GDITE();
    *denv=*mesv=*godv=0;

/***************
    if(VV.VVOD_SPISOK_return_data(0)[0] == '\0' || \
       VV.VVOD_SPISOK_return_data(1)[0] == '\0' || \
       VV.VVOD_SPISOK_return_data(2)[0] == '\0')
     {
      VVOD SOOB(1);
      SOOB.VVOD_SPISOK_add_MD(gettext("Не введены все обязательные реквизиты !"));
      soobshw(&SOOB,stdscr,-1,-1,0,1);
      goto nz;

     }  
*************/
    if(VV.VVOD_SPISOK_return_data(0)[0] != '\0')
     if(rsdat(denv,mesv,godv,VV.VVOD_SPISOK_return_data(0),1) != 0)
      {
       iceb_t_soob(gettext("Не верно введена дата !"));
       goto nz;
      }

    innom->new_plus(VV.VVOD_SPISOK_return_data(1));   
    rnd->new_plus(VV.VVOD_SPISOK_return_data(2));   
    if(rnd->ravno()[0] == '\0')
     rnd->new_plus("N");     
    break;
      
  default:
    goto nz;
 }

return(0);

}

/*******************************/
/* Увеличение количества       */
/*******************************/

void vmatkolp(int skl,const char *nomdok,int kodm,int nk,short gd,short md,short dd)
{
char strsql[512];
class iceb_tu_str vstr("");
int		kom;
VVOD VVOD1(2);

helstr(LINES-1,0,"F10",gettext("выход"),NULL);


VVOD1.VVOD_SPISOK_add_MD(gettext("Введите количество"));
strsql[0]='\0';
//if((kom=vvod1(&vstr,16,&VVOD1,&readscan,stdscr,-1,-1)) == FK10)
if((kom=vvod1(&vstr,16,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
   return;
if(kom == ESC)
  return;

sprintf(strsql,"update Dokummat1 set kolih=kolih+%f where sklad=%d and \
nomd='%s' and kodm=%d and nomkar=%d and datd='%04d-%d-%d'",
vstr.ravno_atof(),skl,nomdok,kodm,nk,gd,md,dd);

if(sql_zap(&bd,strsql) != 0)
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

}
