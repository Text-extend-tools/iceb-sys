/* $Id: dirprov.c,v 5.72 2014/01/31 12:11:33 sasa Exp $ */
/*12.02.2016    18.02.1993      Белых А.И.      dirprov.c
Подпрограмма просмотра проводок для главной книги
*/
#include        <math.h>
#include        "buhg.h"

void shpd(short,short,short,short,short,short,const char*,double,double,long,long);
void     korprov(VVOD*,short,short,short,const char*,const char*,const char*,time_t,int,const char*);
void	 sozkorprov(VVOD*);
void     korprovnbs(VVOD*,short,short,short,const char*,const char*,time_t,int,const char*);
void	 sozkorprovnbs(VVOD*);
int rasprov(SQLCURSOR*,int,class poiprov_rek *poi);
void udgrupprov(SQLCURSOR *cur,long kolstr,class poiprov_rek *poi);
int pdokmu(short dd,short md,short gd,int tipz,int sklad,const char *nomdok);
int pdokusl(short dd,short md,short gd,int podr,const char *nomdok,int tipz);
int pdokpd(const char *metka,short dd,short md,short gd,const char *nomdok);
int pdokko(short dd,short md,short gd,int kassa,int tipz,const char *nomdok);
int pdokuos(short dd,short md,short gd,const char *nomdok,int tipz);
int pdokzp(short dd,short md,short gd,const char *nomdok);
int pdokukr(short dd,short md,short gd,const char *nomdok);

int provprov(SQL_str row,class poiprov_rek *poi);


int dirprov(class poiprov_rek *poi)
{
class iceb_tu_str nomdok("");
class iceb_t_mstr spis_menu(0);
short		d,m,g;
class iceb_tu_str shetk("");  /*Номер счета*/
class iceb_tu_str kodkon("");  /*Код организации*/
short           x=0,y=3;  /*Позиция вывода списка*/
int             i,ii,prc;
int             kom,kom1;
class iceb_tu_str shi(""); /*Счёт исходный*/
int             K;
double		db,kr;
class iceb_tu_str kto1(""); /*размер должен быть именно такой иначе нестабильно работает*/
double		debi=0.,krei=0.;
long		kolstr;
long		pozz,pozz1;
class SQLCURSOR cur,cur1;
SQL_str         row,row1;
class iceb_tu_str kontr("");
char		strsql1[5120];
short		mspz; /*метка соответствия первой записи*/
class iceb_tu_str kodop("");
char		bros[1024];
short dn,mn,gn;
short dk,mk,gk;
class iceb_tu_str kekv_char("");
short    dt,mt,gt; /*Месяц год*/
class iceb_tu_str komen("");
short           kls=LINES-6; /*Количество строк в меню*/

//Создаем класс меню
VVOD DANET(1);
VVOD VV(0);
VVOD SOOB(1);
VVOD MENU(3);
VVOD PAR(2);


poltekdat(&dt,&mt,&gt);

/***************
rsdat(&dn,&mn,&gn,poi->dat1.ravno(),1);
rsdat(&dk,&mk,&gk,poi->dat2.ravno(),1);
*************/
if(poi->dat1.getdlinna() > 1)
 rsdatp(&dn,&mn,&gn,poi->dat1.ravno(),&dk,&mk,&gk,poi->dat2.ravno());

move(0,0);
pozz=pozz1=kom=0;


naz1:;
GDITE();

kls=LINES-6; /*Количество строк в меню*/
time_t vrem[kls]; //Время
int val[kls];  //Признак проводки
int ktoi[kls];  //Признак проводки

if((kolstr=cur.make_cursor(&bd,poi->zapros.ravno())) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),poi->zapros.ravno());
  return(0);
 }  

