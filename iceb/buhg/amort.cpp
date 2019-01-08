/* $Id: amort.c,v 5.42 2013/09/26 09:43:28 sasa Exp $ */ 
/*29.12.2016	08.07.1996 Белых А.И.  amort.c
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
void	amortbo(int,int,int kodotl,double,short,double,double,short mra,class poiinpdw_data *rekin);
void amort_udzap(int in,short mr,short gr,short mkr);

extern float      nemi; /*Необлагаемый минимум*/
extern short metkabo;  //Если 1 то организация бюджетная

short amort(int in,
short mr,short gr,
short mra, //0-расчёт в потоке 1-нет 2-без вывода на экран
short kvrt,  //Квартал
const char *grupa,
FILE *ff_prot)

{
int             i,i1;
class bsizw_data bal_st;
double		amort[4];
double		kof;
int		podr=0;
SQL_str         row;
SQLCURSOR curr;
char		strsql[1024];
time_t		vrem;
int		kodotl;
short mkr=mr;
if(metkabo == 1)
  mr=1;
  

memset(&amort,'\0',sizeof(amort));
 
if(kvrt != 0)
 {
  if(kvrt == 1)
    mr=1;
  if(kvrt == 2)
    mr=4;
  if(kvrt == 3)
    mr=7;
  if(kvrt == 4)
    mr=10;
  mkr=mr+2;
 }

if(mra == 0)
  printw("in-%d %d.%d\n",in,mr,gr);
if(ff_prot != NULL)
  fprintf(ff_prot,"Расчёт амортизации для налогового учёта.\nin-%d %d.%d\n",in,mr,gr);


if(metkabo == 1)
 {
  if((i=poiin(in,1,1,gr+1,&podr,&kodotl)) != 0)
   {
    printw("%s %d %s %d.%d ! %s %d.\n",
    gettext("Инвентарный номер"),in,
    gettext("не числится по дате"),1,gr+1,
    gettext("Код результата поиска"),i);

    if(ff_prot != NULL)
     fprintf(ff_prot,"%s %d %s %d.%d ! %s %d.\n",
    gettext("Инвентарный номер"),in,
    gettext("не числится по дате"),1,gr+1,
    gettext("Код результата поиска"),i);
    return(1);
   }   
 }
else
 if((i=poiin(in,1,mr,gr,&podr,&kodotl)) != 0)
  {
   printw("%s %d %s %d.%d ! %s %d.\n",
   gettext("Инвентарный номер"),in,
   gettext("не числится по дате"),mr,gr,
   gettext("Код результата поиска"),i);


   if(ff_prot != NULL)
     fprintf(ff_prot,"%s %d %s %d.%d ! %s %d.\n",
   gettext("Инвентарный номер"),in,
   gettext("не числится по дате"),mr,gr,
   gettext("Код результата поиска"),i);
   return(1);
  }   

class poiinpdw_data rekin;

if(metkabo == 1)
 {
  if((i=poiinpd(in,1,gr+1,&rekin)) != 0)
   {
    printw("%s %d %s %d.%d ! %s %d.\n",
    gettext("Инвентарный номер"),in,
    gettext("не найдено настройки на дату"),1,gr+1,
    gettext("Код результата поиска"),i);
    

   if(ff_prot != NULL)
     fprintf(ff_prot,"%s %d %s %d.%d ! %s %d.\n",
    gettext("Инвентарный номер"),in,
    gettext("не найдено настройки на дату"),1,gr+1,
    gettext("Код результата поиска"),i);

    return(1);
   }  
 }
else
if((i=poiinpd(in,mr,gr,&rekin)) != 0)
 {
  printw("%s %d %s %d.%d ! %s %d.\n",
  gettext("Инвентарный номер"),in,
  gettext("не найдено настройки на дату"),mr,gr,
  gettext("Код результата поиска"),i);

  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %d %s %d.%d ! %s %d.\n",
  gettext("Инвентарный номер"),in,
  gettext("не найдено настройки на дату"),mr,gr,
  gettext("Код результата поиска"),i);

  return(1);
 }  

if(grupa[0] != '\0')
if(proverka(grupa,rekin.hna.ravno(),0,0) != 0 || uosprovgr(rekin.hna.ravno()) != 0)
 {
  if(mra == 1)
   {
    printw("%s %s %s.\n",
    gettext("Инвентарный номер в группе"),rekin.hna.ravno(),
    gettext("(амортизация не считается)"));
    OSTANOV();
   }
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %s %s.\n",
  gettext("Инвентарный номер в группе"),rekin.hna.ravno(),
  gettext("(амортизация не считается)"));
  return(1);
 }

