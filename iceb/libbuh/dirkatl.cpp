/*$Id: dirkatl.c,v 5.13 2013-04-07 10:22:54 sasa Exp $*/
/*27.03.2013	27.04.2001	Белых А.И.	dirkatl.c
Просмотр текущего каталога файлов с целью выбора нужного
файла
Если вернули 0 - выбрали файл
             1 - нет
*/
#include        <sys/stat.h>
#include	"buhl.h"


int dirkatl(class iceb_tu_str *imafil)
{
struct  tm      *bf;
struct stat work;
class SPISOK	masfil; /*Массив с именами файлов*/
int		kolfil=0;
int		prc;
class iceb_tu_str name("");
int		K=0;
char		metkafil[6];
class iceb_t_mstr spis_menu(0);
char		bros[1024];
char imaf_v_menu[1024];
class iceb_tu_str poisk("");
class iceb_tu_str imaf_tmp("");

class VVOD VVOD1(2);
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите образец для поиска"));

naz1:;

clear();

masfil.free_class();

kolfil=dirfajl("*",0,&masfil);

if(kolfil == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одного файла !"));
  return(1);
 }

spis_menu.spis_str.free_class();
int ns=0;
for(int ii=0 ; ii < kolfil ; ii++)
 {
  name.new_plus(masfil.ravno(ii));  
  if(stat(name.ravno(),&work) != 0)
   {
//    beep();
//    printw("\nИмя файла - %s\n",name.ravno());
//    OSTANOV();
    return(1);
   }
  if(poisk.getdlinna() > 1)
   {
    if(strstrm(name.ravno(),poisk.ravno()) == 0)
     continue;
   }


  memset(metkafil,'\0',sizeof(metkafil));
  switch(work.st_mode & S_IFMT)
   {
    case S_IFREG: //Обычный файл
      break;
    case S_IFDIR: //Каталог
      strcpy(metkafil,"/");
      break;
   }
  bf=localtime(&work.st_mtime);

  sprintf(imaf_v_menu,"%s%s",metkafil,name.ravno()); /*чтобы был одинаковым размер поля с именем файла в меню - с / и без него*/
  
  memset(bros,'\0',sizeof(bros));
  
  sprintf(bros,"%-*s|%8ld|%02d:%02d|%02d.%02d.%d",
  iceb_tu_kolbait(30,imaf_v_menu),imaf_v_menu,
  work.st_size,bf->tm_hour,
  bf->tm_min,bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900);

  ns++;

  spis_menu.spis_str.plus(bros);

 }


naz:;

prc=100;

helstr(LINES-1,0,
//"F1",gettext("помощь"),
"FK4",gettext("поиск"),
"F10",gettext("выход"),NULL);

if(poisk.getdlinna() > 1)
 {
  short		y=2;
  short         x=COLS-20;
  mvprintw(y++,x,gettext("Поиск:"));
  mvprintw(y++,x,"%s",poisk.ravno());
 }

int nomer=spis_menu.menu(0,0,0,NULL,bros,prc,&K,iceb_tu_getcurrentdir(),0);


switch(K)
 {
  case ENTER: 
    polen(spis_menu.spis_str.ravno(nomer),&imaf_tmp,1,'|');
     

    if(imaf_tmp.ravno()[0] == '/')
     {
      memset(bros,'\0',sizeof(bros));
      sprintf(bros,"%s",iceb_tu_adrsimv(1,imaf_tmp.ravno()));

      if(chdir(bros) != 0)
       {
        iceb_t_soob("Не могу сменить каталог !");
       }
      goto naz1;
     }
    imafil->new_plus(imaf_tmp.ravno());
    return(0);

  case FK4:

     helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
     
     if(vvod1(&poisk,64,&VVOD1,NULL,stdscr,-1,-1) == FK10)
       poisk.new_plus("");
     clear();
   
   goto naz1;
   
  case FK10: 
  case ESC: 
    imafil->new_plus("");
    return(1);  

  default:
    goto naz;
 }

}
