/*$Id: dirzardkz.c,v 5.34 2013/08/13 05:49:47 sasa Exp $*/
/*01.01.2018	01.06.2006	Былых А.И.	dirzardkz.c
Работа с записями в документе
*/
#include <errno.h>
#include "buhg.h"

class poi_dirzardkz
 {
  public:

  class iceb_tu_str tabn;
  class iceb_tu_str fio;
  class iceb_tu_str knah;
  class iceb_tu_str shet;
  class iceb_tu_str koment;

  short metka_poi; //0-без поиска 1-с поиском
  double suma_itogo;
  int it_dni;  
  class iceb_tu_str fio_s; //записывается фамилия для вывода на экран
  
  poi_dirzardkz()
   {
    clear_data();
   }

  void clear_data()
   {
    tabn.new_plus("");
    fio.new_plus("");
    knah.new_plus("");
    shet.new_plus("");
    koment.new_plus("");
    metka_poi=0;    
   }  
 };

int dirzardkz_vvod(short dd,short md,short gd,const char *nomdok,const char *tabn,int prn,const char *knah,const char *podr,const char *shet,int nomz,short mesn,short godn);
void dirzardkz_sap(short prn,short dd,short md,short gd,const char *nomdok,class poi_dirzardkz *poisk);
int dirzardkz_prov(SQL_str row,class poi_dirzardkz *poisk);
int  dirzarkz_uz(short dd,short md,short gd,const char *tabn,int prn,const char *knah,const char *podr,const char *shet,int nomz,short mesn,short godn);
int dirzardkz_ud(SQLCURSOR *cur,int kolstr,short god_sd,short dd,short md,short gd,const char *nomdok,short prn);
void dirzardkz_poi(class poi_dirzardkz *poisk);
void dirzardkz_ras(SQLCURSOR *cur,int kolstr,short dd,short md,short gd,const char *nomdok,short prn,class poi_dirzardkz *poisk);

void imp_zardok(short prn,short dd,short md,short gd,const char *nomdok);

extern short           ddd,mmm,ggg; /*Месяц год*/

void dirzardkz(short prn, /*1-начисления 2-удержания*/
short god_dok,
short dd,short md,short gd, /*дата начисления*/
class iceb_tu_str *nomdok)
{
class iceb_t_mstr spis_menu(0);
int pozz=0,pozz1=0;
class poi_dirzardkz poisk;
int kom=0;
int K=0;
char strsql[1024];
int kolstr=0;
SQL_str row;
SQLCURSOR cur;
short mspz; /*метка соответствия первой записи*/
int i=0;
int prc=0;
int x=0,y=3;
SQL_str row1;
SQLCURSOR cur1;
class iceb_tu_str fio("");
class iceb_tu_str naim_nah("");
class iceb_tu_str tabn("");
class iceb_tu_str knah("");
class iceb_tu_str podr("");
class iceb_tu_str shet("");
short godn=0,mesn=0;
class iceb_tu_str fio_in("");
class iceb_tu_str kol_dnei("");

VVOD DANET(1);

naz1:;

GDITE();
poisk.suma_itogo=0.;
poisk.it_dni=0;
short           kls=LINES-y-3;

int nomz[kls];
int podr_i[kls];

sprintf(strsql,"select tabn,knah,suma,shet,nomz,podr,kom,godn,mesn,kdn from Zarp  where datz = '%04d-%02d-%02d' \
and nd='%s' and prn='%d' order by tabn asc",gd,md,dd,nomdok->ravno(),prn);



if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {

lll:;

  dirzardkz_sap(prn,dd,md,gd,nomdok->ravno(),&poisk);

  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи!"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);
  
  if(K == FK5 || K == SFK2 || K == FK3 || K == FK6 || K == ENTER)
     goto lll;
  if(K == KEY_RESIZE)
   goto naz1;
     
  goto vper;
 }
else
 {
  while(cur.read_cursor(&row) != NULL)
   {
    if(dirzardkz_prov(row,&poisk) != 0)
     continue;
    poisk.suma_itogo+=atof(row[2]);
    poisk.it_dni+=atoi(row[9]);
   }
 }

naz:;

GDITE();

if(kolstr != 0 && pozz >= kolstr)
  pozz = kolstr-1;


cur.poz_cursor(pozz);
mspz=2;
pozz1=0;
spis_menu.spis_str.free_class();
for(i=0; i<kls ; )
 {
  
  if(cur.read_cursor(&row) == 0)
    break;

  pozz1++;

  if(dirzardkz_prov(row,&poisk) != 0)
   continue;
  naim_nah.new_plus("");
  if(prn == 1)
    sprintf(strsql,"select naik from Nash where kod=%s",row[1]);
  if(prn == 2)
    sprintf(strsql,"select naik from Uder where kod=%s",row[1]);
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_nah.new_plus(row1[0]);

  nomz[i]=atoi(row[4]);
  podr_i[i]=atoi(row[5]);
  godn=atoi(row[7]);
  mesn=atoi(row[8]);


  if(atoi(row[9]) != 0)
   kol_dnei.new_plus(row[9]);
  else
   kol_dnei.new_plus("");  
  iceb_tu_fio(poisk.fio_s.ravno(),&fio_in); /*Получаем фамилию с инициалами*/
    
  sprintf(strsql,"%-5s|%-*.*s|%-*s|%8.2f|%-2s|%02d.%04d|%-2s|%s",
  row[0],
  iceb_tu_kolbait(15,fio_in.ravno()),iceb_tu_kolbait(15,fio_in.ravno()),fio_in.ravno(),
  iceb_tu_kolbait(5,row[3]),row[3],
  atof(row[2]),row[1],mesn,godn,kol_dnei.ravno(),naim_nah.ravno());


  spis_menu.spis_str.plus(strsql);
  i++;
  
  if(pozz1 == 1) /*Первая запись удовлетворяет условиям поиска*/
   mspz=1;

 }

if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

if(i == 0)
 {
  if(kolstr != 0 && pozz != 0)
   {
    //Если записи есть но не подходят по условиям поиска
    pozz--;
    goto naz;
   }
  goto lll;
 }

dirzardkz_sap(prn,dd,md,gd,nomdok->ravno(),& poisk);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,prc,&K,"",COLS-24);


