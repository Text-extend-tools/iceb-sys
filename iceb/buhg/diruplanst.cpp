/*$Id: diruplanst.c,v 5.2 2013/05/17 14:55:59 sasa Exp $*/
/*14.07.2015	10.04.2012	Белых А.И.	diruplanst.c
Ввод и корректировка списка норм списания топлива для автомобиля
*/
#include        <errno.h>
#include        "buhg.h"

class diruplanst_poi
 {
  public:
   int kod_avt;
   int metkar;
  diruplanst_poi()
   {
   }
 };
 
void diruplanst_vvod(int kod_avt,const char *kod_skl);
void diruplanst_sap(class diruplanst_poi*);
int  diruplanst_zap(int kod_avt,VVOD*,const char*);
void diruplanst_ras(int kod_avt,SQLCURSOR*);



int diruplanst(int metkar,int kod_avt,class iceb_tu_str *nst)
{
char		strsql[1024];
class iceb_t_mstr spis_menu(0);
SQL_str		row,row1;
int		kolstr=0;
int		K=0;
int		pozz=0,pozz1=0;
int		i=0,kom=0;
int		mspz=0;
int		prc;
int             Y=3;
int             X=0;
class iceb_tu_str koment("");
class diruplanst_poi poisk;
class iceb_tu_str kod_nst("");
class iceb_tu_str naim_nst("");
nst->new_plus("");

poisk.kod_avt=kod_avt;
poisk.metkar=metkar;
//Создаем класс меню
VVOD DANET(1);
VVOD VVOD1(2);

if(metkar == 2)
 {
  Y=-1;
  X=-1;
  koment.new_plus(gettext("Выберите нужное и нажмите Enter"));
 }

naz1:;

short           kls=LINES-6;

sprintf(strsql,"select * from Uplavt1 where kod=%d order by knst asc",kod_avt);

  
class SQLCURSOR cur,cur1;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
lll:;
  diruplanst_sap(&poisk);
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


  naim_nst.new_plus("");
  sprintf(strsql,"select naik from Uplnst where kod='%s'",row[1]);
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_nst.new_plus(row1[0]);
        
  sprintf(strsql,"%-*s|%-*.*s|%-*s|%7s",
  iceb_tu_kolbait(6,row[1]),
  row[1],
  iceb_tu_kolbait(20,naim_nst.ravno()),  
  iceb_tu_kolbait(20,naim_nst.ravno()),  
  naim_nst.ravno(),
  iceb_tu_kolbait(6,row[3]),
  row[3],
  row[2]);

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

diruplanst_sap(&poisk);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,Y,X,NULL,strsql,prc,&K,koment.ravno(),0);


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

  polen(spis_menu.spis_str.ravno(kom),&kod_nst,1,'|');

 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return(1);
     break;

  case FK1:  /*Получить подсказку*/
    GDITE();
//    iceb_t_pdoc("matu4_6.txt");
    goto naz;

  case ENTER:
    if(metkar != 0)
     {
      nst->new_plus(kod_nst.ravno());
      return(0);
     }     
  case SFK2:   /*Корректировать*/
    diruplanst_vvod(kod_avt,kod_nst.ravno());
    goto naz1;

  case FK2:  /*Ввести запись*/
  case PLU:
    diruplanst_vvod(kod_avt,"");
    goto naz1;


  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
     goto naz;


    sprintf(strsql,"delete from Uplavt1 where kod=%d and knst='%s'",kod_avt,kod_nst.ravno());
    sql_zapis(strsql,0,0);

    goto naz;


  case FK5:    //Распечатать
    diruplanst_ras(kod_avt,&cur);
    goto naz;


  default:
    goto naz;
    break;
 }

return(1);


}
/**********************/
/*Ввод и корректировки*/
/**********************/

void diruplanst_vvod(int kod_avt,const char *kod_nst)
{
char		strsql[2048];
SQL_str		row;
time_t		vremz=0;
int		ktozap=0;
int		N=0,K=0;
class iceb_tu_str kod("");
class iceb_tu_str naimkod("");

SQLCURSOR curr;

VVOD VV(0);

VV.VVOD_SPISOK_add_data(16);//0
VV.VVOD_SPISOK_add_data(16);//1
VV.VVOD_SPISOK_add_data(20);//2



if(kod_nst[0] != '\0')
 {

  sprintf(strsql,"select * from Uplavt1 where kod=%d and knst='%s'",kod_avt,kod_nst);
  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
    iceb_t_soob(gettext("Не найдена запись для корректировки!"));
    return;   
   }
  VV.VVOD_SPISOK_zapis_data(0,row[1]);
  VV.VVOD_SPISOK_zapis_data(1,row[3]);
  VV.VVOD_SPISOK_zapis_data(2,row[2]);

  ktozap=atoi(row[4]);
  vremz=atol(row[5]);

  VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка вида нормы списания топлива"));
  kzvz(ktozap,vremz,&VV);     
 }
