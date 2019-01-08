/*$Id: impuosdok.c,v 5.29 2014/07/31 07:09:16 sasa Exp $*/
/*15.02.2016	18.11.2002	Белых А.И. 	impuosdok.c
Импорт документов для "Учета основных средств"
*/
#include        <errno.h>
#include        <sys/stat.h>
#include        "buhg.h"

int	impuosdokz(int,const char*);

extern double	okrcn;  /*Округление цены*/

void	impuosdok(void)
{
struct stat     work;
static class iceb_tu_str imafim("");
char		strsql[512];

naz:;

if(iceb_t_menu_import(&imafim,gettext("Импорт документов"),"impuosdok.alx","uos2_4.txt") != 0)
  return;

clear();
move(LINES-1,0);
GDITE();

if(stat(imafim.ravno(),&work) != 0)
 {
  sprintf(strsql,"%s !",gettext("Нет такого файла"));
  iceb_t_soob(strsql);
  goto naz;
 }
printw("%s\n",gettext("Проверяем записи в файле"));
int koloh=impuosdokz(0,imafim.ravno());
if(koloh != 0)
 {
  VVOD SOOB(1);
  sprintf(strsql,"%s:%d",gettext("Количество ошибок"),koloh);
  SOOB.VVOD_SPISOK_add_MD(strsql);
  sprintf(strsql,gettext("Импорт документов осуществляется если нет ни одной ошибки"));
  SOOB.VVOD_SPISOK_add_MD(strsql);
  soobshw(&SOOB,stdscr,-1,-1,0,1);
 }
else
 {
  printw("\n\n%s\n\n",gettext("Загружаем документы из файла"));
  int koldok = impuosdokz(1,imafim.ravno());

  VVOD SOOB(1);
  sprintf(strsql,"%s",gettext("Загрузка завершена"));
  SOOB.VVOD_SPISOK_add_MD(strsql);
  sprintf(strsql,"%s:%d",gettext("Количество документов"),koldok);
  SOOB.VVOD_SPISOK_add_MD(strsql);
  soobshw(&SOOB,stdscr,-1,-1,0,1);
 }
}
/*****************************************/
/*Проверка-загрузка                      */
/*****************************************/

