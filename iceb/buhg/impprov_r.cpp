/*$Id: impprov_r.c,v 5.3 2014/07/31 07:09:16 sasa Exp $*/
/*14.02.2016	08.09.2013	Белых А.И.	impprov_r.c
Импорт проводок из файла
*/
#include <math.h>
#include <errno.h>
#include <sys/stat.h>
#include "buhg.h"

int impprov_r(const char *imafim)
{
class iceb_tu_str bros("");
class iceb_tu_str strok("");
char strsql[2048];
FILE *ff,*ffprom;
char imafprom[64];
class OPSHET shetv;
struct stat work;

if(stat(imafim,&work) != 0)
 {
  sprintf(strsql,"%s %s!",gettext("Нет такого файла"),imafim);
  iceb_t_soob(strsql);
  return(1);
 }


if((ff = fopen(imafim,"r")) == NULL)
 {
  sprintf(strsql,"%s %s:%d !",gettext("Ошибка окрытия файла"),imafim,errno);
  iceb_t_soob(strsql);
  return(1);
 }
sprintf(imafprom,"prom%d.tmp",getpid());
if((ffprom = fopen(imafprom,"w")) == NULL)
 {
  sprintf(strsql,"%s %s:%d !",gettext("Ошибка окрытия файла"),imafprom,errno);
  iceb_t_soob(strsql);
  return(1);
 }

int  val=0;
short d,m,g;
class iceb_tu_str shet("");
class iceb_tu_str shetk("");
class iceb_tu_str kontrsh("");
class iceb_tu_str kontrshk("");
double  deb=0.,kre=0.;
class iceb_tu_str koment("");
int  kolprov=0;
int razmer=0;
//#define KOLPOL 17
int nom_kod_kontr=1;
//class iceb_tu_str rekkon[KOLPOL];
int koloh=0;
class iceb_tu_str rek1("");
double ksd=0.,ksk=0.;
double ideb=0.,ikre=0.;
class iceb_tu_str naim_kontr("");
int metka_sumo=0;
while(iceb_tu_fgets(&strok,ff) == 0)
 {

  razmer+=strlen(strok.ravno());

  printw("%s",strok.ravno());

  if(strok.ravno()[0] == '#')
   continue;
  
  strzag(LINES-1,0,work.st_size,razmer);


  kontrsh.new_plus("");
  kontrshk.new_plus("");

  if(polen(strok.ravno(),&rek1,1,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }
  if(SRAV(rek1.ravno(),"sumo",0) == 0)
   {
    polen(strok.ravno(),&ksd,2,'|');
    polen(strok.ravno(),&ksk,3,'|');
    metka_sumo++;
   }
      

  if(SRAV(rek1.ravno(),"KON",0) == 0) /*запись реквизитов контрагента*/
   {
    iceb_t_zkvsk(strok.ravno(),&nom_kod_kontr,ffprom);
    continue;
   }

  if(polen(strok.ravno(),&bros,1,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  if(bros.ravno()[0] == '\0')
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  val=bros.ravno_atoi();
  if(val != 0 && val != -1)
   {
    iceb_t_soob(gettext("Не правильная метка проводки !"));
    
    fprintf(ffprom,"%s",strok.ravno());
    fprintf(ffprom,"#%s\n",gettext("Не правильная метка проводки !"));
    koloh++;
    continue;

   }
  //Берем дату проводки
  if(polen(strok.ravno(),&bros,2,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  if(rsdat(&d,&m,&g,bros.ravno(),1) != 0)
   {
    
    fprintf(ffprom,"%s",strok.ravno());
    fprintf(ffprom,"#%s\n",gettext("Не верно введена дата проводки !"));
    koloh++;
    continue;
   }

  if(iceb_t_pvglkni(m,g) != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    fprintf(ffprom,"#%s\n",gettext("Дата заблокирована"));
    continue;
   }

  //Берем счет
  if(polen(strok.ravno(),&shet,3,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  if(shet.getdlinna() <= 1)
   {
    fprintf(ffprom,"%s",strok.ravno());
    fprintf(ffprom,"#%s\n",gettext("Не введён счёт"));
    continue;
   }

  if(prsh1(shet.ravno(),&shetv) != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    fprintf(ffprom,"#%s\n",gettext("Не подходит счёт"));
    continue;
   }

  if(shetv.saldo == 3)
   { 
    
    if(polen(strok.ravno(),&kontrsh,4,'|') != 0)
     {
      fprintf(ffprom,"%s",strok.ravno());
      fprintf(ffprom,"#%s!\n",gettext("Не введено поле с кодом контрагента для счёта"));
      koloh++;
      continue;
     }

    if(polen(strok.ravno(),&naim_kontr,10,'|') != 0)
     {
      fprintf(ffprom,"%s",strok.ravno());
      fprintf(ffprom,"#%s %s!\n",gettext("Не введено поле с наименованием контрагента для счёта"),shet.ravno());
      koloh++;
      continue;
     }

    /*определяем код контрагента в общем списке контрагентов*/
    if(iceb_t_get_kkfname(naim_kontr.ravno(),kontrsh.ravno(),&kontrsh,&nom_kod_kontr) < 0)
     {
      fprintf(ffprom,"%s",strok.ravno());
      fprintf(ffprom,"#%s %s!\n",gettext("Не смогли определить код контрагента для наименования"),naim_kontr.ravno());
      koloh++;
      continue;
     }

    if(kontrsh.getdlinna() <= 1)
     {
      fprintf(ffprom,"%s",strok.ravno());
      fprintf(ffprom,"#%s %s!\n",gettext("Не определён код контрагента для счёта"),shet.ravno());
      koloh++;
      continue;
     }

    //Проверяем введён ли этот контрагент в список счета
    sprintf(strsql,"select kodkon from Skontr where ns='%s' and kodkon='%s'",shet.ravno(),kontrsh.ravno());
    if(readkey(strsql) == 0)
     {
      sprintf(strsql,"insert into Skontr (ns,kodkon,ktoi,vrem) values('%s','%s',%d,%ld)",shet.ravno_filtr(),kontrsh.ravno_filtr(),iceb_t_getuid(),time(NULL));
      if(sql_zapis(strsql,1,0) != 0)
       {
        fprintf(ffprom,"%s",strok.ravno());
        fprintf(ffprom,"#%s %s %s Skontr!\n",gettext("Ошибка записи кода контрагента"),kontrsh.ravno(),gettext("в таблицу"));
        koloh++;
        continue;

       }
     }
   }

  if(val == 0)
   {     
    //Берем счёт корреспондент     
    if(polen(strok.ravno(),&shetk,5,'|') != 0)
     {
      fprintf(ffprom,"%s",strok.ravno());
      continue;
     }
    if(shetk.getdlinna() <= 1)
     {
      fprintf(ffprom,"%s",strok.ravno());
      fprintf(ffprom,"#%s\n",gettext("Не введён счёт корреспондент"));
      continue;
     }
    if(prsh1(shetk.ravno(),&shetv) != 0)
     {
      fprintf(ffprom,"%s",strok.ravno());
      fprintf(ffprom,"#%s\n",gettext("Не подходить счёт корреспондент"));
      continue;
     }

    if(shetv.saldo == 3)
     { 
      if(polen(strok.ravno(),&kontrshk,6,'|') != 0)
       {
        fprintf(ffprom,"%s",strok.ravno());
        fprintf(ffprom,"#%s %s!\n",gettext("Не введено поле с кодом контрагента для счёта"),shetk.ravno());
        koloh++;
        continue;
       }
      if(polen(strok.ravno(),&naim_kontr,11,'|') != 0)
       {
        fprintf(ffprom,"%s",strok.ravno());
        fprintf(ffprom,"#%s %s!\n",gettext("Не введено поле с наименованием контрагента для счёта"),shetk.ravno());
        koloh++;
        continue;
       }

      /*определяем код контрагента в общем списке контрагентов*/
      if(iceb_t_get_kkfname(naim_kontr.ravno(),kontrshk.ravno(),&kontrshk,&nom_kod_kontr) < 0)
       {
        fprintf(ffprom,"%s",strok.ravno());
        fprintf(ffprom,"#%s %s!\n",gettext("Не смогли определить код контрагента для наименования"),naim_kontr.ravno());
        koloh++;
        continue;
       }
      if(kontrshk.getdlinna() <= 1)
       {
        fprintf(ffprom,"%s",strok.ravno());
        fprintf(ffprom,"#%s %s!\n",gettext("Не введён код контрагента для счёта"),shetk.ravno());
        koloh++;
        continue;
       }

      //Проверяем введён ли этот контрагент в список счета
      sprintf(strsql,"select kodkon from Skontr where ns='%s' and kodkon='%s'",shetk.ravno(),kontrshk.ravno());
      if(readkey(strsql) == 0)
       {
        sprintf(strsql,"insert into Skontr (ns,kodkon,ktoi,vrem) values('%s','%s',%d,%ld)",shetk.ravno_filtr(),kontrshk.ravno_filtr(),iceb_t_getuid(),time(NULL));
        if(sql_zapis(strsql,1,0) != 0)
         {
          fprintf(ffprom,"%s",strok.ravno());
          fprintf(ffprom,"#%s %s %s Skontr!\n",gettext("Ошибка записи кода контрагента"),kontrshk.ravno(),gettext("в таблицу"));
          koloh++;
          continue;

         }
       }
     }
  }

  //Берем дебет     
  if(polen(strok.ravno(),&deb,7,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  //Берем кредит     
  if(polen(strok.ravno(),&kre,8,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  if(deb+kre == 0.)
   {
    iceb_t_soob(gettext("Не введён ни дебет ни кредит !"));
    fprintf(ffprom,"%s",strok.ravno());
    fprintf(ffprom,"#%s\n",gettext("Не введён ни дебет ни кредит !"));
    koloh++;
    continue;
   }

  if( deb != 0. && kre != 0.)
   {
    iceb_t_soob(gettext("Введен и дебет и кредит !"));
    fprintf(ffprom,"%s",strok.ravno());
    fprintf(ffprom,"#%s\n",gettext("Введен и дебет и кредит !"));
    koloh++;
    continue;
   }

  //Берем комментарий
  if(polen(strok.ravno(),&koment,9,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  
  if(zapprov(val,g,m,d,shet.ravno(),shetk.ravno(),kontrsh.ravno(),kontrshk.ravno(),"","","",deb,kre,koment.ravno(),2,0,time(NULL),0,0,0,0,0) == 0)
   {
    kolprov++;
    ideb+=deb;
    ikre+=kre;  
   }
 }
strzag(LINES-1,0,work.st_size,work.st_size);

printw("\n%s:%.2f\n",gettext("Сумма по дебету"),ideb);
printw("%s:%.2f\n",gettext("Сумма по кредиту"),ikre);

fprintf(ffprom,"\n%s:%.2f\n",gettext("Сумма по дебету"),ideb);
fprintf(ffprom,"%s:%.2f\n",gettext("Сумма по кредиту"),ikre);

if(metka_sumo != 0)
 {
  if(fabs(ideb-ksd) > 0.009)
   {
    fprintf(ffprom,"\n#%s: %.2f != %.2f\n",gettext("Итоговые дебеты не равны"),ideb,ksd);
    printw("\n%s: %.2f != %.2f\n\n",gettext("Итоговые дебеты не равны"),ideb,ksd);
    OSTANOV();
   }

  if(fabs(ikre-ksk) > 0.009)
   {
    fprintf(ffprom,"\n#%s: %.2f != %.2f\n\n",gettext("Итоговые кредиты не равны"),ikre,ksk);
    printw("\n%s: %.2f != %.2f\n\n",gettext("Итоговые кредиты не равны"),ikre,ksk);
    OSTANOV();
   }
 }


fclose(ff);
fclose(ffprom);
unlink(imafim);

rename(imafprom,imafim);

VVOD SOOB(1);
SOOB.VVOD_SPISOK_add_MD(gettext("Загрузка завершена"));

sprintf(strsql,"%s:%d",gettext("Количество загруженых проводок"),kolprov);
SOOB.VVOD_SPISOK_add_MD(strsql);

sprintf(strsql,"%s:%.2f",gettext("Сумма по дебету"),ideb);
SOOB.VVOD_SPISOK_add_MD(strsql);

sprintf(strsql,"%s:%.2f",gettext("Сумма по кредиту"),ikre);
SOOB.VVOD_SPISOK_add_MD(strsql);

soobshw(&SOOB,stdscr,-1,-1,0,1);
return(koloh);
}
