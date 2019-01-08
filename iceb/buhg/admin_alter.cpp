/*$Id: admin_alter.c,v 5.9 2014/06/30 06:36:34 sasa Exp $*/
/*08.02.2012	09.07.2010	Белых А.И.	admin_alter.c
Преобразование баз данных под новую версию системы
*/
#define DVERSIQ "20.04.2018"
#include <errno.h>
#include "buhg.h"

int admin_alter_r(const char *imabazz,int kolvsehbaz,const char *put_na_alx,const char *put_na_doc);

extern char *putnansi;

void admin_alter()
{
char strsql[512];
class iceb_tu_str imabazz("");
static class iceb_tu_str put_na_alx("/usr/share/iceB/alx");
static class iceb_tu_str put_na_doc("/usr/share/iceB/doc");
char bros[512];
int par=0;
/***********
put_na_alx.new_plus(".");
put_na_alx.plus(G_DIR_SEPARATOR_S,"bx");

put_na_doc.new_plus(".");
put_na_doc.plus(G_DIR_SEPARATOR_S,"doc");
**************/


VVOD VVOD1(2);
VVOD DANET(1);

DANET.VVOD_SPISOK_add_MD(gettext("Выполнить преобразование всех баз?"));
VVOD  VV(0);
sprintf(strsql,"%s 9.0",gettext("Программа преобразования базы для новой версии системы от версии"));
VV.VVOD_SPISOK_add_ZAG(strsql);

sprintf(bros,"%s %s от %s",gettext("Версия"),VERSION,DVERSIQ);
VV.VVOD_SPISOK_add_ZAG(bros);

sprintf(bros,"%s:%s",gettext("Путь на файлы настроек"),putnansi);
VV.VVOD_SPISOK_add_ZAG(bros);

VV.VVOD_SPISOK_add_MD(gettext("Имя базы................................"));
VV.VVOD_SPISOK_add_MD(gettext("Откуда импортировать настроечные файлы.."));
VV.VVOD_SPISOK_add_MD(gettext("Откуда импортировать документацию......."));

VV.VVOD_SPISOK_add_data(32);
VV.VVOD_SPISOK_add_data(put_na_alx.ravno(),112);
VV.VVOD_SPISOK_add_data(put_na_doc.ravno(),112);


naz:;


clear();
mvprintw(LINES-2,0,gettext("Если имя базы не введено, то преобразованы будут все доступные базы"));  
helstr(LINES-1,0,
"F2/+","преоб-зовать",
"F10","выход",NULL);

par=VV.vvod(0,1,1,-1,-1);

switch(par)
 {
  case FK1:
   goto naz;

  case FK10:
   return;

  case PLU:
  case FK2:
   break;


  default:
    goto naz;
 }

imabazz.new_plus(VV.data_ravno(0));
put_na_alx.new_plus(VV.data_ravno(1));
put_na_doc.new_plus(VV.data_ravno(2));

if(imabazz.getdlinna() <= 1)
 if(danet(&DANET,2,stdscr) == 2)
     goto naz;


/*Проверяем возможность открытия настроечного файла*/   
class iceb_tu_str imafilnast(put_na_alx.ravno());
imafilnast.plus(G_DIR_SEPARATOR_S,"nastsys.alx");
 

FILE *ff;
if((ff = fopen(imafilnast.ravno(),"r")) == NULL)
 {
  
  error_op_nfil(imafilnast.ravno(),errno,gettext("Неправильно указан путь на настроечные файлы!"));
  goto naz;
 }
fclose(ff);
 

/*Проверяем возможность открытия файла с документцией*/   
imafilnast.new_plus(put_na_doc.ravno());
imafilnast.plus(G_DIR_SEPARATOR_S,"buhg.txt");

if((ff = fopen(imafilnast.ravno(),"r")) == NULL)
 {
  
  error_op_nfil(imafilnast.ravno(),errno,gettext("Неправильно указан путь на документцию!"));
  goto naz;
 }
fclose(ff);

if(imabazz.getdlinna() > 1)
 {
  if(admin_alter_r(imabazz.ravno(),1,put_na_alx.ravno(),put_na_doc.ravno()) != 0)
   goto naz;
 }
else
 {
  GDITE();
  class SQLCURSOR cur,cur1;
  SQL_str row,row1;
  int kolstr=0;
  if((kolstr=cur.make_cursor(&bd,"SHOW DATABASES")) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),"SHOW DATABASES");
    return;
   }

  while(cur.read_cursor(&row) != 0)
   {
   
    sprintf(strsql,"select naikon from %s.Kontragent where kodkon='00'",row[0]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1) //именно так чтобы не показывала не нужных сообщений
      continue;

    admin_alter_r(row[0],kolstr,put_na_alx.ravno(),put_na_doc.ravno());

   }    
 }
iceb_t_soob(gettext("Преобразование завершено"));

}
