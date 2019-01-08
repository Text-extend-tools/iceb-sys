/*$Id: imp_ks.c,v 5.28 2014/07/31 07:09:16 sasa Exp $*/
/*13.02.2016	04.04.2004	Белых А.И.	imp_ks.c
Импорт карт-счетов в подсистему "Расчёт зарплаты"
*/
#include        <errno.h>
#include        <sys/stat.h>
#include        "buhg.h"

void imp_ukrsoc(char *imafim);



void imp_ks()
{
struct stat work;
int kom=0;
static class iceb_tu_str imafim(""); 
static char imafprom[64];
FILE		*ff,*ffprom;
char            strsql[512];
SQL_str row;
class SQLCURSOR cur;

VVOD VVOD1(2);
clear();
VVOD1.VVOD_SPISOK_add_ZAG(gettext("Импорт карт-счетов из файла"));
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
    iceb_t_pdoc("zarp2_13.txt");
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
int kod_banka=zarvbank();
if(kod_banka < 0)
 goto naz;
 
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

int razmer=0;
class iceb_tu_str strok("");
class iceb_tu_str inn("");
class iceb_tu_str kartshet("");
class iceb_tu_str fio("");
class iceb_tu_str imq("");
class iceb_tu_str oth("");

while(iceb_tu_fgets(&strok,ff) == 0)
 {
  razmer+=strlen(strok.ravno());

  if(strok.ravno()[0] == '#')
   continue;

  printw("%s",strok.ravno());
  strzag(LINES-1,0,work.st_size,razmer);

  if(polen(strok.ravno(),&inn,1,' ') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  //Ищем табельный номер
  sprintf(strsql,"select tabn from Kartb where inn='%s' order by tabn desc",inn.ravno());
  if(sql_readkey(&bd,strsql,&row,&cur) == 0)
   {
    polen(strok.ravno(),&fio,3,' ');
    polen(strok.ravno(),&imq,4,' ');
    polen(strok.ravno(),&oth,5,' ');

    sprintf(strsql,"select tabn,inn from Kartb where fio='%s %s %s'",
    fio.ravno_filtr(),
    imq.ravno_filtr(),
    oth.ravno_filtr());
    
    if(readkey(strsql,&row,&cur) != 1)
     {
      sprintf(strsql,"%s:%s",gettext("Не найден идентификационный код"),inn.ravno());
      iceb_t_soob(strsql);

      fprintf(ffprom,"%s",strok.ravno());
      continue;
     }
    if(row[1][0] != '\0')
     {
      sprintf(strsql,"%s:%s",gettext("Не найден идентификационный код"),inn.ravno());
      iceb_t_soob(strsql);

      fprintf(ffprom,"%s",strok.ravno());
      continue;
     }
    sprintf(strsql,"update Kartb set inn='%s' where tabn=%s",inn.ravno(),row[0]);
    printw("%s\n",strsql);  
    sql_zapis(strsql,1,0);
   }
  polen(strok.ravno(),&kartshet,2,' ');

  sprintf(strsql,"update Kartb set bankshet='%s',kb=%d where inn='%s'",kartshet.ravno(),kod_banka,inn.ravno());
//  printw("%s\n",strsql);  
  sql_zapis(strsql,1,0);

  sprintf(strsql,"replace into Zarkh values(%s,%d,'%s',%d,%ld)",row[0],kod_banka,kartshet.ravno(),iceb_t_getuid(),time(NULL));
  sql_zapis(strsql,1,0);
      
 }
strzag(LINES-1,0,work.st_size,work.st_size);

fclose(ff);
fclose(ffprom);
unlink(imafim.ravno());

rename(imafprom,imafim.ravno());

}
/*************************************/
/*Загрузка для Укрсоцбанка*/
/***************************/

void imp_ukrsoc(char *imafim)
{
struct stat work;
char strsql[512];
char	imafprom[64];


clear();
move(LINES-1,0);
GDITE();

if(stat(imafim,&work) != 0)
 {
  sprintf(strsql,"%s !",gettext("Нет такого файла"));
  iceb_t_soob(strsql);
  return;
 }

FILE *ff;
FILE *ffprom;

if((ff = fopen(imafim,"r")) == NULL)
 {
  sprintf(strsql,"%s %s:%d !",gettext("Ошибка окрытия файла"),imafim,errno);
  iceb_t_soob(strsql);
  return;
 }
sprintf(imafprom,"prom%d.tmp",getpid());
if((ffprom = fopen(imafprom,"w")) == NULL)
 {
  sprintf(strsql,"%s %s:%d !",gettext("Ошибка окрытия файла"),imafprom,errno);
  iceb_t_soob(strsql);
  return;
 }

int razmer=0;
class iceb_tu_str strok("");
int  i=0;
class iceb_tu_str inn("");
class iceb_tu_str kartshet("");
SQLCURSOR cur;
SQL_str   row;

while(iceb_tu_fgets(&strok,ff) == 0)
 {
  i=strlen(strok.ravno());
  razmer+=i;
  if(strok.ravno()[i-2] == '\r') //Возврат каретки меняем на пробел
   iceb_tu_vstav(&strok," ",i-2,i-1,1);
//   strok.ravno()[i-2]=' ';

  printw("%s",strok.ravno());
  strzag(LINES-1,0,work.st_size,razmer);

  if(polen(strok.ravno(),&inn,1,',') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  //Проверяем налоговый номер
  sprintf(strsql,"select inn from Katrb where inn='%s'",inn.ravno());
  if(sql_readkey(&bd,strsql,&row,&cur) < 1)
   {
    fprintf(ffprom,"%s",strok.ravno());
   }

  if(polen(strok.ravno(),&kartshet,8,',') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  sprintf(strsql,"update Kartb set bankshet='%s' where inn='%s'",
  kartshet.ravno(),inn.ravno());
  
  sql_zapis(strsql,1,0);
    
 }

fclose(ff);
fclose(ffprom);
unlink(imafim);

rename(imafprom,imafim);


}