else
  VV.VVOD_SPISOK_add_ZAG(gettext("Ввод нового вида нормы списания топлива"));



VV.VVOD_SPISOK_add_MD(gettext("Код нормы списания.."));//0
VV.VVOD_SPISOK_add_MD(gettext("Единица измерения..."));//1
VV.VVOD_SPISOK_add_MD(gettext("Норма списания......"));//2


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
    if(N == 0) //норма списния топлива
     {
      kod.new_plus("");
      naimkod.new_plus("");
      if(diruplnst(2,&kod,&naimkod) == 0)
       {
        VV.data_plus(0,kod.ravno());
        N++;        
       }
      goto naz; //всегда чтобы поверх предыдущего меню появилось рабочее меню
     }

    if(N == 1) //единица измерения
     {
      kod.new_plus("");
      naimkod.new_plus("");
      if(iceb_tl_ei(2,&kod,&naimkod) == 0)
       {
        VV.data_plus(1,kod.ravno());
        N++;        
       }
      goto naz;
     }
 
    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));
/**************
    if(N == 1 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')
     {
     }

    if(N == 5 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')
     {
     }
*******************/
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
//    prosf(strsql);
    clear();
    goto naz;
   }

  if(K == FK2 || K == PLU) //Записываем
   {
    if(diruplanst_zap(kod_avt,&VV,kod_nst) != 0)
     goto naz;
    break;
   }

 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
clear();
refresh();
return;
}

/*********************/
/*Запись             */
/*********************/
//Если вернули 0 - записали 1-нет

int   diruplanst_zap(int kod_avt,VVOD *VV,const char *kod_nst)
{
char	strsql[2048];

attroff(VV->VVOD_return_cs(iceb_CFS));
attron(VV->VVOD_return_cs(iceb_CFM));

if(VV->VVOD_SPISOK_return_data(0)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(1)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(2)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
  return(1);
 }       

/*проверяем код в списке*/
sprintf(strsql,"select kod from Uplnst where kod='%s'",VV->data_ravno(0));
if(readkey(strsql) != 1)
 {
  sprintf(strsql,"%s %s!",gettext("Не существует кода нормы списания топлива"),VV->data_ravno(0));
  iceb_t_soob(strsql);
  return(1);
 }
/*провераяем единицу измерения*/
sprintf(strsql,"select kod from Edizmer where kod='%s'",VV->data_ravno(1));
if(readkey(strsql) != 1)
 {
  sprintf(strsql,"%s %s!",gettext("Не существует единицы измерения"),VV->data_ravno(1));
  iceb_t_soob(strsql);
  return(1);
 }

if(SRAV(kod_nst,VV->data_ravno(0),0) != 0)
 {
  //Проверяем может код уже введён
  sprintf(strsql,"select kod from Uplavt1 where kod=%d and knst='%s'",kod_avt,VV->data_ravno(0));
  if(readkey(strsql) >= 1)
   {
    iceb_t_soob(gettext("Такой код нормы списани топлива уже ввели !"));
    return(1);
   }

  if(kod_nst[0] != '\0')
   {
/*********
    VVOD DANET(1);
    DANET.VVOD_SPISOK_add_MD(gettext("Корректировать код ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      return(1);
**************/
    
    iceb_t_soob(gettext("Корректировка кода нормы списания топлива невозможна!"));
    return(1);
   }

 }
 
time_t vrem;
time(&vrem);

if(kod_nst[0] == '\0') //Ввод новой записи
  sprintf(strsql,"insert into Uplavt1 values(%d,'%s',%.3f,'%s',%d,%ld)",
  kod_avt,
  VV->data_ravno_filtr(0),  
  VV->data_ravno_atof(2),
  VV->data_ravno_filtr(1),  
  iceb_t_getuid(),vrem);
else
  sprintf(strsql,"update Uplavt1 set \
nst=%.3f,\
ei='%s',\
ktoz=%d,\
vrem=%ld \
where kod=%d and knst='%s'",  
  VV->data_ravno_atof(2),  
  VV->data_ravno_filtr(1),
  iceb_t_getuid(),
  vrem,
  kod_avt,kod_nst);


if(sql_zapis(strsql,0,0) != 0)
 return(1);



return(0);


}


