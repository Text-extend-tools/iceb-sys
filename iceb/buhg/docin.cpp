/*$Id: docin.c,v 5.6 2014/07/31 07:09:16 sasa Exp $*/
/*14.07.2015	22.09.2010	Белых А.И.	Белых А.И.	docin.c
Загрузка в таблицу документации
*/
#include <errno.h>
#include "buhg.h"

void docin(const char *put_na_doc,WINDOW *win)
{
int nomer_str=0;
class iceb_tu_str stroka("");
//char strsql[4096];
FILE *ff;

GDir *dirp;
if((dirp=g_dir_open(put_na_doc,0,NULL)) == NULL)
 {
  perror(__FUNCTION__);
  class iceb_tu_str repl;
  repl.plus(__FUNCTION__);
  repl.ps_plus(gettext("Не могу открыть каталог !"));
  repl.ps_plus(put_na_doc);
  repl.ps_plus(strerror(errno));
  iceb_t_soob(repl.ravno());
  
 }

if(sql_zapis("delete from Doc",1,0) != 0)
   return;

class iceb_tu_str imafil;
class iceb_tu_str rashir;
const char *imafil_read;

while((imafil_read=g_dir_read_name(dirp)) != NULL)
 {
  if(polen(imafil_read,&rashir,2,'.') != 0)
   continue;

  if(SRAV("txt",rashir.ravno(),0) != 0 && SRAV("TXT",rashir.ravno(),0) != 0)
   continue;

  
  imafil.new_plus(put_na_doc);
  imafil.plus(G_DIR_SEPARATOR_S,imafil_read);
//  wprintw(win,"Загрузка файла-%s\n",imafil.ravno());
  wprintw(win,".");
  wrefresh(win);

  if((ff=fopen(imafil.ravno(),"r")) == NULL)
    {
     error_op_nfil(imafil.ravno(),errno,"");
     return;
    }

  while(iceb_tu_fgets(&stroka,ff) == 0)
   {
    gchar *strsql=g_strdup_printf("insert into Doc values('%s',%d,'%s')",imafil_read,++nomer_str,stroka.ravno_filtr());

    sql_zapis(strsql,1,0);

    g_free(strsql);

   }
  fclose(ff);

 }
wprintw(win,"\n");
wrefresh(win);

g_dir_close(dirp);
}
