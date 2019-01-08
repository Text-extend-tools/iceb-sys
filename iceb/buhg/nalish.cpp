/* $Id: nalish.c,v 5.21 2013/09/26 09:43:36 sasa Exp $ */
/*15.07.2015	13.06.1998	Белых А.И.	nalish.c
Программа распечатки документа для сдачи наличных в банк
*/
#include        <errno.h>
#include        "buhg.h"


extern short    ddd,mmm,ggg; /*Месяц год*/
extern char     *printer; /*Печать*/

void nalish(VVOD *VV,short mv) //0- ввод нового 1-корректировка
{
int		par,kom1;
short		d,m,g;
double		bb;
class iceb_tu_str kod("");
class iceb_tu_str naim("");
time_t		vrem;
short		dz,mz,gz;
class iceb_tu_str nppz("");
SQL_str         row1;
SQLCURSOR       cur1;
char		strsql[1024];
int		poz,komv;
VVOD MENU(3);

rsdat(&dz,&mz,&gz,VV->VVOD_SPISOK_return_data(2),1);
nppz.new_plus(VV->VVOD_SPISOK_return_data(0));


if(VV->VVOD_SPISOK_return_data(3)[0] != '\0')
 {
  sprintf(strsql,"select naik from Oznvb where kod='%s'",VV->VVOD_SPISOK_return_data(3));
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код операции"),VV->VVOD_SPISOK_return_data(3));
    iceb_t_soob(strsql);
   }
  else
    naim.new_plus(row1[0]);
 }

if(VV->VVOD_SPISOK_return_data(4)[0] == '\0')
 {
  class iceb_tu_str fio("");
  iceb_t_poldan("Фамилия сдающего наличные в банк",&fio,"nastdok.alx");
  VV->data_plus(4,fio.ravno());
 }

VV->VVOD_SPISOK_add_ZAG(gettext("Ввод и корректировка объявок для здачи налички в банк"));
VV->VVOD_SPISOK_add_ZAG(naim.ravno());

VV->VVOD_SPISOK_add_MD(gettext("Номер документа.........."));
VV->VVOD_SPISOK_add_MD(gettext("Сумма...................."));
VV->VVOD_SPISOK_add_MD(gettext("Дата....................."));
VV->VVOD_SPISOK_add_MD(gettext("Код назначения платежа..."));
VV->VVOD_SPISOK_add_MD(gettext("Через кого..............."));

naz:;
clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2",gettext("запись"),
"F3",gettext("реквизиты"),
"F10",gettext("выход"),NULL);

par=VV->vvod(0,1,1,-1,-1);

switch(par)
 {
  case FK2:
  case PLU:
    break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр кодов назначения платежа"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка фамилий"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 2 :
      case -1:
       goto naz;

      case 0 :

        clear();

        naim.new_plus("");
        kod.new_plus("");
        poz=komv=0;
        if(dirmat1("Oznvb",&kod,&naim,LINES-5,1,2,1,&poz,&komv,"","\0") == 0)
         {
          VV->VVOD_SPISOK_zapis_data(3,kod.ravno());
         }
        goto naz;

      case 1 :

        clear();

        naim.new_plus("");
        kod.new_plus("");

        poz=komv=0;
        if(dirmat1("Ksn",&kod,&naim,LINES-5,1,2,1,&poz,&komv,"","\0") == 0)
         {
          VV->VVOD_SPISOK_zapis_data(4,naim.ravno());
         }
        goto naz;
     
     }

    goto naz;


  case FK10:
  case ESC:
    return;

  default:
    goto naz;
 }


if(VV->VVOD_SPISOK_return_data(0)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введён номер документа !"));
  goto naz;
 }

if(VV->VVOD_SPISOK_return_data(1)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введена сумма !"));
  goto naz;
 }
if(rsdat(&d,&m,&g,VV->VVOD_SPISOK_return_data(2),1) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата !"));
  goto naz;
 }

/*Проверяем номер документа*/
if(mv == 0)
 {
  sprintf(strsql,"select nomd from Obqvka where datd >= '%d-01-01' and \
datd <= '%d-12-31' and nomd='%s'",g,g,VV->VVOD_SPISOK_return_data(0));
  if(sql_readkey(&bd,strsql,&row1,&cur1) >= 1)
   {
    sprintf(strsql,gettext("С номером %s документ уже есть !"),VV->VVOD_SPISOK_return_data(0));
    iceb_t_soob(strsql);
    goto naz;
   }
 }  

/*Проверяем код операции*/
sprintf(strsql,"select naik from Oznvb where kod='%s'",VV->VVOD_SPISOK_return_data(3));
if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
 {
  sprintf(strsql,gettext("Не найден код операции %s !"),VV->VVOD_SPISOK_return_data(3));
  iceb_t_soob(strsql);
  goto naz;
 }

naim.new_plus(row1[0]);

/*Если корректировка удаляем старую запись*/

time(&vrem);

bb=ATOFM(VV->VVOD_SPISOK_return_data(1));
bb=okrug(bb,0.01);

if(mv == 1)
 {
  sprintf(strsql,"update Obqvka \
set \
datd='%d-%02d-%02d',\
nomd='%s',\
suma=%.2f,\
kodop='%s',\
fio='%s',\
ktoz=%d,\
vrem=%ld \
where datd='%d-%02d-%02d' and nomd='%s'",
  g,m,d,VV->VVOD_SPISOK_return_data(0),bb,VV->VVOD_SPISOK_return_data(3),VV->VVOD_SPISOK_return_data(4),
  iceb_t_getuid(),vrem,gz,mz,dz,nppz.ravno());
 }  
else
 {
  sprintf(strsql,"insert into Obqvka \
values ('%d-%02d-%02d','%s',%.2f,'%s','%s',0,0,%d,%ld)",
  g,m,d,VV->VVOD_SPISOK_return_data(0),bb,VV->VVOD_SPISOK_return_data(3),VV->VVOD_SPISOK_return_data(4),
  iceb_t_getuid(),vrem);   

 }

if(sql_zapis(strsql,0,0) != 0)
 {
  goto naz;
 }


rasobqv(VV->VVOD_SPISOK_return_data(0),VV->VVOD_SPISOK_return_data(1),VV->VVOD_SPISOK_return_data(2),naim.ravno(),VV->VVOD_SPISOK_return_data(4));
 
}

/**********************************/
void	soznalish(VVOD *VV)
{
VV->VVOD_SPISOK_add_data(10);
VV->VVOD_SPISOK_add_data(20);
VV->VVOD_SPISOK_add_data(11);
VV->VVOD_SPISOK_add_data(30);
VV->VVOD_SPISOK_add_data(30);
}
