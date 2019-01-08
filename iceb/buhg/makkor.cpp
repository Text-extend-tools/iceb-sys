/* $Id: makkor.c,v 5.16 2013/05/17 14:56:04 sasa Exp $ */
/*15.07.2015	20.01.1999	Белых А.И.	makkor.c
Работа с файлом настройки для корректировки суммы по документа
Если все в норме возвращаем 0
                            1- ошибка
*/
#include        <errno.h>
#include	"buhg.h"


int makkor(VVOD *MENU,
short tr, //1-создать меню 2-взять настройку для выбранной корректировки
const char *naikor, //Наименование корректировки*/
double **maskor,
const char imaf[])
{
class iceb_tu_str br(""),br1("");
short		ns,i,i1;


SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf);
  iceb_t_soob(strsql);
  return(1);
 }


if(tr == 1)
 {
  ns=0;
  while(cur_alx.read_cursor(&row_alx) != 0)
   {
    if(row_alx[0][0] == '#')
     continue;
    if(SRAV(row_alx[0],"Наименование корректировки",1) == 0)
     {
      polen(row_alx[0],&br,2,'|');
      MENU->VVOD_SPISOK_add_MD(br.ravno());
      ns++;
     }
   }
 }

if(tr == 2)
 {
  while(cur_alx.read_cursor(&row_alx) != 0)
   {
    if(row_alx[0][0] == '#')
     continue;
    if(SRAV(row_alx[0],"Наименование корректировки",1) == 0)
     {
      polen(row_alx[0],&br,2,'|');
      if(SRAV(naikor,br.ravno(),0) == 0)
       {
        if((*maskor) != NULL)
         {
          free(*maskor);
          *maskor=NULL;
         }
        cur_alx.read_cursor(&row_alx);
        polen(row_alx[0],&br,2,'|');
        ns=pole2(br.ravno(),',');        
        if(ns == 0 && br.ravno()[0] != '\0')
         ns=1;
        
        if((*maskor=(double *)calloc(ns*2+1,sizeof(double))) == NULL)
         {
          sprintf(strsql,"%s-%s maskor !",__FUNCTION__,gettext("Не могу выделить память для"));
          iceb_t_soob(strsql);
          return(1);
         }
        (*maskor[0])=ns;
        for(i=1; i < ns+1; i++)
         {
          if(polen(br.ravno(),&br1,i,',') != 0)
            br1.new_plus(br.ravno()); 
          (*maskor)[i]=br1.ravno_atof();          
         }
        cur_alx.read_cursor(&row_alx);
        polen(row_alx[0],&br,2,'|');
        i1=1;
        for(i=ns+1; i < ns*2+1; i++)
         {
          if(polen(br.ravno(),&br1,i1,',') != 0)
            br1.new_plus(br.ravno()); 
          i1++;
          (*maskor)[i]=br1.ravno_atof();          
         }
       }
     }
    
   }
 }
return(0);
}
