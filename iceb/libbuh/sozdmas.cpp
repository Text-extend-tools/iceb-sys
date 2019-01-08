/* $Id: sozdmas.c,v 5.1 2013/09/26 09:43:55 sasa Exp $ */
/*02.04.2010	12.11.1998	Белых А.И.	sozdmas.c
Создание и расширение массива
*/
#include	<scr.h>


void sozdmas(char *obr,char **maskmat,long *dlmas,char razd)
{
long		dlmat;
int		i;
dlmat=strlen(obr)+1;
if(*maskmat == NULL)
 {

  *dlmas=dlmat+1; 
/* +1 Чтобы иметь возможность прибавить разделитель при необходимости
   если будет всего одно поле*/

  if((*maskmat=(char *)malloc(*dlmas*sizeof(char))) == NULL)
   {
    printw("\n%s-Не могу выделить пямять для maskmat !\n",__FUNCTION__);
    refresh();
   }
  strcpy(*maskmat,obr);
 }
else
 {
  i=strlen(*maskmat);
  if(i+1+dlmat > *dlmas)
   {
    *dlmas=*dlmas+(i+1+dlmat - *dlmas);
    if((*maskmat=(char *)realloc(*maskmat,*dlmas*sizeof(char))) == NULL)
     {
      printw("\n%s-Не могу добавить память для maskmat !\n",__FUNCTION__);
      refresh();
     } 	
   } 
  (*maskmat)[i]=razd;
  (*maskmat)[i+1]='\0';
  strcat(*maskmat,obr);
 }    
}
