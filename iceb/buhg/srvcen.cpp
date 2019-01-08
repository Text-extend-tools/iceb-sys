/* $Id: srvcen.c,v 5.26 2013/09/26 09:43:41 sasa Exp $ */
/*19.03.2013	23.02.2001	Белых А.И	srvcen.c	
Программа определения средневзвешенной цены на материал
  Стартовое меню
*/
#include        <errno.h>
#include        "buhg.h"

void srvcenr(short,short,const char[],const char[],const char[],const char[],const char *kodop_r);

extern struct M2 LL;
extern char     data[24][80]; /*Данные для отображения или ввода*/
extern short    dls[24]; /*Массив для определения длинны данных*/
extern char	*maskmat; /*Массив кодов отмеченных материалов*/
extern char	*imabaz;

void	srvcen()
{
char	strsql[512];
static class iceb_tu_str dat1("");
static class iceb_tu_str sklz("");
static class iceb_tu_str shbz("");
static class iceb_tu_str kodmz("");
static class iceb_tu_str kgrmz("");
static class iceb_tu_str kodop_r("");
int		kom=0;
int		kom1=0;
char		kod[64];
char		bros[512];
int		kgr,kodm;
short		mr,gr;

VVOD MENU(3);
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт средневзвешенных цен на материалы"));

VV.VVOD_SPISOK_add_data(dat1.ravno(),8);
VV.VVOD_SPISOK_add_data(shbz.ravno(),128);
VV.VVOD_SPISOK_add_data(sklz.ravno(),128);
VV.VVOD_SPISOK_add_data(kgrmz.ravno(),128);
VV.VVOD_SPISOK_add_data(kodmz.ravno(),128);
VV.VVOD_SPISOK_add_data(kodop_r.ravno(),128);

VV.VVOD_SPISOK_add_MD(gettext("Дата расчёта (м.г)......"));//0
VV.VVOD_SPISOK_add_MD(gettext("Счёт..(,,).............."));//1
VV.VVOD_SPISOK_add_MD(gettext("Склад..(,,)............."));//2
VV.VVOD_SPISOK_add_MD(gettext("Группа материалов..(,,)."));//3
VV.VVOD_SPISOK_add_MD(gettext("Код материалла.(,,/*)..."));//4
VV.VVOD_SPISOK_add_MD(gettext("Код операции............"));//5

naz:;
clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.VVOD_SPISOK_return_data(0));
shbz.new_plus(VV.VVOD_SPISOK_return_data(1));
sklz.new_plus(VV.VVOD_SPISOK_return_data(2));
kgrmz.new_plus(VV.VVOD_SPISOK_return_data(3));
kodmz.new_plus(VV.VVOD_SPISOK_return_data(4));
kodop_r.new_plus(VV.data_ravno(5));

class iceb_tu_str kods("");
class iceb_tu_str naims("");
switch (kom)
 {
  case ESC:
  case FK10:
     return;
     break;

  case FK1:
    GDITE();
    iceb_t_pdoc("matu2_4.txt");
    clear();
    goto naz;

  case PLU:
  case FK2:
     break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите  нужное и нажмите Enter"));

    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка складов"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка материалов"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));//3
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций"));//4
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//5

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 5 :
      case -1:
       goto naz;


      case 0 :
        if(dirsklad(1,&kods,&naims) == 0)
        VV.data_plus(2,kods.ravno());
//         MENU.data_z_plus(2,kods.ravno());
        goto naz;

      case 1 :
        vibrek("Grup",&kgrmz);
        VV.data_plus(3,kgrmz.ravno());
        goto naz;

      case 2 :

        memset(kod,'\0',sizeof(kod));
        bros[0]='\0';
        if(dirmatr(&kgr,&kodm,&naims,1,0,bros,0,0) == 0)
         {
          VV.data_plus(4,kodm);
         }

        clear();
        goto naz;

      case 3 :
        vibrek("Plansh",&shbz);
        VV.data_plus(1,shbz.ravno());
        goto naz;

      case 4 :
//        vibrek("Rashod",&kods,&naims);
        if(dirrashod(1,&kods,&naims) == 0)
          VV.data_plus(5,kods.ravno());
//        MENU.VVOD_SPISOK_zapis_data(5,kods.ravno());
        goto naz;
     }

  case FK4:
    VV.VVOD_clear_data();
    goto naz;

  default:
    goto naz;
    break;
 }

if(rsdat1(&mr,&gr,dat1.ravno()) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата !"));
  goto naz;
 }

if(iceb_t_pbpds(mr,gr) != 0)
  goto naz;

if(kodop_r.getdlinna() <= 1)
 {
  iceb_t_soob(gettext("Не введён код операции!"));
  goto naz;
 }

//Проверяем есть ли код операции в списке приходов и расходов
sprintf(strsql,"select kod from Prihod where kod='%s'",kodop_r.ravno());
if(readkey(strsql) != 1)
 {
  sprintf(strsql,gettext("Не найден код %s в списке операций приходов !"),kodop_r.ravno());
  iceb_t_soob(strsql);
  goto naz;

 }
sprintf(strsql,"select kod from Rashod where kod='%s'",kodop_r.ravno());
if(readkey(strsql) != 1)
 {
  sprintf(strsql,gettext("Не найден код %s в списке операций расходов !"),kodop_r.ravno());
  iceb_t_soob(strsql);
  goto naz;

 }
if(kodmz.ravno()[0] == '*' && maskmat == NULL)
 { 
  iceb_t_soob(gettext("Не сделан список материалов !"));
  goto naz;
 } 

srvcenr(mr,gr,shbz.ravno(),kgrmz.ravno(),kodmz.ravno(),sklz.ravno(),kodop_r.ravno());

}
