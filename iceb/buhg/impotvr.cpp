/*$Id: impotvr.c,v 1.29 2014/07/31 07:09:16 sasa Exp $*/
/*13.02.2016	29.01.2002	Белых А.И. 	impotvr.c
Импорт отработанного времени из файла в расчёт зарплаты
*/
#include        <errno.h>
#include        <sys/stat.h>
#include        "buhg.h"


void	impotvr(short mes,short god)
{
struct stat work;
char		strsql[512];
SQL_str		row;
SQLCURSOR       curr;
static class iceb_tu_str imafim("");
static char	imafprom[40];
int		kom=0;
int		razmer=0;
FILE		*ff,*ffprom;
class iceb_tu_str strok("");
class iceb_tu_str bros("");
int		tabnom=0; //Табельный номер
int		kodtb=0;  //Код табеля
float		kolrd=0.; //Количество рабочих дней
float		kolrh=0.;  //Количество рабочих часов
float 		kolkd=0.;  //Количество календарных дней
float		mkolrd=0.; //Количество рабочих дней в месяце
float		mkolrh=0.;  //Количество рабочих часов в месяце

VVOD VVOD1(2);
clear();
VVOD1.VVOD_SPISOK_add_ZAG(gettext("Импорт отработанного времени из файла"));
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите имя файла"));

naz:;

clear();

helstr(LINES-1,0,
"F1",gettext("помощь"),
"F2/+",gettext("загруз."),
"F3",gettext("просмотр"),
"F4",gettext("очистить"),
"F5",gettext("настроить"),
"F6",gettext("выполнить"),
"F10",gettext("выход"),NULL);

if((kom=vvod1(&imafim,112,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
  return;
if(kom == ESC)
  return;

switch(kom)
 {
  case FK1:
    GDITE();
    iceb_t_pdoc("zarp2_6_2.txt");
    clear();
    goto naz;

  case ENTER:
  case FK2:
  case PLU:
    if(imafim.ravno()[0] == '\0')
     {
      dirkatl(&imafim);
      goto naz;
     }
    break;

  case FK3:
   GDITE();
   if(stat(imafim.ravno(),&work) != 0)
    {
     sprintf(strsql,"%s !",gettext("Нет такого файла"));
     iceb_t_soob(strsql);
     goto naz;
    }
    vizred(imafim.ravno());
    goto naz;

 case FK4 : /*очистить*/
    imafim.new_plus("");
    goto naz;

 case FK5 : /*настройка для выполнения */
    iceb_redfil("zarimpot.alx",0);
    goto naz;

 case FK6 : /*Запустить программу*/
    iceb_t_fsystem("zarimpot.alx",imafim.ravno());
    goto naz;

  default:
    goto naz;
 }


clear();
move(LINES-1,0);
GDITE();
if(stat(imafim.ravno(),&work) != 0)
 {
  sprintf(strsql,"%s !",gettext("Нет такого файла"));
  iceb_t_soob(strsql);
  goto naz;
 }


if((ff = fopen(imafim.ravno(),"r")) == NULL)
 {
  sprintf(strsql,"%s %s:%d !",
  gettext("Ошибка окрытия файла"),imafim.ravno(),errno);
  iceb_t_soob(strsql);
  goto naz;
 }
sprintf(imafprom,"prom%d.tmp",getpid());
if((ffprom = fopen(imafprom,"w")) == NULL)
 {
  sprintf(strsql,"%s %s:%d !",
  gettext("Ошибка окрытия файла"),imafprom,errno);
  iceb_t_soob(strsql);
  goto naz;
 }

class iceb_tu_str tabnom_str("");
razmer=0;
while(iceb_tu_fgets(&strok,ff) == 0)
 {
  razmer+=strlen(strok.ravno());
  if(strok.ravno()[0] == '#')
   continue;

  printw("%s",strok.ravno());
  strzag(LINES-1,0,work.st_size,razmer);

  if(polen(strok.ravno(),&bros,1,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }
  if(bros.ravno()[0] == '"') /*Для Госстандарта*/
   tabnom_str.new_plus(iceb_tu_adrsimv(1,bros.ravno()));
  else 
   tabnom_str.new_plus(bros.ravno());
   
  if((tabnom=tabnom_str.ravno_atoi()) == 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }
    
  sprintf(strsql,"select tabn from Kartb where tabn=%d",tabnom);
  if(sql_readkey(&bd,strsql,&row,&curr) <= 0)
   {

    sprintf(strsql,"%s:%d",gettext("Не найден табельный номер"),tabnom);
    iceb_t_soob(strsql);

    fprintf(ffprom,"%s",strok.ravno());
    fprintf(ffprom,"%s:%d\n",\
    gettext("Не найден табельный номер"),tabnom);
    continue;
   }

  if(polen(strok.ravno(),&kodtb,2,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  sprintf(strsql,"select kod from Tabel where kod=%d",kodtb);
  if(sql_readkey(&bd,strsql,&row,&curr) <= 0)
   {

    sprintf(strsql,"%s:%d",gettext("Не найден код табеля"),kodtb);
    iceb_t_soob(strsql);

    fprintf(ffprom,"%s",strok.ravno());
    fprintf(ffprom,"%s:%d\n",\
    gettext("Не найден код табеля"),kodtb);
    continue;
   }

  sprintf(strsql,"select god from Ztab where tabn=%d and god=%d and \
mes=%d and kodt=%d and nomz=%d",tabnom,god,mes,kodtb,0);
  if(sql_readkey(&bd,strsql,&row,&curr) > 0)
   {
    iceb_t_soob(gettext("Такая запись уже есть !"));

    fprintf(ffprom,"%s",strok.ravno());
    fprintf(ffprom,"%s\n",gettext("Такая запись уже есть !"));
    continue;
   }

  //Проверяем блокировку карточки
  sprintf(strsql,"select blok from Zarn where god=%d and mes=%d and \
tabn=%d",god,mes,tabnom);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
   if(atoi(row[0]) != 0)
    {
     sprintf(strsql,"%s !",gettext("Карточка заблокирована"));
     iceb_t_soob(strsql);

     fprintf(ffprom,"%s",strok.ravno());
     fprintf(ffprom,"%s !\n",gettext("Карточка заблокирована"));
     continue;
    }
   

  if(polen(strok.ravno(),&kolrd,3,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  if(polen(strok.ravno(),&kolrh,4,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  if(polen(strok.ravno(),&kolkd,5,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  if(polen(strok.ravno(),&mkolrd,6,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  if(polen(strok.ravno(),&mkolrh,7,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  zaptabel(0,mes,god,tabnom,kodtb,kolrd,kolrh,kolkd,0,0,0,0,0,0,mkolrd,mkolrh,"",0,0);

 }
strzag(LINES-1,0,work.st_size,work.st_size);

fclose(ff);
fclose(ffprom);
unlink(imafim.ravno());

rename(imafprom,imafim.ravno());

}
