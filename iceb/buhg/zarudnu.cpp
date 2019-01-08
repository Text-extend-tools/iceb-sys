/* $Id: zarudnu.c,v 5.13 2013-04-07 10:22:45 sasa Exp $ */
/*30.10.2006	11.05.2001	Белых А.И.	zarudnu.c
Удаление начисления/удержания с проверкой удаления из списка
настройки
Если вернули 0-удалили
             1-не удалили
*/
#include        "buhg.h"


int zarudnu(short gp,short mp,short dz,long tabn,
short nahud,short kodna,short gz,short mz,int podr,const char *shet,short nz)
{
char	strsql[512];

sprintf(strsql,"delete from Zarp where datz='%d-%02d-%02d' and \
tabn=%ld and prn='%d' and knah=%d and godn=%d and mesn=%d and \
podr=%d and shet='%s' and nomz=%d",
gp,mp,dz,tabn,nahud,kodna,gz,mz,podr,shet,nz);
    
//printw("\nstrsql=%s\n",strsql);
//OSTANOV();
if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Доступ только на чтение
  {
   iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
   return(1); 
  }
 else
  {
   msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);
   return(1);
  }
 }
sprintf(strsql,"select * from Zarp where datz >='%d-%02d-01' and \
datz <= '%d-%02d-31' and tabn=%ld and prn='%d' and knah=%d limit 1",
gp,mp,gp,mp,tabn,nahud,kodna);

/*Если нет ни одной записи удалять*/
if(readkey(strsql) == 0)
 {
  
  sprintf(strsql,"delete from Zarn1 where tabn=%ld and prn='%d' \
and knah=%d",tabn,nahud,kodna);
  
  /*    printw("\nstrsql=%s\n",strsql);*/
  if(sql_zap(&bd,strsql) != 0)
   {
    if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //У оператора доступ только на чтение
      return(1);
    else
      msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);
   }
 }
return(0);

}
