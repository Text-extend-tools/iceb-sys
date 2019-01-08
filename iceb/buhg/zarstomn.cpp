/* $Id: zarstomn.c,v 5.29 2013/09/26 09:43:46 sasa Exp $ */
/*28.03.2018	25.03.2000	Белых А.И.	zarstomn.c
Меню для запуска определения доли бюджетных денег в выдаче на руки и в
платежах (удержаниях).
Отчет разработан для бюджетных организаций
*/
#include        <errno.h>
#include        "buhg.h"

void zarstom(short,short,short,short,const char*,const char*,const char*,FILE*,FILE*,FILE*);

extern short	mmm,ggg;

int zarstomn(class spis_oth *oth)
{
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
static class iceb_tu_str podr("");
static class iceb_tu_str tabn("");
static class iceb_tu_str grupap("");
int		kom=0,kom1=0;
short		mn,gn,mk,gk;
FILE		*kaw;
FILE		*ffhoz;
FILE		*ffbud;
char		imaf[64];
char		imafhoz[64];
char		imafbud[64];
class iceb_tu_str fiov("");
time_t		tmm;
int tabnom=0;
int pod=0;

class VVOD MENU(3);

if(dat1.ravno()[0] == '\0')
 {
  char bros[64];
  sprintf(bros,"%d.%d",mmm,ggg);
  dat1.new_plus(bros);
 }
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт долей бюджетных счетов"));

VV.VVOD_SPISOK_add_data(dat1.ravno(),8);
VV.VVOD_SPISOK_add_data(dat2.ravno(),8);
VV.VVOD_SPISOK_add_data(podr.ravno(),128);
VV.VVOD_SPISOK_add_data(tabn.ravno(),128);
VV.VVOD_SPISOK_add_data(grupap.ravno(),128);

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.....(м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца......(м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения.(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Табельный номер...(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Группа под-ния....(,,)."));

naz:;

clear(); 

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
podr.new_plus(VV.data_ravno(2));
tabn.new_plus(VV.data_ravno(3));
grupap.new_plus(VV.data_ravno(4));

class iceb_tu_str naim("");

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("zarp4_5_3.txt");
   goto naz;

  case FK10:
  case ESC:
   return(1);

  case FK5:
  case FK2:
  case PLU:
   break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка табельных номеров"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп"));
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
       break;

      case 1 :
       fiov.new_plus("");
       if(dirtab(&tabnom,&fiov,pod,1) == 0)
        {
         tabn.z_plus(tabnom);
//         sprintf(bros,"%d",tabnom);
          VV.VVOD_SPISOK_zapis_data(3,tabn.ravno());
                  
        }
       break;
       
      case 2 :
       if(vibrek("Zargrupp",&grupap,&naim) == 0)
          VV.VVOD_SPISOK_zapis_data(4,grupap.ravno());
       
       break;
     }
    goto naz;
    

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

time(&tmm);
clear();

sprintf(imaf,"stom%d.lst",getpid());
if((kaw = fopen(imaf,"w")) == NULL)
   {
    error_op_nfil(imaf,errno,"");
    return(1);
   }

sprintf(imafhoz,"hoz%d.lst",getpid());
if((ffhoz = fopen(imafhoz,"w")) == NULL)
   {
    error_op_nfil(imafhoz,errno,"");
    return(1);
   }

sprintf(imafbud,"bud%d.lst",getpid());
if((ffbud = fopen(imafbud,"w")) == NULL)
   {
    error_op_nfil(imafbud,errno,"");
    return(1);
   }


zarstom(mn,gn,mk,gk,podr.ravno(),tabn.ravno(),grupap.ravno(),kaw,ffhoz,ffbud);



fclose(kaw);
fclose(ffhoz);
fclose(ffbud);

iceb_t_ustpeh(imaf,0);
iceb_t_ustpeh(imafhoz,0);
iceb_t_ustpeh(imafbud,0);

printw_vr(tmm);


OSTANOV();

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Расчёт долей бюджетных счетов"));
oth->spis_imaf.plus(imafhoz);
oth->spis_naim.plus(gettext("Хозрасчёт"));
oth->spis_imaf.plus(imafbud);
oth->spis_naim.plus(gettext("Бюджет"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);
 
return(0);

}
