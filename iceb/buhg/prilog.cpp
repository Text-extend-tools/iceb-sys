/* $Id: prilog.c,v 5.10 2013/05/17 14:56:07 sasa Exp $ */
/*15.07.2015	07.11.2000	Белих А.И.	prilog.c
Загрузка-выгрузка придожения к счету
Если вернули 0-порядок
         
*/
#include        <errno.h>
#include        "buhg.h"


int prilog(char mr, //+ загрузить - выгрузить
int podr, //подразделение
short god,  //год
short tipz, //1-приход 2-расход
const char nomd[],  //Номер документа
const char imaf[], //имя файла
const char tabl[]) //Имя таблицы
{
int		nomz;
char		strok[1024];
char		strsql[512];
SQL_str  	row;
FILE		*ff;
int		kolstr,i;

/*Загрузка из файла в базу данных*/

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

  sql_zapis(strsql,1,0);

  sprintf(strsql,"delete from %s where podr=%d and god=%d and tp=%d \
and nomd='%s'",tabl,podr,god,tipz,nomd);

  sql_zapis(strsql,1,0);

  nomz=0;
  while(fgets(strok,sizeof(strok),ff) != NULL)
   {
    i=strlen(strok);
    if(strok[i-1] == '\n')
      strok[i-1]='\0';
    sqlfiltr(strok,sizeof(strok));          
    sprintf(strsql,"insert into %s \
values (%d,%d,%d,'%s',%d,'%s')",tabl,podr,god,tipz,nomd,++nomz,strok);

    sql_zapis(strsql,1,0);
    
   }
  sprintf(strsql,"UNLOCK TABLES");

  sql_zapis(strsql,1,0);

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
  
  sprintf(strsql,"select zapis from %s where podr=%d \
and god=%d and tp=%d and nomd='%s'",tabl,podr,god,tipz,nomd);
  /*
  printw("\n%s\n",strsql);
  refresh();
  */
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
