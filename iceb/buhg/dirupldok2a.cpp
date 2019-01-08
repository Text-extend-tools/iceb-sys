/*$Id: dirupldok2a.c,v 5.2 2013/05/17 14:55:59 sasa Exp $*/
/*14.07.2014	30.04.2012	Белых А.И.	dirupldok2a.c
Ввод и корректировка списания топлива по счетам
*/
#include        <errno.h>
#include <math.h>
#include        "buhg.h"

class dirupldok2a_poi
 {
  public:
  int kod_avt;
  int kod_vod;
  class iceb_tu_str nom_dok; //номер документа в начале и изменённый
  class iceb_tu_str podr; //код подразделения в символьном виде в начале и изменённый
  short dd,md,gd;
  
  class SPISOK KST; //Коды списанного в путевом листе топлива
  class masiv_din_double KOL;  //Количество списанного топлива
  
  dirupldok2a_poi()
   {
    kod_avt=0;
    kod_vod=0;
    nom_dok.plus("");
    dd=md=gd=0;    
    podr.plus("");
   }
 };
 
void dirupldok2a_sap(class dirupldok2a_poi*);
void dirupldok2a_ras(short dd,short md,short gd,const char *nom_dok,int kp,SQLCURSOR *cur);
void dirupldok2a_vvod(short dd,short md,short gd,int kp,const char *nom_dok,const char *kod_top,const char *shet);
void dirupldok2a_spst(short dd,short md,short gd,const char *nomd,int podr,SPISOK *KOT,masiv_din_double *KOL);
void dirupldok2a_provpst(short dd,short md,short gd,const char *nomd,int podr);

void dirupldok2a(short dd,short md,short gd,
const char *nom_dok,
int kp)
{
char		strsql[1024];
class iceb_t_mstr spis_menu(0);
SQL_str		row;
int		kolstr=0;
int		K=0;
int		pozz=0,pozz1=0;
int		mspz=0;
int		prc;
int             Y=6;
int             X=0;
class iceb_tu_str koment("");
class dirupldok2a_poi poisk;
class iceb_tu_str kod_top("");
class iceb_tu_str shet("");
class SQLCURSOR cur;
int i=0,kom=0;

/*узнаём код автомобиля*/
sprintf(strsql,"select ka,kv,psv,psz from Upldok where god=%d and kp=%d and nomd='%s'",gd,kp,nom_dok);
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s-%s-%s:%d %s:%d %s:%s",
  __FUNCTION__,
  gettext("Не найдена шапка документа"),
  gettext("Год"),
  gd,
  gettext("Подразделение"),
  kp,
  gettext("Номер документа"),
  nom_dok);
  
  iceb_t_soob(strsql);
  return;
 }
poisk.kod_avt=atoi(row[0]);
poisk.kod_vod=atoi(row[1]);

poisk.nom_dok.new_plus(nom_dok);
poisk.podr.new_plus(kp);
poisk.dd=dd;
poisk.md=md;
poisk.gd=gd;

dirupldok2a_spst(dd,md,gd,nom_dok,kp,&poisk.KST,&poisk.KOL);

//Создаем класс меню
VVOD DANET(1);
VVOD VVOD1(2);

naz1:;

short           kls=LINES-Y+3;

sprintf(strsql,"select kt,shet,zt,zpn,koment from Upldok2a where datd='%04d-%02d-%02d' and kp=%d and nomd='%s'",gd,md,dd,kp,nom_dok);

  

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
lll:;
  dirupldok2a_sap(&poisk);
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == SFK2 || K == ENTER || K == FK3 || K == FK5 || K == FK6)
    goto lll;

  if(K == KEY_RESIZE)
   goto naz1;   

  goto vper;
 }


if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;

naz:;

cur.poz_cursor(pozz);
mspz=2;
pozz1=0;

