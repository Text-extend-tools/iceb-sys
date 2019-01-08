/*$Id:$*/
/*20.12.2017	28.03.2017	Белых А.И.	zarvds.cpp
ввод даты и счёта
*/
 
#include "buhg.h"


int zarvds(class iceb_tu_str *data,class iceb_tu_str *shet,int *metka_vd)
{
class SPISOK sp_ved;
class iceb_tu_str vid_ved("");
int N=0,K=0;

sp_ved.plus(gettext("Ведомость по подразделениям"));
sp_ved.plus(gettext("Всех в одной ведомости"));
sp_ved.plus(gettext("Каждого в отдельной ведомости"));

vid_ved.new_plus(sp_ved.ravno(*metka_vd));

class VVOD VV(0);

VV.VVOD_SPISOK_add_ZAG(gettext("Введите данные для зарплатной ведомости"));

VV.VVOD_SPISOK_add_MD(gettext("Дата зарплаты.(м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт................."));
VV.VVOD_SPISOK_add_MD(gettext("Вид ведомости........"));

VV.VVOD_SPISOK_add_data(data->ravno(),8);
VV.VVOD_SPISOK_add_data(shet->ravno(),20);
VV.VVOD_SPISOK_add_data(vid_ved.ravno(),50);



naz:;

VV.data_plus(2,vid_ved.ravno());
clear();

helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("ввод"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

VV.vramka(-1,-1,0);



class iceb_tu_str kod("");
class iceb_tu_str naim("");


for(;;)
 {
  attron(VV.VVOD_return_cs(iceb_CFS)); 
  VV.vprintw(N,stdscr);
  
  K=getch();
  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));

  VV.vprintw(N,stdscr);

  if(K == KEY_RESIZE)
   goto naz;
  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));

    VV.vprintw_clear(N,stdscr);


    if(N == 2)      /*Вид ведомости*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFM));
      class VVOD MENU(3);

      MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное"));
      for(int nom=0; nom < sp_ved.kolih(); nom++)
       MENU.VVOD_SPISOK_add_MD(sp_ved.ravno(nom));

      clearstr(LINES-1,0);
      
      while(menu3w(stdscr,&MENU,metka_vd,-1,-1,0) != 0);
      
      vid_ved.new_plus(sp_ved.ravno(*metka_vd));

      goto naz;
     }  

    VV.vgetstr(N,stdscr);
    
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

  switch (K)
   {
    case ESC :
    case FK10:
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));
      return(1);

    case PLU:
    case FK2:
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));

      data->new_plus(VV.VVOD_SPISOK_return_data(0));
      shet->new_plus(VV.VVOD_SPISOK_return_data(1));

      return(0);
      
    case FK4:
      VV.data_plus(0,"");
      VV.data_plus(1,"");
      metka_vd=0;
      goto naz;
   } 
 }

return(0);

}

