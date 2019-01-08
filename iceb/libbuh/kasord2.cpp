/*$Id: kasord2.c,v 5.38 2013/11/05 10:50:57 sasa Exp $*/
/*15.06.2018	04.10.2000	Белых А.И.	kasord2.c
Распечатка расходного кассового ордера
*/
#include        <errno.h>
#include	"buhl.h"


void	kasord2_0(const char *imaf,const char *nomd,short dd,short md,short gd,const char *shet,const char *shetkas,double suma,const char *fio,const char *osnov,const char *dopol,const char *dokum,const char *hcn,FILE *ff_f);
void	kasord2_1(const char *imaf,const char *nomd,short dd,short md,short gd,const char *shet,const char *shetkas,double suma,const char *fio,const char *osnov,const char *dopol,const char *dokum,const char *hcn,FILE *ff_f);
void	kasord2_2(const char *imaf,const char *nomd,short dd,short md,short gd,const char *shet,const char *shetkas,double suma,const char *fio,const char *osnov,const char *dopol,const char *dokum,const char *hcn,FILE *ff_f);

void	kasord2(const char *imaf, //Имя файла
const char *nomd, //Номер документа
short dd,short md,short gd, //Дата
const char *shet, //Счет кореспондент
const char *shetkas, //Счет кассы ,Шифр аналитического учета
double suma, //Сумма
const char *fio, //Фамилия имя отчество
const char *osnov, //Основание
const char *dopol, //Дополнение
const char *dokum, //Реквизита документа по которому выданы деньги
const char *hcn, //Шифр целевого назначения
FILE *ff_f)
{
if(sravmydat(dd,md,gd,16,6,2009) < 0)
  kasord2_0(imaf,nomd,dd,md,gd,shet,shetkas,suma,fio,osnov,dopol,dokum,hcn,ff_f);
if(sravmydat(dd,md,gd,16,6,2009) >= 0 && sravmydat(dd,md,gd,5,1,2018) < 0)
  kasord2_1(imaf,nomd,dd,md,gd,shet,shetkas,suma,fio,osnov,dopol,dokum,hcn,ff_f);
else
  kasord2_2(imaf,nomd,dd,md,gd,shet,shetkas,suma,fio,osnov,dopol,dokum,hcn,ff_f);

}
/******************************/
/*******************************/
void kasord2_2(const char *imaf, //Имя файла
const char *nomd, //Номер документа
short dd,short md,short gd, //Дата
const char *shet, //Счет кореспондент
const char *shetkas, //Счет кассы ,Шифр аналитического учета
double suma, //Сумма
const char *fio, //Фамилия имя отчество
const char *osnov, //Основание
const char *dopol, //Дополнение
const char *dokum, //Реквизита документа по которому выданы деньги
const char *hcn, //Шифр целевого назначения
FILE *ff_f)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[1024];
SQL_str		row;
SQLCURSOR       cur;
class iceb_tu_str naim("");
class iceb_tu_str kodedrp("");
class iceb_tu_str strf("");
short		kols;
class iceb_tu_str sumac("");
class iceb_t_fioruk_rk rukov;
class iceb_t_fioruk_rk glbuh;
class iceb_tu_str naim_mes("");
FILE *ff=ff_f;
char rukov_str[512];
char glbuh_str[512];
char kassir_str[512];

preobr(suma,&sumac,0);


iceb_t_fioruk(1,&rukov); /*узнаём фамилию главного бухгалтера*/
iceb_t_fioruk(2,&glbuh); /*узнаём фамилию главного бухгалтера*/

/*записываем со смещением в право*/
memset(glbuh_str,'\0',sizeof(glbuh_str));
sprintf(glbuh_str,"%*.*s",
iceb_tu_kolbait(23,glbuh.famil_inic.ravno()),
iceb_tu_kolbait(23,glbuh.famil_inic.ravno()),
glbuh.famil_inic.ravno());

for(int nom=0; nom < (int)strlen(glbuh_str); nom++)
 if(glbuh_str[nom] == ' ')
  glbuh_str[nom]='_';

/*записываем со смещением в право*/
memset(rukov_str,'\0',sizeof(rukov_str));
sprintf(rukov_str,"%*.*s",
iceb_tu_kolbait(27,rukov.famil_inic.ravno()),
iceb_tu_kolbait(27,rukov.famil_inic.ravno()),
rukov.famil_inic.ravno());