spis_menu.spis_str.free_class();
for(i=0;i<=kls-1;)
 {
  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;

  sprintf(strsql,"%-*s|%-*s|%10.3f|%10.3f|%s",
  iceb_tu_kolbait(6,row[1]),row[1],
  iceb_tu_kolbait(6,row[0]),row[0],
  atof(row[3]),
  atof(row[2]),
  row[4]);

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
  goto lll;
 }  
if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

dirupldok2a_sap(&poisk);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,Y,X,NULL,strsql,prc,&K,koment.ravno(),COLS-24);


if(kom == -7) //Изменение границы экрана
  goto naz1;

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
//    if(poisk.naimpoi.ravno()[0] != '\0' && strstrm(row[5],poisk.naimpoi.ravno()) == 0)
//     continue;
    i++;
   }
  kom=kls-1;
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
//    if(poisk.naimpoi.ravno()[0] != '\0' && strstrm(row[5],poisk.naimpoi.ravno()) == 0)
//     continue;
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
//    if(poisk.naimpoi.ravno()[0] != '\0' && strstrm(row[5],poisk.naimpoi.ravno()) == 0)
//     continue;
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

  polen(spis_menu.spis_str.ravno(kom),&kod_top,2,'|');
  polen(spis_menu.spis_str.ravno(kom),&shet,1,'|');

 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     dirupldok2a_provpst(poisk.dd,poisk.md,poisk.gd,poisk.nom_dok.ravno(),poisk.podr.ravno_atoi());
     return;

  case FK1:  /*Получить подсказку*/
    GDITE();
    iceb_t_pdoc("upl2_3_1.txt");
    goto naz;

  case ENTER:
  case SFK2:   /*Корректировать*/
    dirupldok2a_vvod(poisk.dd,poisk.md,poisk.gd,poisk.podr.ravno_atoi(),poisk.nom_dok.ravno(),kod_top.ravno(),shet.ravno());
    goto naz1;

  case FK2:  /*Ввести запись*/
  case PLU:

    dirupldok2a_vvod(poisk.dd,poisk.md,poisk.gd,poisk.podr.ravno_atoi(),poisk.nom_dok.ravno(),"","");
    goto naz1;

  case FK3:       /*Удалить запись*/
    if(iceb_t_pbpds(md,gd) != 0)
     goto naz;

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
     goto naz;


    sprintf(strsql,"delete from Upldok2a where datd='%04d-%02d-%02d' and kp=%d and nomd='%s' and kt='%s' and shet='%s'",gd,md,dd,poisk.podr.ravno_atoi(),poisk.nom_dok.ravno(),kod_top.ravno(),shet.ravno());
    sql_zapis(strsql,0,0);
    goto naz1;


  case FK5:    //Распечатать
    dirupldok2a_ras(poisk.dd,poisk.md,poisk.gd,poisk.nom_dok.ravno(),poisk.podr.ravno_atoi(),&cur);
    goto naz;


  default:
    goto naz;
    break;
 }



}
/**************************/
/*Шапка                   */
/**************************/

void	dirupldok2a_sap(class dirupldok2a_poi *poisk)
{
short		y=5;
short           x=COLS-20;
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;


clear();
printw(gettext("Списания топлива по счетам"));

mvprintw(1,0,"%s:%d.%d.%d %s:%s %s:%d",
gettext("Дата"),
poisk->dd,poisk->md,poisk->gd,
gettext("Номер"),
poisk->nom_dok.ravno(),
gettext("Подразделение"),
poisk->podr.ravno_atoi());

class iceb_tu_str nom_znak("");
class iceb_tu_str naim("");
sprintf(strsql,"select nomz,naik from Uplavt where kod=%d",poisk->kod_avt);

if(readkey(strsql,&row,&cur) == 1)
 {
  nom_znak.new_plus(row[0]);
  naim.new_plus(row[1]);
 }

mvprintw(2,0,"%s:%d %s %s\n",gettext("Автомобиль"),poisk->kod_avt,nom_znak.ravno(),naim.ravno());

naim.new_plus("");
sprintf(strsql,"select naik from Uplouot where kod=%d and mt=1",poisk->kod_vod);

if(readkey(strsql,&row,&cur) == 1)
 {
  naim.new_plus(row[0]);
 }

mvprintw(3,0,"%s:%d %s\n",gettext("Водитель"),poisk->kod_vod,naim.ravno());




mvprintw(y,0,"   1       2         3         4       5");
mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,"1-%s",gettext("счёт"));
mvprintw(y++,x,"2-%s",gettext("код топлива"));
mvprintw(y++,x,"3-%s",gettext("затраты по норме"));
mvprintw(y++,x,"4-%s",gettext("затраты по факту"));
mvprintw(y++,x,"5-%s",gettext("коментарий"));