if(kom == -7) //Изменение размера экрана
  goto naz1;
  
if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  for(i=0; i< mspz  ; )
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

    if(dirzardkz_prov(row,&poisk) != 0)
     continue;
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
    if(dirzardkz_prov(row,&poisk) != 0)
     continue;
    i++;
   }

  goto naz;
 }
 
if(kom == -5) /*Страница вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }

  if(pozz + kls >= kolstr)
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
    if(dirzardkz_prov(row,&poisk) != 0)
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
   goto naz;

  polen(str_vib.ravno(),&tabn,1,'|');
  polen(str_vib.ravno(),&knah,5,'|');
  polen(str_vib.ravno(),&shet,3,'|');
  polen(str_vib.ravno(),strsql,sizeof(strsql),6,'|');
  rsdat1(&mesn,&godn,strsql);
  
  podr.new_plus(podr_i[kom]);
  
 }

vper:;

switch(K)
 {  

  case FK1: /*Помощь*/
    GDITE();
    iceb_t_pdoc("zarp2_12.txt");
    if(kolstr > 0)
      goto naz;
    else
      goto lll;

  case SFK1: //работать с шапкой документа
    dirzardk_vvod(prn,&dd,&md,&gd,nomdok);
    goto naz;

  case PLU:  //Ввод новой записи
  case FK2:  //Ввод новой записи

    if(dirnuprovb(md,gd,0) != 0)
     goto naz;


    dirzardkz_vvod(dd,md,gd,nomdok->ravno(),"",prn,"","","",0,md,gd);
    goto naz1;

  case ENTER:  //корректировка записи
  case SFK2:  //корректировка записи

    dirzardkz_vvod(dd,md,gd,nomdok->ravno(),tabn.ravno(),prn,knah.ravno(),podr.ravno(),shet.ravno(),nomz[kom],mesn,godn);
    goto naz1;


  case FK3:  //Удалить запись

    if(dirnuprovb(md,gd,atoi(tabn.ravno())) != 0)
     goto naz;

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить запись в документе ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
     goto naz;
     
    dirzarkz_uz(dd,md,gd,tabn.ravno(),prn,knah.ravno(),podr.ravno(),shet.ravno(),nomz[kom],mesn,godn);

    goto naz1;

  case SFK3:  //Удалить документ
    if(dirnuprovb(md,gd,0) != 0)
     goto naz;

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить документ ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
     goto naz;

    if(dirzardkz_ud(&cur,kolstr,god_dok,dd,md,gd,nomdok->ravno(),prn) == 0)
     return;
    goto naz1;

  case FK4:  /*Поиск*/
    dirzardkz_poi(&poisk);
    pozz=0;
    goto naz1;
    
  case FK5: //печать
     dirzardkz_ras(&cur,kolstr,dd,md,gd,nomdok->ravno(),prn,&poisk);
     goto naz1;

  case FK6: //воод и корректировка отработанного времени
      
      fio.new_plus("");
      sprintf(strsql,"select fio from Kartb where tabn=%d",tabn.ravno_atoi());
      if(readkey(strsql,&row1,&cur1) == 1)
       fio.new_plus(row1[0]);

      dirtabel(md,gd,tabn.ravno_atoi(),fio.ravno());  
      goto naz1;

  case FK9:
    if(dirnuprovb(md,gd,0) != 0)
     goto naz;
    imp_zardok(prn,dd,md,gd,nomdok->ravno());
    goto naz1;
      
  case FK10:
  case ESC:
    return;
      
  default:
    if(kolstr > 0)
      goto naz;
    else
      goto lll; 
 }

}

