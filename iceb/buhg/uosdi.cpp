/*$Id: uosdi.c,v 5.3 2013/05/17 14:56:13 sasa Exp $*/
/*15.05.2017	04.01.2012	Белых А.И.	uosdi.c
Загрузка/выгрузка дополнительной информации
*/
#include <errno.h>
#include "buhg.h"

int uosdi(char mr, //+ загрузить - выгрузить
int md,
int innom,
const char *imaf) //имя файла
{
char strok[2048];
FILE *ff;
char strsql[2048];
if(mr == '+')
 {
  int nomz=0;

  if((ff = fopen(imaf,"r")) == NULL)
   {
     if(errno == ENOENT)
      return(0);
     else
      {
       error_op_nfil(imaf,errno,"");
       return(1);
      }
   }

  class lock_tables kkk("LOCK TABLES Uosin1 WRITE");
  sprintf(strsql,"delete from Uosin1 where innom=%d and md=%d",innom,md);
  sql_zapis(strsql,1,0);

  while(fgets(strok,sizeof(strok),ff) != NULL)
   {
    sprintf(strsql,"insert into Uosin1 \
values (%d,%d,'%s',%d)",innom,++nomz,sql_escape_string(strok,0),md);
    sql_zapis(strsql,1,0);
    
   }  
  fclose(ff);
  unlink(imaf);
  return(0);
 }

if(mr == '-')
 {
  if((ff = fopen(imaf,"w")) == NULL)
    {
     error_op_nfil(imaf,errno,"");
     return(1);
    }
  int kolstr=0;  
  sprintf(strsql,"select zapis from Uosin1 where innom=%d and md=%d order by nomz asc",innom,md);
  SQL_str row;
  SQLCURSOR cur;  
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,__FUNCTION__,strsql);
    return(1);
   }

  if(kolstr == 0)
   {
    if(md == 1)
      iceb_t_insfil("uosoz6end.alx",ff);
    if(md == 2)
      iceb_t_insfil("uosik_end.alx",ff);
   } 
  else
   while(cur.read_cursor(&row) != 0)
    fprintf(ff,"%s",row[0]);


  fclose(ff);
  return(0);
 }


return(1);

}