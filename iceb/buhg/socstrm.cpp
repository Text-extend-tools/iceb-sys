/* $Id: socstrm.c,v 5.33 2013/09/26 09:43:40 sasa Exp $ */
/*28.03.2018	05.04.2000	Белых А.И.	socstrm.c
Меню для запуска расчёта свода социальных отчислений с
фонда зарплаты
*/
#include        <errno.h>
#include        "buhg.h"

void	socstr(short,short,short,short,const char*,const char*,const char*,char*,FILE*,FILE*);

extern short	mmm,ggg;

int socstrm(class spis_oth *oth)
{
static class iceb_tu_str kodf("");
static class iceb_tu_str tabnom("");
static class iceb_tu_str podr("");
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
int		kom,kom1;
class iceb_tu_str naim("");
short		mn,gn;
short		mk,gk;
time_t		tmm;
char		imaf[64],imaf1[64];
FILE		*ff,*ff1;
int		tabn;
class iceb_tu_str fiov("");

if(dat1.ravno()[0] == '\0')
 {
  char bros[512];
  sprintf(bros,"%d.%d",mmm,ggg);
  dat1.new_plus(bros);
 }
class VVOD MENU(3);
class VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт начисления ЕСВ"));

VV.VVOD_SPISOK_add_data(dat1.ravno(),8);
VV.VVOD_SPISOK_add_data(dat2.ravno(),8);
VV.VVOD_SPISOK_add_data(podr.ravno(),128);
VV.VVOD_SPISOK_add_data(tabnom.ravno(),128);
VV.VVOD_SPISOK_add_data(kodf.ravno(),64);

VV.VVOD_SPISOK_add_MD(gettext("Дата начала (м.г)...."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца (м.г)....."));
VV.VVOD_SPISOK_add_MD(gettext("Подразделение.(,,)..."));
VV.VVOD_SPISOK_add_MD(gettext("Табельный номер (,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Код соц. фонда..(,,)."));

naz:;
clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F5",gettext("настройка"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
podr.new_plus(VV.data_ravno(2));
tabnom.new_plus(VV.data_ravno(3));
kodf.new_plus(VV.data_ravno(4));
switch (kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("zarp4_7_4.txt");
   goto naz;
  case ESC:
  case FK10:
     return(1);
     break;

  case PLU:
  case FK2:
     break;

  case FK3:

    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка табельных номеров"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка соц. фондов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 3 :
      case -1:
       goto naz;

      case 0 :
        if(dirzarpodr(2,&podr,&naim) == 0)
         VV.data_z_plus(2,podr.ravno());
        goto naz;

      case 1 :

        tabn=0;
        if(dirtab(&tabn,&fiov,0,1) == 0)
         {
          tabnom.z_plus(tabn);
          VV.VVOD_SPISOK_zapis_data(3,tabnom.ravno());
         }
        goto naz;

      case 2 :
        vibrek("Zarsoc",&kodf,&naim);
        VV.VVOD_SPISOK_zapis_data(4,kodf.ravno());
        goto naz;

     }    
  case FK4:
    VV.VVOD_clear_data();
    goto naz;

  case FK5:
    iceb_redfil("socstr.alx",0);
    goto naz;
    
  default:
    goto naz;
    break;
 }
 
if(rsdat1(&mn,&gn,dat1.ravno()) != 0)
 { 
  iceb_t_soob(gettext("Не верно введена дата начала!"));
  goto naz;
 } 
if(dat2.ravno()[0] == '\0')
 {
  mk=mn; gk=gn;
 }
else
 if(rsdat1(&mk,&gk,dat2.ravno()) != 0)
  {
   iceb_t_soob(gettext("Не верно введена дата конца !"));
   goto naz;
  }

clear(); /*Очистить экран и закрасить фоновым цветом*/
printw("%s\n",gettext("Расчёт начисления ЕСВ"));
printw("%d.%d%s => %d.%d%s\n",
mn,gn,gettext("г."),
mk,gk,gettext("г."));
GDITE();

time(&tmm);

sprintf(imaf,"fond%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
fprintf(ff,"\x1B\x45"); //режим выделенного шрифта

sprintf(imaf1,"fondr%d.lst",getpid());

if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  error_op_nfil(imaf1,errno,"");
  return(1);
 }
fprintf(ff1,"\x1B\x45"); //режим выделенного шрифта

char imaf_dol[64];
memset(imaf_dol,'\0',sizeof(imaf_dol));
socstr(mn,gn,mk,gk,podr.ravno(),tabnom.ravno(),kodf.ravno(),imaf_dol,ff,ff1);

podpis(ff);
podpis(ff1);

fclose(ff);
fclose(ff1);
printw_vr(tmm);

OSTANOV();

oth->spis_imaf.plus(imaf1);
oth->spis_naim.plus(gettext("Расчёт начисления ЕСВ по работникам"));
oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Общие итоги"));
if(imaf_dol[0] != '\0')
 {
  oth->spis_imaf.plus(imaf_dol);
  oth->spis_naim.plus(gettext("Расчёт долей ЕСВ"));
 }

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),0);
 
return(0);
}