/***********************/
/*Ввод и корректировка записи*/
/*****************************/
int dirzardkz_vvod(short dd,short md,short gd,
const char *nomdok,
const char *tabn,
int prn, //1-начисление 2-удержание
const char *knah,
const char *podr,
const char *shet,
int nomz,
short mesn,
short godn)
{
class ZARP zp;
char strsql[1024];
VVOD VV(0);
SQL_str row;
SQLCURSOR cur;
class iceb_tu_str shet_nah("");
static class iceb_tu_str kodnah("");
int N=0,K=0;
int ktoi=0;
time_t vremz=0;
class OPSHET rekshet;
VVOD MENU(3);
class iceb_tu_str kod("");
class iceb_tu_str fiov("");
static iceb_tu_str vschet_mes("");

if(vschet_mes.getdlinna() <= 1)
 {
  sprintf(strsql,"%02d.%d",md,gd);
  vschet_mes.new_plus(strsql);  
 }

VV.VVOD_SPISOK_add_data(10);//0
VV.VVOD_SPISOK_add_data(10);//1
VV.VVOD_SPISOK_add_data(10);//2
VV.VVOD_SPISOK_add_data(10);//3
VV.VVOD_SPISOK_add_data(40);//4
VV.VVOD_SPISOK_add_data(10);//5
VV.VVOD_SPISOK_add_data(8);//6
VV.VVOD_SPISOK_add_data(3);//7

if(atoi(tabn) != 0)
 {
  //Узнаём сумму и комментарий
  sprintf(strsql,"select suma,kom,ktoz,vrem,kdn from Zarp where datz='%04d-%02d-%02d' and \
tabn=%s and prn='%d' and knah=%s and godn=%d and mesn=%d and podr=%s and shet='%s' \
and nomz=%d",
  gd,md,dd,tabn,prn,knah,godn,mesn,podr,shet,nomz);
  int kol=0;
  if((kol=readkey(strsql,&row,&cur)) != 1)
   {
    printw("\n%d-%s\n",kol,strsql);
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено запись для корректировки !"));
    soobshw(&SOOB,stdscr,-1,-1,1,1);

    return(1);
    
   }
  VV.data_plus(0,tabn);

  kodnah.new_plus(knah);
  double sum=atof(row[0]);
  if(prn == 2)
   sum*=-1;  
  VV.data_plus(2,sum);
  
  shet_nah.new_plus(shet);  
  ktoi=atoi(row[2]);
  vremz=atol(row[3]);  
    
  VV.data_plus(4,row[1]);
  if(*podr != 0)
   VV.data_plus(5,*podr);
  
  sprintf(strsql,"%d.%d",mesn,godn);
  VV.data_plus(6,strsql);
  VV.data_plus(7,row[4]);  
  
  zp.dz=dd;
  zp.godn=gd;
  zp.mesn=md;
  zp.nomz=nomz;
  zp.podr=atoi(podr);
  strcpy(zp.shet,shet);
  
 }
else
 VV.data_plus(6,vschet_mes.ravno());
 
VV.data_plus(1,kodnah.ravno());
VV.data_plus(3,shet_nah.ravno());
VV.data_plus(5,podr);

VV.VVOD_SPISOK_add_MD(gettext("Табельный номер......"));//0

if(prn == 1)
 VV.VVOD_SPISOK_add_MD(gettext("Код начисления......."));//1
if(prn == 2)
 VV.VVOD_SPISOK_add_MD(gettext("Код удержания........"));//1

if(prn == 1)
  VV.VVOD_SPISOK_add_MD(gettext("Сумма начисления....."));//2
if(prn == 2)
  VV.VVOD_SPISOK_add_MD(gettext("Сумма удержания......"));//2

VV.VVOD_SPISOK_add_MD(gettext("Счёт................."));//3
VV.VVOD_SPISOK_add_MD(gettext("Коментарий..........."));//4
VV.VVOD_SPISOK_add_MD(gettext("Подразделение........"));//5
VV.VVOD_SPISOK_add_MD(gettext("В счёт месяца (м.г).."));//6
VV.VVOD_SPISOK_add_MD(gettext("Количество дней......"));//7



naz:;

VV.VVOD_delete_ZAG();
if(atoi(tabn) != 0)
 {
  VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка записи"));

  kzvz(ktoi,vremz,&VV);
 }    
else
 {
  VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой записи"));
 } 

if(VV.data_ravno_atoi(0) != 0)
 {
  sprintf(strsql,"select fio,podr,shet from Kartb where tabn=%d",VV.data_ravno_atoi(0));
  if(readkey(strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),VV.data_ravno_atoi(0));
    iceb_t_soob(strsql);
   }
  else
   {
    sprintf(strsql,"%s:%s",gettext("Фамилия"),row[0]);
    VV.VVOD_SPISOK_add_ZAG(strsql);

    if(VV.data_ravno(3)[0] == '\0')
      VV.data_plus(3,row[2]);

    if(VV.data_ravno_atoi(5) == 0)
     VV.data_plus(5,row[1]);
   }
 }

