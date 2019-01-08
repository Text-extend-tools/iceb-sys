/* $Id: prpuos.c,v 5.16 2013/08/13 05:49:51 sasa Exp $ */
/*24.05.2013    07.04.1997      Белых А.И.      prpuos.c
Программа просмотра проводок и определения не сделанных
В учете основных средств
*/
#include        <math.h>
#include        "buhg.h"
#include "poiinpd.h"

extern double   okrg1; /*Округление*/

extern struct M2 LL;

void prpuos(short d,short m,short g, //Дата документа
const char *ndk, //Номер документа
int podd, //Код подразделения
class SPISOK *spsh,
class masiv_din_double *sn,
class masiv_din_double *sp,
short mr) /*0-Проверить выполнены ли проводки
            1-составить список проводок которые нужно сделать
            2-распечатку на экран
          */
{
short           mpi=0; /*0 - удалить отметку >0 записать отметку*/
class iceb_tu_str sh1("");
short           xv=54,yv=1;
double          bb;
short           mop=0; /*0- нет проводок >0 -есть*/
int             i;
long            in;
double		pbs;
long		kolstr;
SQL_str         row,row1;
char		strsql[1024];
short		tipz;
int		X=COLS-26,Y=1;

if(mr == 2)
 {
  xv=X; yv=Y;
  bb=0.;
  for(i=0; i < spsh->kolih() ; i++)
   {
    move(yv++,xv);
    printw("%-8s %14s",spsh->ravno(i),prnbr(sn->ravno(i)));
    bb+=sn->ravno(i);
   }
  move(yv++,xv);
  printw("%-8s %14s",gettext("Итого"),prnbr(bb));
  refresh();
  return;
 }


if(mr == 1)
 {
  sprintf(strsql,"select innom,bs,bsby from Uosdok1 where \
datd='%d-%d-%d' and nomd='%s'",g,m,d,ndk);
  class SQLCURSOR cur;
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    return;
   }
//  ksh=0;
  while(cur.read_cursor(&row) != 0)
   {

    in=atol(row[0]);
    pbs=atof(row[2]);
    if(pbs == 0.)
      pbs=atof(row[1]);
    class poiinpdw_data rekin;     
    /*Читаем переменные данные*/
//    poiinpd(in,m,g,shetu,hzt,hau,hna,&popkf,&soso,nomz,&mz,&gz,hnaby,&popkfby);
    poiinpd(in,m,g,&rekin);


    move(14,0);

    int nomer=0;
    if((nomer=spsh->find(rekin.shetu.ravno())) < 0)
     spsh->plus(rekin.shetu.ravno());
    sn->plus(pbs,nomer);
    sp->plus(0.,nomer);

   }
  return;
 }

/******************************************/
  /*Ищем проводки*/
if( mr == 0 )
 {
  class SQLCURSOR cur;  
  
  /*Читаем шапку документа*/
  sprintf(strsql,"select tipz from Uosdok where \
datd='%d-%d-%d' and nomd='%s'",g,m,d,ndk);

  if(readkey(strsql,&row1,&cur) != 1)
   {
    VVOD SOOB(1);
    sprintf(strsql,"prpuos-%s %s %d.%d.%d%s",
    gettext("Не найден документ !"),ndk,d,m,g,
    gettext("г."));
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    return;
   }
  else
   {
    tipz=atoi(row1[0]);
   }  

  move(14,0);
  mop=mpi=0;
  sprintf(strsql,"select sh,shk,deb,kre from Prov \
where kto='%s' and pod=%d and nomd='%s' and datd='%d-%d-%d'",
  ICEB_MP_UOS,podd,ndk,g,m,d);
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    return;
   }

  if(kolstr == 0)
   {
    goto vp1;
   }

  while(cur.read_cursor(&row) != 0)
   {
    if(fabs(atof(row[3])) > 0.009)
     continue;

    mop++;

    if(tipz == 1 )
      sh1.new_plus(row[0]);

    if(tipz == 2 )
      sh1.new_plus(row[1]);


    for(i=0; i<spsh->kolih() ;i++)
     {
      if(SRAV(spsh->ravno(i),sh1.ravno(),0) == 0)
       {
        sp->plus(atof(row[2]),i);
        break;
       }
     }
   }
  
  /***********************/
  VVOD SOOB(1);
  if( mop > 0 )
   {
    for(i=0; i < spsh->kolih() ; i++)
     {
      if(fabs(sp->ravno(i) - sn->ravno(i)) > 0.009)
       {
        beep();
        sprintf(strsql,gettext("По счету %s не сделаны все проводки ! %.14g != %.14g %d/%d"),
        spsh->ravno(i),sp->ravno(i),sn->ravno(i),i,spsh->kolih());
        mpi++;
        SOOB.VVOD_SPISOK_add_MD(strsql);
       }
     }
   }

  if(mpi > 0)
   {
    soobshw(&SOOB,stdscr,-1,-1,0,1);
   }
 }

vp1:;

if(mop == 0)
  mpi++;

if(mpi == 0 )
 {
  /*Проводки выполнены*/
  sprintf(strsql,"update Uosdok \
set prov=0 where datd='%d-%d-%d' and nomd='%s'",g,m,d,ndk);
 }
else
 {
  /*Делаем отметку о несделанных проводках*/
  sprintf(strsql,"update Uosdok \
set prov=1 where datd='%d-%d-%d' and nomd='%s'",g,m,d,ndk);

 }

if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_DBACCESS_DENIED_ERROR)
  {
   msql_error(&bd,__FUNCTION__,strsql);
  }

}
