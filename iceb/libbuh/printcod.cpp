/*$Id: printcod.c,v 5.10 2013-04-07 10:22:54 sasa Exp $*/
/*27.03.2013	24.03.2000	Белых А.И.  printcod.c
Распечатка списка кодов в файл
*/
#include	"buhl.h"

void printcod(FILE *ff,
const char *tabl,    //Имя таблицы
const char *naimkod, //Наименование поля по которому надо искать
const char *naimp,   //Наименование поля в таблице, которое надо найти
int metka,      //0-код цифровой 1-символьный
const char *spisok,  //Список параметров по которым надо искать
int *kolstr)   //Количество распечатанных строчек
{
SQL_str         row;
SQLCURSOR       cur;
char		strsql[512];
int		kolkod,i;
char		kod[32];

*kolstr=0;
if(spisok[0] == '\0')
 return;

kolkod=pole2(spisok,',');

if(kolkod > 0)
 for(i=1; i <= kolkod; i++)
  {
   polen(spisok,kod,sizeof(kod),i,',');
   if(kod[0] == '\0')
    break;
   if(metka == 0)
    sprintf(strsql,"select %s from %s where %s=%s",
    naimp,tabl,naimkod,kod);
   if(metka == 1)
    sprintf(strsql,"select %s from %s where %s='%s'",
    naimp,tabl,naimkod,kod);

   if(sql_readkey(&bd,strsql,&row,&cur) != 1)
     fprintf(ff,"%s-%s %s %s !\n",__FUNCTION__,gettext("Код"),kod,gettext("не найден"));
   else
     fprintf(ff,"%s %s\n",kod,row[0]);
   *kolstr+=1;
  }

if(kolkod == 0 && spisok[0] != '\0')
 {
  memset(kod,'\0',sizeof(kod));
  strncpy(kod,spisok,sizeof(kod)-1);
  if(metka == 0)
   sprintf(strsql,"select %s from %s where %s=%s",
   naimp,tabl,naimkod,kod);
  if(metka == 1)
   sprintf(strsql,"select %s from %s where %s='%s'",
   naimp,tabl,naimkod,kod);

  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
    fprintf(ff,"%s-%s %s %s !\n",__FUNCTION__,gettext("Код"),kod,gettext("не найден"));
  else
    fprintf(ff,"%s %s\n",kod,row[0]);
  *kolstr+=1;
 }
}