for(int i=0; i < poisk->KST.kolih(); i++)
 mvprintw(++y,x,"%-4s %.3f",poisk->KST.ravno(i),poisk->KOL.ravno(i));



helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
//"F4",gettext("поиск"),
"F5",gettext("печать"),
"F10",gettext("выход"),NULL);


}
/*********************/
/*Запись             */
/*********************/
//Если вернули 0 - записали 1-нет

int   dirupldok2a_zap(short dd,short md,short gd,
int kp,
const char *nom_dok,
VVOD *VV,
const char *kod_top,
const char *shet)
{
char	strsql[2048];

attroff(VV->VVOD_return_cs(iceb_CFS));
attron(VV->VVOD_return_cs(iceb_CFM));

if(iceb_t_pbpds(md,gd) != 0)
 return(1);


if(VV->VVOD_SPISOK_return_data(0)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(1)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(2)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(3)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
  return(1);
 }       

struct OPSHET reksh;
if(prsh1(VV->data_ravno(0),&reksh) != 0)
  return(1);
sprintf(strsql,"select kodt from Uplmt where kodt='%s'",VV->data_ravno_filtr(1));
if(readkey(strsql) != 1)
 {
  sprintf(strsql,"%s %s %s!",gettext("Не найдена марка топлива"),VV->data_ravno_filtr(1),gettext("в справочнике"));
  iceb_t_soob(strsql);
  return(1);
 }


if(SRAV(kod_top,VV->data_ravno(1),0) != 0 || SRAV(shet,VV->data_ravno(0),0) != 0)
 {
  //Проверяем может код уже введён
  sprintf(strsql,"select shet from Upldok2a where datd='%04d-%02d-%02d' and kp=%d and nomd='%s' and kt='%s' and shet='%s'",gd,md,dd,kp,nom_dok,VV->data_ravno(1),VV->data_ravno(0));
  if(readkey(strsql) >= 1)
   {
    iceb_t_soob(gettext("Такая запись уже есть !"));
    return(1);
   }


 }
 

if(kod_top[0] == '\0') //Ввод новой записи
  sprintf(strsql,"insert into Upldok2a values('%04d-%02d-%02d',%d,'%s','%s','%s',%.3f,%.3f,'%s',%d,%ld)",
  gd,md,dd,
  kp,
  nom_dok,
  VV->data_ravno_filtr(1),
  VV->data_ravno_filtr(0),
  VV->data_ravno_atof(3),
  VV->data_ravno_atof(2),
  VV->data_ravno_filtr(4),
  iceb_t_getuid(),time(NULL));
else
  sprintf(strsql,"update Upldok2a set \
shet='%s',\
kt='%s',\
zt=%.3f,\
zpn=%.3f,\
koment='%s',\
ktoz=%d,\
vrem=%ld \
where datd='%04d-%02d-%02d' and kp=%d and nomd='%s' and kt='%s' and shet='%s'",
  VV->data_ravno_filtr(0),
  VV->data_ravno_filtr(1),  
  VV->data_ravno_atof(3),
  VV->data_ravno_atof(2),
  VV->data_ravno_filtr(4),
  iceb_t_getuid(),
  time(NULL),
  gd,md,dd,kp,nom_dok,kod_top,shet);
  
if(sql_zapis(strsql,0,0) != 0)
 return(1);



return(0);


}