if(VV.data_ravno_atoi(1) != 0)
 {
  if(prn == 1)
    sprintf(strsql,"select naik from Nash where kod=%d",VV.data_ravno_atoi(1));
  if(prn == 2)
    sprintf(strsql,"select naik from Uder where kod=%d",VV.data_ravno_atoi(1));
  if(readkey(strsql,&row,&cur) != 1)
   {
    VVOD SOOB(1);
    if(prn == 1)
      sprintf(strsql,"%s %d !",gettext("Не найден код начисления"),VV.data_ravno_atoi(1));
    if(prn == 2)
      sprintf(strsql,"%s %d !",gettext("Не найден код удержания"),VV.data_ravno_atoi(1));
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,1,1);
   }
  else
   {
    if(prn == 1)
     sprintf(strsql,"%s:%s",gettext("Начисление"),row[0]);
    if(prn == 2)
     sprintf(strsql,"%s:%s",gettext("Удержание"),row[0]);
    VV.VVOD_SPISOK_add_ZAG(strsql);
   }
 }

if(VV.data_ravno(3)[0] != '\0')
 {
  if(prsh1(VV.data_ravno(3),&rekshet) == 0)
   {
    sprintf(strsql,"%s:%s",gettext("Счёт"),rekshet.naim);
    VV.VVOD_SPISOK_add_ZAG(strsql);
   }
 }

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F5",gettext("счёт"),
"F10",gettext("выход"),NULL);


VV.vramka(-1,-1,0);


for(;;)
 {
  attron(VV.VVOD_return_cs(iceb_CFS)); 

  if(N > VV.KLST-1)
   {
    N=0;
   }
  if(N < 0)
   {
    N=VV.KLST-1;
   }

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

    if(N == 0) //Табельный номер
     {
      if(atoi(tabn) != 0)
       if(SRAV(tabn,VV.data_ravno(0),0) != 0)
        {   
          iceb_t_soob(gettext("Корректировка табельного номера невозможна !"));
          VV.data_plus(0,tabn);
          goto naz;
        }
      N++;
      goto naz;
     }

    if(N == 1) //код начисления
     {
      if(prn == 2)
       {
        /*узнаём счёт*/
        sprintf(strsql,"select shet from Uder where kod=%d",VV.data_ravno_atoi(N));
        if(readkey(strsql,&row,&cur) == 1)
         {
          VV.data_plus(3,row[0]);
         }
       }      
      N++;
      goto naz;
     }

    if(N == 3) //Счёт
     {

      
      N++;
      goto naz;
     }

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


  if(K == ESC || K == FK10)
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    return(1);
    break;
   }

  if(K == FK1)
   {
    attroff(VV.VVOD_return_cs(iceb_CFS)); 
    GDITE();
    iceb_t_pdoc("zarp2_12_1.txt");
    goto naz;     
   }

  if(K == FK2 || K == PLU) /*Записываем*/
   {
    vschet_mes.new_plus(VV.data_ravno(6));        
    break;
   }
  
  if(K == FK3) //реквизиты меню
   {
    attroff(VV.VVOD_return_cs(iceb_CFS)); 
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите  нужное и нажмите Enter"));
    
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка начисленй"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка табельных номеров"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    
    int kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);
    class iceb_tu_str naim_kod("");
    switch (kom1)
     {
      case 3 :
      case -1:
       goto naz;

      case 0 :
        if(vibrek("Plansh",&kod) == 0)
          VV.data_plus(3,kod.ravno());
        break;

      case 1 :
        if(dirnach(1,&kod,&naim_kod) == 0)
          VV.data_plus(1,kod.ravno());
        break;

      case 2 :
        int tabnom=0;
        if(dirtab(&tabnom,&fiov,0,1) == 0)
          VV.data_plus(0,tabnom);
        clear();
        break;

     }
    goto naz;
   }
  if(K == FK4) //очистить меню
   {
    VV.VVOD_clear_data();
    goto naz;
   }

  if(K == FK5) //взять счёт из карточки
   {
    if(prn == 2)
     goto naz;
    if(VV.data_ravno(0)[0] == '\0')
     goto naz;
    sprintf(strsql,"select shet from Kartb where tabn=%s",VV.data_ravno(0));
    if(readkey(strsql,&row,&cur) == 1)
     VV.data_plus(3,row[0]);
    goto naz;
   }

 }

attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));

GDITE();

if(dirnuprovb(md,gd,VV.data_ravno_atoi(0)) != 0)
     goto naz;

if(atoi(tabn) != 0)
 if(SRAV(tabn,VV.data_ravno(0),0) != 0)
  {   
   iceb_t_soob(gettext("Корректировка табельного номера невозможна !"));
   goto naz;
  }

//Проверяем табельный номер
sprintf(strsql,"select tabn from Kartb where tabn=%d",VV.data_ravno_atoi(0));
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),VV.data_ravno_atoi(0));
  iceb_t_soob(strsql);
  goto naz;
 }

//Проверяем код подразделения
sprintf(strsql,"select kod from Podr where kod=%d",VV.data_ravno_atoi(5));
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден код подразделения"),VV.data_ravno_atoi(5));
  iceb_t_soob(strsql);
  goto naz;
 }


/*Проверяем код начисления*/
if(prn == 1)
  sprintf(strsql,"select kod from Nash where kod=%d",VV.data_ravno_atoi(1));
if(prn == 2)
  sprintf(strsql,"select kod from Uder where kod=%d",VV.data_ravno_atoi(1));
if(readkey(strsql,&row,&cur) != 1)
 {
  if(prn == 1)
    sprintf(strsql,"%s %d !",gettext("Не найден код начисления"),VV.data_ravno_atoi(1));
  if(prn == 2)
    sprintf(strsql,"%s %d !",gettext("Не найден код удержания"),VV.data_ravno_atoi(1));

  iceb_t_soob(strsql);
  goto naz;
 }

/*Проверяем счёт*/
if(prsh1(VV.data_ravno(3),&rekshet) != 0)
  goto naz;

/*Проверяем в счёт месяца*/

if(VV.data_ravno(6)[0] != '\0')
 if(rsdat1(&mesn,&godn,VV.data_ravno(6)) != 0)
  {
    iceb_t_soob(gettext("Неправильно введена дата!"));
    goto naz;
  }
 
/*Записываем список начислений если его ещё нет*/
if(tabn[0] == '\0')
 provzzag(md,gd,VV.data_ravno_atol(0),prn,0); /*если подразделение 0,то запишет с кодом подразделения, который в карточке*/

/*Если код начисления изменился то запись нужно удалить*/
if(atoi(knah) != 0)
if(atoi(knah) != VV.data_ravno_atoi(1))
 {
  sprintf(strsql,"delete from Zarp where datz='%04d-%02d-%02d' and \
  tabn=%d and prn='%d' and knah=%d and godn=%d and mesn=%d and \
  podr=%d and shet='%s' and nomz=%d",gd,md,dd,atoi(tabn),prn,atoi(knah),
  godn,mesn,atoi(podr),shet,nomz);

  sql_zapis(strsql,1,0);   
 }
double suma=VV.data_ravno_atof(2);
if(prn == 2)
 suma*=-1; 
