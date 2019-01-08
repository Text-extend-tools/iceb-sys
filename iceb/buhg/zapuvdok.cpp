/* $Id: zapuvdok.c,v 5.21 2013/05/17 14:56:16 sasa Exp $ */
/*04.02.2015	19.04.2000	Белых А.И.	zapuvdok.c
Запис в документ услуг
Если вернули 0 -записали
             1 -нет
*/
#include	"buhg.h"

extern double	okrcn;  /*Округление цены*/

int zapuvdok(int tp, //1-приход 2-расход
const char *datdok, //Дата документа
const char *nomd,
int metka,int kodzap,double kolih,double cena,const char *ei,const char *shetu,
int podr,
const char *shet_sp, //счёт списания/приобретения
const char *dop_naim) //дополнение к наименованию услуги
{
short dd=0,md=0,gd=0;
rsdat(&dd,&md,&gd,datdok,1);
return(zapuvdok(tp,dd,md,gd,nomd,metka,kodzap,kolih,cena,ei,shetu,podr,shet_sp,dop_naim));
}

/***************************************************/

int zapuvdok(int tp, //1-приход 2-расход
short dd,short md,short gd, //Дата документа
const char *nomd,
int metka,int kodzap,double kolih,double cena,const char *ei,const char *shetu,
int podr,
const char *shet_sp, //счёт списания/приобретения
const char *dop_naim) //дополнение к наименованию услуги
{
time_t 		vrem;
char		strsql[1024];

cena=okrug(cena,okrcn);
time(&vrem);
int nomzap=0;
for(;;)
 {
  sprintf(strsql,"select tp from Usldokum1 where datd='%04d-%02d-%02d' \
and podr=%d and tp=%d and nomd='%s' and metka=%d and kodzap=%d and \
nz=%d",gd,md,dd,podr,tp,nomd,metka,kodzap,nomzap);   

  if(readkey(strsql) >= 1)
   {
    
      nomzap++;
      continue;
   }   
  sprintf(strsql,"insert into Usldokum1 \
  values (%d,'%04d-%02d-%02d','%s',%d,%d,%.10g,%.10g,'%s','%s',%d,%d,%ld,'%s',%d,'%s')",
  tp,gd,md,dd,nomd,metka,kodzap,kolih,cena,ei,shetu,podr,iceb_t_getuid(),vrem,dop_naim,nomzap,shet_sp);

  if(sql_zap(&bd,strsql) != 0)
   {
    if(sql_nerror(&bd) == ER_DUP_ENTRY) //Запись уже есть
     {
      VVOD SOOB(1);
      SOOB.VVOD_SPISOK_add_MD(gettext("Такая запись уже есть !"));
//      SOOB.VVOD_SPISOK_add_MD(strsql);
      soobshw(&SOOB,stdscr,-1,-1,0,1);
      
     }
    else
      msql_error(&bd,__FUNCTION__,strsql);
    return(1);
   }
  break;
 }
return(0);
}
