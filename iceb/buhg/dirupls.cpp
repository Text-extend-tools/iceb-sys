/*$Id: dirupls.c,v 5.36 2013/09/26 09:43:31 sasa Exp $*/
/*14.07.2015	25.03.2003	Белых А.И.	dirvpls.c
Ввод и корректировка стартового сальдо по топливу
*/
#include        <errno.h>
#include        "buhg.h"

class upls
 {
  public:
  class iceb_tu_str god;
  class iceb_tu_str kodavt;
  class iceb_tu_str kodvod;
  class iceb_tu_str kodmt;
  upls()
   {
    clear();
   }  
  void clear()
   {
    god.new_plus("");
    kodavt.new_plus("");
    kodvod.new_plus("");
    kodmt.new_plus("");
   }
 };

void            vvodupls(upls*);
void            sapupls(upls*,int);
int             zapupls(VVOD*,upls*);
void            rasupls(int,upls*,SQLCURSOR*);
int             menupoi(upls*);
int             uplpoi(int,upls*,SQL_str);
void	dirupols_uszap(int metkapoi,int kolstr,upls *poisk,SQLCURSOR *cur);



void	dirupls()
{
class iceb_t_mstr spis_menu(0);
char		strsql[1024];
SQL_str		row,row1;
int		kolstr=0;
int		K=0;
int		pozz=0,pozz1=0;
int		i=0,kom=0;
int		mspz=0;
int		prc;
int             Y=2;
int             X=0;
int		metkapoi=0; //0-поиска нет 1-есть
class iceb_tu_str naimob("");
SQLCURSOR curr;
class upls zapis;
class upls poisk;

//Создаем класс меню
VVOD DANET(1);
VVOD VVOD1(2);

naz1:;

short           kls=LINES-5;


sprintf(strsql,"select * from Uplsal order by god desc");


SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
if(kolstr == 0)
 {
lll:;
  sapupls(&poisk,metkapoi);
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == SFK2 || K == ENTER || K == FK3 || K == SFK3 || K == FK5)
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

  if(uplpoi(metkapoi,&poisk,row) != 0)
   continue;

  naimob.new_plus("");
  sprintf(strsql,"select naik from Uplouot where kod=%s",row[2]);
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
   naimob.new_plus(row1[0]);
          
  sprintf(strsql,"%s|%-3s|%-3s|%-*.*s|%-*s|%.2f",
  row[0],row[1],row[2],
  iceb_tu_kolbait(19,naimob.ravno()),iceb_tu_kolbait(19,naimob.ravno()),naimob.ravno(),
  iceb_tu_kolbait(10,row[3]),row[3],
  atof(row[4]));

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

sapupls(&poisk,metkapoi);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,Y,X,NULL,strsql,prc,&K,"",0);


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
    if(uplpoi(metkapoi,&poisk,row) != 0)
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
    if(uplpoi(metkapoi,&poisk,row) != 0)
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
    if(uplpoi(metkapoi,&poisk,row) != 0)
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

  polen(spis_menu.spis_str.ravno(kom),&zapis.god,1,'|');
  polen(spis_menu.spis_str.ravno(kom),&zapis.kodavt,2,'|');
  polen(spis_menu.spis_str.ravno(kom),&zapis.kodvod,3,'|');
  polen(spis_menu.spis_str.ravno(kom),&zapis.kodmt,5,'|');

 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return;
     break;

  case FK1:  /*Получить подсказку*/
    GDITE();
    iceb_t_pdoc("upl3_5.txt");
    goto naz;

  case ENTER:
  case SFK2:   /*Корректировать*/
    vvodupls(&zapis);
    goto naz1;

  case FK2:  /*Ввести запись*/
  case PLU:
    zapis.clear();
    vvodupls(&zapis);
    goto naz1;

  case FK3:       /*Удалить запись*/
    if(iceb_t_pbpds(1,atoi(zapis.god.ravno())) != 0)
     goto naz1;
     
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      sprintf(strsql,"delete from Uplsal where god=%s and ka=%s and kv=%s \
and kmt='%s'",zapis.god.ravno(),zapis.kodavt.ravno(),zapis.kodvod.ravno(),zapis.kodmt.ravno());
      if(sql_zapis(strsql,0,0) == 0)
        goto naz1;
     }
    goto naz;

  case SFK3:  //Удалить записи с учётом условий поиска
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить все записи ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      goto naz;
      
    dirupols_uszap(metkapoi,kolstr,&poisk,&cur);
    goto naz1;
  
  case FK4:       /*Поиск*/
     metkapoi=menupoi(&poisk);

     clear();
     kom=pozz=0;
     goto naz;

  case FK5:    //Распечатать
    rasupls(metkapoi,&poisk,&cur);
    goto naz;

  default:
    goto naz;
    break;
 }

