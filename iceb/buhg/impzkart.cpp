/*$Id: impzkart.c,v 5.25 2014/07/31 07:09:16 sasa Exp $*/
/*13.02.2016	26.11.2002	Белых А.И.	impzkart.c
Загрузка списка работников в подсистему "Расчёт зарплаты
Только фио и идентификационный код
*/
#include        <errno.h>
#include        <sys/stat.h>
#include        "buhg.h"


void	impzkart(void)
{
struct stat work;
char		strsql[1024];
SQL_str		row;
SQLCURSOR curr;
static class iceb_tu_str imafim("");
static char imafprom[64];
int		kom=0;
int		razmer=0;
FILE		*ff,*ffprom;
class iceb_tu_str strok("");

VVOD VVOD1(2);
clear();
VVOD1.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0));
VVOD1.VVOD_SPISOK_add_ZAG(gettext("Импорт списка работников из файла"));
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите имя файла"));

naz:;

clear();

helstr(LINES-1,0,
"F1",gettext("помощь"),
"F2/+",gettext("загруз."),
"F3",gettext("просмотр"),
"F10",gettext("выход"),NULL);

if((kom=vvod1(&imafim,112,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
  return;
if(kom == ESC)
  return;

switch(kom)
 {
  case FK1:
    GDITE();
    iceb_t_pdoc("zarp2_10.txt");
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
  sprintf(strsql,"%s %s:%d !",gettext("Ошибка окрытия файла"),imafim.ravno(),errno);
  iceb_t_soob(strsql);
  goto naz;
 }
sprintf(imafprom,"prom%d.tmp",getpid());
if((ffprom = fopen(imafprom,"w")) == NULL)
 {
  sprintf(strsql,"%s %s:%d !",gettext("Ошибка окрытия файла"),imafprom,errno);
  iceb_t_soob(strsql);
  goto naz;
 }

class iceb_tu_str tabnom("");
class iceb_tu_str fam("");
class iceb_tu_str imq("");
class iceb_tu_str oth("");
class iceb_tu_str inn("");
class iceb_tu_str fio("");

int  kolkart=0;
razmer=0;
while(iceb_tu_fgets(&strok,ff) == 0)
 {
  razmer+=strlen(strok.ravno());
  if(strok.ravno()[0] == '#')
   continue;

  printw("%s",strok.ravno());
  strzag(LINES-1,0,work.st_size,razmer);

  if(polen(strok.ravno(),&tabnom,1,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  if(tabnom.ravno_atoi() == 0)
   {
    iceb_t_soob(gettext("Табельный номер не может равняться нолю !"));
    
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  //Проверяем на отсутсвие такого табельного номера в базе данных
  sprintf(strsql,"select fio from Kartb where tabn=%s",tabnom.ravno());
  if(sql_readkey(&bd,strsql,&row,&curr) > 0)
   {
    VVOD SOOB(1);
    sprintf(strsql,gettext("Табельный номер %s уже введён !"),tabnom.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);
    SOOB.VVOD_SPISOK_add_MD(row[0]);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    fprintf(ffprom,"%s\n#%s\n",strok.ravno(),strsql);
    continue;
   } 

  if(polen(strok.ravno(),&fam,2,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  if(polen(strok.ravno(),&imq,3,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  if(polen(strok.ravno(),&oth,4,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  if(polen(strok.ravno(),&inn,5,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  //Проверяем на отсутствие индивидуального налогового номера
  if(inn.ravno()[0] != 0)
   {
    sprintf(strsql,"select tabn,fio from Kartb where inn='%s'",inn.ravno());
    if(sql_readkey(&bd,strsql,&row,&curr) > 0)
     {
      VVOD SOOB(1);
      SOOB.VVOD_SPISOK_add_MD(gettext("Такой индентификационный код уже введён !"));
      sprintf(strsql,"%s %s",row[0],row[1]);
      SOOB.VVOD_SPISOK_add_MD(strsql);
      soobshw(&SOOB,stdscr,-1,-1,0,1);

      fprintf(ffprom,"%s\n#%s:%s\n",strok.ravno(),
      gettext("Такой индентификационный код уже введён !"),
      strsql);
      continue;
     } 
   }

  fio.new_plus(fam.ravno());
  fio.plus(" ",imq.ravno());
  fio.plus(" ",oth.ravno());
      
  sprintf(strsql,"insert into Kartb set tabn=%s, fio='%s', inn='%s'",
  tabnom.ravno(),fio.ravno_filtr(),inn.ravno());
  if(sql_zap(&bd,strsql) != 0)
   {
    msql_error(&bd,gettext("Ошибка записи !"),strsql);
    fprintf(ffprom,"%s\n#%s\n",strok.ravno(),gettext("Ошибка записи!"));
    
    continue;
   }    
  kolkart++;  
 }
strzag(LINES-1,0,work.st_size,work.st_size);

fclose(ff);
fclose(ffprom);
unlink(imafim.ravno());

rename(imafprom,imafim.ravno());

VVOD SOOB(1);
SOOB.VVOD_SPISOK_add_MD(gettext("Загрузка завершена"));
sprintf(strsql,"%s:%d",gettext("Количество загруженых карточек"),kolkart);
SOOB.VVOD_SPISOK_add_MD(strsql);
soobshw(&SOOB,stdscr,-1,-1,0,1);
}
