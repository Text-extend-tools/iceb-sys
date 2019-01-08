/*$Id: vdkr.c,v 5.33 2013/09/26 09:43:43 sasa Exp $*/
/*24.06.2013	24.10.2002	Белых А.И.	vdkr.c
Ввод шапки документа для командировочных расходов
Если вернули 0 - записали шапку
             1- нет
*/
#include	"buhg.h"

int	vdkrzap(short,short,short,const char*,const char*,VVOD*);


int	vdkr(short *dd,short *md,short *gd, //Дата документа
class iceb_tu_str *nomdok) //Номер документа
{
struct  tm      *bf;
class iceb_tu_str naimo("");
int             N=0,K=0;
class iceb_tu_str kod("");
class iceb_tu_str naik("");
class iceb_tu_str naimkod("");
int		kom1=0;
char		strsql[1024];
SQL_str		row;
class iceb_tu_str naimkom("");
short		d,m,g;
class iceb_tu_str kontrz("");
class iceb_tu_str vkom("");
time_t		vrem=0;
class iceb_tu_str ktov("");
int		poz=0,komv=0;
static class iceb_tu_str datdok("");
short           metkanovdok=0;
class iceb_tu_str kodn("");
class iceb_tu_str naimn("");
SQLCURSOR curr;

VVOD MENU(3);
VVOD VVOD1(2);
VVOD VV(0);


VV.VVOD_SPISOK_add_MD(gettext("Код контрагента...."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата документа....."));//1
VV.VVOD_SPISOK_add_MD(gettext("Номер документа...."));//2
VV.VVOD_SPISOK_add_MD(gettext("Вид командировки..."));//3
VV.VVOD_SPISOK_add_MD(gettext("Пунк назначения...."));//4
VV.VVOD_SPISOK_add_MD(gettext("Организация........"));//5
VV.VVOD_SPISOK_add_MD(gettext("Цель командировки.."));//6
VV.VVOD_SPISOK_add_MD(gettext("Номер приказа......"));//7
VV.VVOD_SPISOK_add_MD(gettext("Дата приказа......."));//8
VV.VVOD_SPISOK_add_MD(gettext("Дата отъезда......."));//9
VV.VVOD_SPISOK_add_MD(gettext("Дата приезда......."));//10
VV.VVOD_SPISOK_add_MD(gettext("Номер аванс.отчёта."));//11
VV.VVOD_SPISOK_add_MD(gettext("Дата аванс.отчёта.."));//12

VV.VVOD_SPISOK_add_data(10);//0
VV.VVOD_SPISOK_add_data(11);//1
VV.VVOD_SPISOK_add_data(10);//2
VV.VVOD_SPISOK_add_data(80);//3
VV.VVOD_SPISOK_add_data(80);//4
VV.VVOD_SPISOK_add_data(80);//5
VV.VVOD_SPISOK_add_data(80);//6
VV.VVOD_SPISOK_add_data(20);//7
VV.VVOD_SPISOK_add_data(11);//8
VV.VVOD_SPISOK_add_data(11);//9
VV.VVOD_SPISOK_add_data(11);//10
VV.VVOD_SPISOK_add_data(10);//11
VV.VVOD_SPISOK_add_data(11);//12

if(nomdok->ravno()[0] != '\0') //Читаем шапку документа
 {
  metkanovdok=1;
  sprintf(strsql,"select kont,pnaz,organ,celk,datn,datk,nompr,datp,\
vkom,nomao,datao,ktoz,vrem from Ukrdok where god=%d and nomd='%s'",
  *gd,nomdok->ravno());
  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("Не найдена шапка документа !"));
    sprintf(strsql,"%d.%d.%d %s",*dd,*md,*gd,nomdok->ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    return(1);
   }  
  VV.VVOD_SPISOK_zapis_data(0,row[0]);
  kontrz.new_plus(row[0]);
  sprintf(strsql,"%d.%d.%d",*dd,*md,*gd);
  VV.VVOD_SPISOK_zapis_data(1,strsql);
  VV.VVOD_SPISOK_zapis_data(2,nomdok->ravno());
  VV.VVOD_SPISOK_zapis_data(3,row[8]);
  VV.VVOD_SPISOK_zapis_data(4,row[1]);
  VV.VVOD_SPISOK_zapis_data(5,row[2]);
  VV.VVOD_SPISOK_zapis_data(6,row[3]);
  VV.VVOD_SPISOK_zapis_data(7,row[6]);

  rsdat(&d,&m,&g,row[7],2);
  if(d != 0)
   {
    sprintf(strsql,"%d.%d.%d",d,m,g);
    VV.VVOD_SPISOK_zapis_data(8,strsql);
   }
  rsdat(&d,&m,&g,row[4],2);
  if(d != 0)
   {
    sprintf(strsql,"%d.%d.%d",d,m,g);
    VV.VVOD_SPISOK_zapis_data(9,strsql);
   }

  rsdat(&d,&m,&g,row[5],2);
  if(d != 0)
   {
    sprintf(strsql,"%d.%d.%d",d,m,g);
    VV.VVOD_SPISOK_zapis_data(10,strsql);
   }

  vkom.new_plus(row[8]);
  VV.VVOD_SPISOK_zapis_data(11,row[9]);

  rsdat(&d,&m,&g,row[10],2);
  if(d != 0)
   {
    sprintf(strsql,"%d.%d.%d",d,m,g);
    VV.VVOD_SPISOK_zapis_data(12,strsql);
   }

  ktov.new_plus(row[11]);
  vrem=atol(row[12]);    

  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
  VV.VVOD_SPISOK_return_data(0));
  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
    VVOD SOOB(1);
    sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),
    VV.VVOD_SPISOK_return_data(0));
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
   }
  else
    naimo.new_plus(row[0]);

  sprintf(strsql,"select naik from Ukrvkr where kod='%s'",vkom.ravno());
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    naimkom.new_plus(row[0]);
  sprintf(strsql,"%s|%s",vkom.ravno(),naimkom.ravno());
  VV.VVOD_SPISOK_zapis_data(3,strsql);
 }