/*Узнаем сумму*/
db=kr=0.;
if(kolstr == 0)
 {
lll:;
  shpd(dn,mn,gn,dk,mk,gk,poi->sh.ravno(),db,kr,kolstr,pozz+pozz1);
  SOOB.VVOD_delete();
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной проводки !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

   move(10,0);
   clrtoeol();
   if(K == FK2 || K == FK3 || K == ENTER)
      goto lll;
   if(K == KEY_RESIZE)
    goto naz1;
   goto vper;
 }
else
 {

  /*Вычисляем сумму по дебету и кредиту*/
  while(cur.read_cursor(&row) != 0)
   {
    if((ii=provprov(row,poi)) != 0)
      continue;
    db+=atof(row[9]);
    kr+=atof(row[10]);
   }
 }
if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;

naz:;

GDITE();

cur.poz_cursor(pozz);
mspz=2;
pozz1=0;
spis_menu.spis_str.free_class();
for(i=0;i<=kls-1;)
 {
  if(cur.read_cursor(&row) == 0)
    break;
  
  pozz1++;

  if(provprov(row,poi) != 0)
    continue;

  vrem[i]=atol(row[12]);
  val[i]=atoi(row[0]);
  debi=atof(row[9]);
  krei=atof(row[10]);
  ktoi[i]=atoi(row[11]);
  
  rsdat(&d,&m,&g,row[1],2);
  kekv_char.new_plus("");
  if(atoi(row[16]) != 0)
   kekv_char.new_plus(row[16]);

  sprintf(bros,"%d.%d.%d|%-*s|%-*s|%-*s|%-*s|%-*s|%8.2f|%8.2f|%-2s|%-4s|%s",
  d,m,g,
  iceb_tu_kolbait(4,row[2]),row[2],
  iceb_tu_kolbait(4,row[3]),row[3],
  iceb_tu_kolbait(5,row[4]),row[4],
  iceb_tu_kolbait(3,row[5]),row[5],
  iceb_tu_kolbait(4,row[6]),row[6],
  debi,krei,row[8],kekv_char.ravno(),row[13]);

  spis_menu.spis_str.plus(bros);
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
  goto lll;
 }  

if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i <= kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

