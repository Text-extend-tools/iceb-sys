/* $Id: specnast.c,v 5.22 2013/11/05 10:50:49 sasa Exp $ */
/*03.10.2013    14.04.1997      Белых А.И.     specnast.c
Чтение настpойки для работы со спецификациями
*/
#include	"buhg.h"

SQL_baza	bd;

short           mfnn; /*Метка формирования номера накладной
                      -1 - Автоматическая нумерация выключена
                       0 - Приходы и расходы - нумерация общая
                       1 - Приходы отдельно расходы отдельно
                       */
short		startgodb;
short		startgod;
short		vplsh; /*0-двух уровневый план счетов 1-многоуровневый*/
//char            *mtsh=NULL;  /*Перечень материальных счетов*/
class iceb_tu_str spmatshets(""); /*список материальных счетов*/
double		okrcn;  /*Округление цены*/
double		okrg1;  /*Округление суммы*/
class iceb_tu_str shrt("");    /*Счета розничной торговли*/
double		kursue;/*Курс условной единицы*/
short		vtara; /*Код группы возвратная тара*/
short metka_pros_mat_s_ost=0; //0-показывать все материалы 1-только имеющие остаток (для расходных документов)
char		*kodopsp=NULL; //Коды операций сторнирования для приходов
char		*kodopsr=NULL; //Коды операций сторнирования для расходов

int		specnast(void)
{
class iceb_tu_str st1("");
class iceb_tu_str bros("");
int		i;
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

mfnn=(-1);

vplsh=startgodb=0;
okrcn=0.;
metka_pros_mat_s_ost=0;

sprintf(strsql,"select str from Alx where fil='matnast.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"matnast.alx");
  iceb_t_soob(strsql);
  return(1);
 }


vtara=startgod=0;
kursue=okrg1=0.;
shrt.new_plus("");
spmatshets.new_plus("");
/**************
if(mtsh != NULL)
 {
  free(mtsh);
  mtsh=NULL;
 }
***************/
if(kodopsp != NULL)
 {
  delete [] kodopsp;
  kodopsp=NULL;
 }
if(kodopsr != NULL)
 {
  delete [] kodopsp;
  kodopsp=NULL;
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  polen(row_alx[0],&st1,1,'|');

  if(SRAV(st1.ravno(),"Раздельная нумерация накладных",0) == 0)
    {
     mfnn=0;
     
     polen(row_alx[0],&st1,2,'|');
     if(st1.ravno()[0] == '\0' || st1.ravno()[0] == '\n')
      mfnn=(-1);
      
     if(SRAV(st1.ravno(),"Вкл",1) == 0)
       mfnn=1;
     continue;
    }
  if(SRAV(st1.ravno(),"Показывать только материалы имеющие остаток",0) == 0)
    {
     polen(row_alx[0],&st1,2,'|');
     if(SRAV(st1.ravno(),"Вкл",1) == 0)
        metka_pros_mat_s_ost=1;
     else
        metka_pros_mat_s_ost=0;
     
     continue;
    }

  if(SRAV(st1.ravno(),"Код группы возвратная тара",0) == 0)
    {
     polen(row_alx[0],&vtara,2,'|');
     continue;
    }
  if(SRAV(st1.ravno(),"Стартовый год",0) == 0)
    {
     polen(row_alx[0],&startgod,2,'|');
     continue;
    }
  if(SRAV(st1.ravno(),"Курс УЕ",0) == 0)
    {
     polen(row_alx[0],&kursue,2,'|');
     continue;
    }
/*********************
  if(SRAV(st1.ravno(),"Перечень материальных счетов",0) == 0)
    {
     polen(row_alx[0],&bros,2,'|');
     i=strlen(bros.ravno())+1;
     if((mtsh=(char *)malloc(i*sizeof(char))) == NULL)
      {
       sprintf(strsql,"%s-%s mtsh !",__FUNCTION__,gettext("Не могу выделить память для"));
       iceb_t_soob(strsql);
      }
     else
      strcpy(mtsh,bros.ravno());
     continue;
    }
******************/
  if(SRAV(st1.ravno(),"Перечень материальных счетов",0) == 0)
    {
     polen(row_alx[0],&spmatshets,2,'|');
     continue;
    }
  if(SRAV(st1.ravno(),"Счета розничной торговли",0) == 0)
    {
     polen(row_alx[0],&shrt,2,'|');
     continue;
    }

  if(SRAV(st1.ravno(),"Округление цены",0) == 0)
    {
     polen(row_alx[0],&okrcn,2,'|');
     continue;
    }

  if(SRAV(st1.ravno(),"Округление суммы",0) == 0)
    {
     polen(row_alx[0],&okrg1,2,'|');
     continue;
    }

  if(SRAV(st1.ravno(),"Коды операций сторнирования для приходов",0) == 0)
   {
     polen(row_alx[0],&bros,2,'|');
     i=strlen(bros.ravno())+1;
     if((kodopsp=new char[i*sizeof(char)]) == NULL)
      {
       sprintf(strsql,"%s-%s kodopsp !",__FUNCTION__,gettext("Не могу выделить память для"));
       iceb_t_soob(strsql);
      }
     else
       strcpy(kodopsp,bros.ravno());
     continue;
   }
  if(SRAV(st1.ravno(),"Коды операций сторнирования для расходов",0) == 0)
   {
     polen(row_alx[0],&bros,2,'|');
     i=strlen(bros.ravno())+1;
     if((kodopsr=new char[i*sizeof(char)]) == NULL)
      {
       sprintf(strsql,"%s-%s kodopsr !",__FUNCTION__,gettext("Не могу выделить память для"));
       iceb_t_soob(strsql);
      }
     else
       strcpy(kodopsr,bros.ravno());
     continue;
   }

 }

iceb_t_poldan("Многопорядковый план счетов",&bros,"nastrb.alx");
if(SRAV(bros.ravno(),"Вкл",1) == 0)
 vplsh=1;

startgodb=0;
if(iceb_t_poldan("Стартовый год",&bros,"nastrb.alx") == 0)
 {
  startgodb=bros.ravno_atoi();
 }
 
return(0);

}
