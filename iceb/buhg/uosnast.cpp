/* $Id: uosnast.c,v 5.32 2013/09/26 09:43:42 sasa Exp $ */
/*16.07.2015    08.04.1996      Белых А.И.   	uosnast.c
Чтение настpойки для учета основных сpедств
*/
#include        <errno.h>
#include        "buhg.h"

float           nemi; /*Необлагаемый минимум*/
short		startgoduos=0; /*Стартовый год для Учёта основных средств*/
short		startgodb; /*Стартовый год просмотров главной книги*/
short		vplsh; /*0-двух-порядковый план счетов 1-многопорядковый*/
char		*bnds1=NULL; /*0% НДС реализация на територии Украины.*/
char            *bnds2=NULL; /*0% НДС экспорт.*/
char            *bnds3=NULL; /*0% Освобождение от НДС статья 5.*/
short		metkabo=0; //Если 1 то организация бюджетная
double okrg1=1.;
double okrcn=1.;

short		startgod=0; /*Стартовый год просмотров в Материальном учёте*/
short           mfnn=-1; /*Метка формирования номера накладной для материального учёта
                      -1 - Автоматическая нумерация выключена
                       0 - Приходы и расходы - нумерация общая
                       1 - Приходы отдельно расходы отдельно
                       */

int            uosnast()
{
class iceb_tu_str st1("");
class iceb_tu_str bros("");
int		i;
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

metkabo=vplsh=0;
nemi=0.;
mfnn=(-1);
startgod=0;
if(bnds1 != NULL)
 {
  delete [] bnds1;
  bnds1=NULL;
 }

if(bnds2 != NULL)
 {
  delete [] bnds2;
  bnds2=NULL;
 }

if(bnds3 != NULL)
 {
  delete [] bnds3;
  bnds3=NULL;
 }

sprintf(strsql,"select str from Alx where fil='uosnast.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"uosnast.alx");
  iceb_t_soob(strsql);
  return(1);
 }



while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
   
  polen(row_alx[0],&st1,1,'|');

  if(SRAV(st1.ravno(),"Бюджетная организация",0) == 0)
   {
    polen(row_alx[0],&st1,2,'|');
    if(SRAV(st1.ravno(),"Вкл",1) == 0)
     metkabo=1;
    continue;
   }
  if(SRAV(st1.ravno(),"Необлагаемый минимум",0) == 0)
   {
    polen(row_alx[0],&nemi,2,'|');
//    nemi=(float)ATOFM(st1);
    continue;
   }

  if(SRAV(st1.ravno(),"Стартовый год",0) == 0)
   {
    polen(row_alx[0],&startgoduos,2,'|');
//    startgoduos=(short)ATOFM(st1);
    continue;
   }

  if(SRAV(st1.ravno(),"0% НДС реализация на територии Украины",0) == 0)
    {
     polen(row_alx[0],&bros,2,'|');
     i=strlen(bros.ravno())+1;
     if((bnds1=new char[i*sizeof(char)]) == NULL)
      {
       sprintf(strsql,"%s-%s bnds1 !",__FUNCTION__,gettext("Не могу выделить память для"));
       iceb_t_soob(strsql);
      }
     else
       strcpy(bnds1,bros.ravno());
     continue;
    }

  if(SRAV(st1.ravno(),"0% НДС экспорт",0) == 0)
    {
     polen(row_alx[0],&bros,2,'|');
     i=strlen(bros.ravno())+1;
     if((bnds2=new char[i*sizeof(char)]) == NULL)
      {
       sprintf(strsql,"%s-%s bnds2 !",__FUNCTION__,gettext("Не могу выделить память для"));
       iceb_t_soob(strsql);
      }
     else
      strcpy(bnds2,bros.ravno());
     continue;
    }

  if(SRAV(st1.ravno(),"0% Освобождение от НДС статья 5",0) == 0)
    {
     polen(row_alx[0],&bros,2,'|');
     i=strlen(bros.ravno())+1;
     if((bnds3=new char[i*sizeof(char)]) == NULL)
      {
       sprintf(strsql,"%s-%s bnds3 !",__FUNCTION__,gettext("Не могу выделить память для"));
       iceb_t_soob(strsql);
      }
     else
       strcpy(bnds3,bros.ravno());
     continue;
    }

 }



iceb_t_poldan("Многопорядковый план счетов",&st1,"nastrb.alx");
if(SRAV(st1.ravno(),"Вкл",1) == 0)
 vplsh=1;

if(iceb_t_poldan("Округление суммы",&st1,"matnast.alx") == 0)
  okrg1=st1.ravno_atof();
if(iceb_t_poldan("Округление цены",&st1,"matnast.alx") == 0)
  okrcn=st1.ravno_atof();
if(iceb_t_poldan("Стартовый год",&st1,"matnast.alx") == 0)
  startgod=st1.ravno_atoi();
if(iceb_t_poldan("Раздельная нумерация накладных",&st1,"matnast.alx") == 0)
 {
  mfnn=0;

  if(st1.ravno()[0] == '\0')
   mfnn=-1;

  if(SRAV(st1.ravno(),"Вкл",1) == 0)
    mfnn=1;
 }
return(0);
}
