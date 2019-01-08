/* $Id: msql_error.c,v 5.12 2013/05/17 14:56:31 sasa Exp $ */
/*18.05.2012	06.03.2001	Белых А.И.	msql_error.c
Меню вывода сообщения об ощибке базы данных
Две подпрограммы одна выводит на экран другая в файл
*/
#include        "buhl.h"

void msql_error(SQL_baza *bd,const char *soobsh,const char *zapros,WINDOW *win)
{
char	strsql[1024];
char	bros[1024];
int kolstr=0;

memset(strsql,'\0',sizeof(strsql));
//sprintf(strsql,"%s %d",soobsh,sql_nerror(bd));
VVOD SOOB(1);

if(soobsh[0] != '\0')
 {
  sprintf(bros,"%.*s",iceb_tu_kolbait(COLS-4,soobsh),soobsh);  

  SOOB.VVOD_SPISOK_add_MD(bros);
  kolstr++;
 } 
memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"%d %s",sql_nerror(bd),sql_error(bd));
if(strsql[0] != '\0')
 {
  sprintf(bros,"%.*s",iceb_tu_kolbait(COLS-4,strsql),strsql);  
  SOOB.VVOD_SPISOK_add_MD(bros);
  kolstr++;
 }
if(iceb_tu_strlen(strsql) >= COLS-4)
 {
//  sprintf(bros,"%.*s",COLS-4,&strsql[COLS-4]);  
  sprintf(bros,"%.*s",iceb_tu_kolbait(COLS-4,iceb_tu_adrsimv(COLS-4,strsql)),iceb_tu_adrsimv(COLS-4,strsql));  
  SOOB.VVOD_SPISOK_add_MD(bros);
 }
if(zapros[0] != '\0')
 {
  for(int nom=0; iceb_tu_strlen(zapros) > nom; nom+=COLS-4)
   {
    kolstr++;
    if(kolstr > LINES-4)
     break;
    
    sprintf(bros,"%.*s",iceb_tu_kolbait(COLS-4,iceb_tu_adrsimv(nom,zapros)),iceb_tu_adrsimv(nom,zapros));  
    SOOB.VVOD_SPISOK_add_MD(bros);
   } 
 }
soobshw(&SOOB,win,-1,-1,0,1);

}

/**************/
/*Вывод в файл*/
/**************/

void	msql_error(SQL_baza *bd,const char *soobsh,const char *zapros,FILE *ff)
{
if(ff == NULL)
 return;

if(soobsh[0] != '\0') 
 fprintf(ff,"%s\n",soobsh);

fprintf(ff,"%d %s\n",sql_nerror(bd),sql_error(bd));

fprintf(ff,"%s\n",zapros);

}

void msql_error(SQL_baza *bd,const char *soobsh,const char *zapros)
{
 msql_error(bd,soobsh,zapros,stdscr);
}
