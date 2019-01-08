/*$Id: dirmt.c,v 5.30 2013/09/26 09:43:54 sasa Exp $*/
/*27.03.2013	21.03.2003	Белых А.И.	dirmt.c
Работа со списком марок топлива
Если вернули 0-выбрали
             1-нет
*/
#include        <errno.h>
#include        "buhl.h"

void		sapkamt(int);
void            sozmt(VVOD*);
void            vvodmart(int,VVOD*,int,time_t);
int             zapismt(VVOD*,const char*);
int             provudmt(const char*);
void            korkodt(const char*,const char*);
void            rasmt(SQLCURSOR*);


int dirmt(class iceb_tu_str *martp,int metkar) //0-работа 1-выбор из списка с корректировкой 2-чистый выбор
{
class iceb_t_mstr spis_menu(0);
char		strsql[1024];
SQL_str		row;
SQL_str		row1;
SQLCURSOR cur;
SQLCURSOR cur1;
int		kolstr=0;
int		K=0;
int		pozz=0,pozz1=0;
int		i=0,kom=0;
int		mspz=0;
int		prc;
int             Y=3;
int             X=0;
class iceb_tu_str koment("");
class iceb_tu_str kodmat("");
class iceb_tu_str naim("");

if(metkar == 2)
 {
  Y=-1;
  X=-1;
  koment.new_plus(gettext("Выберите нужное и нажмите Enter"));
 }

//Создаем класс меню
VVOD VV(0);
VVOD DANET(1);
VVOD VVOD1(2);

naz1:;

short           kls=LINES-6;
int		ktozap[kls];
time_t		vremzap[kls];



sprintf(strsql,"select * from Uplmt");

//printw("\nstrsql=%s\n",strsql);
//OSTANOV();

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
if(kolstr == 0)
 {
lll:;
  sapkamt(metkar);
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == SFK2 || K == ENTER || K == FK3 || K == FK5)
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
  
  ktozap[i]=atoi(row[2]);
  vremzap[i]=atol(row[3]);
  naim.new_plus("");
  //Читаем наименование
  sprintf(strsql,"select naimat from Material where kodm=%s",row[1]);
  if(readkey(strsql,&row1,&cur1) == 1)
   naim.new_plus(row1[0]);

  sprintf(strsql,"%-*s|%-*s|%.*s",
  iceb_tu_kolbait(10,row[0]),row[0],
  iceb_tu_kolbait(10,row[1]),row[1],
  iceb_tu_kolbait(30,naim.ravno()),naim.ravno());

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


sapkamt(metkar);

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
/***********
    if(metpoi == 1)
    if(poisaldo(row,shetpoi,kontrpoi,godpoi) != 0)
      continue;
**************/
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
/***********
    if(metpoi == 1)
    if(poisaldo(row,shetpoi,kontrpoi,godpoi) != 0)
      continue;
*********/
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
/*********
    if(metpoi == 1)
    if(poisaldo(row,shetpoi,kontrpoi,godpoi) != 0)
      continue;
*********/
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

  polen(spis_menu.spis_str.ravno(kom),martp,1,'|');
  polen(spis_menu.spis_str.ravno(kom),&kodmat,2,'|');

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
    iceb_t_pdoc("upl3_2.txt");
    goto naz;

  case ENTER:
    if(metkar != 0)
      return(0);
      
  case SFK2:   /*Корректировать*/
    VV.VVOD_delete();
    sozmt(&VV);
    VV.data_plus(0,martp->ravno());
    VV.data_plus(1,kodmat.ravno());
    vvodmart(1,&VV,ktozap[kom],vremzap[kom]);
    goto naz1;

  case FK2:  /*Ввести запись*/
  case PLU:
    VV.VVOD_delete();
    sozmt(&VV);
    vvodmart(0,&VV,0,0);
    goto naz1;

  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      if(provudmt(martp->ravno()) != 0)
        goto naz;
      sprintf(strsql,"delete from Uplmt where kodt='%s'",martp->ravno());
      if(sql_zapis(strsql,0,0) == 0)
        goto naz1;
     }
    goto naz;

  case FK5:    //Распечатать
    rasmt(&cur);
    goto naz;

  default:
    goto naz;
    break;
 }