shpd(dn,mn,gn,dk,mk,gk,poi->sh.ravno(),db,kr,kolstr,(long)(pozz+pozz1));
prc=(pozz+pozz1)*100/kolstr;
memset(bros,'\0',sizeof(bros));
kom=spis_menu.menu(kom,y,x,NULL,bros,prc,&K,"",0);

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
    if(provprov(row,poi) != 0)
      continue;

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

    if(provprov(row,poi) != 0)
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

    if(provprov(row,poi) != 0)
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

  polen(str_out.ravno(),bros,sizeof(bros),1,'|'); /* Берем дату для расшифровки*/
  rsdat(&d,&m,&g,bros,1);
  polen(str_out.ravno(),&shi,2,'|');  /*cчет*/
  polen(str_out.ravno(),&shetk,3,'|');  /*Входящий счет*/
  polen(str_out.ravno(),&kodkon,4,'|'); /*Код контрагента*/
  polen(str_out.ravno(),&kto1,5,'|'); /*Кто*/
  polen(str_out.ravno(),&nomdok,6,'|'); //Номер документа
  polen(str_out.ravno(),&debi,7,'|'); 
  debi=okrug(debi,0.01);
  polen(str_out.ravno(),bros,sizeof(bros),8,'|'); 
  krei=ATOFM(bros);
  krei=okrug(krei,0.01);

  polen(str_out.ravno(),&kodop,9,'|'); 
  polen(str_out.ravno(),&kekv_char,10,'|');
  polen(str_out.ravno(),&komen,11,'|'); 
 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return(1);

  case FK1:
   GDITE();
   iceb_t_pdoc("buhg3a.txt");
   clear();
   goto naz;

  case ENTER :
  case FK2:
  case SFK2:
    /* Ищем встречную проводку для определения кода контрагента в ней
      если он там есть и остальных нужных реквизитов*/

    GDITE();
    kontr.new_plus("");
               
    sqlfiltr(bros,sizeof(bros));

    if(val[kom] == -1)
      kontr.new_plus(kodkon.ravno());

    if(val[kom] == 0)
     {
      sprintf(strsql1,"select kodkon from Prov where \
val=%d and datp='%04d-%d-%d' and sh='%s' and shk='%s' and kto='%s' \
and deb=%.2f and kre=%.2f and vrem=%ld and nomd='%s' and oper='%s' \
and komen='%s'",
      val[kom],g,m,d,shetk.ravno(),shi.ravno(),kto1.ravno(),krei,debi,vrem[kom],nomdok.ravno(),kodop.ravno(),komen.ravno_filtr());
      if((i=readkey(strsql1,&row1,&cur1)) != 1)
       {
        mvprintw(LINES-7,0,"%s",strsql1);
        SOOB.VVOD_delete();
        SOOB.VVOD_SPISOK_add_MD("Не нашли парную проводку !");
        sprintf(strsql1,"%d",i);
        SOOB.VVOD_SPISOK_add_MD(strsql1);
       
        soobshw(&SOOB,stdscr,-1,-1,0,1);
        goto naz1;
       }

      kontr.new_plus(row1[0]);

      VV.VVOD_delete();
      sozkorprov(&VV);
      VV.VVOD_SPISOK_zapis_data(0,shi.ravno());      
      VV.VVOD_SPISOK_zapis_data(1,shetk.ravno());
      polen(spis_menu.spis_str.ravno(kom),bros,sizeof(bros),1,'|'); /* Берем дату для расшифровки*/
      VV.VVOD_SPISOK_zapis_data(2,bros);
      VV.VVOD_SPISOK_zapis_data(3,komen.ravno());

      if(debi != 0.)
       {
        sprintf(bros,"%.2f",debi);
        VV.VVOD_SPISOK_zapis_data(4,bros);
       }
      if(krei != 0.)
       {
        sprintf(bros,"%.2f",krei);
        VV.VVOD_SPISOK_zapis_data(5,bros);
       }
      VV.VVOD_SPISOK_zapis_data(6,kodkon.ravno());
      if(kontr.ravno()[0] != '\0' && SRAV(kontr.ravno(),kodkon.ravno(),0) != 0)
       {
        VV.data_strcat(6,"/");
        VV.data_strcat(6,kontr.ravno());
       }
      VV.VVOD_SPISOK_zapis_data(7,kekv_char.ravno());
      
      korprov(&VV,d,m,g,shi.ravno(),shetk.ravno(),kodkon.ravno(),vrem[kom],ktoi[kom],kto1.ravno());
     }

     if(val[kom] == -1)
      {

        VV.VVOD_delete();
        sozkorprovnbs(&VV);

        VV.VVOD_SPISOK_zapis_data(0,shi.ravno());
        polen(spis_menu.spis_str.ravno(kom),bros,sizeof(bros),1,'|'); /* Берем дату для расшифровки*/
        VV.VVOD_SPISOK_zapis_data(1,bros);
        VV.VVOD_SPISOK_zapis_data(2,komen.ravno());

        if(debi != 0.)
         {
          sprintf(bros,"%.2f",debi);
          VV.VVOD_SPISOK_zapis_data(3,bros);
         }
        if(krei != 0.)
         {
          sprintf(bros,"%.2f",krei);
          VV.VVOD_SPISOK_zapis_data(4,bros);
         }
        VV.VVOD_SPISOK_zapis_data(5,kodkon.ravno());

        korprovnbs(&VV,d,m,g,shi.ravno(),kodkon.ravno(),vrem[kom],ktoi[kom],kto1.ravno());
      }
     goto naz1;

  case FK3:

    if(iceb_t_pvglkni(m,g) != 0) /*Проверка блокировки подсистемы*/
     goto naz;

    if(iceb_t_pvkup(ktoi[kom]) != 0) /*удалять может только хозяин или тот кому разрешена блокировка/разблокировка*/
     goto naz1;

    if(prvkp(kto1.ravno()) == 0)
     {
      DANET.VVOD_delete();
      DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

      if(danet(&DANET,2,stdscr) == 2)
       goto naz1;
      if(val[kom] == -1)        
         udprov(val[kom],g,m,d,shi.ravno(),shetk.ravno(),vrem[kom],debi,krei,komen.ravno_filtr(),1);
      else
         udprov(val[kom],g,m,d,shi.ravno(),shetk.ravno(),vrem[kom],debi,krei,komen.ravno_filtr(),2);
      goto naz1;
     }
  
    clear(); /*Очистить экран и закрасить фоновым цветом*/
    goto naz;

  case SFK3:  /*Безусловное удаление*/

    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Удалить выбранную проводку"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Удалить все найденные проводки"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2
    kom1=0;

    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case  2 :
      case -1 :
       goto naz;

      case  0 :

        if(iceb_t_pvglkni(m,g) != 0) /*Проверка блокировки подсистемы*/
         goto naz;


        DANET.VVOD_delete();
        DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

        if(danet(&DANET,2,stdscr) == 2)
          goto naz;



        PAR.VVOD_delete();
        PAR.VVOD_SPISOK_add_MD(gettext("Введите пароль"));
        if(parolv(&PAR,dt,mt,gt,0) != 0)
          goto naz;

        GDITE();
        if(val[kom] == -1)        
         udprov(val[kom],g,m,d,shi.ravno(),shetk.ravno(),vrem[kom],debi,krei,komen.ravno_filtr(),1);
        else
         udprov(val[kom],g,m,d,shi.ravno(),shetk.ravno(),vrem[kom],debi,krei,komen.ravno_filtr(),2);
        goto naz1;

      case  1 :

        DANET.VVOD_delete();
        DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

        if(danet(&DANET,2,stdscr) == 2)
          goto naz;

        PAR.VVOD_delete();
        PAR.VVOD_SPISOK_add_MD(gettext("Введите пароль"));
        if(parolv(&PAR,dt,mt,gt,0) != 0)
          goto naz;

        GDITE();

        udgrupprov(&cur,kolstr,poi);
        goto naz1;
      
     }
    goto naz;


  case FK4:

    SOOB.VVOD_delete();
    sprintf(bros,"%s: %d.%d.%d > %d.%d.%d",gettext("Диапазон поиска"),dn,mn,gn,dk,mk,gk);
    SOOB.VVOD_SPISOK_add_MD(bros);
    sprintf(bros,gettext("Счёт дебета    : %s"),poi->sh.ravno());
    SOOB.VVOD_SPISOK_add_MD(bros);
    sprintf(bros,gettext("Счёт кредита   : %s"),poi->shk.ravno());
    SOOB.VVOD_SPISOK_add_MD(bros);
    sprintf(bros,gettext("Код контрагента: %s"),poi->kor.ravno());
    SOOB.VVOD_SPISOK_add_MD(bros);
    sprintf(bros,gettext("Код операции   : %s"),poi->kop.ravno());
    SOOB.VVOD_SPISOK_add_MD(bros);
    sprintf(bros,gettext("Номер документа: %s"),poi->nn.ravno());
    SOOB.VVOD_SPISOK_add_MD(bros);
    sprintf(bros,gettext("Дебет          : %s"),poi->deb.ravno());
    SOOB.VVOD_SPISOK_add_MD(bros);
    sprintf(bros,gettext("Кредит         : %s"),poi->kre.ravno());
    SOOB.VVOD_SPISOK_add_MD(bros);
    sprintf(bros,gettext("Коментарий     : %s"),poi->raz.ravno());
    SOOB.VVOD_SPISOK_add_MD(bros);
    sprintf(bros,gettext("Группа         : %s"),poi->grupa.ravno());
    SOOB.VVOD_SPISOK_add_MD(bros);

    soobshw(&SOOB,stdscr,-1,-1,0,1);

    clear();      
    if(kolstr > 0)
     goto naz;
    else
     goto lll;
     
  case FK5:
   rasprov(&cur,kolstr,poi);
   clear();      
   goto naz;

  case FK6:
    if(kto1.ravno()[0] == '\0')
     {
      iceb_t_soob(gettext("Проводка не привязана к документу!"));
      goto naz;
     }

    sprintf(strsql1,"select nomd,pod,datd,tz from Prov where \
val=%d and datp='%04d-%d-%d' and sh='%s' and shk='%s' and kto='%s' \
and deb=%.2f and kre=%.2f and vrem=%ld and nomd='%s' and oper='%s' \
and komen='%s'",
    val[kom],g,m,d,shetk.ravno(),shi.ravno(),kto1.ravno(),krei,debi,vrem[kom],nomdok.ravno(),kodop.ravno(),komen.ravno_filtr());
    if((i=readkey(strsql1,&row1,&cur1)) != 1)
     {
      mvprintw(LINES-7,0,"%s",strsql1);
      iceb_t_soob(gettext("Не нашли проводку"));
      goto naz;
     }

   rsdat(&d,&m,&g,row1[2],2);

   if(SRAV(ICEB_MP_MATU,kto1.ravno(),0) == 0)
     pdokmu(d,m,g,atoi(row1[3]),atoi(row1[1]),row1[0]);

   if(SRAV(ICEB_MP_USLUGI,kto1.ravno(),0) == 0)
     pdokusl(d,m,g,atoi(row1[1]),row1[0],atoi(row1[3]));

   if(SRAV(ICEB_MP_PPOR,kto1.ravno(),0) == 0)
     pdokpd(kto1.ravno(),d,m,g,row1[0]);

   if(SRAV(ICEB_MP_PTRE,kto1.ravno(),0) == 0)
     pdokpd(kto1.ravno(),d,m,g,row1[0]);

   if(SRAV(ICEB_MP_KASA,kto1.ravno(),0) == 0)
    pdokko(d,m,g,atoi(row1[1]),atoi(row1[3]),row1[0]);
   if(SRAV(ICEB_MP_UOS,kto1.ravno(),0) == 0)
    pdokuos(d,m,g,row1[0],atoi(row1[3]));

   if(SRAV(ICEB_MP_ZARP,kto1.ravno(),0) == 0)
    pdokzp(d,m,g,row1[0]);

   if(SRAV(ICEB_MP_UKR,kto1.ravno(),0) == 0)
    pdokukr(d,m,g,row1[0]);
   goto naz;
   
  default:
    goto naz1;
    break;
 }

}

