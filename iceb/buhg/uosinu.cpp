/*$Id: uosinu.c,v 5.5 2013/05/17 14:56:13 sasa Exp $*/
/*16.07.2015	01.06.2011	Белых А.И.	uosinu.c
Расчёт разницы балансовой стоимости и износа между бухучётом и налоговым учётом
*/
#include <errno.h>
#include "buhg.h"
#include "poiinpd.h"
#include "bsizt.h"


int uosinu(class spis_oth *oth)
{
char strsql[512];
static class iceb_tu_str dat1("");
static class iceb_tu_str inv_nom("");
static class iceb_tu_str haup("");
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт разницы балансовой стоимости и износа"));

VV.VVOD_SPISOK_add_MD(gettext("Дата расчёта...........(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Инвентарный номер.........(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Щифр аналитического учёта.(,,).."));

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(inv_nom.ravno(),128); 
VV.VVOD_SPISOK_add_data(haup.ravno(),128); 

naz:;

clear();
 
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F4",gettext("очистить"),
//"F5",gettext("настройка"),
"F10",gettext("выход"),NULL);

int kom=VV.vvod(0,1,1,-1,-1);

move(LINES-1,0);

dat1.new_plus(VV.VVOD_SPISOK_return_data(0));
inv_nom.new_plus(VV.VVOD_SPISOK_return_data(1));
haup.new_plus(VV.VVOD_SPISOK_return_data(2));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   iceb_t_pdoc("uos2_6.txt");
   goto naz;

  case ESC:
  case FK10:
   return(1);

  case FK2:
  case PLU:
     break;



  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

time_t vrem;
time(&vrem);

GDITE();
short dr,mr,gr;
if(rsdat(&dr,&mr,&gr,dat1.ravno(),1) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата !"));
  goto naz;
 }

sprintf(strsql,"select innom,naim from Uosin");
class SQLCURSOR cur;
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

int kolstr1=0;
SQL_str row;
int inn;
int podr;
int kod_mo;
//double sbs,siz,bs,iz,iz1,sbsby,sizby,bsby,izby,iz1by;
class bsizw_data bal_st;
double bs_nu=0.;
double iz_nu=0.;
double bs_bu=0.;
double iz_bu=0.;
double bs_raz=0.;
double iz_raz=0.;
double itogo[6];
memset(itogo,'\0',sizeof(itogo));

FILE *ff;
char imaf[64];
sprintf(imaf,"uosinu%d.tmp",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

zagolov(gettext("Расчёт разницы балансовой стоимости и износа"),0,0,0,dr,mr,gr,iceb_t_get_pnk("00",0),ff);

if(haup.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Шифр аналитического учёта"),haup.ravno());
if(inv_nom.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Инвентарный номер"),inv_nom.ravno());

fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"\
Инвентарн.|      Наименование            | Код | Код |   Налоговый учёт    | Бухгалтерский учёт  |      Разница        |\n\
 номер    |                              |подр.|мат.о|Бал.стоим.| Износ    |Бал.стоим.|  Износ   |Бал.стоим.|  Износ   |\n");
/*****
1234567890                                12345 12345 
*********/

fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------\n");
class poiinpdw_data rekin;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(inv_nom.ravno(),row[0],0,0) != 0)
   continue;
  inn=atol(row[0]);
  if(poiin(inn,dr,mr,gr,&podr,&kod_mo) != 0) 
     continue; //Произошел расход или на эту дату небыло прихода

//  poiinpd(inn,mr,gr,shetu,hzt,hau,hna,&popkf,&soso,nomz,&m1,&g1,hnaby,&popkfby);
  poiinpd(inn,mr,gr,&rekin);
  if(proverka(haup.ravno(),rekin.hau.ravno(),0,0) != 0)
   continue;
   
//  if(bsiz(inn,podr,dr,mr,gr,&sbs,&siz,&bs,&iz,&iz1,&sbsby,&sizby,&bsby,&izby,&iz1by,0) != 0)
  if(bsiz(inn,podr,dr,mr,gr,&bal_st,NULL) != 0)
   continue;
  
  bs_nu=bal_st.sbs+bal_st.bs;
  iz_nu=bal_st.siz+bal_st.iz+bal_st.iz1;
  
  bs_bu=bal_st.sbsby+bal_st.bsby;
  iz_bu=bal_st.sizby+bal_st.izby+bal_st.iz1by;
  
  bs_raz=bs_nu-bs_bu;
  iz_raz=iz_nu-iz_bu;    

  itogo[0]+=bs_nu;
  itogo[1]+=iz_nu;
  itogo[2]+=bs_bu;
  itogo[3]+=iz_bu;
  itogo[4]+=bs_raz;
  itogo[5]+=iz_raz;
  
  fprintf(ff,"%10d|%-*.*s|%5d|%5d|%10.2f|%10.2f|%10.2f|%10.2f|%10.2f|%10.2f|\n",
  inn,
  iceb_tu_kolbait(30,row[1]),
  iceb_tu_kolbait(30,row[1]),
  row[1],
  podr,
  kod_mo,
  bs_nu,
  iz_nu,
  bs_bu,
  iz_bu,
  bs_raz,
  iz_raz);
    
 }

fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"%*s|%10.2f|%10.2f|%10.2f|%10.2f|%10.2f|%10.2f|\n",
iceb_tu_kolbait(53,gettext("Итого")),gettext("Итого"),
itogo[0],itogo[1],itogo[2],itogo[3],itogo[4],itogo[5]);

podpis(ff);

fclose(ff);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Расчёт разницы балансовой стоимости и износа"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),1);

return(0);
}