else
  VV.VVOD_SPISOK_zapis_data(1,datdok.ravno());
 
nazad:;

clear();
printw(gettext("Работа с документами."));
if(ktov.ravno_atoi() != 0)
 {
  mvprintw(1,0,"%s %s",gettext("Записал     :"),iceb_t_kszap(ktov.ravno()));
 }
if(vrem != 0)
 {
  bf=localtime(&vrem);
  mvprintw(2,0,"%s %d.%d.%d%s %s: %d:%d:%d",
  gettext("Дата записи :"),
  bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
  gettext("г."),
  gettext("Время"),
  bf->tm_hour,bf->tm_min,bf->tm_sec);
 }

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("реквизиты"),
"F4",gettext("номер докум."),
"F10",gettext("выход"),NULL);

VV.vramka(4,0,0);

attron(VV.VVOD_return_cs(iceb_CFS)); 

VV.vprintw(N,stdscr);

for(;;)
 {

  if(naimo.ravno()[0] != '\0')
   {
    move(3,0);
    printw("%s%-.*s",gettext("Контрагент:"),iceb_tu_kolbait(40,naimo.ravno()),naimo.ravno());
   }
  refresh();

  K=getch();

  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));

  VV.vprintw(N,stdscr);

  if(K == KEY_RESIZE)
    goto nazad;

  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);
    
  if(K == TAB && N == 0)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));

    naimkod.new_plus("");
    kod.new_plus("");

    helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
    
    VVOD1.VVOD_delete();
    VVOD1.VVOD_SPISOK_add_MD(gettext("Введите наименование контрагента для поиска"));
    if((kom1=vvod1(&naimkod,112,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
      goto nazad;
    if(kom1 == ESC)
      goto nazad;

    clear();
    if(dirkontr(&kodn,&naimkod,1) == 0)
     {
      naimo.new_plus(naimn.ravno());
      VV.VVOD_SPISOK_zapis_data(0,kodn.ravno());
      goto nazad;
     }
    goto nazad;
   }

  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));
    VV.vprintw_clear(N,stdscr);

    if(N == 3) //Вид командировки
     {     
      attroff(VV.VVOD_return_cs(iceb_CFV));
      naik.new_plus("");
      if(vvodkod("Ukrvkr",&kod,&naik,-1,-1,0) == 0)
        goto nazad;
      sprintf(strsql,"%s|%s",kod.ravno(),naik.ravno());
      VV.data_plus(N,strsql);
      N++;
      goto nazad;
     }