for(int nom=0; nom < (int)strlen(rukov_str); nom++)
 if(rukov_str[nom] == ' ')
  rukov_str[nom]='_';

/*Чтобы узнать фамилию кассира нужно узнать номер кассы*/
memset(kassir_str,'\0',sizeof(kassir_str));
/**************
sprintf(strsql,"select kassa from Kasord where nomd='%s' and datd='%04d-%02d-%02d' and tp=2",
nomd,gd,md,dd);
if(readkey(strsql,&row,&cur) == 1)
 {
  sprintf(strsql,"select fio from Kas where kod=%s",row[0]);
  if(readkey(strsql,&row,&cur) == 1)
   sprintf(kassir_str,"%*.*s",iceb_tu_kolbait(38,row[0]),iceb_tu_kolbait(38,row[0]),row[0]); //чтобы получить со смещением вправо
 }
***************/

strcpy(strsql,iceb_t_getfioop());
sprintf(kassir_str,"%*.*s",iceb_tu_kolbait(38,strsql),iceb_tu_kolbait(38,strsql),strsql); /*чтобы получить со смещением вправо*/

for(int nom=0; nom < (int)strlen(kassir_str); nom++)
 if(kassir_str[nom] == ' ')
  kassir_str[nom]='_';

/*Читаем реквизиты организации*/

sprintf(strsql,"select naikon,kod from Kontragent where kodkon='00'");

if(readkey(strsql,&row,&cur) == 1)
 {
  naim.new_plus(row[0]);
  kodedrp.new_plus(row[1]);
 }

if(imaf[0] != '\0')
 {
  if((ff = fopen(imaf,"w")) == NULL)
   {
    error_op_nfil(imaf,errno,"");
    return;
   }
 }
 
fprintf(ff,"\x1B\x33%c\n",30); /*Уменьшаем межстрочный интервал*/

sprintf(strsql,"select str from Alx where fil='kasord2_1.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s-%s kasord2_1.alx!",__FUNCTION__,gettext("Не найден бланк кассового ордера"));
  iceb_t_soob(strsql);
  return;
 }


short metka_sum_prop=0;
if(iceb_t_poldan("Печатать сумму прописью в расходном документе",&strf,"kasnast.alx") == 0)
 if(SRAV("Вкл",strf.ravno(),1) == 0)
  metka_sum_prop=1;

mesc(md,1,&naim_mes);

kols=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  strf.new_plus(row_alx[0]);
  kols++;
  switch (kols)
   {
    case 6:
      iceb_tu_vstav(&strf,naim.ravno(),0,85,1);
      break;     

    case 10:
      iceb_tu_vstav(&strf,kodedrp.ravno(),41,85,1);
      break;     

    case 13:
      if(nomd[0] != '\0')
       {
        sprintf(strsql,"%02d",dd);
        iceb_tu_vstav(&strf,strsql,26,27,1);

        sprintf(strsql,"%s %d року",naim_mes.ravno(),gd);
        iceb_tu_vstav(&strf,strsql,30,79,1);
       }
      break;     

    case 21:
      iceb_tu_vstav(&strf,"1",2,4,1);
      iceb_tu_vstav(&strf,nomd,5,16,1);
      if(nomd[0] != '\0')
       {
        sprintf(strsql,"%02d.%02d.%d",dd,md,gd);
        iceb_tu_vstav(&strf,strsql,15,25,1);
       }
      iceb_tu_vstav(&strf,shet,29,42,1);
      iceb_tu_vstav(&strf,shetkas,44,56,1);
      sprintf(strsql,"%.2f",suma);
      iceb_tu_vstav(&strf,strsql,57,69,1);
      if(atoi(hcn) != 0)
        iceb_tu_vstav(&strf,hcn,70,80,1);
      break;     

    case 23:
      iceb_tu_vstav(&strf,fio,7,86,1);
      break;     

    case 25:
      iceb_tu_vstav(&strf,osnov,9,86,1);
      break;     

    case 26:
      if(iceb_tu_strlen(osnov) > 78)
       iceb_tu_vstav(&strf,iceb_tu_adrsimv(78,osnov),0,86,1);
      break;     

    case 27:
       iceb_tu_vstav(&strf,sumac.ravno(),5,86,1);
       break;     

    case 29:
       iceb_tu_vstav(&strf,dopol,9,86,1);
       break;     

    case 30:
       if(iceb_tu_strlen(dopol) > 78)
        iceb_tu_vstav(&strf,iceb_tu_adrsimv(78,dopol),0,86,1);
       break;     

    case 32:
       iceb_tu_vstav(&strf,rukov_str,9,35,1);
       iceb_tu_vstav(&strf,glbuh_str,60,86,1);
       break;     

    case 34:
       if(metka_sum_prop == 0)
        break;
       iceb_tu_vstav(&strf,sumac.ravno(),9,86,1);
       break;
       
    case 36:
       iceb_tu_vstav(&strf,gd,26,29,1);

       break;     

    case 38:
       iceb_tu_vstav(&strf,dokum,3,86,1);
       break;     

    case 40:
       if(iceb_tu_strlen(dokum) > 84)
         iceb_tu_vstav(&strf,iceb_tu_adrsimv(84,dokum),0,86,1);
       break;     

    case 42:
       iceb_tu_vstav(&strf,kassir_str,12,49,1);
       break;     
   }
  
  fprintf(ff,"%s",strf.ravno());
  
 }


