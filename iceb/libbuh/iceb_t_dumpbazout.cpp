/*$Id: iceb_t_dumpbazout.c,v 5.6 2014/05/07 10:57:29 sasa Exp $*/
/*02.07.2015	24.08.2012	Белых А.И.	iceb_t_dumpbazout.c
Выгрузка дампа базы данных
*/
#include <errno.h>
#include <sys/stat.h>
#include "buhl.h"

void iceb_t_dumpbazout(const char *namebases,const char *login,const char *parol,const char *hostname)
{
char strsql[1024];
class iceb_tu_str ima_arh("");





sprintf(strsql,"%s %s\n%s",gettext("Выгрузка дампа базы данных"),namebases,gettext("Введите пароль"));

helstr(LINES-1,0,
"FK2",gettext("ввод"),
"F10",gettext("выход"),NULL);

if(parolv(strsql,0) != 0)
 return;
/****************************************
helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
     
class VVOD VVOD1(2);
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите путь на каталог для выгрузки дампа"));

if(vvod1(&ima_arh,128,&VVOD1,NULL,stdscr,-1,-1) == FK10)
  return;
*********************************/
class iceb_tu_str put_na_nastr(g_get_home_dir());
put_na_nastr.plus("/");
put_na_nastr.plus(ICEB_KAT_ALX,"/lno.alx");

//put_na_nastr.plus("/.iceB/lno.alx");

poldan("Путь на каталог для выгрузки дампа базы",&ima_arh,put_na_nastr.ravno());
time_t time_start=time(NULL); 

clear();
GDITE();

if(ima_arh.ravno()[0] == '\0')
  ima_arh.new_plus(iceb_tu_getcurrentdir());

ima_arh.plus("/mysqlbaza_");

if(namebases[0] != '\0')
 {
  ima_arh.plus(namebases);
  printw("%s %s %s %s\n",gettext("Выгружаем дамп базы данных"),namebases,gettext("в файл"),ima_arh.ravno());
  sprintf(strsql,"mysqldump -u %s -p%s --databases %s --lock-tables -h %s > %s",
  login,parol,namebases,hostname,ima_arh.ravno());
 }
else
 {
  ima_arh.plus("all");
  printw("%s %s %s\n",gettext("Выгружаем дамп всех баз данных"),gettext("в файл"),ima_arh.ravno());
  sprintf(strsql,"mysqldump -u %s -p%s --all-databases --lock-tables -h %s > %s",
  login,parol,hostname,ima_arh.ravno());
 }  

refresh();
FILE *ff;
if((ff = fopen(ima_arh.ravno(),"w")) == NULL)
 {
  error_op_nfil(ima_arh.ravno(),errno,"");
  return;
 }
fclose(ff);
unlink(ima_arh.ravno());


if(iceb_t_system(strsql,0) != 0)
 return;

struct stat bufstat;
char razmerfil[128];
memset(razmerfil,'\0',sizeof(razmerfil));

memset(razmerfil,'\0',sizeof(razmerfil));
if(stat(ima_arh.ravno(),&bufstat) == 0)
 {
  if(bufstat.st_size > 1000000)
   {
    double razm=bufstat.st_size/1000000.;
    razm=okrug(razm,0.01);
    sprintf(razmerfil,"%.2f %s",razm,gettext("мегабайт"));
   }
  else
   sprintf(razmerfil,"%ld %s",bufstat.st_size,gettext("байт"));
 }    
printw("%s %s %s\n",gettext("Размер файла дампа"),ima_arh.ravno(),razmerfil);

printw("%s %s %s bzip2\n",gettext("Сжимаем файл"),ima_arh.ravno(),gettext("программой"));
refresh();

sprintf(strsql,"bzip2 -f %s",ima_arh.ravno());
class iceb_tu_str ima_arh_bz2(ima_arh.ravno());
int metka_zip=0;
if(iceb_t_system(strsql,0) == 0)
 {
  metka_zip=1;   
  printw("\n%s %s.bz2\n",gettext("Дамп базы выгружен в файл"),ima_arh.ravno());
  ima_arh_bz2.plus(".bz2");
 }
else
 {
  printw("\n%s %s\n",gettext("Дамп базы выгружен в файл"),ima_arh.ravno());
 }
refresh();

if(metka_zip == 1)
 {
  memset(razmerfil,'\0',sizeof(razmerfil));
  if(stat(ima_arh_bz2.ravno(),&bufstat) == 0)
   {
    if(bufstat.st_size > 1000000)
     {
      double razm=bufstat.st_size/1000000.;
      razm=okrug(razm,0.01);
      sprintf(razmerfil,"%.2f %s",razm,gettext("мегабайт"));
     }
    else
     sprintf(razmerfil,"%ld %s",bufstat.st_size,gettext("байт"));
   }    
 }
printw("%s %s\n",gettext("Размер файла"),razmerfil);
printw_vr(time_start);
OSTANOV();

}
