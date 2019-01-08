/*$Id: diruosgrbu.c,v 5.18 2013/05/17 14:55:59 sasa Exp $*/
/*18.02.2017	12.04.2009	Белых А.И.	diruosgrbu.c
Ввод и корректирвка списка групп бухгалтерского учёта
*/
#include        <errno.h>
#include        "buhg.h"

class diruosgrbu_poi
 {
  public:
   int metka_sort;    //0-сортировка по возростанию номера группы 1-по наименованию
   class iceb_tu_str naimpoi;  
  diruosgrbu_poi()
   {
    metka_sort=0;    //0-сортировка по возростанию номера группы 1-по наименованию
    naimpoi.plus("");
   }
 };
 
void diruosgrbu_vvod(const char *kod_gr);
void diruosgrbu_sap(int,class diruosgrbu_poi*);
int  diruosgrbu_zap(VVOD*,const char*,int);
int             diruosgrbu_prov(const char*,int);
void            diruosgrbu_ras(SQLCURSOR*);
void diruosgrbu_ud_neisp();
void diruosgrbu_korkod(const char *nkod,const char *skod);


int diruosgrbu(int metkar, //0-работа 1-выбор из списка с корректировкой 2-чистый выбор
class iceb_tu_str *kod_gr,class iceb_tu_str *naim_gr)
{
char strsql[1024];
char stroka_menu[1024];
class iceb_t_mstr spis_menu(0);
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
class diruosgrbu_poi poisk;
class iceb_tu_str metka_ar("");
poisk.naimpoi.new_plus(naim_gr->ravno());

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


sprintf(strsql,"select * from Uosgrup1");


if(poisk.metka_sort == 0)
 strcat(strsql," order by kod asc");
if(poisk.metka_sort == 1)
 strcat(strsql," order by naik asc");
  
class SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
lll:;
  diruosgrbu_sap(metkar,&poisk);
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

  if(poisk.naimpoi.ravno()[0] != '\0' && strstrm(row[1],poisk.naimpoi.ravno()) == 0)
   continue;


  if(atoi(row[5]) == 0)
   metka_ar.new_plus(gettext("От начальной балансовой стоимости"));  
  else
   metka_ar.new_plus(gettext("От остаточной балансовой стоимости"));  
  
  sprintf(strsql,"%-*s|%-*.*s|%8s|%s",
  iceb_tu_kolbait(4,row[0]),
  row[0],
  iceb_tu_kolbait(20,row[1]),iceb_tu_kolbait(20,row[1]),row[1],
  row[2],metka_ar.ravno());

  sprintf(stroka_menu,"%.*s",iceb_tu_kolbait(COLS-26,strsql),strsql);

  spis_menu.spis_str.plus(stroka_menu);
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

diruosgrbu_sap(metkar,&poisk);

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

  polen(spis_menu.spis_str.ravno(kom),kod_gr,1,'|');
  polen(spis_menu.spis_str.ravno(kom),naim_gr,2,'|');

 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     kod_gr->new_plus("");
     naim_gr->new_plus("");
     return(1);
     break;

  case FK1:  /*Получить подсказку*/
    GDITE();
//    prosf(strsql);
    goto naz;

  case ENTER:
    if(metkar != 0)
      return(0);
      
  case SFK2:   /*Корректировать*/
    diruosgrbu_vvod(kod_gr->ravno());
    goto naz1;

  case FK2:  /*Ввести запись*/
  case PLU:
    diruosgrbu_vvod("");
    goto naz1;

  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      goto naz;
    GDITE();
    if(diruosgrbu_prov(kod_gr->ravno(),1) != 0)
        goto naz;

    sprintf(strsql,"delete from Uosgrup1 where kod='%s'",kod_gr->ravno());
    sql_zapis(strsql,0,0);
    goto naz1;

  case SFK3:       //Удалить неиспользуемые записи

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить неиспользуемые записи ? Вы уверены ?"));
    if(danet(&DANET,2,stdscr) == 2)
      goto naz;

    diruosgrbu_ud_neisp();
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
    diruosgrbu_ras(&cur);
    goto naz;

  case FK6:    //Переключение сортировки
    poisk.metka_sort++;
    if(poisk.metka_sort > 1)
     poisk.metka_sort=0;
    goto naz1;


  default:
    goto naz;
    break;
 }

return(1);


}
/**********************/
/*Ввод и корректировки*/
/**********************/