if(imaf[0] != '\0')
 {
  fclose(ff);
  iceb_t_ustpeh(imaf,1);
 }

}
/******************************/
/*******************************/
void kasord2_1(const char *imaf, //Имя файла
const char *nomd, //Номер документа
short dd,short md,short gd, //Дата
const char *shet, //Счет кореспондент
const char *shetkas, //Счет кассы ,Шифр аналитического учета
double suma, //Сумма
const char *fio, //Фамилия имя отчество
const char *osnov, //Основание
const char *dopol, //Дополнение
const char *dokum, //Реквизита документа по которому выданы деньги
const char *hcn, //Шифр целевого назначения
FILE *ff_f)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[1024];
SQL_str		row;
SQLCURSOR       cur;
class iceb_tu_str naim("");
class iceb_tu_str kodedrp("");
class iceb_tu_str strf("");
short		kols;
class iceb_tu_str sumac("");
class iceb_t_fioruk_rk rukov;
class iceb_t_fioruk_rk glbuh;
class iceb_tu_str naim_mes("");
FILE *ff=ff_f;
char rukov_str[512];
char glbuh_str[512];
char kassir_str[512];

preobr(suma,&sumac,0);


iceb_t_fioruk(1,&rukov); /*узнаём фамилию главного бухгалтера*/
iceb_t_fioruk(2,&glbuh); /*узнаём фамилию главного бухгалтера*/

/*записываем со смещением в право*/
memset(glbuh_str,'\0',sizeof(glbuh_str));
sprintf(glbuh_str,"%*.*s",
iceb_tu_kolbait(23,glbuh.famil_inic.ravno()),
iceb_tu_kolbait(23,glbuh.famil_inic.ravno()),
glbuh.famil_inic.ravno());

for(int nom=0; nom < (int)strlen(glbuh_str); nom++)
 if(glbuh_str[nom] == ' ')
  glbuh_str[nom]='_';

/*записываем со смещением в право*/
memset(rukov_str,'\0',sizeof(rukov_str));
sprintf(rukov_str,"%*.*s",
iceb_tu_kolbait(27,rukov.famil_inic.ravno()),
iceb_tu_kolbait(27,rukov.famil_inic.ravno()),
rukov.famil_inic.ravno());

for(int nom=0; nom < (int)strlen(rukov_str); nom++)
 if(rukov_str[nom] == ' ')
  rukov_str[nom]='_';

/*Чтобы узнать фамилию кассира нужно узнать номер кассы*/
memset(kassir_str,'\0',sizeof(kassir_str));
sprintf(strsql,"select kassa from Kasord where nomd='%s' and datd='%04d-%02d-%02d' and tp=2",
nomd,gd,md,dd);
if(readkey(strsql,&row,&cur) == 1)
 {
  sprintf(strsql,"select fio from Kas where kod=%s",row[0]);
  if(readkey(strsql,&row,&cur) == 1)
   sprintf(kassir_str,"%*.*s",iceb_tu_kolbait(38,row[0]),iceb_tu_kolbait(38,row[0]),row[0]); /*чтобы получить со смещением вправо*/
 }