int	impuosdokz(int metka, //0-проверять 1-грузить
const char *imaf)
{
int		koloh=0;
FILE		*ff;
class iceb_tu_str strok("");
char		strsql[1024];
short		dd,md,gd;
int		tipz=0;
class iceb_tu_str kodop("");
class iceb_tu_str kontr("");
class iceb_tu_str nomdok("");
class iceb_tu_str podr("");
class iceb_tu_str motv("");
time_t		vrem;
class iceb_tu_str innom("");
double		bsnu=0.;
double		iznu=0.;
double		bsbu=0.;
double		izbu=0.;
char		shetu[64];
struct OPSHET	shetv;
class iceb_tu_str hzatrat("");
class iceb_tu_str hanuh("");
class iceb_tu_str grupnu("");
class iceb_tu_str grupbu("");
double		popkfnu=0.;
double		popkfbu=0.;
class iceb_tu_str sost("");
class iceb_tu_str naim("");
short		dv,mv,gv;
int		kol=0;
int		koldok=0;
int		razmer=0;
class iceb_tu_str naim_kontr("");

double dpnds=0.; /*Действующий процент НДС действующий на момент создания документа*/

struct stat work;

if(stat(imaf,&work) != 0)
 {
  sprintf(strsql,"%s %s!",gettext("Нет такого файла"),imaf);
  iceb_t_soob(strsql);
  return(-1);
 }

if((ff = fopen(imaf,"r")) == NULL)
 {
  sprintf(strsql,"%s %d !",gettext("Ошибка окрытия файла"),errno);
  iceb_t_soob(strsql);
  return(-1);
 }
time(&vrem);
int nom_kod_kontr=1;
class iceb_tu_str rek1("");

while(iceb_tu_fgets(&strok,ff) == 0)
 {
  razmer+=strlen(strok.ravno());
  if(strok.ravno()[0] == '#')
   continue;

  printw("%s",strok.ravno());
  strzag(LINES-1,0,work.st_size,razmer);
  if(polen(strok.ravno(),&rek1,1,'|') != 0)
   {
    continue;
   }

  if(SRAV(rek1.ravno(),"KON",0) == 0) /*запись реквизитов контрагента*/
   {
    iceb_t_zkvsk(strok.ravno(),&nom_kod_kontr,NULL);
    continue;
   }

/**************************************/
  if(SRAV(rek1.ravno(),"DOC",0) == 0)
   {
    nomdok.new_plus("");
    dd=md=gd=0;
    tipz=0;
    kol=0;
    
    polen(strok.ravno(),strsql,sizeof(strsql),2,'|');
    if(rsdat(&dd,&md,&gd,strsql,1) != 0)
     {
      iceb_t_soob(gettext("Не верно введена дата!"));
      koloh++;
      continue;
     }

    polen(strok.ravno(),strsql,sizeof(strsql),3,'|');

    tipz=0;
    if(strsql[0] == '+')
     {
      tipz=1;
      kol=1;
     }
    if(strsql[0] == '-')
     {
      tipz=2;
      kol=-1;
     }
    if(tipz == 0)
     {
      iceb_t_soob(gettext("Не определен вид документа (приход/расход) !"));
      koloh++;
      continue;
     }
    polen(strok.ravno(),&kodop,4,'|');
    if(tipz == 1)
      sprintf(strsql,"select kod from Uospri where kod='%s'",kodop.ravno());
    if(tipz == 2)
      sprintf(strsql,"select kod from Uosras where kod='%s'",kodop.ravno());

    if(readkey(strsql) != 1)
     {
      sprintf(strsql,gettext("Не найден код операции %s !"),kodop.ravno());
      iceb_t_soob(strsql);
      koloh++;
      continue;
     }

    polen(strok.ravno(),&kontr,5,'|');
/********************
    sprintf(strsql,"select kodkon from Kontragent where kodkon='%s'",kontr.ravno());
    if(readkey(strsql) != 1)
     {
      sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),kontr.ravno());
      iceb_t_soob(strsql);
      koloh++;
      continue;
     }
*********************/
    polen(strok.ravno(),&podr,7,'|');
    sprintf(strsql,"select kod from Uospod where kod=%s",podr.ravno());
    if(readkey(strsql) != 1)
     {
      sprintf(strsql,"%s %s !",gettext("Не найден код подразделения"),podr.ravno());
      iceb_t_soob(strsql);
      koloh++;
      continue;
     }

    polen(strok.ravno(),&motv,8,'|');
    sprintf(strsql,"select kod from Uosol where kod=%s",
    motv.ravno());
    if(readkey(strsql) != 1)
     {
      sprintf(strsql,"%s %s !",gettext("Не найден мат. ответственный"),motv.ravno());
      iceb_t_soob(strsql);
      koloh++;
      continue;
     }

    polen(strok.ravno(),&dpnds,9,'|');

    if(polen(strok.ravno(),&naim_kontr,10,'|') != 0)
     {
      iceb_t_soob(gettext("Не найдено поле наименование контрагента"));
      koloh++;
      continue;
     }
    if(naim_kontr.getdlinna() <= 1)
     {
      iceb_t_soob(gettext("Не введено наименование контрагента"));
      koloh++;
      continue;
     }

    /*определяем код контрагента в общем списке контрагентов*/
    if(iceb_t_get_kkfname(naim_kontr.ravno(),kontr.ravno(),&kontr,&nom_kod_kontr) < 0)
     {
      koloh++;
      continue;
     }

    //Номер документа определяем последним так как он является индикатором
    //наличия шапки документа
    polen(strok.ravno(),&nomdok,6,'|');
    if(SRAV(nomdok.ravno(),"авто",0) == 0 || nomdok.ravno()[0] == '\0')
      uosnd(gd,tipz,&nomdok);

    sprintf(strsql,"select nomd from Uosdok where tipz=%d and \
datd >= '%04d-01-01' and datd <= '%04d-12-31' and nomd='%s'",
    tipz,gd,gd,nomdok.ravno_filtr());
    if(readkey(strsql) >= 1)
     {
      sprintf(strsql,gettext("С номером %s документ уже есть !"),nomdok.ravno());
      iceb_t_soob(strsql);
      koloh++;
      continue;
     }

    //Запись в базу
    if(metka != 0 && nomdok.ravno()[0] != '\0' && koloh == 0)
     {

      sprintf(strsql,"insert into Uosdok (datd,tipz,kodop,kontr,nomd,podr,kodol,prov,ktoz,vrem,pn) \
values ('%04d-%02d-%02d',%d,'%s','%s','%s',%s,%s,%s,%d,%ld,%.10g)",
      gd,md,dd,tipz,kodop.ravno(),kontr.ravno(),nomdok.ravno_filtr(),podr.ravno(),motv.ravno(),"1",iceb_t_getuid(),vrem,dpnds);

      if(sql_zap(&bd,strsql) != 0)
       {
        msql_error(&bd,gettext("Ошибка записи !"),strsql);
        koloh++;
       }
      else
        koldok++;

     }        
     

   }

/**************************************/
  if(SRAV(rek1.ravno(),"ZP1",0) == 0)
   {
    polen(strok.ravno(),&innom,2,'|');
    sprintf(strsql,"select innom from Uosin where innom=%s",innom.ravno());
    if(readkey(strsql) >= 1)
     {
      if(tipz == 1)
       {
        sprintf(strsql,"%s %s",innom.ravno(),gettext("Такой инвентарный номер уже есть !"));
        iceb_t_soob(strsql);
        koloh++;
        continue;
       }
     }
    else
     if(tipz == 2)
      {
        sprintf(strsql,"%s %s",gettext("Не найден инвентарный номер"),innom.ravno());
        iceb_t_soob(strsql);
        koloh++;
        continue;

      }

    polen(strok.ravno(),strsql,sizeof(strsql),3,'|');
    bsnu=ATOFM(strsql);

    polen(strok.ravno(),strsql,sizeof(strsql),4,'|');
    iznu=ATOFM(strsql);

    polen(strok.ravno(),strsql,sizeof(strsql),5,'|');
    bsbu=ATOFM(strsql);

    polen(strok.ravno(),strsql,sizeof(strsql),6,'|');
    izbu=ATOFM(strsql);

    polen(strok.ravno(),shetu,sizeof(shetu),7,'|');
    if(prsh1(shetu,&shetv) != 0)
     {
      koloh++;
      continue;
     }

    polen(strok.ravno(),&hzatrat,8,'|');

    sprintf(strsql,"select kod from Uoshz where kod='%s'",hzatrat.ravno());

    if(readkey(strsql) != 1)
     {
      sprintf(strsql,"%s %s %s",innom.ravno(),gettext("Не найден шифр производственных затрат"),hzatrat.ravno());
      iceb_t_soob(strsql);
      koloh++;
      continue;
     }

    polen(strok.ravno(),&hanuh,9,'|');

    sprintf(strsql,"select kod from Uoshau where kod='%s'",hanuh.ravno());

    if(readkey(strsql) != 1)
     {
      sprintf(strsql,"%s %s %s",innom.ravno(),gettext("Не найден шифр аналитического учета"),hanuh.ravno());
      iceb_t_soob(strsql);
      koloh++;
      continue;
     }

    polen(strok.ravno(),&grupnu,10,'|');

    sprintf(strsql,"select kod from Uosgrup where kod='%s'",grupnu.ravno());

    if(readkey(strsql) != 1)
     {
      sprintf(strsql,"%s %s %s",innom.ravno(),gettext("Не найден код группы"),grupnu.ravno());
      iceb_t_soob(strsql);
      koloh++;
      continue;
     }

    polen(strok.ravno(),strsql,sizeof(strsql),11,'|');
    popkfnu=ATOFM(strsql);
    
    polen(strok.ravno(),&grupbu,12,'|');

    sprintf(strsql,"select kod from Uosgrup1 where kod='%s'",grupbu.ravno());

    if(readkey(strsql) != 1)
     {
      sprintf(strsql,"%s %s %s",innom.ravno(),gettext("Не найден код группы"),grupbu.ravno());
      iceb_t_soob(strsql);
      koloh++;
      continue;
     }

    polen(strok.ravno(),strsql,sizeof(strsql),13,'|');
    popkfbu=ATOFM(strsql);

    polen(strok.ravno(),&sost,14,'|');

    polen(strok.ravno(),&naim,15,'|');
    
    polen(strok.ravno(),strsql,sizeof(strsql),16,'|');
    rsdat(&dv,&mv,&gv,strsql,1);

    //Запись в базу
    if(metka != 0 && nomdok.ravno()[0] != '\0' && innom.ravno()[0] != '\0' && koloh == 0)
     {
      if(tipz == 1)
       {
        sprintf(strsql,"insert into Uosin \
values (%s,%s,'%s','%s','%s','%s','%s','%04d-%d-%d',%d,%ld)",
        innom.ravno(),"0",naim.ravno_filtr(),"","","","",gv,mv,dv,iceb_t_getuid(),vrem);
        if(sql_zapis(strsql,1,0) != 0)
          continue;
       }      

      sprintf(strsql,"insert into Uosdok1 \
values ('%04d-%02d-%02d',%d,%s,%s,'%s',%s,%s,%d,%.2f,%.2f,%.2f,'%s',%d,%ld,\
%.2f,%.2f,%.2f,'%s')",
      gd,md,dd,tipz,"0",innom.ravno(),nomdok.ravno_filtr(),podr.ravno(),motv.ravno(),kol,bsnu,iznu,1.,kodop.ravno(),iceb_t_getuid(),vrem,\
      bsbu,izbu,0.,"");
      
      
      if(sql_zapis(strsql,1,0) != 0)
       continue;

      if(tipz == 1)
       { 
        sprintf(strsql,"insert into Uosinp \
values (%s,%d,%d,'%s','%s','%s','%s',%.2f,%s,'%s',%d,%ld,'%s',%.2f)",
        innom.ravno(),md,gd,shetu,hzatrat.ravno(),hanuh.ravno(),grupnu.ravno(),popkfnu,sost.ravno(),"",iceb_t_getuid(),vrem,\
        grupbu.ravno(),popkfbu);

      
        sql_zapis(strsql,1,0);
       }
     }
   }

 }
strzag(LINES-1,0,work.st_size,work.st_size);
fclose(ff);
printw("Количество ошибок:%d\n",koloh);

if(metka == 0)
  return(koloh);
if(metka == 1)
  return(koldok);
return(0);
}