if(rekin.soso == 1 || rekin.soso == 2  )
 {


  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %d %s.\n",gettext("Инвентарный номер"),in,gettext("не эксплуатируется"));
  if(mra == 1)
   {
    sprintf(strsql,"%s %d %s.",gettext("Инвентарный номер"),in,gettext("не эксплуатируется"));
    iceb_t_soob(strsql);
   }
  amort_udzap(in,mr,gr,mkr);
  return(1);
//  goto vp;
 }

if(metkabo != 1)
  bsiz(in,podr,1,mr,gr,&bal_st,ff_prot);
else
  bsiz(in,podr,1,1,gr+1,&bal_st,ff_prot);

bal_st.bs+=bal_st.sbs-uosgetls(in,1,mr,gr,0);
bal_st.iz+=bal_st.siz;
bal_st.iz+=bal_st.iz1;

if(bal_st.bs <= bal_st.iz)
 {
  
  if(mra == 1)
   {
    printw("%s %d %s. %.2f <= %.2f\n",
    gettext("Инвентарный номер"),in,
    gettext("амортизирован полностью"),bal_st.bs,bal_st.iz);
    OSTANOV();
   }
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %d %s. bs=%.2f <= iz=%.2f\n",
  gettext("Инвентарный номер"),in,
  gettext("амортизирован полностью"),bal_st.bs,bal_st.iz);
 }

/*Читаем коэффициент*/
sprintf(strsql,"select kof from Uosgrup where kod='%s'",rekin.hna.ravno());
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  if(mra == 1)
   {
    printw("%s %d - %s %s !\n",
    gettext("Инвентарный номер"),in,
    gettext("не нашли кода группы"),rekin.hna.ravno());
    OSTANOV();
   }

  if(ff_prot != NULL)
    fprintf(ff_prot,"%s %d - %s %s !\n",
  gettext("Инвентарный номер"),in,
  gettext("не нашли кода группы"),rekin.hna.ravno());
  return(1);
 }

kof=atof(row[0]);
/*
printw("bs=%f iz=%f popkf=%f kof=%f nemi=%f\n",bs,iz,uos.popkf,kof,nemi);
OSTANOV();    
*/

if(metkabo == 1)
 {
  amortbo(in,podr,kodotl,kof,gr,bal_st.bs,bal_st.iz,mra,&rekin);
  return(0);
 }

if(uosprovar(rekin.hna.ravno(),0) == 1) /*Амортизация от остаточной балансовой стоимости*/
 {
/**********
  if(kvrt == 0)
   {
    return(1);
   }   
***********/
  if(SRAV(rekin.hna.ravno(),"1",0) == 0 && bal_st.bs-bal_st.iz <= 100*nemi)
   {
    amort[1]=bal_st.bs-bal_st.iz;
   }
  else
   {      
    
    amort[0]=(bal_st.bs-bal_st.iz)*rekin.popkf*3*kof/100/12.;

    if(ff_prot != NULL)
      fprintf(ff_prot,"%s=(%.2f-%.2f)*%.2f/100/12*3*%.2f=%.2f\n",
    gettext("Амортизация"),
    bal_st.bs,bal_st.iz,kof,rekin.popkf,amort[0]);
    if(mra == 1)
     {
      sprintf(strsql,"%s\n(%.2f-%.2f)*%.2f/100/12*3*%.2f=%.2f",
      gettext("Амортизация"),
      bal_st.bs,bal_st.iz,kof,rekin.popkf,amort[0]);
      iceb_t_soob(strsql);
     }

    amort[0]=okrug(amort[0],0.01);
    amort[1]=amort[0]/3;
    amort[1]=okrug(amort[1],0.01);
    amort[2]=amort[1];
    amort[3]=amort[0]-amort[1]-amort[2];


   }
 }
else  /*Амортизация от начальной балансовой стоимости*/
 {
/************
  if(kvrt != 0 )
   {
    return(1); 
   }
************/
  amort[1]=bal_st.bs*rekin.popkf*kof/100/12.;
  if(amort[1] > bal_st.bs-bal_st.iz)
    amort[1]=bal_st.bs-bal_st.iz;
  amort[0]=okrug(amort[0],0.01);
  if(mra == 1)
   {
    sprintf(strsql,"%s\n%.2f*%.2f/100/12.*%.2f=%.2f",
    gettext("Амортизация"),bal_st.bs,kof,rekin.popkf,amort[1]);

    iceb_t_soob(strsql);
   }
 }

