/*$Id: dirsubbal.c,v 5.20 2013-04-07 10:22:54 sasa Exp $*/
/*14.01.2010	30.03.2007	Белых А.И.	dirsubbal.c
Ввод и корректировка списка суббалансов
Если вернули 0-выбрали
             1- нет
*/
#include        <errno.h>
#include        "buhl.h"

class dirsubbal_poi
 {
  public:
   int metka_sort;    //0-сортировка по возростанию номера склада 1-по наименованию
   class iceb_tu_str naimpoi;  
  dirsubbal_poi()
   {
    metka_sort=0;    //0-сортировка по возростанию номера склада 1-по наименованию
    naimpoi.plus("");
   }
 };

void	dirsubbal_sap(int metkar,class dirsubbal_poi *poisk);
void dirsubbal_vvod(const char *kod_subbal);
int dirsubbal_provud(const char *kod_subbal,int metka_soob);
void dirsubbal_unk();
void dirsubbal_korkod(const char *old_kod,const char *new_kod);
void dirsubbal_rs(class SQLCURSOR *cur);



int dirsubbal(int metkar, //0-работа 1-выбор из списка с корректировкой 2-чистый выбор
class iceb_tu_str *kod_subbal,class iceb_tu_str *naim_subbal)
{
char		strsql[1024];
SQL_str		row;
int		kolstr=0;
int		K=0;
int		pozz=0,pozz1=0;
int		i=0,kom=0;
int		mspz=0;
int		prc;
int             Y=3;
int             X=0;
class iceb_tu_str koment("");
class dirsubbal_poi poisk;
class iceb_t_mstr spis_menu(0);

poisk.naimpoi.new_plus(naim_subbal->ravno());

if(metkar == 2)
 {
  Y=-1;
  X=-1;
  koment.new_plus(gettext("Выберите нужное и нажмите Enter"));
 }

//Создаем класс меню
VVOD DANET(1);
VVOD VVOD1(2);

naz1:;

short           kls=LINES-6;

/*MEN5 MM5(kls,COLS,0);*/

sprintf(strsql,"select * from Glksubbal");

  
class SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
lll:;
  dirsubbal_sap(metkar,&poisk);
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


/*MM5.clearMEN5();*/

cur.poz_cursor(pozz);
mspz=2;
pozz1=0;
spis_menu.spis_str.free_class();

for(i=0;i<=kls-1;)
 {
  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;

  if(poisk.naimpoi.ravno()[0] != '\0' && strstrm(row[1],poisk.naimpoi.ravno()) == 0)
   continue;
  
  sprintf(strsql,"%-*s|%s",iceb_tu_kolbait(3,row[0]),row[0],row[1]);
  
  i++;
  spis_menu.spis_str.plus(strsql);
  
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

dirsubbal_sap(metkar,&poisk);

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
    if(poisk.naimpoi.ravno()[0] != '\0' && strstrm(row[5],poisk.naimpoi.ravno()) == 0)
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
    if(poisk.naimpoi.ravno()[0] != '\0' && strstrm(row[5],poisk.naimpoi.ravno()) == 0)
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
    if(poisk.naimpoi.ravno()[0] != '\0' && strstrm(row[5],poisk.naimpoi.ravno()) == 0)
     continue;
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

  polen(spis_menu.spis_str.ravno(kom),kod_subbal,1,'|');
  polen(spis_menu.spis_str.ravno(kom),naim_subbal,2,'|');

 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     kod_subbal->new_plus("");
     naim_subbal->new_plus("");
     return(1);
     break;

  case FK1:  /*Получить подсказку*/
    GDITE();
    iceb_t_pdoc("buhg4_8.txt");
    goto naz;

  case ENTER:
    if(metkar != 0)
      return(0);
      
  case SFK2:   /*Корректировать*/
    dirsubbal_vvod(kod_subbal->ravno());
    goto naz1;

  case FK2:  /*Ввести запись*/
  case PLU:
    dirsubbal_vvod("");
    goto naz1;

  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      if(dirsubbal_provud(kod_subbal->ravno(),1) != 0)
        goto naz;
      sprintf(strsql,"delete from Glksubbal where kod=%s",kod_subbal->ravno());
      if(sql_zapis(strsql,0,0) == 0)
        goto naz1;
     }
    goto naz;

  case SFK3:       //Удалить неиспользуемые коды суббаланса
    dirsubbal_unk();
    goto naz1;
    

  case FK4:       /*Поиск*/

     helstr(LINES-1,0," F10 ",gettext("выход"),NULL);

     VVOD1.VVOD_delete();
     VVOD1.VVOD_SPISOK_add_MD(gettext("Введите образец для поиска"));
     
     poisk.naimpoi.new_plus("");
     
