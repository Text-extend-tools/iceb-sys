/*$Id: impzarnu.c,v 5.37 2014/07/31 07:09:16 sasa Exp $*/
/*01.11.2017	02.07.2002	Белых А.И.	impzarnu.c
Импорт начислений/удержаний в базу данных "Расчёта зарплаты".
*/
#include        <errno.h>
#include        <sys/stat.h>
#include        "buhg.h"


void	impzarnu(short prn,short knah,
short mz,short gz)
{
struct stat work;
char		strsql[1024];
SQL_str		row,row1;
SQLCURSOR curr;
static class iceb_tu_str imafim("");
static char imafprom[64];
char bros1[1024];
int		kom=0;
int		razmer=0;
FILE		*ff,*ffprom;
class iceb_tu_str strok("");
class iceb_tu_str bros("");
int tabnom=0; //Табельный номер
class iceb_tu_str naim("");
class ZARP     zp;
short           prov;   //0-не выполнены 1-выполнены
short		mzz=mz,gzz=gz;
short		metkadat=0;
class iceb_tu_str shet_nu("");

VVOD VVOD1(2);

clear();

if(prn == 1)
 {
  sprintf(bros1,"%s %s. %s:%d.%d%s",
  gettext("Импорт"),gettext("начислений"),
  gettext("Дата"),
  mz,gz,gettext("г."));  

  sprintf(strsql,"select naik,shet from Nash where kod=%d",knah);
  
 }
if(prn == 2)
 {
  sprintf(bros1,"%s %s. %s:%d.%d%s",
  gettext("Импорт"),gettext("удержаний"),
  gettext("Дата"),
  mz,gz,gettext("г."));   
  sprintf(strsql,"select naik,shet from Uder where kod=%d",knah);
 }

VVOD1.VVOD_SPISOK_add_ZAG(bros1);


if(sql_readkey(&bd,strsql,&row,&curr) == 1)
 {
  naim.new_plus(row[0]);  
  shet_nu.new_plus(row[1]);
 }

sprintf(bros1,"%d %s",knah,naim.ravno());
VVOD1.VVOD_SPISOK_add_ZAG(bros1);

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
    iceb_t_pdoc("zarp2_8_1_2.txt");
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

short den=0;
class iceb_tu_str shet(shet_nu.ravno());
short mvr,gvr;
int   podr=0;
short nomzap;
double suma=0.;
class iceb_tu_str koment("");
short denp=0;
struct OPSHET	shetv;
dpm(&denp,&mz,&gz,5);
razmer=0;
short dn,mn,gn;
short du,mu,gu;
class iceb_tu_str shetkar("");
int podrkar=0;
int kateg;
int sovm;
int zvan;
class iceb_tu_str lgot("");
class iceb_tu_str dolg("");

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

 
  if(SRAV(bros.ravno(),"Дата",0) == 0)
   {
    fprintf(ffprom,"%s",strok.ravno()); //Всегда записываем
    metkadat=0;
    polen(strok.ravno(),&bros,2,'|');    

    if(rsdat1(&mzz,&gzz,bros.ravno()) != 0)
     {
      iceb_t_soob(gettext("Не верно введена дата !"));
      metkadat=1;      
      continue;
     }
    denp=1;
    dpm(&denp,&mzz,&gzz,5);
    continue;
   }

  if(metkadat == 1)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }
  
  if((tabnom=bros.ravno_atoi()) == 0)
   {
    fprintf(ffprom,"%s%s\n",strok.ravno(),"Табельный номер равен нолю");
    continue;
   }

  //Проверяем есть ли такой табельный номер  
  sprintf(strsql,"select datn,datk,shet,podr,kodg,kateg,\
sovm,zvan,lgoti,dolg from Kartb where tabn=%d",tabnom);
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    printw("%s %d !\n",gettext("Не найден табельный номер"),tabnom);
    OSTANOV();    
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  rsdat(&dn,&mn,&gn,row1[0],2);
  rsdat(&du,&mu,&gu,row1[1],2);
  shetkar.new_plus(row1[2]);
  podrkar=atoi(row1[3]);
  kateg=atoi(row1[5]);
  sovm=atoi(row1[6]);
  zvan=atoi(row1[7]);
  lgot.new_plus(row1[8]);
  dolg.new_plus(row1[9]);

  if(polen(strok.ravno(),&den,3,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }


  if(den < 1 && den > denp)
   {
    printw("%s\n",gettext("Не верно введён день начисления/удержания !"));
    OSTANOV();
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  if(shet_nu.getdlinna() <= 1)
   if(polen(strok.ravno(),&shet,4,'|') != 0)
    {
     fprintf(ffprom,"%s",strok.ravno());
     continue;
    }

   
  //Проверить есть ли счёт в плане счетов
  if(prsh1(shet.ravno(),&shetv) != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  //Дата в счёт какого месяца
  if(polen(strok.ravno(),&bros,5,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  if(rsdat1(&mvr,&gvr,bros.ravno()) != 0)
   {
    printw("%s\n",gettext("Не верно введена дата !"));
    OSTANOV();
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }
  
  //Подразделение     
  if(polen(strok.ravno(),&podr,6,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }
  //Проверяем код подразделения
  sprintf(strsql,"select naik from Podr where kod=%d",podr);

  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)  
   {
    printw("%s %d !\n",gettext("Не найден код подразделения"),podr);
    OSTANOV();
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  //Номер записи
  if(polen(strok.ravno(),&nomzap,7,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  //Сумма
  if(polen(strok.ravno(),&suma,8,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }
  if(suma == 0.)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }
   
  if(prn == 2)
   suma*=-1;
   
  //Коментарий
  if(polen(strok.ravno(),&koment,9,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }
      
  if(zapzarpv(den,mzz,gzz,tabnom,prn,knah,suma,shet.ravno(),mvr,gvr,0,nomzap,koment.ravno(),podr,0,0,0,"",zp) != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }   

  if(prn == 1)
   zarsoc(mzz,gzz,tabnom);
  zaravpr(tabnom,mzz,gzz,NULL);
  prov=0;
  
  //Проверяем есть ли запись карточки по этой дате и если нет делаем запись
  sprintf(strsql,"select god from Zarn where tabn=%d and god=%d and mes=%d",tabnom,gzz,mzz);
  if(readkey(strsql) == 0)  
    zapzarn(mzz,gzz,podrkar,tabnom,kateg,sovm,zvan,shetkar.ravno(),lgot.ravno(),dn,mn,gn,du,mu,gu,prov,dolg.ravno());

 }
strzag(LINES-1,0,work.st_size,work.st_size);

fclose(ff);
fclose(ffprom);
unlink(imafim.ravno());

rename(imafprom,imafim.ravno());
}