/*******************/
/*Шапка и подсказка*/
/*******************/
void shpd(short dn,short mn,short gn,short dk,short mk,
short gk,const char *sh,double db,double kr,
long kolstr, //Общее количество проводок
long kolpr)  //Количество просмотренних проводок
{
int		prc;
clear();

move(0,0);
if(sh[0] != '\0')
  printw("%s - %s",gettext("Поросмотр счета"),sh);
else
 {
  printw(gettext("Просмотр по всем счетам"));
 }

move(1,0);
prc=0;
if(kolstr != 0)
  prc=kolpr*100/kolstr;
printw("%s - %d.%d.%d%s > %d.%d.%d%s %s %d%% %s.",
gettext("Даты поиска"),
dn,mn,gn,
gettext("г."),
dk,mk,gk,
gettext("г."),
gettext("Просмотрено"),
prc,
gettext("проводок"));

move(2,0);
printw("%s - %.2f  %s - %.2f  %s - %.2f",
gettext("Дебет"),
db,
gettext("Кредит"),
kr,
gettext("Разница"),
db-kr);

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2",gettext("корректировка"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F6",gettext("документ"),
//"F7",gettext("пред.день"),
"F10",gettext("выход"),NULL);
}

/*************************/
/*Проверка условий поиска*/
/*************************/