int kvoz;
for(int nomzap=nomz; nomzap < 1000 ; nomzap++)
 {
  kvoz=zapzarp(dd,md,gd,VV.data_ravno_atol(0),prn,VV.data_ravno_atoi(1),suma,VV.data_ravno(3),
  mesn,godn,VV.data_ravno_atoi(7),nomzap,VV.data_ravno(4),VV.data_ravno_atoi(5),nomdok,zp);
  
  if(kvoz == 0) //записали
   break;

  if(zp.dz != 0) //Корректировка записи - не может быть неуспешного завершения
   return(1);

  if(kvoz != 1) //Не равно - запись уже есть
   return(1);

 }
zarsoc(md,gd,VV.data_ravno_atoi(0));
zaravpr(VV.data_ravno_atol(0),md,gd,NULL);

return(0);
}

/**************************/
/*Шапка документа*/
/***************************/
void dirzardkz_sap(short prn,short dd,short md,short gd,const char *nomdok,class poi_dirzardkz *poisk)
{
int		Y,X;

clear();

move(0,0);
printw("%s:%d.%d.%d %s:%s %s:%.2f %s:%d",
gettext("Дата"),dd,md,gd,
gettext("Номер"),nomdok,
gettext("Сумма"),poisk->suma_itogo,
gettext("Дни"),poisk->it_dni);
if(prn == 1)
 printw(" %s",gettext("Начисления"));
if(prn == 2)
 printw(" %s",gettext("Удержания"));
 
move(2,0);
printw("    1            2        3      4      5    6     7   8");

mvprintw(Y=1,X=COLS-20,gettext("Расшифровка полей:"));
mvprintw(++Y,X,"1-%s",gettext("таб.номер"));
mvprintw(++Y,X,"2-%s",gettext("фамилия"));
mvprintw(++Y,X,"3-%s",gettext("счёт"));
mvprintw(++Y,X,"4-%s",gettext("сумма"));
mvprintw(++Y,X,"5-%s",gettext("код начис."));
mvprintw(++Y,X,"6-%s",gettext("в счёт мес."));
mvprintw(++Y,X,"7-%s",gettext("дни"));
mvprintw(++Y,X,"8-%s",gettext("наим.начисления"));

if(poisk->metka_poi == 1)
 {
//  attron(A_BLINK);
  mvprintw(Y+=2,X,gettext("Поиск:"));
  if(poisk->tabn.getdlinna() > 1)
    mvprintw(++Y,X,"%s:%.12s",gettext("Таб.н."),poisk->tabn.ravno());
  if(poisk->knah.getdlinna() > 1)
    mvprintw(++Y,X,"%s:%.12s",gettext("Код.нар."),poisk->knah.ravno());
  if(poisk->fio.getdlinna() > 1)
    mvprintw(++Y,X,"%s:%.12s",gettext("Фамил."),poisk->fio.ravno());
  if(poisk->shet.getdlinna() > 1)
    mvprintw(++Y,X,"%s:%.12s",gettext("Счёт"),poisk->shet.ravno());
  if(poisk->koment.getdlinna() > 1)
    mvprintw(++Y,X,"%s:%.12s",gettext("Комент."),poisk->koment.ravno());

//  attroff(A_BLINK);
  
 }

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("ввод"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F6",gettext("табель"),
"F9",gettext("импорт"),
"F10",gettext("выход"),NULL);


}

/*****************/
/*Проверка на условия поиска*/
/****************************/

int dirzardkz_prov(SQL_str row,class poi_dirzardkz *poisk)
{

char strsql[512];
SQL_str row1;
SQLCURSOR cur;

poisk->fio_s.new_plus("");


sprintf(strsql,"select fio from Kartb where tabn=%s",row[0]);
if(readkey(strsql,&row1,&cur) == 1)
 poisk->fio_s.new_plus(row1[0]);
 
if(poisk->metka_poi == 0)
 return(0);
 
if(proverka(poisk->tabn.ravno(),row[0],0,0) != 0)
 return(1);
if(proverka(poisk->shet.ravno(),row[3],0,0) != 0)
 return(1);
if(proverka(poisk->knah.ravno(),row[1],0,0) != 0)
 return(1);

if(poisk->fio.getdlinna() > 1)
 if(strstrm(poisk->fio_s.ravno(),poisk->fio.ravno()) == 0)
      return(1);

if(poisk->koment.getdlinna() > 1)
 if(strstrm(row[6],poisk->koment.ravno()) == 0)
      return(1);

return(0);

}
/**************************/
/*Поиск*/
/*************************/

