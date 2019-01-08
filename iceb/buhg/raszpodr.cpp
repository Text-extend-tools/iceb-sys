/* $Id: raszpodr.c,v 5.24 2013/09/26 09:43:39 sasa Exp $ */
/*28.03.2018	17.04.2000	Белых А.И.	raszpodr.c
Расчёт свода начислений и удержаний по подразделениям
с учетом начисления зарплаты по соответствующему подразделению
*/
#include        <errno.h>
#include        "buhg.h"

void raszpodrr(short,short,const char[],const char[],const char[],FILE*,FILE*);


extern short	mmm,ggg;

int raszpodr(class spis_oth *oth)
{
static class iceb_tu_str dat1("");
static class iceb_tu_str podr("");
static class iceb_tu_str kategr("");
static class iceb_tu_str tabnom("");
int		kom,kom1;
short		mn,gn;
FILE		*ff,*ffsh;
char		imaf[64],imafsh[64];
time_t		tmm;

if(dat1.ravno()[0] == '\0')
 {
  char bros[64];
  sprintf(bros,"%d.%d",mmm,ggg);
  dat1.new_plus(bros);
 }
VVOD MENU(3);
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт свода начислений и удержаний по подразделениям"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.....(м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения.(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Код категории.....(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Табельный номер...(,,)."));

VV.VVOD_SPISOK_add_data(dat1.ravno(),8);
VV.VVOD_SPISOK_add_data(podr.ravno(),128);
VV.VVOD_SPISOK_add_data(kategr.ravno(),128);
VV.VVOD_SPISOK_add_data(tabnom.ravno(),128);

naz:;

clear(); 

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
podr.new_plus(VV.data_ravno(1));
kategr.new_plus(VV.data_ravno(2));
tabnom.new_plus(VV.data_ravno(3));

class iceb_tu_str naim("");

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("zarp4_8.txt");
   goto naz;

  case FK10:
  case ESC:
   return(1);

  case FK2:
  case PLU:
   break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка категорий"));
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
        if(dirzarpodr(2,&podr,&naim) == 0)
         VV.data_z_plus(1,podr.ravno());
        goto naz;      

      case 1 :
        vibrek("Kateg",&kategr,&naim);
        VV.VVOD_SPISOK_zapis_data(2,kategr.ravno());
        goto naz;      

     }

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();
    goto naz;

  default:
    goto naz;
 }

if(rsdat1(&mn,&gn,dat1.ravno()) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата начала !"));
  goto naz;
 }


time(&tmm);
clear();

sprintf(imaf,"podr%d.lst",getpid());
sprintf(imafsh,"podrsh%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
   {
    error_op_nfil(imaf,errno,"");
    return(1);
   }
if((ffsh = fopen(imafsh,"w")) == NULL)
   {
    error_op_nfil(imafsh,errno,"");
    return(1);
   }

raszpodrr(mn,gn,podr.ravno(),kategr.ravno(),tabnom.ravno(),ff,ffsh);



fclose(ff);
fclose(ffsh);
printw_vr(tmm);

OSTANOV();

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Расчёт свода начислений и удержаний по подразделениям"));
oth->spis_imaf.plus(imafsh);
oth->spis_naim.plus(gettext("Расчёт свода начислений и удержаний по подразделениям"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
  iceb_t_ustpeh(oth->spis_imaf.ravno(nom),0);

return(0);
}