int provprov(SQL_str row,class poiprov_rek *poi)
{
char		strsql[512];
SQL_str		row1;
SQLCURSOR       cur1;

if(proverka(poi->raz.ravno(),row[13],4,0) != 0)
 return(8);

if(proverka(poi->kto.ravno(),row[5],0,0) != 0)
  return(9);

if(proverka(poi->nn.ravno(),row[6],4,0) != 0)
  return(10);

if(proverka(poi->kop.ravno(),row[8],0,0) != 0)
   return(11);


if(fabs(poi->deb.ravno_atof()) > 0.009 && fabs(poi->deb.ravno_atof()-atof(row[9])) > 0.009)
  return(12); 

if(fabs(poi->kre.ravno_atof()) > 0.009 && fabs(poi->kre.ravno_atof()-atof(row[10])) > 0.009)
  return(13); 

if(poi->deb.ravno()[0] == '+' && atof(row[9]) == 0.)
  return(14);

if(poi->kre.ravno()[0] == '+' && atof(row[10]) == 0.)
  return(15);

if(poi->grupa.ravno()[0] != '\0')
 {
  if(row[4][0] == '\0')
    return(16);
  sprintf(strsql,"select grup from Kontragent where kodkon='%s'",
  row[4]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
   {
    if(proverka(poi->grupa.ravno(),row1[0],0,0) != 0)
      return(16);
   }      
 }

if(proverka(poi->kekv_poi.ravno(),row[16],0,0) != 0)
   return(17);

return(0);

}
/*********************************/
/*Удаление группы проводок       */
/*********************************/
void udgrupprov(SQLCURSOR *cur,
long kolstr,
class poiprov_rek *poi)
{
SQL_str		row;
short		val;
double		debi,krei;

short dn,mn,gn;
short dk,mk,gk;
rsdat(&dn,&mn,&gn,poi->dat1.ravno(),1);
rsdat(&dk,&mk,&gk,poi->dat2.ravno(),1);

class VVOD SOOBN(1);
SOOBN.VVOD_SPISOK_add_MD(gettext("Проверка возможности удаления"));
soobsh(&SOOBN,-1,-1,0);
short d=1,m=mn,g=gn;

/*проверяем на блокировку*/
while(sravmydat(1,m,g,1,mk,gk) <= 0)
 {
  if(iceb_t_pvglkni(m,g) != 0)
   return;

  dpm(&d,&m,&g,3); //Увеличить дату на месяц
 }
move(LINES-1,0);
GDITE();

cur->poz_cursor(0);
int kol=0;

/*проверям все проводки на возможность удалиения*/
while(cur->read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kol);
  if(provprov(row,poi) != 0)
    continue;

  if(iceb_t_pvkup(atoi(row[11])) != 0) /*удалять может только хозяин или тот кому разрешена блокировкa/разблокировка*/
   {
    return;
   }

  rsdat(&d,&m,&g,row[1],2);

  if(iceb_t_pbsh(m,g,row[2],row[3],gettext("Проводку удалить невозможно!")) != 0) /*Проверка блокировки счетов*/
   return;
 }

class VVOD SOOBN1(1);
SOOBN1.VVOD_SPISOK_add_MD(gettext("Удаление группы проводок"));
soobsh(&SOOBN1,-1,-1,0);



cur->poz_cursor(0);
kol=0;
while(cur->read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kol);
  if(provprov(row,poi) != 0)
    continue;

  val=atoi(row[0]); 
  rsdat(&d,&m,&g,row[1],2);

  debi=atof(row[9]);
  debi=okrug(debi,0.01);

  krei=atof(row[10]);
  krei=okrug(krei,0.01);
  
  if(val == -1)        
     udprov(val,g,m,d,row[2],row[3],atol(row[12]),debi,krei,row[13],1);
  else
     udprov(val,g,m,d,row[2],row[3],atol(row[12]),debi,krei,row[13],2);

 }
}