void dirzardkz_poi(class poi_dirzardkz *poisk)
{
class iceb_tu_str fio("");
class iceb_tu_str kod("");
int kom1=0;
VVOD MENU(3);
VVOD VV(0);

VV.data_sp_plus(poisk->tabn.ravno(),32);
VV.data_sp_plus(poisk->knah.ravno(),128);
VV.data_sp_plus(poisk->fio.ravno(),128);
VV.data_sp_plus(poisk->shet.ravno(),128);
VV.data_sp_plus(poisk->koment.ravno(),128);

VV.VVOD_SPISOK_add_ZAG(gettext("Поиск"));

VV.VVOD_SPISOK_add_MD(gettext("Табельный номер (,,).."));//0
VV.VVOD_SPISOK_add_MD(gettext("Код начисления  (,,).."));//1
VV.VVOD_SPISOK_add_MD(gettext("Фамилия..............."));//2
VV.VVOD_SPISOK_add_MD(gettext("Счёт............(,,).."));//3
VV.VVOD_SPISOK_add_MD(gettext("Коментарий............"));//4


naz:;
    

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("поиск"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);


int kom=VV.vvod(0,1,1,-1,-1);

GDITE();
poisk->tabn.new_plus(VV.data_ravno(0));
poisk->knah.new_plus(VV.data_ravno(1));
poisk->fio.new_plus(VV.data_ravno(2));
poisk->shet.new_plus(VV.data_ravno(3));
poisk->koment.new_plus(VV.data_ravno(4));
fio.new_plus("");
switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("zarp2_12_2.txt");
   goto naz;

  case FK2:
  case PLU:

    poisk->metka_poi=1;     
    return;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите  нужное и нажмите Enter"));
    
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка начисленй"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка табельных номеров"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);
    switch (kom1)
     {
      case 3 :
      case -1:
       goto naz;

      case 0 :
        if(vibrek("Plansh",&kod) == 0)
          VV.data_z_plus(3,kod.ravno());
        break;

      case 1 :
        if(dirnach(1,&kod,&fio) == 0)
          VV.data_z_plus(1,kod.ravno());
        break;

      case 2 :
        int tabnom=0;
        if(dirtab(&tabnom,&fio,0,1) == 0)
          VV.data_z_plus(0,tabnom);
        clear(); //обязательно очищаем экран от предыдущего меню
        break;

     }
    
    goto naz;

  case FK4:
    VV.VVOD_clear_data();
    goto naz;
    
  case FK10:
  case ESC:
    poisk->metka_poi=0;
    return;

  default:
   goto naz;
 }

}
/***********************/
/*Удалить запись*/
/********************/
int  dirzarkz_uz(short dd,short md,short gd,
const char *tabn,
int prn,
const char *knah,
const char *podr,
const char *shet,
int nomz,
short mesn,
short godn)
{
if(zarudnu(gd,md,dd,atol(tabn),prn,atoi(knah),godn,mesn,atoi(podr),shet,nomz) != 0)
 return(1);

zarsoc(md,gd,atoi(tabn));

zaravpr(atol(tabn),md,gd,NULL);
return(0);
}
/****************************/
/*Удалить документ*/
/*********************/
int dirzardkz_ud(SQLCURSOR *cur,int kolstr,short god_dok,short dd,short md,short gd,const char *nomdok,short prn)
{

SQL_str row;
cur->poz_cursor(0);
int voz=0;
int kolstr1=0;
while(cur->read_cursor(&row) != NULL)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(dirzarkz_uz(dd,md,gd,row[0],prn,row[1],row[5],row[3],atoi(row[4]),atoi(row[8]),atoi(row[7])) != 0)
   {
   if(kolstr1 == 1)
    {
     return(1);
    }    
   else
    voz++;
  }
 }

if(voz != 0) 
 return(voz); 

