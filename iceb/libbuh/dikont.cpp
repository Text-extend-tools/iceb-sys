/*$Id: dikont.c,v 5.23 2013/08/13 05:50:22 sasa Exp $*/
/*19.07.2013	24.11.2000	Белых А.И.	dikont.c
Ввод дополнительной информации к коду
*/
#include        <errno.h>
#include	"buhl.h"


int dikont(char mr, //+ загрузить - выгрузить
const char *imaf, //имя файла
const char *kod,
const char *tabl, //Имя таблицы
const char *metka)
{
int		nomz;
char		strok[2048];
char		strsql[512];
SQL_str  	row;
FILE		*ff;
int		kolstr,i;
/*
printw("\ndikont-%c %s %s %s\n",mr,imaf,kod,tabl);
refresh();
*/
if(mr == '+')
 {

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

  sprintf(strsql,"LOCK TABLES %s WRITE",tabl);
  if(sql_zap(&bd,strsql) != 0)
   {
    sprintf(strsql,"%s-%s\n%s\n",__FUNCTION__,gettext("Ошибка блокировки таблицы !"),sql_error(&bd));
    iceb_t_soob(strsql);
   }

  if(SRAV(tabl,"Kontragent1",0) == 0)
    sprintf(strsql,"delete from %s where kodkon='%s'",tabl,kod);

  if(SRAV(tabl,"Kartb1",0) == 0)
    sprintf(strsql,"delete from %s where tabn=%s",tabl,kod);

  if(sql_zap(&bd,strsql) != 0)
   {
    msql_error(&bd,__FUNCTION__,strsql);
   }
  nomz=0;
  while(fgets(strok,sizeof(strok),ff) != NULL)
   {
    i=strlen(strok);
    if(strok[i-1] == '\n')
      strok[i-1]='\0';
    
    sqlfiltr(strok,sizeof(strok));
              
    if(SRAV(tabl,"Kontragent1",0) == 0)
      sprintf(strsql,"insert into %s \
values ('%s',%d,'%s')",tabl,kod,++nomz,strok);


    if(SRAV(tabl,"Kartb1",0) == 0)
      sprintf(strsql,"insert into %s \
values (%s,%d,'%s')",tabl,kod,++nomz,strok);
    
    if(sql_zap(&bd,strsql) != 0)
     {
      msql_error(&bd,__FUNCTION__,strsql);
     }
   }
  sprintf(strsql,"UNLOCK TABLES");
  if(sql_zap(&bd,strsql) != 0)
   {
    msql_error(&bd,__FUNCTION__,strsql);
   }

  fclose(ff);
  unlink(imaf);
  return(0);
 }

//Выгрузка из базы в файл

if(mr == '-')
 {
  if((ff = fopen(imaf,"w")) == NULL)
    {
     error_op_nfil(imaf,errno,"");
     return(1);
    }
  
  if(SRAV(tabl,"Kontragent1",0) == 0)
    sprintf(strsql,"select zapis from %s where kodkon='%s'",
    tabl,kod);

  if(SRAV(tabl,"Uosin1",0) == 0)
    sprintf(strsql,"select zapis from %s where innom=%s",
    tabl,kod);

  if(SRAV(tabl,"Kartb1",0) == 0)
    sprintf(strsql,"select zapis from %s where tabn=%s",
    tabl,kod);

//  printw("\n%s\n",strsql);
//  refresh();
  SQLCURSOR cur;  
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    return(1);
   }
  if(kolstr == 0)
   {
    fclose(ff);
    return(1);
   }  


  while(cur.read_cursor(&row) != 0)
    fprintf(ff,"%s\n",row[0]);


  fclose(ff);
  return(0);
 }

return(1);
}
