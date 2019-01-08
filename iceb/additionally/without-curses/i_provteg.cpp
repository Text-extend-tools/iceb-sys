/*$Id:$*/
/*18.04.2016	18.04.2016	Белых А.И.	i_provteg.c
Проверка парности тегов
*/

#include <errno.h>
#include        <locale.h>
#include        <sys/stat.h>
#include "bezcurses.h"


int main(int argc,char **argv,char **envp)
{
FILE *ff;
struct stat work;
setlocale(LC_ALL,"");/*Устанавливает переменные определяющие локаль окружения*/
printf("Проверка парности тегов\n");

if(argc == 1) /*Запуск без ключей*/
 {
  return(1); 
                    
 }
printf("Имя файла-%s\n",argv[1]);

if(stat(argv[1],&work) != 0)
 {
  printf("%s-%s %s!\n",__FUNCTION__,gettext("Нет такого файла"),argv[1]);
  return(1);
 }


if((ff = fopen(argv[1],"r")) == NULL)
 {
  printf("%s-%s %s:%d !\n",__FUNCTION__,gettext("Ошибка окрытия файла"),argv[1],errno);
  return(1);
 }
class iceb_tu_str stroka("");
char buf1[1024];
char buf2[1024];


int nom_buf1=0;
int nom_buf2=0;
int metka_start1=0;
int metka_start2=0;
int nomer_stroki=0;
while(iceb_tu_fgets(&stroka,ff) == 0)
 {
  nomer_stroki++;
  memset(buf1,'\0',sizeof(buf1));
  memset(buf2,'\0',sizeof(buf2));
  metka_start1=metka_start2=0;
  nom_buf1=nom_buf2=0;

  for(int nom=0; nom < stroka.getdlinna(); nom++)
   {
    if(stroka.ravno()[nom] == '<' && metka_start1 == 0)
     {
      metka_start1=1;
      continue;
     }

    if(stroka.ravno()[nom] == ' ' && stroka.ravno()[nom+1] == 'R' && metka_start1 == 1)
     {
      metka_start1=2;
      continue;
      
     }
    if(stroka.ravno()[nom] == '>' && metka_start1 == 1)
     {
      metka_start1=2;
      continue;
     }

    if(stroka.ravno()[nom] == '<'&& stroka.ravno()[nom+1] == '/' && metka_start1 == 2)
     {
      nom++;
      metka_start2=1;
      continue;
     }
    if(stroka.ravno()[nom] == '>' && metka_start2 == 1)
     {
      metka_start2=2;
      break;
     }

    if(metka_start1 == 1)
     buf1[nom_buf1++]=stroka.ravno()[nom];

    if(metka_start2 == 1)
     buf2[nom_buf2++]=stroka.ravno()[nom];


   }

  if(metka_start1 == 2 && metka_start2 == 2)
   {
    if(SRAV(buf1,buf2,0) != 0)
     {
      printf("%5d %s",nomer_stroki,stroka.ravno());
      printf("Тег %s не закрыт %s\n",buf1,buf2);
     }
      
   }            
  
 }

fclose(ff);



return(0);
}
