/*$Id:$*/
/*01.08.2013	28.12.2010	Белых А.И.	c_to_cpp.cpp

*/
#include <glib.h>
#include <errno.h>
#include <util.h>


int main(int argc,char **argv)
{
printf("переименование файлов\n");
GDir *dirp;
if((dirp=g_dir_open("./",0,NULL)) == NULL)
 {

  printf("Немогу открыть каталог!\n%s\n",strerror(errno));
  return(1);
 }
const char *imafil_read;
class iceb_tu_str rashir("");
char stroka[1024];
while((imafil_read=g_dir_read_name(dirp)) != NULL)
 {
  if(polen(imafil_read,&rashir,2,'.') != 0)
   continue;

  if(SRAV("c",rashir.ravno(),0) != 0)
   continue;

  sprintf(stroka,"%spp",imafil_read);  
  printf("Файл-%s -> %s\n",imafil_read,stroka);
//  rename(imafil_read,stroka);

 }

g_dir_close(dirp);

return(0);
}