return;


}
/**********************/
/*Ввод и корректировки*/
/**********************/

void	vvodupls(upls *zapis)
{
char		strsql[512];
SQL_str		row;
time_t		vremz=0;
int		ktozap=0;
struct  tm      *bf;
int		N=0,K=0;
int             kom1=0;
short		tekgod;
class iceb_tu_str kodn("");
class iceb_tu_str naimn("");
SQLCURSOR curr;
time(&vremz);
bf=localtime(&vremz);
tekgod=bf->tm_year+1900;
vremz=0;

VVOD VV(0);

VV.VVOD_SPISOK_add_data(5); //0
VV.VVOD_SPISOK_add_data(10);//1
VV.VVOD_SPISOK_add_data(10);//2
VV.VVOD_SPISOK_add_data(20);//3
VV.VVOD_SPISOK_add_data(10);//4

if(zapis->god.getdlinna() > 1)
 {

  VV.VVOD_SPISOK_zapis_data(0,zapis->god.ravno());
  VV.VVOD_SPISOK_zapis_data(1,zapis->kodavt.ravno());
  VV.VVOD_SPISOK_zapis_data(2,zapis->kodvod.ravno());
  VV.VVOD_SPISOK_zapis_data(3,zapis->kodmt.ravno());

  sprintf(strsql,"select sal,ktoz,vrem from Uplsal where god=%s \
and ka=%s and kv=%s and kmt='%s'",
  zapis->god.ravno(),zapis->kodavt.ravno(),zapis->kodvod.ravno(),zapis->kodmt.ravno());
  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
    iceb_t_soob(gettext("Не найдена запись для корректировки !"));
    return;
    
   }
  VV.VVOD_SPISOK_zapis_data(4,row[0]);

  ktozap=atoi(row[1]);
  vremz=atol(row[2]);


 }

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

VV.VVOD_SPISOK_add_MD(gettext("Год............."));//0
VV.VVOD_SPISOK_add_MD(gettext("Код автомобиля.."));//1
VV.VVOD_SPISOK_add_MD(gettext("Код водителя...."));//2
VV.VVOD_SPISOK_add_MD(gettext("Код топлива....."));//3
VV.VVOD_SPISOK_add_MD(gettext("Сальдо.........."));//4

naz:;

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("реквизиты"),
"F10",gettext("выход"),NULL);


VV.vramka(2,0,0);

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