for(int nom=0; nom < (int)strlen(kassir_str); nom++)
 if(kassir_str[nom] == ' ')
  kassir_str[nom]='_';

/*Читаем реквизиты организации*/

sprintf(strsql,"select naikon,kod from Kontragent where kodkon='00'");

if(readkey(strsql,&row,&cur) == 1)
 {
  naim.new_plus(row[0]);
  kodedrp.new_plus(row[1]);
 }

if(imaf[0] != '\0')
 {
  if((ff = fopen(imaf,"w")) == NULL)
   {
    error_op_nfil(imaf,errno,"");
    return;
   }
 }
 
fprintf(ff,"\x1B\x33%c\n",30); /*Уменьшаем межстрочный интервал*/

sprintf(strsql,"select str from Alx where fil='kasord2.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s-%s!",__FUNCTION__,gettext("Не найден бланк кассового ордера"));
  iceb_t_soob(strsql);
  return;
 }


short metka_sum_prop=0;
if(iceb_t_poldan("Печатать сумму прописью в расходном документе",&strf,"kasnast.alx") == 0)
 if(SRAV("Вкл",strf.ravno(),1) == 0)
  metka_sum_prop=1;

mesc(md,1,&naim_mes);

kols=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  strf.new_plus(row_alx[0]);
  kols++;
  switch (kols)
   {
    case 6:
      iceb_tu_vstav(&strf,naim.ravno(),0,53,1);
      iceb_tu_vstav(&strf,kodedrp.ravno(),55,73,1);
      break;     

    case 8:
      if(nomd[0] != '\0')
       {
        sprintf(strsql,"%02d",dd);
        iceb_tu_vstav(&strf,strsql,26,27,1);

        sprintf(strsql,"%s %d р.",naim_mes.ravno(),gd);
        iceb_tu_vstav(&strf,strsql,30,79,1);
       }
      break;     

    case 16:
      iceb_tu_vstav(&strf,nomd,1,10,1);
      if(nomd[0] != '\0')
       {
        sprintf(strsql,"%02d.%02d.%d",dd,md,gd);
        iceb_tu_vstav(&strf,strsql,11,20,1);
       }
      iceb_tu_vstav(&strf,shet,24,37,1);
      iceb_tu_vstav(&strf,shetkas,39,51,1);
      sprintf(strsql,"%.2f",suma);
      iceb_tu_vstav(&strf,strsql,52,64,1);
      if(atoi(hcn) != 0)
        iceb_tu_vstav(&strf,hcn,65,75,1);
      break;     

    case 18:
      iceb_tu_vstav(&strf,fio,7,79,1);
      break;     

    case 19:
      iceb_tu_vstav(&strf,osnov,9,79,1);
      break;     

    case 20:
      if(iceb_tu_strlen(osnov) > 71)
       iceb_tu_vstav(&strf,iceb_tu_adrsimv(71,osnov),0,79,1);
      break;     

    case 21:
       iceb_tu_vstav(&strf,sumac.ravno(),5,79,1);
       break;     

    case 22:
       iceb_tu_vstav(&strf,dopol,9,79,1);
       break;     

    case 23:
       if(iceb_tu_strlen(dopol) > 71)
        iceb_tu_vstav(&strf,iceb_tu_adrsimv(71,dopol),0,79,1);
       break;     

    case 25:
       iceb_tu_vstav(&strf,rukov_str,9,35,1);
       iceb_tu_vstav(&strf,glbuh_str,55,78,1);
       break;     

    case 27:
       if(metka_sum_prop == 0)
        break;
       iceb_tu_vstav(&strf,sumac.ravno(),9,79,1);
       break;
       
    case 29:
       iceb_tu_vstav(&strf,gd,26,29,1);

       break;     

    case 31:
       iceb_tu_vstav(&strf,dokum,3,79,1);
       break;     

    case 32:
       if(iceb_tu_strlen(dokum) > 77)
         iceb_tu_vstav(&strf,iceb_tu_adrsimv(77,dokum),0,79,1);
       break;     

    case 34:
       iceb_tu_vstav(&strf,kassir_str,12,49,1);
       break;     
   }
  
  fprintf(ff,"%s",strf.ravno());
  
 }


if(imaf[0] != '\0')
 {
  fclose(ff);
  iceb_t_ustpeh(imaf,1);
 }

}

