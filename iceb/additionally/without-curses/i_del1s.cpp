/*$Id:$*/
/*05.02.2016	05.02.2016	Белых А.И.	i_del1s.c
Программа удаляет первый символ во всех строках файла
*/
#define VERSION "1.0"
#define DATA_VERSION "09.02.2016"
#include <errno.h>
#include        <locale.h>
#include "bezcurses.h"


int main(int argc,char **argv,char **envp)
{
setlocale(LC_ALL,"");/*Устанавливает переменные определяющие локаль окружения*/

printf("%s\n%s.\n\
%s %s %s %s %s\n",
argv[0],
gettext("Программа удаления первого символа в всех строках файла"),
gettext("Версия"),
VERSION,
gettext("от"),
DATA_VERSION,
gettext("г."));

if(argc == 1) /*Запуск без ключей*/
 return(1);

char stroka[4096];
char strbros[1024];
sprintf(strbros,"i_del1s%d.tmp",getpid());

printf("%s\n",argv[1]);
class iceb_tu_str imaf(argv[1]);
class iceb_tu_str imaftmp(strbros);


if(imaf.getdlinna() <= 1)
 {
  printf("%s\n",gettext("Не задано имя файла!"));
  return(2); 
 }

FILE *ff;
FILE *ff_tmp;
if((ff = fopen(imaf.ravno(),"r")) == NULL)
 {
  printf("%s-%s %s %d !",__FUNCTION__,gettext("Ошибка окрытия файла"),imaf.ravno(),errno);
  return(3);
 }
if((ff_tmp = fopen(imaftmp.ravno(),"w")) == NULL)
 {
  printf("%s-%s %s %d !",__FUNCTION__,gettext("Ошибка окрытия файла"),imaftmp.ravno(),errno);
  return(4);
 }

/*использовать iceb_u_fgets нельзя так как она будет сразу складывать строки а нам нужно удалить только первый пробел до складывания строк*/
while(fgets(stroka,sizeof(stroka),ff) != NULL)
 {
  fprintf(ff_tmp,"%s",iceb_tu_adrsimv(1,stroka));
 }
 
fclose(ff);
fclose(ff_tmp);

unlink(imaf.ravno());

rename(imaftmp.ravno(),imaf.ravno());

return(0);

}