     vvod1(&poisk.naimpoi,20,&VVOD1,NULL,stdscr,-1,-1);

     GDITE();

     clear();
     kom=pozz=0;
     goto naz;

  case FK5:    //Распечатать
    dirsubbal_rs(&cur);
    goto naz;


  default:
    goto naz;
    break;
 }

return(1);



}
/**************************/
/*Шапка                   */
/**************************/

void	dirsubbal_sap(int metkar,class dirsubbal_poi *poisk)
{
short		y=2;
short           x=COLS-20;



if(metkar != 2)
 {
  clear();
  printw(gettext("Ввод и корректировка списка суббалансов"));
 }
if(metkar == 1)
 {
  move(1,0);
  printw(gettext("Найдите нужное и нажмите \"Enter\""));
 }
                       
mvprintw(2,0,"   1         2");
mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,"1-%s",gettext("код суббаланса"));
mvprintw(y++,x,"2-%s",gettext("наименование"));

if(poisk->naimpoi.ravno()[0] != '\0')
 {
  y++;
  attron(A_BLINK);
  mvprintw(y++,x,gettext("Поиск:"));
  mvprintw(y++,x,"%s",poisk->naimpoi.ravno());
  attroff(A_BLINK);
  
 }

if(metkar < 2)
  helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F10",gettext("выход"),NULL);

if(metkar == 2)
  helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
  "F10",gettext("выход"),NULL);

}



/*********************/
/*Запись             */
/*********************/
//Если вернули 0 - записали 1-нет