void diruosgrbu_vvod(const char *kod_gr)
{
char		strsql[512];
SQL_str		row;
int		N=1,K=0;
int metka_ar=0; /*алгоритм расчёта амортизации 0-от начальной 1- от осточной*/

SQLCURSOR curr;

VVOD VV(0);

VV.VVOD_SPISOK_add_data(10);//0
VV.VVOD_SPISOK_add_data(60);//1
VV.VVOD_SPISOK_add_data(30);//2
VV.VVOD_SPISOK_add_data(60);//3

VV.VVOD_SPISOK_add_ZAG(gettext("Ввод и корректировка групп бухгалтерского учёта"));

if(kod_gr[0] != '\0')
 {

  sprintf(strsql,"select * from Uosgrup1 where kod='%s'",kod_gr);
  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   return;   

  VV.VVOD_SPISOK_zapis_data(0,row[0]);
  VV.VVOD_SPISOK_zapis_data(1,row[1]);
  VV.VVOD_SPISOK_zapis_data(2,row[2]);

  metka_ar=atoi(row[5]);

  kzvz(row[2],row[3],&VV);     
 }
else
 {
  sprintf(strsql,"%d",oprsk("Uosgrup1"));
  VV.VVOD_SPISOK_zapis_data(0,strsql);
 }



VV.VVOD_SPISOK_add_MD(gettext("Код группы...................."));//0
VV.VVOD_SPISOK_add_MD(gettext("Наименование группы..........."));//1
VV.VVOD_SPISOK_add_MD(gettext("Годовая норма амортизации....."));//2
VV.VVOD_SPISOK_add_MD(gettext("Алгоритм расчёта амортизации.."));//3


naz:;

if(metka_ar == 0)
  VV.VVOD_SPISOK_zapis_data(3,gettext("От начальной балансовой стоимости"));
else
 {
  VV.VVOD_SPISOK_zapis_data(3,gettext("От остаточной балансовой стоимости"));
 }     


helstr(LINES-1,0,
//"F1",gettext("помощь"),
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


    if(N == 3) /*алгоритм расчёта амортизации*/
     {
      VVOD MENUW(3);
      MENUW.VVOD_SPISOK_add_MD(gettext("От начальной балансовой стоимости"));
      MENUW.VVOD_SPISOK_add_MD(gettext("От остаточной балансовой стоимости"));
     
      MENUW.VVOD_SPISOK_add_ZAG(gettext("Выберите алгоритм расчёта амортизации"));
      menu3w(stdscr,&MENUW,&metka_ar,-1,-1,0);
      goto naz;
            
     }


//    my_getstr(VV.VVOD_SPISOK_return_data(N),dlinastr,VV.VVOD_SPISOK_return_dls(N),stdscr);
    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 1 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')
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
    if(diruosgrbu_zap(&VV,kod_gr,metka_ar) != 0)
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

int   diruosgrbu_zap(VVOD *VV,const char *kod_gr,int metka_ar)
{
char	strsql[512];
//SQL_str row;

attroff(VV->VVOD_return_cs(iceb_CFS));
attron(VV->VVOD_return_cs(iceb_CFM));

if(VV->VVOD_SPISOK_return_data(0)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(1)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(2)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
  return(1);
 }       


if(SRAV(kod_gr,VV->data_ravno(0),0) != 0)
 {
  //Проверяем может такой склад уже введён
  sprintf(strsql,"select kod from Uosgrup1 where kod='%s'",VV->data_ravno(0));
  if(readkey(strsql) >= 1)
   {
    iceb_t_soob(gettext("Такой код группы уже есть!"));
    return(1);
   }

  if(kod_gr[0] != '\0')
   {
    VVOD DANET(1);
    DANET.VVOD_SPISOK_add_MD(gettext("Корректировать код ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      return(1);
   }

 }
 
time_t vrem;
time(&vrem);

if(kod_gr[0] == '\0') //Ввод новой записи
  sprintf(strsql,"insert into Uosgrup1 values('%s','%s',%.3f,%d,%ld,%d)",
  VV->VVOD_SPISOK_return_data(0),  
  VV->VVOD_SPISOK_return_data(1),  
  VV->data_ravno_atof(2),
  iceb_t_getuid(),vrem,
  metka_ar);
else
  sprintf(strsql,"update Uosgrup1 set \
kod='%s',\
naik='%s',\
kof=%.3f,\
ktoz=%d,\
vrem=%ld,\
ar=%d \
where kod='%s'",  
  VV->data_ravno(0),
  VV->data_ravno(1),  
  VV->data_ravno_atof(2),
  iceb_t_getuid(),
  vrem,
  metka_ar,
  kod_gr);


if(sql_zapis(strsql,0,0) != 0)
 return(1);

if(kod_gr[0] != '\0' && SRAV(kod_gr,VV->VVOD_SPISOK_return_data(0),0) != 0)
  diruosgrbu_korkod(VV->data_ravno(0),kod_gr);
// korkod("Uosgrup1",kod_gr,VV->data_ravno(0)); //Корректировка кода таблицах


return(0);


}


/**************************/
/*Шапка                   */
/**************************/

void	diruosgrbu_sap(int metkar,class diruosgrbu_poi *poisk)
{
short		y=2;
short           x=COLS-20;



if(metkar != 2)
 {
  clear();
  printw(gettext("Ввод и корректировка групп бухгалтерского учёта"));
 }
if(metkar == 1)
 {
  move(1,0);
  printw(gettext("Найдите нужное и нажмите \"Enter\""));
 }
mvprintw(2,0,"   1           2                3         4");
mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,"1-%s",gettext("код группы"));
mvprintw(y++,x,"2-%s",gettext("наименование"));
mvprintw(y++,x,"3-%s",gettext("норм.амортот."));
mvprintw(y++,x,"4-%s",gettext("алгоритм амортизации"));

if(poisk->naimpoi.ravno()[0] != '\0')
 {
  y++;
  attron(A_BLINK);
  mvprintw(y++,x,gettext("Поиск:"));
  mvprintw(y++,x,"%s",poisk->naimpoi.ravno());
  attroff(A_BLINK);
  
 }

if(metkar < 2)
  helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F6",gettext("сорт."),
"F10",gettext("выход"),NULL);

if(metkar == 2)
  helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
  "F10",gettext("выход"),NULL);

}

/********************/
/*Распечатать       */
/********************/

void	diruosgrbu_ras(SQLCURSOR *cur)
{
char	imaf[64];
FILE	*ff;
SQL_str row;

sprintf(imaf,"komt%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }


iceb_t_zagolov(gettext("Список групп бухгалтерского учёта"),0,0,0,0,0,0,ff);


fprintf(ff,"\
-------------------------------------------------------------\n");
fprintf(ff,gettext("\
Код | Наименование группы          |Год.н.а.|Алгоритм аморт.|\n"));

/*

*/

fprintf(ff,"\
-------------------------------------------------------------\n");

cur->poz_cursor(0);
class iceb_tu_str tipam("");

while(cur->read_cursor(&row) != 0)
 {
  if(atoi(row[5]) == 0)
   tipam.new_plus(gettext("От начальной балансовой стоимости"));
  else
   tipam.new_plus(gettext("От остаточной балансовой стоимости"));

  fprintf(ff,"%-*s %-*.*s %8s %-*.*s\n",
  iceb_tu_kolbait(4,row[0]),
  row[0],
  iceb_tu_kolbait(30,row[1]),iceb_tu_kolbait(30,row[1]),row[1],
  row[2],
  iceb_tu_kolbait(15,tipam.ravno()),iceb_tu_kolbait(15,tipam.ravno()),tipam.ravno());
 }
fprintf(ff,"\
-------------------------------------------------------------\n");

podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Список групп бухгалтерского учёта"));
iceb_t_ustpeh(imaf,3);
iceb_t_rabfil(&oth,"");

}
/************************/
/*Удаление неиспользуемых складов*/
/*********************************/
void diruosgrbu_ud_neisp()
{
int kolstr;
char strsql[512];
SQL_str row;
SQLCURSOR cur;

clear();
GDITE();

sprintf(strsql,"select * from %s","Uosgrup1");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
int kolstr1=0;
int kolsklud=0;
while(cur.read_cursor(&row) != 0)
 {
  
  printw("%5s %-40s ",row[0],row[1]);
  strzag(LINES-1,0,kolstr,++kolstr1);
  
  if(diruosgrbu_prov(row[0],0) != 0)
   {
    printw("\n");
    continue;
   }
  sprintf(strsql,"delete from %s where kod='%s'","Uosgrup1",row[0]);
  if(sql_zapis(strsql,1,0) != 0)
   continue;
   
  kolsklud++;  
  printw("%s\n",gettext("Удален"));
  
 }

VVOD		SOOB(1);
sprintf(strsql,"%s %d",gettext("Количество удалённых записей"),kolsklud);
SOOB.VVOD_SPISOK_add_MD(strsql);
soobshw(&SOOB,stdscr,-1,-1,0,1);

}
/********************************/
/*Проверка возможности удаления*/
/********************************/
//Если вернули 0-можно удалять 1-нет
int diruosgrbu_prov(const char *kod,int metka_s) /*0-без выдачи сообщений 1- с выдачей*/
{
char	strsql[512];

sprintf(strsql,"select hnaby from Uosinp where hnaby='%s' limit 1",kod);

if(readkey(strsql) > 0)
 {
  if(metka_s == 1)
    iceb_t_ndelkod("Uosinp",kod);
  return(1);
 }
sprintf(strsql,"select hna from Uosamor1 where hna='%s' limit 1",kod);
if(readkey(strsql) > 0)
 {
  if(metka_s == 1)
    iceb_t_ndelkod("Uosamor1",kod);
  return(1);
 }

return(0);


}
/************************/
/*Корректировка кода*/
/**********************/

void diruosgrbu_korkod(const char *nkod,const char *skod)
{
char strsql[512];
GDITE();
sprintf(strsql,"update Uosinp set hnaby='%s' where hnaby='%s'",nkod,skod);
sql_zapis(strsql,1,0);

sprintf(strsql,"update Uosamor1 set hna='%s' where hna='%s'",nkod,skod);
sql_zapis(strsql,1,0);

}

