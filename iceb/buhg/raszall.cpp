/* $Id: raszall.c,v 5.31 2013/09/26 09:43:39 sasa Exp $ */
/*28.03.2018	15.09.2000	Белых А.И.	raszall.c
Автоматизированный расчёт зарплаты по всем карточкам сразу
*/
#include        "buhg.h"

void	raszallr(short,short,const char*,const char*,const char *nah_only,const char *uder_only,int);

extern short	mmm,ggg;

void	raszall(int metka)   //1-только начисления 2-только удержания 3-все
{
time_t          tmm;
static class iceb_tu_str dat1("");
static class iceb_tu_str podr("");
static class iceb_tu_str tabnom("");
static class iceb_tu_str nah_only("");
static class iceb_tu_str uder_only("");
int		kom,kom1;
short		mn,gn;
int		metkaprot=0;
char		imaf[64];
int tabnomer=0;
if(dat1.ravno()[0] == '\0')
 {
  char bros[64];
  sprintf(bros,"%d.%d",mmm,ggg);
  dat1.new_plus(bros);
 }
VVOD VV(0);
VVOD MENU(3);

if(metka == 3)
  VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт начислений и удержаний по всем карточкам"));
if(metka == 1)
  VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт только начислений по всем карточкам"));
if(metka == 2)
  VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт только удержаний по всем карточкам"));

VV.VVOD_SPISOK_add_MD(gettext("Дата.............(м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения..(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Табельный номер....(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Код начисления.....(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Код удержания......(,,)."));

VV.VVOD_SPISOK_add_data(dat1.ravno(),8);
VV.VVOD_SPISOK_add_data(podr.ravno(),112);
VV.VVOD_SPISOK_add_data(tabnom.ravno(),112);
VV.VVOD_SPISOK_add_data(nah_only.ravno(),112);
VV.VVOD_SPISOK_add_data(uder_only.ravno(),112);

naz:;

clear(); 

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"-",gettext("рас.с прот."),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.VVOD_SPISOK_return_data(0));
podr.new_plus(VV.VVOD_SPISOK_return_data(1));
tabnom.new_plus(VV.VVOD_SPISOK_return_data(2));
nah_only.new_plus(VV.VVOD_SPISOK_return_data(3));
uder_only.new_plus(VV.VVOD_SPISOK_return_data(4));

class iceb_tu_str kod("");
class iceb_tu_str naimp("");

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("zarp2_7.txt");
   goto naz;

  case FK10:
  case ESC:
   return;

  case FK2:
  case PLU:
   metkaprot=0; //Без протокола
   break;

  case OTN:
   metkaprot=1; //С протоколом
   break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка начислений"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка удержаний"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка табельных номеров"));//3
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//4

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 4 :
      case -1:
       goto naz;

      case 0 :
        if(dirzarpodr(2,&podr,&naimp) == 0)
          VV.data_z_plus(1,podr.ravno());
        break;

      case 1 :
        if(dirnach(1,&nah_only,&naimp) == 0)
          VV.VVOD_SPISOK_zapis_data(3,nah_only.ravno());
        break;

      case 2 :
        if(diruder(1,&kod,&naimp) == 0)
         uder_only.z_plus(kod.ravno());
        VV.VVOD_SPISOK_zapis_data(4,uder_only.ravno());
        break;
      case 3 :
        if(dirtab(&tabnomer,&naimp,0,1) == 0)
          VV.data_z_plus(2,tabnomer);
        break;
       
     }      

    goto naz;

  case FK4:
   VV.VVOD_clear_data();
   goto naz;

  default:
    goto naz;
 }

if(rsdat1(&mn,&gn,dat1.ravno()) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата !"));
  goto naz;
 }

if(iceb_t_pbpds(mn,gn) != 0)
 goto naz;

time(&tmm);

sprintf(imaf,"avtr%d.lst",getpid());

raszallr(mn,gn,podr.ravno(),tabnom.ravno(),nah_only.ravno(),uder_only.ravno(),metka);

printw_vr(tmm);

iceb_t_soob(gettext("Расчёт закончен"));


if(metkaprot == 1)
 {
  zar_pr_show(0,0,mn,gn); 
 }



}
