/* $Id: amortby.c,v 5.31 2014/08/31 06:18:17 sasa Exp $ */
/*13.07.2015    06.12.2000      Белых А.И.      amortby.c
Расчёт амортзационных отчислений
для заданного инвентарного номера за заданный месяц
Определить числиться ли инвентарны номер и в каком подразделении
он находиться на заданную дату. Если не числится расчёт не проводим
Если числится смотрим настройку и делаем расчёт
В месяце убытия амортизация считается в подразделении
где числился. В подразделении где прибыл амотизация считается со
следующего месяца
Если вернули 0 - расчитали
	     1 - нет
*/
#include        "buhg.h"
#include        "uos.h"
#include "poiinpd.h"
#include "bsizt.h"

extern float      nemi; /*Необлагаемый минимум*/
void amort_udzap(int in,short mr,short gr);

short amortby(int in,
short mr,short gr,
short mra, //0-расчёт в потоке 1-нет 2-без вывода на экран
const char *grup_bu, //Группа бухгалтерского учёта
FILE *ff_prot)
{
int             i;
double		amort[4];
double		kof;
int		podr;
SQL_str         row;
SQLCURSOR curr;
char		strsql[1024];
int		kodotl=0;
int	vrabu=uosprovar(grup_bu,1); //Вариант расчёта амотизации бухгалтерского учёта


podr=0;
 
memset(amort,'\0',sizeof(amort));
 

//printw("in-%d %d.%d\n",in,mr,gr);
if(ff_prot != NULL)
  fprintf(ff_prot,"Расчёт амортизационных отчислений для бухгалтерского учёта\nin-%d %d.%d\n",in,mr,gr);


if((i=poiin(in,1,mr,gr,&podr,&kodotl)) != 0)
 {
  printw("%s %d %s %d.%d ! %s %d.\n",
  gettext("Инвентарный номер"),in,
  gettext("не числится по дате"),mr,gr,
  gettext("Код ошибки"),i);
  
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %d %s %d.%d ! %s %d.\n",
  gettext("Инвентарный номер"),in,
  gettext("не числится по дате"),mr,gr,
  gettext("Код ошибки"),i);
  return(1);
 }   

class poiinpdw_data rekin;

if((i=poiinpd(in,mr,gr,&rekin)) != 0)
 {
  printw("%s %d %s %d.%d ! %s %d.\n",
  gettext("Инвентарный номер"),in,
  gettext("не найдено настройки на дату"),mr,gr,
  gettext("Код ошибки"),i);
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %d %s %d.%d ! %s %d.\n",
  gettext("Инвентарный номер"),in,
  gettext("не найдено настройки на дату"),mr,gr,
  gettext("Код ошибки"),i);

  return(1);
 }  
/*
printw("uos.popkfby=%f uos.popkf=%f\n",uos.popkfby,uos.popkf);
OSTANOV();
*/
//sbs=siz=bs=iz=iz1=0.;
//sbsby=sizby=bsby=izby=iz1by=0.;
//Должны знать начальную стоимость даже если не считается амортизация
class bsizw_data bal_st;
//bsiz(in,podr,1,mr,gr,&sbs,&siz,&bs,&iz,&iz1,&sbsby,&sizby,&bsby,&izby,&iz1by,ff_prot);
bsiz(in,podr,1,mr,gr,&bal_st,ff_prot);

//printw("in=%d podr=%d %d.%d\n",in,podr,mr,gr);
/************
printw("bsby=%f sbsby=%f sizby=%f iz1by=%f\n",
bsby,sbsby,sizby,iz1by);
OSTANOV();
*******************/

bal_st.bsby+=bal_st.sbsby-uosgetls(in,1,mr,gr,1);
bal_st.izby+=bal_st.sizby;
bal_st.izby+=bal_st.iz1by;
double balst=bal_st.bsby;
if(rekin.soso == 1 ||  rekin.soso == 3)
 {
/*************
  printw("%s %d %s.\n",
  gettext("Инвентарный номер"),in,
  gettext("не эксплуатируется"));
***********/
  if(ff_prot != NULL)
    fprintf(ff_prot,"%s %d %s.\n",
  gettext("Инвентарный номер"),in,
  gettext("не эксплуатируется"));
  refresh();
  if(mra == 1)
   {
    sprintf(strsql,"%s %d %s.",gettext("Инвентарный номер"),in,gettext("не эксплуатируется"));
    iceb_t_soob(strsql);
   }

  amort_udzap(in,mr,gr);
  /*Записываем с нулём*/
  sprintf(strsql,"insert into Uosamor1  values (%d,'%04d-%02d-01',%d,'%s','%s',%.2f,%d,%ld,%d)",in,gr,mr,podr,rekin.hzt.ravno(),rekin.hnaby.ravno(),0.,iceb_t_getuid(),time(NULL),kodotl);

  sql_zapis(strsql,1,0);

  return(1);
 }



