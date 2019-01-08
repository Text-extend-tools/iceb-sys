/*$Id: iceb_t_system.c,v 5.12 2012-11-29 09:49:04 sasa Exp $*/
/*31.10.2012	10.09.2007	Белых А.И.	iceb_t_system.c
Запуск программы, чтение и вывод на экран её вывод
*/
#include "buhl.h"

int iceb_t_system(const char *komanda,int metkav) //0-вывод на экран 1-без вывода
{
char stroka[1024];
class SPISOK spis_strok;
FILE *ff;

if((ff=popen(komanda,"r")) == NULL)
 {
  printw("%s-%s:%s\n",__FUNCTION__,gettext("Ошибка открытия канала"),komanda);
  return(1);
 }
int metka_1str=0;
while(fgets(stroka,sizeof(stroka),ff) != NULL)
 {
  if(metkav == 0)
   {
    if(metka_1str == 0)
     printw("\n"); 
    printw("%s",stroka);
    refresh();
    metka_1str++;
   }
  spis_strok.plus(stroka);
 }

int voz=pclose(ff);

//if(WIFEXITED(voz) != TRUE)
if(voz != 0)
 {
  
  if(metkav == 1)
   {
    for(int ii=0 ; ii < spis_strok.kolih(); ii++)
     printw("%s",spis_strok.ravno(ii));
   }  
  printw("\n%s-%s:\n%s\n",__FUNCTION__,gettext("Ошибка при выполнении команды"),komanda);
  OSTANOV();
 }  
return(voz);

}
