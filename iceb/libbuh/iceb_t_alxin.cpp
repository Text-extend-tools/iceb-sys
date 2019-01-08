/*$Id: iceb_t_alxin.c,v 5.5 2014/06/30 06:36:40 sasa Exp $*/
/*13.05.2014	07.09.2010	Белых А.И.	iceb_t_alxin.c
Загрузка файла alx в базу
*/
#include <errno.h>
#include "buhl.h"

void iceb_t_alxin(const char *imaf)
{
class iceb_tu_str stroka("");
gchar *strsql=NULL;
FILE *ff;
class iceb_tu_str imafz;
int nom_op=iceb_t_getuid();
time_t vrem=time(NULL);
int nomer_str=0;

int kolb=pole2(imaf,G_DIR_SEPARATOR);
if(kolb == 0)
 imafz.plus(imaf);
else
 polen(imaf,&imafz,kolb,G_DIR_SEPARATOR);  
if((ff=fopen(imaf,"r")) == NULL)
  {
   error_op_nfil(imaf,errno,"");
   return;
  }

class lock_tables kkk("LOCK TABLES Alx WRITE");

strsql=g_strdup_printf("delete from Alx where fil='%s'",imafz.ravno());
if(sql_zapis(strsql,1,0) != 0)
 {
  g_free(strsql);
  return;
 }
g_free(strsql);
//while(fgets(stroka,sizeof(stroka),ff) != NULL)
while(iceb_tu_fgets(&stroka,ff) == 0)
 {

  strsql=g_strdup_printf("insert into Alx values('%s',%d,%d,%ld,'%s')",imafz.ravno(),++nomer_str,nom_op,vrem,stroka.ravno_filtr());
  sql_zapis(strsql,1,0);
  g_free(strsql);
 }
fclose(ff);

}