if(bal_st.bsby <= bal_st.izby)
 {
//  map=1;

  if(mra == 0)
   printw("%s %d %s. %.2f <= %.2f\n",
   gettext("Инвентарный номер"),in,
   gettext("амортизирован полностью"),bal_st.bsby,bal_st.izby);

  refresh();
  if(ff_prot != NULL)
    fprintf(ff_prot,"%s %d %s. bsby=%.2f <= izby=%.2f\n",
  gettext("Инвентарный номер"),in,
  gettext("амортизирован полностью"),bal_st.bsby,bal_st.izby);

  if(mra == 1)
   {
    sprintf(strsql,"%s %d %s. %.2f <= %.2f",
    gettext("Инвентарный номер"),in,
    gettext("амортизирован полностью"),bal_st.bsby,bal_st.izby);
    iceb_t_soob(strsql);
   }
  bal_st.bsby=bal_st.izby=0.;
 }

/*Читаем коэффициент*/
sprintf(strsql,"select kof from Uosgrup1 where kod='%s'",rekin.hnaby.ravno());
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  beep();
  if(mra == 1)
   {
    printw("%s %d - %s %s !\n",
    gettext("Инвентарный номер"),in,
    gettext("не нашли кода группы"),rekin.hnaby.ravno());
    OSTANOV();
   }
  if(ff_prot != NULL)
    fprintf(ff_prot,"%s %d - %s %s !\n",
  gettext("Инвентарный номер"),in,
  gettext("не нашли кода группы"),rekin.hnaby.ravno());
  refresh();
  return(1);
 }

kof=atof(row[0]);
/*
printw("bsby=%f izby=%f popkfby=%f kof=%f nemi=%f\n",bsby,izby,uos.popkfby,kof,nemi);
OSTANOV();    
*/

if(vrabu == 1)
  amort[1]=(bal_st.bsby-bal_st.izby)*rekin.popkfby*kof/100/12.;
if(vrabu == 0)
 {
  amort[1]=bal_st.bsby*rekin.popkfby*kof/100/12.;
  if(amort[1] > bal_st.bsby -bal_st.izby)
     amort[1]=bal_st.bsby -bal_st.izby;
 }
amort[0]=okrug(amort[0],0.01);
if(mra == 1)
 {
  if(vrabu == 1)
    sprintf(strsql,"(%.2f-%.2f)*%.2f/100/12*%.2f=%.2f",
    bal_st.bsby,bal_st.izby,kof,rekin.popkfby,amort[1]);
  if(vrabu == 0)
    sprintf(strsql,"%.2f*%.2f/100/12*%.2f=%.2f",
    bal_st.bsby,kof,rekin.popkfby,amort[1]);
  iceb_t_soob(strsql);
 }

if(bal_st.izby + amort[0] > bal_st.bsby)
 {
  
  if(mra == 1)
   {
    printw("%s %d %s.\n",
    gettext("Инвентарный номер"),in,
    gettext("остаточная амортизация"));
    OSTANOV();
   }
  if(ff_prot != NULL)
    fprintf(ff_prot,"%s %d %s.\n",
  gettext("Инвентарный номер"),in,
  gettext("остаточная амортизация"));
 }

amort_udzap(in,mr,gr);

printw("%s: %5.2f %s: %.2f\n",
gettext("Амортизация"),amort[1],
gettext("Балансовая стоимость"),bal_st.bsby);

if(ff_prot != NULL)
  fprintf(ff_prot,"%s: %5.2f %s: %.2f\n",
gettext("Амортизация"),amort[1],
gettext("Балансовая стоимость"),bal_st.bsby);

if(balst != 0.) //Если начальная балансовая стоимость равна 0. амортизацию не записывать
 { 

  sprintf(strsql,"insert into Uosamor1 values (%d,'%04d-%02d-01',%d,'%s','%s',%.2f,%d,%ld,%d)",in,gr,mr,podr,rekin.hzt.ravno(),rekin.hnaby.ravno(),amort[1],iceb_t_getuid(),time(NULL),kodotl);

  sql_zapis(strsql,1,0);
 }
else
 if(ff_prot != NULL)
  fprintf(ff_prot,"Начальная балансовая стоимость равна нолю. Амортизация не записывается.\n");

return(0);
}
/******************************/
/*удаление записи*/
/****************************/
void amort_udzap(int in,short mr,short gr)
{
char strsql[512];
/*Удаляем старую запись если она есть*/
//sprintf(strsql,"delete from Uosamor1 where innom=%d and god=%d and mes=%d",in,gr,mr);
sprintf(strsql,"delete from Uosamor1 where innom=%d and da >= '%04d-%02d-01' and da <= '%04d-%02d-31'",in,gr,mr,gr,mr);

sql_zapis(strsql,1,0);

}