return(1);
}
/**************************/
/*Ввод и корректировка    */
/**************************/

void	vvodmart(int metka, //0-ввод новой записи 1-корретировка
VVOD *VV,
int ktozap,time_t vremz)
{
char		strsql[512];
struct  tm      *bf;
int		kom=0;
//SQL_str		row;
class iceb_tu_str kodmt("");

clearstr(LINES-1,0);

kodmt.new_plus(VV->VVOD_SPISOK_return_data(0));

if(ktozap != 0)
 {
  sprintf(strsql,"%s %s",gettext("Записал"),iceb_t_kszap(ktozap));
  VV->VVOD_SPISOK_add_ZAG(strsql);
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
  VV->VVOD_SPISOK_add_ZAG(strsql);
 }

VV->VVOD_SPISOK_add_MD(gettext("Марка топлива............................"));
VV->VVOD_SPISOK_add_MD(gettext("Код материала............................"));

naz:;

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F10",gettext("выход"),NULL);

kom=VV->vvod(0,1,1,-1,-1);

switch(kom)
 {
  case FK10:
  case ESC:
    break; 

  case FK1:
    GDITE();
    iceb_t_pdoc("upl3_2_1.txt");
    clear();
    goto naz;

  case FK2:
  case PLU:
    if(zapismt(VV,kodmt.ravno()) != 0)
     goto naz;


    break;

  default:
   goto naz;
 }
 
}
/****************/
/*Запись строки*/
/***************/
//Если вернули 0 запись сделана 1-нет
int	zapismt(VVOD *VV,
const char *kodt) //Старый код топлива
{
char		strsql[512];
//SQL_str		row;

if(VV->VVOD_SPISOK_return_data(0)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введена марка топлива !"));
  return(1);
 }

if(VV->VVOD_SPISOK_return_data(1)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введён код материала !"));
  return(1);
 }

if(VV->VVOD_SPISOK_return_data(1)[0] != '\0')
 {
  //проверяем наличие кода в списке материалов
  sprintf(strsql,"select kodm from Material where kodm=%s",VV->VVOD_SPISOK_return_data(1));
  if(sql_readkey(&bd,strsql) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код материала"),VV->VVOD_SPISOK_return_data(1));
    iceb_t_soob(strsql);
    return(1);
   }
 }

if(SRAV(kodt,VV->VVOD_SPISOK_return_data(0),0) != 0)
 {
  sprintf(strsql,"select kodt from Uplmt where kodt='%s'",VV->VVOD_SPISOK_return_data(0));
  if(sql_readkey(&bd,strsql) > 0)
   {
    iceb_t_soob(gettext("Такая запись уже есть !"));
    return(1);
   }
  if(kodt[0] != '\0')
   {
    VVOD DANET(1);
    DANET.VVOD_SPISOK_add_MD(gettext("Корректировать код ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      return(1);
   }

 }

time_t vrem;
time(&vrem);

if(kodt[0] == '\0') //Ввод новой записи
 {
  sprintf(strsql,"insert into Uplmt \
values ('%s',%s,%d,%ld)",VV->VVOD_SPISOK_return_data(0),VV->VVOD_SPISOK_return_data(1),iceb_t_getuid(),vrem);
 }
else //Корректировка старой записи
 {
  sprintf(strsql,"replace into Uplmt \
values ('%s',%s,%d,%ld)",VV->VVOD_SPISOK_return_data(0),VV->VVOD_SPISOK_return_data(1),iceb_t_getuid(),vrem);
 }

if(sql_zapis(strsql,0,0) != 0)
 return(1);

if(kodt[0] != '\0' && SRAV(kodt,VV->VVOD_SPISOK_return_data(0),0) != 0)
 korkodt(kodt,VV->VVOD_SPISOK_return_data(0)); //Корректирка марки топлива в таблицах
 
return(0);

}

/***************************/
/* Шапка меню              */
/***************************/

void		sapkamt(int metkar)
{
short		y=2;
short           x=COLS-20;

if(metkar != 2)
 {
  clear();
  printw(gettext("Ввод и корректировка марок топлива"));
 }
if(metkar == 1)
 {
  move(1,0);
  printw(gettext("Найдите нужное и нажмите \"Enter\""));
 }

mvprintw(2,0,"       1        2         3");
mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,"1-%s",gettext("марка топлива"));
mvprintw(y++,x,"2-%s",gettext("код материала"));
mvprintw(y++,x,"3-%s",gettext("наименование "));

if(metkar < 2)
  helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
//"F4",gettext("поиск"),
"F5",gettext("печать"),
"F10",gettext("выход"),NULL);

if(metkar == 2)
  helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
  "F10",gettext("выход"),NULL);

}






