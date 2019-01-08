/* $Id: dooc.c,v 5.15 2013/05/17 14:56:01 sasa Exp $ */
/*01.12.2016	05.03.2001	Белых А.И.	dooc.c
Программа дооценки полученных материалов
*/
#include        <errno.h>
#include        "buhg.h"

int doocr(short,short,short,const char*,int,short,short,short,double,const char*,const char*,const char *kodop);


void dooc(short dd,short md,short gd,const char *nomdok,int skl)
{
static class iceb_tu_str dat1("");
static class iceb_tu_str osnov("");
static class iceb_tu_str kodmat("");
static class iceb_tu_str kodop("");
class iceb_tu_str suma("");
int		kom;
short		dr,mr,gr;
char		bros[512];
int kom1=0;
class VVOD VV(0);
class VVOD MENU(3);


VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(kodop.ravno(),32);
VV.VVOD_SPISOK_add_data(osnov.ravno(),64);
VV.VVOD_SPISOK_add_data(kodmat.ravno(),128);

sprintf(bros,"%s: %s %d.%d.%d %d",
gettext("Дооценка приходного документа"),nomdok,dd,md,gd,skl);
VV.VVOD_SPISOK_add_ZAG(bros);

VV.VVOD_SPISOK_add_MD(gettext("Дата документов (д.м.г).."));//0
VV.VVOD_SPISOK_add_MD(gettext("Сумма дооценки..........."));//1
VV.VVOD_SPISOK_add_MD(gettext("Код операции дооценки...."));//2
VV.VVOD_SPISOK_add_MD(gettext("Основание................"));//3
VV.VVOD_SPISOK_add_MD(gettext("Код материалла.....(,,).."));//4


naz1:;
clear();


helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.VVOD_SPISOK_return_data(0));
suma.new_plus(VV.VVOD_SPISOK_return_data(1));
kodop.new_plus(VV.VVOD_SPISOK_return_data(2));
osnov.new_plus(VV.VVOD_SPISOK_return_data(3));
kodmat.new_plus(VV.VVOD_SPISOK_return_data(4));

class iceb_tu_str naim("");
class iceb_tu_str kod("");
int kodm=0;
int kgr=0;
switch (kom)
 {
  case ESC:
  case FK10:
     return;
     break;

  case FK1:
    GDITE();
    iceb_t_pdoc("matu2_1_2h.txt");
    clear();
    goto naz1;

  case PLU:
  case FK2:
     break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка материалов"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 2 :
      case -1:
       goto naz1;


      case 0 :
//        if(vibrek("Prihod",&kod) == 0)
         if(dirprihod(1,&kod,&naim) == 0)
          VV.VVOD_SPISOK_zapis_data(2,kod.ravno());
        goto naz1;

      case 1 :

        if(dirmatr(&kgr,&kodm,&kod,1,0,"",0,0) == 0)
         {
            VV.data_z_plus(4,kodm);
         }

        goto naz1;

     }

  default:
    goto naz1;
    break;
 }

if(rsdat(&dr,&mr,&gr,dat1.ravno(),0) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата !"));
  goto naz1;
 }

if(suma.ravno_atof() <= 0.)
 {
  iceb_t_soob(gettext("Не верно введена сумма дооценки !"));
  goto naz1;
 }

if(kodop.getdlinna() <= 1)
 {
  iceb_t_soob(gettext("Не ввели код операции!"));
  goto naz1;
 }
SQL_str row;
class SQLCURSOR cur;

char strsql[1024];
sprintf(strsql,"select vido from Prihod where kod='%s'",kodop.ravno());
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %s!",gettext("В справочнике приходных операций не найден код операции"),kodop.ravno());
  iceb_t_soob(strsql);
  goto naz1;
 }

if(atof(row[0]) != 2)
 {
  iceb_t_soob(gettext("Вид приходной операции должен быть \"Смена стоимости\""));
  goto naz1;
 }

sprintf(strsql,"select vido from Rashod where kod='%s'",kodop.ravno());
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %s!",gettext("В справочнике расходных операций не найден код операции"),kodop.ravno());
  iceb_t_soob(strsql);
  goto naz1;
 }

if(atof(row[0]) != 2)
 {
  iceb_t_soob(gettext("Вид расходной операции должен быть \"Смена стоимотси\""));
  goto naz1;
 }


if(doocr(dd,md,gd,nomdok,skl,dr,mr,gr,suma.ravno_atof(),osnov.ravno(),kodmat.ravno(),kodop.ravno()) != 0)
 goto naz1;

}