/***********************/
/************************/

void	kasord2_0(const char *imaf, //Имя файла
const char *nomd, //Номер документа
short dd,short md,short gd, //Дата
const char *shet, //Счет кореспондент
const char *shetkas, //Счет кассы ,Шифр аналитического учета
double suma, //Сумма
const char *fio, //Фамилия имя отчество
const char *osnov, //Основание
const char *dopol, //Дополнение
const char *dokum, //Реквизита документа по которому выданы деньги
const char *hcn, //Шифр целевого назначения
FILE *ff_f)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
SQL_str		row;
SQLCURSOR       cur;
char		naim[512];
class iceb_tu_str strf("");
short		kols;
char		sumac[200];
FILE *ff=ff_f;

memset(naim,'\0',sizeof(naim));
memset(sumac,'\0',sizeof(sumac));
preobr(suma,sumac,0);

/*Читаем реквизиты организации*/

sprintf(strsql,"select naikon from Kontragent where kodkon='00'");

if(readkey(strsql,&row,&cur) == 1)
 {
  strncpy(naim,row[0],sizeof(naim)-1);
 }

if(imaf[0] != '\0')
 {
  if((ff = fopen(imaf,"w")) == NULL)
   {
    error_op_nfil(imaf,errno,"");
    return;
   }
  
 }

sprintf(strsql,"select str from Alx where fil='kasorder2.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  iceb_t_soob("Не найдены настройки kasorder2.alx");
  return;
 }


short metka_sum_prop=0;
if(iceb_t_poldan("Печатать сумму прописью в расходном документе",&strf,"kasnast.alx") == 0)
 if(SRAV("Вкл",strf.ravno(),1) == 0)
  metka_sum_prop=1;

kols=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  strf.new_plus(row_alx[0]);
  kols++;
  switch (kols)
   {
    case 1:
      iceb_tu_vstav(&strf,naim,0,49,1);
      break;     

    case 3:
      
      iceb_tu_vstav(&strf,nomd,43,59,1);
      break;     

    case 7:
      if(nomd[0] != '\0')
       {
        sprintf(strsql,"%02d",dd);      
        iceb_tu_vstav(&strf,strsql,19,20,1);

        sprintf(strsql,"%02d",md);      
        iceb_tu_vstav(&strf,strsql,25,27,1);
       }
      if(gd != 0)
       {
        sprintf(strsql,"%02d%s",gd,gettext("г."));      
        iceb_tu_vstav(&strf,strsql,32,41,1);
       }
      break;     

    case 13:
      iceb_tu_vstav(&strf,shet,9,18,1);
      iceb_tu_vstav(&strf,shetkas,22,37,1);
      sprintf(strsql,"%15.2f",suma);
      iceb_tu_vstav(&strf,strsql,39,54,1);
      iceb_tu_vstav(&strf,hcn,56,68,1);
      break;     

    case 15:
      iceb_tu_vstav(&strf,fio,7,68,1);
      break;


    case 16:
      iceb_tu_vstav(&strf,osnov,9,69,1);
      break;     

    case 17:
      if(iceb_tu_strlen(osnov) > 61)
         iceb_tu_vstav(&strf,iceb_tu_adrsimv(61,osnov),0,69,1);
      break;     

    case 18:
      iceb_tu_vstav(&strf,sumac,0,68,1);
      break;     

    case 19:
      iceb_tu_vstav(&strf,dopol,9,68,1);
      break;     

    case 23:
      if(metka_sum_prop == 1)
       iceb_tu_vstav(&strf,sumac,8,68,1);
      break;     

    case 25:
      sprintf(strsql,"%02d%s",gd,gettext("г."));      
      iceb_tu_vstav(&strf,strsql,21,31,1);
      break;     

    case 27:
      iceb_tu_vstav(&strf,dokum,4,68,1);
      if(iceb_tu_strlen(dokum) > 69-4)
       {
        fprintf(ff,"%s",strf.ravno());
        strf.new_plus(iceb_tu_adrsimv(69-4,dokum));
        strf.plus("\n");
       }
      break;     
      
   }
  
  fprintf(ff,"%s",strf.ravno());
  
 }


if(imaf[0] != '\0')
 {
  fclose(ff);
  iceb_t_ustpeh(imaf,1);
 }

}