if(mra == 1 && bal_st.bs == 0.)
 {
  printw("%d-%s\n",in,gettext("Балансовая стоимость равна нолю !"));
  if(ff_prot != NULL)
    fprintf(ff_prot,"%d-%s\n",in,gettext("Балансовая стоимость равна нолю !"));
  OSTANOV();
 }

 
if(bal_st.iz + amort[0] > bal_st.bs)
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


amort_udzap(in,mr,gr,mkr);

printw("%s: %5.2f %s: %.2f\n",
gettext("Амортизация"),amort[1],
gettext("Балансовая стоимость"),bal_st.bs);

if(ff_prot != NULL)
 fprintf(ff_prot,"%s: %5.2f %s: %.2f\n",
gettext("Амортизация"),amort[1],
gettext("Балансовая стоимость"),bal_st.bs);

time(&vrem);

i1=1;
 for(i=mr ; i <= mkr; i++)
  {
   sprintf(strsql,"insert into Uosamor \
values (%d,'%04d-%02d-01',%d,'%s','%s',%.2f,%d,%ld,%d)",
   in,gr,i,podr,rekin.hzt.ravno(),rekin.hna.ravno(),amort[i1++],iceb_t_getuid(),vrem,kodotl);

   sql_zapis(strsql,1,0);
  }

return(0);
}
/*************************/
/*Амортизация для бюджетных организаций*/
/***************************************/

void amortbo(int in,
int podr,
int kodotl,
double kof,
short gr,
double bs,
double iz,
short mra, //0-расчёт в потоке 1-нет 2-без вывода на экран
class poiinpdw_data *rekin)
{
time_t  vrem;
char	strsql[1024];
SQL_str row;
class SQLCURSOR cur;
int kol_mes=12;

/*Если остновное средство получено в течение года то нужно знать количество месяцев амортизации*/
sprintf(strsql,"select datd from Uosdok1 where innom=%d and podt=1 and tipz=1 order by datd desc limit 1",in);
if(readkey(strsql,&row,&cur) == 1)
 {
  short dd=0,md=0,gd=0;
  rsdat(&dd,&md,&gd,row[0],2);
  if(gd == gr)
   {
    kol_mes=period(dd,md,gd,1,12,gr,1);
   }
 } 

double amort=0.;

if(kol_mes == 12)
 amort=bs*rekin->popkf/100*kof;
else
 amort=bs*rekin->popkf/100*kof/12*kol_mes;

if(mra == 1)
 {
  if(kol_mes == 12)
   sprintf(strsql,"%.2f*%.6g/100*%.6g=%.2f",bs,rekin->popkf,kof,amort);
  else
   sprintf(strsql,"%.2f*%.6g/100*%.6g/12*%d=%.2f",bs,rekin->popkf,kof,kol_mes,amort);
  
  iceb_t_soob(strsql);
 }

if(amort > bs-iz)
 {
  if(mra == 1)
   {
    sprintf(strsql,"%s %.2f > %.2f\n%s",gettext("Сумма амортизации больше остаточной стоимости"),amort,bs-iz,gettext("Берём в расчёт остаточную стоимость"));
  
    iceb_t_soob(strsql);
   }
  amort=bs-iz;
 }
amort=okrug(amort,1.); //Округляем до гривны

/*Удаляем старую запись если она есть*/
sprintf(strsql,"delete from Uosamor where innom=%d and da >= '%04d-01-01' and da <= '%04d-12-31'",in,gr,gr);

sql_zapis(strsql,1,0);

time(&vrem);

sprintf(strsql,"insert into Uosamor \
values (%d,'%04d-12-01',%d,'%s','%s',%.2f,%d,%ld,%d)",
in,gr,podr,rekin->hzt.ravno(),rekin->hna.ravno(),amort,iceb_t_getuid(),vrem,kodotl);

sql_zapis(strsql,0,0);

}
/*******************************/
/*Удаление */
/*******************************/
void amort_udzap(int in,short mr,short gr,short mkr)
{
char strsql[512];
/*Удаляем старую запись если она есть*/
 for(int i=mr ; i <= mkr; i++)
  {
   sprintf(strsql,"delete from Uosamor where innom=%d and da='%04d-%02d-01'",in,gr,i);
   sql_zapis(strsql,1,0);
  }

}