//    mygetstr(VV.VVOD_SPISOK_return_data(N),dlinastr+1,stdscr);
    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 0)  /*Код организации*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFM));


      if(VV.VVOD_SPISOK_return_data(N)[0] != '\0')
       {
        sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
        VV.VVOD_SPISOK_return_data(N));
        if(sql_readkey(&bd,strsql,&row,&curr) != 1)
         {
          sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),VV.VVOD_SPISOK_return_data(N));
          iceb_t_soob(strsql);

          naimo.new_plus("");
          VV.VVOD_SPISOK_zapis_data(N,"");

          goto nazad;
         }

        naimo.new_plus(row[0]);
        
       }
     }

    if(N == 1 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')  //Дата документа
     {
      if(rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(N),0) != 0)
       {
        iceb_t_soob(gettext("Не верно введена дата документа !"));
	goto nazad;
       }
      if(iceb_t_pbpds(m,g) != 0)
       goto nazad;
      
       /*Автоматическая установка номера накладной*/

      if(nomdok->ravno()[0] == '\0' && VV.VVOD_SPISOK_return_data(2)[0] == '\0' && VV.VVOD_SPISOK_return_data(1)[0] != '\0')
       {
        iceb_t_nomnak(g,"",strsql,0,3,0);
        VV.VVOD_SPISOK_zapis_data(2,strsql);
       }
      N++;
      goto nazad;
     }

    if(N == 2 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')  //номер документа
     if(SRAV(nomdok->ravno(),VV.VVOD_SPISOK_return_data(N),0) != 0)
      {
       rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(1),1);      
       //Проверяем номер документа
       sprintf(strsql,"select god from Ukrdok where god=%d and nomd='%s'",
       g,VV.VVOD_SPISOK_return_data(N));
       if(sql_readkey(&bd,strsql,&row,&curr) != 0)
        {
         attroff(VV.VVOD_return_cs(iceb_CFM));

         sprintf(strsql,gettext("С номером %s документ уже есть !"),VV.VVOD_SPISOK_return_data(N));

         iceb_t_soob(strsql);
         goto nazad;
        }
      }

    if(N == 8 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')  //Дата документа
     {
      if(rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(N),0) != 0)
       {
        iceb_t_soob(gettext("Не верно введена дата приказа !"));
	goto nazad;
       }
     }     

    if(N == 9 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')  //Дата документа
     {
      if(rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(N),0) != 0)
       {
        iceb_t_soob(gettext("Не верно введена дата отъезда !"));
	goto nazad;
       }
     }

    if(N == 10 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')  //Дата документа
     {
      if(rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(N),0) != 0)
       {
        iceb_t_soob(gettext("Не верно введена дата приезда !"));
	goto nazad;
       }
     }

    VV.vprintw(N,stdscr);
    N++;
   }

  if(K==SDN) /*Стрелка вниз*/
    N++;

  if(K==SUP) /*Стрелка вверх*/
    N--;

  if(N > VV.KLST-1)
    N=0;

  if(N < 0)
    N=VV.KLST-1;

  attron(VV.VVOD_return_cs(iceb_CFS)); 
  VV.vprintw(N,stdscr);
  
  switch(K)
   {
    case FK1:
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CB));
      iceb_t_pdoc("ukr2_1.txt");
      clear();
      goto nazad;

    case ESC:
    case FK10:
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CB));
      return(1);

    case FK2: //Запись
    case PLU: //Запись
      attroff(VV.VVOD_return_cs(iceb_CFS)); 

      if(metkanovdok == 0)
       datdok.new_plus(VV.VVOD_SPISOK_return_data(1));

      if(vdkrzap(*dd,*md,*gd,nomdok->ravno(),kontrz.ravno(),&VV) == 0)
       {
        rsdat(dd,md,gd,VV.VVOD_SPISOK_return_data(1),0);
        nomdok->new_plus(VV.VVOD_SPISOK_return_data(2));
        return(0);
       }
      goto nazad;

    case FK3: //Поосмотр списков реквизитов
      attroff(VV.VVOD_return_cs(iceb_CFS)); 

      MENU.VVOD_delete();
      MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));
      MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка видов командировок"));
      MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

      clearstr(LINES-1,0);
      kom1=0;      
      while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

      switch (kom1)
       {
        case 2:
        case -1:
         goto nazad;

        case 0:

         naimn.new_plus("");        
         clear();
         if(dirkontr(&kodn,&naimn,1) == 0)
          {
           VV.VVOD_SPISOK_zapis_data(0,kodn.ravno());
           naimo.new_plus(naimn.ravno());
          }
         break;

        case 1:
          clear();
          naik.new_plus("");
          poz=0;komv=0;

          if(dirmat1("Ukrvkr",&kod,&naik,LINES-5,0,2,1,&poz,&komv,"","\0") == 0)
           {
            naimkom.new_plus(naik.ravno());
            sprintf(strsql,"%s|%s",kod.ravno(),naimkom.ravno());
            VV.VVOD_SPISOK_zapis_data(3,strsql);
           }

         break;

        }
      goto nazad;
      
    case FK4: //Получение номера документа
      attroff(VV.VVOD_return_cs(iceb_CFS)); 
      rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(1),0);
      if(g != 0)
       {
        iceb_t_nomnak(g,"",strsql,0,3,0);
        VV.VVOD_SPISOK_zapis_data(2,strsql);
       }
      goto nazad;
   }
 }

}