int dirsubbal_zap(VVOD *VV,const char *kod_subbal)
{
char	strsql[512];
//SQL_str row;

attroff(VV->VVOD_return_cs(iceb_CFS));
attron(VV->VVOD_return_cs(iceb_CFM));

if(VV->VVOD_SPISOK_return_data(0)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(1)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
  return(1);
 }       


if(SRAV(kod_subbal,VV->data_ravno(0),0) != 0)
 {
  //Проверяем может такой склад уже введён
  sprintf(strsql,"select kod from Glksubbal where kod=%s",VV->data_ravno(0));
  if(readkey(strsql) >= 1)
   {
    iceb_t_soob(gettext("Такой код суббаланса уже ввели !"));
    return(1);
   }

  if(kod_subbal[0] != '\0')
   {
    VVOD DANET(1);
    DANET.VVOD_SPISOK_add_MD(gettext("Корректировать код ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      return(1);
   }

 }
 
time_t vrem;
time(&vrem);

if(kod_subbal[0] == '\0') //Ввод новой записи
  sprintf(strsql,"insert into Glksubbal values(%s,'%s',%d,%ld)",
  VV->VVOD_SPISOK_return_data(0),  
  VV->VVOD_SPISOK_return_data(1),  
  iceb_t_getuid(),vrem);
else
  sprintf(strsql,"update Glksubbal set \
kod=%s,\
naik='%s',\
ktoz=%d,\
vrem=%ld \
where kod=%s",  
  VV->data_ravno(0),
  VV->data_ravno(1),  
  iceb_t_getuid(),
  vrem,
  kod_subbal);


if(sql_zapis(strsql,0,0) != 0)
 return(1);


if(kod_subbal[0] != '\0' && SRAV(kod_subbal,VV->VVOD_SPISOK_return_data(0),0) != 0)
  dirsubbal_korkod(kod_subbal,VV->data_ravno(0)); //Корректировка кода таблицах


return(0);


}

/**********************/
/*Ввод и корректировки*/
/**********************/

void dirsubbal_vvod(const char *kod_subbal)
{
char		strsql[512];
SQL_str		row;
time_t		vremz=0;
int		ktozap=0;
struct  tm      *bf;
int		N=1,K=0;

SQLCURSOR curr;

VVOD VV(0);

VV.VVOD_SPISOK_add_data(10);//0
VV.VVOD_SPISOK_add_data(60);//1


if(kod_subbal[0] != '\0')
 {

  sprintf(strsql,"select * from Glksubbal where kod=%s",kod_subbal);
  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   return;   

  VV.VVOD_SPISOK_zapis_data(0,row[0]);
  VV.VVOD_SPISOK_zapis_data(1,row[1]);
   
  ktozap=atoi(row[2]);
  vremz=atol(row[3]);
     
 }
else
 {
  sprintf(strsql,"%d",oprsk("Glksubbal"));
  VV.VVOD_SPISOK_zapis_data(0,strsql);
 }



VV.VVOD_SPISOK_add_MD(gettext("Код суббаланса..........."));//0
VV.VVOD_SPISOK_add_MD(gettext("Наименование суббаланса.."));//1


naz:;


VV.VVOD_delete_ZAG();

VV.VVOD_SPISOK_add_ZAG(gettext("Ввод и корректировка суббалансов"));

if(ktozap != 0)
 {
  sprintf(strsql,"%s:%s",gettext("Записал"),iceb_t_kszap(ktozap));
  VV.VVOD_SPISOK_add_ZAG(strsql);
 }

if(vremz != 0)
 {
  bf=localtime(&vremz);
  sprintf(strsql,"%s: %d.%d.%d%s %s:%02d:%02d:%02d",
  gettext("Дата записи"),
  bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
  gettext("г."),
  gettext("Время"),
  bf->tm_hour,bf->tm_min,bf->tm_sec);
  VV.VVOD_SPISOK_add_ZAG(strsql);
 }


 helstr(LINES-1,0,
 //"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("реквизиты"),
"F10",gettext("выход"),NULL);


VV.vramka(2,0,0);

for(;;)
 {

  attron(VV.VVOD_return_cs(iceb_CFS)); 
  VV.vprintw(N,stdscr);

  refresh();
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



//    my_getstr(VV.VVOD_SPISOK_return_data(N),dlinastr,VV.VVOD_SPISOK_return_dls(N),stdscr);
    VV.vgetstr(N,stdscr);

    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 1 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')
     {
     }

    if(N == 5 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')
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
//    prosf(strsql);
    clear();
    goto naz;
   }

  if(K == FK2 || K == PLU) //Записываем
   {
    if(dirsubbal_zap(&VV,kod_subbal) != 0)
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
/********************************/
/*Проверка возможности удаления*/
/*******************************/
//Если вернули 0- удалять можно 1-нельзя
int dirsubbal_provud(const char *kod_subbal,int metka_soob) //0-без сообщения 1-сообщить о невозможности удаления
{
char strsql[512];
if(metka_soob == 1)
 {
  clear();
  printw("%s Plansh\n",gettext("Проверяем записи в таблице"));
  refresh();
 }
 
sprintf(strsql,"select ns from Plansh where ksb=%s",kod_subbal);
if(readkey(strsql) >= 1)
 {
  if(metka_soob == 1)
   {
    printw("%s\n",gettext("Код %s удалить невозможно, потому что есть записи с этим кодом"),kod_subbal); 
    OSTANOV();
   }
  return(1);
 }
return(0);

}
/****************************************/
/*Удалить неиспользуемые коды суббаланса*/
/****************************************/
void dirsubbal_unk()
{
int kolstr;
char strsql[512];
SQL_str row;
SQLCURSOR cur;

clear();
printw("%s\n\n",gettext("Удаление незадействованых кодов"));

GDITE();

sprintf(strsql,"select * from %s","Glksubbal");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
int kolstr1=0;
int kolsklud=0;
while(cur.read_cursor(&row) != 0)
 {
  
  printw("%5s %-40s ",row[0],row[1]);
  strzag(LINES-1,0,kolstr,++kolstr1);
  
  if(dirsubbal_provud(row[0],0) != 0)
   {
    printw("\n");
    continue;
   }
  sprintf(strsql,"delete from %s where kod=%s","Glksubbal",row[0]);
  if(sql_zapis(strsql,1,0) != 0)
   continue;
   
  kolsklud++;  
  printw("%s\n",gettext("Удален"));
  
 }

sprintf(strsql,"%s %d",gettext("Количество удалённых записей"),kolsklud);
iceb_t_soob(strsql);
}
/***************************/
/*Корректировка кода суббаланса*/
/*******************************/

void dirsubbal_korkod(const char *old_kod,const char *new_kod)
{

char strsql[512];
sprintf(strsql,"update Plansh set ksb=%s where ksb=%s",new_kod,old_kod);
sql_zapis(strsql,0,0);


}
/*****************************/
/*Распечатка списка суббалансов*/
/*******************************/
void dirsubbal_rs(class SQLCURSOR *cur)
{
char	imaf[64];
FILE	*ff;
SQL_str row;

sprintf(imaf,"subbal%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }


iceb_t_zagolov(gettext("Список суббалансов"),0,0,0,0,0,0,ff);


fprintf(ff,"\
------------------------------------\n");
fprintf(ff,"\
Код | Наименование суббаланса      |\n");

/*

*/

fprintf(ff,"\
------------------------------------\n");

cur->poz_cursor(0);

while(cur->read_cursor(&row) != 0)
 {
   
  fprintf(ff,"%-*s %-*s\n",
  iceb_tu_kolbait(4,row[0]),row[0],
  iceb_tu_kolbait(30,row[1]),row[1]);
 }
fprintf(ff,"\
------------------------------------\n");

podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Список суббалансов"));
iceb_t_ustpeh(imaf,3);
iceb_t_rabfil(&oth,"");

}

