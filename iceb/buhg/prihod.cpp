/*$Id:$ */
/*22.03.2017    20.01.1993      Белых А.И.      prihod.c
Ввод приходов в уже существующие карточки материалов
Возвращяем 1 если не записали
	   0 записали
*/
#include	"buhg.h"


int prihod(short dd,short md,short gd,int skl,const char *nomd,
int km, //Код материалла
int nk, //Номер карточки
double cena,const char *ei,int mnds,double ndsg,
const char *nomz, //Номер заказа
int kmm) //Если = 0 привязать карточку
{
char		strsql[1024];
double		kolih;
int N=0,K=0;


if(kmm != 0)
 {
  /*Привязываем к карточке*/
  sprintf(strsql,"update Dokummat1 \
set \
nomkar=%d \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' and kodm=%d and \
nomkar=0",nk,gd,md,dd,skl,nomd,km);
  
  sql_zapis(strsql,1,0);
  return(0);
 }   

VVOD VV(0);
VV.VVOD_SPISOK_add_MD(gettext("Количество........."));//0
VV.VVOD_SPISOK_add_MD(gettext("Счёт получения....."));//7
VV.VVOD_SPISOK_add_data(20);//0
VV.VVOD_SPISOK_add_data(20);//1

naz:;

//clear();

helstr(LINES-1,0,"F2/+",gettext("запись"),
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
    getch();
    goto naz;
   }

  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));
    VV.vprintw_clear(N,stdscr);

    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 0) /*Количество*/
     {
      N++;
      goto naz;
     }

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
    return(1);
//    N=-1;
//    break;
   }

  if(K == FK2 || K == PLU) /*Записываем*/
   {
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));

    struct OPSHET shetv;

    if(VV.data_ravno(1)[0] != '\0')
     if(prsh1(VV.data_ravno(1),&shetv) != 0)
       goto naz;

    break;
   }
 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));

kolih=VV.data_ravno_atof(0);

if(kolih == 0.)
 {
  iceb_t_soob(gettext("Не введено количество !"));
  goto naz;
 }
if(kolih < 0.)
 {
  short dt,mt,gt;
  poltekdat(&dt,&mt,&gt);
  class ostatokt ost;
  /*Остаток по карточкам*/
  ostkar(1,1,gt,31,mt,gt,skl,nk,&ost);
  double ostkart=ost.ostg[3];
  if( ostkart+kolih < -0.0001)
   {
    sprintf(strsql,"%s!",gettext("Отрицательный остаток"));    
    iceb_t_soob(strsql);
    return(1);
   }
 }
zapvdok(dd,md,gd,skl,nk,km,nomd,kolih,cena,ei,ndsg,mnds,0,1,0,VV.data_ravno(1),nomz,0,"");

return(0);
}