/******************************************/
/*Запись                                  */
/******************************************/

int vdkrzap(short denz,short mesz,short godz,
const char *nomdok,
const char *kontrz,
VVOD *VV)
{
char	strsql[512];
SQL_str	row;
short	dd=0,md=0,gd=0; //Дата документа
short	dp=0,mp=0,gp=0; //Дата приказа
short	dot=0,mot=0,got=0; //Дата отъезда
short	dpr=0,mpr=0,gpr=0; //Дата приезда
short   dao=0,mao=0,gao=0; //Дата авансового отчёта
time_t	vrem;
class iceb_tu_str vkom("");
SQLCURSOR curr;
sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
VV->VVOD_SPISOK_return_data(0));
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),VV->VVOD_SPISOK_return_data(0));
  iceb_t_soob(strsql);
  return(1);
 }


if(rsdat(&dd,&md,&gd,VV->VVOD_SPISOK_return_data(1),0) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата документа !"));
  return(1);
 }


if(iceb_t_pbpds(md,gd) != 0)
 return(1);
 
if(VV->VVOD_SPISOK_return_data(3)[0] == '\0')  //Вид командировки
 {
  iceb_t_soob(gettext("Не введено вид командировки !"));
  return(1);
 }
  
if(VV->VVOD_SPISOK_return_data(8)[0] != '\0')  //Дата документа
 {
  if(rsdat(&dp,&mp,&gp,VV->VVOD_SPISOK_return_data(8),0) != 0)
   {
    iceb_t_soob(gettext("Не верно введена дата приказа !"));
    return(1);
   }
 }     

if(VV->VVOD_SPISOK_return_data(9)[0] != '\0')
 {
  if(rsdat(&dot,&mot,&got,VV->VVOD_SPISOK_return_data(9),0) != 0)
   {
    iceb_t_soob(gettext("Не верно введена дата отъезда !"));
    return(1);
   }
 }

if(VV->VVOD_SPISOK_return_data(10)[0] != '\0')  //Дата
 {
  if(rsdat(&dpr,&mpr,&gpr,VV->VVOD_SPISOK_return_data(10),0) != 0)
   {
    iceb_t_soob(gettext("Не верно введена дата приезда !"));
    return(1);
   }
 }

if(VV->VVOD_SPISOK_return_data(12)[0] != '\0')  //Дата
 {
  if(rsdat(&dao,&mao,&gao,VV->VVOD_SPISOK_return_data(12),0) != 0)
   {
    iceb_t_soob(gettext("Не верно введена дата авансового отчёта !"));
    return(1);
   }
 }

if(SRAV(nomdok,VV->VVOD_SPISOK_return_data(2),0) != 0)
 {
  //Проверяем номер документа
  sprintf(strsql,"select god from Ukrdok where god=%d and nomd='%s'",
  gd,VV->VVOD_SPISOK_return_data(2));
  if(sql_readkey(&bd,strsql,&row,&curr) != 0)
   {
    attroff(VV->VVOD_return_cs(iceb_CFM));

    sprintf(strsql,gettext("С номером %s документ уже есть !"),VV->VVOD_SPISOK_return_data(2));

    iceb_t_soob(strsql);
    return(1);
   }
 }

polen(VV->VVOD_SPISOK_return_data(3),&vkom,1,'|');

time(&vrem);

if(nomdok[0] == '\0') //Новый документ
 {
  sprintf(strsql,"insert into Ukrdok \
values (%d,'%s','%04d-%d-%d','%s','%s','%s','%s','%04d-%d-%d','%04d-%d-%d',\
'%s','%04d-%d-%d',%d,%d,'%s','%s','%04d-%d-%d',%d,%ld)",
  gd,
  VV->VVOD_SPISOK_return_data(2),
  gd,md,dd,
  VV->VVOD_SPISOK_return_data(0),
  VV->VVOD_SPISOK_return_data(4),
  VV->VVOD_SPISOK_return_data(5),
  VV->VVOD_SPISOK_return_data(6),
  got,mot,dot,
  gpr,mpr,dpr,
  VV->VVOD_SPISOK_return_data(7),
  gp,mp,dp,
  0,
  0,
  vkom.ravno(),
  VV->VVOD_SPISOK_return_data(11),
  gao,mao,dao,
  iceb_t_getuid(),
  vrem);
 }