/**********************/
/*Ввод и корректировки*/
/**********************/

void dirupldok2a_vvod(short dd,short md,short gd,
int kp,
const char *nom_dok,
const char *kod_top,
const char *shet)
{
char		strsql[2048];
SQL_str		row;
int		N=0,K=0;
class iceb_tu_str kod("");
class iceb_tu_str naimkod("");

class SQLCURSOR cur;

class VVOD MENU(3);
class VVOD VV(0);

VV.VVOD_SPISOK_add_data(16);//0
VV.VVOD_SPISOK_add_data(16);//1
VV.VVOD_SPISOK_add_data(16);//2
VV.VVOD_SPISOK_add_data(16);//3
VV.VVOD_SPISOK_add_data(255);//4



if(kod_top[0] != '\0')
 {
  int vv=0;
  sprintf(strsql,"select kt,shet,zt,zpn,koment,ktoz,vrem from Upldok2a where datd='%04d-%02d-%02d' and kp=%d and nomd='%s' and kt='%s' and shet='%s'",gd,md,dd,kp,nom_dok,kod_top,shet);
  if((vv=readkey(strsql,&row,&cur)) != 1)
   {
    printw("\n%s-vv=%d\n",strsql,vv);
    iceb_t_soob(gettext("Не найдена запись для корректировки!"));
    return;   
   }

  VV.VVOD_SPISOK_zapis_data(0,row[1]);
  VV.VVOD_SPISOK_zapis_data(1,row[0]);
  VV.VVOD_SPISOK_zapis_data(2,row[3]);
  VV.VVOD_SPISOK_zapis_data(3,row[2]);
  VV.VVOD_SPISOK_zapis_data(4,row[4]);

  VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка"));
  kzvz(row[5],row[6],&VV);     
 }
else
  VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой записи"));



VV.VVOD_SPISOK_add_MD(gettext("Счёт.............."));//0
VV.VVOD_SPISOK_add_MD(gettext("Код топлива......."));//1
VV.VVOD_SPISOK_add_MD(gettext("Затраты по норме.."));//2
VV.VVOD_SPISOK_add_MD(gettext("Затраты по факту.."));//3
VV.VVOD_SPISOK_add_MD(gettext("Коментарий........"));//4


naz:;



helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("реквизиты"),
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
    goto naz;


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


    if(N == 0 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')
     {
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

  if(K == FK1) //Помощь
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    GDITE();
    iceb_t_pdoc("upl2_3_2.txt");
    clear();
    goto naz;
   }

  if(K == FK2 || K == PLU) //Записываем
   {
    if(dirupldok2a_zap(dd,md,gd,kp,nom_dok,&VV,kod_top,shet) != 0)
     goto naz;
    break;
   }

  if(K == FK3) //Реквизиты
   {
    class VVOD MENU(3);

    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка кодов топлива"));
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
        kod.new_plus("");
        vibrek("Plansh",&kod);
       
        VV.data_plus(0,kod.ravno());
        break;

      case 1 :
        kod.new_plus("");
        if(dirmt(&kod,1) == 0)
          VV.data_plus(1,kod.ravno());
                 
        break;
     }
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




/********************/
/*Распечатать       */
/********************/

void dirupldok2a_ras(short dd,short md,short gd,
const char *nom_dok,
int kp,SQLCURSOR *cur)
{
char	imaf[64];
FILE	*ff;
SQL_str row;
class SQLCURSOR cur1;

sprintf(imaf,"uplsh%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }


iceb_t_zagolov(gettext("Списания топлива по счетам к путевому листу"),ff);
fprintf(ff,"%s:%02d.%02d.%04d\n",gettext("Дата путевого листа"),dd,md,gd);
fprintf(ff,"%s:%s\n",gettext("Номер документа"),nom_dok);
fprintf(ff,"%s:%d\n",gettext("Подразделение"),kp);

fprintf(ff,"\
---------------------------------------------\n");
fprintf(ff,gettext("\
 Счёт |Код топлива|Затраты|Затраты|Коментарий\n\
      |           |по нор.|по факт|\n"));
/*
123456 12345678901 1234567 
*/
fprintf(ff,"\
---------------------------------------------\n");

cur->poz_cursor(0);
while(cur->read_cursor(&row) != 0)
 {
   

  fprintf(ff,"%-*s|%-*s|%7.7g|%7.7g|%s\n",
  iceb_tu_kolbait(6,row[1]),
  row[1],
  iceb_tu_kolbait(11,row[0]),
  row[0],
  atof(row[3]),
  atof(row[2]),
  row[4]);


 }
fprintf(ff,"\
---------------------------------------------\n");

podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Списания топлива по счетам к путевому листу"));

iceb_t_ustpeh(imaf,3);

iceb_t_rabfil(&oth,"");

}
/*****************************************************/
/*Создание списка списанного в путевом листе топлива*/
/*****************************************************/
void dirupldok2a_spst(short dd,short md,short gd, //Дата
const char *nomd, //Номер документа
int podr, //Подразделение
SPISOK *KOT, //Коды топлива
masiv_din_double *KOL) //Количество топлива
{
char	strsql[512];
int	kolstr=0;
SQL_str row;
int nomer=0;

sprintf(strsql,"select kodtp,kolih from Upldok1 where datd='%d-%d-%d' and kp=%d and nomd='%s' \
and tz=2",gd,md,dd,podr,nomd);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  if((nomer=KOT->find(row[0])) < 0)
     KOT->plus(row[0]);
  KOL->plus(atof(row[1]),nomer);
  
 }


}
/*************************************/
/*Проверка полноты списания топлива  */
/*************************************/

void dirupldok2a_provpst(short dd,short md,short gd, //Дата
const char *nomd, //Номер документа
int podr) //Подразделение
{
char	strsql[512];
int	kolstr=0;
SQL_str row;
int nomer=0;
int kolmt=0;
int metkasp=1; //0-не полнстью списано 1- полностью

//printw("\nprovpst-%d.%d.%d %s %s\n",dd,md,gd,nomd,podr);

class SPISOK KST; //Коды списанного в путевом листе топлива
class masiv_din_double KOL;  //Количество списанного топлива

dirupldok2a_spst(dd,md,gd,nomd,podr,&KST,&KOL);

kolmt=KST.kolih();
if(kolmt > 0)
 {
  SQLCURSOR cur;
  for(int ii=0; ii < kolmt; ii++)
   {
    double kolsp=0.;
    
    sprintf(strsql,"select zt from Upldok2a where datd='%d-%d-%d' and kp=%d and nomd='%s' and kt='%s'",
    gd,md,dd,podr,nomd,KST.ravno(ii));
    if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
     {
      msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
      continue;
     }
    VVOD SOOB(1);

    while(cur.read_cursor(&row) != 0)
      kolsp+=atof(row[0]);

   int metkanr=0;
   if(fabs(kolsp - KOL.ravno(ii)) > 0.0009)
    {
      metkanr++;
      metkasp=0;
      if(metkanr == 1)
         SOOB.VVOD_SPISOK_add_MD(gettext("Не полностью списано топливо по объектам списания !"));
             
       sprintf(strsql,"%s %.3f != %.3f",
       KST.ravno(nomer),
       kolsp,
       KOL.ravno(ii));
       
       SOOB.VVOD_SPISOK_add_MD(strsql);
       
       soobshw(&SOOB,stdscr,-1,-1,0,1);
     }
   }
 }
else
  metkasp=0;

sprintf(strsql,"update Upldok set mt=%d where  datd='%d-%d-%d' and kp=%d and nomd='%s'",
metkasp,gd,md,dd,podr,nomd);
sql_zapis(strsql,1,1);

}
