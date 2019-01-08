/*$Id: muraskod.c,v 5.5 2013/05/17 14:56:05 sasa Exp $*/
/*15.07.2015	04.11.2010	Белых А.И.	muraskod.c
Распечатка движения по карточкам конкретного кода материала
*/
#include <errno.h>
#include "buhg.h"


int muraskod(class spis_oth *oth,int kodm)
{
char strsql[1024];
static iceb_tu_str dat1("");
static iceb_tu_str dat2("");
static iceb_tu_str sklad("");
class iceb_tu_str naimmat("");
int kolstr=0,kolstr2=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;
FILE *ff;
char imaf[56];
double itogo[8];

sprintf(strsql,"select naimat from Material where kodm=%d",kodm);
if(readkey(strsql,&row,&cur) == 1)
 naimmat.new_plus(row[0]); 
int kom=0;

VVOD MENU(3);
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Движение материала по карточкам"));
sprintf(strsql,"%s %d %s",gettext("Код материала"),kodm,naimmat.ravno());
VV.VVOD_SPISOK_add_ZAG(strsql);

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Склад..........(,,).."));

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(sklad.ravno(),112);

naz:;

clear();
 
helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.VVOD_SPISOK_return_data(0));
dat2.new_plus(VV.VVOD_SPISOK_return_data(1));
sklad.new_plus(VV.VVOD_SPISOK_return_data(2));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   iceb_t_pdoc("buhg5_1.txt");
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

short dn=0,mn=0,gn=0;
short dk=0,mk=0,gk=0;

if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
 goto naz;


sprintf(strsql,"select sklad,nomk,ei,shetu,cena,innom from Kart where kodm=%d",kodm);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(1);
 }

sprintf(imaf,"murk%d.lst",getpid());
if((ff=fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

iceb_t_zagolov(gettext("Движение материала по карточкам"),dn,mn,gn,dk,mk,gk,ff);

fprintf(ff,"%s:%d %s\n",gettext("Код материала"),kodm,naimmat.ravno());

if(sklad.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Склад"),sklad.ravno());
 
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
Склад |Номер |Инвентарн.|Единица| Счёт   |   Цена   |Остаток на %02d.%02d.%04d|      Приход         |      Расход         |Остаток на %02d.%02d.%04d|\n\
      |ка-тки| номер    |измере.| учёта  |          |Количество|  Сумма   |Количество|  Сумма   |Количество|  Сумма   |Количество|  Сумма   |\n",dn,mn,gn,dk,mk,gk);
/*******************
123456|123456|1234567890|1234567|12345678|1234567890|1234567890|1234567890|
********************/
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------\n");
memset(itogo,'\0',sizeof(itogo));
short dd,md,gd;
int kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(sklad.ravno(),row[0],0,0) != 0)
   continue;
  class ostatokt ost;
  ostkar(dn,mn,gn,dk,mk,gk,atoi(row[0]),atoi(row[1]),&ost);

  if(ost.ostm[0] == 0. &&   ost.ostm[1] == 0. &&   ost.ostm[2] == 0. &&   ost.ostm[3] == 0.)
   continue;

  fprintf(ff,"%-6s %-6s %-10s %-*s %-*s %10.10g %10.10g %10.2f %10.10g %10.2f %10.10g %10.2f %10.10g %10.2f\n",
  row[0],row[1],row[5],
  iceb_tu_kolbait(7,row[2]),row[2],
  iceb_tu_kolbait(8,row[3]),row[3],
  atof(row[4]),
  ost.ostm[0],ost.ostmc[0],  
  ost.ostm[1],ost.ostmc[1],  
  ost.ostm[2],ost.ostmc[2],  
  ost.ostm[3],ost.ostmc[3]);
  
  itogo[0]+=ost.ostm[0];
  itogo[1]+=ost.ostmc[0];
  itogo[2]+=ost.ostm[1];
  itogo[3]+=ost.ostmc[1];
  itogo[4]+=ost.ostm[2];
  itogo[5]+=ost.ostmc[2];
  itogo[6]+=ost.ostm[3];
  itogo[7]+=ost.ostmc[3];
    
  sprintf(strsql,"select nomd,datdp,datd,tipz,kolih from Zkart where \
  sklad=%s and nomk=%s and datdp >= '%04d-%02d-%02d' and datdp <=  '%04d-%02d-%02d'",
  row[0],row[1],gn,mn,dn,gk,mk,dk);

  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    return(1);
   }

  while(cur1.read_cursor(&row1) != 0)
   {
    rsdat(&dd,&md,&gd,row1[1],2);
    if(SRAV("000",row1[0],0) == 0)
     if(sravmydat(dn,mn,gn,dd,md,gd) != 0)
      continue;

    sprintf(strsql,"%s %-10s",iceb_tu_datzap(row1[1]),row1[0]);
    if(atoi(row1[3]) == 1) /*Приход*/
      fprintf(ff,"%74s %10.10g %10.2f\n",strsql,atof(row1[4]),atof(row1[4])*atof(row[4]));
    if(atoi(row1[3]) == 2) /*расход*/
      fprintf(ff,"%74s %10s %10s %10.10g %10.2f\n",strsql,"","",atof(row1[4]),atof(row1[4])*atof(row[4]));
   }
fprintf(ff,"\
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");

 }
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"%52s:%10.10g %10.2f %10.10g %10.2f %10.10g %10.2f %10.10g %10.2f\n",
gettext("Итого"),
itogo[0],itogo[1],itogo[2],itogo[3],itogo[4],itogo[5],itogo[6],itogo[7]);

podpis(ff);
fclose(ff);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Движение материала по карточкам"));

for(int nomer=0; nomer < oth->spis_imaf.kolih(); nomer++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nomer),1);

return(0);
}