else //Корректировка старого документа
 {
  if(md != mesz || gd != godz)
   if(iceb_t_pvkdd(8,denz,mesz,godz,md,gd,0,nomdok,0) != 0)
    return(1);   

  sprintf(strsql,"update Ukrdok \
set \
god=%d,\
nomd='%s',\
datd='%04d-%d-%d',\
kont='%s',\
pnaz='%s',\
organ='%s',\
celk='%s',\
datn='%04d-%d-%d',\
datk='%04d-%d-%d',\
nompr='%s',\
datp='%04d-%d-%d',\
mprov=%d,\
mpodt=%d,\
vkom='%s',\
nomao='%s',\
datao='%04d-%d-%d',\
ktoz=%d,\
vrem=%ld \
where god=%d and nomd='%s'",
  gd,
  VV->VVOD_SPISOK_return_data(2),
  gd,md,dd,
  VV->VVOD_SPISOK_return_data(0),
  VV->VVOD_SPISOK_return_data(4),
  VV->VVOD_SPISOK_return_data(5),
  VV->VVOD_SPISOK_return_data(6),
  got,mot,dot,
  gpr,mpr,dpr,
  VV->VVOD_SPISOK_return_data(7),
  gp,mp,dp,
  0,
  0,
  vkom.ravno(),
  VV->VVOD_SPISOK_return_data(11),
  gao,mao,dao,
  iceb_t_getuid(),
  vrem,
  godz,nomdok);
  
 }

if(sql_zap(&bd,strsql) != 0)
 {
  msql_error(&bd,gettext("Ошибка записи !"),strsql);
  return(1);
 }

if(nomdok[0] != '\0') //Корректировка записей в документе и в проводках
 {
  sprintf(strsql,"update Ukrdok1 set datd='%04d-%d-%d', nomd='%s' \
where  datd='%04d-%d-%d' and nomd='%s'",
  gd,md,dd,  VV->VVOD_SPISOK_return_data(2),
  godz,mesz,denz,nomdok);

  if(sql_zap(&bd,strsql) != 0)
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

  /*Исправляем проводки в которых дата проводки совпадает с
  датой документа*/
  printw(gettext("Проверяем проводки...\n"));
  refresh();
  sprintf(strsql,"update Prov \
set \
oper='%s',\
nomd='%s',\
datp='%04d-%02d-%02d',\
datd='%04d-%02d-%02d',\
ktoi=%d,\
vrem=%ld \
where datd='%04d-%d-%d' and datp='%04d-%d-%d' and nomd='%s' and \
pod=%s and kto='%s'",
  vkom.ravno(),VV->VVOD_SPISOK_return_data(2),gd,md,dd,gd,md,dd,iceb_t_getuid(),vrem,
  godz,mesz,denz,godz,mesz,denz,nomdok,"0",ICEB_MP_UKR);
  if(sql_zap(&bd,strsql) != 0)
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
  /*Исправляем остальные проводки*/

  sprintf(strsql,"update Prov \
set \
oper='%s',\
nomd='%s',\
datd='%04d-%02d-%02d',\
ktoi=%d,\
vrem=%ld \
where datd='%04d-%02d-%02d' and nomd='%s' and \
pod=%s and kto='%s'",
  vkom.ravno(),VV->VVOD_SPISOK_return_data(2),gd,md,dd,iceb_t_getuid(),vrem,
  godz,mesz,denz,nomdok,"0",ICEB_MP_UKR);

  if(sql_zap(&bd,strsql) != 0)
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

  if(SRAV(kontrz,VV->VVOD_SPISOK_return_data(0),0) != 0)
   {
    /*Исправляем код контрагента в проводках*/
    sprintf(strsql,"update Prov \
set \
kodkon='%s' \
where datd='%d-%d-%d' and nomd='%s' and \
pod=%s and kto='%s' and kodkon='%s'",
    VV->VVOD_SPISOK_return_data(0),
    gd,md,dd,nomdok,"0",ICEB_MP_UKR,kontrz);
    if(sql_zap(&bd,strsql) != 0)
      msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
   }
 }
return(0);
}
