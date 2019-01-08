/*$Id: imp_zardok.c,v 5.20 2014/07/31 07:09:16 sasa Exp $*/
/*20.04.2016	04.01.2008	Белых А.И.	imp_zardok.c
Импорт записей в документ
*/
#include        <errno.h>
#include        <sys/stat.h>
#include        "buhg.h"




void imp_zardok(short prn,short dd,short md,short gd,const char *nomdok)
{
struct stat work;
int kom=0;
static class iceb_tu_str imafim("");
FILE		*ff;
char            strsql[512];

VVOD VVOD1(2);
clear();
if(prn == 1)
 VVOD1.VVOD_SPISOK_add_ZAG(gettext("Импорт начислений в документ"));
if(prn == 2)
 VVOD1.VVOD_SPISOK_add_ZAG(gettext("Импорт удержаний в документ"));
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
    iceb_t_pdoc("zarp2_12_3.txt");
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
char imafprom[64];
sprintf(imafprom,"prom%d.tmp",getpid());
FILE *ffprom;
if((ffprom = fopen(imafprom,"w")) == NULL)
 {
  sprintf(strsql,"%s %s:%d !",
  gettext("Ошибка окрытия файла"),imafprom,errno);
  iceb_t_soob(strsql);
  goto naz;
 }
clear();

class iceb_tu_str strok("");
int razmer=0;
int tabnom=0;
int knah=0;
double suma=0.;
SQL_str row;
class SQLCURSOR cur;
class iceb_tu_str shet("");
int podr=0;
class ZARP     zp;
int nomzap=0;
int voz=0;
short dnr,mnr,gnr;
short dkr,mkr,gkr;
class iceb_tu_str shetkar("");
class iceb_tu_str dolg("");
int kateg=0;
int sovm=0;
class iceb_tu_str lgoti("");
int zvan=0;
class iceb_tu_str koment("");

while(iceb_tu_fgets(&strok,ff) == 0)
 {
  razmer+=strlen(strok.ravno());

  if(strok.ravno()[0] == '#')
   continue;


  printw("%s",strok.ravno());
  
  strzag(LINES-1,0,work.st_size,razmer);
  
  if(polen(strok.ravno(),&tabnom,1,'|') != 0)
   continue;
  
  if(tabnom == 0)
   {
    fprintf(ffprom,"%s\n%s\n",strok.ravno(),"Табельный номер равен нолю");
    continue;
   }
  /*Проверяем табельный номер и узнаём код подразделения,счёт в карточке*/
  sprintf(strsql,"select dolg,zvan,podr,kateg,datn,datk,shet,sovm,lgoti from Kartb where tabn=%d",tabnom);
  if(readkey(strsql,&row,&cur) != 1)
   {
   
    printw("%s %d !\n",gettext("Не найден табельный номер"),tabnom);
    OSTANOV();    
    fprintf(ffprom,"%s\n%s\n",strok.ravno(),gettext("Не найден табельный номер"));
    continue;
    
   }
  
  shet.new_plus(row[6]);
  shetkar.new_plus(row[6]);
  podr=atoi(row[2]);
  kateg=atoi(row[3]);
  sovm=atoi(row[7]);
  lgoti.new_plus(row[8]);
  dolg.new_plus(row[0]);
  rsdat(&dnr,&mnr,&gnr,row[4],2);
  rsdat(&dkr,&mkr,&gkr,row[5],2);
  zvan=atoi(row[1]);
      
  polen(strok.ravno(),&knah,2,'|');
  if(knah == 0)
   {
    if(prn == 1)
     {
      printw("Код начисления равен нолю!\n");
      fprintf(ffprom,"%s\nКод начисления равен нолю!\n",strok.ravno());
     }
    if(prn == 2)
     {
      printw("Код удержания равен нолю!\n");
      fprintf(ffprom,"%s\nКод удержания равен нолю!\n",strok.ravno());
     }
    OSTANOV();
    continue;
   }
  polen(strok.ravno(),&koment,4,'|');
   
  /*Проверяем код начисления*/
  if(prn == 1)
    sprintf(strsql,"select kod from Nash where kod=%d",knah);
  if(prn == 2)
    sprintf(strsql,"select kod from Uder where kod=%d",knah);
  if(readkey(strsql) != 1)
   {
    if(prn == 1)
     {
      printw("Не найден код начисления %d !\n",knah);
      fprintf(ffprom,"%s\nНе найден код начисления %d!\n",strok.ravno(),knah);
     }
    if(prn == 2)
     {
      printw("Не найден код удержания %d !\n",knah);
      fprintf(ffprom,"%s\nНе найден код удержания %d!\n",strok.ravno(),knah);
     }
    OSTANOV();
    continue;
   }

  polen(strok.ravno(),&suma,3,'|');
  if(suma == 0.)
   {
    printw("Сумма равняется нолю!\n");
    fprintf(ffprom,"%s\nСумма равняется нолю!\n",strok.ravno());
    OSTANOV();
    continue;
   }

  /*Смотрим счёт в списке начислений на работника*/
  sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='%d' and knah=%d",tabnom,prn,knah);
  if(readkey(strsql,&row,&cur) == 1)
   {
    shet.new_plus(row[0]);
    
   }
  else
   {
    if(prn == 1)
      sprintf(strsql,"select shet from Nash where kod=%d",knah);
    if(prn == 2)
      sprintf(strsql,"select shet from Uder where kod=%d",knah);
    if(readkey(strsql,&row,&cur) == 1)
     {
      if(row[0][0] != '\0')
       {
        shet.new_plus(row[0]);

       }
     }
         
   }
  nomzap=0;

zapis:;  

  if((voz=zapzarpv(dd,md,gd,tabnom,prn,knah,suma,shet.ravno(),md,gd,0,nomzap,koment.ravno_filtr(),podr,0,0,0,nomdok,zp)) != 0)
   {
    if(voz == 1)
     {
      nomzap++;
      goto zapis;
     }     
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }   

  zarsoc(md,gd,tabnom);
  zaravpr(tabnom,md,gd,NULL);
  
  //Проверяем есть ли запись карточки по этой дате и если нет делаем запись
  sprintf(strsql,"select god from Zarn where tabn=%d and god=%d and mes=%d",tabnom,gd,md);
  if(readkey(strsql) == 0)  
    zapzarn(md,gd,podr,tabnom,kateg,sovm,zvan,shetkar.ravno(),lgoti.ravno(),dnr,mnr,gnr,dkr,mkr,gkr,0,dolg.ravno());
 }
strzag(LINES-1,0,work.st_size,work.st_size);

fclose(ff);
fclose(ffprom);
unlink(imafim.ravno());

rename(imafprom,imafim.ravno());


}
