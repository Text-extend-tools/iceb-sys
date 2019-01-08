/*$Id:$*/
/*15.02.2016	08.09.2013	Белых А.И.	impprov_rf.c
Импорт проводок из файла
*/
#include        <errno.h>
#include        <math.h>
#include        <time.h>
#include        <sys/types.h>
#include        <sys/stat.h>
#include "bezcurses.h"
#include <unistd.h>

int impprov_rf(const char *imafim)
{
//class SQLCURSOR cur;
//SQL_str row;
class iceb_tu_str bros("");
class iceb_tu_str strok("");
char strsql[2048];
FILE *ff,*ffprom;
char imafprom[64];
class OPSHET shetv;
struct stat work;

if(stat(imafim,&work) != 0)
 {
  printf("%s-%s %s!\n",__FUNCTION__,gettext("Нет такого файла"),imafim);
  return(1);
 }


if((ff = fopen(imafim,"r")) == NULL)
 {
  printf("%s-%s %s:%d !\n",__FUNCTION__,gettext("Ошибка окрытия файла"),imafim,errno);
  return(1);
 }
sprintf(imafprom,"prom%d.tmp",getpid());
if((ffprom = fopen(imafprom,"w")) == NULL)
 {
  printf("%s-%s %s:%d !\n",__FUNCTION__,gettext("Ошибка окрытия файла"),imafprom,errno);
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
int i=0;
int  kolprov=0;
int razmer=0;
//#define KOLPOL 17
class iceb_tu_str kontr_zag("");
int nom_kod_kontr=1;
//class iceb_tu_str rekkon[KOLPOL];
class iceb_tu_str naim_kontr("");
int koloh=0;
double ideb=0.,ikre=0.;
class iceb_tu_str rek1("");

while(iceb_tu_fgets(&strok,ff) == 0)
 {
  if(strok.ravno()[0] == '#')
   continue;
  i=strlen(strok.ravno());
  razmer+=i;

  printf("%s",strok.ravno());

  kontrsh.new_plus("");
  kontrshk.new_plus("");

  if(polen(strok.ravno(),&rek1,1,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
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
    printf("%s\n",gettext("Не правильная метка проводки !"));
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
    printf("%s\n",gettext("Не верно введена дата проводки !"));
    fprintf(ffprom,"%s",strok.ravno());
    fprintf(ffprom,"#%s\n",gettext("Не верно введена дата проводки !"));
    koloh++;
    continue;
   }
  if(iceb_t_pvglkni(m,g) != 0)
   continue;


  //Берем счет
  if(polen(strok.ravno(),&shet,3,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  if(prsh1(shet.ravno(),&shetv) != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
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
      fprintf(ffprom,"#%s!\n",gettext("Не введён код контрагента для счёта"));
      koloh++;
      continue;
     }

    //Проверяем введен ли этот контрагент в список счета
    sprintf(strsql,"select kodkon from Skontr where ns='%s' and \
kodkon='%s'",shet.ravno(),kontrsh.ravno());
    if(sql_readkey(&bd,strsql) != 1)
     {
      sprintf(strsql,"insert into Skontr (ns,kodkon,ktoi,vrem) values('%s','%s',%d,%ld)",shetk.ravno_filtr(),kontrsh.ravno_filtr(),iceb_t_getuid(),time(NULL));
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
    //Берем счет корреспондент     
    if(polen(strok.ravno(),&shetk,5,'|') != 0)
     {
      fprintf(ffprom,"%s",strok.ravno());
      continue;
     }

    if(prsh1(shetk.ravno(),&shetv) != 0)
     {
      fprintf(ffprom,"%s",strok.ravno());
      continue;
     }

    if(shetv.saldo == 3)
     { 
      if(polen(strok.ravno(),&kontrshk,6,'|') != 0)
       {
        fprintf(ffprom,"%s",strok.ravno());
        fprintf(ffprom,"#%s!\n",gettext("Не введено поле с кодом контрагента для счёта кореспондента"));
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
        fprintf(ffprom,"#%s!\n",gettext("Не введён код контрагента для счёта кореспондента"));
        koloh++;
        continue;
       }
 
      //Проверяем введен ли этот контрагент в список счета
      sprintf(strsql,"select kodkon from Skontr where ns='%s' and kodkon='%s'",shetk.ravno(),kontrshk.ravno());
      if(readkey(strsql) != 1)
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
    printf("%s\n",gettext("Не введен ни дебет ни кредит !"));
    fprintf(ffprom,"%s",strok.ravno());
    fprintf(ffprom,"#%s\n",gettext("Не введен ни дебет ни кредит !"));
    koloh++;
    continue;
   }

  if( deb != 0. && kre != 0.)
   {
    printf("%s\n",gettext("Введен и дебет и кредит !"));
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

fclose(ff);
fclose(ffprom);
unlink(imafim);

rename(imafprom,imafim);

printf("%s\n",gettext("Загрузка завершена"));
printf("%s:%d\n",gettext("Количество загруженых проводок"),kolprov);
printf("%s:%d\n",gettext("Количество ошибок"),koloh);
printf("%s:%.2f\n",gettext("Сумма по дебету"),ideb);
printf("%s:%.2f\n",gettext("Сумма по кредиту"),ikre);

return(koloh);
}