/**************************/
/*Шапка                   */
/**************************/

void	diruplanst_sap(class diruplanst_poi *poisk)
{
short		y=2;
short           x=COLS-20;

if(poisk->metkar != 2)
 {
  clear();
  printw(gettext("Ввод и корректировка списка видов норм списания топлива"));
 }
if(poisk->metkar == 1)
 {
  move(1,0);
  printw(gettext("Найдите нужное и нажмите \"Enter\""));
 }

if(poisk->metkar < 2)
 {
  class iceb_tu_str naim_avt("");  
  class iceb_tu_str nom_znak("");
  char strsql[512];
  SQL_str row;
  class SQLCURSOR cur;
  
  sprintf(strsql,"select nomz,naik from Uplavt where kod=%d",poisk->kod_avt);
  if(readkey(strsql,&row,&cur) == 1)
   {
    nom_znak.new_plus(row[0]);
    naim_avt.new_plus(row[1]);
   }
  mvprintw(1,0,"%s:%d %s %s",gettext("Автомобиль"),poisk->kod_avt,nom_znak.ravno(),naim_avt.ravno());

  mvprintw(2,0,"   1            2               3       4");
  mvprintw(y++,x,gettext("Наименование полей:"));
  mvprintw(y++,x,"1-%s",gettext("код"));
  mvprintw(y++,x,"2-%s",gettext("наименование"));
  mvprintw(y++,x,"3-%s",gettext("един.измерения"));
  mvprintw(y++,x,"4-%s",gettext("норма"));


  helstr(LINES-1,0,
  //"F1",gettext("помощь"),
  "F2/+",gettext("запись"),
  "F3",gettext("удалить"),
  "F5",gettext("печать"),
  "F10",gettext("выход"),NULL);

 }

if(poisk->metkar == 2)
  helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
  "F10",gettext("выход"),NULL);
 
}

/********************/
/*Распечатать       */
/********************/

void	diruplanst_ras(int kod_avt,SQLCURSOR *cur)
{
char	imaf[64];
FILE	*ff;
SQL_str row,row1;
class SQLCURSOR cur1;
char strsql[512];

sprintf(imaf,"nsta%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }


zagolov(gettext("Список норм списания топлива"),0,0,0,0,0,0,iceb_t_get_pnk("00",0),ff);

class iceb_tu_str naim_avt("");
class iceb_tu_str nom_znak("");
sprintf(strsql,"select nomz,naik from Uplavt where kod=%d",kod_avt);
if(readkey(strsql,&row1,&cur1) == 1)
 {
  nom_znak.new_plus(row1[0]);
  naim_avt.new_plus(row1[1]);
 }

fprintf(ff,"%s:%d %s %s\n",gettext("Автомобиль"),kod_avt,nom_znak.ravno(),naim_avt.ravno());

fprintf(ff,"\
-------------------------------------------------------------------------------\n");
fprintf(ff,"\
 Код  | Наименование  вида списания топлива        |Един.измер.|Норма списания|\n");
/*
123456 12345678901234567890123456789012345678901234 12345678901 12345678901234
*/
fprintf(ff,"\
-------------------------------------------------------------------------------\n");
cur->poz_cursor(0);
class iceb_tu_str naim_nst("");
while(cur->read_cursor(&row) != 0)
 {
  naim_nst.new_plus("");
  sprintf(strsql,"select naik from Uplnst where kod='%s'",row[1]);
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_nst.new_plus(row1[0]);
   
  fprintf(ff,"%-*s %-*.*s %-*s %14s\n",
  iceb_tu_kolbait(6,row[1]),
  row[1],
  iceb_tu_kolbait(44,naim_nst.ravno()),
  iceb_tu_kolbait(44,naim_nst.ravno()),
  naim_nst.ravno(),
  iceb_tu_kolbait(11,row[3]),
  row[3],
  row[2]);
 }
fprintf(ff,"\
-------------------------------------------------------------------------------\n");

podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Список норм списания топлива"));

iceb_t_ustpeh(imaf,3);

iceb_t_rabfil(&oth,"");

}