/**************************/
/*Создание списка для меню*/
/**************************/
void	sozmt(VVOD *VV)
{
VV->VVOD_SPISOK_add_data(20);
VV->VVOD_SPISOK_add_data(10);

}
/*******************************/
/*Проверка возможности удаления записи*/
/***************************************/
//Если вернули 0-можно удалять
//             1-нет
int provudmt(const char *kodtp)
{
char	strsql[1024];

sprintf(strsql,"select kodtp from Upldok1 where kodtp='%s' limit 1",kodtp);
if(sql_readkey(&bd,strsql) > 0)
 {
  iceb_t_soob(gettext("Запись удалить нельзя потому, что есть документы с этим топливом !"));

  return(1);
 }

sprintf(strsql,"select kmt from Uplsal where kmt='%s' limit 1",kodtp);
if(sql_readkey(&bd,strsql) > 0)
 {
  iceb_t_soob(gettext("Запись удалить нельзя потому, что есть сальдо с этим топливом !"));

  return(1);
 }

sprintf(strsql,"select kt from Upldok2a where kt='%s' limit 1",kodtp);
if(sql_readkey(&bd,strsql) > 0)
 {
  sprintf(strsql,"%s Upldok2a\n",gettext("Удалить нельзя! Есть запись в таблице"));
  iceb_t_soob(strsql);

  return(1);
 }
sprintf(strsql,"select kodt from Uplmtsc where kodt='%s' limit 1",kodtp);
if(sql_readkey(&bd,strsql) > 0)
 {
  sprintf(strsql,"%s Uplmtsc\n",gettext("Удалить нельзя! Есть запись в таблице"));
  iceb_t_soob(strsql);

  return(1);
 }

return(0);
}
/*******************************/
/*Каскадная корректировка марки доплива в таблицах*/
/**************************************************/

void  korkodt(const char *kodtps,const char *kodtpn)
{
char		strsql[512];

VVOD SOOBN(1);
SOOBN.VVOD_SPISOK_add_MD(gettext("Выполняется каскадная корректировка кода в таблицах !"));
soobsh(&SOOBN,-1,-1,1);

GDITE();

sprintf(strsql,"update Upldok1 set kodtp='%s' where kodtp='%s'",kodtpn,kodtps);
if(sql_zap(&bd,strsql) != 0)
 {
  msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
  return;
 }
sprintf(strsql,"update Uplsal set kmt='%s' where kmt='%s'",kodtpn,kodtps);
if(sql_zap(&bd,strsql) != 0)
 {
  msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
  return;
 }
//Удаляем запись которую откорректировали
sprintf(strsql,"delete from Uplmt where kodt='%s'",kodtps);
sql_zapis(strsql,1,0);

}
/***************************/
/*Распечатка списка материалов*/
/*******************************/

void	rasmt(SQLCURSOR *cur)
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
startfil(ff);


iceb_t_zagolov(gettext("Распечатка списка марок топлива"),0,0,0,0,0,0,ff);


cur->poz_cursor(0);

while(cur->read_cursor(&row) != 0)
 {

  fprintf(ff,"%-*s %-*s\n",
  iceb_tu_kolbait(10,row[0]),row[0],
  iceb_tu_kolbait(10,row[1]),row[1]);
 }

podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка списка марок топлива"));
iceb_t_rabfil(&oth,"");

}