//    mygetstr(VV.VVOD_SPISOK_return_data(N),VV.VVOD_SPISOK_return_dls(N),stdscr);
    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 0 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')  //Год
     {
      int god=atoi(VV.VVOD_SPISOK_return_data(N));
      if(god < tekgod-100 || god > tekgod+100)
       {
        iceb_t_soob(gettext("Не верно введён год !"));
        goto naz;;
       } 
     }

    if(N == 1 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')  //Код автомобиля
     {
      //Проверяем код автомобиля
      sprintf(strsql,"select kod from Uplavt where kod=%s",VV.VVOD_SPISOK_return_data(N));
      if(sql_readkey(&bd,strsql,&row,&curr) != 1)
       {
        VVOD SOOB(1);
        sprintf(strsql,"%s %s !",gettext("Не найден код автомобиля"),
        VV.VVOD_SPISOK_return_data(N));
        SOOB.VVOD_SPISOK_add_MD(strsql);
        soobshw(&SOOB,stdscr,-1,-1,0,1);
        goto naz;;
       }
     }

    if(N == 2 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')  //Код водителя
     {
      //Проверяем код водителя
      sprintf(strsql,"select kod from Uplouot where kod=%s",VV.VVOD_SPISOK_return_data(N));
      if(sql_readkey(&bd,strsql,&row,&curr) != 1)
       {
        VVOD SOOB(1);
        sprintf(strsql,"%s %s !",gettext("Не найден код водителя"),
        VV.VVOD_SPISOK_return_data(N));
        SOOB.VVOD_SPISOK_add_MD(strsql);
        soobshw(&SOOB,stdscr,-1,-1,0,1);
        goto naz;;
       }
     }

    if(N == 3 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')  //Код топлива
     {
      //Проверяем код водителя
      sprintf(strsql,"select kodt from Uplmt where kodt='%s'",VV.VVOD_SPISOK_return_data(N));
      if(sql_readkey(&bd,strsql,&row,&curr) != 1)
       {
        VVOD SOOB(1);
        sprintf(strsql,"%s %s !",gettext("Не найден код топлива"),
        VV.VVOD_SPISOK_return_data(N));
        SOOB.VVOD_SPISOK_add_MD(strsql);
        soobshw(&SOOB,stdscr,-1,-1,0,1);
        goto naz;;
       }
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
    iceb_t_pdoc("upl3_5_1.txt");
    clear();
    goto naz;
   }

  if(K == FK2 || K == PLU) /*Записываем*/
   {
    if(zapupls(&VV,zapis) != 0)
     goto naz;
    break;
   }

  if(K == FK3) //Реквизиты
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    VVOD MENU(3);
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка водителей"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка автомобилей"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка марок топлива"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//3

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 3 :
      case -1:
        break;

      case 0 :
        naimn.new_plus("");
        if(dirout(&kodn,&naimn,1,2) == 0)
         VV.VVOD_SPISOK_zapis_data(2,kodn.ravno());

        break;

      case 1:
        naimn.new_plus("");
        if(diravt0(&kodn,&naimn,1) == 0)
         VV.VVOD_SPISOK_zapis_data(1,kodn.ravno());
        break;

      case 2 :
        kodn.new_plus("");
        if(dirmt(&kodn,1) == 0)
         VV.VVOD_SPISOK_zapis_data(3,kodn.ravno());
        break;

     }
    goto naz;
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

int   zapupls(VVOD *VV,upls *zapis)
{
char	strsql[1024];
SQL_str row;
SQLCURSOR curr;

attroff(VV->VVOD_return_cs(iceb_CFS));
attron(VV->VVOD_return_cs(iceb_CFM));

if(VV->VVOD_SPISOK_return_data(0)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(1)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(2)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(3)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(4)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
  return(1);
 }       
time_t vrem;
time(&vrem);
struct  tm      *bf;
bf=localtime(&vrem);


int god=atoi(VV->VVOD_SPISOK_return_data(0));
if(god < bf->tm_year+1900-100 || god > bf->tm_year+1900+100)
 {
  iceb_t_soob(gettext("Не верно введён год !"));
  return(1);
 } 

if(iceb_t_pbpds(1,god) != 0)
 return(1);
 
//Проверяем есть ли такой код водителя

sprintf(strsql,"select kod from Uplouot where kod=%s and mt=1",VV->VVOD_SPISOK_return_data(2));
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {

  sprintf(strsql,"%s %s !",gettext("Не найден код водителя"),VV->VVOD_SPISOK_return_data(2));
  iceb_t_soob(strsql);
  return(1);
 }

//Проверяем код автомобиля

sprintf(strsql,"select kod from Uplavt where kod=%s ",VV->VVOD_SPISOK_return_data(1));
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {

  sprintf(strsql,"%s %s !",gettext("Не найден код автомобиля"),VV->VVOD_SPISOK_return_data(1));
  iceb_t_soob(strsql);
  return(1);
 }

//Проверяем код топлива

sprintf(strsql,"select kodt from Uplmt where kodt='%s'",VV->VVOD_SPISOK_return_data(3));
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {

  sprintf(strsql,"%s %s !",gettext("Не найден код топлива"),VV->VVOD_SPISOK_return_data(1));
  iceb_t_soob(strsql);
  return(1);
 }

if(zapis->god.ravno_atoi() != atoi(VV->VVOD_SPISOK_return_data(0)) || \
   zapis->kodavt.ravno_atoi() != atoi(VV->VVOD_SPISOK_return_data(1)) || \
   zapis->kodvod.ravno_atoi() != atoi(VV->VVOD_SPISOK_return_data(2)) || \
   SRAV(zapis->kodmt.ravno(),VV->VVOD_SPISOK_return_data(3),0) != 0)
 {
  sprintf(strsql,"select god from Uplsal where god=%s and ka=%s and kv=%s and kmt='%s'",
  VV->VVOD_SPISOK_return_data(0),
  VV->VVOD_SPISOK_return_data(1),
  VV->VVOD_SPISOK_return_data(2),
  VV->VVOD_SPISOK_return_data(3));

  if(sql_readkey(&bd,strsql,&row,&curr) > 0)
   {
    iceb_t_soob(gettext("Такая запись уже есть !"));
    return(1);
   }

  if(zapis->god.getdlinna() > 1)
   {

    sprintf(strsql,"delete from Uplsal where god=%s and ka=%s and kv=%s \
  and kmt='%s'",zapis->god.ravno(),zapis->kodavt.ravno(),zapis->kodvod.ravno(),zapis->kodmt.ravno());
    if(sql_zapis(strsql,0,0) != 0)
     return(1);  
   }
 }
 

if(zapis->god.getdlinna() <= 1) //Ввод новой записи
  sprintf(strsql,"insert into Uplsal values(%s,%s,%s,'%s',%.2f,%d,%ld)",
  VV->VVOD_SPISOK_return_data(0),  
  VV->VVOD_SPISOK_return_data(1),  
  VV->VVOD_SPISOK_return_data(2),  
  VV->VVOD_SPISOK_return_data(3),  
  ATOFM(VV->VVOD_SPISOK_return_data(4)),
  iceb_t_getuid(),vrem);
else
  sprintf(strsql,"replace into Uplsal values(%s,%s,%s,'%s',%.2f,%d,%ld)",
  VV->VVOD_SPISOK_return_data(0),  
  VV->VVOD_SPISOK_return_data(1),  
  VV->VVOD_SPISOK_return_data(2),  
  VV->VVOD_SPISOK_return_data(3),  
  ATOFM(VV->VVOD_SPISOK_return_data(4)),
  iceb_t_getuid(),vrem);

printw("\n%s\n",strsql);
refresh();

if(sql_zapis(strsql,0,0) != 0)
 return(1);

return(0);

}


/**************************/
/*Шапка                   */
/**************************/

void	sapupls(upls *poisk,int metkapoi)
{
short		y=2;
short           x=COLS-20;



clear();
printw(gettext("Ввод и корректировка сальдо по топливу"));

mvprintw(1,0,"    1   2   3            4              5      6");
mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,gettext("1-год"));
mvprintw(y++,x,gettext("2-код автомобиля"));
mvprintw(y++,x,gettext("3-код водителя"));
mvprintw(y++,x,gettext("4-фамилия"));
mvprintw(y++,x,gettext("5-код топлива"));
mvprintw(y++,x,gettext("6-сальдо"));

if(metkapoi != 0)
 {
  y++;
  attron(A_BLINK);
  mvprintw(y++,x,gettext("Поиск:"));
  mvprintw(y++,x,"%s",poisk->god.ravno());
  mvprintw(y++,x,"%s",poisk->kodavt.ravno());
  mvprintw(y++,x,"%s",poisk->kodvod.ravno());
  mvprintw(y++,x,"%s",poisk->kodmt.ravno());
  attroff(A_BLINK);
  
 }

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F10",gettext("выход"),NULL);

}

/********************/
/*Распечатать       */
/********************/

void	rasupls(int metkapoi,upls *poisk,SQLCURSOR *cur)
{
char	imaf[64];
FILE	*ff;
SQL_str row,row1;
class iceb_tu_str vodit("");
char	strsql[512];
SQLCURSOR curr;
sprintf(imaf,"komt%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
   {
    error_op_nfil(imaf,errno,"");
    return;
   }

iceb_t_zagolov(gettext("Распечатка сальдо по топливу"),0,0,0,0,0,0,ff);


fprintf(ff,"\
------------------------------------------------------------\n");
fprintf(ff,gettext("\
Год |Автом.|      Водитель      | Топливо  |Сальдо\n"));
fprintf(ff,"\
------------------------------------------------------------\n");

double suma=0.;

cur->poz_cursor(0);

while(cur->read_cursor(&row) != 0)
 {
  if(uplpoi(metkapoi,poisk,row) != 0)
   continue;

  vodit.new_plus(row[2]);
  sprintf(strsql,"select naik from Uplouot where kod=%s",row[2]);
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
   {
    vodit.plus(" ",row1[0]);
   }        
  fprintf(ff,"%-4s %-*s %-*.*s %-*s %s\n",
  row[0],
  iceb_tu_kolbait(6,row[1]),row[1],
  iceb_tu_kolbait(20,vodit.ravno()),iceb_tu_kolbait(20,vodit.ravno()),vodit.ravno(),
  iceb_tu_kolbait(10,row[3]),row[3],
  row[4]);
  suma+=atof(row[4]);
 }
fprintf(ff,"\
------------------------------------------------------------\n");
fprintf(ff,"%*s:%.10g\n",iceb_tu_kolbait(43,gettext("Итого")),gettext("Итого"),suma);

podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка сальдо по топливу"));

iceb_t_ustpeh(oth.spis_imaf.ravno(0),0);

iceb_t_rabfil(&oth,"");

}

/******************************/
/*Меню поиска                 */
/******************************/
//Если вернули 0-поиск не выполнять 1-выполнять
int   menupoi(upls *poisk)
{
int	kom=0;
int	kom1=0;
class iceb_tu_str kodn("");
class iceb_tu_str naimn("");
VVOD MENU(3);

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Введите данные для поиска"));

VV.VVOD_SPISOK_add_MD(gettext("Год............."));//0
VV.VVOD_SPISOK_add_MD(gettext("Код автомобиля.."));//1
VV.VVOD_SPISOK_add_MD(gettext("Код водителя...."));//2
VV.VVOD_SPISOK_add_MD(gettext("Код топлива....."));//3

VV.VVOD_SPISOK_add_data(poisk->god.ravno(),5);
VV.VVOD_SPISOK_add_data(poisk->kodavt.ravno(),32);
VV.VVOD_SPISOK_add_data(poisk->kodvod.ravno(),32);
VV.VVOD_SPISOK_add_data(poisk->kodmt.ravno(),32);

naz:;

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("поиск"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

move(LINES-1,0);

poisk->god.new_plus(VV.VVOD_SPISOK_return_data(0));
poisk->kodavt.new_plus(VV.VVOD_SPISOK_return_data(1));
poisk->kodvod.new_plus(VV.VVOD_SPISOK_return_data(2));
poisk->kodmt.new_plus(VV.VVOD_SPISOK_return_data(3));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("upl3_5_2.txt");
   goto naz;

  case ESC:
  case FK10:
   return(0);

  case FK2:
  case PLU:
     break;

  case FK3:  //Реквизиты
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка водителей"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка автомобилей"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка марок топлива"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//3

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 3 :
      case -1:
        break;

      case 0 :
        naimn.new_plus("");
        if(dirout(&kodn,&naimn,1,2) == 0)
         VV.VVOD_SPISOK_zapis_data(2,kodn.ravno());

        break;

      case 1:
        naimn.new_plus("");
        if(diravt0(&kodn,&naimn,1) == 0)
         VV.VVOD_SPISOK_zapis_data(1,kodn.ravno());
        break;

      case 2 :
        kodn.new_plus("");
        if(dirmt(&kodn,1) == 0)
         VV.VVOD_SPISOK_zapis_data(3,kodn.ravno());
        break;

     }
    goto naz;


  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

if(poisk->god.getdlinna() > 1 || poisk->kodavt.getdlinna() > 1 || poisk->kodvod.getdlinna() > 1 || \
poisk->kodmt.getdlinna() > 1)
  return(1);
else
  return(0);
}

/*******************************/
/*Проверка на условия поиска*/
/*****************************/
//Если вернули 0-подходит 1-нет

int	uplpoi(int metkapoi,upls *poisk,SQL_str row)
{
if(metkapoi == 0)
 return(0);
 
if(proverka(poisk->god.ravno(),row[0],0,0) != 0)
  return(1);

if(proverka(poisk->kodavt.ravno(),row[1],0,0) != 0)
  return(1);

if(proverka(poisk->kodvod.ravno(),row[2],0,0) != 0)
  return(1);

if(proverka(poisk->kodmt.ravno(),row[3],0,0) != 0)
  return(1);

return(0);

}
/**************************************/
/*Удаление записей с учётом условий поиска*/
/******************************************/

void	dirupols_uszap(int metkapoi,int kolstr,upls *poisk,SQLCURSOR *cur)
{
SQL_str row;
char strsql[512];

cur->poz_cursor(0);
int kolstr1=0;

while(cur->read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  
  if(uplpoi(metkapoi,poisk,row) != 0)
   continue;

  sprintf(strsql,"delete from Uplsal where god=%s and ka=%s and kv=%s and kmt='%s'",
  row[0],row[1],row[2],row[3]);

  sql_zapis(strsql,1,0);
  
 }



}