//Удаляем шапку документа
char strsql[512];
sprintf(strsql,"delete from Zardok where god=%d and nomd='%s'",god_dok,nomdok);
return(sql_zapis(strsql,0,0));
}
/************************/
/*шапка*/
/***************************/
void dirzardkz_hap(int nomstr,int *kolstr,FILE *ff)
{
if(kolstr != NULL)
 *kolstr+=4;
 
fprintf(ff,"%70s %s N%d\n","",gettext("Страница"),nomstr);

fprintf(ff,"\
------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
Таб.н.|Фамилия, Имя, Отчество        | Счёт  |  Сумма   |Месяц  |Дни|Код|Наименование\n"));
/********
123456 123456789012345678901234567890 1234567 1234567890 124 1234567
***********/
fprintf(ff,"\
------------------------------------------------------------------------------------------\n");

}

/****************/
/*распечатка*/
/*****************/
void dirzardkz_ras(SQLCURSOR *cur,int kolstr,
short dd,short md,short gd,
const char *nomdok,
short prn,
class poi_dirzardkz *poisk)
{

FILE *ff;
char imaf[64];
sprintf(imaf,"nar%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

if(prn == 1)
  iceb_t_zagolov(gettext("Начисления зарплаты"),ff); 
if(prn == 2)
  iceb_t_zagolov(gettext("Удержания зарплаты"),ff); 

fprintf(ff,"%s:%s %s:%d.%d.%d%s\n",
gettext("Номер документа"),nomdok,
gettext("Дата"),dd,md,gd,
gettext("г."));

SQL_str row;
SQL_str row1;
SQLCURSOR cur1;

/*Узнаём комментарий документа*/
char strsql[512];
sprintf(strsql,"select koment from Zardok where nomd=%s and god=%d",nomdok,gd);
if(readkey(strsql,&row1,&cur1) == 1)
 if(row1[0][0] != '\0')
  fprintf(ff,"%s\n",row1[0]);

dirzardkz_hap(1,NULL,ff);

class iceb_tu_str naim_nah("");
int kolstr1=0;
double itogo=0.;
cur->poz_cursor(0);
int it_dn=0;
class masiv_din_int kod_nah;
class masiv_din_double kod_nah_sum;
class masiv_din_int kod_nah_dni;

int nomer_nah;
while(cur->read_cursor(&row) != NULL)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(dirzardkz_prov(row,poisk) != 0)
   continue;
  
  if((nomer_nah=kod_nah.find(atoi(row[1]))) < 0)
    kod_nah.plus(row[1]);
  kod_nah_sum.plus(atof(row[2]),nomer_nah);
  kod_nah_dni.plus(atoi(row[9]),nomer_nah);
  
  naim_nah.new_plus("");

  if(prn == 1)
    sprintf(strsql,"select naik from Nash where kod=%s",row[1]);
  if(prn == 2)
    sprintf(strsql,"select naik from Uder where kod=%s",row[1]);
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_nah.new_plus(row1[0]);

  fprintf(ff,"%-6s %-*.*s %-*s %10.2f %02d.%04d %3s %-3s %s\n",
  row[0],
  iceb_tu_kolbait(30,poisk->fio_s.ravno()),iceb_tu_kolbait(30,poisk->fio_s.ravno()),poisk->fio_s.ravno(),
  iceb_tu_kolbait(7,row[3]),row[3],
  atof(row[2]),atoi(row[8]),atoi(row[7]),row[9],row[1],naim_nah.ravno());
  
  itogo+=atof(row[2]); 
  it_dn+=atoi(row[9]);
 }

fprintf(ff,"ICEB_LST_END\n\
------------------------------------------------------------------------------------------\n");

fprintf(ff,"\
%50s %10.2f         %3d\n",gettext("Итого"),itogo,it_dn);

fprintf(ff,"%s\n",gettext("Свод"));
fprintf(ff,"\
--------------------------------------------------\n");
fprintf(ff,"\
Код|         Наименование         |  Сумма   |Дни|\n");
fprintf(ff,"\
--------------------------------------------------\n");
for(int ii=0; ii < kod_nah.kolih(); ii++)
 {
  naim_nah.new_plus("");

  if(prn == 1)
    sprintf(strsql,"select naik from Nash where kod=%d",kod_nah.ravno(ii));
  if(prn == 2)
    sprintf(strsql,"select naik from Uder where kod=%d",kod_nah.ravno(ii));
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_nah.new_plus(row1[0]);
  
  fprintf(ff,"%3d %-*.*s %10.2f %3d\n",
  kod_nah.ravno(ii),
  iceb_tu_kolbait(30,naim_nah.ravno()),iceb_tu_kolbait(30,naim_nah.ravno()),naim_nah.ravno(),
  kod_nah_sum.ravno(ii),kod_nah_dni.ravno(ii));
 }
fprintf(ff,"\
--------------------------------------------------\n");
fprintf(ff,"%39.39s:%10.2f %3d\n",gettext("Итого"),itogo,it_dn);

podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка документа"));
int orient=iceb_t_ustpeh(oth.spis_imaf.ravno(0),0);

iceb_t_rnl(imaf,orient,"",&dirzardkz_hap);

iceb_t_rabfil(&oth,"");

}
