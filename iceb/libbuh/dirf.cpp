/*$Id: dirf.c,v 5.16 2013/08/13 05:50:22 sasa Exp $*/
/*28.07.2016    15.12.1992      Белых А.И.   dirf.c
Подпрограмма поиска файлов заданных по трафарету
Вызывает dirfajl
Если вернули 0 - выбрали файл
	    -1 - нет
*/
#include        <sys/stat.h>
#include	"buhl.h"


int dirf(char *tif, //Имя файла
int mp, //Метка поиска 1-до точки , 2-росле точки, 3 полное имя файла
int y,int x, //Координаты вывода меню
int kk)      //Номер строки или -10 это значит сразу выйти
{
struct  tm      *bf;
struct stat work;
class SPISOK	mfa; /*Массив с именами файлов*/
int		prc;
int             nf; /*Номер файла в массиве*/
int             ns; /*Номер строки меню*/
int             voz;
class iceb_tu_str name("");
int             kf; /*Количество файлов*/
int		ii;
int		K;
class iceb_t_mstr spis_menu(0);
int zap_kk=kk;


voz=0;
kf=0;
kf=dirfajl(tif,mp,&mfa);

if(kf == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одного файла !"));
  return(1);
 }
/*узнаём самое длинное имя файла*/
int max_name=0;
for(int nom=0; nom < mfa.kolih(); nom++)
 {
  if(max_name < iceb_tu_strlen(mfa.ravno(nom)))
   max_name=iceb_tu_strlen(mfa.ravno(nom));
 }
 
nf=1;

naz:;


int ks=LINES-4-y;

spis_menu.spis_str.free_class();

if(kf > 0 && nf > kf)
 nf=kf;
if(nf < 1)
 nf=1; 

ns=0;
char bros[1024];
for(ii=nf ; ii <= kf && ns<ks ; ii++)
 {
  name.new_plus(mfa.ravno(ii-1));
  if(stat(name.ravno(),&work))
   {
    continue;
   }
  bf=localtime(&work.st_mtime);

  sprintf(bros,"%-*.*s %6ld %02d:%02d %02d.%02d.%d",
  iceb_tu_kolbait(max_name,name.ravno()),iceb_tu_kolbait(max_name,name.ravno()),name.ravno(),
  work.st_size,bf->tm_hour,
  bf->tm_min,bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900);

  ns++;
  spis_menu.spis_str.plus(bros);
 }

if(kk >= ns)
  kk=ns-1;

if(kk != -10 && kk < 0)
 kk=0;

 
if(ns < ks)
  spis_menu.spis_str.plus(gettext("Конец списка"));

prc=nf*100/kf;
memset(bros,'\0',sizeof(bros));

kk=spis_menu.menu(zap_kk,y,x,NULL,bros,prc,&K,"",0);


if(kk == -7) //Изменение размеров экрана
 goto naz;
 
if(kk == -5)  /*Экран вниз*/
 {
  nf+=ks;
  if(nf > kf)
   {
    beep();
    nf=kf;
   }
  goto naz;
 }

if(kk == -3 )  /*Стрелка вниз*/
 {
  nf++;
  
  if(nf > kf)
   {
    beep();
    nf=kf;
   }

  kk=ks;

  goto naz;
 }

if(kk == -2 ) /*Стрелка вверх*/
 {

  nf--;
  if(nf <1)
    nf=1;

  if(nf == 1)
   beep();
  goto naz;
 }

if(kk == -4) /*Экран в верх*/
 {
  nf-=ks;
  if(nf < 1)
   {
    beep();
    nf=1;
   }
  goto naz;
 } 


if(kk == -1 || kk == -10)
  voz=1;
else
 {
  if(SRAV(gettext("Конец списка"),spis_menu.spis_str.ravno(kk),0) == 0)
     goto naz;
  strcpy(tif,mfa.ravno(nf+kk-1));
 }

return(voz);
}
